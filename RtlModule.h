//
// Created by unknown on 2024/11/2.
//

#ifndef VERIPYTHON_RTLMODULE_H
#define VERIPYTHON_RTLMODULE_H

#include "AST.h"
#include "CircuitSymbol.h"
#include <memory>
#include <string>
#include <vector>

class RtlModule {
    std::string xmlAstData = "You should build circuit at first!";

    std::vector<CircuitConnection> circuitConnections;

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
    std::vector<std::shared_ptr<CircuitSymbolReg>> registers;
    std::string moduleName;

    std::shared_ptr<ModuleIOPort> getModuleIOPortByName(const std::string &name);

    void addCircuitConnection(CircuitConnection &&connection);

    void addAlwaysBlock(std::unique_ptr<AlwaysBlockAST> &&alwaysBlock);

    void buildCircuit();

    [[nodiscard]] std::string toString() const;
};


#endif //VERIPYTHON_RTLMODULE_H
