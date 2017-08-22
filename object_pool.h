#ifndef _OBJECT_POOL_H_INCLUDED
#define _OBJECT_POOL_H_INCLUDED


#include <memory>
#include <functional>
#include <vector>

template<typename T, typename Storage = std::aligned_storage_t<sizeof(T)>>
class ObjectPool 
    : public std::enable_shared_from_this<ObjectPool<T, Storage>>
    , private std::vector<std::unique_ptr<Storage>>
{
public:
    using Base = std::vector<std::unique_ptr<Storage>>;

    ObjectPool() = default;

    template<typename...Args>
    std::unique_ptr<T, std::function<void(T *)>> getUnique(Args&&...args)
    {
        auto storage = resolveStorage();

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
    std::shared_ptr<T> getShared(Args&&...args)
    {
        return getUnique(std::forward<Args>(args)...);
    }

    using Base::empty;
    using Base::size;
    using Base::clear;

private:
    std::unique_ptr<Storage, std::function<void(Storage *)>> resolveStorage()
    {
        Storage *storage = nullptr;

        if (empty()) {
            storage = new Storage;
        }
        else {
            storage = this->back().release();
            this->pop_back();
        }

        return {
            storage,
            [this](Storage *p) {
                this->emplace_back(p);
            }
        };
    }

};

#endif // _OBJECT_POOL_H_INCLUDED

