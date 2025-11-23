#include <cassert>
#include <iostream>
#include <memory>

#include "OrderBook.h"

int main() {
    std::cout << "=== Good Till Cancel Test Cases ===\n\n";

    // Test 1: GTC Buy matches resting Sell
    {
        std::cout << "Test 1: GTC Buy matches resting Sell\n";
        OrderBook ob;

        auto sell = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Sell, 100, 5);
        ob.AddOrder(sell, OrderType::GoodTillCancel);

        auto buy = std::make_shared<Order>(OrderType::GoodTillCancel, 2, Side::Buy, 100, 5);
        auto trades = ob.AddOrder(buy, OrderType::GoodTillCancel);

        assert(!trades.empty());
        assert(trades.size() == 1);
        assert(ob.Size() == 0); // Both orders filled
        std::cout << "✓ Passed\n\n";
    }

    // Test 2: GTC Buy does NOT match (no resting Sell)
    {
        std::cout << "Test 2: GTC Buy rejects (no resting Sell)\n";
        OrderBook ob;

        auto buy = std::make_shared<Order>(OrderType::GoodTillCancel, 3, Side::Buy, 100, 5);
        auto trades = ob.AddOrder(buy, OrderType::GoodTillCancel);

        assert(trades.empty());
        assert(ob.Size() == 1);
        std::cout << "✓ Passed\n\n";
    }

    // Test 3: GTC Sell matches resting Buy
    {
        std::cout << "Test 3: GTC Sell matches resting Buy\n";
        OrderBook ob;

        auto buy = std::make_shared<Order>(OrderType::GoodTillCancel, 4, Side::Buy, 100, 5);
        ob.AddOrder(buy, OrderType::GoodTillCancel);

        auto sell = std::make_shared<Order>(OrderType::GoodTillCancel, 5, Side::Sell, 100, 5);
        auto trades = ob.AddOrder(sell, OrderType::GoodTillCancel);

        assert(!trades.empty());
        assert(trades.size() == 1);
        assert(ob.Size() == 0); // Both orders filled
        std::cout << "✓ Passed\n\n";
    }

    // Test 4: Cancel a resting order
    {
        std::cout << "Test 4: Cancel a resting order\n";
        OrderBook ob;

        auto buy = std::make_shared<Order>(OrderType::GoodTillCancel, 6, Side::Buy, 100, 5);
        ob.AddOrder(buy, OrderType::GoodTillCancel);

        ob.CancelOrder(6);
        assert(ob.Size() == 0);
        std::cout << "✓ Passed\n\n";
    }

    std::cout << "=== All Good Till Cancel Tests Passed ===\n";
    return 0;
}
