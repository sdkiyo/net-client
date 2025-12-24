#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

#include <netInterface.h>


uint8_t userCallback(const char *const pResponse, const uint16_t responseLen, char* pRequest, uint16_t* pCurrentRequestLen, const uint16_t requestMaxLen, void* pUserData, const uint8_t prevCallbackReturnValue)
{
	printf("USER CALLBACK START\n");
	printf("userData: %s\n", (char*)pUserData);
	printf("request max len: %d\n", requestMaxLen);
	printf("response len: %d\n", responseLen);
	printf("prev callback value: %d\n", prevCallbackReturnValue);
//	printf("response: %s\n", pResponse);

	if (prevCallbackReturnValue == CALLBACK_FIRST_TRY)// first try
	{
		snprintf(pRequest, requestMaxLen, "GET /api/v1/test HTTP/1.1\nHost: 1.1.1.1\n\n");
		*pCurrentRequestLen = (uint16_t)strlen(pRequest);

		printf("USER CALLBACK END (first try)\n");
		return 0;
	}
	else if (prevCallbackReturnValue != CALLBACK_FIRST_TRY && responseLen == 0)// NOT first try + no response
	{
		printf("USER CALLBACK END (no response)\n");
		return CALLBACK_CLOSE_CONNECTION;// close connection with server
	}
	else if (prevCallbackReturnValue != CALLBACK_FIRST_TRY && responseLen > 0)// get response
	{
		printf("BLUE" "server responce start: \n" "RESET_COLOR" "%s" "BLUE" "server responce end\n" "RESET_COLOR", pResponse);

		snprintf(pRequest, requestMaxLen, "GET /api/v1/test HTTP/2.0\nHost: sdlfkjsdf\n\n");
		*pCurrentRequestLen = (uint16_t)strlen(pRequest);

		printf("USER CALLBACK END (get response)\n");
		return CALLBACK_CLOSE_CONNECTION;
	}

	printf("USER CALLBACK END\n");
	return 0;
}


int main()
{
	fprintf(stderr, "TRANSPARENT_COLOR" "test1 start" "RESET_COLOR" "\n");


	void* netClientLib = dlopen("./libnet-client.so", RTLD_LAZY);

	if (netClientLib == nullptr)
	{
		fprintf(stderr, "RED" "failed to load https library" "RESET_COLOR" "\n");
		return -1;
	}

	PFN_httpsClientConnect httpsClientConnect = (PFN_httpsClientConnect) dlsym(netClientLib, "httpsClientConnect");


	char userData[] = "test data123";

	if (httpsClientConnect("1.1.1.1", userCallback, /*request max len ->*/ (uint16_t)1024, /*response max len -> */ (uint16_t)1024, (void*)userData) != 0)
	{
		fprintf(stderr, "RED" "httpsClientConnect() failed" "RESET_COLOR" "\n");
		return -1;
	}


	dlclose(netClientLib);


	fprintf(stderr, "RESET_COLOR" "TRANSPARENT_COLOR" "test1 end" "RESET_COLOR" "\n");
	return 0;
}
