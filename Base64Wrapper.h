/**
 * @file Base64Wrapper.h
 * @brief Declaration of the Base64Wrapper class for the MessageU project.
 *
 * This header file declares the Base64Wrapper class, which provides encoding and decoding
 * functionality using the Base64 algorithm.
 *
 * @version 2.0
 * @author Dmitriy Gorodov
 * @id 324725405
 * @date 19/03/2025
 */

#pragma once

#include <string>
#include <base64.h>


class Base64Wrapper
{
public:
	static std::string encode(const std::string& str);
	static std::string decode(const std::string& str);
};