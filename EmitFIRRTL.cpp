//
// Created by hanyuan on 2024/11/20.
//

#include "EmitFIRRTL.h"
#include "Parser.h"
#include "CombLogics.h"
#include <circt/Firtool/Firtool.h>
#include <circt/Dialect/FIRRTL/FIRRTLAnnotations.h>
#include <circt/Dialect/FIRRTL/FIRRTLOps.h>
#include <circt/Dialect/FIRRTL/FIRRTLTypes.h>
#include <circt/Dialect/FIRRTL/FIRRTLUtils.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/Pass/PassManager.h>
#include <llvm/Support/raw_ostream.h>
#include <iostream>

template<class MlirOp>
class CombNormalBinaryBuildOp {
public:
    circt::Value operator()(EmitFIRRTL *emit, const std::shared_ptr<CombLogic> &comb) {
        auto &backward = comb->getBackwardSymbols();
        auto operand1 = emit->emitFromSymbol(std::get<0>(backward[0]), std::get<1>(backward[0]));
        auto operand2 = emit->emitFromSymbol(std::get<0>(backward[1]), std::get<1>(backward[1]));
        auto type = MlirOp::inferReturnType(
                llvm::cast<circt::firrtl::FIRRTLType>(operand1.getType()),
                llvm::cast<circt::firrtl::FIRRTLType>(operand2.getType()), {});
        auto op = emit->implicitLocOpBuilder.create<MlirOp>(type, operand1, operand2);
        return op.getResult();
    }
};

circt::Value EmitFIRRTL::concatFromRange(const std::vector<circt::Value> &values, int start, int end) {
    if (start == end) {
        return values[start];
    }

    auto leftValue = concatFromRange(values, start, start + (end - start) / 2);
    auto rightValue = concatFromRange(values, start + (end - start) / 2 + 1, end);
    auto type = circt::firrtl::CatPrimOp::inferReturnType(
            llvm::cast<circt::firrtl::FIRRTLType>(leftValue.getType()),
            llvm::cast<circt::firrtl::FIRRTLType>(rightValue.getType()), {});
    auto op = implicitLocOpBuilder.create<circt::firrtl::CatPrimOp>(type, leftValue, rightValue);
    return op.getResult();
}

