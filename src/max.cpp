#include <thread>
#include <random>
#include <chrono>
#include <fstream>
#include <span>
#include <algorithm>

#include <spdlog/spdlog.h>

int max_of(std::span<int> data)
{
    return *std::max_element(data.begin(), data.end());
}

int maximum(std::vector<int> & numbers, std::size_t n_workers)
{
    const std::size_t chunk_size = numbers.size() / n_workers;
    const std::size_t chunk_remainder = chunk_size + numbers.size() % n_workers;
    std::vector<std::thread> workers;

    std::vector<int> max_values(n_workers, 0);

    for (std::size_t i = 0; i < n_workers - 1; ++i) {
        workers.emplace_back(std::thread([&, i]{
            std::span<int> chunk(numbers.begin() + chunk_size * i, chunk_size);
            max_values[i] = max_of(chunk);
        }));
    }

    std::span<int> chunk(numbers.begin() + chunk_size * (n_workers - 1), numbers.end());
    max_values[n_workers - 1] = max_of(chunk);

    for (auto & w : workers) {
        w.join();
    }

    return max_of(max_values);
}

int main(int argc, char** argv)
{
    const std::string path = argv[1];
    const std::size_t n_workers = std::stoull(argv[2]);

    spdlog::info("Find max from {} and sort with {} workers", path, n_workers);

    std::ifstream file{path};
    std::string line;
    std::vector<int> numbers;
    while (std::getline(file, line, ',')) {
        numbers.emplace_back(std::stoi(line));
    }

    spdlog::info("initial data:\n{}", fmt::join(numbers, ", "));

    const auto start = std::chrono::high_resolution_clock::now();

    auto max = maximum(numbers, n_workers);

    const auto finish = std::chrono::high_resolution_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();

    spdlog::info("max value is {}", max);
    spdlog::info("elapsed: {}mcs {} workers", elapsed, n_workers);
    spdlog::info("{} hw cores", std::thread::hardware_concurrency());
}
