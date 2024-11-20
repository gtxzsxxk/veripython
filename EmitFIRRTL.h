//
// Created by hanyuan on 2024/11/20.
//

#ifndef VERIPYTHON_EMITFIRRTL_H
#define VERIPYTHON_EMITFIRRTL_H
#include "RtlModule.h"

class EmitFIRRTL {
    const RtlModule &rtlModule;
public:
    explicit EmitFIRRTL(const RtlModule &module): rtlModule(module) {}
};


#endif //VERIPYTHON_EMITFIRRTL_H
