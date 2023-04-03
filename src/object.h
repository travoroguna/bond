#pragma once

#include "ast.h"
#include "gc.h"
#include <optional>
#include <utility>

namespace bond {

enum class Opcode : uint32_t {

  LOAD_CONST,
  BIN_ADD,
  BIN_SUB,
  BIN_MUL,
  BIN_DIV,
  RETURN,
  PUSH_TRUE,
  PUSH_FALSE,
  PUSH_NIL,
  LOAD_GLOBAL,
  STORE_GLOBAL,
  LOAD_FAST,
  STORE_FAST,

  PRINT,

  NE,
  EQ,
  LE,
  GT,
  GE,
  LT,
  OR,
  AND,

  POP_TOP,

  CREATE_GLOBAL,
  CREATE_LOCAL,
  BUILD_LIST,
  GET_ITEM,
  SET_ITEM,
  JUMP_IF_FALSE,
  JUMP,
  CALL,

  ITER,
  ITER_NEXT,
  ITER_END,
};

class Map : public Object {
 public:
  Map() = default;

  // TODO: implement equal and hash correctly
  bool equal(const GcPtr<Object> &other) override { return false; }

  size_t hash() override { return 0; }

  std::optional<GcPtr<Object>> get(const GcPtr<Object> &key);

  GcPtr<Object> get_unchecked(const GcPtr<Object> &key);

  void set(const GcPtr<Object> &key, const GcPtr<Object> &value);

  bool has(const GcPtr<Object> &key);

  OBJ_RESULT $_bool() override;

  void mark() override;

  void unmark() override;

 private:
  std::unordered_map<GcPtr<Object>, GcPtr<Object>, Object::HashMe>
      m_internal_map;
};

class ListObj : public Object {
 public:
  ListObj() = default;

  OBJ_RESULT $set_item(const GcPtr<Object> &index,
                       const GcPtr<Object> &value) override;

  OBJ_RESULT $get_item(const GcPtr<Object> &index) override;

  GcPtr<Object> get_unchecked(size_t index) { return m_internal_list[index]; }

  size_t length() { return m_internal_list.size(); }

  OBJ_RESULT $_bool() override;

  void prepend(const GcPtr<Object> &value);

  bool equal(const GcPtr<Object> &other) override { return false; }

  OBJ_RESULT $iter(const GcPtr<Object> &self) override;

  size_t hash() override { return 0; }

  void mark() override;

  void unmark() override;

  std::string str() override;

 private:
  std::vector<GcPtr<Object>> m_internal_list;
};

class Code : public Object {

 public:
  Code() = default;

  Code(std::vector<uint32_t> code, std::vector<GcPtr<Object>> constants)
      : m_code{std::move(code)}, m_constants{std::move(constants)} {}

  [[nodiscard]] std::vector<uint32_t> get_opcodes() const { return m_code; }

  [[nodiscard]] std::vector<GcPtr<Object>> get_constants() const {
    return m_constants;
  }

  template<typename T, typename... Args>
  uint32_t add_constant(Args &&...args) {
    GarbageCollector::instance().stop_gc();
    auto tmp = GarbageCollector::instance().make<T>(args...);

    if (m_const_map.contains(tmp)) {
      GarbageCollector::instance().resume_gc();
      return m_const_map[tmp];
    }

    auto t = GarbageCollector::instance().make_immortal<T>(
        std::forward<Args>(args)...);
    m_constants.push_back(t);
    m_const_map[tmp] = m_constants.size() - 1;

    GarbageCollector::instance().resume_gc();

    return m_constants.size() - 1;
  }

  void add_code(Opcode code, const SharedSpan &span);
  void patch_code(uint32_t offset, uint32_t oprand) { m_code[offset] = oprand; }
  uint32_t current_index() { return m_code.size(); }
  void add_code(Opcode code, uint32_t oprand, const SharedSpan &span);

  GcPtr<Object> get_constant(size_t index) { return m_constants[index]; }

  uint32_t get_code(size_t index) { return m_code[index]; }

  SharedSpan get_span(size_t index) { return m_spans[index]; }

  SharedSpan last_span() { return m_spans[m_spans.size() - 1]; }

  std::string dissasemble();

  // TODO: implement equal and hash correctly
  bool equal(const GcPtr<Object> &other) override { return false; }

  size_t hash() override { return 0; }

  OBJ_RESULT $_bool() override;

 private:
  std::vector<uint32_t> m_code{};
  std::vector<GcPtr<Object>> m_constants{};
  std::unordered_map<GcPtr<Object>, uint32_t, Object::HashMe> m_const_map{};
  std::vector<SharedSpan> m_spans{};

