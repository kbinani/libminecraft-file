#pragma once

namespace mcfile {
namespace detail {

template <class T>
class Pos3 {
public:
    Pos3(T x, T y, T z) : fX(x), fZ(z), fY(y) {}

    bool operator==(Pos3<T> const& other) const { return fX == other.fX && fZ == other.fZ && fY == other.fY; }

    static double DistanceSquare(Pos3<T> const& a, Pos3<T> const& b) {
        double dx = a.fX - b.fX;
        double dy = a.fY - b.fY;
        double dz = a.fZ - b.fZ;
        return dx * dx + dy * dy + dz * dz;
    }

public:
    T fX;
    T fZ;
    T fY;
};

template <class T>
class Pos3Hasher {
public:
    size_t operator()(Pos3<T> const& k) const {
        size_t res = 17;
        res = res * 31 + std::hash<T>{}(k.fX);
        res = res * 31 + std::hash<T>{}(k.fY);
        res = res * 31 + std::hash<T>{}(k.fZ);
        return res;
    }
};

template <class T>
inline Pos3<T> operator + (Pos3<T> const& lhs, Pos3<T> const& rhs) {
    return Pos3<T>(lhs.fX + rhs.fX, lhs.fY + rhs.fY, lhs.fZ + rhs.fZ);
}

template <class T>
inline Pos3<T> operator - (Pos3<T> const& lhs, Pos3<T> const& rhs) {
    return Pos3<T>(lhs.fX - rhs.fX, lhs.fY - rhs.fY, lhs.fZ - rhs.fZ);
}

}

using Pos3i = detail::Pos3<int>;
using Pos3iHasher = detail::Pos3Hasher<int>;

}
