#include "gc.h"
#include <cstdlib>
#include <fmt/core.h>
#include <chrono>
#include <sstream>
#include <cassert>

#undef DEBUG

namespace bond {
    std::string thread_string() {
        return thread_string(std::this_thread::get_id());
    }

    std::string thread_string(std::thread::id id) {
        std::stringstream ss;
        ss << id;
        return ss.str();
    }

    GarbageCollector &GarbageCollector::instance() {
        static GarbageCollector gc;
        return gc;
    }

    GarbageCollector::GarbageCollector() {
        m_gc = this;
    }

    void GarbageCollector::collect_if_needed() {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        m_gc->m_thread_storages[std::this_thread::get_id()]->collect_if_needed();
//            fmt::print("[GC]({}) done. {} threads\n", thread_string(), m_gc->m_thread_storages.size());
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
        for (auto &obj: m_immortal) {
            delete obj.get();
        }

        if (m_main_thread_id != std::this_thread::get_id()) return;

        for (auto &[_, storage]: m_gc->m_thread_storages) {
            delete storage;
        }
    }

    std::string Object::str() {
        return fmt::format("<{} at {}>", typeid(this).name(), (void *) this);
    }

    void GarbageCollector::free(void *ptr) {
//        auto obj = (GcPtr<Object>*) ptr;
//        obj->get()->~Object();
//        std::free(obj->get());
//        obj->reset();
    }

    void GarbageCollector::remove_thread_storage(std::thread::id id) {
//        auto lock = LockGc();
        std::lock_guard<std::recursive_mutex> lock(m_gc->m_mutex);

        for (auto &[id_, storage]: m_gc->m_thread_storages) {
            if (id_ == std::this_thread::get_id()) continue;
            storage->mark_roots();
        }

        assert(m_gc->m_thread_storages.contains(id));
        auto drop = m_gc->m_thread_storages[id];
        drop->drop_reachable();

        for (auto &[_, storage]: m_gc->m_thread_storages) {
            if (_ == std::this_thread::get_id()) continue;
            storage->unmark_roots();
        }

        auto it = m_gc->m_thread_storages.find(id);
        if (it != m_gc->m_thread_storages.end()) {
            m_gc->m_thread_storages.erase(it);
        }

        delete drop;
    }

    void ThreadStorage::mark_roots() {
        for (auto root: m_roots) {
            root->mark();
        }
    }

    void ThreadStorage::drop_reachable() {
        for (auto &obj: m_objects) {

            if (obj.get() == nullptr) continue;
            if (!obj.is_marked()) {
//                fmt::print("[GC]({}) collect: {}\n", thread_string(), obj->str());
//                free((void *)&obj);
                obj.get()->~Object();
                std::free(obj.get());
                obj.reset();
            }
        }

        m_objects = {};
    }

    void ThreadStorage::unmark_roots() {
        for (auto root: m_roots) {
            root->unmark();
        }
    }

    void ThreadStorage::stop_collect() {
        m_collect = false;
    }

    void ThreadStorage::resume_collect() {
        m_collect = true;
    }

    ThreadStorage::~ThreadStorage() {
        for (auto &obj: m_objects) {
            // allocated using malloc, but we also want the destructor to be called
            if (obj.get() == nullptr) continue;
            obj.get()->~Object();
            std::free(obj.get());
            obj.reset();
        }
    }

    bool ThreadStorage::collect_if_needed() {
        if (m_objects.size() < m_allocation_limit or !m_collect) return false;
        collect();
        m_allocation_limit = m_objects.size() * 2 + 200;
        return true;
    }

    void ThreadStorage::collect() {
        for (auto root: m_roots) {
            root->mark();
        }

//        fmt::print("[GC]({}) collect start, objects: {}, alloc limit {}\n", thread_string(), m_objects.size(),
//                   m_allocation_limit);

        for (auto &obj: m_objects) {

            if (obj.get() == nullptr) continue;
            if (!obj.is_marked()) {
//                fmt::print("[GC]({}) collect: {}\n", thread_string(), obj->str());
//                free((void *)&obj);
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

//        fmt::print("[GC]({}) collect end, objects: {}, alloc limit {}\n", thread_string(), m_objects.size(),
//                   m_allocation_limit);
    }

}; // namespace bond

void *operator new(size_t size, bond::GarbageCollector &gc) {
    return gc.allocate(size);
}

void operator delete(void *ptr, bond::GarbageCollector &gc) {
    return gc.free(ptr);
}
