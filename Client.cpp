/**
 * @file Client.cpp
 * @brief Implementation of the Client class for the MessageU project.
 * 
 * This file implements the methods for connecting to the server, handling user input,
 * processing commands, and integrating encryption via Crypto++ wrappers.
 *
 * @version 2.0
 * @author Dmitriy Gorodov
 * @id 342725405
 * @date 19/03/2025
 */

#include "Client.h"
#include "RSAWrapper.h"
#include "AESWrapper.h"
#include "Base64Wrapper.h"
#include "RequestBuilder.h"
#include "ResponseHandler.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cstring>
#include <boost/array.hpp>
#include <filesystem>

using boost::asio::ip::tcp;

Client::Client(const std::string& server_ip, uint16_t server_port)
    : server_ip_(server_ip), server_port_(server_port), socket_(io_context_) 
{
    load_client_info();
}

void Client::run() 
{
    try
    {
        connect_to_server();
    }
    catch (const std::exception& e) 
    {
        std::cerr << "Connection failed: " << e.what() << "\n";
        exit(EXIT_FAILURE);
    }

    while (true)
    {
        print_menu();

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "\nInvalid input. Please try again...\n";
            continue;
        }

        std::cout << "\n";

        if (choice == CommandCode::EXIT)
        {
            std::cout << "Client is shutting down...\n";
            break;
        }

        switch (choice)
        {
        case CommandCode::REGISTRATION:
            register_client();
            break;
        case CommandCode::CLIENT_LIST:
            request_client_list();
            break;
        case CommandCode::PUBLIC_KEY:
            request_public_key();
            break;
        case CommandCode::PENDING_MESSAGES:
            request_pending_messages();
            break;
        case CommandCode::SEND_TEXT_MESSAGE:
            request_send_text_message();
            break;
        case CommandCode::RECEIVE_SYMMETRIC_KEY:
            request_receive_symmetric_key();
            break;
        case CommandCode::SEND_SYMMETRIC_KEY:
            request_send_symmetric_key();
            break;
		case CommandCode::SEND_FILE:
			request_send_file();
			break;
        default:
            std::cout << "Invalid option. Please try again...\n";
            break;
        }
    }

    socket_.close();
}

void Client::load_client_info() 
{
    std::ifstream file("my.info");

    if (!file) 
    {
        return;
    }

    if (!std::getline(file, client_name_) || client_name_.empty()) 
    {
        std::cerr << "Error: 'my.info' is corrupted or improperly formatted (missing client name).\n";
        exit(EXIT_FAILURE);
    }

    std::string id_hex;
    if (!std::getline(file, id_hex) || id_hex.empty()) 
    {
        std::cerr << "Error: 'my.info' is corrupted or improperly formatted (missing client ID).\n";
        exit(EXIT_FAILURE);
    }
    client_id_ = hex_string_to_bytes(id_hex);

    std::stringstream ss;
    ss << file.rdbuf();
    private_key_ = ss.str();

    if (private_key_.empty()) 
    {
        std::cerr << "Error: 'my.info' is corrupted or improperly formatted (missing private key).\n";
        exit(EXIT_FAILURE);
    }
}

void Client::save_client_info() const 
{
    std::ofstream file("my.info");
    if (!file)
        throw std::runtime_error("Unable to open my.info for writing.");
    file << client_name_ << "\n" << bytes_to_hex_string(client_id_) << "\n" << private_key_;
}

void Client::connect_to_server() 
{
    tcp::resolver resolver(io_context_);
    auto endpoints = resolver.resolve(server_ip_, std::to_string(server_port_));
    boost::asio::connect(socket_, endpoints);
	std::cout << "Connected to the server at " << server_ip_ << ":" << server_port_ << "\n";
}

void Client::print_menu() 
{
    std::cout << "\nMessageU client at your service.\n"
        << "110) Register\n"
        << "120) Request for clients list\n"
        << "130) Request for public key\n"
        << "140) Request for waiting messages\n"
        << "150) Send a text message\n"
        << "151) Send a request for symmetric key\n"
        << "152) Send your symmetric key\n"
		<< "153) Send a file\n"
        << "0) Exit client\n"
        << "Enter choice: ";
}

bool Client::is_client_registered() 
{
    if (client_id_.empty()) 
    {
        std::cerr << "You must register first.\n";
        return false;
    }
    return true;
}

