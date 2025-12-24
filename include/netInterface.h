#pragma once
#ifndef NET_CLIENT_INCLUDE_NET_INTERFACE_H_
#define NET_CLIENT_INCLUDE_NET_INTERFACE_H_


#include <stdint.h>


#define SKIP_NEXT_SERVER_RESPONSE 4
#define CALLBACK_FIRST_TRY 2
#define CALLBACK_CLOSE_CONNECTION 1


typedef uint8_t (*PFN_httpsUserCallback)(
	const char *const	pResponse,
	const uint16_t		currentResponseLen,
	char*			pRequest,
	uint16_t*		pCurrentRequestLen,
	const uint16_t		requestMaxLen,
	void*			pUserData,
	const uint8_t		prevCallbackReturnValue);


typedef int (*PFN_httpsClientConnect)(
	const char *const		pHostname,
	PFN_httpsUserCallback		userCallback,
	const uint16_t			requestMaxLen,
	const uint16_t			responseMaxLen,
	void*				pUserData);




#endif // NET_CLIENT_INCLUDE_NET_INTERFACE_H_
