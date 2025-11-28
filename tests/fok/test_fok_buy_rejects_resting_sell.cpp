#include <gtest/gtest.h>
#include <memory>
#include "OrderBook.h"

TEST(OrderBookTest, FOKBuyRejectsRestingSell_PriceTooLow)
{
    OrderBook orderBook;

    // Resting SELL order at 105
    auto sell = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Sell, 105, 5);
    orderBook.AddOrder(sell);

    ASSERT_EQ(orderBook.GetAsks().size(), 1);
    ASSERT_EQ(orderBook.Size(), 1);

    // Incoming BUY @100, qty 5 (below sell price)
    auto buy = std::make_shared<Order>(OrderType::FillOrKill, 2, Side::Buy, 100, 5);
    auto trades = orderBook.AddOrder(buy);

    // FOK order should not be added, trades empty
    EXPECT_TRUE(trades.empty());
    EXPECT_EQ(orderBook.GetBids().size(), 0);

    // Resting sell still exists
    EXPECT_EQ(orderBook.GetAsks().size(), 1);
    EXPECT_EQ(orderBook.Size(), 1);
}
