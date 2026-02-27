#pragma once
#include <vector>
#include <unordered_map>
#include <iostream>
#include "Order.h"

class OrderBook {
private:
     // Stores all orders in insertion order
    std::vector<Order*> orders;

    // Maps order ID → position in vector for instant lookup
    // TODO: later we'll store smarter references, not just index
    std::unordered_map<int, Order*> orderMap;  // key=orderID, value=index in vector

public:

    // Destructor
    ~OrderBook() {
        for (Order* ptr : orders) {
            delete ptr;
        }
    }

    void addOrder (Order order) {
        Order* ptr = new Order(order);
        orderMap[order.id] = ptr;

        // Add to vector
        orders.push_back(ptr);

        std::cout << "Order added: ID=" << order.id
                  << " Side=" << (order.side == Side::BUY ? "BUY" : "SELL")
                  << " Price=" << order.price
                  << " Qty=" << order.quantity
                  << std::endl;
    }

    void printOrders() {
        std::cout << "\n--- Order Book ---" << std::endl;
        if (orders.empty()) {
            std::cout << "Book is empty." << std::endl;
            return;
        }
        for (Order* orderPtr : orders) {
            std::cout << "ID=" << orderPtr->id 
                      << " Side=" << (orderPtr->side == Side::BUY ? "BUY" : "SELL")
                      << " Price=" << orderPtr->price 
                      << " Qty=" << orderPtr->quantity 
                      << std::endl;
        }
    }

    bool cancelOrder (int id) {
        // Check if the ID exists in our map
        if (orderMap.count(id) == 0) {
            std::cout << "Order not found: ID=" << id << std::endl;
            return false;
        }
 
        Order* ptr = orderMap[id];

        for (auto it = orders.begin(); it != orders.end(); it++) {
            if (*it == ptr) {
                orders.erase(it);
                break;
            }
        }

        orderMap.erase(id);


        delete ptr;
        return true;
    }


    int count() {
        return orders.size();
    }
};