std::string Client::prompt_target_username() 
{
    std::cout << "Enter the target client's username: ";
    std::string target_username;
    std::getline(std::cin, target_username);
    return target_username;
}

std::vector<uint8_t> Client::get_target_id(const std::string& target_username) 
{
    std::vector<uint8_t> target_id = get_client_id_by_username(target_username);
    if (target_id.empty()) 
    {
        std::cerr << "The user with the username \"" << target_username << "\" does not exist.\n";
    }
    return target_id;
}

bool Client::receive_response(std::vector<uint8_t>* response_payload) 
{
    try 
    {
        boost::array<uint8_t, RESPONSE_HEADER_SIZE> response_header_raw;
        boost::asio::read(socket_, boost::asio::buffer(response_header_raw));

        ResponseHandler response_handler;
        ResponseHeader response_header_struct = response_handler.get_response_header(response_header_raw);

        if (response_header_struct.code == SERVER_ERROR_CODE) 
        {
            std::cerr << "Server responded with an error: " << response_header_struct.code << "\n";
            return false;
        }

        uint32_t payload_size = response_header_struct.payload_size;
        response_payload->resize(payload_size);

        if (payload_size > 0) 
        {
            boost::asio::read(socket_, boost::asio::buffer(*response_payload));
        }

        return true;
    }
    catch (const std::exception& e) 
    {
        std::cerr << "Communication error: " << e.what() << "\n";
        return false;
    }
}


void Client::register_client() 
{
    std::ifstream file("my.info");
    if (file) 
    {
        std::cerr << "Registration failed. You are already registered.\n";
        return;
    }

    std::cout << "Enter your name: ";
    std::getline(std::cin, client_name_);
    if (client_name_.empty() || client_name_.length() > MAX_CLIENT_NAME_SIZE) 
    {
		std::cerr << "Invalid name. Please try again...\n";
        return;
    }

    RSAPrivateWrapper rsa_private;
    std::string public_key = rsa_private.getPublicKey();

    if (public_key.size() < MAX_PUBLIC_KEY_SIZE)
        public_key.resize(MAX_PUBLIC_KEY_SIZE, '\0');
    else if (public_key.size() > MAX_PUBLIC_KEY_SIZE)
        public_key = public_key.substr(0, MAX_PUBLIC_KEY_SIZE);
    std::vector<uint8_t> public_key_bytes(public_key.begin(), public_key.end());

    RequestBuilder request_builder;
    std::vector<uint8_t> request = request_builder.build_registration_request(client_name_, public_key_bytes);
    boost::asio::write(socket_, boost::asio::buffer(request));

    std::vector<uint8_t> response_payload;
    if (receive_response(&response_payload)) 
    {
        std::cout << "Registration successful.\n";
        client_id_ = response_payload;
        
        std::string private_key_bin = rsa_private.getPrivateKey();
        std::string private_key_base64 = Base64Wrapper::encode(private_key_bin);
        private_key_ = private_key_base64;
        save_client_info();
    }
    else 
    {
        std::cerr << "Registration failed.\n";
    }
}

void Client::request_client_list() 
{
    if (!is_client_registered()) return;

    RequestBuilder request_builder;
    std::vector<uint8_t> request = request_builder.build_client_list_request(client_id_);
    boost::asio::write(socket_, boost::asio::buffer(request));

    std::vector<uint8_t> response_payload;
	bool is_pending_messages_response = false;
    if (receive_response(&response_payload)) 
    {
        if (response_payload.empty())
        {
            std::cout << "No clients are currently registered.\n";
            return;
        }

        uint16_t record_size = MAX_CLIENT_NAME_SIZE + MAX_CLIENT_ID_SIZE;
        size_t num_records = response_payload.size() / record_size;
        std::cout << "Registered clients:\n";

        for (size_t i = 0; i < num_records; i++) 
        {
            size_t offset = i * record_size;
            std::string client_name(response_payload.begin() + offset + MAX_CLIENT_ID_SIZE, response_payload.begin() + offset + MAX_CLIENT_ID_SIZE + MAX_CLIENT_NAME_SIZE);
            client_name.erase(std::find(client_name.begin(), client_name.end(), '\0'), client_name.end());
            std::cout << " - " << client_name << "\n";
        }
    } 
    else
    {
        std::cerr << "Failed to retrieve client list.\n";
    }

}

