// lob.cpp
#include "lob.h"
#include <chrono>
#include <iostream>
#include <iomanip>

// Order constructor implementation
Order::Order(int id, OrderSide side, OrderType type, double price, int quantity, long long timestamp)
    : id(id), side(side), type(type), price(price), quantity(quantity), timestamp(timestamp) {}

LimitOrderBook::LimitOrderBook() : next_order_id(1) {}

long long LimitOrderBook::get_timestamp()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(
               std::chrono::steady_clock::now().time_since_epoch())
        .count();
}

void LimitOrderBook::execute_trade(std::shared_ptr<Order> aggressive_order,
                                   std::shared_ptr<Order> passive_order,
                                   int trade_quantity)
{
    std::cout << "TRADE: " << trade_quantity << " shares at $"
              << std::fixed << std::setprecision(2) << passive_order->price << std::endl;
}

void LimitOrderBook::match_market_order(std::shared_ptr<Order> market_order)
{
    if (market_order->side == OrderSide::BUY)
    {
        // Market buy - match against asks
        while (market_order->quantity > 0 && !ask_levels.empty())
        {
            auto &[price, queue] = *ask_levels.begin();

            if (queue.empty())
            {
                ask_levels.erase(ask_levels.begin());
                continue;
            }

            auto passive_order = queue.front();
            int trade_quantity = std::min(market_order->quantity, passive_order->quantity);

            execute_trade(market_order, passive_order, trade_quantity);

            market_order->quantity -= trade_quantity;
            passive_order->quantity -= trade_quantity;

            if (passive_order->quantity == 0)
            {
                order_locations.erase(passive_order->id);
                queue.pop();
            }
            else
            {
                queue.update_quantity(passive_order->quantity);
            }

            if (queue.empty())
            {
                ask_levels.erase(ask_levels.begin());
            }
        }
    }
    else
    {
        // Market sell - match against bids
        while (market_order->quantity > 0 && !bid_levels.empty())
        {
            auto &[price, queue] = *bid_levels.begin();

            if (queue.empty())
            {
                bid_levels.erase(bid_levels.begin());
                continue;
            }

            auto passive_order = queue.front();
            int trade_quantity = std::min(market_order->quantity, passive_order->quantity);

            execute_trade(market_order, passive_order, trade_quantity);

            market_order->quantity -= trade_quantity;
            passive_order->quantity -= trade_quantity;

            if (passive_order->quantity == 0)
            {
                order_locations.erase(passive_order->id);
                queue.pop();
            }
            else
            {
                queue.update_quantity(passive_order->quantity);
            }

            if (queue.empty())
            {
                bid_levels.erase(bid_levels.begin());
            }
        }
    }

    if (market_order->quantity > 0)
    {
        std::cout << "WARNING: Market order partially filled. "
                  << market_order->quantity << " shares remain unfilled." << std::endl;
    }
}

void LimitOrderBook::match_limit_order(std::shared_ptr<Order> limit_order)
{
    if (limit_order->side == OrderSide::BUY)
    {
        // Buy limit order - match against asks at or below limit price
        while (limit_order->quantity > 0 && !ask_levels.empty())
        {
            auto &[price, queue] = *ask_levels.begin();

            if (price > limit_order->price)
                break; // No more matching prices

            if (queue.empty())
            {
                ask_levels.erase(ask_levels.begin());
                continue;
            }

            auto passive_order = queue.front();
            int trade_quantity = std::min(limit_order->quantity, passive_order->quantity);

            execute_trade(limit_order, passive_order, trade_quantity);

            limit_order->quantity -= trade_quantity;
            passive_order->quantity -= trade_quantity;

            if (passive_order->quantity == 0)
            {
                order_locations.erase(passive_order->id);
                queue.pop();
            }
            else
            {
                queue.update_quantity(passive_order->quantity);
            }

            if (queue.empty())
            {
                ask_levels.erase(ask_levels.begin());
            }
        }

        // Add remaining quantity to bid book
        if (limit_order->quantity > 0)
        {
            bid_levels[limit_order->price].add_order(limit_order);
            order_locations[limit_order->id] = {limit_order->price, OrderSide::BUY};
        }
    }
    else
    {
        // Sell limit order - match against bids at or above limit price
        while (limit_order->quantity > 0 && !bid_levels.empty())
        {
            auto &[price, queue] = *bid_levels.begin();

            if (price < limit_order->price)
                break; // No more matching prices

            if (queue.empty())
            {
                bid_levels.erase(bid_levels.begin());
                continue;
            }

            auto passive_order = queue.front();
            int trade_quantity = std::min(limit_order->quantity, passive_order->quantity);

            execute_trade(limit_order, passive_order, trade_quantity);

            limit_order->quantity -= trade_quantity;
            passive_order->quantity -= trade_quantity;

            if (passive_order->quantity == 0)
            {
                order_locations.erase(passive_order->id);
                queue.pop();
            }
            else
            {
                queue.update_quantity(passive_order->quantity);
            }

            if (queue.empty())
            {
                bid_levels.erase(bid_levels.begin());
            }
        }

        // Add remaining quantity to ask book
        if (limit_order->quantity > 0)
        {
            ask_levels[limit_order->price].add_order(limit_order);
            order_locations[limit_order->id] = {limit_order->price, OrderSide::SELL};
        }
    }
}

