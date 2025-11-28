#include <gtest/gtest.h>
#include <memory>
#include "OrderBook.h"

TEST(OrderBookTest, GTCBuyMatchesMultipleRestingSell_FullFill)
{
    OrderBook orderBook;

    // Resting SELL orders
    auto sell1 = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Sell, 105, 5);
    auto sell2 = std::make_shared<Order>(OrderType::GoodTillCancel, 2, Side::Sell, 110, 5);

    orderBook.AddOrder(sell1);
    orderBook.AddOrder(sell2);

    ASSERT_EQ(orderBook.GetAsks().size(), 2);
    ASSERT_EQ(orderBook.Size(), 2);

    // Incoming BUY @110, qty 10
    // Should fill sell@105 + sell@110
    auto buy = std::make_shared<Order>(OrderType::GoodTillCancel, 3, Side::Buy, 110, 10);
    auto trades = orderBook.AddOrder(buy);

    EXPECT_EQ(trades.size(), 2);

    // All sells removed
    EXPECT_EQ(orderBook.GetAsks().size(), 0);

    // No buy left
    EXPECT_EQ(orderBook.GetBids().size(), 0);

    EXPECT_EQ(orderBook.Size(), 0);
}

TEST(OrderBookTest, GTCBuyMatchesMultipleRestingSell_PartialFill)
{
    OrderBook orderBook;

    // Resting SELL orders
    auto sell1 = std::make_shared<Order>(OrderType::GoodTillCancel, 4, Side::Sell, 105, 5);
    auto sell2 = std::make_shared<Order>(OrderType::GoodTillCancel, 5, Side::Sell, 110, 5);

    orderBook.AddOrder(sell1);
    orderBook.AddOrder(sell2);

    ASSERT_EQ(orderBook.GetAsks().size(), 2);

    // Incoming BUY @110, qty 7
    auto buy = std::make_shared<Order>(OrderType::GoodTillCancel, 6, Side::Buy, 110, 7);
    auto trades = orderBook.AddOrder(buy);

    EXPECT_EQ(trades.size(), 2);

    // Check remaining sell quantity
    EXPECT_EQ(orderBook.GetAsks().size(), 1);
    EXPECT_TRUE(orderBook.GetAsks().count(110) == 1);

    // No buy left
    EXPECT_EQ(orderBook.GetBids().size(), 0);

    EXPECT_EQ(orderBook.Size(), 1);
}
