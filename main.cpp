/**
 * @file main.cpp
 * @brief Entry point for the MessageU client application.
 *
 * Reads the server configuration from "server.info", creates a Client object,
 * and starts the client.
 * 
 * @version 2.0
 * @author Dmitriy Gorodov
 * @id 342725405
 * @date 19/03/2025
 */

#include "Client.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdexcept>
#include <boost/asio.hpp>

int main() 
{
	try 
	{
		std::ifstream server_info_file("server.info");
		if (!server_info_file)
			throw std::runtime_error("Unable to open server.info for reading.");
		std::string server_line;
		std::getline(server_info_file, server_line);
		auto pos = server_line.find(':');
		if (pos == std::string::npos)
			throw std::runtime_error("Invalid server.info format.");
		std::string server_ip = server_line.substr(0, pos);
		uint16_t server_port = static_cast<uint16_t>(std::stoi(server_line.substr(pos + 1)));

		Client client(server_ip, server_port);
		client.run();
	}
	catch (const std::exception& e) 
	{
		std::cerr << "Error: " << e.what() << "\n";
	}

	return 0;	
}