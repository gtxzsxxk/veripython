//
// Created by hanyuan on 2024/12/15.
//

#ifndef VERIPYTHON_ARCBACKEND_H
#define VERIPYTHON_ARCBACKEND_H

#include <mlir/IR/BuiltinOps.h>
#include <mlir/Target/LLVMIR/Export.h>

class ArcBackend {
    mlir::MLIRContext &context;
public:
    explicit ArcBackend(mlir::MLIRContext &context);

    void convertHWToArc(mlir::ModuleOp &moduleOp);

    std::string convertArcToLLVMIR(mlir::ModuleOp &moduleOp);

    void arcExportState();
};


#endif //VERIPYTHON_ARCBACKEND_H
