#pragma once
#include "meta.h"
#include <utility>
#include <cstdint>

namespace qnd
{

    namespace detail
    {
        template <typename...> struct proxy{};

        template <typename...>
        union union_storage { };

        template <typename T, typename... Rest>
        union union_storage<T, Rest...>
        {
        public:
            union_storage()
            { }

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
            U const& get() const { return get(proxy<U>{}); }

            template <typename U, typename... Args>
            void emplace(Args&&... args)
            {
                emplace_impl(proxy<U>{}, std::forward<Args>(args)...);
            }

            template <typename U>
            void destroy() { get(proxy<U>{}).~U(); }

        private:
            template <typename... Args>
            void emplace_impl(proxy<T>, Args&&... args)
            {
                new (&val_) T(std::forward<Args>(args)...);
            }

            template <typename U, typename... Args>
            void emplace_impl(proxy<U> p, Args&&... args)
            {
                rest_.template emplace<U>(std::forward<Args>(args)...);
            }

            template <typename U>
            U& get(proxy<U>) { return rest_.template get<U>(); }

            template <typename U>
            U const& get(proxy<U>) const { return rest_.template get<U>(); }

            T& get(proxy<T>) { return val_; }
            T const& get(proxy<T>) const { return val_; }

            T val_;
            union_storage<Rest...> rest_;
        };

        struct destroyer
        {
            template <typename T>
            void operator() (T& val)
            {
                val.~T();
            }
        };
    }

    template <typename... Types>
    class variant
    {
    public:
        template <typename T>
        variant(T val)
            : index(meta::index_of<T>::template in<Types...>::value)
            , storage_(std::move(val))
        { }

        ~variant()
        {
            visit(detail::destroyer{});
        }

        template <typename F>
        void visit(F&& f)
        {
            visit_impl(std::forward<F>(f), detail::proxy<Types...>{});
        }

    private:
        template <typename F>
        void visit_impl(F&& f, detail::proxy<>)
        {

        }

        template <typename F, typename T, typename... Rest>
        void visit_impl(F&& f, detail::proxy<T, Rest...>)
        {
            constexpr const uint8_t current_index =
                sizeof...(Types) - (sizeof...(Rest) + 1);
            if (index == current_index)
            {
                return f(storage_.template get<T>());
            }
            else
            {
                return visit_impl(std::forward<F>(f), detail::proxy<Rest...>{});
            }
        }

        uint8_t index;
        detail::union_storage<Types...> storage_;
    };
}
