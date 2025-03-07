#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <random>

#define MAX_TICKERS 1024
#define MAX_ORDERS 2000
#define THREADS 4

struct Order {
    bool isBuy;  // true for Buy, false for Sell
    int ticker;  // Ticker ID (0 to MAX_TICKERS - 1)
    std::atomic<int> quantity;  // Make quantity atomic
    double price;  

    Order(bool buy, int t, int q, double p) 
        : isBuy(buy), ticker(t), quantity(q), price(p) {}
};

std::vector<Order> orders[MAX_TICKERS]; // Array of vectors to hold orders per ticker

void addOrder(bool isBuy, int ticker, int quantity, double price) {
    if (ticker < 0 || ticker >= MAX_TICKERS) return;
    orders[ticker].emplace_back(isBuy, ticker, quantity, price);
}

void matchOrder(int ticker) {
    if (ticker < 0 || ticker >= MAX_TICKERS) return;
    auto& orderList = orders[ticker];
    
    // Separate buy and sell orders
    std::vector<Order*> buyOrders, sellOrders;
    for (auto& order : orderList) {
        if (order.isBuy) buyOrders.push_back(&order);
        else sellOrders.push_back(&order);
    }
    
    // Sort buy orders by descending price (highest price first)
    std::sort(buyOrders.begin(), buyOrders.end(), [](Order* a, Order* b) { return a->price > b->price; });
    
    // Sort sell orders by ascending price (lowest price first)
    std::sort(sellOrders.begin(), sellOrders.end(), [](Order* a, Order* b) { return a->price < b->price; });
    
    size_t i = 0, j = 0;
    while (i < buyOrders.size() && j < sellOrders.size()) {
        if (buyOrders[i]->price >= sellOrders[j]->price) {
            int tradeQuantity = std::min(buyOrders[i]->quantity, sellOrders[j]->quantity);
            buyOrders[i]->quantity -= tradeQuantity;
            sellOrders[j]->quantity -= tradeQuantity;
            
            std::cout << "Trade Executed: Ticker " << ticker << " - "
                      << "Buy Price: " << buyOrders[i]->price << " - Sell Price: " << sellOrders[j]->price
                      << " - Quantity: " << tradeQuantity << std::endl;
            
            if (buyOrders[i]->quantity == 0) i++;
            if (sellOrders[j]->quantity == 0) j++;
        } else {
            break;
        }
    }
}

void simulateMarket() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> tickerDist(0, MAX_TICKERS - 1);
    std::uniform_int_distribution<int> quantityDist(1, 100);
    std::uniform_real_distribution<double> priceDist(10.0, 500.0);
    std::bernoulli_distribution orderType(0.5);
    
    for (int i = 0; i < MAX_ORDERS; ++i) {
        addOrder(orderType(gen), tickerDist(gen), quantityDist(gen), priceDist(gen));
    }

    for (int i = 0; i < MAX_TICKERS; ++i) {
        matchOrder(i);
    }
}

int main() {
    std::vector<std::thread> threads;

    for (int i = 0; i < THREADS; ++i) {
        threads.emplace_back(simulateMarket);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    return 0;
}
