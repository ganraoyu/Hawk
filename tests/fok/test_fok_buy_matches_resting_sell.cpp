#include <gtest/gtest.h>
#include <memory>
#include "OrderBook.h"

TEST(OrderBookTest, FOKBuyMatchesRestingSell)
{
    OrderBook orderBook;

    // Resting SELL order at 105
    auto sell = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Sell, 105, 5);
    orderBook.AddOrder(sell);

    ASSERT_EQ(orderBook.Size(), 1);
    ASSERT_EQ(orderBook.GetAsks().size(), 1);

    // Incoming BUY @105, qty 5
    auto buy = std::make_shared<Order>(OrderType::FillOrKill, 2, Side::Buy, 105, 5);
    auto trades = orderBook.AddOrder(buy);

    EXPECT_FALSE(trades.empty());
    EXPECT_EQ(trades.size(), 1);

    // Verify both orders fully filled
    EXPECT_EQ(orderBook.Size(), 0);
    EXPECT_EQ(orderBook.GetAsks().size(), 0);
    EXPECT_EQ(orderBook.GetBids().size(), 0);
}
