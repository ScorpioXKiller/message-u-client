#include "Client.h"
#include "RSAWrapper.h"
#include "Base64Wrapper.h"
#include "RequestBuilder.h"
#include "ResponseHandler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <boost/array.hpp>

using boost::asio::ip::tcp;

const short MAX_NAME_SIZE = 255;

Client::Client(const std::string& server_ip, uint16_t server_port)
	: server_ip_(server_ip), server_port_(server_port), socket_(io_context_) {
	load_client_info();
}

void Client::run() {
	connect_to_server();
	while (true) {
		print_menu();

		int choice;
		std::cin >> choice;
		std::cin.ignore();

		if (choice == 0) break;
		switch (choice)
		{
		case 110:
			register_client();
			break;
		case 120:
			//request_client_list();
			break;
		case 130:
			//request_public_key();
			break;
		case 140:
			//request_pending_messages();
			break;
		case 150:
			//send_text_message();
			break;
		case 151:
			//request_symmetric_key();
			break;
		case 152:
			//send_symmetric_key();
			break;

		default:
			std::cout << "Invalid option.\n";
		}
	}

	socket_.close();
}

void Client::load_client_info() {
	std::ifstream file("my.info");
	if (file) {
		std::getline(file, client_name_);
		std::string id_hex;
		std::getline(file, id_hex);
		client_id_ = hex_string_to_bytes(id_hex);
		std::getline(file, private_key_);
	}
}

void Client::save_client_info() {
	std::ofstream file("my.info");
	if (!file)
		throw std::runtime_error("Unable to open my.info for writing.");
	file << client_name_ << "\n" << bytes_to_hex_string(client_id_) << "\n" << private_key_ << "\n";
}

std::vector<uint8_t> Client::hex_string_to_bytes(const std::string& hex) {
	std::vector<uint8_t> bytes;
	for (size_t i = 0; i < hex.size(); i += 2) {
		std::string byte_string = hex.substr(i, 2);
		uint8_t byte = static_cast<uint8_t>(std::stoul(byte_string, nullptr, 16));
		bytes.push_back(byte);
	}

	return bytes;
}

std::string Client::bytes_to_hex_string(const std::vector<uint8_t>& bytes) {
	std::ostringstream oss;

	for (auto b : bytes)
		oss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
	return oss.str();
}

std::vector<uint8_t> Client::pack_header(const std::vector<uint8_t> &client_id, uint8_t version, uint16_t code, uint32_t payload_size)
{
	std::vector<uint8_t> header;
	if (client_id.size() != 16)
		throw std::runtime_error("Client ID must be bytes");

	header.insert(header.end(), client_id.begin(), client_id.end());
	header.push_back(version);

	header.push_back(static_cast<uint8_t>(code & 0xFF));
	header.push_back(static_cast<uint8_t>((code >> 8) & 0xFF));

	header.push_back(static_cast<uint8_t>(payload_size & 0xFF));
	header.push_back(static_cast<uint8_t>(payload_size >> 8) & 0xFF);
	header.push_back(static_cast<uint8_t>(payload_size >> 16) & 0xFF);
	header.push_back(static_cast<uint8_t>(payload_size >> 24) & 0xFF);

	return header;
}

void Client::connect_to_server() {
	try {
		tcp::resolver resolver(io_context_);
		auto endpoints = resolver.resolve(server_ip_, std::to_string(server_port_));
		boost::asio::connect(socket_, endpoints);
		std::cout << "Connected to the server " << server_ip_ << ":" << server_port_ << "\n";
	}
	catch (const std::exception& e) {
		std::cerr << "Connection failed: " << e.what() << "\n";
	}
}

void Client::print_menu() {
	std::cout << "\nMessageU client at your service.\n"
			  << "110) Register\n"
		      << "120) Request for clients list\n"
			  << "130) Request for public key\n"
			  << "140) Request for waiting messages\n"
			  << "150) Send a text message\n"
			  << "151) Send a request for symmetric key\n"
			  << "152) Send your symmetric key\n"
			  << "0) Exit client\n"
			  << "Enter choice: ";
}

void Client::register_client() {
	std::ifstream file("my.info");
	if (file) {
		std::cerr << "Registartion failed. You are already registered.\n";
		return;
	}

	std::cout << "Enter your name: ";
	std::getline(std::cin, client_name_);

	RSAPrivateWrapper rsa_private;
	std::string public_key = rsa_private.getPublicKey();

	if (public_key.size() < 160)
		public_key.resize(160, '\0');
	else if (public_key.size() > 160)
		public_key = public_key.substr(0, 160);
	std::vector<uint8_t> public_key_bytes(public_key.begin(), public_key.end());

	std::string private_key_base_64 = Base64Wrapper::encode(rsa_private.getPrivateKey());

	// build and send registration request
	RequestBuilder request_builder;
	std::vector<uint8_t> request = request_builder.build_registration_request(client_name_, public_key_bytes);
	boost::asio::write(socket_, boost::asio::buffer(request));

	// read and process registration response
	boost::array<uint8_t, 7> response_header;
	boost::asio::read(socket_, boost::asio::buffer(response_header));

	ResponseHandler response_handler;
	ResponseHeader response_header_struct = response_handler.get_response_header(response_header);
	std::vector<uint8_t> response_payload = response_handler.handle_registration_response(response_header);
	
	boost::asio::read(socket_, boost::asio::buffer(response_payload));

	if (!response_payload.empty()) {
		std::cout << "Registration successful. Your client ID: ";
		std::cout << bytes_to_hex_string(response_payload) << "\n";
		client_id_ = response_payload;
		private_key_ = private_key_base_64;
		save_client_info();
	}
	else {
		std::cout << "Registration failed. Server responded with an error: " << response_header_struct.code << "\n";
	}
}

//void Client::request_client_list() {
//	if (client_id_.empty()) {
//		std::cerr << "You must register first.\n";
//		return;
//	}
//
//	uint8_t version = 1;
//	uint16_t code = 601;
//	uint32_t payload_size = 0;
//	std::vector<uint8_t> header = pack_header(version, code, payload_size);
//
//
