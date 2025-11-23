#pragma once

#include "Order.h"

class OrderModify
{
  public:
    OrderModify(OrderId orderId, OrderType orderType, Side side, Price price, Quantity quantity)
        : orderId_{orderId}, orderType_{orderType}, price_{price}, side_{side}, quantity_{quantity}
    {}

    OrderId GetOrderId() const { return orderId_; }
    OrderType GetOrderType() const { return orderType_; }
    Price GetPrice() const { return price_; }
    Side GetSide() const { return side_; }
    Quantity GetQuantity() const { return quantity_; }

    OrderPointer ToOrderPointer(OrderType type) const
    {
        return std::make_shared<Order>(type, GetOrderId(), GetSide(), GetPrice(), GetQuantity());
    }

  private:
    OrderId orderId_;
    OrderType orderType_;
    Price price_;
    Side side_;
    Quantity quantity_;
};
