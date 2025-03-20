/**
 * @file RequestBuilder.h
 * @brief Declaration of the RequestBuilder class for constructing protocol requests.
 *
 * This class provides functions for building different types of requests (registration,
 * client list, public key, pending messages, send message) according to the MessageU protocol.
 * 
 * @version 2.0
 * @author Dmitriy Gorodov
 * @id 324725405
 * @date 19/03/2025
 */

#pragma once 

#include "utils.h"
#include <cstdint>

/**
 * @brief Structure representing a request header.
 */
struct RequestHeader
{
	std::vector<uint8_t> client_id;
	uint8_t version;
	uint16_t code;
	uint32_t payload_size;
};

/**
 * @brief The RequestBuilder class encapsulates functionality for building requests.
 */
class RequestBuilder
{
public:
	RequestBuilder();

	/**
	 * @brief Builds a registration request.
	 * @param client_name The client's name.
	 * @param public_key The client's public key (vector of 160 bytes).
	 * @return A vector of bytes representing the complete request.
	 */
	const std::vector<uint8_t> build_registration_request(const std::string& client_name, const std::vector<uint8_t>& public_key);
	
	/**
	 * @brief Builds a client list request.
	 * @param client_id The client's unique identifier.
	 * @return A vector of bytes representing the request.
	 */
	const std::vector<uint8_t> build_client_list_request(const std::vector<uint8_t> &client_id);
	
	/**
	 * @brief Builds a public key request.
	 * @param client_id The client's unique identifier.
	 * @param target_id The target client's unique identifier.
	 * @return A vector of bytes representing the request.
	 */
	const std::vector<uint8_t> build_public_key_request(const std::vector<uint8_t>& client_id, const std::vector<uint8_t> &target_id);
	
	/**
	 * @brief Builds a pending messages request.
	 * @param client_id The client's unique identifier.
	 * @return A vector of bytes representing the request.
	 */
	const std::vector<uint8_t> build_pending_messages_request(const std::vector<uint8_t>& client_id);
	
	/**
	 * @brief Builds a send message request.
	 * @param client_id The sender's client ID.
	 * @param target_id The target client's ID.
	 * @param message_type The type of the message.
	 * @param message_content The encrypted message content.
	 * @return A vector of bytes representing the request.
	 */
	const std::vector<uint8_t> build_send_message_request(const std::vector<uint8_t> client_id, const std::vector<uint8_t> client_target_id, const uint8_t message_type, const std::string message_content);
	
	/**
	 * @brief Packs the request header into a vector of bytes.
	 * @param header The RequestHeader structure.
	 * @return A vector of bytes representing the packed header.
	 */
	const std::vector<uint8_t> pack_header(const RequestHeader &header);
};