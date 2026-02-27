#pragma once
#include <string.h>

enum class Side {
    BUY,
    SELL
};

struct Order {
    int id;
    Side side;
    double price;
    int quantity;
    // This is what newbies do
    // Order (int id, Side side, double price, int quantity) {
    //     this->id = id;
    //     this->side = side;
    //     this->price = price;
    //     this->quantity = quantity;
    // }

    // This is what experienced developers do
    // This is called initializer list
    Order (int id, Side side, double price, int quantity)
        : id(id), side(side), price(price), quantity(quantity) {}
};