void Client::request_public_key() 
{
    if (!is_client_registered()) return;

    std::string target_username = prompt_target_username();
    std::vector<uint8_t> target_id = get_target_id(target_username);
    if (target_id.empty()) return;

    RequestBuilder request_builder;
    std::vector<uint8_t> request = request_builder.build_public_key_request(client_id_, target_id);
    boost::asio::write(socket_, boost::asio::buffer(request));

    std::vector<uint8_t> response_payload;
    if (receive_response(&response_payload)) 
    {
		uint16_t record_size = MAX_CLIENT_ID_SIZE + MAX_PUBLIC_KEY_SIZE;

        if (response_payload.size() >= record_size) 
        {
            std::vector<uint8_t> target_public_key(response_payload.begin() + 16, response_payload.begin() + record_size);
            std::cout << "The public key has been received.\n";
            public_keys_[bytes_to_hex_string(target_id)] = bytes_to_hex_string(target_public_key);
        }
        else 
        {
            std::cerr << "Invalid response received from server.\n";
        }
    }
}

void Client::request_pending_messages() 
{
    if (!is_client_registered()) return;

    RequestBuilder request_builder;
    std::vector<uint8_t> request = request_builder.build_pending_messages_request(client_id_);

    boost::asio::write(socket_, boost::asio::buffer(request));

    std::vector<uint8_t> response_payload;
    if (receive_response(&response_payload)) 
    {
        if (response_payload.empty())
        {
            std::cout << "There are no pending messages. You are up to date.\n";
            return;
        }

        std::unordered_map<std::string, std::string> client_map = get_client_mapping();
        std::unordered_map<std::string, std::string> client_reverse_map;

        for (const auto& pair : client_map) 
        {
            client_reverse_map[pair.second] = pair.first;
        }

        size_t offset = 0;
        while (offset < response_payload.size()) 
        {
			uint8_t record_size = MAX_CLIENT_ID_SIZE + MAX_MESSAGE_ID_BYTES + MAX_MESSAGE_TYPE_BYTES + MAX_MESSAGE_CONTENT_BYTES;
            
            if (offset + record_size > response_payload.size())
                break;

            std::vector<uint8_t> sender_id(response_payload.begin() + offset, response_payload.begin() + offset + MAX_CLIENT_ID_SIZE);
            offset += MAX_CLIENT_ID_SIZE;

            uint32_t message_id;
            memcpy(&message_id, &response_payload[offset], MAX_MESSAGE_ID_BYTES);
            offset += MAX_MESSAGE_ID_BYTES;

            uint8_t message_type = response_payload[offset];
            offset += MAX_MESSAGE_TYPE_BYTES;

            uint32_t message_size;
            memcpy(&message_size, &response_payload[offset], MAX_MESSAGE_CONTENT_BYTES);
            offset += MAX_MESSAGE_CONTENT_BYTES;

            if (offset + message_size > response_payload.size())
                break;

            std::vector<uint8_t> message_content(response_payload.begin() + offset, response_payload.begin() + offset + message_size);
            offset += message_size;

            std::string sender_id_hex = bytes_to_hex_string(sender_id);
            std::string sender_username = client_reverse_map[sender_id_hex];
            std::cout << "From: " << sender_username << "\n";

            handle_incoming_message(sender_id_hex, message_type, message_content);
            std::cout << "-----<EOM>-----\n\n";
        }
    }
    else 
    {
        std::cerr << "Failed to retrieve pending messages.\n";
    }
}

