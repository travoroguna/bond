#include "object.h"
#include <functional>
#include <cmath>

namespace bond {
OBJ_RESULT Number::$add(const GcPtr<Object> &other) {
  if (!other->is<Number>()) {
    return std::unexpected(RuntimeError::TypeError);
  }

  return GarbageCollector::instance().make<Number>(m_value + other->as<Number>()->get_value());
}

OBJ_RESULT Number::$sub(const GcPtr<Object> &other) {
  if (!other->is<Number>()) {
    return std::unexpected(RuntimeError::TypeError);
  }

  return GarbageCollector::instance().make<Number>(m_value - other->as<Number>()->get_value());
}

OBJ_RESULT Number::$mul(const GcPtr<Object> &other) {
  if (!other->is<Number>()) {
    return std::unexpected(RuntimeError::TypeError);
  }

  return GarbageCollector::instance().make<Number>(m_value * other->as<Number>()->get_value());
}

OBJ_RESULT Number::$div(const GcPtr<Object> &other) {
  auto res = const_cast<GcPtr<Object> &>(other)
      .use_if<Number, std::optional<GcPtr<Number>>>(
          [&](Number &other_number) -> std::optional<GcPtr<Number>> {
            if (other_number.get_value() == 0) {
              return std::nullopt;
            }
            return GarbageCollector::instance().make<Number>(m_value / other_number.get_value());

          });

  if (res.has_value()) {
    if (res.value().has_value()) {
      return res.value().value();
    }
    return std::unexpected(RuntimeError::DivisionByZero);
  }
  return std::unexpected(RuntimeError::TypeError);
}

std::string Number::str() {
  return std::to_string(m_value);
}

bool Number::equal(const GcPtr<Object> &other) {
  if (!is<Number>(other)) return false;
  return m_value == as<Number>(other)->get_value();
}

size_t Number::hash() {
  return std::hash<float>{}(m_value);
}

std::expected<GcPtr<Object>, RuntimeError> Number::$eq(const GcPtr<Object> &other) {
  return GarbageCollector::instance().make<Bool>(this->equal(other));
}

std::expected<GcPtr<Object>, RuntimeError> Number::$ne(const GcPtr<Object> &other) {
  return GarbageCollector::instance().make<Bool>(!this->equal(other));
}

std::expected<GcPtr<Object>, RuntimeError> Number::$lt(const GcPtr<Object> &other) {
  if (!is<Number>(other)) return std::unexpected(RuntimeError::TypeError);
  return GarbageCollector::instance().make<Bool>(m_value < as<Number>(other)->get_value());
}

std::expected<GcPtr<Object>, RuntimeError> Number::$le(const GcPtr<Object> &other) {
  if (!is<Number>(other)) return std::unexpected(RuntimeError::TypeError);
  return GarbageCollector::instance().make<Bool>(m_value <= as<Number>(other)->get_value());
}

std::expected<GcPtr<Object>, RuntimeError> Number::$gt(const GcPtr<Object> &other) {
  if (!is<Number>(other)) return std::unexpected(RuntimeError::TypeError);
  return GarbageCollector::instance().make<Bool>(m_value > as<Number>(other)->get_value());
}

std::expected<GcPtr<Object>, RuntimeError> Number::$ge(const GcPtr<Object> &other) {
  if (!is<Number>(other)) return std::unexpected(RuntimeError::TypeError);
  return GarbageCollector::instance().make<Bool>(m_value >= as<Number>(other)->get_value());
}

std::expected<GcPtr<Object>, RuntimeError> Number::$_bool() {
  return GarbageCollector::instance().make<Bool>(m_value != 0);
}

OBJ_RESULT String::$add(const GcPtr<Object> &other) {
  auto res = const_cast<GcPtr<Object> &>(other)
      .use_if<String, GcPtr<String>>([&](String &other_string) {
        return GarbageCollector::instance().make<String>(m_value + other_string.get_value());
      });

  if (res.has_value()) {
    return res.value();
  }
  return std::unexpected(RuntimeError::TypeError);
}

bool String::equal(const GcPtr<Object> &other) {
  if (!is<String>(other)) return false;
  return m_value == as<String>(other)->get_value();
}

std::expected<GcPtr<Object>, RuntimeError> String::$eq(const GcPtr<Object> &other) {
  return GarbageCollector::instance().make<Bool>(this->equal(other));
}

std::expected<GcPtr<Object>, RuntimeError> String::$ne(const GcPtr<Object> &other) {
  return GarbageCollector::instance().make<Bool>(!this->equal(other));
}

size_t String::hash() {
  return std::hash<std::string>{}(m_value);
}

std::string String::str() {
  return fmt::format("\"{}\"", m_value);
}

std::expected<GcPtr<Object>, RuntimeError> String::$_bool() {
  return GarbageCollector::instance().make<Bool>(!m_value.empty());
}

std::string Bool::str() {
  return m_value ? "true" : "false";
}

bool Bool::equal(const GcPtr<Object> &other) {
  if (!is<Bool>(other)) return false;
  return m_value == as<Bool>(other)->get_value();
}

std::expected<GcPtr<Object>, RuntimeError> Bool::$eq(const GcPtr<Object> &other) {
  return GarbageCollector::instance().make<Bool>(this->equal(other));
}

std::expected<GcPtr<Object>, RuntimeError> Bool::$ne(const GcPtr<Object> &other) {
  return GarbageCollector::instance().make<Bool>(!this->equal(other));
}

size_t Bool::hash() {
  return std::hash<bool>{}(m_value);
}

std::expected<GcPtr<Object>, RuntimeError> Bool::$_bool() {
  return GarbageCollector::instance().make<Bool>(m_value);
}

std::string Nil::str() {
  return "nil";
}

bool Nil::equal(const GcPtr<Object> &other) {
  return is<Nil>(other);
}

std::expected<GcPtr<Object>, RuntimeError> Nil::$eq(const GcPtr<Object> &other) {
  return GarbageCollector::instance().make<Bool>(this->equal(other));
}

std::expected<GcPtr<Object>, RuntimeError> Nil::$ne(const GcPtr<Object> &other) {
  return GarbageCollector::instance().make<Bool>(!this->equal(other));
}

size_t Nil::hash() {
  //Todo: find a good way to hash as all nill
  //      values are the same
  return 7654345678900987654;
}

std::expected<GcPtr<Object>, RuntimeError> Nil::$_bool() {
  return GarbageCollector::instance().make<Bool>(false);
}

void Map::set(const GcPtr<Object> &key, const GcPtr<Object> &value) {
  m_internal_map[key] = value;
}

bool Map::has(const GcPtr<Object> &key) {
  return m_internal_map.contains(key);
}

std::optional<GcPtr<Object>> Map::get(const GcPtr<Object> &key) {
  if (has(key)) {
    return m_internal_map[key];
  }

  return std::nullopt;
}

GcPtr<Object> Map::get_unchecked(const GcPtr<Object> &key) {
  return m_internal_map[key];
}

void Map::mark() {
  Object::mark();

  for (auto &pair : m_internal_map) {
    pair.first->mark();
    pair.second->mark();
  }
}

void Map::unmark() {
  Object::unmark();

  for (auto &pair : m_internal_map) {
    pair.first->unmark();
    pair.second->unmark();
  }
}

std::expected<GcPtr<Object>, RuntimeError> Map::$_bool() {
  return GarbageCollector::instance().make<Bool>(!m_internal_map.empty());
}

std::expected<GcPtr<Object>, RuntimeError>
ListObj::$set_item(const GcPtr<Object> &index, const GcPtr<Object> &value) {
  if (!is<Number>(index)) return std::unexpected(RuntimeError::ExpectedNumberIndex);
  auto index_value = as<Number>(index)->get_value();

  if (std::floor(index_value) != index_value) {
    return std::unexpected(RuntimeError::ExpectedWholeNumberIndex);
  }

  auto i = static_cast<size_t>(index_value);

  if (i < 0 || i >= m_internal_list.size()) {
    return std::unexpected(RuntimeError::IndexOutOfBounds);
  }

  m_internal_list[i] = value;
  return value;
}

std::expected<GcPtr<Object>, RuntimeError>
ListObj::$get_item(const GcPtr<Object> &index) {
  if (!is<Number>(index)) return std::unexpected(RuntimeError::ExpectedNumberIndex);
  auto index_value = as<Number>(index)->get_value();

  if (std::floor(index_value) != index_value) {
    return std::unexpected(RuntimeError::ExpectedWholeNumberIndex);
  }

  auto i = static_cast<size_t>(index_value);

  if (i < 0 || i >= m_internal_list.size()) {
    return std::unexpected(RuntimeError::IndexOutOfBounds);
  }

  return m_internal_list[i];
}

void ListObj::mark() {
  Object::mark();

  for (auto &item : m_internal_list) {
    item->mark();
  }
}

void ListObj::unmark() {
  Object::unmark();

  for (auto &item : m_internal_list) {
    item->unmark();
  }
}

void ListObj::prepend(const GcPtr<Object> &value) {
  m_internal_list.insert(m_internal_list.begin(), value);
}

std::string ListObj::str() {
  std::string result = "[";

  for (auto &item : m_internal_list) {
    result += item->str() + ", ";
  }

  if (result.size() > 1) {
    result.pop_back();
    result.pop_back();
  }

  result += "]";

  return result;
}

std::expected<GcPtr<Object>, RuntimeError> ListObj::$_bool() {
  return GarbageCollector::instance().make<Bool>(!m_internal_list.empty());
}

std::expected<GcPtr<Object>, RuntimeError> ListObj::$iter(const GcPtr<Object> &self) {
  return GarbageCollector::instance().make<ListIterator>(self);
}

}