#include <stdio.h>
#include <dlfcn.h> /* for dlopen() and dlclose() */
#include <string.h>

#include <netInterface.h>


uint8_t userCallback(const char *const pResponse, const uint16_t responseLen, char* pRequest, uint16_t* pCurrentRequestLen, const uint16_t requestMaxLen, void* pUserData, const uint8_t prevCallbackReturnValue)
{
	printf("pUserData: %s\n", (char*)pUserData);

	if (prevCallbackReturnValue == CALLBACK_FIRST_TRY)
	{
		/* first try */
		snprintf(pRequest, requestMaxLen, "GET /api/v1/example HTTP/1.1\nHost: example.site.com\nContent-Length: 1234\nContent-Type: application/json\n\n{json: \"example\"}\0");
		*pCurrentRequestLen = (uint16_t)strlen(pRequest);

		return 0;
	}
	else if (prevCallbackReturnValue != CALLBACK_FIRST_TRY && responseLen == 0)
	{
		/* NOT first try + no response */
		return CALLBACK_CLOSE_CONNECTION;
	}
	else if (prevCallbackReturnValue != CALLBACK_FIRST_TRY && responseLen > 0)
	{
		/* get response from server */

		printf("get response from server:\n");
		for (uint16_t i = 0; i < responseLen; i++)
		{
			printf("%c", pResponse[i]);
		}

		return CALLBACK_CLOSE_CONNECTION;
	}

	return CALLBACK_CLOSE_CONNECTION;
}


int main()
{
	// load library with dlopen()
	void* netClientLib = dlopen("./path/to/library", RTLD_LAZY);// path must begin with "./" or "../"

	if (netClientLib == nullptr)
	{
		fprintf(stderr, "failed to load library\n");
		return -1;
	}

	// load functions from library
	PFN_httpsClientConnect httpsClientConnect = (PFN_httpsClientConnect) dlsym(netClientLib, "httpsClientConnect");

	char userData[] = "test data123";

	// connect to server
	if (httpsClientConnect("1.1.1.1", userCallback, /*request max len ->*/ (uint16_t)1024, /*response max len -> */ (uint16_t)1024, (void*)userData) != 0)
	{
		fprintf(stderr, "RED" "httpsClientConnect() failed" "RESET_COLOR" "\n");
		return -1;
	}

	// close library with dlclose()
	dlclose(netClientLib);

	return 0;
}
