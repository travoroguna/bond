#pragma once

#include <algorithm>
#include <array>
#include <cstdlib>
#include <memory>
#include <type_traits>
#include <vector>
#include <functional>
#include <expected>
#include <optional>
#include <mutex>
#include <ranges>
#include <cassert>
#include <thread>


#define DEBUG

namespace bond::Globs {
    extern std::recursive_mutex m_recursive_mutex;
}

namespace bond {
    std::string thread_string();

    std::string thread_string(std::thread::id id);

    class GarbageCollector;

    class Object;

    template<typename Base, typename T>
    inline bool instanceof(const T *ptr) {
        return dynamic_cast<const Base *>(ptr) != nullptr;
    }

    template<typename T, typename = std::enable_if_t<std::is_base_of_v<Object, T>>>
    class GcPtr {
    public:
        GcPtr() = default;

        GcPtr(T *GcPtr) : m_ptr(GcPtr) {}

        GcPtr(const GcPtr &other) : m_ptr(other.m_ptr) {}

        GcPtr(GcPtr &&other) noexcept: m_ptr(other.m_ptr) { other.m_ptr = nullptr; }

        virtual ~GcPtr() { m_ptr = nullptr; }

        GcPtr &operator=(const GcPtr &other) {
            if (this == &other)
                return *this;
            m_ptr = other.m_ptr;
            return *this;
        }

        GcPtr &operator=(const GcPtr *other) {
            m_ptr = other->m_ptr;
            return *this;
        }

        GcPtr &operator=(T *other) {
            m_ptr = other;
            return *this;
        }

        GcPtr &operator=(GcPtr &&other) noexcept {
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
            return *this;
        }

        T *operator->() const { return m_ptr; }

        T &operator*() const { return *m_ptr; }

        [[nodiscard]] T *get() const { return m_ptr; }

        void set(T *ptr) { m_ptr = ptr; }

        [[nodiscard]] const char *type_name() const {
            return typeid(*m_ptr).name();
        }

        explicit operator bool() const { return m_ptr != nullptr; }

        void mark() {
            if (m_ptr)
                m_ptr->mark();
        }

        void unmark() {
            if (m_ptr)
                m_ptr->unmark();
        }

        void reset() { m_ptr = nullptr; }

        [[nodiscard]] bool is_marked() const { return m_ptr->is_marked(); }

        bool operator==(GcPtr const &other) const { return m_ptr->equal(other); }

        template<typename K>
        GcPtr(const GcPtr <K> &other) : m_ptr(other.get()) {}

        template<typename K>
        void use_if(std::function<void(K *)> const &func) {
            if (instanceof<K>(m_ptr))
                func(dynamic_cast<K *>(m_ptr));
        }

        template<typename K>
        void use_if(std::function<void(K &)> const &func) {
            if (instanceof<K>(m_ptr))
                func(*dynamic_cast<K *>(m_ptr));
        }

        template<typename K, typename R>
        std::optional<R> use_if(std::function<R(K &)> const &func) {
            if (instanceof<K>(m_ptr))
                return func(*dynamic_cast<K *>(m_ptr));

            return std::nullopt;
        }

    private:
        T *m_ptr = nullptr;
    };

    template<typename T>
    struct GcPtrTraits {
        using element_type = typename T::element_type;
    };

    template<typename T>
    struct GcPtrTraits<T *> {
        using element_type = T;
    };

    template<typename T>
    struct GcPtrTraits<GcPtr<T>> {
        using element_type = T;
    };

    template<typename T>
    using GcPtrElement = typename GcPtrTraits<T>::element_type *;

    enum class RuntimeError {
        TypeError,
        Unimplemented,
        DivisionByZero,
        GenericError,
        ExpectedNumberIndex,
        ExpectedWholeNumberIndex,
        IndexOutOfBounds,
        InvalidArgument,
        AttributeNotFound,
        ExpectedStringIndex,
        FileNotFound,
    };

#define UNIMPLEMENTED return std::unexpected(RuntimeError::Unimplemented)
#define OBJ_RESULT std::expected<GcPtr<Object>, RuntimeError>

    class Object {
    public:
        Object() = default;

        virtual ~Object() = default;

        [[nodiscard]] bool is_marked() const { return m_marked; }

        bool is_immortal() const { return m_immortal; }

        void set_immortal(bool immortal) { m_immortal = immortal; }

        template<typename T>
        static bool is(GcPtr<Object> const &obj) {
            return instanceof<T>(obj.get());
        }

