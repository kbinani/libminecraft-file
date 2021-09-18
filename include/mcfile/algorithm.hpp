#pragma once

namespace mcfile {

template<class R>
inline auto Clamp(auto v) -> typename std::enable_if<std::is_integral<R>::value == std::is_integral<decltype(v)>::value && std::is_floating_point<R>::value == std::is_floating_point<decltype(v)>::value, R>::type {
    using V = decltype(v);
    using C = typename std::common_type<R, V>::type;
    C constexpr min = static_cast<C>(std::numeric_limits<R>::lowest());
    C constexpr max = static_cast<C>(std::numeric_limits<R>::max());
    C clamped = std::min<C>(std::max<C>(v, min), max);
    return static_cast<R>(clamped);
}

} // namespace mcfile
