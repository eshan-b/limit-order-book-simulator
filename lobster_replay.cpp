#include "lobster_replay.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

LobsterReplayEngine::LobsterReplayEngine()
    : processed_messages(0), successful_operations(0),
      failed_operations(0), trades_executed(0) {}

bool LobsterReplayEngine::load_data(const std::string &filename)
{
    reset();
    bool success = parser.load_file(filename);
    if (success)
    {
        parser.print_stats();
    }
    return success;
}

void LobsterReplayEngine::reset()
{
    parser.reset();
    lobster_to_internal_id.clear();
    internal_to_lobster_id.clear();
    processed_messages = 0;
    successful_operations = 0;
    failed_operations = 0;
    trades_executed = 0;

    // Reset LOB (create new instance)
    lob = LimitOrderBook();
}

void LobsterReplayEngine::print_message_info(const LobsterMessage &msg)
{
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "[" << msg.timestamp << "s] "
              << msg.type_to_string() << " - "
              << "ID:" << msg.order_id << " "
              << "Size:" << msg.size << " "
              << "Price:$" << std::setprecision(2) << msg.price << " "
              << "Side:" << (msg.direction == 1 ? "BUY" : "SELL") << std::endl;
}

void LobsterReplayEngine::process_new_order(const LobsterMessage &msg)
{
    try
    {
        int internal_id = lob.add_limit_order(msg.get_order_side(), msg.price, msg.size);
        lobster_to_internal_id[msg.order_id] = internal_id;
        internal_to_lobster_id[internal_id] = msg.order_id;
        successful_operations++;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error processing new order: " << e.what() << std::endl;
        failed_operations++;
    }
}

void LobsterReplayEngine::process_cancellation(const LobsterMessage &msg)
{
    // Note: LOBSTER cancellations are partial deletions
    // For simplicity, we'll treat them as full cancellations
    // In a full implementation, you'd need to track partial quantities

    auto it = lobster_to_internal_id.find(msg.order_id);
    if (it != lobster_to_internal_id.end())
    {
        int internal_id = it->second;
        if (lob.cancel_order(internal_id))
        {
            lobster_to_internal_id.erase(it);
            internal_to_lobster_id.erase(internal_id);
            successful_operations++;
        }
        else
        {
            std::cerr << "Warning: Could not cancel order " << msg.order_id
                      << " (internal ID: " << internal_id << ")" << std::endl;
            failed_operations++;
        }
    }
    else
    {
        std::cerr << "Warning: Cancellation for unknown order ID " << msg.order_id << std::endl;
        failed_operations++;
    }
}

void LobsterReplayEngine::process_deletion(const LobsterMessage &msg)
{
    auto it = lobster_to_internal_id.find(msg.order_id);
    if (it != lobster_to_internal_id.end())
    {
        int internal_id = it->second;
        if (lob.cancel_order(internal_id))
        {
            lobster_to_internal_id.erase(it);
            internal_to_lobster_id.erase(internal_id);
            successful_operations++;
        }
        else
        {
            std::cerr << "Warning: Could not delete order " << msg.order_id
                      << " (internal ID: " << internal_id << ")" << std::endl;
            failed_operations++;
        }
    }
    else
    {
        std::cerr << "Warning: Deletion for unknown order ID " << msg.order_id << std::endl;
        failed_operations++;
    }
}

void LobsterReplayEngine::process_execution(const LobsterMessage &msg)
{
    // Executions in LOBSTER data represent trades that already happened
    // We don't need to execute them again in our LOB, just count them
    trades_executed++;
    successful_operations++;

    // Remove the executed order from our tracking
    auto it = lobster_to_internal_id.find(msg.order_id);
    if (it != lobster_to_internal_id.end())
    {
        int internal_id = it->second;
        // The order should already be removed/reduced by the matching engine
        lobster_to_internal_id.erase(it);
        internal_to_lobster_id.erase(internal_id);
    }
}

void LobsterReplayEngine::process_trading_halt(const LobsterMessage &msg)
{
    std::cout << "TRADING HALT at " << std::fixed << std::setprecision(6)
              << msg.timestamp << "s" << std::endl;
    successful_operations++;
}

void LobsterReplayEngine::replay_all(bool verbose, bool step_by_step)
{
    std::cout << "\nStarting LOBSTER data replay..." << std::endl;
    std::cout << "Total messages to process: " << parser.get_total_messages() << std::endl;

    if (step_by_step)
    {
        std::cout << "Step-by-step mode: Press Enter after each message..." << std::endl;
    }

    while (parser.has_next_message())
    {
        LobsterMessage msg = parser.get_next_message();
        processed_messages++;

        if (verbose)
        {
            print_message_info(msg);
        }

        switch (msg.type)
        {
        case LobsterMessageType::NEW_ORDER:
            process_new_order(msg);
            break;
        case LobsterMessageType::CANCELLATION:
            process_cancellation(msg);
            break;
        case LobsterMessageType::DELETION:
            process_deletion(msg);
            break;
        case LobsterMessageType::EXECUTION_VISIBLE:
        case LobsterMessageType::EXECUTION_HIDDEN:
            process_execution(msg);
            break;
        case LobsterMessageType::TRADING_HALT:
            process_trading_halt(msg);
            break;
        }

        if (step_by_step)
        {
            lob.print_book();
            std::cout << "Press Enter to continue...";
            std::cin.get();
        }

        // Progress indicator for large files
        if (!verbose && processed_messages % 1000 == 0)
        {
            std::cout << "Processed " << processed_messages << " messages..." << std::endl;
        }
    }

    std::cout << "\nReplay completed!" << std::endl;
    print_statistics();
    print_current_book();
}

void LobsterReplayEngine::replay_n_messages(int n, bool verbose)
{
    std::cout << "\nReplaying next " << n << " messages..." << std::endl;

    int count = 0;
    while (parser.has_next_message() && count < n)
    {
        LobsterMessage msg = parser.get_next_message();
        processed_messages++;
        count++;

        if (verbose)
        {
            print_message_info(msg);
        }

        switch (msg.type)
        {
        case LobsterMessageType::NEW_ORDER:
            process_new_order(msg);
            break;
        case LobsterMessageType::CANCELLATION:
            process_cancellation(msg);
            break;
        case LobsterMessageType::DELETION:
            process_deletion(msg);
            break;
        case LobsterMessageType::EXECUTION_VISIBLE:
        case LobsterMessageType::EXECUTION_HIDDEN:
            process_execution(msg);
            break;
        case LobsterMessageType::TRADING_HALT:
            process_trading_halt(msg);
            break;
        }
    }

    std::cout << "Processed " << count << " messages." << std::endl;
    print_current_book();
}

void LobsterReplayEngine::print_statistics() const
{
    std::cout << "\n=== REPLAY STATISTICS ===" << std::endl;
    std::cout << "Messages Processed: " << processed_messages << std::endl;
    std::cout << "Successful Operations: " << successful_operations << std::endl;
    std::cout << "Failed Operations: " << failed_operations << std::endl;
    std::cout << "Trades Executed: " << trades_executed << std::endl;
    std::cout << "Active Orders: " << lobster_to_internal_id.size() << std::endl;

    if (processed_messages > 0)
    {
        double success_rate = (double)successful_operations / processed_messages * 100.0;
        std::cout << "Success Rate: " << std::fixed << std::setprecision(1)
                  << success_rate << "%" << std::endl;
    }
    std::cout << "=========================" << std::endl;
}

void LobsterReplayEngine::print_current_book() const
{
    lob.print_book();
}