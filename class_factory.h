#ifndef _CLASS_FACTORY_H_INCLUDED
#define _CLASS_FACTORY_H_INCLUDED

#include <tuple>
#include <any>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <string>
#include <memory>

constexpr std::size_t HashCombine(std::size_t lhs, std::size_t rhs) {
    return lhs ^ (rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2));
}

template<typename T>
constexpr std::hash<T> hasher{};

namespace std {
    template<typename First, typename Second>
    struct hash<std::pair<First, Second>> {
        constexpr std::size_t operator()(const std::pair<First, Second>& x) const noexcept {
            return HashCombine(hasher<First>(x.first), hasher<Second>(x.second));
        }
    };
}

class ClassFactory {
public:
    template<typename T, typename ... Args>
    bool Register(const std::string& name) {
        return creators_.try_emplace(
            { name, typeid(std::tuple<Args...>) },
            [](std::any arg) {
                return reinterpret_cast<void *>(
                    std::apply(ClassFactory::ApplyObject<T, Args...>,
                        std::any_cast<std::tuple<Args...>>(arg)));
            }
        ).second;
    }

    template<typename T, typename ... Args>
    std::unique_ptr<T> CreateObject(const std::string& name, Args&& ... args) {
        auto it = creators_.find({ name, typeid(std::tuple<Args...>) });
        if (creators_.end() == it) {
            return nullptr;
        }

        auto p = reinterpret_cast<T *>(it->second(std::make_tuple(std::forward<Args>(args)...)));
        return std::unique_ptr<T>{ p };
    }
private:
    template<typename T, typename ... Args>
    static T *ApplyObject(Args&& ... args) {
        return new T(std::forward<Args>(args)...);
    }

    using CreatorMap = std::unordered_map<std::pair<std::string, std::type_index>, std::function<void *(std::any)>>;
    CreatorMap creators_;
};

#endif // !_CLASS_FACTORY_H_INCLUDED
