#include "order_queue.h"

OrderQueue::OrderQueue() : total_quantity(0) {}

void OrderQueue::add_order(std::shared_ptr<Order> order)
{
    orders.push(order);
    total_quantity += order->quantity;
}

std::shared_ptr<Order> OrderQueue::front()
{
    if (orders.empty())
        return nullptr;
    return orders.front();
}

void OrderQueue::pop()
{
    if (!orders.empty())
    {
        total_quantity -= orders.front()->quantity;
        orders.pop();
    }
}

bool OrderQueue::empty() const
{
    return orders.empty();
}

int OrderQueue::get_total_quantity() const
{
    return total_quantity;
}

void OrderQueue::update_quantity(int new_quantity)
{
    if (!orders.empty())
    {
        int old_quantity = orders.front()->quantity;
        orders.front()->quantity = new_quantity;
        total_quantity = total_quantity - old_quantity + new_quantity;
    }
}

bool OrderQueue::remove_order(int order_id)
{
    std::queue<std::shared_ptr<Order>> temp_queue;
    bool found = false;

    while (!orders.empty())
    {
        auto order = orders.front();
        orders.pop();

        if (order->id == order_id)
        {
            total_quantity -= order->quantity;
            found = true;
        }
        else
        {
            temp_queue.push(order);
        }
    }

    orders = temp_queue;
    return found;
}