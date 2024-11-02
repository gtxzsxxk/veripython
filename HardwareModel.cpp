//
// Created by hanyuan on 2024/11/2.
//

#include "HardwareModel.h"
#include <stdexcept>

const std::string &ModuleIOPort::getPortName() const {
    return identifier;
}

void ModuleIOPort::setPortDirection(PortDirection newDirection) {
    if (direction != PortDirection::Unspecified) {
        throw std::runtime_error("This port's direction has been specified");
    }
    direction = newDirection;
}

ModuleIOPort &HardwareModule::getModuleIOPortByName(const std::string &name) {
    for (auto &port: ioPorts) {
        if (port.getPortName() == name) {
            return port;
        }
    }

    throw std::runtime_error("No such an I/O port");
}
