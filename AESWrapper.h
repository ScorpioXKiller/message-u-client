/** 
 * @file AESWrapper.h
 * @brief Declaration of the AESWrapper class for the MessageU project.
 *
 * This header file declares the AESWrapper class, which provides encryption and decryption
 * functionality using the AES algorithm.
 *
 * @version 2.0
 * @author Dmitriy Gorodov
 * @id 324725405
 * @date 19/03/2025
 */

#pragma once

#include <string>

 /**
  * @brief The AESWrapper class provides encryption and decryption using the AES algorithm.
  */
class AESWrapper
{
public:
	static const unsigned int DEFAULT_KEYLENGTH = 16;
private:
	unsigned char _key[DEFAULT_KEYLENGTH];

	AESWrapper(const AESWrapper& aes);
public:
	static unsigned char* GenerateKey(unsigned char* buffer, unsigned int length);

	AESWrapper();
	AESWrapper(const unsigned char* key, unsigned int size);
	~AESWrapper();

	const unsigned char* getKey() const;

	std::string encrypt(const char* plain, unsigned int length) const;
	std::string decrypt(const char* cipher, unsigned int length) const;
};
