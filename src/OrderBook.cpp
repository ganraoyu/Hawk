#include "OrderBook.h"
#include <algorithm>
#include <numeric>
#include <ostream>

bool OrderBook::CanMatch(Side side, Price price) const
{
    if (side == Side::Buy)
    {
        if (asks_.empty())
            return false;
        return price >= asks_.begin()->first; // top ask
    }
    else
    {
        if (bids_.empty())
            return false;
        return price <= bids_.begin()->first; // top bid
    }
}

bool OrderBook::CanMatchMarket(Side side) const
{
    return (side == Side::Buy) ? !asks_.empty() : !bids_.empty();
}

bool OrderBook::CanFillFOK(OrderPointer incomingOrder, const OrderPointers &counterList) const
{
    Quantity totalAvailable{0};
    for (auto &counterOrder : counterList)
    {
        totalAvailable += counterOrder->GetRemainingQuantity();
        if (totalAvailable >= incomingOrder->GetRemainingQuantity())
            break;
    }
    return totalAvailable >= incomingOrder->GetRemainingQuantity();
}

Quantity OrderBook::GetAvailableLiquidity(Side side, Price price, Quantity needed) const
{
    Quantity totalQuantity{0};

    if (side == Side::Buy)
    {
        for (const auto &[askPrice, orderList] : asks_)
        {
            if (askPrice > price)
                break;

            for (const auto &order : orderList)
            {
                totalQuantity += order->GetRemainingQuantity();

                if (totalQuantity >= needed)
                    break;
            }
        }
    }
    else
    {
        for (const auto &[bidPrice, orderList] : bids_)
        {
            if (bidPrice > price)
                break;
            
            for (const auto &order : orderList)
            {
                totalQuantity += order->GetRemainingQuantity();

                if (totalQuantity >= needed)
                    break;
            }
        }
    }

    return totalQuantity;
}

Trades OrderBook::MatchOrder(OrderPointer incomingOrder)
{
    if (incomingOrder->GetSide() == Side::Buy)
    {
        return MatchAgainstBook(incomingOrder, asks_, orders_, [](Price askPrice, Price buyPrice)
                                { return askPrice <= buyPrice; });
    }
    else
    {
        return MatchAgainstBook(incomingOrder, bids_, orders_, [](Price bidPrice, Price sellPrice)
                                { return sellPrice <= bidPrice; });
    }
}

Trades OrderBook::AddOrder(OrderPointer order)
{
    if (!order || orders_.count(order->GetOrderId()))
        return {};
    
    if (order->GetOrderType() == OrderType::FillOrKill)
    {

        auto QuantityAvailable = GetAvailableLiquidity(
            order->GetSide(),
            order->GetPrice(),
            order->GetInitialQuantity()
        );

        if (QuantityAvailable < order->GetInitialQuantity())
            return {};
    }

    if (order->GetOrderType() == OrderType::FillAndKill)
    {
        const OrderPointers* counterListPtr = nullptr;
        
        if (order->GetSide() == Side::Buy)
        {
            if (!asks_.empty())
            {
                auto iterator = asks_.begin(); 
                if (order->GetPrice() >= iterator->first) 
                {
                    counterListPtr = &(iterator->second);
                }
            }
        }
        else
        {
            if (!bids_.empty())
            {
                auto iterator = bids_.begin();
                if (order->GetPrice() <= iterator->first)
                {
                    counterListPtr = &(iterator->second);
                }
            }
        }
        
        if (!counterListPtr || !CanFillFOK(order, *counterListPtr))
        {
            return {}; 
        }
    }
    
    auto &levelList =
        (order->GetSide() == Side::Buy) ? bids_[order->GetPrice()] : asks_[order->GetPrice()];
    levelList.push_back(order);
    orders_[order->GetOrderId()] = {order, std::prev(levelList.end())};
    
    Trades trades = MatchOrder(order);
    
    if (order->IsFilled() || order->GetOrderType() == OrderType::FillAndKill)
    {
        auto iterator = orders_.find(order->GetOrderId());
        if (iterator != orders_.end())
        {
            levelList.erase(iterator->second.location_);
            orders_.erase(iterator);
        }
    }
    
    return trades;
}
void OrderBook::CancelOrder(OrderId orderId)
{
    auto iterator = orders_.find(orderId);
    if (iterator == orders_.end())
        return;

    auto orderPointer = iterator->second.order_;
    auto listIterator = iterator->second.location_;

    orders_.erase(iterator);

    auto &levelList = (orderPointer->GetSide() == Side::Buy) ? bids_.at(orderPointer->GetPrice())
                                                             : asks_.at(orderPointer->GetPrice());
    levelList.erase(listIterator);
    if (levelList.empty())
    {
        if (orderPointer->GetSide() == Side::Buy)
            bids_.erase(orderPointer->GetPrice());
        else
            asks_.erase(orderPointer->GetPrice());
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