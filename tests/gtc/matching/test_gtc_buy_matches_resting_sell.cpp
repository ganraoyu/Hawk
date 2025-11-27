#include "../include/test_gtc_buy_matches_resting_sell.h"
#include "OrderBook.h"
#include <memory>

bool test_gtc_buy_matches_resting_sell()
{
    OrderBook orderBook;
    auto sellOrder = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Sell, 100, 5);
    orderBook.AddOrder(sellOrder);

    if (orderBook.Size() == 0 || orderBook.GetAsks().size() == 0)
        return false;

    auto buyOrder = std::make_shared<Order>(OrderType::GoodTillCancel, 2, Side::Buy, 100, 5);
    auto trades = orderBook.AddOrder(buyOrder);

    bool tradeHappened = !trades.empty();
    bool orderBookEmpty = orderBook.Size() == 0;

    return tradeHappened && orderBookEmpty;
}
