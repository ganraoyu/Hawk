#pragma once

#include "LevelInfo.h"
#include "Order.h"
#include "OrderBookLevelInfos.h"
#include "OrderModify.h"
#include "Trade.h"
#include <list>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

class OrderBook
{
  private:
    struct OrderEntry
    {
        OrderPointer order_{nullptr};
        OrderPointers::iterator location_;
    };

    std::map<Price, OrderPointers, std::greater<Price>> bids_;
    std::map<Price, OrderPointers, std::less<Price>> asks_;
    std::unordered_map<OrderId, OrderEntry> orders_;

    bool CanMatch(Side side, Price price) const;
    bool CanMatchMarket(Side side, Price price);
    Trades MatchOrder();

  public:
    Trades AddOrder(OrderPointer order);
    void CancelOrder(OrderId orderId);
    Trades MatchOrder(OrderModify mod);
    std::size_t Size() const;
    OrderBookLevelInfos GetOrderInfo() const;
};
