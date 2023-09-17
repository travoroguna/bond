//
// Created by travor on 20/08/2023.
//

#ifndef BOND_RUNTIME_H
#define BOND_RUNTIME_H
#include "object.h"


#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#include <limits.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#endif


namespace bond {
    std::string get_exe_path();

    class Runtime {
        GcPtr<Int> int_cache[256];
        std::unordered_map<t_string, GcPtr<String>> string_cache;
        std::vector<GcPtr<Object>> m_immortals;
        std::vector<std::function<void()>> m_exit_callbacks;
        std::unordered_map<t_string, t_map> module_types;

    public:
        static Runtime* ins() {
            static Runtime runtime;
            return &runtime;
        }

        ~Runtime() {
            for (auto &immortal: m_immortals) {
                delete immortal.get();
            }
        }

        void add_exit_callback(const std::function<void()>& callback) {
            m_exit_callbacks.push_back(callback);
        }

        void register_type(const t_string& module_name, const t_string& type_name, const GcPtr<NativeStruct>& type) {
            if (module_types.contains(module_name)) assert(!module_types[module_name].contains(type_name) && "Type already registered");
            module_types[module_name][type_name] = type;
        }

        GcPtr<NativeStruct> get_type(const t_string& module_name, const t_string& type_name) {
            assert(module_types.contains(module_name) && "Module not found");
            assert(module_types[module_name].contains(type_name) && "Type not found");
            return module_types[module_name][type_name]->as<NativeStruct>();
        }

        template <typename T, typename ...Args>
        GcPtr<Object> construct(const t_string& module_name, const t_string& type_name, Args&&... args) {
            auto type = get_type(module_name, type_name);
            return type->create_instance<T>(std::forward<Args>(args)...);
        }

        void exit() {
            for (auto& callback: m_exit_callbacks) {
                callback();
            }
        }

        void init_caches() {
            for (int i = 0; i < 256; i++) {
                int_cache[i] = INT_STRUCT->create_immortal<Int>(i);
            }
        }

        void set_runtime(Runtime *runtime_ptr) {
            CLOSURE_STRUCT = runtime_ptr->CLOSURE_STRUCT;
            RESULT_STRUCT = runtime_ptr->RESULT_STRUCT;
            FLOAT_STRUCT = runtime_ptr->FLOAT_STRUCT;
            INT_STRUCT = runtime_ptr->INT_STRUCT;
            BOOL_STRUCT = runtime_ptr->BOOL_STRUCT;
            STRING_STRUCT = runtime_ptr->STRING_STRUCT;
            NONE_STRUCT = runtime_ptr->NONE_STRUCT;
            MAP_STRUCT = runtime_ptr->MAP_STRUCT;
            STRUCT_STRUCT = runtime_ptr->STRUCT_STRUCT;
            INSTANCE_STRUCT = runtime_ptr->INSTANCE_STRUCT;
            FUNCTION_STRUCT = runtime_ptr->FUNCTION_STRUCT;
            NATIVE_FUNCTION_STRUCT = runtime_ptr->NATIVE_FUNCTION_STRUCT;
            CODE_STRUCT = runtime_ptr->CODE_STRUCT;
            MODULE_STRUCT = runtime_ptr->MODULE_STRUCT;
            LIST_STRUCT = runtime_ptr->LIST_STRUCT;
            BOUND_METHOD_STRUCT = runtime_ptr->BOUND_METHOD_STRUCT;
            HASHMAP_STRUCT = runtime_ptr->HASHMAP_STRUCT;
            FUTURE_STRUCT = runtime_ptr->FUTURE_STRUCT;
            BYTES_STRUCT = runtime_ptr->BYTES_STRUCT;

            C_TRUE = runtime_ptr->C_TRUE;
            C_FALSE = runtime_ptr->C_FALSE;
            C_NONE = runtime_ptr->C_NONE;

            C_NONE_RESULT = runtime_ptr->C_NONE_RESULT;
            C_NONE_FUTURE = runtime_ptr->C_NONE_FUTURE;
            C_NONE_RESULT_FUTURE = runtime_ptr->C_NONE_RESULT_FUTURE;

            init_caches();
        }

        void init() {
            init_closure();
            init_result();
            init_float();
            init_int();
            init_bool();
            init_string();
            init_none();
            init_map();
            init_struct();
            init_instance_method();
            init_function();
            init_code();
            init_module();
            init_list();
            init_hash_map();
            init_future();
            init_bytes();
            ins()->init_caches();

            C_NONE_RESULT = make_result(C_NONE, false);
            C_NONE_RESULT_FUTURE = make_future();
            C_NONE_RESULT_FUTURE->set_value(C_NONE_RESULT);
            C_NONE_FUTURE = make_future();
            C_NONE_FUTURE->set_value(C_NONE);
        }

