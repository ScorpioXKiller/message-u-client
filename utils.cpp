/**
 * @file utils.cpp
 * @brief Implements utility functions for the MessageU client.
 *
 * Provides conversion between hexadecimal strings and byte vectors, and checks for valid hexadecimal strings.
 * 
 * @version 2.0
 * @author Dmitriy Gorodov
 * @id 342725405
 * @date 19/03/2025
 */

#include "utils.h"
#include <iomanip>
#include <sstream>
#include <regex>

std::vector<uint8_t> hex_string_to_bytes(const std::string& hex) 
{
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.size(); i += 2) 
    {
        std::string byteString = hex.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoul(byteString, nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

std::string bytes_to_hex_string(const std::vector<uint8_t>& bytes)
{
	std::ostringstream oss;
    for (auto b : bytes)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    return oss.str();
}

bool is_valid_hex(const std::string& hex) 
{
    return std::all_of(hex.begin(), hex.end(), ::isxdigit);
}