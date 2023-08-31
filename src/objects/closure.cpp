//
// Created by travor on 11/07/2023.
//
#include "../object.h"
#include "../runtime.h"

namespace bond {

    void init_closure() {
        Runtime::ins()->CLOSURE_STRUCT = make_immortal<NativeStruct>("Closure", "Closure(closure)", c_Default<Closure>);
    }
}