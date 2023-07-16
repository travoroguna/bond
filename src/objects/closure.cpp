//
// Created by travor on 11/07/2023.
//
#include "../object.h"


namespace bond {
    GcPtr<NativeStruct> CLOSURE_STRUCT = make_immortal<NativeStruct>("Closure", "Closure(closure)", c_Default<Closure>);
}