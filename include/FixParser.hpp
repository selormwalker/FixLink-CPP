#ifndef FIX_PARSER_HPP
#define FIX_PARSER_HPP

#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace FixLink {

// Standard FIX Delimiter (SOH = Start of Header, ASCII 1)
const char SOH = '\x01';

struct FixField {
    int tag;
    std::string_view value;
};

class FixMessage {
private:
    std::string_view rawMessage;
    std::unordered_map<int, std::string_view> fields;

public:
    explicit FixMessage(std::string_view msg) : rawMessage(msg) {
        parse();
    }

    void parse() {
        size_t start = 0;
        size_t end = rawMessage.find(SOH);

        while (end != std::string_view::npos) {
            std::string_view pair = rawMessage.substr(start, end - start);
            size_t eqPos = pair.find('=');
            
            if (eqPos != std::string_view::npos) {
                int tag = std::stoi(std::string(pair.substr(0, eqPos)));
                std::string_view value = pair.substr(eqPos + 1);
                fields[tag] = value;
            }

            start = end + 1;
            end = rawMessage.find(SOH, start);
        }
    }

    std::string_view getField(int tag) const {
        auto it = fields.find(tag);
        return (it != fields.end()) ? it->second : "";
    }

    bool hasField(int tag) const {
        return fields.find(tag) != fields.end();
    }

    void debugPrint() const {
        std::cout << "FIX Message Fields:" << std::endl;
        for (const auto& [tag, value] : fields) {
            std::cout << "  Tag " << tag << " = " << value << std::endl;
        }
    }
};

} // namespace FixLink

#endif
