#include <tryConnect.h>


int tryTlsConnect(SSL** ppSsl, SSL_CTX** ppSslContext, int *const pSocketFd, const struct sockaddr *const pSocketAddrInfo, const char *const pHostname)
{
	*pSocketFd = socket(AF_INET, SOCK_STREAM, 0);

	if (*pSocketFd == -1)
	{
		fprintf(stderr, "\x1b[31m%s(), line %d, 'socket() failed'\x1b[0m\n", __func__, __LINE__);
		return -1;
	}

	*ppSsl = SSL_new(*ppSslContext);

	if (*ppSsl == nullptr)
	{
		close(*pSocketFd);
		fprintf(stderr, "\x1b[31m" "SSL_new() failed\x1b[0m\n");
		return -1;
	}

	if (SSL_set_tlsext_host_name(*ppSsl, pHostname) != 1)
	{
		SSL_shutdown(*ppSsl);
		SSL_free(*ppSsl);
		*ppSsl = nullptr;
		close(*pSocketFd);
		fprintf(stderr, "\x1b[31m" "SSL_set_tlsext_host_name() failed\x1b[0m\n");
		return -1;
	}

	if (SSL_set_fd(*ppSsl, *pSocketFd) != 1)
	{
		SSL_shutdown(*ppSsl);
		SSL_free(*ppSsl);
		*ppSsl = nullptr;
		close(*pSocketFd);
		fprintf(stderr, "\x1b[31m" "SSL_set_fd() failed\x1b[0m\n");
		return -1;
	}

	if (connect(*pSocketFd, pSocketAddrInfo, sizeof(*pSocketAddrInfo)) != 0)
	{
		SSL_shutdown(*ppSsl);
		SSL_free(*ppSsl);
		*ppSsl = nullptr;
		close(*pSocketFd);
		fprintf(stderr, "\x1b[31m" "connect() failed\x1b[0m\n");
		return -1;
	}

	if (SSL_connect(*ppSsl) != 1)
	{
		SSL_shutdown(*ppSsl);
		SSL_free(*ppSsl);
		*ppSsl = nullptr;
		close(*pSocketFd);
		fprintf(stderr, "\x1b[31m" "SSL_connect() failed\x1b[0m\n");
		return -1;
	}

	long verifyResult = SSL_get_verify_result(*ppSsl);

	if (verifyResult != X509_V_OK)
	{
		SSL_shutdown(*ppSsl);
		SSL_free(*ppSsl);
		*ppSsl = nullptr;
		close(*pSocketFd);
		fprintf(stderr, "\x1b[31m" "SSL_get_verify_result() failed: %ld" "\x1b[0m" "\n", verifyResult);
		return -1;
	}

	return 0;
}
