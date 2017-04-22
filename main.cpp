#include <string>
#include "object_pool.h"


int main()
{
    auto pool = std::make_shared<ObjectPool<std::string>>();

    using namespace std::string_literals;
    auto a = pool->get_unique("a"s);
    a.reset();

    auto b = pool->get_shared("b");
    auto c = pool->get_unique("c");
    //b.reset();
    //c.reset();

    pool.reset();


    return 0;
}