#include "tcp.h"
#include <errno.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <string>
#include <sstream>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <thread>
#include <set>

#include <unistd.h>

namespace tcp {

	std::set<uint16_t> listening_ports;

	enum {
		TCP_OK = 0,
		TCP_ERROR = -1,
		TCP_TIMEOUT = -2
	};

	struct util { 

		static int select(int& sockfd, int to_sec) {
			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(sockfd, &fds);

			struct timeval tv = as_timeval(to_sec);
			int n = ::select(sockfd + 1, &fds, NULL, NULL, &tv);
			return (n == -1 ? sockfd = -1, TCP_ERROR : n == 0 ? TCP_TIMEOUT : TCP_OK);
		}

		static int done(int sockfd, int flags, int error) {
			fcntl(sockfd, F_SETFL, flags);
			if(error) {
				::close(sockfd);
				errno = error;
				return -1;
			}
			return 0;
		}

		static struct timeval as_timeval(int to_sec) {
			struct timeval res;
			res.tv_sec = to_sec / 1000;
			res.tv_usec= (to_sec % 1000) * 1000;
			return res;
		}

		static int connect_nonb(int sockfd, const sockaddr* saptr, socklen_t salen, int to_sec) {
			int flags, n, error;
			socklen_t len;
			fd_set rset, wset;
			struct timeval tval;

			flags = fcntl(sockfd, F_GETFL, 0);
			fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

			error = 0;
			if((n=::connect(sockfd, (struct sockaddr*)saptr, salen)) < 0) 
				if(errno != EINPROGRESS)
					return false;

			if(n == 0) {
				return done(sockfd, flags, error);
			}

			FD_ZERO(&rset);
			FD_SET(sockfd, &rset);
			wset = rset;
			tval = as_timeval(to_sec);
			if((n=::select(sockfd + 1, &rset, &wset, NULL, to_sec > 0 ? &tval : NULL)) == 0) {
				::close(sockfd);
				errno = ETIMEDOUT;
				return -1;
			}

			if(FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
				len = sizeof(error);
				if(::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
					return -1;
				} 
			} else {
				return -1;
			}
			return done(sockfd, flags, error);
		}               
	};

	bool open(int& sockfd, const std::string& ip, uint16_t port, int to_sec) {
		addrinfo hints, *server_info;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family     = AF_UNSPEC;
		hints.ai_socktype   = SOCK_STREAM;
		hints.ai_flags      = AI_PASSIVE;

		if(getaddrinfo(ip.c_str(), std::to_string(port).c_str(), &hints, &server_info) != 0) {
			return sockfd = -1, false;
		} 

		sockfd = ::socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

		bool res = tcp::util::connect_nonb(sockfd, server_info->ai_addr, server_info->ai_addrlen, to_sec) == 0;
		freeaddrinfo(server_info);

		return res;
	}

	bool close(int& sockfd) {
		if(sockfd != -1) {
			::shutdown(sockfd, SHUT_RDWR);
			::close(sockfd);
			sockfd = -1;
		}
		return true;
	}

	int send(int sockfd, const void* src, int len) {
		int sent = 0;
		do {
			int s = ::send(sockfd, &((char*)src)[sent], len - sent, 0);
			if(s == 0) return  0;
			if(s  < 0) return -1;

			sent += s;                    
		} while(sent <= len);
		/**
		  if(::shutdown(sockfd, SHUT_WR) < 0) {
		  return -1;
		  }
		  */
		return sent;
	}

	int set_blocking(int sockfd, bool val) {

		int flags = fcntl(sockfd, F_GETFL);
		if(!val) {
			flags |= O_NONBLOCK;
		} else {
			flags &= ~O_NONBLOCK;
		}

		if(fcntl(sockfd, F_SETFL, flags) < 0) {
			return -1;
		}
		return 0;
	}

	int receive(int sockfd, void* dst, int len, int to_sec) {
		if(sockfd < 0)
			return false;
		bool is_receiving = true;
		int bytes_recv = 0;
		while(is_receiving) {
			//if(to_sec > 0) {
			if(util::select(sockfd, to_sec) != TCP_OK) {
				return -1;
			}
			//}

			int rc = ::recv(sockfd, &((char*)dst)[bytes_recv], len - bytes_recv, 0);

			if(rc == 0) return  0;
			if(rc < 0)  return -1;

			bytes_recv += rc;

			is_receiving = bytes_recv != len;
		}
		return bytes_recv;
	}

	bool listen(int& sockfd, uint16_t port, const listen_callback& cb) {

		sockfd = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		struct sockaddr_in sa;
		::memset(&sa, 0, sizeof(struct sockaddr_in));
		sa.sin_family       = AF_INET;
		sa.sin_port         = htons(port);
		sa.sin_addr.s_addr  = INADDR_ANY;

		if(::bind(sockfd, (struct sockaddr*)&sa, sizeof(sa)) == - 1) {
			::close(sockfd);
			return false;
		}
		if(::listen(sockfd, 1024)) {
			::close(sockfd);
			return false;
		}

		volatile bool is_ready = false;

		std::thread([&] (volatile bool* ready_, int master_fd, const listen_callback& cb) {
				*ready_ = true;
				for(;;) {
				struct sockaddr_in sa;
				socklen_t sa_len = sizeof(sa);
				memset(&sa, 0, sa_len);

				int fd = ::accept(master_fd, (struct sockaddr*)&sa, &sa_len);
				if(fd < 0) {
				continue;
				}
				const char* sa_addr = inet_ntoa(sa.sin_addr);
				uint16_t sa_port    = ntohs(sa.sin_port);
				if(cb != nullptr) 
				cb(master_fd, fd, sa_addr, sa_port);
				}
				}, &is_ready, sockfd, cb).detach();

		while(!is_ready) 
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		listening_ports.insert(port);
		return true;
	} 
} // tcp
