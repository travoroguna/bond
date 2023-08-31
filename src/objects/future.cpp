//
// Created by Travor Oguna Oneya on 24/04/2023.
//

#include "../object.h"
#include "../runtime.h"
#include "../api.h"


namespace bond {
    obj_result is_ready(const GcPtr<Object>&Self, const t_vector& args) {
        TRY(parse_args(args));
        auto self = Self->as<Future>();
        return OK(AS_BOOL(self->is_ready()));
    }

    obj_result get_res_value(const GcPtr<Object>&Self, const t_vector& args) {
        TRY(parse_args(args));
        auto self = Self->as<Future>();
        return self->get_value();
    }

    obj_result then(const GcPtr<Object>&Self, const t_vector& args) {
        Function* func;
        TRY(parse_args(args, func));
        auto self = Self->as<Future>();
        self->set_then(func);

        return OK();
    }

    void Future::set_value(const GcPtr<Object> &value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_value = value;
        m_ready = true;
        m_cv.notify_one();

        if (m_then.has_value()) {
            auto func = m_then.value();
            auto res = get_current_vm()->call_function_ex(func, {value});

            if (res->is<Result>()) {
                auto result = res->as<Result>();
                if (result->has_error()) {
                    get_current_vm()->runtime_error(result->str());
                }
            }
        }
    }


    obj_result f_constructor(const t_vector& args) {
        Object *obj;
        TRY(parse_args(args, obj));
        auto f = Runtime::ins()->make_future();
        f->set_value(obj);
        return f;
    }

    void init_future() {
        auto methods = method_map {
                {"get_value", {get_res_value, "get_value() -> Object\nreturns the value of the future"}},
                {"is_ready", {is_ready, "is_ready() -> bool\nreturns true if the future is ready"}},
                {"then", {then, "then(func: Function)\nsets the function to be called when the future is ready"}}
        };

        Runtime::ins()->FUTURE_STRUCT = make_immortal<NativeStruct>("Future", "Future()", f_constructor, methods);
    }

}
