#ifndef ORDER_QUEUE_H
#define ORDER_QUEUE_H

#include "order.h"
#include <queue>
#include <memory>

/**
 * @class OrderQueue
 * @brief Manages a queue of orders at a specific price level in a limit order book.
 *
 * This class maintains a queue of orders, tracks the total quantity of all orders,
 * and provides methods for adding, removing, and querying orders in the queue.
 */
class OrderQueue
{
private:
    // Queue of orders at this price level
    std::queue<std::shared_ptr<Order>> orders;

    // Total quantity of all orders in the queue
    int total_quantity;

public:
    /**
     * @brief Constructs a new OrderQueue instance.
     */
    OrderQueue();

    /**
     * @brief Adds an order to the queue.
     * @param order The order to be added.
     */
    void add_order(std::shared_ptr<Order> order);

    /**
     * @brief Retrieves the order at the front of the queue.
     * @return A shared pointer to the front order, or nullptr if the queue is empty.
     */
    std::shared_ptr<Order> front();

    /**
     * @brief Removes the order at the front of the queue.
     */
    void pop();

    /**
     * @brief Checks if the queue is empty.
     * @return True if the queue is empty, false otherwise.
     */
    bool empty() const;

    /**
     * @brief Gets the total quantity of all orders in the queue.
     * @return The total quantity of orders.
     */
    int get_total_quantity() const;

    /**
     * @brief Updates the total quantity of the queue.
     * @param new_quantity The new total quantity.
     */
    void update_quantity(int new_quantity);

    /**
     * @brief Removes an order from the queue by its ID.
     * @param order_id The unique ID of the order to remove.
     * @return True if the order was successfully removed, false otherwise.
     */
    bool remove_order(int order_id);
};

#endif // ORDER_QUEUE_H