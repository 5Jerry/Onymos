# Onymos interview: Implement a real-time Stock trading engine for matching Stock Buys with Stock Sells
- I implemented `atomic` on `quantity` in the `order` structure to handle race conditions with lock-free data structure.
- In `simulateMarket`, the for loop loops all the tickers for `matchOrder`. Since each order is related to only one ticker, time-complexity is O(n) although there are for loops in `matchOrder`.
- I encountered an error "call to implicitly-deleted copy constructor of 'Order'", which is related to using `atomic` on `quantity`. I would appreciate that you give me feedback on fixing this problem.
