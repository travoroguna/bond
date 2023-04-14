#include "gc.h"
#include <cstdlib>
#include <fmt/core.h>
#include <chrono>

#undef DEBUG

namespace bond {
    GarbageCollector &GarbageCollector::instance() {
        static GarbageCollector gc;
        return gc;
    }

    GarbageCollector::GarbageCollector() = default;

    void GarbageCollector::collect() {
        for (auto &root: m_gc->m_roots) {
            root->mark();
        }

        for (auto &obj: m_gc->m_objects) {
            if (obj.is_marked()) {
            } else {
                obj.get()->~Object();
                std::free(obj.get());
                obj.reset();
            }
        }

        m_objects.erase(std::remove_if(m_gc->m_objects.begin(),
                                       m_gc->m_objects.end(),
                                       [](GcPtr<Object> &obj) { return obj.get() == nullptr; }),
                        m_gc->m_objects.end());

        for (auto root: m_gc->m_roots) {
            root->unmark();
        }
    }

    void GarbageCollector::collect_if_needed() {
        if (!m_gc->m_collect) return;
        if (m_gc->m_objects.size() < m_gc->m_alloc_limit) return;
        auto prev = m_objects.size();
        auto start = std::chrono::high_resolution_clock::now();
        m_gc->collect();
        auto end = std::chrono::high_resolution_clock::now();
        m_gc->m_alloc_limit = m_gc->m_objects.size() * 2 + 100;

#ifdef DEBUG
        fmt::print("[GC] allocated {}, freed {}, alloc limit {}, took {} ms\n", m_objects.size(),
                   prev - m_objects.size(), m_alloc_limit, m_immortal.size(),
                   std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
#endif
    }

    void Root::mark() {
        size_t count = 0;

        while (count < m_stack_ptr) {
            m_stack[count++].mark();
        }
    }

    void Root::unmark() {
        size_t count = 0;

        while (count < m_stack_ptr) {
            m_stack[count++].unmark();
        }
    }

    GarbageCollector::~GarbageCollector() {
        fmt::print("[Gc] at exit, allocated objects: {}, roots {}, immortal objects {}\n",
                   m_objects.size(),
                   m_roots.size(),
                   m_immortal.size());

        for (auto &obj: m_objects) {
            // allocated using malloc, but we also want the destructor to be called
            obj.get()->~Object();
            std::free(obj.get());
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
    // do nothing
}
