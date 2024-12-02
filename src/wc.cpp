#include <thread>
#include <random>
#include <chrono>
#include <fstream>
#include <span>
#include <algorithm>
#include <filesystem>
#include <map>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/std.h>

#include <BS_thread_pool.hpp>

#include <range/v3/view/split_when.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/common.hpp>
#include <range/v3/view/transform.hpp>

#include <range/v3/range/conversion.hpp>

namespace fs = std::filesystem;
namespace rg = ranges;
namespace vw = ranges::views;

using word_map = std::unordered_map<std::string, std::size_t>;
using word_list = std::vector<std::tuple<std::string, std::size_t>>;

struct word_histogram
{
    fs::path file;
    word_list map;
};

std::vector<word_histogram> folder_word_histogram(fs::path const & path, std::size_t n_workers)
{
    BS::thread_pool pool(n_workers);

    std::vector<std::future<word_histogram>> results;
    for (auto const & dir : fs::directory_iterator(path)) {
        results.emplace_back(pool.submit_task([=]{
            std::ifstream file{dir.path()};
            std::string data{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
            auto words = data
                | vw::split_when([](char c) { return c == ',' || c == ' '; })
                | vw::filter([](auto const & word){ return !word.empty(); });
            
            word_map map;
            for (auto && word : words) {
                auto w = word | vw::common;
                map[std::string(w.begin(), w.end())] += 1;
            }

            word_list wl(map.cbegin(), map.cend());
            std::sort(wl.begin(), wl.end(), [](auto const & lhs, auto const & rhs) {
                auto const & [w1, c1] = lhs;
                auto const & [w2, c2] = rhs;
                return c1 != c2 ? c1 > c2 : w1 < w2;
            });

            return word_histogram{dir.path(), wl};
        }));
    }

    return results
        | vw::transform([](auto && f) { return f.get(); })
        | rg::to<std::vector>();
}

int main(int argc, char** argv)
{
    const std::string path = argv[1];
    const std::size_t n_workers = std::stoull(argv[2]);

    const auto start = std::chrono::high_resolution_clock::now();

    auto hists = folder_word_histogram(path, n_workers);

    const auto finish = std::chrono::high_resolution_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();

    for (auto & h : hists) {
        spdlog::info("Word map for {}", h.file);
        for (auto && entry : h.map) {
            auto && [k, v] = entry;
            spdlog::info("{}: {}", k, v);
        }
    }

    spdlog::info("==================");
    spdlog::info("elapsed: {}mcs {} workers", elapsed, n_workers);
    spdlog::info("{} hw cores", std::thread::hardware_concurrency());
}
