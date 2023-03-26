#include <algorithm>
#include <array>
#include <cstdlib>
#include <memory>
#include <type_traits>
#include <vector>
#include <functional>
#include <expected>
#include <optional>

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

        bool is_marked() const { return m_ptr && m_ptr->is_marked(); }

        bool operator==(GcPtr const &other) const { return m_ptr == other.m_ptr; }

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
        DivisionByZero
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

        virtual std::string str();

    protected:
        bool m_marked = false;
    };

#define STACK_SIZE 1024
#define FRAME_MAX 1024

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

    private:
        std::array<GcPtr<Object>, STACK_MAX> m_stack;
        size_t m_stack_ptr = 0;
    };

    class GarbageCollector {
    public:
        static GarbageCollector &instance();

        void collect();

        void push(GcPtr<Object> const &obj) { m_stack[m_stack_ptr++] = obj; }

        GcPtr<Object> pop() { return m_stack[--m_stack_ptr]; }

        template<typename T, typename... Args>
        GcPtr<T> make_immortal(Args &&...args) {
            auto t = GcPtr<T>(new(instance()) T(std::forward<Args>(args)...));
            m_immortal.emplace_back(t);
            return t;
        }

        template<typename T, typename... Args>
        GcPtr<T> make(Args &&...args) {
            return GcPtr<T>(new(instance()) T(std::forward<Args>(args)...));
        }

        template<typename T, typename... Args>
        void push(Args &&...args) {
            auto ptr = make<T>(std::forward<Args>(args)...);
            Object *obj = ptr.get();
            push(GcPtr<Object>(obj));
        }

        void *allocate(size_t size) {
            auto ptr = std::malloc(size);
            m_objects.emplace_back((Object *) ptr);
            return ptr;
        }

        ~GarbageCollector();

        void add_root(Root *root) { m_roots.push_back(root); }

    private:
        GarbageCollector();

        std::vector<GcPtr<Object>> m_objects;
        std::vector<GcPtr<Object>> m_immortal;
        std::vector<Root *> m_roots;
    };

}; // namespace bond

void *operator new(size_t size, bond::GarbageCollector &gc);

void operator delete(void *ptr, bond::GarbageCollector &gc);
