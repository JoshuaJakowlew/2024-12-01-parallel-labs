#include <thread>
#include <random>
#include <chrono>
#include <fstream>
#include <span>
#include <algorithm>
#include <filesystem>
#include <map>
#include <queue>
#include <mutex>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/std.h>

#include <BS_thread_pool.hpp>

#include <nlohmann/json.hpp>

using nlohmann::json;

void bfs(json const & root, std::size_t n_workers)
{
    std::mutex mtx;

    BS::thread_pool pool(n_workers);

    struct level
    {
        int mul_value;
        int elapsed_mcs;
    };

    std::vector<level> res;
    std::queue<json> current_lvl;
    current_lvl.push(root);

    std::size_t lvl_id = 0;

    while (!current_lvl.empty()) {
        std::queue<json> next_lvl;
        std::vector<std::future<int>> futures;

        const auto start = std::chrono::high_resolution_clock::now();

        while (!current_lvl.empty()) {
            auto node = current_lvl.front();
            current_lvl.pop();

            futures.emplace_back(pool.submit_task([&, node]{
                int value = node["value"];

                if (node.contains("left") && !node["left"].empty()) {
                    std::unique_lock lock{mtx};
                    next_lvl.push(node["left"]);
                }
                if (node.contains("right") && !node["right"].empty()) {
                    std::unique_lock lock{mtx};
                    next_lvl.push(node["right"]);
                }

                return value;
            }));
        }

        int lvl_product = 1;
        for (auto & f : futures) {
            lvl_product *= f.get();
        }

        const auto finish = std::chrono::high_resolution_clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();

        spdlog::info("elapsed {}mcs for lvl {}", elapsed, lvl_id);
        spdlog::info("product: {}", lvl_product);

        current_lvl = std::move(next_lvl);
        lvl_id++;
    }
}

int main(int argc, char** argv)
{
    const std::string path = argv[1];
    const std::size_t n_workers = std::stoull(argv[2]);

    std::ifstream file{path};
    auto data = json::parse(file);

    bfs(data, n_workers);
}
