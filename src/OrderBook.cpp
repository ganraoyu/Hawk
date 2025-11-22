#include "OrderBook.h"
#include <algorithm>
#include <numeric>

bool OrderBook::CanMatch(Side side, Price price) const {
    if (side == Side::Buy) {
        if (asks_.empty())
            return false;
        return price >= asks_.begin()->first;
    } else {
        if (bids_.empty())
            return false;
        return price <= bids_.begin()->first;
    }
}

bool OrderBook::CanMatchMarket(Side side, Price price) {
    return (side == Side::Buy) ? !asks_.empty() : !bids_.empty();
}

Trades OrderBook::MatchOrder() {
    Trades trades;

    while (!bids_.empty() && !asks_.empty()) {
        auto &bidPrice = bids_.begin()->first;
        auto &bidList = bids_.begin()->second;
        auto &askPrice = asks_.begin()->first;
        auto &askList = asks_.begin()->second;

        if (bidPrice < askPrice)
            break;

        while (!bidList.empty() && !askList.empty()) {
            auto bid = bidList.front();
            auto ask = askList.front();

            Quantity qty = std::min(bid->GetRemainingQuantity(),
                                    ask->GetRemainingQuantity());
            bid->Fill(qty);
            ask->Fill(qty);

            trades.push_back(Trade{{bid->GetOrderId(), bid->GetPrice(), qty},
                                   {ask->GetOrderId(), ask->GetPrice(), qty}});

            if (bid->IsFilled()) {
                orders_.erase(bid->GetOrderId());
                bidList.pop_front();
            }

            if (ask->IsFilled()) {
                orders_.erase(ask->GetOrderId());
                askList.pop_front();
            }
        }

        if (bidList.empty())
            bids_.erase(bids_.begin());
        if (askList.empty())
            asks_.erase(asks_.begin());
    }

    return trades;
}

Trades OrderBook::AddOrder(OrderPointer order) {
    if (!order || orders_.count(order->GetOrderId()))
        return {};

    OrderPointers &bookSide = (order->GetSide() == Side::Buy)
                                  ? bids_[order->GetPrice()]
                                  : asks_[order->GetPrice()];
    bookSide.push_back(order);

    orders_[order->GetOrderId()] = {order, std::prev(bookSide.end())};

    return MatchOrder();
}

void OrderBook::CancelOrder(OrderId orderId) {
    auto iteratorEntry = orders_.find(orderId);
    if (iteratorEntry == orders_.end())
        return;

    auto orderPtr = iteratorEntry->second.order_;
    auto listIter = iteratorEntry->second.location_;

    orders_.erase(iteratorEntry);

    if (orderPtr->GetSide() == Side::Buy) {
        auto &bidList = bids_.at(orderPtr->GetPrice());
        bidList.erase(listIter);
        if (bidList.empty())
            bids_.erase(orderPtr->GetPrice());
    } else {
        auto &askList = asks_.at(orderPtr->GetPrice());
        askList.erase(listIter);
        if (askList.empty())
            asks_.erase(orderPtr->GetPrice());
    }
}

Trades OrderBook::MatchOrder(OrderModify mod) {
    if (!orders_.count(mod.GetOrderId()))
        return {};
    CancelOrder(mod.GetOrderId());
    auto newOrder = mod.ToOrderPointer(OrderType::GoodTillCancel);
    return AddOrder(newOrder);
}

std::size_t OrderBook::Size() const {
    return orders_.size();
}

OrderBookLevelInfos OrderBook::GetOrderInfo() const {
    LevelInfos bidInfos, askInfos;

    for (const auto &[price, orders] : bids_) {
        Quantity total = 0;
        for (auto &o : orders)
            total += o->GetRemainingQuantity();
        bidInfos.push_back({price, total});
    }

    for (const auto &[price, orders] : asks_) {
        Quantity total = 0;
        for (auto &o : orders)
            total += o->GetRemainingQuantity();
        askInfos.push_back({price, total});
    }

    return {bidInfos, askInfos};
}
