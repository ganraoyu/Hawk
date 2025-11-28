#include <gtest/gtest.h>
#include <memory>
#include "OrderBook.h"

TEST(OrderBookTest, FAKSellMatchesRestingBuy)
{
    OrderBook orderBook;

    // Resting BUY order at 105
    auto buy = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Buy, 105, 5);
    orderBook.AddOrder(buy);

    ASSERT_EQ(orderBook.GetBids().size(), 1);
    ASSERT_EQ(orderBook.Size(), 1);

    // Incoming SELL @105, qty 5
    auto sell = std::make_shared<Order>(OrderType::FillAndKill, 2, Side::Sell, 105, 5);
    auto trades = orderBook.AddOrder(sell);

    EXPECT_FALSE(trades.empty());
    EXPECT_EQ(trades.size(), 1);

    // Verify both orders fully filled
    EXPECT_EQ(orderBook.GetBids().size(), 0);
    EXPECT_EQ(orderBook.GetAsks().size(), 0);
    EXPECT_EQ(orderBook.Size(), 0);
}
