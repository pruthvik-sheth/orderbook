#include <iostream>
#include <chrono>
#include <random>
#include "../include/Order.h"
#include "../include/OrderBook.h"


int main() {
    OrderBook book;

    std::mt19937 rng(42);
    std::uniform_real_distribution<double> priceDist(99.0, 101.0);
    std::uniform_int_distribution<int> qtyDist(1, 100);

    int totalOrders = 100000;
    int halfOrders = totalOrders / 2;

    std::cout << "Starting benchmark with " << totalOrders << " orders..." << std::endl;

    book.setSilent(true);
    // Start timing
    auto start = std::chrono::high_resolution_clock::now();

    // Adding half buy orders
    for (int i = 1; i <= halfOrders; i++) {
        double price = (int) (priceDist(rng) * 100) / 100.0;
        int qty = qtyDist(rng);
        book.addOrder(Order(i, Side::BUY, price, qty));
    }

    // Adding half sell orders
    for (int i = 1; i <= halfOrders; i++) {
        double price = (int) (priceDist(rng) * 100) / 100.0;
        int qty = qtyDist(rng);
        book.addOrder(Order(i, Side::SELL, price, qty));
    }

    auto end = std::chrono::high_resolution_clock::now();

    auto nanoseconds  = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    auto milliseconds = nanoseconds / 1000000.0;
    double ordersPerSecond = (totalOrders / (nanoseconds / 1000000000.0));

    std::cout << "\n─── Benchmark Results ───────────────────────" << std::endl;
    std::cout << "Total orders processed : " << totalOrders          << std::endl;
    std::cout << "Total time             : " << milliseconds << " ms"       << std::endl;
    std::cout << "Orders per second      : " << (int)ordersPerSecond        << std::endl;
    std::cout << "Avg time per order     : " << (nanoseconds / totalOrders) << " nanoseconds" << std::endl;
    std::cout << "─────────────────────────────────────────────" << std::endl;

    return 0;
}