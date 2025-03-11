#pragma once 
#include <cstdint>
#include <vector>
#include <string>

struct RequestHeader
{
	std::vector<uint8_t> client_id;
	uint8_t version;
	uint16_t code;
	uint32_t payload_size;
};

class RequestBuilder
{
public:
	RequestBuilder();
	const std::vector<uint8_t> build_registration_request(const std::string& client_name, const std::vector<uint8_t>& public_key);
private:
	const std::vector<uint8_t> pack_header(const RequestHeader &header);
};