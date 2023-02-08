#pragma once

namespace mcfile {
namespace detail {

template<class T>
class Pos3 {
public:
    Pos3(T x, T y, T z)
        : fX(x)
        , fZ(z)
        , fY(y) {}

    Pos3()
        : fX(0)
        , fZ(0)
        , fY(0) {}

    bool operator==(Pos3<T> const &other) const { return fX == other.fX && fZ == other.fZ && fY == other.fY; }

    static double DistanceSquare(Pos3<T> const &a, Pos3<T> const &b) {
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

template<class T>
class Pos3Hasher {
public:
    size_t operator()(Pos3<T> const &k) const {
        size_t res = 17;
        res = res * 31 + std::hash<T>{}(k.fX);
        res = res * 31 + std::hash<T>{}(k.fY);
        res = res * 31 + std::hash<T>{}(k.fZ);
        return res;
    }
};

template<class T>
inline Pos3<T> operator+(Pos3<T> const &lhs, Pos3<T> const &rhs) {
    return Pos3<T>(lhs.fX + rhs.fX, lhs.fY + rhs.fY, lhs.fZ + rhs.fZ);
}

template<class T>
inline Pos3<T> operator-(Pos3<T> const &lhs, Pos3<T> const &rhs) {
    return Pos3<T>(lhs.fX - rhs.fX, lhs.fY - rhs.fY, lhs.fZ - rhs.fZ);
}

template<class T>
inline Pos3<T> operator*(Pos3<T> const &v, T scalar) {
    return Pos3(v.fX * scalar, v.fY * scalar, v.fZ * scalar);
}

} // namespace detail

using Pos3i = detail::Pos3<int32_t>;
using Pos3iHasher = detail::Pos3Hasher<int32_t>;

inline std::shared_ptr<mcfile::nbt::Tag> Pos3iToNbt(Pos3i const &pos) {
    using namespace std;
    using namespace mcfile::nbt;
    auto c = make_shared<CompoundTag>();
    (*c)["x"] = make_shared<IntTag>(pos.fX);
    (*c)["y"] = make_shared<IntTag>(pos.fY);
    (*c)["z"] = make_shared<IntTag>(pos.fZ);
    return c;
}

inline std::optional<Pos3i> Pos3iFromNbt(mcfile::nbt::Tag const &tag) {
    using namespace std;
    auto c = tag.asCompound();
    if (!c) {
        return nullopt;
    }
    auto x = c->int32("x");
    auto y = c->int32("y");
    auto z = c->int32("z");
    if (!x || !y || !z) {
        return nullopt;
    }
    return Pos3i(*x, *y, *z);
}

} // namespace mcfile
