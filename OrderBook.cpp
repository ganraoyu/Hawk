#include <algorithm>
#include <cmath>
#include <ctime>
#include <deque>
#include <format>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>

enum class OrderType {
  GoodTillCancel,
  FillAndKill,
};

enum class Side {
  Buy,
  Sell,
};

using Price = std::int32_t;
using Quantity = std::uint32_t;
using OrderId = std::uint64_t;

struct LevelInfo {
  Price price_;
  Quantity quantity_;
};

using LevelInfos = std::vector<LevelInfo>;

class OrderBookLevelInfos {
public:
  OrderBookLevelInfos(const LevelInfos &bids, const LevelInfos &asks)
    : bids_{ bids }, asks_{ asks } { }

    const LevelInfos &GetBids() const { return bids_; };
    const LevelInfos &GetAsks() const { return asks_; };
  
private:
  LevelInfos bids_;
  LevelInfos asks_;
};

class Order {
public:
    Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity)
      : orderType_{ orderType }, 
        orderId_{ orderId }, 
        side_{ side }, 
        price_{ price }, 
        intialQuantity_{ quantity },
        remainingQuantity_{ quantity }
      { }

      OrderId GetOrderId() const { return orderId_; }
      Side getSide() const { return side_; }
      Price GetPrice() const { return price_; }
      
      OrderType GetOrderType() const { return orderType_; }
      Quantity GetIntialQuantity() const { return intialQuantity_; }
      Quantity GetRemainingQuantity() const { return remainingQuantity_; }
private:
  OrderType orderType_;
  OrderId orderId_;
  Side side_;
  Price price_;
  Quantity intialQuantity_;
  Quantity remainingQuantity_;
};

int main() {

  return 0;
};