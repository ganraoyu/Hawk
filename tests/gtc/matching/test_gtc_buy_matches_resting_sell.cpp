#include "test_gtc_buy_matches_resting_sell.h"
#include "OrderBook.h"
#include <memory>
#include <iostream>

bool test_gtc_buy_matches_resting_sell()
{
    OrderBook orderBook;

    auto sellOrder = std::make_shared<Order>(OrderType::GoodTillCancel, 1, Side::Sell, 100, 5);
    orderBook.AddOrder(sellOrder);

    auto buyOrder = std::make_shared<Order>(OrderType::GoodTillCancel, 2, Side::Buy, 100, 5);
    auto trades = orderBook.AddOrder(buyOrder);

    bool tradeHappened = !trades.empty();
    bool orderBookEmpty = !orderBook.Size();

    std::cout << std::boolalpha; 
    std::cout << "Trade happened: " << tradeHappened << '\n';
    std::cout << "Order book empty: " << orderBookEmpty << '\n';

    return tradeHappened && orderBookEmpty;
}
