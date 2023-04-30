#include "object.h"

namespace bond {

    void BoundMethod::mark() {
        Object::mark();
        m_receiver.mark();
        m_method.mark();
    }

    void BoundMethod::unmark() {
        Object::unmark();
        m_receiver.unmark();
        m_method.unmark();
    }

    std::expected<GcPtr<Object>, RuntimeError> ListIterator::$has_next() {
        return BOOL_(m_index < m_list->as<ListObj>()->length());
    }

    Coroutine::Coroutine(const GcPtr<Function> &function) {
        m_function = function;
    }
}