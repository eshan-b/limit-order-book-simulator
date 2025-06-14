#include "lob.h"
#include "lobster_replay.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class LOBSimulator
{
private:
    LimitOrderBook lob;
    LobsterReplayEngine replay_engine;

    std::vector<std::string> split(const std::string &str, char delimiter)
    {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;

        while (std::getline(ss, token, delimiter))
        {
            if (!token.empty())
            {
                tokens.push_back(token);
            }
        }

        return tokens;
    }

    void print_help()
    {
        std::cout << "\n=== LOB SIMULATOR COMMANDS ===" << std::endl;
        std::cout << "=== Manual Trading ===" << std::endl;
        std::cout << "limit buy <price> <quantity>   - Add buy limit order" << std::endl;
        std::cout << "limit sell <price> <quantity>  - Add sell limit order" << std::endl;
        std::cout << "market buy <quantity>          - Execute market buy order" << std::endl;
        std::cout << "market sell <quantity>         - Execute market sell order" << std::endl;
        std::cout << "cancel <order_id>              - Cancel order by ID" << std::endl;
        std::cout << "print                          - Display current book state" << std::endl;
        std::cout << "\n=== LOBSTER Data Replay ===" << std::endl;
        std::cout << "load <filename>                - Load LOBSTER CSV file" << std::endl;
        std::cout << "replay all [verbose] [step]    - Replay all messages" << std::endl;
        std::cout << "replay <n> [verbose]           - Replay next n messages" << std::endl;
        std::cout << "reset                          - Reset replay to beginning" << std::endl;
        std::cout << "stats                          - Show replay statistics" << std::endl;
        std::cout << "\n=== General ===" << std::endl;
        std::cout << "help                           - Show this help message" << std::endl;
        std::cout << "exit                           - Exit simulator" << std::endl;
        std::cout << "===============================" << std::endl;
    }

public:
    void run()
    {
        std::cout << "Welcome to the Limit Order Book Simulator!" << std::endl;
        std::cout << "Now with LOBSTER data replay support!" << std::endl;
        std::cout << "Type 'help' for available commands." << std::endl;

        std::string input;
        while (true)
        {
            std::cout << "\nlob> ";
            std::getline(std::cin, input);

            if (input.empty())
                continue;

            auto tokens = split(input, ' ');
            if (tokens.empty())
                continue;

            std::string command = tokens[0];

            try
            {
                if (command == "exit")
                {
                    std::cout << "Goodbye!" << std::endl;
                    break;
                }
                else if (command == "help")
                {
                    print_help();
                }
                else if (command == "print")
                {
                    lob.print_book();
                }
                else if (command == "load")
                {
                    if (tokens.size() != 2)
                    {
                        std::cout << "Usage: load <filename>" << std::endl;
                        continue;
                    }

                    std::string filename = tokens[1];
                    if (replay_engine.load_data(filename))
                    {
                        std::cout << "LOBSTER data loaded successfully!" << std::endl;
                    }
                    else
                    {
                        std::cout << "Failed to load LOBSTER data from " << filename << std::endl;
                    }
                }
                else if (command == "replay")
                {
                    if (tokens.size() < 2)
                    {
                        std::cout << "Usage: replay <all|n> [verbose] [step]" << std::endl;
                        continue;
                    }

                    std::string mode = tokens[1];
                    bool verbose = false;
                    bool step_by_step = false;

                    // Check for verbose and step flags
                    for (size_t i = 2; i < tokens.size(); i++)
                    {
                        if (tokens[i] == "verbose")
                            verbose = true;
                        if (tokens[i] == "step")
                            step_by_step = true;
                    }

                    if (mode == "all")
                    {
                        replay_engine.replay_all(verbose, step_by_step);
                    }
                    else
                    {
                        try
                        {
                            int n = std::stoi(mode);
                            if (n <= 0)
                            {
                                std::cout << "Error: Number of messages must be positive" << std::endl;
                                continue;
                            }
                            replay_engine.replay_n_messages(n, verbose);
                        }
                        catch (const std::exception &e)
                        {
                            std::cout << "Error: Invalid number of messages" << std::endl;
                        }
                    }
                }
                else if (command == "reset")
                {
                    replay_engine.reset();
                    std::cout << "Replay engine reset to beginning" << std::endl;
                }
                else if (command == "stats")
                {
                    replay_engine.print_statistics();
                }
                else if (command == "limit")
                {
                    if (tokens.size() != 4)
                    {
                        std::cout << "Usage: limit <buy|sell> <price> <quantity>" << std::endl;
                        continue;
                    }

                    std::string side_str = tokens[1];
                    double price = std::stod(tokens[2]);
                    int quantity = std::stoi(tokens[3]);

                    if (quantity <= 0)
                    {
                        std::cout << "Error: Quantity must be positive" << std::endl;
                        continue;
                    }

                    if (price <= 0)
                    {
                        std::cout << "Error: Price must be positive" << std::endl;
                        continue;
                    }

                    OrderSide side;
                    if (side_str == "buy")
                    {
                        side = OrderSide::BUY;
                    }
                    else if (side_str == "sell")
                    {
                        side = OrderSide::SELL;
                    }
                    else
                    {
                        std::cout << "Error: Side must be 'buy' or 'sell'" << std::endl;
                        continue;
                    }

                    int order_id = lob.add_limit_order(side, price, quantity);
                    std::cout << "Limit order added with ID: " << order_id << std::endl;
                    lob.print_book();
                }
                else if (command == "market")
                {
                    if (tokens.size() != 3)
                    {
                        std::cout << "Usage: market <buy|sell> <quantity>" << std::endl;
                        continue;
                    }

                    std::string side_str = tokens[1];
                    int quantity = std::stoi(tokens[2]);

                    if (quantity <= 0)
                    {
                        std::cout << "Error: Quantity must be positive" << std::endl;
                        continue;
                    }

                    OrderSide side;
                    if (side_str == "buy")
                    {
                        side = OrderSide::BUY;
                    }
                    else if (side_str == "sell")
                    {
                        side = OrderSide::SELL;
                    }
                    else
                    {
                        std::cout << "Error: Side must be 'buy' or 'sell'" << std::endl;
                        continue;
                    }

                    lob.add_market_order(side, quantity);
                    lob.print_book();
                }
                else if (command == "cancel")
                {
                    if (tokens.size() != 2)
                    {
                        std::cout << "Usage: cancel <order_id>" << std::endl;
                        continue;
                    }

                    int order_id = std::stoi(tokens[1]);
                    bool success = lob.cancel_order(order_id);

                    if (success)
                    {
                        std::cout << "Order " << order_id << " cancelled successfully" << std::endl;
                    }
                    else
                    {
                        std::cout << "Order " << order_id << " not found" << std::endl;
                    }
                    lob.print_book();
                }
                else
                {
                    std::cout << "Unknown command: " << command << std::endl;
                    std::cout << "Type 'help' for available commands." << std::endl;
                }
            }
            catch (const std::exception &e)
            {
                std::cout << "Error processing command: " << e.what() << std::endl;
            }
        }
    }
};

int main()
{
    LOBSimulator simulator;
    simulator.run();
    return 0;
}