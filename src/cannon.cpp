#include <cannon.hpp>

#include <random>
#include <chrono>
#include <iostream>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ranges.h>

#include <range/v3/view/chunk.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/stride.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/zip_with.hpp>

#include <range/v3/algorithm/rotate.hpp>
#include <range/v3/algorithm/fold.hpp>

#include <BS_thread_pool.hpp>


namespace rg = ranges;
namespace vw = ranges::views;

std::vector<int> random_matrix(std::size_t N) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(-10, 10);

    std::vector<int> mat;
    mat.reserve(N * N);

    for (std::size_t i = 0; i < N*N; ++i) {
        mat.emplace_back(dist(gen));
    }

    return mat;
}

std::vector<int> deserealize(std::size_t N) {
    spdlog::info("input matrix ({}x{}):", N, N);
    
    std::vector<int> m;
    m.reserve(N*N);
    for (std::size_t i = 0; i < N * N; ++i) {
        int x; std::cin >> x;
        m.emplace_back(x);
    }
    return m;
}

std::string serialize(std::vector<int> const & m, std::size_t N) {
    std::string res;
    for (auto const row : m | vw::chunk(N)) {
        res += fmt::format("{:3}\n", fmt::join(row, ", "));
    }
    return fmt::format("[\n{}]", res);
}

void block_row_shift(std::vector<int>& m, std::size_t N, std::size_t bs, std::size_t row, std::size_t shift_blocks) {
    for (int i = 0; i < bs; ++i) {
        const auto shift = shift_blocks * bs;
        auto row_start = m.begin() + (row * bs + i) * N; // i-th row of block
        auto row_end = row_start + N;
        auto mid = row_start + shift;
        rg::rotate(row_start, mid, row_end);
    }    
}

void block_col_shift(std::vector<int>& m, std::size_t N, std::size_t bs, std::size_t col, std::size_t shift_blocks) {
    for (int i = 0; i < bs; ++i) {
        auto t = m | vw::drop(col * bs + i) | vw::stride(N);
        const auto shift = shift_blocks * bs;
        rg::rotate(t.begin(), t.begin() + shift, t.end());
    }    
}

void initial_row_shift(std::vector<int>& m, std::size_t N, std::size_t bs) {
    const auto n_blocks = N / bs;
    for (std::size_t row = 0; row < n_blocks; ++row) {
        block_row_shift(m, N, bs, row, row);
    }
}

void initial_col_shift(std::vector<int>& m, std::size_t N, std::size_t bs) {
    const auto n_blocks = N / bs;
    for (std::size_t col = 0; col < n_blocks; ++col) {
        block_col_shift(m, N, bs, col, col);
    }
}

void row_shift(std::vector<int>& m, std::size_t N, std::size_t bs, std::size_t shift_blocks) {
    const auto n_blocks = N / bs;
    for (std::size_t row = 0; row < n_blocks; ++row) {
        block_row_shift(m, N, bs, row, shift_blocks);
    }
}

void col_shift(std::vector<int>& m, std::size_t N, std::size_t bs, std::size_t shift_blocks) {
    const auto n_blocks = N / bs;
    for (std::size_t col = 0; col < n_blocks; ++col) {
        block_col_shift(m, N, bs, col, shift_blocks);
    }
}

void block_mul(std::vector<int>& A, std::vector<int>& B, std::vector<int>& C, std::size_t row, std::size_t col, std::size_t N, std::size_t bs) {
    const auto rb = row * bs;
    const auto cb = col * bs;
    for (std::size_t i = 0; i < bs; ++i) {
        for (std::size_t j = 0; j < bs; ++j) {
            auto row_range = A | vw::drop(N * (rb + i) + cb) | vw::take(bs);
            auto col_range = B | vw::drop(N * rb + cb + j) | vw::stride(N) | vw::take(bs);
            // spdlog::info("A_{}_{} ({}, {}): {}", row, col, i, j, fmt::join(row_range, " "));
            // spdlog::info("B_{}_{} ({}, {}): {}\n", row, col, i, j, fmt::join(col_range, " "));

            auto res = rg::fold_left_first(
                vw::zip_with(std::multiplies{}, row_range, col_range),
                std::plus{}
            );
            C[N * (rb + i) + cb + j] += res.value();
        }
    }
}

void cannon_multiply(std::vector<int> & A, std::vector<int> & B, std::vector<int> & C, std::size_t N, std::size_t block_size, std::size_t n_workers)
{
    if (N % block_size != 0) {
        spdlog::error("Matrix is not dividable into blocks: N={} block_size={}", N, block_size);
        std::terminate();
    }

    const auto n_blocks = N / block_size;

    const auto blocks_per_worker = n_blocks / n_workers;
    const auto blocks_per_worker_remainder = blocks_per_worker + n_blocks % n_workers;

    spdlog::info("Create threadpool with {} workers", n_workers);
    BS::thread_pool pool(n_workers);

    initial_row_shift(A, N, block_size);
    initial_col_shift(B, N, block_size);

    for (std::size_t i = 0; i < n_blocks; ++i) {
        std::vector<std::future<void>> futures;
        for (std::size_t worker = 0; worker < n_workers; ++worker) {
            futures.emplace_back(pool.submit_task([&, worker]{
                const auto row_start = worker * blocks_per_worker;
                const auto row_end = row_start + (worker == n_workers - 1 ? blocks_per_worker_remainder : blocks_per_worker);
                for (std::size_t row = row_start; row < row_end; ++row) {
                    for (std::size_t col = 0; col < n_blocks; ++col) {
                        block_mul(A, B, C, row, col, N, block_size);
                    }
                }
            }));
        }
        for (auto const & f : futures) {
            f.wait();
        }

        row_shift(A, N, block_size, 1);
        col_shift(B, N, block_size, 1);
    }
}

std::vector<int> cannon_matmul(std::size_t N, std::size_t n_workers) {
    auto A = deserealize(N);
    auto B = deserealize(N);
    auto C = std::vector(N*N, 0);

    spdlog::info("A = {}", serialize(A, N));
    spdlog::info("B = {}", serialize(B, N));

    std::size_t block_size = N / n_workers;

    const auto start = std::chrono::high_resolution_clock::now();
    cannon_multiply(A, B, C, N, block_size, n_workers);
    const auto finish = std::chrono::high_resolution_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    spdlog::info("C = {}", serialize(C, N));
    spdlog::info("elapsed {}ms for N={} workers={}", elapsed, N, n_workers);
    spdlog::info("physical cores = {}", std::thread::hardware_concurrency());

    return C;
}

int main(int argc, char** argv)
{
    const std::size_t n_points = std::stoull(argv[1]);
    const std::size_t n_workers = std::stoull(argv[2]);
    cannon_matmul(n_points, n_workers);
}
