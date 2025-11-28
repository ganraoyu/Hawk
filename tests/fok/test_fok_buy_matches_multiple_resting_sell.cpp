#include <gtest/gtest.h>
#include <memory>
#include "OrderBook.h"

TEST(OrderBookTest, FOKBuyMatchesMultipleRestingSell_FullFill)
{
    OrderBook orderBook;

    // Resting SELL orders
    auto sell1 = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Sell, 109, 5);
    auto sell2 = std::make_shared<Order>(OrderType::GoodTillCancel, 2, Side::Sell, 110, 5);

    orderBook.AddOrder(sell1);
    orderBook.AddOrder(sell2);

    ASSERT_EQ(orderBook.GetAsks().size(), 2);
    ASSERT_EQ(orderBook.Size(), 2);

    // Incoming BUY @110, qty 10
    auto buy = std::make_shared<Order>(OrderType::FillOrKill, 3, Side::Buy, 110, 10);
    auto trades = orderBook.AddOrder(buy);

    // Both sell orders fully matched
    EXPECT_EQ(trades.size(), 2);  

    // All sells removed
    EXPECT_EQ(orderBook.GetAsks().size(), 0);

    // No buy left because it filled exactly
    EXPECT_EQ(orderBook.GetBids().size(), 0);

    EXPECT_EQ(orderBook.Size(), 0);
}
