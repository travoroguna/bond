//
// Created by Travor Oguna Oneya on 28/03/2023.
//

#include "test.h"


void test_create_object(){
    bond::GarbageCollector::instance().set_main_thread_id(std::this_thread::get_id());
    bond::GarbageCollector::instance().make_thread_storage();

    auto obj = bond::GarbageCollector::instance().make_immortal<bond::String>("hello");
    ASSERT(obj.get() != nullptr)
    ASSERT(obj->get_value() == "hello")

    auto obj2 = bond::GarbageCollector::instance().make<bond::String>("world");
    ASSERT(obj2.get() != nullptr)
    ASSERT(obj2->get_value() == "world")
};

int main() {
    test_create_object();
}