circt::Value EmitFIRRTL::combToCirctOpValue(const std::shared_ptr<CircuitSymbol> &symbol) {
    auto &backward = symbol->getBackwardSymbols();
    const auto combSymbol = std::dynamic_pointer_cast<CombLogic>(symbol);
    if (!combSymbol) {
        throw std::runtime_error("Cannot convert a symbol into combinatorial symbol");
    }
    auto derivedType = combSymbol->getDerivedType();

    if (derivedType == "mux") {
        auto cond = emitFromSymbol(std::get<0>(backward[0]), std::get<1>(backward[0]));
        auto branch1 = emitFromSymbol(std::get<0>(backward[1]), std::get<1>(backward[1]));
        auto branch2 = emitFromSymbol(std::get<0>(backward[2]), std::get<1>(backward[2]));
        auto type = circt::firrtl::MuxPrimOp::inferReturnType(
                llvm::cast<circt::firrtl::FIRRTLType>(cond.getType()),
                llvm::cast<circt::firrtl::FIRRTLType>(branch1.getType()),
                llvm::cast<circt::firrtl::FIRRTLType>(branch2.getType()), {});
        auto op = implicitLocOpBuilder.create<circt::firrtl::MuxPrimOp>(type, cond, branch1, branch2);
        return op.getResult();
    } else if (derivedType == "normalBinary" || derivedType == "compareBinary" || derivedType == "arithBinary") {
        switch (combSymbol->getOperator()) {
            case TOKEN_logical_or:
                [[fallthrough]];
            case TOKEN_bitwise_or:
                return CombNormalBinaryBuildOp<circt::firrtl::OrPrimOp>()(this, combSymbol);
            case TOKEN_logical_and:
                [[fallthrough]];
            case TOKEN_bitwise_and:
                return CombNormalBinaryBuildOp<circt::firrtl::AndPrimOp>()(this, combSymbol);
            case TOKEN_bitwise_xor:
                return CombNormalBinaryBuildOp<circt::firrtl::XorPrimOp>()(this, combSymbol);
            case TOKEN_cond_eq:
                return CombNormalBinaryBuildOp<circt::firrtl::EQPrimOp>()(this, combSymbol);
            case TOKEN_cond_ne:
                return CombNormalBinaryBuildOp<circt::firrtl::NEQPrimOp>()(this, combSymbol);
            case TOKEN_cond_lt:
                return CombNormalBinaryBuildOp<circt::firrtl::LTPrimOp>()(this, combSymbol);
            case TOKEN_cond_le:
                return CombNormalBinaryBuildOp<circt::firrtl::LEQPrimOp>()(this, combSymbol);
            case TOKEN_cond_gt:
                return CombNormalBinaryBuildOp<circt::firrtl::GTPrimOp>()(this, combSymbol);
            case TOKEN_cond_ge:
                return CombNormalBinaryBuildOp<circt::firrtl::GEQPrimOp>()(this, combSymbol);
            case TOKEN_op_add:
                return CombNormalBinaryBuildOp<circt::firrtl::AddPrimOp>()(this, combSymbol);
            case TOKEN_op_sub:
                return CombNormalBinaryBuildOp<circt::firrtl::SubPrimOp>()(this, combSymbol);
            case TOKEN_op_mod:
                return CombNormalBinaryBuildOp<circt::firrtl::RemPrimOp>()(this, combSymbol);
            case TOKEN_op_mul:
                return CombNormalBinaryBuildOp<circt::firrtl::MulPrimOp>()(this, combSymbol);
            case TOKEN_op_div:
                return CombNormalBinaryBuildOp<circt::firrtl::DivPrimOp>()(this, combSymbol);
            default:
                break;
        }
    } else if (derivedType == "shiftLeft") {
        return CombNormalBinaryBuildOp<circt::firrtl::DShlwPrimOp>()(this, combSymbol);
    } else if (derivedType == "shiftRight") {
        if (combSymbol->getOperator() == TOKEN_arith_rshift) {
            auto operand1 = emitFromSymbol(std::get<0>(backward[0]), std::get<1>(backward[0]));
            auto operand2 = emitFromSymbol(std::get<0>(backward[1]), std::get<1>(backward[1]));
            auto opType = circt::firrtl::CvtPrimOp::inferReturnType(
                    llvm::cast<circt::firrtl::FIRRTLType>(operand1.getType()), {});
            auto signedOp = implicitLocOpBuilder.create<circt::firrtl::CvtPrimOp>(opType, operand1);
            auto signedOperand = signedOp.getResult();

            auto type = circt::firrtl::DShrPrimOp::inferReturnType(
                    llvm::cast<circt::firrtl::FIRRTLType>(signedOperand.getType()),
                    llvm::cast<circt::firrtl::FIRRTLType>(operand2.getType()), {});
            auto op = implicitLocOpBuilder.create<circt::firrtl::DShrPrimOp>(type, signedOperand, operand2);
            return op.getResult();
        } else {
            return CombNormalBinaryBuildOp<circt::firrtl::DShrPrimOp>()(this, combSymbol);
        }
    } else if (derivedType == "unary") {
        auto operand = emitFromSymbol(std::get<0>(backward[0]), std::get<1>(backward[0]));
        auto type = circt::firrtl::NotPrimOp::inferReturnType(
                llvm::cast<circt::firrtl::FIRRTLType>(operand.getType()), {});
        auto op = implicitLocOpBuilder.create<circt::firrtl::NotPrimOp>(type, operand);
        return op.getResult();
    } else if (derivedType == "concat") {
        std::vector<circt::Value> values;
        for (const auto &[backSymbol, inputSlicing, _2]: backward) {
            values.push_back(emitFromSymbol(backSymbol, inputSlicing));
        }
        return concatFromRange(values, 0, (int) (backward.size() - 1));
    }

    throw std::runtime_error("Unsupported combinatorial operator");
}

