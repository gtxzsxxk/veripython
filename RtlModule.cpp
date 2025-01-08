//
// Created by unknown on 2024/11/2.
//

#include "RtlModule.h"
#include "CombLogics.h"
#include <sstream>

std::shared_ptr<ModuleIOPort> RtlModule::getModuleIOPortByName(const std::string &name) {
    for (auto port: ioPorts) {
        if (port->getIdentifier() == name) {
            return port;
        }
    }

    throw CircuitException("No such an I/O port");
}

void RtlModule::addCircuitConnection(CircuitConnection &&connection) {
    circuitConnections.push_back(std::move(connection));
}

void RtlModule::addAlwaysBlock(std::unique_ptr<AlwaysBlockAST> &&alwaysBlock) {
    alwaysBlocks.push_back(std::move(alwaysBlock));
}

std::shared_ptr<CircuitSymbol> RtlModule::getPortOrSymbolById(const std::string &id) {
    for (auto &ioPort: ioPorts) {
        if (ioPort->getIdentifier() == id) {
            return ioPort;
        }
    }

    for (auto symbol: circuitSymbols) {
        if (symbol->getIdentifier() == id) {
            return symbol;
        }
    }

    throw CircuitException("No such an identifier when lookup the circuit");
    return nullptr;
}

std::shared_ptr<CircuitSymbol> RtlModule::genCircuitSymbolByHDLExprAST(HDLExpressionAST *ast) {
    if (ast->nodeType == "const_number") {
        auto constSym = std::make_shared<CircuitSymbolConstant>(dynamic_cast<HDLPrimaryAST *>(ast));
        circuitSymbols.push_back(constSym);
        return constSym;
    } else if (ast->nodeType == "identifier") {
        return getPortOrSymbolById(dynamic_cast<HDLPrimaryAST *>(ast)->getIdentifier());
    } else {
        if (!HDLExpressionAST::canParseToCombLogics(ast->_operator)) {
            throw CircuitException("AST is not an operator");
        }
        auto combLogic = CombLogicFactory::create(ast->_operator);
        if (ast->nodeType.starts_with("multiplexer")) {
            auto muxAST = dynamic_cast<HDLMuxAST *>(ast);
            if (muxAST->identifier.starts_with("__hw_mux_fake__")) {
                auto hdlExprAST = dynamic_cast<HDLExpressionAST *>(ast->condition.get());
                auto circuitSymbol = genCircuitSymbolByHDLExprAST(hdlExprAST);
                if (hdlExprAST->exprSlicing.isTrivial()) {
                    combLogic->registerInput(circuitSymbol);
                } else {
                    combLogic->registerInput(circuitSymbol, hdlExprAST->exprSlicing);
                }
            }
        }
        for (const auto &child: ast->children) {
            auto hdlExprAST = dynamic_cast<HDLExpressionAST *>(child.get());
            auto circuitSymbol = genCircuitSymbolByHDLExprAST(hdlExprAST);
            if (hdlExprAST->exprSlicing.isTrivial()) {
                combLogic->registerInput(circuitSymbol);
            } else {
                combLogic->registerInput(circuitSymbol, hdlExprAST->exprSlicing);
            }
        }
        std::shared_ptr<CombLogic> sharedCombLogic = std::move(combLogic);
        circuitSymbols.push_back(sharedCombLogic);
        return sharedCombLogic;
    }
}

