#include "../include/test_fak_buy_matches_resting_sell.h"
#include "OrderBook.h"
#include <memory>

bool test_fak_buy_matches_resting_sell()
{
    OrderBook orderBook;
    auto sellOrder = std::make_shared<Order>(OrderType::FillOrKill, 1, Side::Sell, 100, 20);
    orderBook.AddOrder(sellOrder);

    bool orderBookAdded = orderBook.Size() > 0 && orderBook.GetAsks().size() > 0;
    if (!orderBookAdded) return false;

    auto buyOrder = std::make_shared<Order>(OrderType::FillAndKill, 2, Side::Buy, 100, 20);
    auto trades = orderBook.AddOrder(buyOrder);

    bool tradeHappened = !trades.empty();
    bool orderBookEmpty = orderBook.Size() == 0;

    return tradeHappened && orderBookEmpty;
}
