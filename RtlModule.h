//
// Created by hanyuan on 2024/11/2.
//

#ifndef VERIPYTHON_RTLMODULE_H
#define VERIPYTHON_RTLMODULE_H

#include "AST.h"
#include "CircuitSymbol.h"
#include <memory>
#include <string>
#include <vector>

class HardwareConnection {
public:
    std::shared_ptr<CircuitSymbol> destSymbol;
    std::shared_ptr<CircuitSymbol> fromSymbol;
    PortSlicingAST destSlicing{-1, -1};
    PortSlicingAST inputSlicing{-1, -1};

    HardwareConnection(std::shared_ptr<CircuitSymbol> &destSymbol,
                       std::shared_ptr<CircuitSymbol> &fromSymbol,
                       PortSlicingAST &inputSlicing);

    HardwareConnection(std::shared_ptr<CircuitSymbol> &destSymbol,
                       std::shared_ptr<CircuitSymbol> &fromSymbol,
                       PortSlicingAST &inputSlicing,
                       PortSlicingAST &destSlicing);
};

class RtlModule {
    std::string xmlAstData = "You should build circuit at first!";

    std::vector<CircuitConnection> circuitConnections;

    std::vector<HardwareConnection> hardwareConnections;

    std::vector<std::unique_ptr<AlwaysBlockAST>> alwaysBlocks;

    std::shared_ptr<CircuitSymbol> getPortOrSymbolById(const std::string &id);

    std::shared_ptr<CircuitSymbol> genCircuitSymbolByHDLExprAST(HDLExpressionAST *ast);

    std::vector<CircuitConnection> genByAlwaysBlockBody(AlwaysBlockBodyAST *ast);

    void addConditionForAlwaysBlockBody(std::vector<CircuitConnection> &blockBody,
                                        std::shared_ptr<HDLExpressionAST> &condition);

    void genAstJson();

public:
    std::vector<std::shared_ptr<ModuleIOPort>> ioPorts;
    std::vector<std::shared_ptr<CircuitSymbol>> circuitSymbols;
    std::string moduleName;

    std::shared_ptr<ModuleIOPort> getModuleIOPortByName(const std::string &name);

    void addCircuitConnection(CircuitConnection &&connection);

    void addAlwaysBlock(std::unique_ptr<AlwaysBlockAST> &&alwaysBlock);

    void buildCircuit();

    [[nodiscard]] const std::vector<HardwareConnection> &getHardwareConnections() const;

    [[nodiscard]] std::string toString() const;
};


#endif //VERIPYTHON_RTLMODULE_H
