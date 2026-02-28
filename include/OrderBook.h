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

    bool silent = true;

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

            if (!silent) std::cout << "TRADE: "
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

    void setSilent(bool s) {
        silent = s;
    }

    void addOrder (Order order) {
        
        std::shared_ptr<Order> ptr = std::make_shared<Order>(order);
        orderMap[order.id] = ptr;

        if (!silent) std::cout << "Order added: ID=" << order.id
              << " Side=" << (order.side == Side::BUY ? "BUY" : "SELL")
              << " Price=" << order.price
              << " Qty=" << order.quantity
              << " Type=" << (order.type == OrderType::GTC ? "GTC" : "FAK")
              << std::endl;

        if (order.side == Side::BUY) {
            bids[order.price].push_back(ptr);
        } else {
            asks[order.price].push_back(ptr);
        }
        // Try to match after new order has arrived
        matchOrders();

        if (order.type == OrderType::FAK && ptr->quantity > 0) {
            if (!silent) std::cout << "FAK order partially filled — killing remaining "
                  << ptr->quantity << " units of ID=" << order.id
                  << std::endl;
            cancelOrder(order.id);
        }
    }

    bool cancelOrder (int id) {
        // Check if the ID exists in our map
        if (orderMap.count(id) == 0) {
            if (!silent) std::cout << "Order not found: ID=" << id << std::endl;
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
        if (!silent) std::cout << "Order cancelled: ID=" << id << std::endl;
        return true;
    }

    // This function does not modify anyhting and hence 
    // it is marked as const for code quality
    void printOrders() const {
        if (!silent) std::cout << "\n--- Order Book ---" << std::endl;

        if (!silent) std::cout << "ASKS:" << std::endl;
        if (asks.empty()) {
            if (!silent) std::cout << "  (empty)" << std::endl;
        }
        // The reason we are doing const auto is because
        // We want to prevent unwanted bugs in this code block
        // This code block solely reads and does not modify anything
        for (const auto& level : asks) {
            for (const auto& ptr : level.second) {
                if (!silent) std::cout << "  $" << level.first 
                          << " Qty=" << ptr->quantity 
                          << " ID=" << ptr->id << std::endl;
            }
        }

        if (!silent) std::cout << "BIDS:" << std::endl;
        if (bids.empty()) {
            if (!silent) std::cout << "  (empty)" << std::endl;
        }
        for (const auto& level : bids) {
            for (const auto& ptr : level.second) {
                if (!silent) std::cout << "  $" << level.first 
                          << " Qty=" << ptr->quantity 
                          << " ID=" << ptr->id << std::endl;
            }
        }
    }


    int count() const {
        return orderMap.size();
    }
};