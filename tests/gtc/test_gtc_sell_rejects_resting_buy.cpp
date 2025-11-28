#include <gtest/gtest.h>
#include <memory>
#include "OrderBook.h"

TEST(OrderBookTest, GTCSellRejectsRestingBuy_PriceTooHigh)
{
    OrderBook orderBook;

    // Resting BUY at 105
    auto buy = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Buy, 105, 5);
    orderBook.AddOrder(buy);

    ASSERT_EQ(orderBook.GetBids().size(), 1);
    ASSERT_EQ(orderBook.Size(), 1);

    // Incoming SELL @110 (above buy)
    auto sell = std::make_shared<Order>(OrderType::GoodTillCancel, 2, Side::Sell, 110, 5);
    auto trades = orderBook.AddOrder(sell);

    EXPECT_TRUE(trades.empty());

    // Orders remain in book
    EXPECT_EQ(orderBook.GetBids().size(), 1);
    EXPECT_EQ(orderBook.GetAsks().size(), 1);
    EXPECT_EQ(orderBook.Size(), 2);
}
