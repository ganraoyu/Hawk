#include <iostream>
#include "gtc/matching/test_gtc_buy_matches_resting_sell.h" 

int main() {
    if (test_gtc_buy_matches_resting_sell())
        std::cout << "Test GTC buy resting sell passed\n";
    else
        std::cout << "Test GTC buy resting sell failed\n";

    return 0;
}
