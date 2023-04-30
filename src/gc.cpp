#include "gc.h"
#include <cstdlib>
#include <fmt/core.h>
#include <chrono>
#include <plibsys.h>

#undef DEBUG

namespace bond {
    GarbageCollector &GarbageCollector::instance() {
        static GarbageCollector gc;
        return gc;
    }

    GarbageCollector::GarbageCollector() {
        m_gc = this;
    };

    void GarbageCollector::collect() {
        for (auto root: m_roots) {
            root->mark();
        }

        for (auto &obj: m_objects) {
            if (!obj.is_marked()) {
                obj.get()->~Object();
                std::free(obj.get());
                obj.reset();
            }
        }

        m_objects.erase(std::remove_if(m_objects.begin(),
                                       m_objects.end(),
                                       [](GcPtr<Object> &obj) { return obj.get() == nullptr; }),
                        m_objects.end());

        for (auto root: m_roots) {
            root->unmark();
        }
        gc_cycles++;
    }

    void GarbageCollector::collect_old_objects() {
        for (auto &obj: m_gc->m_old_objects) {
            if (obj.is_marked()) {

            } else {
                obj.get()->~Object();
                std::free(obj.get());
                obj.reset();
            }
        }

        m_gc->m_old_objects.erase(std::remove_if(m_gc->m_old_objects.begin(),
                                                 m_gc->m_old_objects.end(),
                                                 [](GcPtr<Object> &obj) { return obj.get() == nullptr; }),
                                  m_gc->m_old_objects.end());
    }

    void GarbageCollector::collect_if_needed() {
        if (!m_gc->m_collect) return;
        if (m_gc->m_objects.size() < m_gc->m_alloc_limit) return;

        m_gc->collect();

        m_gc->m_alloc_limit = m_gc->m_objects.size() * 2;

#ifdef DEBUG
        fmt::print("[GC] allocated {}, freed {}, alloc limit {}, immortal {}, old {}, took {} ms\n", m_objects.size(),
                   prev - m_objects.size(), m_alloc_limit, m_immortal.size(), m_old_objects.size(),
                   std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
#endif
    }

    void Root::mark() {
        for (auto &obj: m_stack) {
            obj.mark();
        }
    }

    void Root::unmark() {
        for (auto &obj: m_stack) {
            obj.unmark();
        }
    }

    GarbageCollector::~GarbageCollector() {
#ifndef DEBUG
        fmt::print("[GC] allocated {}, alloc limit {}, immortal {}, old {}\n", m_objects.size(),
                   m_alloc_limit, m_immortal.size(), m_old_objects.size());
#endif

        for (auto &obj: m_objects) {
            // allocated using malloc, but we also want the destructor to be called
            obj.get()->~Object();
            std::free(obj.get());
        }

        for (auto &old: m_old_objects) {
            old.get()->~Object();
            std::free(old.get());
        }

        for (auto &obj: m_immortal) {
            delete obj.get();
        }
    }

    std::string Object::str() {
        return "GarbageCollector";
    }


}; // namespace bond

void *operator new(size_t size, bond::GarbageCollector &gc) {
    return gc.allocate(size);
}

void operator delete(void *ptr, bond::GarbageCollector &gc) {
    auto obj = reinterpret_cast<bond::GcPtr<bond::Object> *>(ptr);
    obj->get()->~Object();
    std::free(obj->get());
    obj->reset();
}
