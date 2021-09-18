#pragma once

namespace mcfile {

template<class T>
inline T Clamp(auto v) {
    using V = decltype(v);
    V min = static_cast<V>(std::numeric_limits<T>::lowest());
    V max = static_cast<V>(std::numeric_limits<T>::max());
    V clamped = std::min<V>(std::max<V>(v, min), max);
    return static_cast<T>(clamped);
}

} // namespace mcfile
