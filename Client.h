/**
 * @file Client.h
 * @brief Declaration of the Client class for the MessageU project.
 *
 * This header defines the Client class, which implements the console-based client.
 * The client supports registration, requesting client lists and public keys, sending text
 * messages, requesting and sending symmetric keys, and sending files.
 *
 * @version 2.0
 * @author Dmitriy Gorodov
 * @id 324725405
 * @date 19/03/2025
 */

#pragma once

#include <string>
#include <vector>
#include <boost/asio.hpp>

/**
 * @brief The Client class encapsulates the client-side functionality.
 *
 * It manages the connection to the server, user interactions through the console,
 * and constructs/handles requests and responses according to the MessageU protocol.
 */
class Client
{
public:
	/**
	 * @brief Constructs a new Client object.
	 *
	 * @param server_ip The IP address of the server.
	 * @param server_port The port number of the server.
	 */
	Client(const std::string& server_ip, uint16_t server_port);
	
	/**
	 * @brief Runs the client, displaying the menu and handling user commands.
	 */
	void run();

private:
	std::string server_ip_;
	uint16_t server_port_;
	boost::asio::io_context io_context_;
	boost::asio::ip::tcp::socket socket_;

	std::string client_name_;
	std::vector<uint8_t> client_id_;
	std::string private_key_;

	/**
	* @brief Mapping of client usernames to their IDs.
	*/
	std::unordered_map<std::string, std::string> public_keys_;

	/**
	* @brief Mapping of client usernames to their symmetric keys.
	*/
	std::unordered_map<std::string, std::string> symmetric_keys_;

	/**
	 * @brief Loads the client information from "my.info" if it exists.
	 * 
	 * The file should contain the client name, UUID, and private key.
	 */
	void load_client_info();

	/**
	 * @brief Saves the client information to "my.info".
	 */
	void save_client_info() const;

	/**
	 * @brief Checks if the client is registered.
	 * @return true if registered, false otherwise.
	 */
	bool is_client_registered();

	/**
	 * @brief Prompts the user for a target username.
	 * @return The entered target username.
	 */
	std::string prompt_target_username();

	/**
	* @brief Gets the target client's ID based on username.
	* @param target_username The target username.
	* @return A vector of bytes representing the client ID.
	*/
	std::vector<uint8_t> get_target_id(const std::string& target_username);

	/**
	 * @brief Receives a response from the server.
	 *
	 * @param response_payload Pointer to store the response payload.
	 * @return true if a valid response is received, false otherwise.
	 */
	bool receive_response(std::vector<uint8_t> *response_payload);

	/**
	 * @brief Handles an incoming message from the server.
	 * @param sender_id_hex The sender's client ID in hexadecimal.
	 * @param message_type The message type.
	 * @param message_content The message content.
	 */
	void handle_incoming_message(const std::string& sender_id_hex, uint8_t message_type, const std::vector<uint8_t>& message_content);

	/**
	 * @brief Checks if the public key for a target client exists.
	 * @param target_id The target client ID.
	 * @param target_username The target username.
	 * @return true if public key is available, false otherwise.
	 */
	bool is_public_key(const std::vector<uint8_t>& target_id, const std::string& target_username);

	/**
	 * @brief Encrypts a message using the target client's public key.
	 * @param target_id The target client ID.
	 * @param message The message to encrypt.
	 * @return The encrypted message as a string.
	 */
	std::string encrypt_with_public_key(const std::vector<uint8_t>& target_id, const std::string& message);

	/**
	 * @brief Registers the client with the server.
	 */
	void register_client();

	/**
	 * @brief Requests the client list from the server.
	 */
	void request_client_list();

	/**
	 * @brief Requests the public key of a target client.
	 */
	void request_public_key();

	/**
	 * @brief Requests pending messages from the server.
	 */
	void request_pending_messages();

	/**
	 * @brief Encrypts a text message with a symmetric key and sends it to a target client.
	 */
	void request_send_text_message();

	/**
	 * @brief Requests a symmetric key from a target client.
	 */
	void request_receive_symmetric_key();

	/**
	 * @brief Creates a symmetric, encrypts it with the target client's public key, and sends it.
	 */
	void request_send_symmetric_key();

	/**
	 * @brief Encrypts a file with a symmetric key and sends it to a target client.
	 */
	void request_send_file();

	/**
	 * @brief Retrieves a client ID by the given username from the client list.
	 * @param username The username to search for.
	 * @return The client ID as a vector of bytes.
	 */
	std::vector<uint8_t> get_client_id_by_username(const std::string& username);
	
	/**
	 * @brief Retrieves a mapping of client usernames to their IDs.
	 * @return An unordered_map where key is the username and value is the client ID in hex.
	 */
	std::unordered_map<std::string, std::string> get_client_mapping();
	
	/**
	 * @brief Prints the client menu.
	 */
	void print_menu();

	/**
	 * @brief Connects to the server.
	 */
	void connect_to_server();
};

