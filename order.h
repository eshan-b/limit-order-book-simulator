#ifndef ORDER_H
#define ORDER_H

/**
 * @enum OrderSide
 * @brief Represents the side of an order in the limit order book.
 */
enum class OrderSide
{
    BUY, /** A buy order. */
    SELL /** A sell order. */
};

/**
 * @enum OrderType
 * @brief Represents the type of an order in the limit order book.
 */
enum class OrderType
{
    LIMIT, /** A limit order with a specified price. */
    MARKET /** A market order executed at the best available price. */
};

/**
 * @struct Order
 * @brief Represents an order in the limit order book.
 *
 * This struct holds the details of an order, including its ID, side, type, price,
 * quantity, and timestamp.
 */
struct Order
{
    int id;              /** Unique identifier for the order. */
    OrderSide side;      /** The side of the order (buy or sell). */
    OrderType type;      /** The type of the order (limit or market). */
    double price;        /** The price of the order (for limit orders). */
    int quantity;        /** The quantity of the order. */
    long long timestamp; /** The timestamp when the order was created. */

    /**
     * @brief Constructs a new Order instance.
     * @param id The unique identifier for the order.
     * @param side The side of the order (buy or sell).
     * @param type The type of the order (limit or market).
     * @param price The price of the order (for limit orders).
     * @param quantity The quantity of the order.
     * @param timestamp The timestamp when the order was created.
     */
    Order(int id, OrderSide side, OrderType type, double price,
          int quantity, long long timestamp);
};

#endif // ORDER_H