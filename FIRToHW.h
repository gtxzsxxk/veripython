//
// Created by unknown on 2024/12/14.
//

#ifndef VERIPYTHON_FIRTOHW_H
#define VERIPYTHON_FIRTOHW_H

#include <mlir/IR/BuiltinOps.h>

class FIRToHW {
    mlir::MLIRContext &context;
public:
    explicit
    FIRToHW(mlir::MLIRContext &context) : context(context) {}

    void convertToHW(mlir::ModuleOp &moduleOp);
};

#endif //VERIPYTHON_FIRTOHW_H
