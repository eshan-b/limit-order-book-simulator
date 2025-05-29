#ifndef LOBSTER_PARSER_H
#define LOBSTER_PARSER_H

#include "order.h"
#include <string>
#include <vector>

enum class LobsterMessageType
{
    NEW_ORDER = 1,
    CANCELLATION = 2,
    DELETION = 3,
    EXECUTION_VISIBLE = 4,
    EXECUTION_HIDDEN = 5,
    TRADING_HALT = 7
};

struct LobsterMessage
{
    double timestamp;        // Seconds after midnight
    LobsterMessageType type; // Message type
    int order_id;            // Unique order ID
    int size;                // Number of shares
    double price;            // Price (converted from 10000x format)
    int direction;           // 1 = buy, -1 = sell

    LobsterMessage(double timestamp, LobsterMessageType type, int order_id,
                   int size, double price, int direction);

    OrderSide get_order_side() const;
    std::string type_to_string() const;
};

class LobsterParser
{
private:
    std::vector<LobsterMessage> messages;
    size_t current_index;

    LobsterMessage parse_line(const std::string &line);
    double convert_price(int price_raw);

public:
    LobsterParser();

    bool load_file(const std::string &filename);
    void reset();
    bool has_next_message() const;
    LobsterMessage get_next_message();
    size_t get_total_messages() const;
    size_t get_current_index() const;
    void print_stats() const;
};

#endif // LOBSTER_PARSER_H