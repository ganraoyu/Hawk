#include "OrderBook.h"
#include <algorithm>
#include <numeric>

bool OrderBook::CanMatch(Side side, Price price) const
{
    if (side == Side::Buy)
    {
        if (asks_.empty())
            return false;
        return price >= asks_.begin()->first;
    }
    else
    {
        if (bids_.empty())
            return false;
        return price <= bids_.begin()->first;
    }
}

// Market order - Sell or buy at ANY price
bool OrderBook::CanMatchMarket(Side side)
{
    return (side == Side::Buy) ? !asks_.empty() : !bids_.empty();
}

Trades OrderBook::MatchOrder(OrderPointer orderPointer)
{
    Trades trades;
    auto &incomingOrder = orderPointer;
    const auto &incomingOrderType = incomingOrder->GetOrderType();

    auto price = incomingOrder->GetPrice();

    // Currently only looks at a exact price, change later on
    OrderPointers &incomingList =
        (incomingOrder->GetSide() == Side::Buy) ? bids_[price] : asks_[price];
    OrderPointers &counterList =
        (incomingOrder->GetSide() == Side::Buy) ? asks_[price] : bids_[price];

    while (!counterList.empty())
    {
        auto &counterOrder = counterList.front();

        Quantity fillQuantity =
            std::min(incomingOrder->GetRemainingQuantity(), counterOrder->GetRemainingQuantity());

        incomingOrder->Fill(fillQuantity);
        counterOrder->Fill(fillQuantity);

        if (incomingOrder->GetSide() == Side::Buy)
        {
            trades.push_back(Trade{
                TradeInfo{incomingOrder->GetOrderId(), incomingOrder->GetPrice(), fillQuantity},
                TradeInfo{counterOrder->GetOrderId(), counterOrder->GetPrice(), fillQuantity}});
        }
        else
        {
            trades.push_back(Trade{
                TradeInfo{counterOrder->GetOrderId(), counterOrder->GetPrice(), fillQuantity},
                TradeInfo{incomingOrder->GetOrderId(), incomingOrder->GetPrice(), fillQuantity}});
        }

        if (incomingOrder->IsFilled() || incomingOrderType == OrderType::FillAndKill)
        {
            auto it = orders_.find(incomingOrder->GetOrderId());
            if (it != orders_.end())
            {
                incomingList.pop_front();
                orders_.erase(it);
            }
            break;
        }

        if (counterOrder->IsFilled())
        {
            counterList.pop_front();
            orders_.erase(counterOrder->GetOrderId());
        }
    }

    return trades;
}

Trades OrderBook::AddOrder(OrderPointer order)
{
    if (!order || orders_.count(order->GetOrderId()))
        return {};

    OrderPointers &bookSide =
        (order->GetSide() == Side::Buy) ? bids_[order->GetPrice()] : asks_[order->GetPrice()];
    bookSide.push_back(order);
    orders_[order->GetOrderId()] = {order, std::prev(bookSide.end())};

    Trades trades;
    auto &incomingOrder = order;
    auto &counterList = (incomingOrder->GetSide() == Side::Buy) ? asks_[incomingOrder->GetPrice()]
                                                                : bids_[incomingOrder->GetPrice()];

    while (!counterList.empty() && !incomingOrder->IsFilled())
    {
        auto &counterOrder = counterList.front();
        Quantity fillQuantity =
            std::min(incomingOrder->GetRemainingQuantity(), counterOrder->GetRemainingQuantity());

        incomingOrder->Fill(fillQuantity);
        counterOrder->Fill(fillQuantity);

        trades.push_back(
            Trade{TradeInfo{incomingOrder->GetOrderId(), incomingOrder->GetPrice(), fillQuantity},
                  TradeInfo{counterOrder->GetOrderId(), counterOrder->GetPrice(), fillQuantity}});

        if (counterOrder->IsFilled())
        {
            orders_.erase(counterOrder->GetOrderId());
            counterList.pop_front();
        }
    }

    if (incomingOrder->IsFilled())
    {
        auto it = orders_.find(incomingOrder->GetOrderId());
        if (it != orders_.end())
        {
            bookSide.erase(it->second.location_);
            orders_.erase(it);
        }
    }

    return trades;
}

void OrderBook::CancelOrder(OrderId orderId)
{
    auto iteratorEntry = orders_.find(orderId);
    if (iteratorEntry == orders_.end())
        return;

    auto orderPtr = iteratorEntry->second.order_;
    auto listIter = iteratorEntry->second.location_;

    orders_.erase(iteratorEntry);

    if (orderPtr->GetSide() == Side::Buy)
    {
        auto &bidList = bids_.at(orderPtr->GetPrice());
        bidList.erase(listIter);
        if (bidList.empty())
            bids_.erase(orderPtr->GetPrice());
    }
    else
    {
        auto &askList = asks_.at(orderPtr->GetPrice());
        askList.erase(listIter);
        if (askList.empty())
            asks_.erase(orderPtr->GetPrice());
    }
}

Trades OrderBook::MatchOrder(OrderModify mod)
{
    if (!orders_.count(mod.GetOrderId()))
        return {};
    CancelOrder(mod.GetOrderId());
    auto newOrder = mod.ToOrderPointer(mod.GetOrderType());
    return AddOrder(newOrder);
}

std::size_t OrderBook::Size() const { return orders_.size(); }

OrderBookLevelInfos OrderBook::GetOrderInfo() const
{
    LevelInfos bidInfos, askInfos;

    for (const auto &[price, orders] : bids_)
    {
        Quantity total = 0;
        for (auto &o : orders)
            total += o->GetRemainingQuantity();
        bidInfos.push_back({price, total});
    }

    for (const auto &[price, orders] : asks_)
    {
        Quantity total = 0;
        for (auto &o : orders)
            total += o->GetRemainingQuantity();
        askInfos.push_back({price, total});
    }

    return {bidInfos, askInfos};
}
