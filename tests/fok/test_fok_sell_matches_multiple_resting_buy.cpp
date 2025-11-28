#include <gtest/gtest.h>
#include <memory>
#include "OrderBook.h"

TEST(OrderBookTest, FOKSellMatchesMultipleRestingBuy_FullFill)
{
    OrderBook orderBook;

    // Resting BUY orders
    auto buy1 = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Buy, 105, 5);
    auto buy2 = std::make_shared<Order>(OrderType::GoodTillCancel, 2, Side::Buy, 110, 5);

    orderBook.AddOrder(buy1);
    orderBook.AddOrder(buy2);

    ASSERT_EQ(orderBook.GetBids().size(), 2);
    ASSERT_EQ(orderBook.Size(), 2);

    // Incoming SELL @105, qty 10
    auto sell = std::make_shared<Order>(OrderType::FillOrKill, 3, Side::Sell, 105, 10);
    auto trades = orderBook.AddOrder(sell);

    EXPECT_EQ(trades.size(), 2);

    // All buys removed
    EXPECT_EQ(orderBook.GetBids().size(), 0);
    EXPECT_EQ(orderBook.GetAsks().size(), 0);
    EXPECT_EQ(orderBook.Size(), 0);
}
