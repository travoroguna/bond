//
// Created by Travor Oguna Oneya on 28/03/2023.
//
#include "test.h"


void test_create(){
    auto map = bond::GarbageCollector::instance().make_immortal<bond::Map>();
    ASSERT(map.get() != nullptr)
}

void test_insert(){
    auto map = bond::GarbageCollector::instance().make_immortal<bond::Map>();
    auto string = bond::GarbageCollector::instance().make_immortal<bond::String>("bond test");
    auto number = bond::GarbageCollector::instance().make_immortal<bond::Number>(1999);

    map->set(string, number);

    ASSERT(map->get(string).has_value())
    ASSERT(map->get_unchecked(string)->equal(number))
}

void test_key_not_found(){
    auto map = bond::GarbageCollector::instance().make_immortal<bond::Map>();
    auto string = bond::GarbageCollector::instance().make_immortal<bond::String>("bond test");
    auto number = bond::GarbageCollector::instance().make_immortal<bond::Number>(1999);

    map->set(string, number);

    ASSERT(!map->get(number).has_value())

}

void test_clobber() {
    auto map = bond::GarbageCollector::instance().make_immortal<bond::Map>();
    auto string = bond::GarbageCollector::instance().make_immortal<bond::String>("bond test");
    auto number = bond::GarbageCollector::instance().make_immortal<bond::Number>(1999);
    auto number2 = bond::GarbageCollector::instance().make_immortal<bond::Number>(2000);


    map->set(string, number);
    ASSERT(map->get(string).has_value())

    map->set(string, number2);
    ASSERT(map->get(string).has_value())


}

int main() {
    test_create();
    test_insert();
}