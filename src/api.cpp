//
// Created by Travor Oguna Oneya on 03/04/2023.
//


#include "api.h"
#include "bond.h"

namespace bond {
    Vm *current_vm = nullptr;

    void set_current_vm(Vm *vm) {
        current_vm = vm;
    }

    Vm *get_current_vm() {
        return current_vm;
    }
};