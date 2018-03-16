#ifndef _OBJECT_POOL_H_INCLUDED
#define _OBJECT_POOL_H_INCLUDED

#include <memory>
#include <functional>
#include <vector>

template<typename T>
using UniquePtr = std::unique_ptr<T, std::function<void(T *)>>;

template<typename T, typename Storage = std::aligned_storage_t<sizeof(T)>>
class ObjectPool 
    : public std::enable_shared_from_this<ObjectPool<T, Storage>>
    , private std::vector<std::unique_ptr<Storage>> {
public:
    using Super = std::vector<std::unique_ptr<Storage>>;
    using Self = ObjectPool<T, Storage>;

    static std::shared_ptr<Self> Create() {
        return { new Self, [](Self *p) { delete p; } };
    }

    template<typename...Args>
    UniquePtr<T> ResolveUnique(Args&&...args) {
        auto storage = ResolveStorage();

        new (storage.get()) T(std::forward<Args>(args)...);

        return {
            reinterpret_cast<T *>(storage.release()),
            [pool = this->shared_from_this()](T *p) {
                p->~T();
                pool->emplace_back(reinterpret_cast<Storage *>(p));
            }
        };
    }

    template<typename...Args>
    std::shared_ptr<T> ResolveShared(Args&&...args) {
        return ResolveUnique(std::forward<Args>(args)...);
    }

    using Super::empty;
    using Super::size;
    using Super::clear;
private:
    ObjectPool() = default;
    ~ObjectPool() = default;

    UniquePtr<Storage> ResolveStorage() {
        std::unique_ptr<Storage> storage;

        if (this->empty()) {
            storage.reset(new Storage);
        } else {
            storage = std::move(this->back());
            this->pop_back();
        }

        return {
            storage.release(),
            [this](Storage *p) {
                this->emplace_back(p);
            }
        };
    }
};

#endif // _OBJECT_POOL_H_INCLUDED

