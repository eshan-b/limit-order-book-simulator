#ifndef LOBSTER_REPLAY_H
#define LOBSTER_REPLAY_H

#include "lob.h"
#include "lobster_parser.h"
#include <unordered_map>

class LobsterReplayEngine
{
private:
    LimitOrderBook lob;
    LobsterParser parser;

    // Map LOBSTER order IDs to our internal order IDs
    std::unordered_map<int, int> lobster_to_internal_id;
    std::unordered_map<int, int> internal_to_lobster_id;

    // Statistics
    int processed_messages;
    int successful_operations;
    int failed_operations;
    int trades_executed;

    void process_new_order(const LobsterMessage &msg);
    void process_cancellation(const LobsterMessage &msg);
    void process_deletion(const LobsterMessage &msg);
    void process_execution(const LobsterMessage &msg);
    void process_trading_halt(const LobsterMessage &msg);

    void print_message_info(const LobsterMessage &msg);

public:
    LobsterReplayEngine();

    bool load_data(const std::string &filename);
    void replay_all(bool verbose = false, bool step_by_step = false);
    void replay_n_messages(int n, bool verbose = false);
    void reset();
    void print_statistics() const;
    void print_current_book() const;
};

#endif // LOBSTER_REPLAY_H