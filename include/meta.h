#pragma once
#include <cstddef>

namespace qnd
{
    namespace meta
    {
        namespace detail
        {
            template <size_t Index, typename Needle, typename... Haystack>
            struct index_of_impl;

            template <size_t Index, typename Needle>
            struct index_of_impl<Index, Needle>
            {
                // Not found
            };

            template <size_t Index, typename Found, typename... Haystack>
            struct index_of_impl<Index, Found, Found, Haystack...>
            {
                static constexpr size_t value = Index;
            };

            template <size_t Index, typename Needle, typename Other, typename... Haystack>
            struct index_of_impl<Index, Needle, Other, Haystack...>
                : index_of_impl<Index + 1, Needle, Haystack...>
            {
            };

        }

        template <typename Needle>
        struct index_of
        {
            template <typename... Haystack>
            struct in : detail::index_of_impl<0, Needle, Haystack...> { };
        };
    }
}
