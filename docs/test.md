# Documentation
text text text
## httpsClientConnect()
~~~C
int httpsClientConnect(
    const char *const                    pHostname,
    const HttpMultipleHeaders *const     pSentHeaders,

    int (responseCallback)(const char *const, const uint16_t, void*),

    void*                                pUserData,
    const uint16_t                       serverResponseMaxLen)
~~~
- `pHostname` server name or ip address
- `pSentHeaders` [headers](ссылка_на_раздел,_да,_да,_да.)
- `responseCallback` user [response callback](ссылку_на_раздел_response_callback_в_документации)
- `pUserData` any user data
- `serverResponseMaxLen` от 1 до 65536
___
Функция httpsClientConnect() создаёт https соединение с сервером, отправляет https заголовки, ждёт запрос от сервера, вызывает user [responseCallback]() передавая ему ответ сервера. После чего закрывает https соединение с сервером. (запятые, ставились, по, вайбу)
## httpCreateMultipleHeaders()
~~~C
void httpCreateMultipleHeaders(
    const HttpMultipleHeadersCreateInfo *const    pMultipleHeadersCreateInfo,
    HttpMultipleHeaders*                          pMultipleHeaders)
~~~
- `pMultipleHeadersCreateInfo` input [headers create info](ссылка_на_структуру)
- `pMultipleHeaders` out [headers](ссылка_на_структуру)
___
create the headers, idk. sdfsdlkfjsdfldsfsdfsdfsdfsdfsdcvcvxsdfsdlkfjsdfldsfsdfsdfsdfsdfsdcvcvxsdfsdlkfjsdfldsfsdfsdfsdfsdfsdcvcvxsdfsdlkfjsdfldsfsdfsdfsdfsdfsdcvcvxsdfsdlkfjsdfldsfsdfsdfsdfsdfsdcvcvxsdfsdlkfjsdfldsfsdfsdfsdfsdfsdcvcvx
## user response callback
~~~C
int responseCallback(
    const char *const    pServerResponse,
    const uint16_t       pResponseLen,
    void*                pUserData)
~~~
- slkdfj
- lskdfj
- sdklj
- sdjflk
___
\"imagine cool description\" sdfsdlkfjsdfldsfsdfsdfsdfsdfsdcvcvxsdfsdlkfjsdfldsfsdfsdfsdfsdfsdcvcvxsdfsdlkfjsdfldsfsdfsdfsdfsdfsdcvcvxsdfsdlkfjsdfldsfsdfsdfsdfsdfsdcvcvx
## headers create info
~~~C
typedef struct HttpMultipleHeadersCreateInfo {
	const char *const *    ppHeads;
	const char *const *    ppJsons;
	uint8_t                headersCount;
} HttpMultipleHeadersCreateInfo;
~~~
`ppHeads` string array top headers [мб ссылка на examples](sdf)\
`ppJsons` string array body headers [мб ссылка на examples](hgfd)\
`headersCount` number of headers
___
## headers
~~~C
typedef struct HttpMultipleHeaders {
	char**		ppHeaders;
	uint16_t*	pHeadersLen;
	uint8_t		headersCount;
} HttpMultipleHeaders;
~~~
`ppHeaders` headers string array\
`pHeadersLen` array with headers string length\
`headersCount` number of headers
___
## Examples
~~~C
#include <stdio.h>
#include <dlfcn.h> /* for dlopen() and dlclose() */

#include "https_interface.h"


// disconnect if return -1
int responseCallback(const char *const pServerResponse, const uint16_t pResponseLen, void* pUserData)
{
	printf("userData: %s\n", (char*)pUserData);

	printf("response len: %d\n", pResponseLen);

	printf("server responce start:\n");
	printf("%s", pServerResponse);
	printf("server responce end\n");

	return 0;
}


int main()
{
	// load library with dlopen()
	void* httpsClientLib = dlopen("./path/to/library", RTLD_LAZY);// path must begin with "./" or "../"

	// load functions from library
	httpsClientConnect = dlsym(httpsClientLib, "httpsClientConnect");
	httpCreateMultipleHeaders = dlsym(httpsClientLib, "httpCreateMultipleHeaders");

	// create head for http headers
	const char *const heads[] = {
		"GET /api/v1/test HTTP/1.1\nHost: example.site.com",
		"GET /api/v1/test HTTP/1.1\nHost: example.site.com\nContent-Type: application/json"
	};

	// create body for http headers
	const char *const jsons[] = {
		"",
		"JSON"
	};

	// create CreateInfo for headers
	HttpMultipleHeadersCreateInfo multipleHeadersCreateInfo = {};
	multipleHeadersCreateInfo.ppHeads	= heads;
	multipleHeadersCreateInfo.ppJsons	= jsons;
	multipleHeadersCreateInfo.headersCount	= 2;

	// create headers
	HttpMultipleHeaders sentHeaders = {};

	// fill headers
	httpCreateMultipleHeaders(&multipleHeadersCreateInfo, &sentHeaders);

	char userData[] = "test user-data123";

	// connect to server
	httpsClientConnect("example.site.com", &sentHeaders, responseCallback, (void*)userData, /* server response max length -> */ 1024);

	// close library with dlclose()
	dlclose(httpsClientLib);

	return 0;
}
~~~
slksdfklsdjfd\
dslfkjsd\
sdfjlsdkfsd\
sdjlfsdf\
sdlfkjsdljwrlkwersds\
23rwefsdsdf\
sdfsdfsd
___
end.
