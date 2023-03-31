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

        explicit GcPtr(T *GcPtr) : m_ptr(GcPtr) {}

        GcPtr(const GcPtr &other) : m_ptr(other.m_ptr) {}

        GcPtr(GcPtr &&other)  noexcept : m_ptr(other.m_ptr) { other.m_ptr = nullptr; }

        ~GcPtr() { m_ptr = nullptr; }

        GcPtr &operator=(const GcPtr &other) {
            m_ptr = other.m_ptr;
            return *this;
        }

        GcPtr &operator=(GcPtr &&other) {
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
            return *this;
        }

        T *operator->() const { return m_ptr; }

        T &operator*() const { return *m_ptr; }

        T *get() const { return m_ptr; }
        void set(T* ptr) { m_ptr = ptr; }

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

    enum class RuntimeError {
        TypeError,
        Unimplemented,
        DivisionByZero,
        GenericError,
        ExpectedNumberIndex,
        ExpectedWholeNumberIndex,
        IndexOutOfBounds,
    };

#define UNIMPLEMENTED return std::unexpected(RuntimeError::Unimplemented)
#define OBJ_RESULT std::expected<GcPtr<Object>, RuntimeError>

    class Object {
    public:
        Object() = default;
        virtual ~Object() = default;


        [[nodiscard]] bool is_marked() const { return m_marked; }

        virtual void mark() { m_marked = true; }

        virtual void unmark() { m_marked = false; }

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

        virtual OBJ_RESULT $add(const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $sub(const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $mul(const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $div(const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $eq(const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $ne(const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $lt(const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $le(const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $gt(const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $ge(const GcPtr<Object> &other) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $_bool() { UNIMPLEMENTED; }

        virtual OBJ_RESULT $set_item(const GcPtr<Object> &index, const GcPtr<Object> &value) { UNIMPLEMENTED; }

        virtual OBJ_RESULT $get_item(const GcPtr<Object> &index) { UNIMPLEMENTED; }

        bool operator==(const GcPtr<Object> &other) { return equal(other); }

        virtual std::string str();

        virtual bool equal(const GcPtr<Object> &other) = 0;
        virtual size_t hash() = 0;

        struct HashMe {
            size_t operator()(const GcPtr<Object> &other) const{
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

        void collect();

        template<typename T, typename... Args>
        GcPtr<T> make_immortal(Args &&...args) {
            auto t = GcPtr<T>(new T(std::forward<Args>(args)...));
            m_immortal.emplace_back(t);
            return t;
        }

        template<typename T, typename... Args>
        GcPtr<T> make(Args &&...args) {
            return GcPtr<T>(new(instance()) T(std::forward<Args>(args)...));
        }

        void *allocate(size_t size) {
            collect_if_needed();
            auto ptr = (Object *) std::malloc(size);
            m_objects.emplace_back(ptr);
            return ptr;
        }

        ~GarbageCollector();

        void add_root(Root *root) { m_roots.push_back(root); }

        void stop_gc() { m_collect = false; }

        void resume_gc() { m_collect = true; }


    private:
        GarbageCollector();

        std::vector<GcPtr<Object>> m_objects;
        std::vector<GcPtr<Object>> m_immortal;
        std::vector<Root *> m_roots;
        size_t m_alloc_limit = 100;

        void collect_if_needed();

        bool m_collect = true;
    };

}; // namespace bond

void *operator new(size_t size, bond::GarbageCollector &gc);

void operator delete(void *ptr, bond::GarbageCollector &gc);
