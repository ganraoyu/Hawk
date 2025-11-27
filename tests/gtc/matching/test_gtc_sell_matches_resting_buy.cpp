#include "../include/test_gtc_sell_matches_resting_buy.h"
#include "OrderBook.h"
#include <memory>

bool test_gtc_sell_matches_resting_buy()
{
    OrderBook orderBook;
    auto buyOrder = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Buy, 100, 2);
    orderBook.AddOrder(buyOrder);

    if (orderBook.Size() == 0 || orderBook.GetBids().size() == 0)
        return false;

    auto sellOrder = std::make_shared<Order>(OrderType::GoodTillCancel, 2, Side::Sell, 100, 2);
    auto trades = orderBook.AddOrder(sellOrder);

    bool tradeHappened = !trades.empty();
    bool orderBookEmpty = orderBook.Size() == 0;

    return tradeHappened && orderBookEmpty;
}
