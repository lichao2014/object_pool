#include "object_pool.h"

int main()
{
    ObjectPool<std::string> p;

    auto a = p.get_unique("a");
    a.reset();

    auto b = p.get_shared("b");
    auto c = p.get_unique("c");
    b.reset();
    c.reset();


    return 0;
}