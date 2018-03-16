#include "object_pool.h"
#include "class_factory.h"

class I {
public:
    virtual ~I() = default;
    virtual void Say() = 0;
};

class A : public I {
public:
    A() = default;
    explicit A(int a) : a_(a) {}

    void Say() override {

    }
private:
    int a_ = 0;
};


void TestObjectPool() {
    auto pool = ObjectPool<A>::Create();

    auto a = pool->ResolveUnique(1);
    a.reset();

    try
    {
        auto b = pool->ResolveShared();
    }
    catch (...)
    {
        //nop
    }

    auto c = pool->ResolveShared(7);
    auto d = pool->ResolveUnique(2);
    //b.reset();
    //c.reset();

    pool.reset();
}

void TestClassFactory() {
    ClassFactory cf;

    cf.Register<A>("A");
    auto p = cf.CreateObject<A>("A");

    p->Say();

    cf.Register<A, int>("A");
    p = cf.CreateObject<A>("A", 123);

    p->Say();
}

int main() {
    TestObjectPool();
    TestClassFactory();

    return 0;
}