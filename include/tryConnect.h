#pragma once
#ifndef NET_CLIENT_INCLUDE_TRY_CONNECT_H_
#define NET_CLIENT_INCLUDE_TRY_CONNECT_H_


#include <stdio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netdb.h>


int tryTlsConnect(SSL** ppSsl, SSL_CTX** ppSslContext, int *const pSocketFd, const struct sockaddr *const pSocketAddrInfo, const char *const pHostname);




#endif // NET_CLIENT_INCLUDE_TRY_CONNECT_H_
