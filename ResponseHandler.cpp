#include "ResponseHandler.h"
#include <boost/array.hpp>

ResponseHandler::ResponseHandler() {}

const ResponseHeader ResponseHandler::get_response_header(boost::array<uint8_t, RESPONSE_HEADER_SIZE> response_header)
{
	uint8_t server_version;
	uint16_t response_code;
	uint32_t response_payload_size;

	memcpy(&server_version, response_header.data(), 1);
	memcpy(&response_code, response_header.data() + 1, 2);
	memcpy(&response_payload_size, response_header.data() + 3, 4);

	return ResponseHeader
	{
		server_version,
		response_code,
		response_payload_size
	};
}

const std::vector<uint8_t> ResponseHandler::handle_registration_response(boost::array<uint8_t, RESPONSE_HEADER_SIZE> response_header)
{
	uint32_t response_payload_size = get_response_header(response_header).payload_size;
	std::vector<uint8_t> response_payload(response_payload_size);
	return response_payload;
}