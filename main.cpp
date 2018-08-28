#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <memory>

std::vector<int64_t> getRandomNumbers(int n) {
    unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed1);
    std::uniform_int_distribution<int64_t> distribution(
        std::numeric_limits<int64_t>::min(),
        std::numeric_limits<int64_t>::max());
    std::vector<int64_t> rv;
    rv.reserve(n);
    for(int i = 0; i < n; ++i) {
        rv.push_back(distribution(generator));
    }
    std::sort(rv.begin(), rv.end());
    return rv;
}

struct TrackingInt {
    std::shared_ptr<int64_t> comp_counter;
    int64_t v;
    TrackingInt(std::shared_ptr<int64_t> comp_counter, int64_t v) : comp_counter(comp_counter), v(v) {}
    friend bool operator<(const TrackingInt& l, const TrackingInt& r) {
        (*(l.comp_counter))++;
        return l.v < r.v;
    }
};

template<class InputIter, class OutputIter>
void dump(InputIter b1, InputIter e1, OutputIter out) {
    while (b1 != e1) {
        *out++ = *b1++;
    }
}

template<class InputIter, class OutputIter>
void naiveMergeSort(InputIter b1, InputIter e1, InputIter b2, InputIter e2, OutputIter out) {
    while (b1 != e1 && b2 != e2) {
        if ((*b1) < (*b2)) {
            *out++ = *b1++;
        } else {
            *out++ = *b2++;
        }
    }
    dump(b1, e1, out);
    dump(b2, e2, out);
}

template<class InputIter, class OutputIter>
void hwangLinMergeSort(InputIter b1, InputIter e1, InputIter b2, InputIter e2, OutputIter out) {
    while (b1 != e1 && b2 != e2) {
        int bound = 3;
        while ((b2+bound+1 < e2) && *(b2+bound) < *b1) {
            auto tmp_iter = b2+bound;
            while (b2 <= tmp_iter) {
                *out++ = *b2++;
            }
        }
        bound = std::min(bound, e2-b2);

        auto b2_iter = std::upper_bound(b2, b2+bound, *b1);
        while (b2 < b2_iter) {
            *out++ = *b2++;
        }
        *out++ = *b1++;
    }
    if (b1 == e1) {
        dump(b2, e2, out);
        return;
    }
    if (b2 == e2) {
        dump(b1, e1, out);
        return;
    }
}

int main() {
    auto naiveCounter = std::make_shared<int64_t>(0);
    auto hwangCounter = std::make_shared<int64_t>(0);

    auto smallList = getRandomNumbers(250000);
    auto bigList = getRandomNumbers(1000000);
    std::vector<TrackingInt> naiveListA, naiveListB, hwangListA, hwangListB;

    for (auto v : smallList) {
        naiveListA.emplace_back(naiveCounter, v);
        hwangListA.emplace_back(hwangCounter, v);
    }
    for (auto v : bigList) {
        naiveListB.emplace_back(naiveCounter, v);
        hwangListB.emplace_back(hwangCounter, v);
    }
    std::vector<TrackingInt> naiveOutput, hwangOutput;
    naiveMergeSort(
        naiveListA.begin(),
        naiveListA.end(),
        naiveListB.begin(),
        naiveListB.end(),
        std::back_inserter(naiveOutput));
    hwangLinMergeSort(
        hwangListA.begin(),
        hwangListA.end(),
        hwangListB.begin(),
        hwangListB.end(),
        std::back_inserter(hwangOutput));

    // Print results
    std::cout << "Naive count: " << *naiveCounter << std::endl;
    std::cout << "Hwang-Lin count: " << *hwangCounter << std::endl;
    if (!std::is_sorted(naiveOutput.begin(), naiveOutput.end(), [](const auto& x, const auto& y) {return x.v < y.v;})) {
        std::cout << "Naive not sorted!" << std::endl;
    }
    if (!std::is_sorted(hwangOutput.begin(), hwangOutput.end(), [](const auto& x, const auto& y) {return x.v < y.v;})) {
        std::cout << "Hwang-Lin not sorted!" << std::endl;
    }
    if (hwangOutput.size() != 1250000 || naiveOutput.size() != 1250000) {
        std::cout << "Wrong size vectors!" << std::endl;
    }
    return 0;
}

//boost::multiprecision::uint1024_t y = 0;
//#include <boost/multiprecision/cpp_int.hpp>
