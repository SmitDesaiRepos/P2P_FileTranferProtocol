#ifndef P2P_SOCKET_HPP
#define P2P_SOCKET_HPP

#include <string>
#include <stdexcept>

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET socket_t;
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
    typedef int socket_t;
    const int INVALID_SOCKET = -1;
    const int SOCKET_ERROR = -1;
#endif

namespace p2p {
    
    class Socket {
        private:
            socket_t sock_fd;
            bool is_started;
        public: 
            Socket();
            explicit Socket(socket_t existing_sock);
            ~Socket();
        // Prevent copying to avoid double-closing socket file descriptors
            Socket(const Socket&) = delete;
            Socket& operator=(const Socket&) = delete;

        // Allow moving resources safely
            Socket(Socket&& other) noexcept;
            Socket& operator=(Socket&& other) noexcept;

            bool create();
            bool bind(int port);
            bool listen(int backlog = 5);
            Socket accept();
            bool connect(const std::string& ip_address, int port);
    
            int send(const char* data, size_t length);
            int receive(char* buffer, size_t length);
    
            void close();
            socket_t get_handle() const { return sock_fd; }
            bool is_valid() const { return sock_fd != INVALID_SOCKET; }
        };

    } // namespace p2p
#endif // P2P_SOCKET_HPP