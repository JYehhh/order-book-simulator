# 📚 Order Book Simulator

A client-server order book system designed to handle financial trading orders. 

## Purpose
A means for me to both bring together and touch up my skills on: **networking**, **software design principles** and **multithreading and concurrency**.

### Design Patterns and Principles
- **Observer Pattern 🔭**: Used for notifying users about trade executions.
- **Adapter Pattern 🔌**: Used for decoupling OrderBook logic with Server logic.
- **Factory Pattern (kind of) 🏭**: Used for creating Order objects in a extensible and decoupled way.
- **Dependency Inversion Principle (DIP) 🔄**: High-level modules do not depend on low-level modules. Both depend on abstractions.

## Installation

Before running the project, ensure you have the following installed:  
- **C++ Compiler**: Supports C++17 (e.g., `g++`, `clang++`).  
- **Boost Library**: Required for networking and asynchronous I/O.  
  ```bash
  brew install boost  # macOS
  sudo apt-get install libboost-all-dev  # Ubuntu
  ```
Clone the repository:
```bash
git clone https://github.com/your-username/order-book-simulator.git
cd order-book-simulator
```

Build the project:
```bash
make all
```

Run the server:
```bash
./server <ip> <port>
```

Run the client:
```bash
./client <server-ip> <server-port>
```

## Features  
- **Order Management**:
  - Add new LIMIT orders (BUY/SELL).
  - Cancel existing orders.
  - Modify existing orders.
- **Order Book Display**:
  - View the current state of the order book (bids and asks).
  - Display user-specific orders.
- **Trade Execution**:
  - Automatically matches BUY and SELL orders.
  - Notifies users when their trades are executed.
- **Client-Server Architecture**:
  - The server handles order processing and matching.
  - The client allows users to interact with the server via a command-line interface.



## Usage  
### Server  
The server listens for incoming connections and processes client requests. Start the server with:
```bash
./server 127.0.0.1 5000
```

### Client  
The client provides a command-line interface for interacting with the server. Supported commands include:

#### Add Order:
```bash
ADD BUY 100.50 10
```

#### Cancel Order:
```bash
CANCEL 123
```

#### Modify Order:
```bash
MODIFY 123 105.00 SELL 15
```

#### Display Order Book:
```bash
DISPLAY
```

#### Display User Orders:
```bash
ORDERS
```

## TODO  
- 

---

## Acknowledgments  
- **Boost.Asio**: For asynchronous networking.
- **nlohmann/json**: For JSON parsing and serialization.
- **Modern C++ Best Practices**: For guiding the design and implementation.
