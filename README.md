# Limit Order Book (LOB) Simulator

A high-performance, educational limit order book simulator written in C++17. This project implements a realistic order matching engine that demonstrates how modern financial exchanges process buy and sell orders.

*Note: This is a single-order LOB.*

## Features

- **Price-Time Priority Matching**: Orders are matched based on price priority first, then time priority (FIFO) for orders at the same price level

- **Multiple Order Types**: Support for both limit orders and market orders

- **Real-time Order Management**: Add, cancel, and track orders with unique IDs

- **Partial Fills**: Orders can be partially executed when insufficient liquidity exists

- **Interactive CLI**: User-friendly command-line interface for testing and learning

- **Live Book Display**: Real-time view of best bid/ask prices and market spread

## Prerequisites

- **C++ Compiler**: GCC 7+ or Clang 5+ with C++17 support

- **Make**: For building the project

- **Operating System**: Linux, macOS, or Windows (with WSL/MinGW)

## Building and Running

### Quick Start

```bash
# Clone or download the project files
cd lob_simulator

# Build and run the project
./build.bash

# Run the project
./lob_simulator
```

## Usage

### Interactive Commands

Once the simulator is running, you can use these commands:

#### Limit Orders
```bash
limit buy <price> <quantity>    # Add buy limit order
limit sell <price> <quantity>   # Add sell limit order

# Examples:
limit buy 100.50 10    # Buy 10 shares at $100.50
limit sell 101.00 5    # Sell 5 shares at $101.00
```

#### Market Orders
```bash
market buy <quantity>     # Execute market buy order
market sell <quantity>    # Execute market sell order

# Examples:
market buy 3     # Buy 3 shares at best available price
market sell 7    # Sell 7 shares at best available price
```

#### Order Management
```bash
cancel <order_id>    # Cancel specific order
print               # Display current book state
help                # Show command help
exit                # Exit simulator
```

### Example Session

```bash
lob> limit buy 100 10
Limit order added with ID: 1
=== ORDER BOOK ===
Best Ask: No asks available
Best Bid: $100.00 (10 shares)
==================

lob> limit sell 101 5
Limit order added with ID: 2
=== ORDER BOOK ===
Best Ask: $101.00 (5 shares)
Best Bid: $100.00 (10 shares)
Spread: $1.00
==================

lob> market buy 3
TRADE: 3 shares at $101.00
=== ORDER BOOK ===
Best Ask: $101.00 (2 shares)
Best Bid: $100.00 (10 shares)
Spread: $1.00
==================

lob> cancel 1
Order 1 cancelled successfully
=== ORDER BOOK ===
Best Ask: $101.00 (2 shares)
Best Bid: No bids available
==================
```

## How It Works

### Order Matching Algorithm

1. **Limit Orders**:
    
   - First attempt to match against existing orders at better prices
  
   - Any remaining quantity is added to the appropriate side of the book
  
   - Orders at the same price level are processed FIFO

2. **Market Orders**:
   
   - Execute immediately against the best available prices
  
   - Continue matching until fully filled or no more liquidity exists
  
   - Warn if partially filled due to insufficient liquidity

3. **Price Levels**:
   
   - Bids stored in descending price order (highest first)
  
   - Asks stored in ascending price order (lowest first)
  
   - Empty price levels are automatically removed

### Data Structures

- **Order**: Contains ID, side (buy/sell), type, price, quantity, and timestamp

- **Order Queue**: FIFO queue managing orders at each price level

- **Limit Order Book**: Core engine using STL maps for efficient price level management

## Technical Details

### Performance Characteristics

- **Order Addition**: $O(\log(n))$ where $n$ is number of price levels

- **Order Cancellation**: $O(m)$ where $m$ is orders at a price level

- **Market Order Execution**: $O(k\log(n))$ where $k$ is orders consumed

## License

This project has a proprietary [license](LICENSE).

## Disclaimer

This project is for demonstration purposes only. It is not financial advice.

The author is not responsible for any losses or damages that may result from using this simulation or the code provided.

Use it at your own risk.