        GcPtr<NativeStruct> CLOSURE_STRUCT;
        GcPtr<NativeStruct> RESULT_STRUCT;
        GcPtr<NativeStruct> FLOAT_STRUCT;
        GcPtr<NativeStruct> INT_STRUCT;
        GcPtr<NativeStruct> BOOL_STRUCT;
        GcPtr<NativeStruct> STRING_STRUCT;
        GcPtr<NativeStruct> NONE_STRUCT;
        GcPtr<NativeStruct> MAP_STRUCT;
        GcPtr<NativeStruct> STRUCT_STRUCT;
        GcPtr<NativeStruct> INSTANCE_STRUCT;
        GcPtr<NativeStruct> FUNCTION_STRUCT;
        GcPtr<NativeStruct> NATIVE_FUNCTION_STRUCT;
        GcPtr<NativeStruct> CODE_STRUCT;
        GcPtr<NativeStruct> MODULE_STRUCT;
        GcPtr<NativeStruct> LIST_STRUCT;
        GcPtr<NativeStruct> BOUND_METHOD_STRUCT;
        GcPtr<NativeStruct> HASHMAP_STRUCT;
        GcPtr<NativeStruct> FUTURE_STRUCT;
        GcPtr<NativeStruct> BYTES_STRUCT;


        GcPtr<Bool> C_TRUE;
        GcPtr<Bool> C_FALSE;
        GcPtr<None> C_NONE;
        GcPtr<Result> C_NONE_RESULT;
        GcPtr<Future> C_NONE_FUTURE;
        GcPtr<Future> C_NONE_RESULT_FUTURE;

        // create functions
        [[nodiscard]] GcPtr<Closure> make_closure(const GcPtr<Function>& function, const GcPtr<StringMap>& up_values) const {
            return CLOSURE_STRUCT->create_instance<Closure>(function, up_values);
        }

        [[nodiscard]] GcPtr<Result> make_result(const GcPtr<Object>& value, bool is_error) const {
            return RESULT_STRUCT->create_instance<Result>(value, is_error);
        }

        [[nodiscard]] GcPtr<Float> make_float(double value) const {
            return FLOAT_STRUCT->create_instance<Float>(value);
        }

        [[nodiscard]] GcPtr<List> make_list(const t_vector &values) const {
            return LIST_STRUCT->create_instance<List>(values);
        }

        GcPtr<Int> make_int(int64_t value) {
            if (value > -1 and value < 256) {
                auto val = int_cache[value];
                return val;
            }

            return INT_STRUCT->create_instance<Int>(value);
        }

        [[nodiscard]] GcPtr<String> make_string(const t_string& value) const {
            if (string_cache.contains(value)) {
                return string_cache.at(value);
            }
            return STRING_STRUCT->create_instance<String>(value);
        }

        GcPtr<String> make_string_cache(const t_string& value) {
            if (string_cache.contains(value)) {
                return string_cache[value];
            }
            auto str = make_string(value);
            string_cache[value] = str;
            return str;
        }

        [[nodiscard]] GcPtr<BoundMethod> make_bound_method(const GcPtr<Instance>& self, const GcPtr<Function>& function) const {
            return BOUND_METHOD_STRUCT->create_instance<BoundMethod>(self, function);
        }

        template<typename... Args>
        inline GcPtr<Code> make_code(Args &&...args) {
            return CODE_STRUCT->create_instance<Code>(std::forward<Args>(args)...);
        }

        [[nodiscard]] GcPtr<Function> make_function(const t_string& name, const std::vector<std::shared_ptr<Param>>& params, const GcPtr<Code>& code) const {
            return FUNCTION_STRUCT->create_instance<Function>(name, params, code);
        }

        [[nodiscard]] GcPtr<NativeFunction> make_native_function(t_string name, t_string doc, const NativeFunctionPtr& function) const {
            return NATIVE_FUNCTION_STRUCT->create_instance<NativeFunction>(std::move(name), std::move(doc), function);
        }

        [[nodiscard]] GcPtr<Module> make_module(t_string path, const GcPtr<StringMap> &globals) const {
            return MODULE_STRUCT->create_instance<Module>(std::move(path), globals);
        }

        static GcPtr<NativeStruct> make_native_struct(t_string name, t_string doc, NativeFunctionPtr constructor, const method_map& methods) {
            return make<NativeStruct>(std::move(name), std::move(doc), std::move(constructor), std::move(methods));
        }

        template<typename... Args>
        [[nodiscard]] GcPtr<StringMap> make_string_map(Args &&...args) const {
            return MAP_STRUCT->create_instance<StringMap>(std::forward<Args>(args)...);
        }

        [[nodiscard]] GcPtr<HashMap> make_hash_map() const{
            return HASHMAP_STRUCT->create_instance<HashMap>();
        }


        GcPtr <Instance> make_instance(Struct *pStruct, const t_map &map) const {
            return INSTANCE_STRUCT->create_instance<Instance>(pStruct, map);
        }

        template<typename... Args>
        GcPtr<Future> make_future(Args &&...args) const {
            return FUTURE_STRUCT->create_instance<Future>(std::forward<Args>(args)...);
        }

        void add_immortal(const GcPtr<Object>& object) {
            m_immortals.push_back(object);
        }

        [[nodiscard]] GcPtr<Bytes> make_bytes(const std::vector<uint8_t>& bytes) const {
            return BYTES_STRUCT->create_instance<Bytes>(bytes);
        }

        [[nodiscard]] GcPtr<Bytes> make_bytes() const {
            return BYTES_STRUCT->create_instance<Bytes>();
        }

    };

    template<typename T, typename... Args>
    inline GcPtr<T> make_immortal(Args &&...args) {
        auto imm = GcPtr<T>(new(GC) T(std::forward<Args>(args)...));
        Runtime::ins()->add_immortal(imm);
        return imm;
    }

#define AS_BOOL(cond) (cond ? bond::Runtime::ins()->C_TRUE : bond::Runtime::ins()->C_FALSE)

}

#endif //BOND_RUNTIME_H
