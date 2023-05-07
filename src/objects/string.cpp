#include "../object.h"


namespace bond {
    class StringIterator : public Object {
    public:
        explicit StringIterator(const GcPtr<String> &string) : m_string(string) {}

        std::expected<GcPtr<Object>, RuntimeError> $next() override {
            return GarbageCollector::instance().make<String>(std::string(m_string->get_value()[m_index++], 1));
        }

        std::expected<GcPtr<Object>, RuntimeError> $has_next() override {
            return BOOL_(m_index < m_string->get_value().size());
        }

        void mark() override {
            Object::mark();
            m_string.mark();
        }

        void unmark() override {
            Object::unmark();
            m_string.unmark();
        }

        bool equal(const GcPtr<bond::Object> &other) override {
            if (!is<StringIterator>(other)) return false;
            return m_string == as<StringIterator>(other)->m_string && m_index == as<StringIterator>(other)->m_index;
        }

        size_t hash() override {
            return std::hash<size_t>{}(m_index) ^ m_string->hash();
        }

    private:
        GcPtr<String> m_string;
        size_t m_index = 0;
    };


    OBJ_RESULT String::$add(const GcPtr<Object> &other) {
        if (!is<String>(other)) return std::unexpected(RuntimeError::TypeError);
        return GarbageCollector::instance().make<String>(m_value + as<String>(other)->get_value());
    }

    bool String::equal(const GcPtr<Object> &other) {
        if (!is<String>(other)) return false;
        return m_value == as<String>(other)->get_value();
    }

    std::expected<GcPtr<Object>, RuntimeError>

    String::$eq(const GcPtr<Object> &other) {
        return BOOL_(this->equal(other));
    }

    std::expected<GcPtr<Object>, RuntimeError>

    String::$ne(const GcPtr<Object> &other) {
        return BOOL_(!this->equal(other));
    }

    size_t String::hash() {
        return std::hash<std::string>{}(m_value);
    }

    std::string String::str() {
        return fmt::format("\"{}\"", m_value);
    }

    std::expected<GcPtr<Object>, RuntimeError>

    String::$_bool() {
        return BOOL_(!m_value.empty());
    }

    NativeErrorOr String::join(const std::vector<GcPtr<Object>> &args) {
        ASSERT_ARG_COUNT(1, args);

        auto iter = args[0]->$iter();
        if (!iter.has_value()) {
            return std::unexpected(
                    FunctionError(fmt::format("Object {} is not iterable", args[0]->str()), RuntimeError::TypeError));
        }

        auto it = iter.value();

        auto test_next = it->$has_next();
        if (!test_next.has_value()) {
            return std::unexpected(
                    FunctionError(fmt::format("Object {} is not iterable, has_next is not defined", args[0]->str()),
                                  RuntimeError::TypeError));
        }

        std::string result;

        while (true) {
            auto has_v = it->$has_next().value();
            auto cond = has_v->$_bool();

            if (!cond.has_value()) {
                return std::unexpected(FunctionError(fmt::format("Unable to convert {} to bool", args[0]->str()),
                                                     RuntimeError::TypeError));
            }

            if (!cond.value()->as<Bool>()->get_value()) {
                break;
            }

            auto next = it->$next();

            if (!next.has_value()) {
                return std::unexpected(
                        FunctionError(fmt::format("Object {} is not iterable, next is not defined", args[0]->str()),
                                      RuntimeError::TypeError));
            }

            if (!is<String>(next.value())) {
                return std::unexpected(FunctionError(fmt::format("Object {} is not a string", args[0]->str()),
                                                     RuntimeError::TypeError));
            }

            result += next.value()->as<String>()->get_value() + m_value;
        }

        return GarbageCollector::instance().make<String>(result.substr(0, result.size() - m_value.size()));
    }

    NativeErrorOr String::split(const std::vector<GcPtr<Object>> &args) {
        ASSERT_ARG_COUNT(1, args);
        DEFINE(sep, String, 0, args);

        auto result = GarbageCollector::instance().make<ListObj>();
        size_t end = m_value.find(sep->get_value());
        auto copy_str = m_value;

        while (end != std::string::npos) {
            result->append(GarbageCollector::instance().make<String>(copy_str.substr(0, end)));
            copy_str.erase(copy_str.begin(), copy_str.begin() + end + sep->get_value().size());
            end = copy_str.find(sep->get_value());
        }

        result->append(GarbageCollector::instance().make<String>(copy_str.substr(0, end)));
        return result;
    }

    NativeErrorOr String::find(const std::vector<GcPtr<Object>> &args) {
        ASSERT_ARG_COUNT(1, args);
        DEFINE(sub, String, 0, args);

        auto pos = m_value.find(sub->get_value());

        if (pos == std::string::npos) {
            return Globs::BondNil;
        }

        return GarbageCollector::instance().make<Integer>(pos);
    }


    NativeErrorOr String::replace(const std::vector<GcPtr<Object>> &args) {
        ASSERT_ARG_COUNT(2, args);
        DEFINE(sub, String, 0, args);
        DEFINE(replace, String, 1, args);

        std::string result;

        size_t pos = 0;

        while ((pos = m_value.find(sub->get_value(), pos)) != std::string::npos) {
            result += m_value.substr(0, pos) + replace->get_value();
            pos += sub->get_value().length();
        }

        result += m_value.substr(pos);

        return GarbageCollector::instance().make<String>(result);
    }

    NativeErrorOr String::starts_with(const std::vector<GcPtr<Object>> &args) {
        ASSERT_ARG_COUNT(1, args);
        DEFINE(sub, String, 0, args);

        return BOOL_(m_value.find(sub->get_value()) == 0);
    }

    NativeErrorOr String::ends_with(const std::vector<GcPtr<Object>> &args) {
        ASSERT_ARG_COUNT(1, args);
        DEFINE(sub, String, 0, args);

        return BOOL_(m_value.find(sub->get_value()) == m_value.size() - sub->get_value().size());
    }

    NativeErrorOr String::to_upper(const std::vector<GcPtr<Object>> &args) {
        ASSERT_ARG_COUNT(0, args);

        auto str = m_value;
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });

        return GarbageCollector::instance().make<String>(str);
    }

    NativeErrorOr String::to_lower(const std::vector<GcPtr<Object>> &args) {
        ASSERT_ARG_COUNT(0, args);

        auto str = m_value;
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });

        return GarbageCollector::instance().make<String>(str);
    }

    NativeErrorOr String::strip(const std::vector<GcPtr<Object>> &args) {
        ASSERT_ARG_COUNT(0, args);

        auto str = m_value;
        str.erase(0, str.find_first_not_of(' '));
        str.erase(str.find_last_not_of(' ') + 1);

        return GarbageCollector::instance().make<String>(str);
    }

    std::expected<GcPtr<Object>, RuntimeError> String::$get_attribute(const GcPtr<bond::Object> &index) {
        if (is<String>(index)) {
            auto key = index->as<String>()->get_value();

            if (m_attributes.contains(key)) {
                return MAKE_FORM_BIND(m_attributes[key]);
            }
        }

        return std::unexpected(RuntimeError::AttributeNotFound);
    }

    std::expected<GcPtr<Object>, RuntimeError> String::$iter() {
        return GarbageCollector::instance().make<StringIterator>(this);
    }

};