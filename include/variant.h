#pragma once
#include "meta.h"
#include <utility>
#include <cstdint>
#include <type_traits>

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

        variant(variant const& other)
        {
            copy_from_other(other);
        }

        variant& operator = (variant const& other)
        {
            if (&other != this)
            {
                // TODO: take care of the monostate if copy throws
                visit(detail::destroyer{});
                copy_from_other(other);
            }
            return *this;
        }

        ~variant()
        {
            visit(detail::destroyer{});
        }

        template <typename F>
        void visit(F&& f)
        {
            visit_impl(*this, std::forward<F>(f), detail::proxy<Types...>{});
        }

        template <typename F>
        void visit(F&& f) const
        {
            visit_impl(*this, std::forward<F>(f), detail::proxy<Types...>{});
        }

    private:
        void copy_from_other(variant const& other)
        {
            index = other.index;
            other.visit([this](auto const& val) {
                using type = std::decay_t<decltype(val)>;
                storage_.template emplace<type>(val);
            });
        }

        template <typename Self, typename F>
        static void visit_impl(Self&& self, F&& f, detail::proxy<>) { }

        template <typename Self, typename F, typename T, typename... Rest>
        static void visit_impl(Self&& self, F&& f, detail::proxy<T, Rest...>)
        {
            constexpr const uint8_t current_index =
                sizeof...(Types) - (sizeof...(Rest) + 1);
            if (self.index == current_index)
            {
                return f(self.storage_.template get<T>());
            }
            else
            {
                return visit_impl(std::forward<Self>(self), std::forward<F>(f), detail::proxy<Rest...>{});
            }
        }

        uint8_t index;
        detail::union_storage<Types...> storage_;
    };
}
