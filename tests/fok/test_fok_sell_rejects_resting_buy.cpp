#include <gtest/gtest.h>
#include <memory>
#include "OrderBook.h"

TEST(OrderBookTest, FOKSellRejectsRestingBuy_PriceTooLow)
{
    OrderBook orderBook;

    // Resting BUY order at 105
    auto buy = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Buy, 105, 5);
    orderBook.AddOrder(buy);

    ASSERT_EQ(orderBook.GetBids().size(), 1);
    ASSERT_EQ(orderBook.Size(), 1);

    // Incoming SELL @110, qty 5 (above resting buy)
    auto sell = std::make_shared<Order>(OrderType::FillOrKill, 2, Side::Sell, 110, 5);
    auto trades = orderBook.AddOrder(sell);

    EXPECT_TRUE(trades.empty());

    // Verify resting buy still exists and sell is killed
    EXPECT_EQ(orderBook.GetBids().size(), 1);
    EXPECT_EQ(orderBook.GetAsks().size(), 0);
    EXPECT_EQ(orderBook.Size(), 1);
}
