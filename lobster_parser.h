#ifndef LOBSTER_PARSER_H
#define LOBSTER_PARSER_H

#include "order.h"
#include <string>
#include <vector>

/**
 * @enum LobsterMessageType
 * @brief Enumerates the possible types of messages in the LOBSTER data format.
 */
enum class LobsterMessageType
{
    NEW_ORDER = 1,      // New order message
    CANCELLATION = 2,   // Order cancellation message
    DELETION = 3,       // Order deletion message
    EXECUTION_VISIBLE = 4, // Visible execution message
    EXECUTION_HIDDEN = 5,  // Hidden execution message
    TRADING_HALT = 7       // Trading halt message
};

/**
 * @struct LobsterMessage
 * @brief Represents a single message in the LOBSTER data format.
 */
struct LobsterMessage
{
    double timestamp;        // Time in seconds after midnight
    LobsterMessageType type; // Type of the message
    int order_id;            // Unique identifier for the order
    int size;                // Number of shares in the order
    double price;            // Price of the order (converted from 10000x format)
    int direction;           // Order direction (1 = buy, -1 = sell)

    /**
     * @brief Constructs a LobsterMessage with specified parameters.
     * @param timestamp Time in seconds after midnight
     * @param type Type of the message
     * @param order_id Unique identifier for the order
     * @param size Number of shares
     * @param price Price of the order
     * @param direction Order direction (1 = buy, -1 = sell)
     */
    LobsterMessage(double timestamp, LobsterMessageType type, int order_id,
                   int size, double price, int direction);

    /**
     * @brief Gets the order side (buy or sell) based on direction.
     * @return OrderSide enum value representing buy or sell
     */
    OrderSide get_order_side() const;

    /**
     * @brief Converts the message type to a string representation.
     * @return String describing the message type
     */
    std::string type_to_string() const;
};

/**
 * @class LobsterParser
 * @brief Parses and manages LOBSTER data messages from a file.
 */
class LobsterParser
{
private:
    std::vector<LobsterMessage> messages; // Container for parsed messages
    size_t current_index;                  // Current position in the message vector

    /**
     * @brief Parses a single line of LOBSTER data into a LobsterMessage.
     * @param line Input string containing the message data
     * @return Parsed LobsterMessage object
     */
    LobsterMessage parse_line(const std::string &line);

    /**
     * @brief Converts raw price (10000x format) to actual price.
     * @param price_raw Raw price value
     * @return Converted price as a double
     */
    double convert_price(int price_raw);

public:
    /**
     * @brief Constructs a new LobsterParser object.
     */
    LobsterParser();

    /**
     * @brief Loads and parses messages from a LOBSTER data file.
     * @param filename Path to the LOBSTER data file
     * @return True if file was successfully loaded and parsed, false otherwise
     */
    bool load_file(const std::string &filename);

    /**
     * @brief Resets the parser to the beginning of the message sequence.
     */
    void reset();

    /**
     * @brief Checks if there are more messages to process.
     * @return True if there are unprocessed messages, false otherwise
     */
    bool has_next_message() const;

    /**
     * @brief Retrieves the next message in the sequence.
     * @return The next LobsterMessage object
     */
    LobsterMessage get_next_message();

    /**
     * @brief Gets the total number of messages loaded.
     * @return Total number of messages
     */
    size_t get_total_messages() const;

    /**
     * @brief Gets the current index in the message sequence.
     * @return Current index
     */
    size_t get_current_index() const;

    /**
     * @brief Prints statistics about the parsed messages.
     */
    void print_stats() const;
};

#endif // LOBSTER_PARSER_H