#ifndef _OBJECT_POOL_H_INCLUDED_
#define _OBJECT_POOL_H_INCLUDED_


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
    using Deleter = std::function<void(T *)>;

    ObjectPool() = default;

    template<typename...Args>
    std::unique_ptr<T, Deleter> get_unique(Args&&...args)
    {
        std::unique_ptr<Storage> block;

        if (this->empty()) {
            block.reset(new Storage);
        }
        else {
            block.reset(this->back().release());
            this->pop_back();
        }

        new (block.get()) T(std::forward<Args>(args)...);

        return {
            reinterpret_cast<T *>(block.release()),
            [pool = shared_from_this()](T *p) {
                p->~T();
                pool->emplace_back(reinterpret_cast<Storage *>(p));
            }
        };
    }

    template<typename...Args>
    std::shared_ptr<T> get_shared(Args&&...args)
    {
        return get_unique(std::forward<Args>(args)...);
    }

    using Base::empty;
    using Base::size;
    using Base::clear;
};



#endif // _OBJECT_POOL_H_INCLUDED_

