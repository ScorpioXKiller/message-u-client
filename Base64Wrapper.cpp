/**
* @file Base64Wrapper.cpp
* @brief Implementation of the Base64Wrapper class for the MessageU project.
* 
* This file implements the methods for encoding and decoding strings using the Base64 algorithm.
* 
* @version 2.0
* @author Dmitriy Gorodov
* @id 342725405
* @date 19/03/2025
*/

#include "Base64Wrapper.h"

std::string Base64Wrapper::encode(const std::string& str)
{
	std::string encoded;
	CryptoPP::StringSource ss(str, true,
		new CryptoPP::Base64Encoder(
			new CryptoPP::StringSink(encoded)
		) // Base64Encoder
	); // StringSource

	return encoded;
}

std::string Base64Wrapper::decode(const std::string& str)
{
	std::string decoded;
	CryptoPP::StringSource ss(str, true,
		new CryptoPP::Base64Decoder(
			new CryptoPP::StringSink(decoded)
		) // Base64Decoder
	); // StringSource

	return decoded;
}