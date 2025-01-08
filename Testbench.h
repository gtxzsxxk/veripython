//
// Created by unknown on 2024/12/16.
//

#ifndef VERIPYTHON_TESTBENCH_H
#define VERIPYTHON_TESTBENCH_H

#include <circt/Dialect/Arc/ModelInfo.h>

class Testbench {
    std::vector<circt::arc::ModelInfo> modelInfos;
public:
    explicit Testbench(mlir::ModuleOp &moduleOp);

    std::string emitPythonModule(const std::string &ir);
};


#endif //VERIPYTHON_TESTBENCH_H
