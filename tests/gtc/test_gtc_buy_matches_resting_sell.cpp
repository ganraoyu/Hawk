#include <gtest/gtest.h>
#include <memory>
#include "OrderBook.h"

TEST(OrderBookTest, GTCBuyMatchesRestingSell)
{
    OrderBook orderBook;

    // Resting SELL at 105
    auto sell = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Sell, 105, 5);
    orderBook.AddOrder(sell);

    ASSERT_EQ(orderBook.GetAsks().size(), 1);
    ASSERT_EQ(orderBook.Size(), 1);

    // Incoming BUY @105
    auto buy = std::make_shared<Order>(OrderType::GoodTillCancel, 2, Side::Buy, 105, 5);
    auto trades = orderBook.AddOrder(buy);

    EXPECT_EQ(trades.size(), 1);

    // Both orders fully filled
    EXPECT_EQ(orderBook.GetAsks().size(), 0);
    EXPECT_EQ(orderBook.GetBids().size(), 0);
    EXPECT_EQ(orderBook.Size(), 0);
}
