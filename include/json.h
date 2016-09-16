#pragma once

#include "variant.h"
#include "format_utils.h"

#include <map>
#include <vector>
#include <string>
#include <iostream>


namespace qnd
{
    namespace json
    {
        struct Value;
        struct Null { constexpr Null() { } };
        constexpr Null null;
        using Num = double;
        using String = std::string;
        using Object = std::map<String, Value>;
        using Array = std::vector<Value>;

        struct Value : variant<Null, String, Num, Object, Array>
        {
            Value()
                : variant(Null{})
            { }

            Value(const char* s)
                : variant(String{s})
            { }

            Value(int v)
                : variant(double(v))
            { }

            using variant::variant;
        };

        namespace detail
        {
            std::ostream& print_value(std::ostream& o, Value const& val, indent_t);

            std::ostream& print_impl(std::ostream& o, Num n, indent_t)
            {
                return o << n;
            }

            std::ostream& print_impl(std::ostream& o, Null, indent_t)
            {
                return o << "null";
            }

            std::ostream& print_impl(std::ostream& o, String const& s, indent_t)
            {
                return o << "\"" << s << "\"";
            }

            std::ostream& print_impl(std::ostream& o, Array const& array, indent_t indent)
            {
                o << "[\n";
                print_sequence(o, array, indent.more(),
                    [&o, indent=indent.more()](Value const& val) {
                        print_value(o, val, indent);
                    }
                );
                return o << "\n" << indent << "]";
            }

            std::ostream& print_impl(std::ostream& o, Object const& obj, indent_t indent)
            {
                o << indent << "{\n";
                print_sequence(o, obj, indent.more(),
                    [&o, indent=indent.more()]
                    (std::pair<String, Value> const& pair) {
                        print_impl(o, pair.first, indent) << " : ";
                        print_value(o, pair.second, indent);
                    }
                );
                return o << "\n" << indent << "}";
            }

            std::ostream& print_impl(std::ostream& o, Value const& val, indent_t indent)
            {
                return print_value(o, val, std::move(indent));
            }

            std::ostream& print_value(std::ostream& o, Value const& val, indent_t indent)
            {
                val.visit([&] (auto const& v) {
                    print_impl(o, v, indent);
                });
                return o;
            }
        }

        template <typename Val>
        std::ostream& operator << (std::ostream& o, Val&& val)
        {
            indent_t indent{};
            return detail::print_impl(
                o,
                std::forward<Val>(val),
                indent
            );
        }
    }
}
