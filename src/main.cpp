#include <cassert>
#include <iostream>
#include <memory>
#include "OrderBook.h"
#include "Timer.h"

int main()
{
    Timer timer;
    std::cout << std::boolalpha; // Print true/false for booleans
    
    std::cout << "=== Fill-Or-Kill Test Cases ===\n\n";
    
    // Test 1: FOK Buy fully matches single resting Sell
    {
        std::cout << "Test 1: FOK Buy fully matches single resting Sell\n";
        OrderBook ob;
        auto sell = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Sell, 100, 10);
        ob.AddOrder(sell);
        
        auto buy = std::make_shared<Order>(OrderType::FillOrKill, 2, Side::Buy, 100, 10);
        auto trades = ob.AddOrder(buy);
        
        std::cout << "Trade happened: " << !trades.empty() << "\n";
        std::cout << "Order book size: " << ob.Size() << "\n";
        assert(!trades.empty());
        assert(trades.size() == 1);
        assert(ob.Size() == 0); // Both fully filled
        std::cout << "✓ Passed\n\n";
    }
    
    // Test 2: FOK Buy rejected - insufficient liquidity
    {
        std::cout << "Test 2: FOK Buy rejected - insufficient liquidity\n";
        OrderBook ob;
        auto sell = std::make_shared<Order>(OrderType::GoodTillCancel, 3, Side::Sell, 100, 5);
        ob.AddOrder(sell);
        
        auto buy = std::make_shared<Order>(OrderType::FillOrKill, 4, Side::Buy, 100, 10);
        auto trades = ob.AddOrder(buy);
        
        std::cout << "Trade happened: " << !trades.empty() << "\n";
        std::cout << "Order book size: " << ob.Size() << "\n";
        assert(trades.empty()); // FOK rejected
        assert(ob.Size() == 1); // Only the sell remains
        std::cout << "✓ Passed\n\n";
    }
    
    // Test 3: FOK Buy matches across multiple price levels
    {
        std::cout << "Test 3: FOK Buy matches across multiple price levels\n";
        OrderBook ob;
        auto sell1 = std::make_shared<Order>(OrderType::GoodTillCancel, 5, Side::Sell, 100, 5);
        auto sell2 = std::make_shared<Order>(OrderType::GoodTillCancel, 6, Side::Sell, 101, 5);
        ob.AddOrder(sell1);
        ob.AddOrder(sell2);
        
        auto buy = std::make_shared<Order>(OrderType::FillOrKill, 7, Side::Buy, 101, 10);
        auto trades = ob.AddOrder(buy);
        
        std::cout << "Trade happened: " << !trades.empty() << "\n";
        std::cout << "Number of trades: " << trades.size() << "\n";
        std::cout << "Order book size: " << ob.Size() << "\n";
        assert(!trades.empty());
        assert(trades.size() == 2); // Matched against 2 orders
        assert(ob.Size() == 0); // All fully filled
        std::cout << "✓ Passed\n\n";
    }
    
    // Test 4: FOK Sell rejected - no liquidity
    {
        std::cout << "Test 4: FOK Sell rejected - no liquidity\n";
        OrderBook ob;
        
        auto sell = std::make_shared<Order>(OrderType::FillOrKill, 8, Side::Sell, 100, 10);
        auto trades = ob.AddOrder(sell);
        
        std::cout << "Trade happened: " << !trades.empty() << "\n";
        std::cout << "Order book size: " << ob.Size() << "\n";
        assert(trades.empty()); // FOK rejected
        assert(ob.Size() == 0); // Nothing in book
        std::cout << "✓ Passed\n\n";
    }
    
    // Test 5: FOK Sell matches multiple resting Buys
    {
        std::cout << "Test 5: FOK Sell matches multiple resting Buys\n";
        OrderBook ob;
        auto buy1 = std::make_shared<Order>(OrderType::GoodTillCancel, 9, Side::Buy, 100, 5);
        auto buy2 = std::make_shared<Order>(OrderType::GoodTillCancel, 10, Side::Buy, 100, 5);
        ob.AddOrder(buy1);
        ob.AddOrder(buy2);
        
        auto sell = std::make_shared<Order>(OrderType::FillOrKill, 11, Side::Sell, 100, 10);
        auto trades = ob.AddOrder(sell);
        
        std::cout << "Trade happened: " << !trades.empty() << "\n";
        std::cout << "Number of trades: " << trades.size() << "\n";
        std::cout << "Order book size: " << ob.Size() << "\n";
        assert(!trades.empty());
        assert(trades.size() == 2); // Matched against 2 orders
        assert(ob.Size() == 0); // All fully filled
        std::cout << "✓ Passed\n\n";
    }
    
    // Test 6: FOK rejected - price doesn't match
    {
        std::cout << "Test 6: FOK Buy rejected - price too low\n";
        OrderBook ob;
        auto sell = std::make_shared<Order>(OrderType::GoodTillCancel, 12, Side::Sell, 105, 10);
        ob.AddOrder(sell);
        
        auto buy = std::make_shared<Order>(OrderType::FillOrKill, 13, Side::Buy, 100, 10);
        auto trades = ob.AddOrder(buy);
        
        std::cout << "Trade happened: " << !trades.empty() << "\n";
        std::cout << "Order book size: " << ob.Size() << "\n";
        assert(trades.empty()); // FOK rejected - can't match at that price
        assert(ob.Size() == 1); // Only sell remains
        std::cout << "✓ Passed\n\n";
    }
    
    // Test 7: FOK partial fill rejected (needs exact amount)
    {
        std::cout << "Test 7: FOK rejected - can only partially fill\n";
        OrderBook ob;
        auto sell1 = std::make_shared<Order>(OrderType::GoodTillCancel, 14, Side::Sell, 100, 3);
        auto sell2 = std::make_shared<Order>(OrderType::GoodTillCancel, 15, Side::Sell, 100, 4);
        ob.AddOrder(sell1);
        ob.AddOrder(sell2);
        
        auto buy = std::make_shared<Order>(OrderType::FillOrKill, 16, Side::Buy, 100, 10);
        auto trades = ob.AddOrder(buy);
        
        std::cout << "Trade happened: " << !trades.empty() << "\n";
        std::cout << "Order book size: " << ob.Size() << "\n";
        assert(trades.empty()); // FOK rejected - only 7 available, need 10
        assert(ob.Size() == 2); // Both sells remain untouched
        std::cout << "✓ Passed\n\n";
    }
    
    std::cout << "=== All Fill-Or-Kill Tests Passed ===\n";
    std::cout << "Elapsed microseconds: " << timer.elapsedMicroseconds() << "\n";
    std::cout << "Elapsed nanoseconds: " << timer.elapsedNanoseconds() << "\n";
    std::cout << "Elapsed seconds: " << timer.elapsedMicroseconds() / 1'000'000.0 << "\n";
    
    return 0;
}