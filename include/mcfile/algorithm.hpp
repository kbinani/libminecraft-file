#pragma once

namespace mcfile {

template<class R>
inline R Clamp(auto v) {
    using V = decltype(v);
    static_assert(std::is_integral_v<R> == std::is_integral_v<V> && std::is_floating_point_v<R> == std::is_floating_point_v<V>);
    using C = typename std::common_type<R, V>::type;
    C constexpr min = static_cast<C>(std::numeric_limits<R>::lowest());
    C constexpr max = static_cast<C>(std::numeric_limits<R>::max());
    C clamped = std::min<C>(std::max<C>(v, min), max);
    return static_cast<R>(clamped);
}

} // namespace mcfile
