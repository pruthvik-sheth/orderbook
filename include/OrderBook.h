#pragma once
#include <vector>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <map>
#include "Order.h"

class OrderBook {
private:
    // Using Smart Pointers

    //Bids (Soreted Highest to Lowest)
    std::map<double, std::vector<std::shared_ptr<Order>>, std::greater<double>> bids;

    // Asks (Sorted from Lowest to Highest)
    std::map<double, std::vector<std::shared_ptr<Order>>> asks;

    std::unordered_map<int, std::shared_ptr<Order>> orderMap;


    void matchOrders() {
        while (!bids.empty() && !asks.empty()) {

            auto& bestBidLevel = bids.begin()->second; // This will give me the vector of highestbids
            auto& bestAskLevel = asks.begin()->second;

            double highestBidPrice = bids.begin()->first;
            double lowestAskPrice = asks.begin()->first;

            // If highestBid is less than the lowestAsk then I need to break this loop
            if (highestBidPrice < lowestAskPrice) {
                break; // No match is possible from this point
            }

            std::shared_ptr<Order> bidOrder = bestBidLevel.front();
            std::shared_ptr<Order> askOrder = bestAskLevel.front();

            int tradeQty = std::min(bidOrder->quantity, askOrder->quantity);
            double tradePrice = askOrder->price;

            std::cout << "TRADE: "
                      << tradeQty << " units at $" << tradePrice
                      << " [BuyID=" << bidOrder->id 
                      << " SellID=" << askOrder->id << "]"
                      << std::endl;

            // Reduce the quantities
            bidOrder->quantity -= tradeQty;
            askOrder->quantity -= tradeQty;


            // Renove the fully filled orders from asks
            if (askOrder->quantity == 0) {
                orderMap.erase(askOrder->id);
                bestAskLevel.erase(bestAskLevel.begin());
                if (bestAskLevel.empty()) {
                    asks.erase(asks.begin());
                }
            }

            // Remove the fully filled orders from bids
            if (bidOrder->quantity == 0) {
                orderMap.erase(bidOrder->id);
                bestBidLevel.erase(bestBidLevel.begin());
                if (bestBidLevel.empty()) {
                    bids.erase(bids.begin());
                }
            }
        }
    }

public:

    // Destructor (now we dont need a destructor because we are using smart pointers)
    // ~OrderBook() {
    //     for (Order* ptr : orders) {
    //         delete ptr;
    //     }
    // }

    void addOrder (Order order) {
        
        std::shared_ptr<Order> ptr = std::make_shared<Order>(order);
        orderMap[order.id] = ptr;

        if (order.side == Side::BUY) {
            bids[order.price].push_back(ptr);
        } else {
            asks[order.price].push_back(ptr);
        }

        std::cout << "Order added: ID=" << order.id
                  << " Side=" << (order.side == Side::BUY ? "BUY" : "SELL")
                  << " Price=" << order.price
                  << " Qty=" << order.quantity
                  << std::endl;

        // Try to match after new order has arrived
        matchOrders();
    }

    bool cancelOrder (int id) {
        // Check if the ID exists in our map
        if (orderMap.count(id) == 0) {
            std::cout << "Order not found: ID=" << id << std::endl;
            return false;
        }
 
        std::shared_ptr<Order> ptr = orderMap[id];

        if (ptr->side == Side::BUY) {
            auto& level = bids[ptr->price];

            for (auto it = level.begin(); it != level.end(); it++) {
                if (*it == ptr) {
                    level.erase(it);
                    break;
                }
            }
            if (level.empty()) {
                bids.erase(ptr->price);
            }
        } else {
            auto& level = asks[ptr->price];
            for (auto it = level.begin(); it != level.end(); it++) {
                if (*it == ptr) {
                    level.erase(it);
                    break;
                }
            }
            if (level.empty()) {
                asks.erase(ptr->price);
            }
        }

        orderMap.erase(id);
        std::cout << "Order cancelled: ID=" << id << std::endl;
        return true;
    }


    void printOrders() {
        std::cout << "\n--- Order Book ---" << std::endl;

        std::cout << "ASKS:" << std::endl;
        if (asks.empty()) {
            std::cout << "  (empty)" << std::endl;
        }
        for (auto& level : asks) {
            for (auto& ptr : level.second) {
                std::cout << "  $" << level.first 
                          << " Qty=" << ptr->quantity 
                          << " ID=" << ptr->id << std::endl;
            }
        }

        std::cout << "BIDS:" << std::endl;
        if (bids.empty()) {
            std::cout << "  (empty)" << std::endl;
        }
        for (auto& level : bids) {
            for (auto& ptr : level.second) {
                std::cout << "  $" << level.first 
                          << " Qty=" << ptr->quantity 
                          << " ID=" << ptr->id << std::endl;
            }
        }
    }


    int count() {
        return orderMap.size();
    }
};