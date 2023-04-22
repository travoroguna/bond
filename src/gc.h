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

#define DEBUG


namespace bond {
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

        ~GcPtr() { m_ptr = nullptr; }

        GcPtr &operator=(const GcPtr &other) {
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

        T *get() const { return m_ptr; }

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

        bool is_marked() const { return m_ptr->is_marked(); }

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

        virtual void mark() { m_marked = true; }

        virtual void unmark() { m_marked = false; }

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

        virtual OBJ_RESULT $get_attribute(const GcPtr<Object> &index) {
            UNIMPLEMENTED;
        }

        virtual OBJ_RESULT $iter([[maybe_unused]] const GcPtr<Object> &self) { UNIMPLEMENTED; }

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

    protected:
        bool m_marked = false;
    };

#define STACK_MAX 1024

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

        void push(GcPtr<Object> const &obj) { m_stack[m_stack_ptr++] = obj; }

        GcPtr<Object> pop() { return m_stack[--m_stack_ptr]; }

        GcPtr<Object> peek() { return m_stack[m_stack_ptr - 1]; }

    protected:
        size_t m_stack_ptr = 0;
        std::array<GcPtr<Object>, STACK_MAX> m_stack;

    };


    class GarbageCollector {
    public:
        static GarbageCollector &instance();

        void set_gc(GarbageCollector *_gc) { m_gc = _gc; }

        void collect();

        void set_main_thread_id(std::thread::id id) { m_main_thread_id = id; }

        template<typename T, typename... Args>
        GcPtr<T> make_immortal(Args &&...args) {
            m_gc->m_mutex.lock();
            auto t = GcPtr<T>(new T(std::forward<Args>(args)...));
            m_gc->m_immortal.emplace_back(t);
            m_gc->m_mutex.unlock();
            return t;
        }

        template<typename T, typename... Args>
        GcPtr<T> make(Args &&...args) {
            m_gc->m_mutex.lock();
            auto t = GcPtr<T>(new(*this) T(std::forward<Args>(args)...));
            m_gc->m_mutex.unlock();
            return t;
        }


        void *allocate(size_t size) {
            collect_if_needed();
            auto ptr = (Object *) std::malloc(size);
            m_gc->m_objects.emplace_back(ptr);
            return ptr;
        }

        ~GarbageCollector();

        void add_root(Root *root) { m_gc->m_roots.push_back(root); }

        void stop_gc() {
            m_gc->m_collect = false;
        }

        void resume_gc() {
            m_gc->m_collect = true;
        }

        void pop_root() {
            m_gc->m_roots.pop_back();
        }

        void set_alloc_limit(size_t limit) { m_gc->m_alloc_limit = limit; }

        size_t get_alloc_limit() { return m_gc->m_alloc_limit; }

        size_t get_alloc_count() { return m_gc->m_objects.size(); }

        size_t get_immortal_count() { return m_gc->m_immortal.size(); }

        void remove_root(Root *root) {
            auto it = std::find(m_gc->m_roots.begin(), m_gc->m_roots.end(), root);
            if (it != m_gc->m_roots.end()) {
                m_gc->m_roots.erase(it);
            }
        }

    private:
        GarbageCollector();

        GarbageCollector *m_gc = this;

        std::vector<GcPtr<Object>> m_objects;
        std::vector<GcPtr<Object>> m_old_objects;
        std::vector<Root *> m_roots;
        size_t m_alloc_limit = 200;
        size_t m_old_alloc_limit = 300;

        void collect_if_needed();

        std::thread::id m_main_thread_id;
        bool m_collect = true;
        std::recursive_mutex m_mutex;

        void collect_old_objects();

        int gc_cycles = 0;
        std::vector<GcPtr<Object>> m_immortal;

    };

}; // namespace bond

void *operator new(size_t size, bond::GarbageCollector &gc);

void operator delete(void *ptr, bond::GarbageCollector &gc);

