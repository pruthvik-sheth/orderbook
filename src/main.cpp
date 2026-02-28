#include <iostream>
#include "../include/Order.h"
#include "../include/OrderBook.h"

int main() {
    OrderBook book;

    std::cout << "=== Setting up the book ===" << std::endl;
    book.addOrder(Order(1, Side::SELL, 101.00, 75));
    book.addOrder(Order(2, Side::SELL, 102.00, 50));
    book.printOrders();

    std::cout << "\n=== GTC Order — partial fill, rest stays ===" << std::endl;
    // Wants 200 but only 75 available at 101 — remaining 125 stays in book
    book.addOrder(Order(3, Side::BUY, 101.00, 200, OrderType::GTC));
    book.printOrders();

    std::cout << "\n=== FAK Order — partial fill, rest killed ===" << std::endl;
    // Wants 100 but only 50 available at 102 — remaining 50 gets killed
    book.addOrder(Order(4, Side::BUY, 102.00, 100, OrderType::FAK));
    book.printOrders();

    return 0;
}