std::vector<CircuitConnection> RtlModule::genByAlwaysBlockBody(AlwaysBlockBodyAST *ast) {
    if (ast->nodeType.starts_with("__hw_always_block_body_if_block__")) {
        auto condition = ast->getCondition();
        auto branch1 = genByAlwaysBlockBody(dynamic_cast<AlwaysBlockBodyAST *>(ast->children[0].get()));
        addConditionForAlwaysBlockBody(branch1, condition);
        if (ast->children.size() == 2) {
            auto branch2 = genByAlwaysBlockBody(dynamic_cast<AlwaysBlockBodyAST *>(ast->children[1].get()));
            if (branch1.size() != branch2.size()) {
                throw CircuitException("Latch is inferred");
            }
            for (auto &conn: branch2) {
                for (auto &connBranch1: branch1) {
                    if (connBranch1.getDestIdentifiers() == conn.getDestIdentifiers()) {

                        connBranch1.ast->children.push_back(std::move(conn.ast));
                        break;
                    }
                }
            }
        } else {
            for (auto &connBranch1: branch1) {
                connBranch1.ast->children.push_back(nullptr);
            }
        }

        return branch1;
    } else if (ast->nodeType.starts_with("__hw_non_blk_assign__")) {
        std::vector<CircuitConnection> ret;
        ret.push_back(std::move(dynamic_cast<NonBlockingAssignAST *>(ast)->connection));
        return ret;
    } else if (ast->nodeType.starts_with("__hw_always_block_body__")) {
        std::vector<CircuitConnection> connections;
        for (auto &child: ast->children) {
            auto lastLevelConnections = genByAlwaysBlockBody(dynamic_cast<AlwaysBlockBodyAST *>(child.get()));
            if (child->nodeType.starts_with("__hw_always_block_body_if_block__")) {
                for (auto &lastLevelConn: lastLevelConnections) {
                    if (lastLevelConn.ast->children[1] == nullptr) {
                        int index = -1, _idx = 0;
                        for (auto &alreadyHadConn: connections) {
                            if (alreadyHadConn.getDestIdentifiers() == lastLevelConn.getDestIdentifiers()) {
                                index = _idx;
                                break;
                            }
                            _idx++;
                        }
                        if (index == -1) {
                            throw CircuitException("Latch inferred");
                        }
                        CircuitConnection alreadyHadConn = std::move(connections[index]);
                        connections.erase(connections.begin() + index);
                        lastLevelConn.ast->children[1] = std::move(alreadyHadConn.ast);
                    }
                }
                for (auto &lastLevelConn: lastLevelConnections) {
                    connections.push_back(std::move(lastLevelConn));
                }
            } else if (child->nodeType.starts_with("__hw_non_blk_assign__")) {
                connections.push_back(std::move(lastLevelConnections[0]));
            }
        }

        return connections;
    }

    throw CircuitException("Not supported always block body");
    return {};
}

void RtlModule::addConditionForAlwaysBlockBody(std::vector<CircuitConnection> &blockBody,
                                               std::shared_ptr<HDLExpressionAST> &condition) {
    for (auto &conn: blockBody) {
        auto merge_ast = std::make_unique<HDLMuxAST>(true);
        if (conn.ast->condition != nullptr) {
            merge_ast->condition = conn.ast->condition;
        }
        merge_ast->children.push_back(std::move(conn.ast));
        conn.ast = std::move(merge_ast);
        conn.ast->condition = condition;
    }
}

void RtlModule::genAstJson() {
    std::stringstream xml;
    xml << "{\n  \"nodeType\": \"RtlModule\"," << std::endl;
    xml << "  \"name\": \"" << moduleName << "\"," << std::endl;
    xml << "  \"ioPorts\": [" << std::endl;
    std::size_t counter = 0;
    for (const auto &ioPort: ioPorts) {
        xml << "    {\n      \"nodeType\": \"Port\"," << std::endl;
        xml << "      \"name\": \"" << ioPort->getIdentifier() << "\"," << std::endl;
        xml << "      \"direction\": \"" <<
            ((ioPort->getPortDirection() == PortDirection::Input) ? "input" : ((ioPort->getPortDirection() ==
                                                                                PortDirection::Output) ? "output"
                                                                                                       : "not specified"))
            << "\"," << std::endl;
        xml << "      \"slicing\":\n";
        auto slicingString = strdup(const_cast<PortSlicingAST &>(ioPort->getSlicing()).toString().c_str());
        for (char *line = strtok(slicingString, "\n"); line != nullptr; line = strtok(nullptr, "\n")) {
            xml << "      " << line << std::endl;
        }
        free(slicingString);
        if (counter != ioPorts.size() - 1) {
            xml << "    }," << std::endl;
        } else {
            xml << "    }" << std::endl;
        }
        counter++;
    }
    xml << "  ]," << std::endl;

    xml << "  \"combinatorial\": [" << std::endl;
    counter = 0;
    for (auto &conn: circuitConnections) {
        xml << "    {\n      \"nodeType\": \"Connection\"," << std::endl;
        xml << "      \"Destination\": [" << std::endl;
        std::size_t counterTmp = 0;
        auto &destData = conn.getDestIdentifiers();
        for (const auto &[name, slicing]: destData) {
            xml << "        {\n          \"nodeType\": \"CircuitSymbol\"," << std::endl;
            xml << "          \"name\": \"" << name << "\"," << std::endl;
            xml << "          \"slicing\":\n";
            auto slicingString = strdup(const_cast<PortSlicingAST &>(slicing).toString().c_str());
            for (char *line = strtok(slicingString, "\n"); line != nullptr; line = strtok(nullptr, "\n")) {
                xml << "          " << line << std::endl;
            }
            free(slicingString);
            if (counterTmp != destData.size() - 1) {
                xml << "        }," << std::endl;
            } else {
                xml << "        }" << std::endl;
            }
            counterTmp++;
        }
        xml << "      ]," << std::endl;

        xml << "      \"expression\": " << std::endl;
        auto exprString = strdup(conn.ast->toString().c_str());
        for (char *line = strtok(exprString, "\n"); line != nullptr; line = strtok(nullptr, "\n")) {
            xml << "          " << line << std::endl;
        }
        free(exprString);
        if (counter != circuitConnections.size() - 1) {
            xml << "    }," << std::endl;
        } else {
            xml << "    }" << std::endl;
        }
        counter++;
    }
    xml << "  ]," << std::endl;

    xml << "  \"alwaysBlocks\": [" << std::endl;
    counter = 0;
    for (auto &blk: alwaysBlocks) {
        xml << "    {\n      \"nodeType\": \"alwaysBlock\"," << std::endl;
        xml << "      \"SensitiveList\": [" << std::endl;
        std::size_t counterTmp = 0;
        auto &sensList = blk->getSensitiveList();
        for (const auto &[triggerType, identifier]: sensList) {
            auto triggerString = (triggerType == TriggerEdgeType::POSITIVE_EDGE ? "PositiveEdge" : (triggerType ==
                                                                                                    TriggerEdgeType::NEGATIVE_EDGE
                                                                                                    ? "NegativeEdge"
                                                                                                    : "LevelTriggered"));
            if (counterTmp != sensList.size() - 1) {
                xml << "        [\"" << triggerString << "\", \"" + identifier + "\"]," << std::endl;
            } else {
                xml << "        [\"" << triggerString << "\", \"" + identifier + "\"]" << std::endl;
            }
            counterTmp++;
        }
        xml << "      ]," << std::endl;
        xml << "      \"alwaysBlockAST\": " << std::endl;
        auto exprString = strdup(blk->toString().c_str());
        for (char *line = strtok(exprString, "\n"); line != nullptr; line = strtok(nullptr, "\n")) {
            xml << "        " << line << std::endl;
        }
        free(exprString);
        if (counter != alwaysBlocks.size() - 1) {
            xml << "    }," << std::endl;
        } else {
            xml << "    }" << std::endl;
        }
        counter++;
    }
    xml << "  ]" << std::endl;

    xml << "}";
    xmlAstData = xml.str();
}

