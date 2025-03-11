#pragma once

#include <string>
#include <vector>
#include <boost/asio.hpp>

class Client
{
public:
	Client(const std::string& server_ip, uint16_t server_port);
	void run();

private:
	std::string server_ip_;
	uint16_t server_port_;
	boost::asio::io_context io_context_;
	boost::asio::ip::tcp::socket socket_;

	std::string client_name_;
	std::vector<uint8_t> client_id_;
	std::string private_key_;

	void load_client_info();
	void save_client_info();
	std::vector<uint8_t> hex_string_to_bytes(const std::string& hex);
	std::string bytes_to_hex_string(const std::vector<uint8_t>& bytes);
	std::vector<uint8_t> pack_header(const std::vector<uint8_t>& client_id, uint8_t version, uint16_t code, uint32_t payload_size);

	void register_client();
	void request_client_list();
	//void request_public_key();
	//void request_pending_messages();
	//void send_text_message();
	//void request_symmetric_key();
	//void send_symmetric_key();

	void print_menu();

	void connect_to_server();
};

