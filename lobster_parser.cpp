#include "lobster_parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

LobsterMessage::LobsterMessage(double timestamp, LobsterMessageType type, int order_id,
                               int size, double price, int direction)
    : timestamp(timestamp), type(type), order_id(order_id),
      size(size), price(price), direction(direction) {}

OrderSide LobsterMessage::get_order_side() const
{
    return (direction == 1) ? OrderSide::BUY : OrderSide::SELL;
}

std::string LobsterMessage::type_to_string() const
{
    switch (type)
    {
    case LobsterMessageType::NEW_ORDER:
        return "NEW_ORDER";
    case LobsterMessageType::CANCELLATION:
        return "CANCELLATION";
    case LobsterMessageType::DELETION:
        return "DELETION";
    case LobsterMessageType::EXECUTION_VISIBLE:
        return "EXECUTION_VISIBLE";
    case LobsterMessageType::EXECUTION_HIDDEN:
        return "EXECUTION_HIDDEN";
    case LobsterMessageType::TRADING_HALT:
        return "TRADING_HALT";
    default:
        return "UNKNOWN";
    }
}

LobsterParser::LobsterParser() : current_index(0) {}

double LobsterParser::convert_price(int price_raw)
{
    return static_cast<double>(price_raw) / 10000.0;
}

LobsterMessage LobsterParser::parse_line(const std::string &line)
{
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;

    // Split by comma
    while (std::getline(ss, token, ','))
    {
        tokens.push_back(token);
    }

    if (tokens.size() != 6)
    {
        throw std::runtime_error("Invalid LOBSTER message format: expected 6 columns");
    }

    double timestamp = std::stod(tokens[0]);
    int type_raw = std::stoi(tokens[1]);
    int order_id = std::stoi(tokens[2]);
    int size = std::stoi(tokens[3]);
    int price_raw = std::stoi(tokens[4]);
    int direction = std::stoi(tokens[5]);

    // Validate message type
    LobsterMessageType type;
    switch (type_raw)
    {
    case 1:
        type = LobsterMessageType::NEW_ORDER;
        break;
    case 2:
        type = LobsterMessageType::CANCELLATION;
        break;
    case 3:
        type = LobsterMessageType::DELETION;
        break;
    case 4:
        type = LobsterMessageType::EXECUTION_VISIBLE;
        break;
    case 5:
        type = LobsterMessageType::EXECUTION_HIDDEN;
        break;
    case 7:
        type = LobsterMessageType::TRADING_HALT;
        break;
    default:
        throw std::runtime_error("Unknown LOBSTER message type: " + std::to_string(type_raw));
    }

    // Validate direction
    if (direction != 1 && direction != -1)
    {
        throw std::runtime_error("Invalid direction: must be 1 or -1");
    }

    double price = convert_price(price_raw);

    return LobsterMessage(timestamp, type, order_id, size, price, direction);
}

bool LobsterParser::load_file(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }

    messages.clear();
    current_index = 0;

    std::string line;
    int line_number = 0;
    int successful_parses = 0;

    while (std::getline(file, line))
    {
        line_number++;

        // Skip empty lines
        if (line.empty())
            continue;

        try
        {
            LobsterMessage message = parse_line(line);
            messages.push_back(message);
            successful_parses++;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Warning: Failed to parse line " << line_number
                      << ": " << e.what() << std::endl;
            std::cerr << "Line content: " << line << std::endl;
        }
    }

    file.close();

    std::cout << "Loaded " << successful_parses << " messages from "
              << filename << std::endl;

    if (line_number > successful_parses)
    {
        std::cout << "Warning: " << (line_number - successful_parses)
                  << " lines could not be parsed" << std::endl;
    }

    return successful_parses > 0;
}

void LobsterParser::reset()
{
    current_index = 0;
}

bool LobsterParser::has_next_message() const
{
    return current_index < messages.size();
}

LobsterMessage LobsterParser::get_next_message()
{
    if (!has_next_message())
    {
        throw std::runtime_error("No more messages available");
    }

    return messages[current_index++];
}

size_t LobsterParser::get_total_messages() const
{
    return messages.size();
}

size_t LobsterParser::get_current_index() const
{
    return current_index;
}

void LobsterParser::print_stats() const
{
    if (messages.empty())
    {
        std::cout << "No messages loaded" << std::endl;
        return;
    }

    // Count message types
    int new_orders = 0, cancellations = 0, deletions = 0;
    int executions_visible = 0, executions_hidden = 0, trading_halts = 0;
    int buy_orders = 0, sell_orders = 0;

    double min_price = messages[0].price;
    double max_price = messages[0].price;
    double start_time = messages[0].timestamp;
    double end_time = messages[0].timestamp;

    for (const auto &msg : messages)
    {
        switch (msg.type)
        {
        case LobsterMessageType::NEW_ORDER:
            new_orders++;
            break;
        case LobsterMessageType::CANCELLATION:
            cancellations++;
            break;
        case LobsterMessageType::DELETION:
            deletions++;
            break;
        case LobsterMessageType::EXECUTION_VISIBLE:
            executions_visible++;
            break;
        case LobsterMessageType::EXECUTION_HIDDEN:
            executions_hidden++;
            break;
        case LobsterMessageType::TRADING_HALT:
            trading_halts++;
            break;
        }

        if (msg.direction == 1)
            buy_orders++;
        else
            sell_orders++;

        min_price = std::min(min_price, msg.price);
        max_price = std::max(max_price, msg.price);
        start_time = std::min(start_time, msg.timestamp);
        end_time = std::max(end_time, msg.timestamp);
    }

    std::cout << "\n=== LOBSTER DATA STATISTICS ===" << std::endl;
    std::cout << "Total Messages: " << messages.size() << std::endl;
    std::cout << "Time Range: " << std::fixed << std::setprecision(3)
              << start_time << "s - " << end_time << "s ("
              << (end_time - start_time) << "s duration)" << std::endl;
    std::cout << "Price Range: $" << std::fixed << std::setprecision(2)
              << min_price << " - $" << max_price << std::endl;

    std::cout << "\nMessage Types:" << std::endl;
    std::cout << "  New Orders: " << new_orders << std::endl;
    std::cout << "  Cancellations: " << cancellations << std::endl;
    std::cout << "  Deletions: " << deletions << std::endl;
    std::cout << "  Visible Executions: " << executions_visible << std::endl;
    std::cout << "  Hidden Executions: " << executions_hidden << std::endl;
    std::cout << "  Trading Halts: " << trading_halts << std::endl;

    std::cout << "\nOrder Directions:" << std::endl;
    std::cout << "  Buy Orders: " << buy_orders << std::endl;
    std::cout << "  Sell Orders: " << sell_orders << std::endl;
    std::cout << "===============================" << std::endl;
}