/**
 * @file ResponseHandler.h
 * @brief Declaration of the ResponseHandler class.
 *
 * This file declares the ResponseHandler class that parses the response header
 * and constructs a vector for the response payload.
 *
 * @version 2.0
 * @author Dmitriy Gorodov
 * @id 342725405
 * @date 19/03/2025
 */

#pragma once

#include "utils.h"
#include <cstdint>
#include <boost/array.hpp>

/**
 * @brief Structure representing a response header.
 */
struct ResponseHeader
{
	uint8_t version;
	uint16_t code;
	uint32_t payload_size;
};

/**
 * @brief The ResponseHandler class processes responses from the server.
 */
class ResponseHandler
{
public:
	ResponseHandler();

	/**
	 * @brief Extracts the response header from a raw header buffer.
	 * @param response_header The raw header data.
	 * @return A ResponseHeader structure.
	 */
	const std::vector<uint8_t> handle_response(boost::array<uint8_t, RESPONSE_HEADER_SIZE> response_header);
	
	/**
	 * @brief Prepares a buffer for reading the response payload.
	 * @param response_header The raw header data.
	 * @return A vector of uint8_t with the size equal to the payload size.
	 */
	const ResponseHeader get_response_header(boost::array<uint8_t, RESPONSE_HEADER_SIZE> response_header);
};