circt::Value
EmitFIRRTL::emitFromSymbol(const std::shared_ptr<CircuitSymbol> &symbol, const PortSlicingAST &inputSlicing) {
    static int tmpCounter = 0;
    auto &backward = symbol->getBackwardSymbols();
    auto symbolIdentifier = symbol->getIdentifier();
    bool isOutput = isOutputPort(symbolIdentifier);
    circt::Value returnValue{}, regOrWireOp;
    bool needConnect = false;

    if (!isOutput && symbolTable.count(symbolIdentifier)) {
        returnValue = symbolTable[symbolIdentifier];
    } else if (symbol->getIdentifier().starts_with("__comb_")) {
        returnValue = combToCirctOpValue(symbol);
    } else if (symbol->getIdentifier().starts_with("__hwconst_")) {
        circt::OpBuilder::InsertPoint savedIP;
        auto width = symbol->getSlicing().getWidth();
        auto constantSymbol = std::dynamic_pointer_cast<CircuitSymbolConstant>(symbol);
        if (!constantSymbol) {
            throw std::runtime_error("Cannot convert a symbol into constant symbol");
        }
        mlir::APInt value{(unsigned) width, (uint64_t) constantSymbol->getValue()};
        auto type = circt::firrtl::IntType::get(implicitLocOpBuilder.getContext(), false, width, true);
        auto attrType = mlir::IntegerType::get(implicitLocOpBuilder.getContext(), value.getBitWidth(),
                                               mlir::IntegerType::Unsigned);
        auto attr = implicitLocOpBuilder.getIntegerAttr(attrType, value);

        returnValue = implicitLocOpBuilder.create<circt::firrtl::ConstantOp>(type, attr).getResult();
    } else if (symbol->isWireSymbol()) {
        if (backward.size() != 1) {
            return {};
        }

        needConnect = true;
        auto width = symbol->getSlicing().getWidth();
        auto type = circt::firrtl::IntType::get(implicitLocOpBuilder.getContext(), false, width, false);
        regOrWireOp = implicitLocOpBuilder.create<circt::firrtl::WireOp>(type, circt::StringRef{symbolIdentifier},
                                                                         circt::firrtl::NameKindEnum::InterestingName,
                                                                         emptyArrayAttr,
                                                                         circt::StringAttr{}).getResult();
        if (symbolTable.count(symbolIdentifier) == 0) {
            symbolTable[symbolIdentifier] = regOrWireOp;
        }
        returnValue = emitFromSymbol(std::get<0>(backward[0]), std::get<1>(backward[0]));
    } else if (symbol->isRegisterSymbol()) {
        if (backward.size() != 2) {
            return {};
        }

        needConnect = true;
        circt::Value clock;
        auto clockSym = std::get<0>(backward[0])->getIdentifier();
        if (symbolTable.count(clockSym)) {
            clock = symbolTable[clockSym];
        } else {
            throw std::runtime_error("Cannot found clock signal " + clockSym);
        }

        auto width = symbol->getSlicing().getWidth();
        auto type = circt::firrtl::IntType::get(implicitLocOpBuilder.getContext(), false, width, false);
        regOrWireOp = implicitLocOpBuilder.create<circt::firrtl::RegOp>(type, clock, circt::StringRef{symbolIdentifier},
                                                                        circt::firrtl::NameKindEnum::InterestingName,
                                                                        emptyArrayAttr,
                                                                        circt::StringAttr{}).getResult();
        if (symbolTable.count(symbolIdentifier) == 0) {
            symbolTable[symbolIdentifier] = regOrWireOp;
        }
        returnValue = emitFromSymbol(std::get<0>(backward[1]), std::get<1>(backward[1]));
    } else {
        if (backward.empty()) {
            return {};
        }

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

    if (!isOutput && symbolTable.count(symbolIdentifier) == 0 && !needConnect) {
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

    if (needConnect) {
        circt::firrtl::emitConnect(implicitLocOpBuilder, regOrWireOp, returnValue);
        return regOrWireOp;
    }

    return returnValue;
}

std::string EmitFIRRTL::emit() {
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

    std::unordered_map<std::string, bool> isClockSignal;
    for (const auto &reg: rtlModule.registers) {
        isClockSignal[reg->clockSymbolName] = true;
    }

    for (const auto &ioPort: rtlModule.ioPorts) {
        auto name = circt::StringAttr::get(&context, circt::StringRef{ioPort->getIdentifier()});
        circt::firrtl::FIRRTLType type;
        if (isClockSignal[ioPort->getIdentifier()]) {
            type = circt::firrtl::ClockType::get(&context);
        } else {
            type = circt::firrtl::UIntType::get(&context, ioPort->getSlicing().getWidth());
        }
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
            if (rhs.getImpl() == nullptr) {
                continue;
            }
            circt::firrtl::emitConnect(implicitLocOpBuilder, symbolTable[ioPort->getIdentifier()], rhs);
        }
    }

    mlir::PassManager pm(&context);
    circt::firtool::FirtoolOptions opt{};
    if (mlir::failed(circt::firtool::populatePreprocessTransforms(pm, opt))) {
        mlirModule.dump();
        throw std::runtime_error("Unable to populate preprocess passes for MLIR");
    }

    if (mlir::failed(pm.run(mlirModule))) {
        mlirModule.dump();
        throw std::runtime_error("Unable to run preprocess passes");
    }

    std::string irStr;
    llvm::raw_string_ostream outs{irStr};
    mlirModule.print(outs);
    mlirModule.erase();

    return irStr;
}

bool EmitFIRRTL::isOutputPort(std::string &identifier) const {
    for (const auto &port: rtlModule.ioPorts) {
        if (port->getPortDirection() == PortDirection::Output && port->getIdentifier() == identifier) {
            return true;
        }
    }
    return false;
}
