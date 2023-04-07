//
// Created by travor on 3/18/23.
//

#pragma once

#include <utility>
#include <vector>
#include <expected>
#include <type_traits>
#include <fmt/core.h>
#include <array>

#include "code.h"
#include "object.h"
#include "context.h"
#include "builtins.h"

namespace bond {

class Frame {
 public:
  Frame() = default;
  explicit Frame(const GcPtr<Code> &code) { m_code = code; }
  GcPtr<Object> get_constant() { return m_code->get_constant(m_code->get_code(m_ip++)); }
  Opcode get_opcode() { return static_cast<Opcode>(m_code->get_code(m_ip++)); }

  uint32_t get_oprand() { return m_code->get_code(m_ip++); }

  SharedSpan get_span() { return m_code->get_span(m_ip); }

  void set_code(const GcPtr<Code> &code) {
    m_code = code;
    m_ip = 0;
  }

  void set_function(const GcPtr<Function> &function) {
    m_function = function;
    set_code(function->get_code());
  }

  GcPtr<Function> get_function() { return m_function; }

  void set_globals(const GcPtr<Map> &globals) { m_globals = globals; }
  void jump_absolute(size_t ip) { m_ip = ip; }
  void set_locals(const GcPtr<Map> &locals) { m_locals = locals; }

  void set_global(const GcPtr<Object> &key, const GcPtr<Object> &value) { m_globals->set(key, value); }

  bool has_global(const GcPtr<Object> &key) { return m_globals->has(key); }

  GcPtr<Object> get_global(const GcPtr<Object> &key) { return m_globals->get_unchecked(key); }

  void set_local(const GcPtr<Object> &key, const GcPtr<Object> &value) { m_locals->set(key, value); }

  bool has_local(const GcPtr<Object> &key) { return m_locals->has(key); }

  GcPtr<Object> get_local(const GcPtr<Object> &key) { return m_locals->get_unchecked(key); }

  void mark() {
    m_code->mark();
    m_globals->mark();
    m_locals->mark();
  }

  void unmark() {
    m_code->unmark();
    m_globals->unmark();
    m_locals->unmark();
  }

 private:
  GcPtr<Function> m_function;
  GcPtr<Code> m_code;
  size_t m_ip = 0;
  GcPtr<Map> m_locals;
  GcPtr<Map> m_globals;
};

#define FRAME_MAX 1024

class Vm : public Root {
 public:
  explicit Vm(Context *ctx) {
    m_ctx = ctx;
    m_True = GarbageCollector::instance().make_immortal<Bool>(true);
    m_False = GarbageCollector::instance().make_immortal<Bool>(false);
    m_Nil = GarbageCollector::instance().make_immortal<Nil>();
    m_globals = GarbageCollector::instance().make_immortal<Map>();
    add_builtins_to_globals(m_globals);
  }
  void run(const GcPtr<Code> &code);

 private:
  GcPtr<Bool> m_True;
  GcPtr<Bool> m_False;
  GcPtr<Nil> m_Nil;
  Context *m_ctx = nullptr;
  bool m_stop = false;

  size_t m_frame_pointer = 0;
  std::array<Frame, FRAME_MAX> m_frames;
  Frame *m_current_frame = nullptr;

  void mark() override;

  void unmark() override;

  void exec();

  void runtime_error(const std::string &error, RuntimeError e, const SharedSpan &span);

  GcPtr<Map> m_globals;
  void print_stack();
  void call_function(const GcPtr<Function> &function, const std::vector<GcPtr<Object>> &args);
};

} // bond

