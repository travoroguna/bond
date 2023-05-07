//
// Created by Travor Oguna Oneya on 24/04/2023.
//

#include "../object.h"


namespace bond {
    void Future::mark() {
        if (m_marked) return;
        Object::mark();

        if (m_value.get() != nullptr) m_value.mark();
    }

    void Future::unmark() {
        if (!m_marked) return;
        Object::unmark();

        if (m_value.get() != nullptr) m_value.unmark();
    }

};
