//
// Created by hanyuan on 2024/11/20.
//

#include "EmitFIRRTL.h"
#include "Parser.h"
#include <circt/Firtool/Firtool.h>
#include <circt/Dialect/FIRRTL/FIRRTLAnnotations.h>
#include <circt/Dialect/FIRRTL/FIRRTLOps.h>
#include <circt/Dialect/FIRRTL/FIRRTLTypes.h>
#include <circt/Dialect/FIRRTL/FIRRTLUtils.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/Pass/PassManager.h>
#include <iostream>

circt::Value
EmitFIRRTL::emitFromSymbol(const std::shared_ptr<CircuitSymbol> &symbol, const PortSlicingAST &inputSlicing) {
    static int tmpCounter = 0;
    std::cout << symbol->getIdentifier() << std::endl;
    auto &backward = symbol->getBackwardSymbols();
    auto symbolIdentifier = symbol->getIdentifier();
    bool isOutput = isOutputPort(symbolIdentifier);
    circt::Value returnValue{};

    if (!isOutput && symbolTable.count(symbolIdentifier)) {
        returnValue = symbolTable[symbolIdentifier];
    } else if (symbol->getIdentifier().starts_with("__comb_" + Parser::operatorName[TOKEN_question])) {
        auto cond = emitFromSymbol(std::get<0>(backward[0]), std::get<1>(backward[0]));
        auto branch1 = emitFromSymbol(std::get<0>(backward[1]), std::get<1>(backward[1]));
        auto branch2 = emitFromSymbol(std::get<0>(backward[2]), std::get<1>(backward[2]));
        auto type = circt::firrtl::MuxPrimOp::inferReturnType(
                llvm::cast<circt::firrtl::FIRRTLType>(cond.getType()),
                llvm::cast<circt::firrtl::FIRRTLType>(branch1.getType()),
                llvm::cast<circt::firrtl::FIRRTLType>(branch2.getType()), {});
        auto op = implicitLocOpBuilder.create<circt::firrtl::MuxPrimOp>(type, cond, branch1, branch2);

        returnValue = op.getResult();
    } else if (symbol->getIdentifier().starts_with("__hwconst_")) {
        circt::OpBuilder::InsertPoint savedIP;
        auto width = symbol->getSlicing().getWidth();
        auto constantSymbol = std::static_pointer_cast<CircuitSymbolConstant>(symbol);
        mlir::APInt value{(unsigned) width, (uint64_t) constantSymbol->getValue()};

//        auto *parentOp = implicitLocOpBuilder.getInsertionPoint()->getParentOp();
//        parentOp->dump();
//        if (!llvm::isa<circt::firrtl::FModuleLike>(parentOp)) {
//            savedIP = implicitLocOpBuilder.saveInsertionPoint();
//            while (!llvm::isa<circt::firrtl::FModuleLike>(parentOp)) {
//                implicitLocOpBuilder.setInsertionPoint(parentOp);
//                parentOp = implicitLocOpBuilder.getInsertionPoint()->getParentOp();
//            }
//        }

        auto type = circt::firrtl::IntType::get(implicitLocOpBuilder.getContext(), false, width, true);
        auto attrType = mlir::IntegerType::get(implicitLocOpBuilder.getContext(), value.getBitWidth(),
                                               mlir::IntegerType::Unsigned);
        auto attr = implicitLocOpBuilder.getIntegerAttr(attrType, value);

        returnValue = implicitLocOpBuilder.create<circt::firrtl::ConstantOp>(type, attr);

//        if (savedIP.isSet()) {
//            implicitLocOpBuilder.setInsertionPoint(savedIP.getBlock(), savedIP.getPoint());
//        }
    } else {
        circt::firrtl::NodeOp node;
        circt::Value next = emitFromSymbol(std::get<0>(backward[0]), std::get<1>(backward[0]));

        if (isOutput) {
            returnValue = next;
        } else {
            node = implicitLocOpBuilder.create<circt::firrtl::NodeOp>(
                    next, circt::StringRef{symbol->getIdentifier()},
                    circt::firrtl::NameKindEnum::InterestingName,
                    emptyArrayAttr,
                    circt::StringAttr{});

            returnValue = node.getResult();
        }
    }

    if (!isOutput && symbolTable.count(symbolIdentifier) == 0) {
        symbolTable[symbolIdentifier] = returnValue;
    }

    if (!inputSlicing.isTrivial()) {
        /* 先临时创建 bits */
        auto tmpId = "_" + symbolIdentifier + "__" + std::to_string(tmpCounter++);
        auto [hi, lo] = inputSlicing.toDownTo();
        auto type = circt::firrtl::BitsPrimOp::inferReturnType(
                llvm::cast<circt::firrtl::FIRRTLType>(returnValue.getType()),
                hi,
                lo, {});
        auto bitsRhs = implicitLocOpBuilder.create<circt::firrtl::BitsPrimOp>(type, returnValue, hi, lo);
        auto nextNode = implicitLocOpBuilder.create<circt::firrtl::NodeOp>(
                bitsRhs, circt::StringRef{tmpId},
                circt::firrtl::NameKindEnum::InterestingName,
                emptyArrayAttr, circt::StringAttr{});
        returnValue = nextNode.getResult();
    }

    return returnValue;
}

