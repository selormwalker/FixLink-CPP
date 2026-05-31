#ifndef FIX_PARSER_HPP
#define FIX_PARSER_HPP

#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <charconv>
#include <iomanip>

namespace FixLink {

const char SOH = '\x01';

/**
 * Fast branchless integer parsing for FIX tags.
 */
inline int fast_atoi(const char* str, const char* end) {
    int val = 0;
    while (str != end) {
        val = val * 10 + (*str++ - '0');
    }
    return val;
}

class FixMessage {
private:
    std::string_view rawMessage;
    std::unordered_map<int, std::string_view> fields;
    bool valid = true;

public:
    explicit FixMessage(std::string_view msg) : rawMessage(msg) {
        parse();
    }

    void parse() {
        size_t start = 0;
        size_t pos = 0;

        while ((pos = rawMessage.find(SOH, start)) != std::string_view::npos) {
            std::string_view pair = rawMessage.substr(start, pos - start);
            size_t eqPos = pair.find('=');
            
            if (eqPos != std::string_view::npos) {
                int tag = fast_atoi(pair.data(), pair.data() + eqPos);
                std::string_view value = pair.substr(eqPos + 1);
                fields[tag] = value;
            }
            start = pos + 1;
        }
    }

    bool validate() {
        // 1. Checksum Validation (Tag 10)
        if (!hasField(10)) return false;
        
        int expectedChecksum = std::stoi(std::string(getField(10)));
        int actualChecksum = 0;
        
        // Checksum is calculated on all chars up to Tag 10
        size_t checksumPos = rawMessage.find("10=");
        if (checksumPos == std::string_view::npos) return false;

        for (size_t i = 0; i < checksumPos; ++i) {
            actualChecksum += static_cast<unsigned char>(rawMessage[i]);
        }
        
        return (actualChecksum % 256) == expectedChecksum;
    }

    std::string_view getField(int tag) const {
        auto it = fields.find(tag);
        return (it != fields.end()) ? it->second : "";
    }

    bool hasField(int tag) const { return fields.find(tag) != fields.end(); }
};

/**
 * Zero-Copy FIX Message Builder
 * Optimized for low-latency order submission.
 */
class FixBuilder {
private:
    char* buffer;
    size_t offset = 0;
    size_t capacity;

public:
    explicit FixBuilder(char* buf, size_t cap) : buffer(buf), capacity(cap) {}

    void addField(int tag, std::string_view value) {
        // Tag
        auto [ptr1, ec1] = std::to_chars(buffer + offset, buffer + capacity, tag);
        offset = ptr1 - buffer;
        buffer[offset++] = '=';
        
        // Value
        std::memcpy(buffer + offset, value.data(), value.size());
        offset += value.size();
        buffer[offset++] = SOH;
    }

    void addInt(int tag, int value) {
        auto [ptr1, ec1] = std::to_chars(buffer + offset, buffer + capacity, tag);
        offset = ptr1 - buffer;
        buffer[offset++] = '=';

        auto [ptr2, ec2] = std::to_chars(buffer + offset, buffer + capacity, value);
        offset = ptr2 - buffer;
        buffer[offset++] = SOH;
    }

    std::string_view finalize() {
        // Add Checksum (Tag 10)
        int sum = 0;
        for (size_t i = 0; i < offset; ++i) sum += static_cast<unsigned char>(buffer[i]);
        int checksum = sum % 256;

        std::string_view tag10 = "10=";
        std::memcpy(buffer + offset, tag10.data(), 3);
        offset += 3;

        // format checksum as 3 digits (e.g. 042)
        char cs_buf[4];
        snprintf(cs_buf, 4, "%03d", checksum);
        std::memcpy(buffer + offset, cs_buf, 3);
        offset += 3;
        buffer[offset++] = SOH;

        return std::string_view(buffer, offset);
    }
};

} // namespace FixLink

#endif
