#include <thread>
#include <random>
#include <chrono>
#include <fstream>
#include <span>
#include <algorithm>
#include <bitset>

#include <spdlog/spdlog.h>

#include <BS_thread_pool.hpp>

#include <gmpxx.h>

// mpz_class fib(std::size_t N, std::size_t n_workers)
// {
//     if (N == 1) return 1;
//     if (N == 2) return 1;

//     BS::thread_pool pool(n_workers);

//     std::vector<mpz_class> memo{-1, 1, 1};
//     memo.reserve(N);

//     for (std::size_t i = 3; i <= N; ++i) {
//         mpz_class f = memo[i - 1] + memo[i - 2];
//         memo.emplace_back(std::move(f));
//     }

//     return memo.back();
// }

std::string dec_to_bin(std::size_t n)
{
    std::string bin = std::bitset<sizeof(std::size_t) * 8>(n).to_string();
    auto loc = bin.find('1');

    if (loc != std::string::npos)
        return bin.substr(loc);
    return "0";
}

mpz_class fib(std::size_t N, std::size_t n_workers) {
    BS::thread_pool pool(n_workers);

    auto bin_of_n = dec_to_bin(N);
 
    mpz_class f[] = { 0, 1 }; // [F(i), F(i+1)] => i=0
 
    for (auto b : bin_of_n) {
        auto f1s_task = pool.submit_task([&]{ mpz_class f1s = f[1] * f[1]; return f1s; }); // F(1)^2
        auto f0s_task = pool.submit_task([&]{ mpz_class f0s = f[0] * f[0]; return f0s; }); // F(0)^2

        auto df1mf0_task = pool.submit_task([&]{ mpz_class res = 2 * f[1] - f[0]; return res; }); // 2 * f[1] - f[0]

        auto f2i1_task = pool.submit_task([&]{ mpz_class f2i1 = f0s_task.get() + f1s_task.get(); return f2i1; }); // F(2i+1)
        auto f2i_task = pool.submit_task([&]{ mpz_class f2i = f[0] * df1mf0_task.get(); return f2i; });   // F(2i+1)
        
        mpz_class f2i1 = f2i1_task.get();
        mpz_class f2i = f2i_task.get();
 
        if (b == '0') {
            f[0] = std::move(f2i); // F(2i)
            f[1] = std::move(f2i1); // F(2i+1)
        }
        else {
            f[1] = f2i1 + f2i; // F(2i+2)
            f[0] = std::move(f2i1); // F(2i+1)
        }
    }
 
    return f[0];
}

int main(int argc, char** argv)
{
    const std::size_t N = std::stoull(argv[1]);
    const std::size_t n_workers = std::stoull(argv[2]);

    spdlog::info("Find {}th fibonacci number with {} workers", N, n_workers);

    const auto start = std::chrono::high_resolution_clock::now();

    auto f = fib(N, n_workers);

    const auto finish = std::chrono::high_resolution_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    spdlog::info("{}th fibonacci number is {}", N, f.get_str());
    spdlog::info("elapsed: {}ms {} workers", elapsed, n_workers);
    spdlog::info("{} hw cores", std::thread::hardware_concurrency());
}
