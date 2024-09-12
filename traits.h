#pragma once
#include <type_traits>

template <class F, class... Args>
struct can_take {
private:
    static constexpr auto test(int) -> decltype(std::declval<F>()(std::declval<Args...>()), std::true_type());
    static constexpr auto test(...) -> std::false_type;
public:
    static constexpr bool value = decltype(test(0))::value;
};