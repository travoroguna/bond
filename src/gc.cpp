#include "gc.h"
#include <cstdlib>

namespace bond {
    GarbageCollector &GarbageCollector::instance() {
        static GarbageCollector gc;
        return gc;
    }

    GarbageCollector::GarbageCollector() { m_stack.fill(nullptr); }

    void GarbageCollector::collect() {
        for (auto &obj: m_stack) {
            if (obj) {
                obj->mark();
            }
        }

        for (auto &obj: m_objects) {
            if (obj.is_marked()) {
                obj.unmark();
            } else {
                obj.get()->~Object();
                std::free(obj.get());
                obj.reset();
            }
        }

        m_objects.erase(std::remove_if(m_objects.begin(),
                                       m_objects.end(),
                        [](GcPtr<Object> &obj) { return obj.get() == nullptr; }),
                        m_objects.end());
    }


    void Root::mark() {
        for (auto &obj: m_stack) {
            if (obj) {
                obj->mark();
            }
        }
    }


    void Root::unmark() {
        for (auto &obj: m_stack) {
            if (obj) {
                obj->unmark();
            }
        }
    }


    GarbageCollector::~GarbageCollector() {
        for (auto &obj: m_objects) {
            obj.get()->~Object();
            std::free(obj.get());
        }

        for (auto &obj: m_immortal) {
            obj.get()->~Object();
            std::free(obj.get());
        }
    }

    std::string Object::str() {
        return "object";
    }
}; // namespace bond

void *operator new(size_t size, bond::GarbageCollector &gc) {
    return gc.allocate(size);
}

void operator delete(void *ptr, bond::GarbageCollector &gc) {
    // do nothing
}