int LimitOrderBook::add_limit_order(OrderSide side, double price, int quantity)
{
    auto order = std::make_shared<Order>(next_order_id++, side, OrderType::LIMIT,
                                         price, quantity, get_timestamp());
    match_limit_order(order);
    return order->id;
}

void LimitOrderBook::add_market_order(OrderSide side, int quantity)
{
    auto order = std::make_shared<Order>(next_order_id++, side, OrderType::MARKET,
                                         0.0, quantity, get_timestamp());
    match_market_order(order);
}

bool LimitOrderBook::cancel_order(int order_id)
{
    auto it = order_locations.find(order_id);
    if (it == order_locations.end())
    {
        return false;
    }

    double price = it->second.first;
    OrderSide side = it->second.second;

    bool found = false;
    if (side == OrderSide::BUY)
    {
        auto level_it = bid_levels.find(price);
        if (level_it != bid_levels.end())
        {
            found = level_it->second.remove_order(order_id);
            if (level_it->second.empty())
            {
                bid_levels.erase(level_it);
            }
        }
    }
    else
    {
        auto level_it = ask_levels.find(price);
        if (level_it != ask_levels.end())
        {
            found = level_it->second.remove_order(order_id);
            if (level_it->second.empty())
            {
                ask_levels.erase(level_it);
            }
        }
    }

    if (found)
    {
        order_locations.erase(it);
    }

    return found;
}

void LimitOrderBook::print_book() const
{
    std::cout << "\n=== ORDER BOOK ===" << std::endl;

    if (bid_levels.empty() && ask_levels.empty())
    {
        std::cout << "Book is empty" << std::endl;
        return;
    }

    // Best ask
    if (!ask_levels.empty())
    {
        auto &[ask_price, ask_queue] = *ask_levels.begin();
        if (!ask_queue.empty())
        {
            std::cout << "Best Ask: $" << std::fixed << std::setprecision(2)
                      << ask_price << " (" << ask_queue.get_total_quantity() << " shares)" << std::endl;
        }
    }
    else
    {
        std::cout << "Best Ask: No asks available" << std::endl;
    }

    // Best bid
    if (!bid_levels.empty())
    {
        auto &[bid_price, bid_queue] = *bid_levels.begin();
        if (!bid_queue.empty())
        {
            std::cout << "Best Bid: $" << std::fixed << std::setprecision(2)
                      << bid_price << " (" << bid_queue.get_total_quantity() << " shares)" << std::endl;
        }
    }
    else
    {
        std::cout << "Best Bid: No bids available" << std::endl;
    }

    // Spread calculation
    if (!bid_levels.empty() && !ask_levels.empty())
    {
        auto &[bid_price, _] = *bid_levels.begin();
        auto &[ask_price, __] = *ask_levels.begin();
        double spread = ask_price - bid_price;
        std::cout << "Spread: $" << std::fixed << std::setprecision(2) << spread << std::endl;
    }

    std::cout << "==================" << std::endl;
}