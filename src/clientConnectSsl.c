#include <stdio.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netdb.h>
#include <netInterface.h>


// хотелось бы добавить подключение по прямому ip
// и подключение без ssl


int clientConnectSsl(const char *const pHostname, const uint16_t port, PFN_netUserCallback userCallback, const uint16_t requestMaxLen, const uint16_t responseMaxLen, void* pUserData)
{
	printf("\x1b[2m%s() start\x1b[0m\n", __func__);

	if (OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS, nullptr) != 1)
	{
		ERR_print_errors_fp(stderr);
		fprintf(stderr, "\x1b[31m%s(), line %d, 'OPENSSL_init_ssl() failed'\x1b[0m\n", __func__, __LINE__);
		return -1;
	}

	struct sockaddr_in socketAddrInfo = {
		.sin_family	= AF_INET,
		.sin_port	= htons(port)
	};

	const struct addrinfo hints = {
		.ai_family	= AF_INET,
		.ai_socktype	= SOCK_STREAM,
	};

	struct addrinfo* pHostAddrList = {};

	if (getaddrinfo(pHostname, nullptr, &hints, &pHostAddrList) != 0)
	{
		fprintf(stderr, "\x1b[31m%s(), line %d, 'getaddrinfo() failed, probably wrong host address'\x1b[0m\n", __func__, __LINE__);
		return -1;
	}

	SSL_CTX* pSslContext = SSL_CTX_new(TLS_client_method());

	if (pSslContext == nullptr)
	{
		SSL_CTX_free(pSslContext);
		freeaddrinfo(pHostAddrList);
		fprintf(stderr, "\x1b[31m%s(), line %d, 'failed to create SSl context'\x1b[0m\n", __func__, __LINE__);
		return -1;
	}

	//SSL_CTX_set_verify(pSslContext, SSL_VERIFY_PEER, NULL);

	if (SSL_CTX_set_default_verify_paths(pSslContext) != 1)
	{
		SSL_CTX_free(pSslContext);
		freeaddrinfo(pHostAddrList);
		fprintf(stderr, "\x1b[31m%s(), line %d, 'failed to set default verify paths for SSL context'\x1b[0m\n", __func__, __LINE__);
		return -1;
	}

	SSL* pSsl = nullptr;
	int socketFd = 0;

	for (const struct addrinfo* temp = pHostAddrList; temp != nullptr; temp = temp->ai_next)
	{
		socketAddrInfo.sin_addr.s_addr = ((const struct sockaddr_in *const)temp->ai_addr)->sin_addr.s_addr;

		// открывать socket

		// создавать + настраивать объект ssl

//		if (tryTlsConnect(&pSsl, &pSslContext, &socketFd, (const struct sockaddr *const)&socketAddrInfo, pHostname) == 0)
//		{
//			break;// success
//		}
		socketFd = socket(AF_INET, SOCK_STREAM, 0);

		if (socketFd == -1)
		{
			fprintf(stderr, "\x1b[31m%s(), line %d, 'socket() failed'\x1b[0m\n", __func__, __LINE__);
		}

		pSsl = SSL_new(pSslContext);

		if (pSsl == nullptr)
		{
			fprintf(stderr, "\x1b[31m" "SSL_new() failed\x1b[0m\n");
		}

		if (SSL_set_tlsext_host_name(pSsl, pHostname) != 1)
		{
			fprintf(stderr, "\x1b[31m" "SSL_set_tlsext_host_name() failed\x1b[0m\n");
		}

		if (SSL_set_fd(pSsl, socketFd) != 1)
		{
			fprintf(stderr, "\x1b[31m" "SSL_set_fd() failed\x1b[0m\n");
		}

		if (connect(socketFd, (const struct sockaddr *const)&socketAddrInfo, sizeof(socketAddrInfo)) != 0)
		{
			fprintf(stderr, "\x1b[31m" "connect() failed\x1b[0m\n");
		}

		if (SSL_connect(pSsl) != 1)
		{
			fprintf(stderr, "\x1b[31m" "SSL_connect() failed\x1b[0m\n");
		}

		long verifyResult = SSL_get_verify_result(pSsl);

		if (verifyResult != X509_V_OK)
		{
			fprintf(stderr, "\x1b[31m" "SSL_get_verify_result() failed: %ld" "\x1b[0m" "\n", verifyResult);
		}
		break;

		// закрывать socket + удалять объект ssl

		fprintf(stderr, "\x1b[33m%s(), line %d, 'connection failed, trying to connect the next ip address'\x1b[0m\n", __func__, __LINE__);
	}

	if (pSsl == nullptr)
	{
		SSL_CTX_free(pSslContext);
		freeaddrinfo(pHostAddrList);
		fprintf(stderr, "\x1b[31m%s(), line %d, 'unable to connect anyone ip'\x1b[0m\n", __func__, __LINE__);
		return -1;
	}

	char* pResponseBuffer	= malloc(sizeof(*pResponseBuffer) * responseMaxLen);
	char* pRequestBuffer	= malloc(sizeof(*pRequestBuffer) * requestMaxLen);

	uint16_t currentResponseLen	= 0;
	uint16_t currentRequestLen	= 0;
	uint8_t callbackReturnValue	= CALLBACK_FIRST_TRY;

	while (callbackReturnValue != CALLBACK_CLOSE_CONNECTION)
	{
		callbackReturnValue = userCallback(pResponseBuffer, currentResponseLen, pRequestBuffer, &currentRequestLen, requestMaxLen, pUserData, callbackReturnValue);
		if (callbackReturnValue == CALLBACK_CLOSE_CONNECTION)
		{
			break;
		}

		if (currentRequestLen > 0)
		{
			SSL_write(pSsl, pRequestBuffer, currentRequestLen);
			currentRequestLen = 0;
		}

		currentResponseLen = 0;// если сервер ничего не прислал то SSL_read() не зануляет currentResponseLen

		if (callbackReturnValue == SKIP_NEXT_SERVER_RESPONSE)
		{
			continue;
		}

		// временный костыль для фикса лимита в 1369 байт за раз
		currentResponseLen = 0;
		int temp = 0;
		while (true)
		{
			currentResponseLen += temp;
			temp = 0;
			temp += (uint16_t) SSL_read(pSsl, pResponseBuffer + currentResponseLen, responseMaxLen);
			if (temp <= 0)
			{
				printf("\033[31m" "SSL_read() error\033[0m\n");
				break;
			}
			if (temp < 1369)
			{
				currentResponseLen += temp;
				break;
			}
	//		printf("responseBuffer такой типа: %s\n", pResponseBuffer);
	//		printf("\x1b[33msdlfkjsdf\x1b[0m\n");
		}
		// временный костыль для фикса лимита в 1369 байт за раз
	}

	free(pResponseBuffer);
	free(pRequestBuffer);

	SSL_shutdown(pSsl);
	SSL_free(pSsl);
	SSL_CTX_free(pSslContext);

	freeaddrinfo(pHostAddrList);

	close(socketFd);


	printf("\x1b[2m%s() end\x1b[0m\n", __func__);
	return 0;
}
