/**
 * @file RequestBuilder.cpp
 * @brief Implements the RequestBuilder class for constructing protocol requests.
 *
 * This file contains implementations for functions that build requests (registration, client list,
 * public key, pending messages, send message) by packing the header and payload according to the protocol.
 *
 * @version 2.0
 * @author Dmitriy Gorodov
 * @id 342725405
 * @date 19/03/2025
 */

#include <stdexcept>
#include "RequestBuilder.h"
#include "utils.h"

RequestBuilder::RequestBuilder() {}

const std::vector<uint8_t> RequestBuilder::pack_header(const RequestHeader &header)
{
	std::vector<uint8_t> header_buffer;
	if (header.client_id.size() != MAX_CLIENT_ID_SIZE)
		throw std::runtime_error("Client ID must be bytes");

	header_buffer.insert(header_buffer.end(), header.client_id.begin(), header.client_id.end());
	header_buffer.push_back(header.version);

	header_buffer.push_back(static_cast<uint8_t>(header.code & 0xFF));
	header_buffer.push_back(static_cast<uint8_t>((header.code >> 8) & 0xFF));

	header_buffer.push_back(static_cast<uint8_t>(header.payload_size & 0xFF));
	header_buffer.push_back(static_cast<uint8_t>(header.payload_size >> 8) & 0xFF);
	header_buffer.push_back(static_cast<uint8_t>(header.payload_size >> 16) & 0xFF);
	header_buffer.push_back(static_cast<uint8_t>(header.payload_size >> 24) & 0xFF);

	return header_buffer;
}

const std::vector<uint8_t> RequestBuilder::build_registration_request(const std::string& client_name, const std::vector<uint8_t>& public_key)
{
	std::vector<uint8_t> payload;

	std::vector<uint8_t> client_name_bytes(client_name.begin(), client_name.end());
	std::vector<uint8_t> public_key_bytes(public_key.begin(), public_key.end());

	if (client_name_bytes.size() < MAX_CLIENT_NAME_SIZE)
		client_name_bytes.resize(MAX_CLIENT_NAME_SIZE, 0);
	else if (client_name_bytes.size() > MAX_CLIENT_NAME_SIZE) 
	{
		client_name_bytes.resize(static_cast<uint8_t>(MAX_CLIENT_NAME_SIZE - 1));
		client_name_bytes.push_back(0);
	}

	payload.insert(payload.end(), client_name_bytes.begin(), client_name_bytes.end());
	payload.insert(payload.end(), public_key_bytes.begin(), public_key_bytes.end());

	uint32_t payload_size = static_cast<uint32_t>(payload.size());

	std::vector<uint8_t> header = pack_header(
		RequestHeader
		{
			std::vector<uint8_t>(MAX_CLIENT_ID_SIZE, 0),
			static_cast<uint8_t>(CLIENT_VERSION),
			static_cast<uint16_t>(RequestCode::REGISTER_CLIENT),
			payload_size
		}
	);

	std::vector<uint8_t> request(header);
	request.insert(request.end(), payload.begin(), payload.end());

	return request;
}

const std::vector<uint8_t> RequestBuilder::build_client_list_request(const std::vector<uint8_t> &client_id)
{
	std::vector<uint8_t> header = pack_header(
		RequestHeader
		{
			client_id,
			static_cast<uint8_t>(CLIENT_VERSION),
			static_cast<uint16_t>(RequestCode::LIST_ALL_CLIENTS),
			static_cast<uint32_t>(0)
		}
	);

	return header;
}

const std::vector<uint8_t> RequestBuilder::build_public_key_request(const std::vector<uint8_t>& client_id, const std::vector<uint8_t>& target_id)
{
	std::vector<uint8_t> payload;

	payload.insert(payload.end(), target_id.begin(), target_id.end());

	uint32_t payload_size = static_cast<uint32_t>(payload.size());

	std::vector<uint8_t> header = pack_header(
		RequestHeader
		{
			client_id,
			static_cast<uint8_t>(CLIENT_VERSION),
			static_cast<uint16_t>(RequestCode::FETCH_PUBLIC_KEY),
			payload_size
		}
	);

	std::vector<uint8_t> request(header);
	request.insert(request.end(), payload.begin(), payload.end());

	return request;
}

const std::vector<uint8_t> RequestBuilder::build_pending_messages_request(const std::vector<uint8_t>& client_id)
{
	std::vector<uint8_t> header = pack_header(
		RequestHeader
		{
			client_id,
			static_cast<uint8_t>(CLIENT_VERSION),
			static_cast<uint16_t>(RequestCode::LIST_PENDING_MESSAGES),
			static_cast<uint32_t>(0)
		}
	);
	return header;
}

const std::vector<uint8_t> RequestBuilder::build_send_message_request(const std::vector<uint8_t> client_id, const std::vector<uint8_t> target_id, const uint8_t message_type, const std::string encrypted_message_content)
{
	std::vector<uint8_t> message_content_bytes(encrypted_message_content.begin(), encrypted_message_content.end());
	uint32_t content_size = static_cast<uint32_t>(message_content_bytes.size());

	std::vector<uint8_t> payload;

	payload.insert(payload.end(), target_id.begin(), target_id.end());
	payload.push_back(message_type);

	uint8_t content_size_bytes[MAX_MESSAGE_CONTENT_BYTES];
	memcpy(content_size_bytes, &content_size, MAX_MESSAGE_CONTENT_BYTES);
	payload.insert(payload.end(), content_size_bytes, content_size_bytes + MAX_MESSAGE_CONTENT_BYTES);
	payload.insert(payload.end(), message_content_bytes.begin(), message_content_bytes.end());

	uint32_t payload_size = static_cast<uint32_t>(payload.size());

	std::vector<uint8_t> header = pack_header(
		RequestHeader
		{
			client_id,
			static_cast<uint8_t>(CLIENT_VERSION),
			static_cast<uint16_t>(RequestCode::SEND_MESSAGE),
			payload_size
		}
	);

	std::vector<uint8_t> request(header);
	request.insert(request.end(), payload.begin(), payload.end());

	return request;
}