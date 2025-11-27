#pragma once

#include "LevelInfo.h"
#include "Order.h"
#include "OrderBookLevelInfos.h"
#include "OrderModify.h"
#include "Trade.h"
#include <list>
#include <map>
#include <memory>
#include <ostream>
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
    bool CanMatchMarket(Side side) const;
    bool CanFillFOK(OrderPointer incomingOrder, const OrderPointers &counterList) const;
    Quantity GetAvailableLiquidity(Side side, Price price, Quantity needed) const;
    Trades MatchOrder(OrderPointer orderPointer);
    
  public:
    friend std::ostream &operator<<(std::ostream &os, const OrderBook &ob);

    const auto &GetBids() const { return bids_; }
    const auto &GetAsks() const { return asks_; }

    Trades AddOrder(OrderPointer orderPointer);
    void CancelOrder(OrderId orderId);
    Trades MatchOrder(OrderModify mod);
    std::size_t Size() const;
    OrderBookLevelInfos GetOrderInfo() const;
};

template <typename CounterMap, typename OrderMap, typename PriceComparator>
Trades MatchAgainstBook(OrderPointer incomingOrder, CounterMap &counterBook, OrderMap &orders,
                        PriceComparator comparator)
{
    Trades trades;
    auto incomingId = incomingOrder->GetOrderId();
    auto incomingPrice = incomingOrder->GetPrice();

    for (auto iterator = counterBook.begin(); 
        iterator != counterBook.end() &&
        comparator(iterator->first, incomingPrice) && !incomingOrder->IsFilled();)
    {
        auto &counterList = iterator->second;

        while (!counterList.empty() && !incomingOrder->IsFilled())
        {
            auto &counterOrder = counterList.front();

            Quantity fillQuantity = std::min(incomingOrder->GetRemainingQuantity(),
                                             counterOrder->GetRemainingQuantity());

            incomingOrder->Fill(fillQuantity);
            counterOrder->Fill(fillQuantity);

            trades.push_back(Trade{
                TradeInfo{incomingId, incomingPrice, fillQuantity},
                TradeInfo{counterOrder->GetOrderId(), counterOrder->GetPrice(), fillQuantity}});
            
                
            if (counterOrder->IsFilled())
            {
                orders.erase(counterOrder->GetOrderId());
                counterList.pop_front();                
            }
        }

        if (counterList.empty())
            iterator = counterBook.erase(iterator);
        else
            ++iterator;
    }

    return trades;
}

std::ostream &operator<<(std::ostream &os, const OrderBook &ob);
