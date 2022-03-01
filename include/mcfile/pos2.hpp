#pragma once

namespace mcfile {
namespace detail {

template<class T>
class Pos2 {
public:
    Pos2(T x, T z)
        : fX(x)
        , fZ(z) {}

    Pos2()
        : fX(0)
        , fZ(0) {}

    bool operator==(Pos2<T> const &other) const { return fX == other.fX && fZ == other.fZ; }

    static double DistanceSquare(Pos2<T> const &a, Pos2<T> const &b) {
        double dx = a.fX - b.fX;
        double dz = a.fZ - b.fZ;
        return dx * dx + dz * dz;
    }

    Pos2<T> rotated(T radian) const {
        T sin = std::sin(radian);
        T cos = std::cos(radian);
        T x = fX * cos - fZ * sin;
        T z = fX * sin + fZ * cos;
        return Pos2<T>(x, z);
    }

public:
    T fX;
    T fZ;
};

template<class T>
class Pos2Hasher {
public:
    size_t operator()(Pos2<T> const &k) const {
        size_t res = 17;
        res = res * 31 + std::hash<T>{}(k.fX);
        res = res * 31 + std::hash<T>{}(k.fZ);
        return res;
    }
};

template<class T>
inline Pos2<T> operator+(Pos2<T> const &lhs, Pos2<T> const &rhs) {
    return Pos2<T>(lhs.fX + rhs.fX, lhs.fZ + rhs.fZ);
}

template<class T>
inline Pos2<T> operator-(Pos2<T> const &lhs, Pos2<T> const &rhs) {
    return Pos2<T>(lhs.fX - rhs.fX, lhs.fZ - rhs.fZ);
}

} // namespace detail

using Pos2i = detail::Pos2<int32_t>;
using Pos2iHasher = detail::Pos2Hasher<int32_t>;

} // namespace mcfile