void Client::handle_incoming_message(const std::string& sender_id_hex, uint8_t message_type, const std::vector<uint8_t>& message_content) 
{
    switch (message_type)
    {
    case MessageType::SYMMETRIC_KEY_REQUEST:
    {
        try
        {
            std::string decoded_private_key = Base64Wrapper::decode(private_key_);
            RSAPrivateWrapper rsa_private(decoded_private_key);
            std::string cipher_text(message_content.begin(), message_content.end());
            std::string decrypted_message = rsa_private.decrypt(cipher_text);
            std::cout << "Content:\n" << decrypted_message << "\n";
        }
        catch (std::exception& e)
        {
            std::cerr << "Content:\nError decrypting message: " << e.what() << "\n";
        }
    }
    break;

    case MessageType::SYMMETRIC_KEY_SEND:
    {
        try
        {
            std::string decoded_private_key = Base64Wrapper::decode(private_key_);
            RSAPrivateWrapper rsa_private(decoded_private_key);
            std::string cipher_text(message_content.begin(), message_content.end());
            std::string decrypted_key = rsa_private.decrypt(cipher_text);
            std::vector<uint8_t> symmetric_key(decrypted_key.begin(), decrypted_key.end());

            if (symmetric_key.size() != AESWrapper::DEFAULT_KEYLENGTH)
            {
                std::cerr << "Content:\nReceived symmetric key has invalid length.\n";
            }
            else
            {
                std::cout << "Content:\nSymmetric key received\n";
                symmetric_keys_[sender_id_hex] = bytes_to_hex_string(symmetric_key);
            }
        }
        catch (std::exception& e)
        {
            std::cerr << "Content:\nError decrypting symmetric key: " << e.what() << "\n";
        }
    }
    break;

    case MessageType::TEXT_MESSAGE_SEND:
    {
        auto symmetric_key_found = symmetric_keys_.find(sender_id_hex);
        if (symmetric_key_found == symmetric_keys_.end())
        {
            std::cerr << "Content:\nCan't decrypt the message (symmetric key not found).\n";
        }
        else
        {
            std::vector<uint8_t> symmetric_key_bytes = hex_string_to_bytes(symmetric_key_found->second);
            try
            {
                AESWrapper aes(&symmetric_key_bytes[0], static_cast<unsigned int>(symmetric_key_bytes.size()));
                std::string cipher_text(message_content.begin(), message_content.end());
                std::string decrypted_message_text = aes.decrypt(cipher_text.c_str(), static_cast<unsigned int>(cipher_text.size()));
                std::cout << "Content:\n" << decrypted_message_text << "\n";
            }
            catch (std::exception& e)
            {
                std::cerr << "Content:\nError decrypting message: " << e.what() << "\n";
            }
        }
    }
    break;

    case MessageType::FILE_SEND:
    {
        auto symmetric_key_found = symmetric_keys_.find(sender_id_hex);
        if (symmetric_key_found == symmetric_keys_.end())
        {
            std::cerr << "Content:\nCan't decrypt the file (symmetric key not found).\n";
        }
        else
        {
            std::vector<uint8_t> symmetric_key_bytes = hex_string_to_bytes(symmetric_key_found->second);
            try
            {
                AESWrapper aes(&symmetric_key_bytes[0], static_cast<unsigned int>(symmetric_key_bytes.size()));
                std::string cipher_text(message_content.begin(), message_content.end());
                std::string decrypted_file_content = aes.decrypt(cipher_text.c_str(), static_cast<unsigned int>(cipher_text.size()));

                char* tmp = nullptr;
                size_t len = 0;
				errno_t err = _dupenv_s(&tmp, &len, "TMP");
				std::string tmp_dir = (err == 0 && tmp != nullptr) ? std::string(tmp) : "C:\\Temp";
				if (tmp)
				{
					free(tmp);
				}

                std::ostringstream oss;
                oss << tmp_dir << "\\received_file_" << std::time(nullptr);
				std::string temp_file_path = oss.str();

                std::ofstream file(temp_file_path, std::ios::binary);
                if (!file)
                {
					std::cerr << "Error saving file to " << temp_file_path << "\n";
				}
                else
                {
                    file.write(decrypted_file_content.c_str(), decrypted_file_content.size());
                    file.close();
                    std::cout << "Content:\nFile saved at: " << temp_file_path << "\n";
                }

            }
            catch (std::exception& e)
            {
                std::cerr << "Content:\nError decrypting file: " << e.what() << "\n";
            }
        }
    }
	break;

    default:
        std::cerr << "Content:\nUnknown message type.\n";
        break;
    }
}

