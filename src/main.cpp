#include <atomic>
#include <vector>
#include <thread>
#include <random>
#include <chrono>

#include <spdlog/spdlog.h>

#include <monte_carlo.hpp>
#include <cannon.hpp>

int main(int argc, char** argv)
{
    const std::size_t n_points = std::stoull(argv[1]);
    const std::size_t n_workers = std::stoull(argv[2]);
    const auto n_cores = std::thread::hardware_concurrency(); 

    const auto start = std::chrono::high_resolution_clock::now();
    // const auto pi_estimate = monte_carlo_pi(n_points, n_workers);
    cannon_matmul(n_points, n_workers);
    const auto finish = std::chrono::high_resolution_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    // spdlog::info("PI={} points={} workers={} cores={} time={}", pi_estimate, n_points, n_workers, n_cores, elapsed);
}
