#include <tryConnect.h>
#include <netInterface.h>


int httpsClientConnect(const char *const pHostname, PFN_httpsUserCallback userCallback, const uint16_t requestMaxLen, const uint16_t responseMaxLen, void* pUserData)
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
		.sin_port	= htons(443)
	};

	const struct addrinfo hints = {
		.ai_family	= AF_INET,
		.ai_socktype	= SOCK_STREAM,
	};

	struct addrinfo* pHostAddrList = {};

	if (getaddrinfo(pHostname, "443", &hints, &pHostAddrList) != 0)
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

		if (tryTlsConnect(&pSsl, &pSslContext, &socketFd, (const struct sockaddr *const)&socketAddrInfo, pHostname) == 0)
		{
			break;
		}

		fprintf(stderr, "\x1b[33m%s(), line %d, 'connection failed, trying to connect the next ip address'\x1b[0m\n", __func__, __LINE__);
	}

	if (pSsl == nullptr)
	{
		SSL_CTX_free(pSslContext);
		freeaddrinfo(pHostAddrList);
		fprintf(stderr, "\x1b[31m%s(), line %d, 'unable to connect anyone ip'\x1b[0m\n", __func__, __LINE__);
		return -1;
	}

	char* pResponseBuffer = malloc(sizeof(*pResponseBuffer) * responseMaxLen);
	char* pRequestBuffer = malloc(sizeof(*pRequestBuffer) * requestMaxLen);

	uint16_t currentResponseLen = 0;
	uint16_t currentRequestLen = 0;
	uint8_t callbackReturnValue = CALLBACK_FIRST_TRY;

	while(callbackReturnValue != CALLBACK_CLOSE_CONNECTION)
	{
		callbackReturnValue = userCallback(pResponseBuffer, currentResponseLen, pRequestBuffer, &currentRequestLen, requestMaxLen, pUserData, callbackReturnValue);

		if (currentRequestLen > 0)
		{
			SSL_write(pSsl, pRequestBuffer, currentRequestLen);
			currentRequestLen = 0;
		}

		currentResponseLen = 0;// если сервер ничего не прислал то SSL_read_ex() не зануляет currentResponseLen

		if (callbackReturnValue == SKIP_NEXT_SERVER_RESPONSE)
		{
			continue;
		}

		currentResponseLen = (uint16_t) SSL_read(pSsl, pResponseBuffer, responseMaxLen);
		if (currentResponseLen <= 0)
		{
			printf("\033[31m" "SSL_read() error\033[0m\n");
		}
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