void Client::request_receive_symmetric_key() 
{
    if (!is_client_registered()) return;

    std::string target_username = prompt_target_username();
    std::vector<uint8_t> target_id = get_target_id(target_username);
    if (target_id.empty()) return;

    if (!is_public_key(target_id, target_username)) return;

    std::string text_message = "Request for symmetric key";
    std::string encrypted_message = encrypt_with_public_key(target_id, text_message);

    RequestBuilder request_builder;
    MessageType message_type = MessageType::SYMMETRIC_KEY_REQUEST;
    std::vector<uint8_t> request = request_builder.build_send_message_request(client_id_, target_id, message_type, encrypted_message);
    boost::asio::write(socket_, boost::asio::buffer(request));

    std::vector<uint8_t> response_payload;
    if (receive_response(&response_payload)) 
    {
        std::cout << "Request for symmetric key successfully sent to " << target_username << ".\n";
    }
}

void Client::request_send_symmetric_key() 
{
    if (!is_client_registered()) return;

    std::string target_username = prompt_target_username();
    std::vector<uint8_t> target_id = get_target_id(target_username);
    if (target_id.empty()) return;

    if (!is_public_key(target_id, target_username)) return;

    AESWrapper aes;
    const unsigned char* symmetric_key_raw = aes.getKey();
    std::vector<uint8_t> symmetric_key(symmetric_key_raw, symmetric_key_raw + AESWrapper::DEFAULT_KEYLENGTH);

    std::string symmetric_key_str(symmetric_key.begin(), symmetric_key.end());
    std::string encrypted_symmetric_key = encrypt_with_public_key(target_id, symmetric_key_str);

    RequestBuilder request_builder;
    MessageType message_type = MessageType::SYMMETRIC_KEY_SEND;
    std::vector<uint8_t> request = request_builder.build_send_message_request(client_id_, target_id, message_type, encrypted_symmetric_key);
    boost::asio::write(socket_, boost::asio::buffer(request));

    std::vector<uint8_t> response_payload;
    if (receive_response(&response_payload)) 
    {
        std::cout << "The symmetric key successfully sent to " << target_username << ".\n";
        symmetric_keys_[bytes_to_hex_string(target_id)] = bytes_to_hex_string(symmetric_key);
    }
}

void Client::request_send_text_message() 
{
    if (!is_client_registered()) return;

    std::string target_username = prompt_target_username();
    std::vector<uint8_t> target_id = get_target_id(target_username);
    if (target_id.empty()) return;

    std::string target_id_hex = bytes_to_hex_string(target_id);

    auto target_symmetric_key = symmetric_keys_.find(target_id_hex);
    if (target_symmetric_key == symmetric_keys_.end()) 
    {
        std::cerr << "Symmetric key for client " << target_username << " not found. Please request a key exchange first.\n";
        return;
    }

    std::vector<uint8_t> target_symmetric_key_bytes = hex_string_to_bytes(target_symmetric_key->second);

    std::cout << "Enter your message:\n";
    std::string text_message;
    std::getline(std::cin, text_message);

    AESWrapper aes(&target_symmetric_key_bytes[0], static_cast<unsigned int>(target_symmetric_key_bytes.size()));
    std::string encrypted_message = aes.encrypt(text_message.c_str(), static_cast<unsigned int>(text_message.size()));

    RequestBuilder request_builder;
    MessageType message_type = MessageType::TEXT_MESSAGE_SEND;
    std::vector<uint8_t> request = request_builder.build_send_message_request(client_id_, target_id, message_type, encrypted_message);
    boost::asio::write(socket_, boost::asio::buffer(request));

    std::vector<uint8_t> response_payload;
    if (receive_response(&response_payload)) 
    {
        std::cout << "Message successfully sent to " << target_username << ".\n";
    }
}

void Client::request_send_file() 
{
   if (!is_client_registered()) return;

    std::string target_username = prompt_target_username();
    std::vector<uint8_t> target_id = get_target_id(target_username);
	if (target_id.empty()) return;

    std::string target_id_hex = bytes_to_hex_string(target_id);

    auto target_symmetric_key = symmetric_keys_.find(target_id_hex);
    if (target_symmetric_key == symmetric_keys_.end())
    {
        std::cerr << "Symmetric key for client " << target_username << " not found. Please request a key exchange first.\n";
        return;
    }

    std::vector<uint8_t> target_symmetric_key_bytes = hex_string_to_bytes(target_symmetric_key->second);

	std::cout << "Enter the path to the file you want to send: ";
	std::string file_path;
	std::getline(std::cin, file_path);

    if (!std::filesystem::exists(file_path))
    {
		std::cerr << "File not found.\n";
		return;
    }

	std::ifstream file(file_path, std::ios::binary);
	if (!file)
	{
		std::cerr << "Error opening file.\n";
		return;
	}

	std::vector<uint8_t> file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

    AESWrapper aes(&target_symmetric_key_bytes[0], static_cast<unsigned int>(target_symmetric_key_bytes.size()));
	std::string encrypted_file_content = aes.encrypt(reinterpret_cast<const char*>(file_content.data()), static_cast<unsigned int>(file_content.size()));

    RequestBuilder request_builder;
    MessageType message_type = MessageType::FILE_SEND;
    std::vector<uint8_t> request = request_builder.build_send_message_request(client_id_, target_id, message_type, encrypted_file_content);
    boost::asio::write(socket_, boost::asio::buffer(request));

    std::vector<uint8_t> response_payload;
    if (receive_response(&response_payload))
    {
        std::cout << "File successfully sent to " << target_username << ".\n";
    }

}

