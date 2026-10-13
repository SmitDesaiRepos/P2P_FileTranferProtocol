#include <p2p/socket.hpp>
#include <iostream>

namespace p2p {

    class WinsockInitializer {
        public: 
            WinsockInitializer() {
                #ifdef _WIN32
                    WSADATA wsaData;
                    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
                    if (result != 0) {
                        throw std::runtime_error("WSAStartup failed with error: " + std::to_string(result));
                    }
                #endif
            }

            ~WinsockInitializer() {
                #ifdef _WIN32
                    WSACleanup();
                #endif
            }
    };

    static WinsockInitializer wsa_init; // Ensures WSAStartup is called before any socket operations

    Socket::Socket() : sock_fd(INVALID_SOCKET), is_started(false){}
    Socket::Socket(socket_t existing_sock) : sock_fd(existing_sock), is_started(true) {}    

    Socket::~Socket() {
        close();
    }

    Socket::Socket(Socket&& other) noexcept : sock_fd(other.sock_fd), is_started(other.is_started) {
        other.sock_fd = INVALID_SOCKET;
        other.is_started = false;
    }

    Socket& Socket::operator=(Socket&& other) noexcept {
        if (this != &other) {
            close();
            sock_fd = other.sock_fd;
            is_started = other.is_started;
            other.sock_fd = INVALID_SOCKET;
            other.is_started = false;
        }
        return *this;
    }

    bool Socket::create() {
        sock_fd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd == INVALID_SOCKET) {
            std::cerr << "Socket creation failed with error: " << errno << std::endl;
            return false;
        }
        is_started = true;
        return true;
    }

    bool Socket::bind(int port) {
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        if (::bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            std::cerr << "Bind failed with error: " << errno << std::endl;
            return false;
        }
        return true;
    }

    bool Socket::listen(int backlog) {
        if (::listen(sock_fd, backlog) == SOCKET_ERROR) {
            std::cerr << "Listen failed with error: " << errno << std::endl;
            return false;
        }
        return true;
    }

    Socket Socket::accept() {
        sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        socket_t client_sock = ::accept(sock_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_sock == INVALID_SOCKET) {
            std::cerr << "Accept failed with error: " << errno << std::endl;
            return Socket(); // Return an invalid socket
        }
        return Socket(client_sock);
    }

    bool Socket::connect(const std::string& ip_addr, int port) {
        sockaddr_in hint{};
        hint.sin_family = AF_INET;
        hint.sin_port = htons(port);
        inet_pton(AF_INET, ip_addr.c_str(), &hint.sin_addr);

        int result = ::connect(sock_fd, (struct sockaddr*)&hint, sizeof(hint));
        return result != SOCKET_ERROR;
    }

    int Socket::send(const char* data, size_t length) {
        return ::send(sock_fd, data, static_cast<int>(length), 0);
    }

    int Socket::receive(char* buffer, size_t length) {
        return ::recv(sock_fd, buffer, static_cast<int>(length), 0);
    }

    void Socket::close() {
        if (sock_fd != INVALID_SOCKET) {
            #ifdef _WIN32
                closesocket(sock_fd);
            #else
                ::close(sock_fd);
            #endif
            sock_fd = INVALID_SOCKET;
            is_started = false;
        }
    }

} // namespace p2p