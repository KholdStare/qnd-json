#pragma once

#include "variant.h"
#include <map>
#include <vector>
#include <string>

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
            Value(const char* s)
                : variant(String{s})
            { }

            Value(int v)
                : variant(double(v))
            { }

            using variant::variant;
        };
    }
}
