//
// Created by hanyuan on 2024/11/4.
//

#ifndef VERIPYTHON_RTLVISUALIZER_H
#define VERIPYTHON_RTLVISUALIZER_H

#include "RtlModule.h"

class RtlVisualizer {
public:
    static void visualize(const RtlModule &hardwareModel, const std::string &filename = "rtl_view");
};


#endif //VERIPYTHON_RTLVISUALIZER_H
