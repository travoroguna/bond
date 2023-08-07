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
#define GC_BUILD

#ifdef DEBUG
#define GC_DEBUG
#endif


#undef _free
#undef _realloc
#include <gc.h>
#include <gc_cpp.h>
#include <gc/gc_allocator.h>



namespace bond {

    class GcObject;

    /**
 * @brief Determines if the given pointer is an instance of a specified base type.
 *
 * This function uses the dynamic_cast operator to check if the pointer `ptr` can be safely
 * casted to a pointer of the base type `Base`. It returns true if the cast is successful and
 * false otherwise.
 *
 * @tparam Base The base type.
 * @tparam T The derived type.
 * @param ptr The pointer to check.
 * @return true if `ptr` is an instance of `Base`, false otherwise.
 */

    template<typename Base, typename T>
    inline bool instanceof(const T *ptr) {
        return dynamic_cast<const Base *>(ptr) != nullptr;
    }

    template<typename T>
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

        void reset() { m_ptr = nullptr; }

        template<typename K>
        GcPtr(const GcPtr <K> &other) : m_ptr(other.get()) {}


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
        InvalidArgument,
        AttributeNotFound,
        ExpectedStringIndex,
        FileNotFound,
    };


    class GcObject: public gc {
    public:
        GcObject() = default;

        virtual ~GcObject() = default;

        template<typename T>
        static bool is(GcPtr<GcObject> const &obj) {
            return instanceof<T>(obj.get());
        }

        template<typename T>
        bool is() { return instanceof<T>(this); }

        template<typename T>
        GcPtr<T> as() {
            return GcPtr<T>(dynamic_cast<T *>(this));
        }

        template<typename T>
        static GcPtr<T> as(GcPtr<GcObject> const &obj) {
            return GcPtr<T>(dynamic_cast<T *>(obj.get()));
        }

        template<typename ...Args>
        bool is_one_of() {
            return (is<Args>() || ...);
        }

        bool operator==(GcObject const &other) const { return this == &other; }
    };

}