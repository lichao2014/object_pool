#include "object_pool.h"


class A : public std::string
{
public:
    A()
    {
        throw 1;
    }

    A(int) {}
};

int main()
{
    auto pool = std::make_shared<ObjectPool<A>>();

    auto a = pool->getUnique(1);
    a.reset();

    try
    {
        auto b = pool->getShared();
    }
    catch (...)
    {
        //nop
    }

    auto c = pool->getShared(7);
    auto d = pool->getUnique(2);
    //b.reset();
    //c.reset();

    pool.reset();


    return 0;
}