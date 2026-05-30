#include <iostream>
#include <chrono>
#include <string>
#include "FixParser.hpp"

int main() {
    std::cout << "FixLink-CPP: High-Performance FIX Engine" << std::endl;

    // Example FIX NewOrderSingle (35=D) message
    // 8=FIX.4.4 | 9=122 | 35=D | 34=215 | 49=CLIENT | 52=20260530-19:00:00 | 56=SERVER | 11=ORDER123 | 21=1 | 55=BTCUSD | 54=1 | 38=100 | 40=2 | 44=74000.50 | 10=171
    std::string rawFix = "8=FIX.4.4\x01" "9=122\x01" "35=D\x01" "34=215\x01" "49=CLIENT\x01" 
                         "52=20260530-19:00:00\x01" "56=SERVER\x01" "11=ORDER123\x01" 
                         "21=1\x01" "55=BTCUSD\x01" "54=1\x01" "38=100\x01" "40=2\x01" 
                         "44=74000.50\x01" "10=171\x01";

    auto start = std::chrono::high_resolution_clock::now();

    FixLink::FixMessage msg(rawFix);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> elapsed = end - start;

    std::cout << "Parsed FIX message in " << elapsed.count() << " microseconds." << std::endl;

    if (msg.hasField(55)) {
        std::cout << "Target Symbol (Tag 55): " << msg.getField(55) << std::endl;
    }
    
    std::cout << "MsgType (Tag 35): " << msg.getField(35) << std::endl;
    std::cout << "Price (Tag 44): " << msg.getField(44) << std::endl;

    std::cout << "\nFull Message Breakdown:" << std::endl;
    msg.debugPrint();

    return 0;
}
