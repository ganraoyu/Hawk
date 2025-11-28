#include <gtest/gtest.h>
#include <memory>
#include "OrderBook.h"

TEST(OrderBookTest, GTCSellMatchesRestingBuy)
{
    OrderBook orderBook;

    // Resting BUY at 105
    auto buy = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Buy, 105, 5);
    orderBook.AddOrder(buy);

    ASSERT_EQ(orderBook.GetBids().size(), 1);
    ASSERT_EQ(orderBook.Size(), 1);

    // Incoming SELL @105
    auto sell = std::make_shared<Order>(OrderType::GoodTillCancel, 2, Side::Sell, 105, 5);
    auto trades = orderBook.AddOrder(sell);

    EXPECT_EQ(trades.size(), 1);

    // Both orders fully filled
    EXPECT_EQ(orderBook.GetBids().size(), 0);
    EXPECT_EQ(orderBook.GetAsks().size(), 0);
    EXPECT_EQ(orderBook.Size(), 0);
}
