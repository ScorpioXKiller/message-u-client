# MessageU Client

## Overview
MessageU Client is a console-based C++ application that allows users to securely exchange messages, request keys, and transfer files using the MessageU protocol. The client uses Boost.Asio for networking and Crypto++ for cryptographic operations (AES, RSA, and Base64 encoding). This README provides detailed installation and configuration instructions for the project tools and libraries, ensuring that you can successfully build and run the client.

## Prerequisites
- **Operating System:** Windows only
- **C++17 Compiler:** (e.g., Visual Studio 2019 or later)
- **Boost Libraries:** Ensure Boost (including Boost.Asio) is installed.
- **Crypto++ Library:** Install Crypto++ (recommended version: 8.80 or later)
- **CMake (Optional):** Recommended for building the project.
- **Environment Variable:** The project uses the `TMP` environment variable for temporary file storage (default fallback is `C:\Temp`).

## Installation and Configuration

### Installing Boost
1. **Download Boost:**  
   Visit [Boost Downloads](https://www.boost.org/users/download/) and download the latest version.
2. **Extract Boost:**  
   Extract Boost to a directory (e.g., `C:\local\boost_1_76_0`).
3. **Configure in Visual Studio:**  
   - Open your project in Visual Studio.
   - Navigate to **Project > Properties > C/C++ > General** and add the Boost include directory (e.g., `C:\local\boost_1_76_0`) to **Additional Include Directories**.
   - Under **Linker > General**, add the Boost library directory (e.g., `C:\local\boost_1_76_0\stage\lib`) to **Additional Library Directories**.
   - Under **Linker > Input**, add the required Boost libraries (e.g., `boost_system.lib`).

### Installing Crypto++
1. **Download Crypto++:**  
   Download Crypto++ from [Crypto++ Downloads](https://www.cryptopp.com/).
2. **Extract Crypto++:**  
   Extract the library to a directory (e.g., `C:\cryptopp`).
3. **Configure in Visual Studio:**  
   - In your project properties, under **C/C++ > General**, add the Crypto++ include directory (e.g., `C:\cryptopp`) to **Additional Include Directories**.
   - Under **Linker > General**, add the Crypto++ library directory to **Additional Library Directories**.
   - Under **Linker > Input**, add `cryptopp.lib` to **Additional Dependencies**.

### Building with CMake (Optional)
If you prefer using CMake:
1. Create a `build` folder in the project root:
   ```bash
   mkdir build && cd build
   ```
2. Run CMake to generate the Visual Studio solution (adjust generator if needed):
   ```bash
   cmake .. -G "Visual Studio 16 2019" -A x64
   ```
3. Build the project:
   ```bash
   cmake --build . --config Release
   ```

### Manual Configuration in Visual Studio
1. Open the project in Visual Studio.
2. Go to **Project > Properties**:
   - Under **C/C++ > General**, add the include paths for Boost and Crypto++.
   - Under **Linker > General**, add the library directories for Boost and Crypto++.
   - Under **Linker > Input**, add `boost_system.lib` and `cryptopp.lib` to **Additional Dependencies**.

## Running the Client
1. **Create a `server.info` file:**  
   In the same folder as the client executable, create a file named `server.info` with the following format:
   ```
   127.0.0.1:1234
   ```
   Replace `127.0.0.1` and `1234` with the actual server IP and port.

2. **Execute the Client:**  
   Run the built executable (e.g., `MessageUClient.exe`) from the command prompt or via Visual Studio. The client will display a menu with the following options:
   - **110) Register:** Register with the server.
   - **120) Request for clients list:** Retrieve the list of registered clients.
   - **130) Request for public key:** Request a target client's public key.
   - **140) Request for pending messages:** Retrieve waiting messages.
   - **150) Send a text message:** Send an encrypted text message.
   - **151) Send a request for symmetric key:** Request a symmetric key from a target client.
   - **152) Send your symmetric key:** Send your symmetric key to a target client.
   - **153) Send a file:** Send an encrypted file.
   - **0) Exit client:** Exit the application.

## Project Structure
- **Client.h / Client.cpp:** Main implementation of client functionalities.
- **main.cpp:** Entry point for the client application.
- **RequestBuilder.h / RequestBuilder.cpp:** Constructs protocol requests (registration, client list, public key, pending messages, send message).
- **ResponseHandler.h / ResponseHandler.cpp:** Processes responses from the server.
- **utils.h / utils.cpp:** Utility functions for byte conversion and helper methods.
- **(Optional) CMakeLists.txt:** Build configuration for CMake.

## Documentation
The source code is documented using Doxygen-style comments. To generate the documentation:
1. Run `doxygen -g` in the project root to generate a default Doxyfile.
2. Edit the Doxyfile to set the `INPUT` variable to include all client source files.
3. Run `doxygen` to produce the documentation in HTML or LaTeX format.

## Known Issues and Future Improvements
- **Key Management:** Enhance key rotation and mutual authentication mechanisms.
- **Error Handling:** Further improve error handling for invalid message formats and unsupported protocol versions.
- **Security Enhancements:** Consider switching to authenticated encryption (e.g., AES-GCM) and adding message integrity verification (HMAC).
- **Performance:** Optimize file transfer performance for large files and improve overall network throughput.