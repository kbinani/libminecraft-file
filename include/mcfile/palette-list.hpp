#pragma once

namespace mcfile {

template<class Value, class Index, size_t size, class Hasher = std::hash<Value>, class Pred = std::equal_to<Value>, size_t factorNum = 2, size_t factorDen = 1>
class PaletteList {
    static_assert(std::is_integral<Index>::value);
    static_assert(std::is_unsigned<Index>::value);
    static_assert(factorDen > 0);
    static_assert(factorNum > 0);
    static_assert(factorNum > factorDen);

    static constexpr double kFactor = (double)factorNum / (double)factorDen;
    static constexpr size_t kMaxPaletteSize = size * kFactor;

    static_assert(kMaxPaletteSize <= std::numeric_limits<Index>::max());
    static_assert(size > 0);

public:
    std::optional<Value> get(size_t idx) const {
        auto index = this->index(idx);
        if (!index) {
            return std::nullopt;
        }
        if (fValue.size() <= *index) {
            return std::nullopt;
        }
        return fValue[*index];
    }

    std::optional<Index> index(size_t idx) const {
        if (fIndex.empty() || fIndex.size() <= idx) {
            return std::nullopt;
        }
        return fIndex[idx];
    }

    bool set(size_t idx, Value value) {
        if (size <= idx) {
            return false;
        }
        if (fIndex.size() != size) {
            fIndex.clear();
            fIndex.resize(size, 0);
            fValue.clear();
            fValue.push_back(value);
            fLut.clear();
            fLut[value] = 0;
            return true;
        }
        auto found = fLut.find(value);
        if (found != fLut.end()) {
            Index index = found->second;
            fIndex[idx] = index;
            return true;
        }
        Index index = fValue.size();
        fValue.push_back(value);
        fLut[value] = index;
        fIndex[idx] = index;

        if (fValue.size() > kMaxPaletteSize) {
            shrinkToFit();
        }

        return true;
    }

    void shrinkToFit() {
        ShrinkToFitImpl(fValue, fIndex, &fLut);
    }

    void copy(std::vector<Value> &palette, std::vector<Index> &index) const {
        palette.clear();
        index.clear();
        std::copy(fValue.begin(), fValue.end(), std::back_inserter(palette));
        std::copy(fIndex.begin(), fIndex.end(), std::back_inserter(index));
    }

    bool reset(std::vector<Value> const &palette, std::vector<Index> const &index) {
        if (index.size() != size) {
            return false;
        }
        if (palette.size() > std::numeric_limits<Index>::max()) {
            return false;
        }
        Index s = static_cast<Index>(palette.size());
        if (std::any_of(index.begin(), index.end(), [s](Index i) { return i >= s; })) {
            return false;
        }

        fIndex.clear();
        fLut.clear();
        fValue.clear();
        std::copy(palette.begin(), palette.end(), std::back_inserter(fValue));
        std::copy(index.begin(), index.end(), std::back_inserter(fIndex));
        for (Index i = 0; i < s; i++) {
            Value const &value = palette[i];
            fLut[value] = i;
        }
        return true;
    }

    bool empty() const {
        return fIndex.size() != size;
    }

    void eachValue(std::function<bool(Value const &)> cb) const {
        for (Value const &v : fValue) {
            if (!cb(v)) {
                return;
            }
        }
    }

    static bool ShrinkToFit(std::vector<Value> &inoutPalette, std::vector<Index> &inoutIndex) {
        return ShrinkToFitImpl(inoutPalette, inoutIndex);
    }

private:
    static bool ShrinkToFitImpl(std::vector<Value> &inoutPalette, std::vector<Index> &inoutIndex, std::unordered_map<Value, Index, Hasher, Pred> *inoutLut = nullptr) {
        std::vector<Value> palette;
        std::unordered_map<Value, Index, Hasher, Pred> lut;
        std::vector<Index> index;
        index.resize(inoutIndex.size());
        for (size_t idx = 0; idx < inoutIndex.size(); idx++) {
            Index i = inoutIndex[idx];
            assert(i < inoutPalette.size());
            if (inoutPalette.size() <= i) {
                return false;
            }
            Value const &v = inoutPalette[i];
            auto found = lut.find(v);
            if (found == lut.end()) {
                Index next = palette.size();
                palette.push_back(v);
                lut[v] = next;
                index[idx] = next;
            } else {
                index[idx] = found->second;
            }
        }
        inoutPalette.swap(palette);
        inoutIndex.swap(index);
        if (inoutLut) {
            inoutLut->swap(lut);
        }
        return true;
    }

private:
    std::vector<Value> fValue;
    std::vector<Index> fIndex;
    std::unordered_map<Value, Index, Hasher, Pred> fLut;
};

} // namespace mcfile
