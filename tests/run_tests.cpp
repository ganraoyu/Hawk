#include "Timer.h"
#include "fak/include/test_fak_buy_matches_resting_sell.h"
#include "gtc/include/test_gtc_buy_matches_resting_sell.h"
#include "gtc/include/test_gtc_sell_matches_resting_buy.h"
#include <iostream>

int main()
{
    Timer timer;
    if (test_gtc_buy_matches_resting_sell())
        std::cout << "Test GTC buy resting sell passed\n";
    else
        std::cout << "Test GTC buy resting sell failed\n";

    if (test_gtc_sell_matches_resting_buy())
        std::cout << "Test GTC sell resting buy passed\n";
    else
        std::cout << "Test GTC sell resting buy failed\n";

    if (test_fak_buy_matches_resting_sell())
        std::cout << "Test FAK sell resting buy passed\n";
    else
        std::cout << "Test FAK sell resting buy failed\n";

    std::cout << "Elapsed microseconds: " << timer.elapsedMicroseconds() << "\n";
    std::cout << "Elapsed nanoseconds: " << timer.elapsedNanoseconds() << "\n";

    return 0;
}
