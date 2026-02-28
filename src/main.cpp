#include <iostream>
#include "../include/Order.h"
#include "../include/OrderBook.h"

int main() {
    OrderBook book;

    book.addOrder(Order(1, Side::BUY,  99.50, 100));
    book.addOrder(Order(2, Side::BUY,  99.75, 200));
    book.addOrder(Order(3, Side::SELL, 101.00, 150));
    book.addOrder(Order(4, Side::SELL, 101.25,  75));

    book.printOrders();

    // Cancel order 2
    book.cancelOrder(2);

    // Now cancel order 3 — this should work correctly now
    book.cancelOrder(3);

    // Try non-existent
    book.cancelOrder(99);

    book.printOrders();

    std::cout << "Total orders: " << book.count() << std::endl;

    return 0;
}