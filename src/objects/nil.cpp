#include "../object.h"


namespace bond {
    std::string Nil::str() {
        return "nil";
    }

    bool Nil::equal(const GcPtr<Object> &other) {
        return is<Nil>(other);
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Nil::$eq(const GcPtr<Object> &other) {
        return GarbageCollector::instance().make<Bool>(this->equal(other));
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Nil::$ne(const GcPtr<Object> &other) {
        return GarbageCollector::instance().make<Bool>(!this->equal(other));
    }

    size_t Nil::hash() {
        //Todo: find a good way to hash as all nill
        //      values are the same
        return 7654345678900987654;
    }

    std::expected<GcPtr<Object>, RuntimeError>

    Nil::$_bool() {
        return BOOL_(false);
    }

};