std::string Client::encrypt_with_public_key(const std::vector<uint8_t>& target_id, const std::string& message) 
{
    std::string target_id_hex = bytes_to_hex_string(target_id);
    std::vector<uint8_t> target_public_key_bytes = hex_string_to_bytes(public_keys_[target_id_hex]);

    std::string public_key_str(target_public_key_bytes.begin(), target_public_key_bytes.end());
    RSAPublicWrapper rsa_public(public_key_str);
    return rsa_public.encrypt(message);
}

bool Client::is_public_key(const std::vector<uint8_t>& target_id, const std::string& target_username) 
{
    auto target_public_key = public_keys_.find(bytes_to_hex_string(target_id));
    if (target_public_key == public_keys_.end()) 
    {
        std::cerr << "Public key for client " << target_username << " not found. Please request the public key first.\n";
        return false;
    }
    return true;
}

std::vector<uint8_t> Client::get_client_id_by_username(const std::string& username) 
{
    RequestBuilder request_builder;
    std::vector<uint8_t> request = request_builder.build_client_list_request(client_id_);
    boost::asio::write(socket_, boost::asio::buffer(request));

    std::vector<uint8_t> response_payload;
    if (!receive_response(&response_payload)) 
    {
        return std::vector<uint8_t>();
    }

    uint16_t record_size = MAX_CLIENT_NAME_SIZE + MAX_CLIENT_ID_SIZE;
    size_t num_records = response_payload.size() / record_size;

    for (size_t i = 0; i < num_records; i++) 
    {
        size_t offset = i * record_size;
        std::string client_name(response_payload.begin() + offset + MAX_CLIENT_ID_SIZE, response_payload.begin() + offset + MAX_CLIENT_ID_SIZE + MAX_CLIENT_NAME_SIZE);
        client_name.erase(std::find(client_name.begin(), client_name.end(), '\0'), client_name.end());

        if (client_name == username) 
        {
            std::vector<uint8_t> found_client_id(response_payload.begin() + offset, response_payload.begin() + offset + MAX_CLIENT_ID_SIZE);
            return found_client_id;
        }
    }
    return std::vector<uint8_t>();
}

std::unordered_map<std::string, std::string> Client::get_client_mapping() 
{
    std::unordered_map<std::string, std::string> client_mapping;

    RequestBuilder request_builder;
    std::vector<uint8_t> request = request_builder.build_client_list_request(client_id_);
    boost::asio::write(socket_, boost::asio::buffer(request));

    std::vector<uint8_t> response_payload;
    if (!receive_response(&response_payload)) 
    {
        return client_mapping;
    }

    const uint16_t record_size = MAX_CLIENT_NAME_SIZE + MAX_CLIENT_ID_SIZE;
    size_t num_records = response_payload.size() / record_size;

    for (size_t i = 0; i < num_records; i++) 
    {
        size_t offset = i * record_size;
        std::vector<uint8_t> user_id(response_payload.begin() + offset, response_payload.begin() + offset + MAX_CLIENT_ID_SIZE);
        std::string user_id_hex = bytes_to_hex_string(user_id);
        std::string username(response_payload.begin() + offset + 16, response_payload.begin() + offset + MAX_CLIENT_ID_SIZE + MAX_CLIENT_NAME_SIZE);
        username.erase(std::find(username.begin(), username.end(), '\0'), username.end());
        client_mapping[username] = user_id_hex;
    }

    return client_mapping;
}