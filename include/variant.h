#pragma once
#include "meta.h"
#include <utility>
#include <cstdint>

namespace qnd
{

    namespace detail
    {
        template <typename> struct proxy{};

        template <typename...>
        union union_storage { };

        template <typename T, typename... Rest>
        union union_storage<T, Rest...>
        {
        public:
            union_storage(T val)
                : val_(std::move(val))
            { }

            template <typename U>
            union_storage(U val)
                : rest_(std::move(val))
            { }

            ~union_storage() { }

            template <typename U>
            U& get() { return get(proxy<U>{}); }

            template <typename U>
            void destroy() { get(proxy<U>{}).~U(); }

        private:
            template <typename U>
            U& get(proxy<U>) { return rest_.template get<U>(); }

            T& get(proxy<T>) { return val_; }

            T val_;
            union_storage<Rest...> rest_;
        };
    }

    template <typename... Types>
    class variant
    {
        //template <typename T>
        //variant(T val)
            //: index(meta::index_of<T>::in<Types...>::value)
            //, storage_(std::move(val))
        //{ }

    private:
        uint8_t index;
        detail::union_storage<Types...> storage_;
    };



}