  static size_t simple_instruction(std::stringstream &ss, const char *name,
                                   size_t offset);

  size_t constant_instruction(std::stringstream &ss, const char *name,
                              size_t offset);

  size_t oprand_instruction(std::stringstream &ss, const char *name,
                            size_t offset);
};

class Number : public Object {
 public:
  explicit Number(float value) { m_value = value; }

  [[nodiscard]] float get_value() const { return m_value; }

  OBJ_RESULT $add(const GcPtr<Object> &other) override;

  OBJ_RESULT $sub(const GcPtr<Object> &other) override;

  OBJ_RESULT $mul(const GcPtr<Object> &other) override;

  OBJ_RESULT $div(const GcPtr<Object> &other) override;

  OBJ_RESULT $eq(const GcPtr<Object> &other) override;

  OBJ_RESULT $ne(const GcPtr<Object> &other) override;

  OBJ_RESULT $lt(const GcPtr<Object> &other) override;

  OBJ_RESULT $le(const GcPtr<Object> &other) override;

  OBJ_RESULT $gt(const GcPtr<Object> &other) override;

  OBJ_RESULT $ge(const GcPtr<Object> &other) override;

  OBJ_RESULT $_bool() override;

  std::string str() override;

  bool equal(const GcPtr<Object> &other) override;

  size_t hash() override;

 private:
  float m_value{0};
};

class String : public Object {
 public:
  explicit String(std::string value) { m_value = std::move(value); }

  [[nodiscard]] std::string get_value() const { return m_value; }

  OBJ_RESULT $add(const GcPtr<Object> &other) override;

  OBJ_RESULT $eq(const GcPtr<Object> &other) override;

  OBJ_RESULT $_bool() override;

  OBJ_RESULT $ne(const GcPtr<Object> &other) override;

  std::string str() override;

  size_t hash() override;

  bool equal(const GcPtr<Object> &other) override;

 private:
  std::string m_value;
};

class Bool : public Object {
 public:
  explicit Bool(bool value) { m_value = value; }

  [[nodiscard]] bool get_value() const { return m_value; }

  OBJ_RESULT $eq(const GcPtr<Object> &other) override;

  OBJ_RESULT $ne(const GcPtr<Object> &other) override;

  std::string str() override;

  bool equal(const GcPtr<Object> &other) override;

  size_t hash() override;

  OBJ_RESULT $_bool() override;

 private:
  bool m_value;
};

class Nil : public Object {
 public:
  Nil() = default;

  std::string str() override;

  OBJ_RESULT $eq(const GcPtr<Object> &other) override;

  OBJ_RESULT $ne(const GcPtr<Object> &other) override;

  bool equal(const GcPtr<Object> &other) override;

  OBJ_RESULT $_bool() override;

  size_t hash() override;
};

class Function : public Object {
 public:
  Function() = default;

  Function(GcPtr<Code> code, std::vector<std::string> params,
           std::vector<std::string> free_vars)
      : m_code{std::move(code)}, m_params{std::move(params)},
        m_free_vars{std::move(free_vars)} {}

  [[nodiscard]] GcPtr<Code> get_code() const { return m_code; }

  [[nodiscard]] std::vector<std::string> get_params() const { return m_params; }

  [[nodiscard]] std::vector<std::string> get_free_vars() const {
    return m_free_vars;
  }

  std::string str() override {
    return fmt::format("<function at {}>", (void *) this);
  }

  bool equal(const GcPtr<Object> &other) override {
    return this == other.get();
  }

  size_t hash() override { return 0; }

  OBJ_RESULT $_bool() override {
    return GarbageCollector::instance().make_immortal<Bool>(true);
  }

 private:
  GcPtr<Code> m_code;
  std::vector<std::string> m_params;
  std::vector<std::string> m_free_vars;
};

class ListIterator : public Object {
 public:
  explicit ListIterator(const GcPtr<Object> &list) : m_list{list} {}
  OBJ_RESULT $next() override { return m_list->as<ListObj>()->get_unchecked(m_index++); }
  OBJ_RESULT $has_next() override {
    return GarbageCollector::instance().make<Bool>(m_index < m_list->as<ListObj>()->length());
  }

  bool equal(const GcPtr<Object> &other) override { return false; }
  size_t hash() override { return 0; }

  void mark() override {
    m_marked = true;
    m_list->mark();
  }
  void unmark() override {
    m_marked = false;
    m_list->unmark();
  }

 private:
  GcPtr<Object> m_list;
  size_t m_index = 0;
};

}; // namespace bond
