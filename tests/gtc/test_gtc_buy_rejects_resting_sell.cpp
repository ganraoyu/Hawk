#include <gtest/gtest.h>
#include <memory>
#include "OrderBook.h"

TEST(OrderBookTest, GTCBuyRejectsRestingSell_PriceTooLow)
{
    OrderBook orderBook;

    // Resting SELL at 105
    auto sell = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Sell, 105, 5);
    orderBook.AddOrder(sell);

    ASSERT_EQ(orderBook.GetAsks().size(), 1);
    ASSERT_EQ(orderBook.Size(), 1);

    // Incoming BUY @100 (below sell)
    auto buy = std::make_shared<Order>(OrderType::GoodTillCancel, 2, Side::Buy, 100, 5);
    auto trades = orderBook.AddOrder(buy);

    EXPECT_TRUE(trades.empty());

    // Both orders remain in book
    EXPECT_EQ(orderBook.GetAsks().size(), 1);
    EXPECT_EQ(orderBook.GetBids().size(), 1);
    EXPECT_EQ(orderBook.Size(), 2);
}
