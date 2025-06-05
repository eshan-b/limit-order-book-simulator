#ifndef LOBSTER_REPLAY_H
#define LOBSTER_REPLAY_H

#include "lob.h"
#include "lobster_parser.h"
#include <unordered_map>

/**
 * @class LobsterReplayEngine
 * @brief Engine to replay and simulate LOBSTER limit order book events from historical data.
 *
 * This class loads LOBSTER-formatted order book event data, replays the events through an internal
 * LimitOrderBook, and provides statistics and utilities for analysis and debugging.
 */
class LobsterReplayEngine
{
private:
    LimitOrderBook lob; ///< Internal limit order book instance.
    LobsterParser parser; ///< Parser for LOBSTER-formatted data.

    /**
     * @brief Maps LOBSTER order IDs to internal order IDs.
     */
    std::unordered_map<int, int> lobster_to_internal_id;

    /**
     * @brief Maps internal order IDs to LOBSTER order IDs.
     */
    std::unordered_map<int, int> internal_to_lobster_id;

    // Statistics
    int processed_messages;      // Number of processed messages.
    int successful_operations;   // Number of successful operations.
    int failed_operations;       // Number of failed operations.
    int trades_executed;         // Number of trades executed.

    /**
     * @brief Processes a new order message.
     * @param msg The LOBSTER message representing a new order.
     */
    void process_new_order(const LobsterMessage &msg);

    /**
     * @brief Processes a cancellation message.
     * @param msg The LOBSTER message representing a cancellation.
     */
    void process_cancellation(const LobsterMessage &msg);

    /**
     * @brief Processes a deletion message.
     * @param msg The LOBSTER message representing a deletion.
     */
    void process_deletion(const LobsterMessage &msg);

    /**
     * @brief Processes an execution message.
     * @param msg The LOBSTER message representing an execution.
     */
    void process_execution(const LobsterMessage &msg);

    /**
     * @brief Processes a trading halt message.
     * @param msg The LOBSTER message representing a trading halt.
     */
    void process_trading_halt(const LobsterMessage &msg);

    /**
     * @brief Prints information about a LOBSTER message.
     * @param msg The message to print information about.
     */
    void print_message_info(const LobsterMessage &msg);

public:
    /**
     * @brief Constructs a new LobsterReplayEngine object.
     */
    LobsterReplayEngine();

    /**
     * @brief Loads LOBSTER data from a file.
     * @param filename The path to the LOBSTER data file.
     * @return True if the data was loaded successfully, false otherwise.
     */
    bool load_data(const std::string &filename);

    /**
     * @brief Replays all loaded messages through the limit order book.
     * @param verbose If true, prints detailed information for each message.
     * @param step_by_step If true, pauses after each message for step-by-step replay.
     */
    void replay_all(bool verbose = false, bool step_by_step = false);

    /**
     * @brief Replays the first n messages through the limit order book.
     * @param n Number of messages to replay.
     * @param verbose If true, prints detailed information for each message.
     */
    void replay_n_messages(int n, bool verbose = false);

    /**
     * @brief Resets the engine state and statistics.
     */
    void reset();

    /**
     * @brief Prints statistics about the replay session.
     */
    void print_statistics() const;

    /**
     * @brief Prints the current state of the limit order book.
     */
    void print_current_book() const;
};

#endif // LOBSTER_REPLAY_H