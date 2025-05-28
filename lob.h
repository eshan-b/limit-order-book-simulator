#ifndef LOB_H
#define LOB_H

#include "order.h"
#include "order_queue.h"
#include <map>
#include <unordered_map>
#include <memory>

/**
 * @class LimitOrderBook
 * @brief Manages a limit order book for matching buy and sell orders.
 *
 * This class implements a limit order book (LOB) that maintains bid and ask price levels,
 * tracks orders by ID, and processes limit and market orders. It supports adding, canceling,
 * and matching orders, as well as printing the current state of the book.
 */
class LimitOrderBook
{
private:
    // Price level maps: price -> OrderQueue
    // Buy orders (descending price)
    std::map<double, OrderQueue> bid_levels;
    // Sell orders (ascending price)
    std::map<double, OrderQueue, std::greater<double>> ask_levels;

    // Order ID tracking
    std::unordered_map<int, std::pair<double, OrderSide>> order_locations;

    int next_order_id;

    /**
     * @brief Retrieves the current timestamp.
     * @return The current timestamp as a long long value.
     */
    long long get_timestamp();

    /**
     * @brief Executes a trade between an aggressive and a passive order.
     * @param aggressive_order The order initiating the trade.
     * @param passive_order The order being matched against.
     * @param trade_quantity The quantity to be traded.
     */
    void execute_trade(std::shared_ptr<Order> aggressive_order,
                       std::shared_ptr<Order> passive_order,
                       int trade_quantity);

    /**
     * @brief Matches a market order against available limit orders.
     * @param market_order The market order to be matched.
     */
    void match_market_order(std::shared_ptr<Order> market_order);

    /**
     * @brief Matches a limit order against available orders in the book.
     * @param limit_order The limit order to be matched.
     */
    void match_limit_order(std::shared_ptr<Order> limit_order);

public:
    /**
     * @brief Constructs a new LimitOrderBook instance.
     */
    LimitOrderBook();

    /**
     * @brief Adds a limit order to the book.
     * @param side The side of the order (buy or sell).
     * @param price The limit price of the order.
     * @param quantity The quantity of the order.
     * @return The unique order ID assigned to the new order.
     */
    int add_limit_order(OrderSide side, double price, int quantity);

    /**
     * @brief Adds a market order to the book.
     * @param side The side of the order (buy or sell).
     * @param quantity The quantity of the order.
     */
    void add_market_order(OrderSide side, int quantity);

    /**
     * @brief Cancels an order from the book.
     * @param order_id The unique ID of the order to cancel.
     * @return True if the order was successfully canceled, false otherwise.
     */
    bool cancel_order(int order_id);

    /**
     * @brief Prints the current state of the order book.
     */
    void print_book() const;
};

#endif // LOB_H