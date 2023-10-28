//
// Created by travor on 3/18/23.
//

#pragma once

#include <array>
#include <expected>
#include <fmt/core.h>
#include <type_traits>
#include <utility>
#include <vector>

#include "builtins.h"
#include "compiler/codegen.h"
#include "compiler/context.h"
#include "object.h"
#include "runtime.h"
#include <cassert>

#define STORE_OPCODES

#ifdef STORE_OPCODES
#include <chrono>
#endif

namespace bond {
#define TO_BOOL(X)                                                             \
  Runtime::ins()->BOOL_STRUCT->create({(X)}).value()->as<Bool>()

class Frame {
public:
  Frame() = default;

  explicit Frame(const GcPtr<Code> &code) { m_code = code; }

  GcPtr<Object> get_constant() {
    return m_code->get_constant(m_code->get_code(m_ip++));
  }

  Opcode get_opcode() { return static_cast<Opcode>(m_code->get_code(m_ip++)); }

  uint32_t get_oprand() { return m_code->get_code(m_ip++); }

  SharedSpan get_span() const {
    if (m_ip == 0) {
      return m_code->get_span(0);
    }

    if (m_ip >= m_code->get_code_size()) {
      return m_code->get_span(m_code->get_code_size() - 1);
    }

    return m_code->get_span(m_ip - 1);
  }

  void set_code(const GcPtr<Code> &code) {
    m_code = code;
    m_ip = 0;
  }

  void set_function(const GcPtr<Function> &function) {
    m_function = function;
    set_code(function->get_code());
  }

  GcPtr<Function> get_function() { return m_function; }

  void set_globals(const GcPtr<StringMap> &globals) { m_globals = globals; }

  void jump_absolute(size_t ip) { m_ip = ip; }

  void set_locals(const GcPtr<StringMap> &locals) { m_locals = locals; }

  void set_global(const t_string &key, const GcPtr<Object> &value) {
    m_globals->set(key, value);
  }

  bool has_global(const t_string &key) { return m_globals->has(key); }

  GcPtr<Object> get_global(const t_string &key) {
    return m_globals->get_unchecked(key);
  }

  void set_local(const t_string &key, const GcPtr<Object> &value) {
    m_locals->set(key, value);
  }

  bool has_local(const t_string &key) { return m_locals->has(key); }

  GcPtr<Object> get_local(const t_string &key) {
    return m_locals->get_unchecked(key);
  }

  GcPtr<StringMap> get_locals() { return m_locals; }

  GcPtr<StringMap> get_globals() { return m_globals; }

  size_t get_ip() const { return m_ip; }

  bool is_at_end() { return m_ip >= m_code->get_code_size(); }

  void clear() {
    m_locals.reset();
    m_globals.reset();
    m_function.reset();
    m_code.reset();
    m_ip = 0;
  }

  t_string &local_name(size_t index) {
    return m_code->get_identifier((uint32_t) index);
  }

private:
  GcPtr<Function> m_function;
  GcPtr<Code> m_code;
  size_t m_ip = 0;
  GcPtr<StringMap> m_locals;
  GcPtr<StringMap> m_globals;
};

struct AsyncFrame : public gc {
  Frame frame;
  GcPtr<Future> future;

  AsyncFrame(Frame frame, const GcPtr<Future> &future)
      : frame(std::move(frame)), future(future) {}
};

#define FRAME_MAX 512

using VectorArgs = std::vector<std::shared_ptr<Param>>;

class Vm {
public:
  Runtime *runtime() { return m_runtime; }

  explicit Vm(Context *ctx) {
    m_ctx = ctx;
    m_True = Runtime::ins()->C_TRUE;
    m_False = Runtime::ins()->C_FALSE;
    m_Nil = Runtime::ins()->C_NONE;
    m_globals = Runtime::ins()->make_string_map();
    assert(m_globals.get() != nullptr);
    add_builtins_to_globals(m_globals);
    m_runtime = Runtime::ins();
  }

  Vm(Context *ctx, const GcPtr<StringMap> &globals) {
    m_ctx = ctx;
    m_True = Runtime::ins()->C_TRUE;
    m_False = Runtime::ins()->C_FALSE;
    m_Nil = Runtime::ins()->C_NONE;
    m_globals = globals;
    add_builtins_to_globals(m_globals);
    m_runtime = Runtime::ins();
  }

  void run(const GcPtr<Code> &code);

  bool has_top() const { return m_stack_pointer > 0; }

  bool had_error() const { return m_has_error; }

  void reset_error() { m_has_error = false; }

  GcPtr<StringMap> get_globals() { return m_globals; }

