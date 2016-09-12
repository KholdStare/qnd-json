#include "include/variant.h"
#include "include/meta.h"
#include <string>
#include <iostream>

void example1()
{
    qnd::detail::union_storage<std::string, double> test{std::string("Hello")};
    std::cout << test.get<std::string>() << std::endl;
    std::cout << test.get<double>() << std::endl;
    test.destroy<std::string>();
}

void example2()
{
    static_assert(
        qnd::meta::index_of<int>::in<double, char, int, bool>::value
        == 2,
        "index_of is broken!"
    );
}

auto printer = [](auto& val) {
    std::cout << val << std::endl;
};

void example3()
{
    using my_variant = qnd::variant<std::string, double>;

    my_variant v1{std::string{"Hello"}};
    my_variant v2{3.14};

    v1.visit(printer);
    v2.visit(printer);
}

void example4()
{
    qnd::detail::union_storage<std::string, double> test;
    test.emplace<std::string>("Woohoo");
    std::cout << test.get<std::string>() << std::endl;
    test.destroy<std::string>();
}

void example5()
{
    using my_variant = qnd::variant<std::string, double>;

    my_variant v1{std::string{"Example5"}};
    my_variant v2{3.14};
    my_variant v3{v2};
    //v2 = v1;

    v1.visit(printer);
    v2.visit(printer);
    v3.visit(printer);
}

int main(int argc, char* argv[])
{
    example1();
    example2();
    example3();
    example4();
    example5();

    return 0;
}
