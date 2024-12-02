#include <atomic>
#include <vector>
#include <thread>
#include <random>

#include <spdlog/spdlog.h>

#include <monte_carlo.hpp>

double monte_carlo_pi(std::size_t n_points, std::size_t n_workers) {
    spdlog::info("Solve Pi with Monte-Carlo: points={} workers={}", n_points, n_workers);

    std::atomic<std::uint64_t> points_in_circle{0};
    const auto points_per_thread = n_points / n_workers;
    const auto points_per_last_thread = points_per_thread + n_points % n_workers;

    auto compute = [&](std::uint64_t points) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(0.0, 1.0);

        std::uint64_t point_count = 0;

        for (int i = 0; i < points; ++i) {
            const double x = dist(gen);
            const double y = dist(gen);
            if (x * x + y * y <= 1.0) {
                ++point_count;
            }
        }

        points_in_circle.fetch_add(point_count);
    };

    std::vector<std::thread> threads;
    for (std::size_t i = 0; i < n_workers - 1; ++i) {
        threads.emplace_back([&]{ compute(points_per_thread); });
    }

    compute(points_per_last_thread);

    for (auto & t : threads) {
        t.join();
    }

    const double pi_estimate = 4.0 * points_in_circle / n_points;
    return pi_estimate;
}

int main(int argc, char** argv)
{
    const std::size_t n_points = std::stoull(argv[1]);
    const std::size_t n_workers = std::stoull(argv[2]);
    const auto n_cores = std::thread::hardware_concurrency(); 

    const auto start = std::chrono::high_resolution_clock::now();
    const auto pi_estimate = monte_carlo_pi(n_points, n_workers);
    const auto finish = std::chrono::high_resolution_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    spdlog::info("PI={} points={} workers={} cores={} time={}", pi_estimate, n_points, n_workers, n_cores, elapsed);
}
