
#define WS_MASKED 0x80


static inline int createWebsocketFrame_8(const char *const pInputData, const uint8_t inputDataLen, char *const pOutputFrame, uint8_t *const pOutputFrameCurrentLen)
{
	if (inputDataLen > 125)
	{
		fprintf(stderr, "\x1b[31m%s(), line %d, 'inputDataLen too big, please choose other createWebsocketFrame function'\x1b[0m\n", __func__, __LINE__);
		return -1;
	}

	*pOutputFrameCurrentLen = inputDataLen + 6;

	pOutputFrame[0] = 0x81;// fin = 1 (0x80), opcode = text (0x1)

	pOutputFrame[1] = WS_MASKED | inputDataLen;

	pOutputFrame[2] = 'a';// FIXME test mask values
	pOutputFrame[3] = 'b';// FIXME test mask values
	pOutputFrame[4] = 'c';// FIXME test mask values
	pOutputFrame[5] = 'd';// FIXME test mask values

	for (uint8_t i = 0; i < inputDataLen; i++)
	{
		pOutputFrame[i + 6] = pInputData[i] ^ pOutputFrame[(i & 0x3) + 2];// я хз что за 0x3 но оно работает
	}

	return 0;
}


// если inputDataLen впритык к лимиту uint16_t то pOutputFrameCurrentLen поскольку она тоже uint16_t может быть переполнена и будет баг
static inline int createWebsocketFrame_16(const char *const pInputData, const uint16_t inputDataLen, char *const pOutputFrame, uint16_t *const pOutputFrameCurrentLen, const uint16_t outputFrameMaxLen)
{
	if (outputFrameMaxLen < *pOutputFrameCurrentLen)
	{
		fprintf(stderr, "\x1b[31m%s(), line %d, 'frame too big, please change outputFrameMaxLen'\x1b[0m\n", __func__, __LINE__);
		return -1;
	}

	*pOutputFrameCurrentLen = inputDataLen + 8;

	pOutputFrame[0] = 0x81;// fin = 1 (0x80), opcode = text (0x1)

	pOutputFrame[1] = (char) (WS_MASKED | 126);
	pOutputFrame[2] = (inputDataLen & 0xFF00) >> 8;
	pOutputFrame[3] = inputDataLen & 0x00FF;

	pOutputFrame[4] = 'a';// FIXME test mask values
	pOutputFrame[5] = 'b';// FIXME test mask values
	pOutputFrame[6] = 'c';// FIXME test mask values
	pOutputFrame[7] = 'd';// FIXME test mask values

	for (uint16_t i = 0; i < inputDataLen; i++)
	{
		pOutputFrame[i + 8] = pInputData[i] ^ pOutputFrame[(i & 0x3) + 4];// я хз что за 0x3 но оно работает
	}

	return 0;
}
