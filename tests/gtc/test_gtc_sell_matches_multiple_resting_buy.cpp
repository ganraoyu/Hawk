#include <gtest/gtest.h>
#include <memory>
#include "OrderBook.h"

TEST(OrderBookTest, GTCSellMatchesMultipleRestingBuy_FullFill)
{
    OrderBook orderBook;

    // Resting BUY orders
    auto buy1 = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Buy, 110, 5);
    auto buy2 = std::make_shared<Order>(OrderType::GoodTillCancel, 2, Side::Buy, 105, 5);

    orderBook.AddOrder(buy1);
    orderBook.AddOrder(buy2);

    ASSERT_EQ(orderBook.GetBids().size(), 2);

    // Incoming SELL @110, qty 10
    auto sell = std::make_shared<Order>(OrderType::GoodTillCancel, 3, Side::Sell, 110, 10);
    auto trades = orderBook.AddOrder(sell);

    EXPECT_EQ(trades.size(), 2);

    // All buys removed
    EXPECT_EQ(orderBook.GetBids().size(), 0);
    EXPECT_EQ(orderBook.GetAsks().size(), 0);
    EXPECT_EQ(orderBook.Size(), 0);
}

TEST(OrderBookTest, GTCSellMatchesMultipleRestingBuy_PartialFill)
{
    OrderBook orderBook;

    // Resting BUY orders
    auto buy1 = std::make_shared<Order>(OrderType::GoodTillCancel, 4, Side::Buy, 110, 5);
    auto buy2 = std::make_shared<Order>(OrderType::GoodTillCancel, 5, Side::Buy, 105, 5);

    orderBook.AddOrder(buy1);
    orderBook.AddOrder(buy2);

    ASSERT_EQ(orderBook.GetBids().size(), 2);

    // Incoming SELL @110, qty 7
    auto sell = std::make_shared<Order>(OrderType::GoodTillCancel, 6, Side::Sell, 110, 7);
    auto trades = orderBook.AddOrder(sell);

    EXPECT_EQ(trades.size(), 2);

    // Check remaining buy quantity
    EXPECT_EQ(orderBook.GetBids().size(), 1);
    EXPECT_TRUE(orderBook.GetBids().count(105) == 1);

    // No resting sell
    EXPECT_EQ(orderBook.GetAsks().size(), 0);
    EXPECT_EQ(orderBook.Size(), 1);
}
