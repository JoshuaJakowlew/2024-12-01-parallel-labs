#include <thread>
#include <random>
#include <chrono>
#include <fstream>
#include <span>
#include <algorithm>

#include <spdlog/spdlog.h>

void sort_chunk(std::span<int> data)
{
    std::sort(data.begin(), data.end());
}

void merge_sort(std::vector<int> & numbers, std::size_t n_workers)
{
    const std::size_t chunk_size = numbers.size() / n_workers;
    const std::size_t chunk_remainder = chunk_size + numbers.size() % n_workers;
    std::vector<std::thread> workers;
    for (std::size_t i = 0; i < n_workers - 1; ++i) {
        workers.emplace_back(std::thread([&, i]{
            std::span<int> chunk(numbers.begin() + chunk_size * i, chunk_size);
            sort_chunk(chunk);
        }));
    }

    std::span<int> chunk(numbers.begin() + chunk_size * (n_workers - 1), numbers.end());
    sort_chunk(chunk);

    for (auto & w : workers) {
        w.join();
    }

    auto mid = numbers.begin() + chunk_size;
    for (std::size_t i = 0; i < n_workers - 1; ++i) {
        std::inplace_merge(numbers.begin(), mid, mid + (i == n_workers - 2 ? chunk_remainder : chunk_size));
        mid += chunk_size;
    }
}

int main(int argc, char** argv)
{
    const std::string path = argv[1];
    const std::size_t n_workers = std::stoull(argv[2]);

    spdlog::info("Read numbers from {} and sort with {} workers", path, n_workers);

    std::ifstream file{path};
    std::string line;
    std::vector<int> numbers;
    while (std::getline(file, line, ',')) {
        numbers.emplace_back(std::stoi(line));
    }

    const auto start = std::chrono::high_resolution_clock::now();

    merge_sort(numbers, n_workers);

    const auto finish = std::chrono::high_resolution_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();

    std::ofstream sorted{fmt::format("{}.sorted", path)};
    sorted << fmt::format("{}", fmt::join(numbers, ","));

    spdlog::info("elapsed: {}mcs {} workers", elapsed, n_workers);
    spdlog::info("{} hw cores", std::thread::hardware_concurrency());
}