        template<typename T>
        bool is() { return instanceof<T>(this); }

        template<typename T>
        GcPtr<T> as() {
            return GcPtr<T>(dynamic_cast<T *>(this));
        }

        template<typename T>
        static GcPtr<T> as(GcPtr<Object> const &obj) {
            return GcPtr<T>(dynamic_cast<T *>(obj.get()));
        }

        bool operator==(Object const &other) const { return this == &other; }

        virtual void mark() {
            m_marked = true;

            for (auto &[_, attr]: m_attr)
                attr->mark();
        }

        virtual void unmark() {
            m_marked = false;

            for (auto &[_, attr]: m_attr)
                attr->mark();
        }

        virtual OBJ_RESULT $add([[maybe_unused]] const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $sub([[maybe_unused]] const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $mul([[maybe_unused]] const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $div([[maybe_unused]] const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $eq([[maybe_unused]] const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $ne([[maybe_unused]] const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $mod([[maybe_unused]] const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $lt([[maybe_unused]] const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $le([[maybe_unused]] const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $gt([[maybe_unused]] const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $ge([[maybe_unused]] const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $call([[maybe_unused]] const std::vector<GcPtr<Object>> &arguments) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $_bool() { UNIMPLEMENTED; }

        virtual OBJ_RESULT $set_item([[maybe_unused]] const GcPtr<Object> &index,
                                     [[maybe_unused]] const GcPtr<Object> &value) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $get_item([[maybe_unused]] const GcPtr<Object> &index) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $set_attribute([[maybe_unused]] const GcPtr<Object> &index,
                                          [[maybe_unused]] const GcPtr<Object> &value) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $get_attribute([[maybe_unused]] const GcPtr<Object> &index) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $iter() { UNIMPLEMENTED; }

        virtual OBJ_RESULT $next() { UNIMPLEMENTED; }

        virtual OBJ_RESULT $has_next() { UNIMPLEMENTED; }

        virtual std::expected<GcPtr<Object>, std::string>
        call__([[maybe_unused]] std::vector<GcPtr<Object>> &arguments) { return std::unexpected(""); }

        virtual std::expected<bool, std::string>
        is_instance([[maybe_unused]] GcPtr<Object> const &other) { return std::unexpected("value is not a type"); }

        bool operator==(const GcPtr<Object> &other) { return equal(other); }

        virtual std::string str();

        virtual bool equal(const GcPtr<Object> &other) = 0;

        virtual size_t hash() = 0;

        struct HashMe {
            size_t operator()(const GcPtr<Object> &other) const {
                return other->hash();
            }
        };

        OBJ_RESULT get_attribute(const std::string &name) {
            if (m_attr.find(name) == m_attr.end()) {
                return std::unexpected(RuntimeError::AttributeNotFound);
            }
            return m_attr[name];
        }

        void set_attribute(const std::string &name, const GcPtr<Object> &value) {
            m_attr[name] = value;
        }


    protected:
        bool m_marked{false};
        bool m_immortal{false};
        std::unordered_map<std::string, GcPtr<Object>> m_attr;
    };


    class Root {
    public:
        Root() = default;

        virtual void mark();

        virtual void unmark();

        template<typename T, typename... Args>
        void push(Args &&...args) {
            auto ptr = make<T>(std::forward<Args>(args)...);
            Object *obj = ptr.get();
            push(GcPtr<Object>(obj));
        }

        void push(GcPtr<Object> const &obj) { m_stack.push_back(obj); }

        GcPtr<Object> pop() {
            auto res = peek();
            m_stack.pop_back();
            return res;
        }

        GcPtr<Object> peek() { return m_stack.back(); }

        void print_stack();

    protected:
        std::vector<GcPtr<Object>> m_stack;

    };


    class ThreadStorage {
    public:
        explicit ThreadStorage(std::thread::id t_id) { m_t_id = t_id; }

        ~ThreadStorage();

        bool collect_if_needed();

        void add_object(GcPtr<Object> const &obj) { m_objects.push_back(obj); }

        void add_root(Root *root) { m_roots.push_back(root); }

        void pop_root() { m_roots.pop_back(); }

        std::vector<Root *> &get_roots() { return m_roots; }

        void remove_root(Root *root) {
            auto it = std::find(m_roots.begin(), m_roots.end(), root);
            if (it != m_roots.end()) {
                m_roots.erase(it);
            }
        }

        void mark_roots();

        void unmark_roots();

        void stop_collect();

        void resume_collect();


        void drop_reachable();

    private:
        std::vector<GcPtr<Object>> m_objects;
        std::vector<Root *> m_roots;
        std::thread::id m_t_id;

        bool m_collect = true;

        size_t m_allocation_limit = 200;

        void collect();

    };

    class GarbageCollector {
    public:
        static GarbageCollector &instance();

        void set_gc(GarbageCollector *_gc) { m_gc = _gc; }

        void set_main_thread_id(std::thread::id id) { m_main_thread_id = id; }

        template<typename T, typename... Args>
        GcPtr<T> make_immortal(Args &&...args) {
            std::lock_guard<std::recursive_mutex> lock(m_gc->m_mutex);
            auto t = GcPtr<T>(::new T(std::forward<Args>(args)...));
            m_immortal.emplace_back(t);
            return t;
        }

        template<typename T, typename... Args>
        GcPtr<T> make(Args &&...args) {
            std::lock_guard<std::recursive_mutex> lock(m_gc->m_mutex);

            auto t = GcPtr<T>(new(*this) T(std::forward<Args>(args)...));
            auto id = std::this_thread::get_id();
            m_gc->m_thread_storages[id]->add_object(t);
            return t;
        }

        void make_thread_storage(std::thread::id id) {
            std::lock_guard<std::recursive_mutex> lock(m_gc->m_mutex);

            m_gc->m_thread_storages[id] = new ThreadStorage(id);
            assert(m_gc->m_thread_storages.contains(id) && m_thread_storages[id] != nullptr &&
                   "thread storage not created");
        }

        void make_thread_storage() {
            make_thread_storage(std::this_thread::get_id());
        }

        void remove_thread_storage(std::thread::id id);

        void *allocate(size_t size) {
            collect_if_needed();
            return (Object *) std::malloc(size);
        }

        ~GarbageCollector();

        void add_root(Root *root) {
            std::lock_guard<std::recursive_mutex> lock(m_gc->m_mutex);

            auto t_id = std::this_thread::get_id();
            m_gc->m_thread_storages[t_id]->add_root(root);
        }

        void stop_gc() {
            std::lock_guard<std::recursive_mutex> lock(m_gc->m_mutex);
            auto t_id = std::this_thread::get_id();
            m_gc->m_thread_storages[t_id]->stop_collect();
        }

        void resume_gc() {
            std::lock_guard<std::recursive_mutex> lock(m_gc->m_mutex);
            auto t_store = m_gc->m_thread_storages[std::this_thread::get_id()];
            t_store->resume_collect();
        }

        void aquire_gc() {
            Globs::m_recursive_mutex.lock();
        }

        void release_gc() {
            Globs::m_recursive_mutex.unlock();
        }

        void pop_root() {
            std::lock_guard<std::recursive_mutex> lock(m_gc->m_mutex);
            m_gc->m_thread_storages[std::this_thread::get_id()]->pop_root();
        }

        void set_alloc_limit(size_t limit) {
            // FIXME: set correct count and make thread safe
        }

        size_t get_alloc_limit() {
            // FIXME: return correct count and make thread safe
            return 100;
        }

        size_t get_alloc_count() {
            // FIXME: return correct count and make thread safe
            return 100;
        }

        size_t get_immortal_count() { return m_gc->m_immortal.size(); }

        void remove_root(Root *root) {
            std::lock_guard<std::recursive_mutex> lock(m_gc->m_mutex);
            m_gc->m_thread_storages[std::this_thread::get_id()]->remove_root(root);
        }

        std::vector<Root *> &get_roots() {
            std::lock_guard<std::recursive_mutex> lock(m_gc->m_mutex);
            return m_gc->m_thread_storages[std::this_thread::get_id()]->get_roots();
        }

        void collect_if_needed();

        void free(void *ptr);

    private:
        GarbageCollector();

        GarbageCollector *m_gc = this;
        size_t m_alloc_limit = 200;

        std::recursive_mutex m_mutex;
        std::vector<GcPtr<Object>> m_immortal;

        std::unordered_map<std::thread::id, ThreadStorage *> m_thread_storages;
        std::thread::id m_main_thread_id;
    };


    class LockGc {
    public:
        LockGc() {
            GarbageCollector::instance().stop_gc();
        }

        ~LockGc() {
            GarbageCollector::instance().resume_gc();
        }
    };

} // namespace bond

void *operator new(size_t size, bond::GarbageCollector &gc);

void operator delete(void *ptr, bond::GarbageCollector &gc);

