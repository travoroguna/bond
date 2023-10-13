//
// Created by Travor Oguna Oneya on 03/04/2023.
//

#ifndef BOND_SRC_API_H_
#define BOND_SRC_API_H_

#include "gc.h"
#include "vm.h"
#include <typeinfo>

#ifdef _WIN32
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT extern "C"
#endif

namespace bond {
extern Vm *current_vm;

void set_current_vm(Vm *vm);

Vm *get_current_vm();
} // namespace bond

#endif // BOND_SRC_API_H_
