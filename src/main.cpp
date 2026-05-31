#include <iostream>
#include <chrono>
#include <string>
#include "FixParser.hpp"

int main() {
    std::cout << "FixLink-CPP v2.0: Institutional FIX Engine" << std::endl;

    // 1. Demonstrate High-Speed Parsing & Validation
    std::string rawFix = "8=FIX.4.4\x01" "9=122\x01" "35=D\x01" "34=215\x01" "55=BTCUSD\x01" "10=212\x01";
    
    FixLink::FixMessage msg(rawFix);
    bool isValid = msg.validate();
    
    std::cout << "[Parser] Message Validated: " << (isValid ? "YES" : "NO") << std::endl;
    std::cout << "[Parser] Symbol: " << msg.getField(55) << std::endl;

    // 2. Demonstrate Zero-Copy Builder (No Allocations)
    char buffer[1024];
    FixLink::FixBuilder builder(buffer, 1024);
    
    auto startBuild = std::chrono::high_resolution_clock::now();
    
    builder.addField(8, "FIX.4.4");
    builder.addInt(35, 68); // 35=D
    builder.addField(55, "ETHUSD");
    builder.addInt(38, 500);
    std::string_view builtMsg = builder.finalize();
    
    auto endBuild = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> elapsedBuild = endBuild - startBuild;

    std::cout << "\n[Builder] Constructed FIX message in " << elapsedBuild.count() << " microseconds." << std::endl;
    std::cout << "[Builder] Output: " << builtMsg << std::endl;

    return 0;
}
