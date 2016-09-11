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

int main(int argc, char* argv[])
{
    example1();
    example2();

    return 0;
}
