#pragma once

#include <cstdint>
#include <vector>
#include <boost/array.hpp>
#include <string>

const short RESPONSE_HEADER_SIZE = 7;

struct ResponseHeader
{
	uint8_t version;
	uint16_t code;
	uint32_t payload_size;
};

class ResponseHandler
{
public:
	ResponseHandler();
	const std::vector<uint8_t> handle_registration_response(boost::array<uint8_t, RESPONSE_HEADER_SIZE> response_header);
	const ResponseHeader get_response_header(boost::array<uint8_t, RESPONSE_HEADER_SIZE> response_header);
};

