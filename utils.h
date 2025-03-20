/**
 * @file utils.h
 * @brief Utility functions and constants for the MessageU client.
 *
 * Contains functions for converting between hexadecimal strings and byte vectors,
 * as well as various protocol constants.
 * 
 * @version 2.0
 * @author Dmitriy Gorodov
 * @id 342725405
 * @date 19/03/2025
 */

#pragma once

#include <vector>
#include <string>

const uint8_t CLIENT_VERSION = 2;
const uint8_t MAX_CLIENT_NAME_SIZE = 255;
const uint8_t MAX_CLIENT_ID_SIZE = 16;
const uint8_t MAX_MESSAGE_ID_BYTES = 4;
const uint8_t MAX_MESSAGE_TYPE_BYTES = 1;
const uint8_t MAX_PUBLIC_KEY_SIZE = 160;
const uint8_t MAX_MESSAGE_CONTENT_BYTES = 4;
const uint8_t RESPONSE_HEADER_SIZE = 7;
const uint16_t SERVER_ERROR_CODE = 9000;

enum MessageType : uint8_t
{
	SYMMETRIC_KEY_REQUEST = 1,
	SYMMETRIC_KEY_SEND = 2,
	TEXT_MESSAGE_SEND = 3,
	FILE_SEND = 4
};

enum RequestCode : uint16_t
{
	REGISTER_CLIENT = 600,
	LIST_ALL_CLIENTS = 601,
	FETCH_PUBLIC_KEY = 602,
	SEND_MESSAGE = 603,
	LIST_PENDING_MESSAGES = 604
};

enum CommandCode : uint8_t
{
	REGISTRATION = 110,
	CLIENT_LIST = 120,
	PUBLIC_KEY = 130,
	PENDING_MESSAGES = 140,
	SEND_TEXT_MESSAGE = 150,
	RECEIVE_SYMMETRIC_KEY = 151,
	SEND_SYMMETRIC_KEY = 152,
	SEND_FILE = 153,
	EXIT = 0
};

/**
 * @brief Converts a hexadecimal string to a vector of bytes.
 * @param hex The hexadecimal string.
 * @return A vector of uint8_t representing the bytes.
 */
std::vector<uint8_t> hex_string_to_bytes(const std::string& hex);

/**
 * @brief Converts a vector of bytes to a hexadecimal string.
 * @param bytes The vector of bytes.
 * @return A hexadecimal string.
 */
std::string bytes_to_hex_string(const std::vector<uint8_t>& bytes);

/**
 * @brief Checks if a given string is a valid hexadecimal string.
 * @param hex The string to check.
 * @return true if the string contains only hexadecimal digits, false otherwise.
 */
bool is_valid_hex(const std::string& hex);