void RtlModule::buildCircuit() {
    genAstJson();

    for (auto &blk: alwaysBlocks) {
        const auto &sensitiveList = blk->getSensitiveList();
        if (sensitiveList.size() != 1) {
            throw CircuitException("Only support one element in sensitive list");
        }
        for (const auto &child: blk->children) {
            auto body = genByAlwaysBlockBody(dynamic_cast<AlwaysBlockBodyAST *>(child.get()));
            for (auto &conn: body) {
                conn.clocking = sensitiveList[0];
                circuitConnections.push_back(std::move(conn));
            }
        }
    }

    for (auto &conn: circuitConnections) {
        auto *ast = conn.getHDLExpressionAST();
        auto symbol = genCircuitSymbolByHDLExprAST(ast);
        std::shared_ptr<CircuitSymbol> clockSignal = nullptr;
        auto triggerType = conn.clocking.first;
        if (!conn.clocking.second.empty() && conn.clocking.second != "*") {
            /* is register, add clock signal */
            clockSignal = getPortOrSymbolById(conn.clocking.second);
        }

        int inputSlicingNextStart = 0;
        for (auto &[destIdentifier, destSlicing]: conn.getDestIdentifiers()) {
            auto destSymbol = getPortOrSymbolById(destIdentifier);
            auto destWidth = destSlicing.isDownTo ? destSlicing.downToHigh - destSlicing.downToLow + 1 : 1;
            if (destSlicing.isTrivial()) {
                destWidth = destSymbol->getSlicing().downToHigh - destSymbol->getSlicing().downToLow + 1;
            }
            PortSlicingAST inputSlicingInTotal = {destWidth - 1, 0};
            inputSlicingInTotal.downToHigh += inputSlicingNextStart;
            inputSlicingInTotal.downToLow += inputSlicingNextStart;
            inputSlicingNextStart += destWidth;

            if (clockSignal != nullptr) {
                auto destRegSymbol = std::dynamic_pointer_cast<CircuitSymbolReg>(destSymbol);
                if (destRegSymbol) {
                    destRegSymbol->setTriggerType(triggerType);
                    destRegSymbol->registerClock(clockSignal);
                } else {
                    throw CircuitException("Failed to convert a circuit symbol into a register symbol. "
                                             "Cannot set clock signal.");
                }
            }

            if (destSlicing.isTrivial()) {
                destSymbol->registerInput(symbol, inputSlicingInTotal);
            } else {
                auto destWireSymbol = std::dynamic_pointer_cast<CircuitSymbolWire>(destSymbol);
                if (destWireSymbol) {
                    destWireSymbol->registerInput(symbol, destSlicing, inputSlicingInTotal);
                } else {
                    throw CircuitException("Failed to convert a circuit symbol into a wire symbol. "
                                             "Cannot set bits slicing.");
                }
            }
        }
    }
}

std::string RtlModule::toString() const {
    return xmlAstData;
}
