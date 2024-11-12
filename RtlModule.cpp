//
// Created by hanyuan on 2024/11/2.
//

#include "RtlModule.h"
#include "CombLogics.h"
#include <stdexcept>

std::shared_ptr<ModuleIOPort> RtlModule::getModuleIOPortByName(const std::string &name) {
    for (auto port: ioPorts) {
        if (port->getIdentifier() == name) {
            return port;
        }
    }

    throw std::runtime_error("No such an I/O port");
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

    throw std::runtime_error("No such an identifier when lookup the circuit");
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
            throw std::runtime_error("AST is not an operator");
        }
        auto combLogic = CombLogicFactory::create(ast->_operator);
        if(ast->nodeType.starts_with("multiplexer")) {
            auto muxAST = dynamic_cast<HDLMuxAST *>(ast);
            if(muxAST->identifier.starts_with("__hw_mux_fake__")) {
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
                throw std::runtime_error("Latch is inferred");
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
                            throw std::runtime_error("Latch inferred");
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

    throw std::runtime_error("Not supported always block body");
    return {};
}

void RtlModule::addConditionForAlwaysBlockBody(std::vector<CircuitConnection> &blockBody,
                                               std::unique_ptr<HDLExpressionAST> &condition) {
    for (auto &conn: blockBody) {
        std::unique_ptr<HDLMuxAST> merge_ast;
        if (conn.conditionAST != nullptr) {
            merge_ast = std::make_unique<HDLMuxAST>(std::move(conn.conditionAST));
        } else {
            merge_ast = std::make_unique<HDLMuxAST>();
        auto merge_ast = std::make_unique<HDLMuxAST>(true);
        }
        conn.conditionAST = std::move(condition);
        merge_ast->children.push_back(std::move(conn.ast));
        conn.ast = std::move(merge_ast);
    }
}

void RtlModule::buildCircuit() {
    for (auto &blk: alwaysBlocks) {
        const auto &sensitiveList = blk->getSensitiveList();
        for (const auto &child: blk->children) {
            auto body = genByAlwaysBlockBody(dynamic_cast<AlwaysBlockBodyAST *>(child.get()));
            for (auto &conn: body) {
                circuitConnections.push_back(std::move(conn));
            }
        }
    }

    for (auto &conn: circuitConnections) {
        auto *ast = conn.getHDLExpressionAST();
        auto symbol = genCircuitSymbolByHDLExprAST(ast);
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

            if (destSlicing.isTrivial()) {
                destSymbol->registerInput(symbol, inputSlicingInTotal);
            } else {
                auto destWireSymbol = std::static_pointer_cast<CircuitSymbolWire>(destSymbol);
                if (destWireSymbol) {
                    destWireSymbol->registerInput(symbol, destSlicing, inputSlicingInTotal);
                } else {
                    throw std::runtime_error("Cannot set slicing");
                }
            }
        }
    }
}
