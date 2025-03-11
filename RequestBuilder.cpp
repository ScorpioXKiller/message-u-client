#include <stdexcept>
#include "RequestBuilder.h"

RequestBuilder::RequestBuilder() {}

const std::vector<uint8_t> RequestBuilder::pack_header(const RequestHeader &header)
{
	std::vector<uint8_t> header_buffer;
	if (header.client_id.size() != 16)
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

	if (client_name_bytes.size() < 255)
		client_name_bytes.resize(255, 0);
	else if (client_name_bytes.size() > 255) {
		client_name_bytes.resize(254);
		client_name_bytes.push_back(0);
	}

	payload.insert(payload.end(), client_name_bytes.begin(), client_name_bytes.end());
	payload.insert(payload.end(), public_key_bytes.begin(), public_key_bytes.end());

	uint32_t payload_size = static_cast<uint32_t>(payload.size());

	std::vector<uint8_t> header = pack_header(
		RequestHeader
		{
			std::vector<uint8_t>(16, 0), // client_id is not known yet so it is set to 0 for now 
			static_cast<uint8_t>(1),
			static_cast<uint16_t>(600),
			payload_size
		}
	);

	std::vector<uint8_t> request(header);
	request.insert(request.end(), payload.begin(), payload.end());

	return request;
}