  void set_globals(const GcPtr<StringMap> &globals) { m_globals = globals; }

  void call_function(const GcPtr<Function> &function, const t_vector &args,
                     const GcPtr<StringMap> &locals = nullptr);

  void exec(uint32_t stop_frame = 0);

  void runtime_error(const t_string &error, RuntimeError e,
                     const SharedSpan &span);

  void runtime_error(const t_string &error, RuntimeError e);

  template<typename... T>
  [[nodiscard]] GcPtr<Object>
  call_slot(Slot slot, const GcPtr<Object> &instance, const t_vector &args,
            fmt::format_string<T...> fmt, T &&...fmt_args);

  std::expected<GcPtr<Object>, t_string>
  call_slot(Slot slot, const GcPtr<Object> &instance, const t_vector &args);

  inline GcPtr<Object> pop() {
    auto obj = stack[m_stack_pointer].get();
    stack[m_stack_pointer--].reset();
    return obj;
  }

  inline void push(GcPtr<Object> const &obj) { stack[++m_stack_pointer] = obj; }

  inline GcPtr<Object> peek() { return stack[m_stack_pointer]; }

  inline GcPtr<Object> peek(size_t rel_index) {
    return stack[(size_t) m_stack_pointer - rel_index];
  }

  //        t_vector m_stack;
  bool call_object_ex(const GcPtr<Object> &obj, t_vector &args);

  void runtime_error(const t_string &error);

  void set_start_event_loop_cb(std::function<void()> cb) {
    m_start_event_loop_cb = cb;
  }

  GcPtr<Object> call_function_ex(const GcPtr<Object> &function,
                                 const t_vector &args);

  std::optional<GcPtr<Object>> get_repl_result() { return repl_result; }

  void init_bin_funcs();

#ifdef STORE_OPCODES
  std::vector<std::pair<Opcode, std::chrono::microseconds>> m_opcodes{};
#endif

private:
  Runtime *m_runtime = nullptr;
  GcPtr<Object> stack[1024];
  int m_stack_pointer = -1;

  std::optional<GcPtr<Object>> repl_result{};
  GcPtr<Bool> m_True;
  GcPtr<Bool> m_False;
  GcPtr<None> m_Nil;
  Context *m_ctx = nullptr;
  bool m_stop = false;
  bool m_has_error = false;

  size_t m_frame_pointer = 0;
  std::array<Frame, FRAME_MAX> m_frames;
  Frame *m_current_frame = nullptr;
  t_vector m_args{};

  GcPtr<StringMap> m_globals;

  void create_instance(const GcPtr<Struct> &_struct, const t_vector &args);

  void call_bound_method(const GcPtr<BoundMethod> &bound_method,
                         t_vector &args);

  void bin_op(Slot slot, const t_string &op_name);

  void setup_bound_call(const GcPtr<Object> &instance,
                        const GcPtr<Function> &function, t_vector &args);

  void compare_op(Slot slot, const t_string &op_name);

  void call_native_function(const GcPtr<Object> &func, t_vector &args);

  void call_script_function(const GcPtr<Object> &func, t_vector &args);

  void call_struct(const GcPtr<Object> &func, t_vector &args);

  void call_bound_method(const GcPtr<Object> &func, t_vector &args);

  void call_native_struct(const GcPtr<Object> &func, t_vector &args);

  void call_closure(const GcPtr<Object> &func, t_vector &args);

  void update_frame_pointer();

  void check_argument_count(const t_vector &args, const VectorArgs &params);

  static void set_local_arguments(Frame *frame, const t_vector &args,
                                  const VectorArgs &params,
                                  const GcPtr<StringMap> &locals);

  void call_object(const GcPtr<Object> &func, t_vector &args);

  void bin_alt(const NativeMethodPtr &meth, const char *op_name);

  NativeMethodPtr i_add{};
  NativeMethodPtr i_sub{};
  NativeMethodPtr i_mul{};
  NativeMethodPtr i_div{};

  NativeMethodPtr f_add{};
  NativeMethodPtr f_sub{};
  NativeMethodPtr f_mul{};
  NativeMethodPtr f_div{};

  t_vector alt = {nullptr};

  std::mutex m_func_ex_lock;
  size_t m_stop_frame = 0;
  std::atomic_bool m_aq = false;

  std::vector<AsyncFrame, gc_allocator<AsyncFrame>> m_yield_frames{};
  std::function<void()> m_start_event_loop_cb;

  void process_events_if_needed();

  void bit_or();

  void bit_and();

  void bit_xor();
};

}; // namespace bond
//
