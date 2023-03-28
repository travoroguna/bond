//
// Created by Travor Oguna Oneya on 28/03/2023.
//

#include "test.h"


int main() {
    bond::GarbageCollector::instance().make_immortal<bond::String>("hello");
    ASSERT(false);
}