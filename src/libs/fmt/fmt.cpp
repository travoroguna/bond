//
// Created by Travor Oguna Oneya on 07/05/2023.
//

#include "fmt.h"
#include <fmt/core.h>
// implement
// fn print(format, ...);
// fn println(format, ...);
// fn format(format, ...);

// format specifiers
// {} - default
// {:?fill_char<count} - left pad
// {:?fill_char>count} - right pad

bond::GarbageCollector *m_gc;
bond::Context *m_ctx;


namespace bond::fmt {
#define CHECK_PARAMS(x)\
if ((x).empty())    \
    return std::unexpected(FunctionError("expected at least 1 argument", RuntimeError::GenericError)); \
if (!(x)[0]->is<String>())                                                                              \
    return std::unexpected(FunctionError("expected first argument to be a string", RuntimeError::GenericError))


    std::expected<std::string, std::string> format_str(const std::vector<GcPtr<Object>> &objects) {
        auto fmt_string = objects[0]->as<String>()->get_value();
        size_t count = 1;
        std::string result;

        size_t fmt_count = 0;
        size_t fmt_size = fmt_string.size();


        while (fmt_count < fmt_size) {
            if (fmt_string[fmt_count] == '{') {
                switch (fmt_string[fmt_count + 1]) {
                    // match closing
                    case '{':
                        fmt_count += 2;
                        break;
                    case '}':

                        if (count >= objects.size()) {
                            return std::unexpected(
                                    ::fmt::format("expected {} arguments, got {}", count + 1, objects.size()));
                        }

                        result += objects[count]->str();
                        count += 1;
                        fmt_count += 2;
                        break;
                        //match specifier
                    case ':': {
                        fmt_count += 2;
                        char fill_char = ' ';

                        // are we at the end
                        if (fmt_string[fmt_count] == '}') {
                            if (count >= objects.size()) {
                                return std::unexpected(
                                        ::fmt::format("expected {} arguments, got {}", count + 1, objects.size()));
                            }

                            result += objects[count]->str();
                            count += 1;
                            fmt_count += 1;
                            break;
                        }

                        // check for fill char
                        auto c = fmt_string[fmt_count];
                        if (c != '<' and c != '>') {
                            fill_char = c;
                            fmt_count += 1;
                        }

                        // assert next char is pad specifier
                        if (fmt_string[fmt_count] != '<' and fmt_string[fmt_count] != '>') {
                            auto err = ::fmt::format("expected pad specifier, got {}", fmt_string[fmt_count]);
                            return std::unexpected(err);
                        }

                        // which pad specifier
                        auto specifier = fmt_string[fmt_count++];

                        // parse_digits end will be a '{'
                        std::string number;

                        while (fmt_string[fmt_count] != '}') {
                            // parse digits
                            auto c_ = fmt_string[fmt_count++];
                            if (c_ < '0' or c_ > '9') {
                                auto err = ::fmt::format("expected digit, got {}", c);
                                return std::unexpected(err);
                            }

                            number += c_;
                        }


                        auto num = number.empty() ? 0 : std::stoi(number);

                        if (count >= objects.size()) {
                            return std::unexpected(
                                    ::fmt::format("expected {} arguments, got {}", count + 1, objects.size()));
                        }

                        // pad

                        auto str = objects[count]->str();

                        if (num > str.size()) {
                            num -= str.size();
                        }

                        switch (specifier) {
                            case '<':
                                for (int i = 0; i < num; i++) {
                                    result += fill_char;
                                }
                                result += objects[count]->str();
                                break;
                            case '>':
                                result += objects[count]->str();
                                for (int i = 0; i < num; i++) {
                                    result += fill_char;
                                }
                                break;
                            default:
                                break;
                        }

                        // increment count for '}'
                        fmt_count += 1;
                        count += 1;
                        break;
                    }

                    default:
                        auto err = ::fmt::format("expected '}}' or ':', got {}", fmt_string[fmt_count + 1]);
                        return std::unexpected(err);
                }
            } else if (fmt_string[fmt_count] == '}') {
                if (fmt_string[fmt_count + 1] == '}') {
                    result += '}';
                    fmt_count += 2;
                } else {
                    auto err = ::fmt::format("unexpected '}}' at position {}", fmt_count);
                    return std::unexpected(err);
                }
            } else {
                result += fmt_string[fmt_count];
                fmt_count += 1;
            }
        }

        if (count != objects.size()) {
            auto err = ::fmt::format("got {} extra arguments", objects.size() - count);
            return std::unexpected(err);
        }

        return result;
    }

    NativeErrorOr print(const std::vector<GcPtr<Object>> &objects) {
        CHECK_PARAMS(objects);
        if (objects.size() == 1) {
            ::fmt::print("{}", objects[0]->as<String>()->get_value());
        }

        auto result = format_str(objects);

        if (result) {
            ::fmt::print("{}", result.value());
            return Globs::BondNil;
        } else {
            return std::unexpected(FunctionError(result.error(), RuntimeError::GenericError));
        }
    }

    NativeErrorOr println(const std::vector<GcPtr<Object>> &objects) {
        CHECK_PARAMS(objects);

        auto result = format_str(objects);
        if (result) {
            ::fmt::print("{}\n", result.value());
            return Globs::BondNil;
        } else {
            return std::unexpected(FunctionError(result.error(), RuntimeError::GenericError));
        }

    }

    NativeErrorOr format(const std::vector<GcPtr<Object>> &objects) {
        CHECK_PARAMS(objects);
        auto result = format_str(objects);
        if (result) {
            return Ok(m_gc->make<String>(result.value()));
        } else {
            return Err(result.error());
        }
    }

} // bond


EXPORT void bond_module_init(bond::Context *ctx, std::string const &path) {
    m_gc = ctx->gc();
    m_ctx = ctx;
    bond::GarbageCollector::instance().set_gc(ctx->gc());


    std::unordered_map<std::string, bond::GcPtr<bond::Object>> mod = {
            {"print",   m_gc->make<bond::NativeFunction>(bond::fmt::print)},
            {"println", m_gc->make<bond::NativeFunction>(bond::fmt::println)},
            {"format",  m_gc->make<bond::NativeFunction>(bond::fmt::format)},
    };

    auto module = ctx->gc()->make<bond::Module>(path, mod);
    ctx->add_module(path, module);
}