void EmitFIRRTL::emit() {
    /* 由 output 开始向后推 */
    circt::ArrayAttr layers;
    circt::SmallVector<circt::firrtl::PortInfo, 8> ports;
    context.loadDialect<circt::firrtl::FIRRTLDialect>();

    auto mlirModule = mlir::ModuleOp::create(mlir::UnknownLoc::get(&context));
    circt::OpBuilder b{mlirModule.getBodyRegion()};
    auto circuit = b.create<circt::firrtl::CircuitOp>(
            mlir::UnknownLoc::get(&context),
            circt::StringAttr::get(
                    &context,
                    circt::StringRef{rtlModule.moduleName}));

    for (const auto &ioPort: rtlModule.ioPorts) {
        auto name = circt::StringAttr::get(&context, circt::StringRef{ioPort->getIdentifier()});
        auto type = circt::firrtl::UIntType::get(&context, ioPort->getSlicing().getWidth());
        auto direction = circt::firrtl::direction::get(ioPort->getPortDirection() == PortDirection::Output);
        ports.push_back({name, type, direction, {}});
    }

    auto builder = circuit.getBodyBuilder();
    auto moduleOp = builder.create<circt::firrtl::FModuleOp>(
            mlir::UnknownLoc::get(&context),
            circt::StringAttr::get(
                    &context, circt::StringRef{rtlModule.moduleName}),
            circt::firrtl::ConventionAttr::get(
                    &context,
                    circt::firrtl::Convention::Internal),
            ports, emptyArrayAttr, layers
    );
    auto &body = moduleOp->getRegion(0).front();
    auto portArgs = body.getArguments();
    for (auto tuple: llvm::zip(ports, portArgs)) {
        circt::firrtl::PortInfo &port = std::get<0>(tuple);
        circt::BlockArgument portArg = std::get<1>(tuple);
        symbolTable[port.getName().str()] = portArg;
    }

    implicitLocOpBuilder.setInsertionPointToEnd(&body);

    for (const auto &ioPort: rtlModule.ioPorts) {
        if (ioPort->getPortDirection() == PortDirection::Output) {
            auto rhs = emitFromSymbol(ioPort);
            /* 进行连接 */
            circt::firrtl::emitConnect(implicitLocOpBuilder, symbolTable[ioPort->getIdentifier()], rhs);
        }
    }

    mlir::PassManager pm(&context);
    circt::firtool::FirtoolOptions opt{};
    if (mlir::failed(circt::firtool::populatePreprocessTransforms(pm, opt))) {
        throw std::runtime_error("Unable to populate preprocess passes for MLIR");
    }

    if (mlir::failed(pm.run(mlirModule))) {
        throw std::runtime_error("Unable to run preprocess passes");
    }

    mlirModule.print(llvm::outs());
}

bool EmitFIRRTL::isOutputPort(std::string &identifier) const {
    for (const auto &port: rtlModule.ioPorts) {
        if (port->getPortDirection() == PortDirection::Output && port->getIdentifier() == identifier) {
            return true;
        }
    }
    return false;
}
