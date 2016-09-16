#include <iostream>

namespace qnd
{
    struct indent_t
    {
        indent_t(size_t depth = 0)
            : s_(depth, '\t')
        { }

        indent_t more() const
        {
            return indent_t(s_.size() + 1);
        }

        friend
        std::ostream& operator << (std::ostream& o, indent_t const& indent)
        {
            return o << indent.s_;
        }

    private:
        std::string s_;
    };

    template <typename Container, typename Action, typename DelimitAction>
    void join(Container const& container, Action&& action, DelimitAction&& delimit)
    {
        auto curIt = container.begin();
        auto end = container.end();
        if (curIt == end) { return; }
        action(*curIt++);

        for(; curIt != end; ++curIt)
        {
            delimit();
            action(*curIt);
        }
    }

    template <typename Container, typename Printer>
    std::ostream& print_sequence(std::ostream& o, Container const& container, indent_t indent, Printer&& printer)
    {
        o << indent;
        join(
            container,
            std::forward<Printer>(printer),
            [&] () {
                o << ",\n" << indent;
            }
        );
        return o;
    }

}
