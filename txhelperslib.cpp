//#ifndef WINCE
//	#include <share.h>
//#endif


#include "txhelperslib.h"

FILE	*debugFile_txcrdwrppr;	// file pointer for debug output
bool	bLogging = false;
wchar_t	szGPath[MAX_PATH] = L"";

void SetLogPath(wchar_t* szPath)
{
	wcscpy_s(szGPath, szPath);
}
void ToggleLogging(bool bToggle)
{
	bLogging = bToggle;	// bLogging is tested in PrintDebugInfo()
}

LPVOID WINAPI Alloc(__in SIZE_T cBytes)
{
    return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cBytes);
}

void WINAPI Free(__in LPVOID pvMem)
{
    HeapFree(GetProcessHeap(), 0, pvMem);
}

inline LPVOID Allocate(size_t cb)
{
    LPVOID pv = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cb);
    if (NULL == pv)
        throw E_OUTOFMEMORY;
    return pv;
}

inline LPVOID Reallocate(LPVOID pv, size_t cb)
{
    LPVOID pvNew = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pv, cb);
    if (NULL == pvNew)
        throw E_OUTOFMEMORY;
    return pvNew;
}

int countBits(unsigned char byte){
    int count = 0;
    for(int i = 0; i < 8; i++)
        count += (byte >> i) & 0x01; // Shift bit[i] to the first position, and mask off the remaining bits.
    return count;
}

BYTE* StringToBytPtr(LPTSTR szText, DWORD dwLen)
{
	BYTE*	bpRet = new byte[dwLen];
	DWORD	dwCntr;

	// move reader string to byte array
	for(dwCntr=0; dwCntr<dwLen; dwCntr++)
		bpRet[dwCntr] = szText[dwCntr] & 0x00FF;

	return bpRet;
}

DWORD ActivatePCSC(LPTSTR szReader, DWORD dwShareMode, TX_PCSC_INFO* pstPCSCInfo)
{
	DWORD	dwActiveProtocol = 0, dwPreferredProtocols = 0;
	HRESULT	hr = 0;

	// Create a PC/SC information structure.
    //TX_PCSC_INFO *pstPCSCInfo = new TX_PCSC_INFO;

	PrintDebugInfo(FILE_LOG, L"ActivatePCSC()::szReader: %s",szReader);

    // Establish the resource manager context.
    if (SCARD_S_SUCCESS != (hr = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &pstPCSCInfo->hContext)))
    {
		//PrintDebugInfo(FILE_LOG, L"ActivatePCSC()::SCardEstablishContext() failed: %X", hr);
        return hr;
    }

	PrintDebugInfo(FILE_LOG, L"ActivatePCSC()::SCardEstablishContext() success");

	if(dwShareMode == SCARD_SHARE_DIRECT)
		dwPreferredProtocols = 0;
	else
		dwPreferredProtocols = SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1;

	// Try to connect to a card in the current reader. The preferred protocol is T=1.
	if (SCARD_S_SUCCESS == (hr = SCardConnect(pstPCSCInfo->hContext, szReader, dwShareMode, dwPreferredProtocols, &pstPCSCInfo->hCard, &dwActiveProtocol)))
    {
		PrintDebugInfo(FILE_LOG, L"ActivatePCSC()::SCardConnect() success");

        // Assign the protocol header structure  
		if(dwActiveProtocol == SCARD_PROTOCOL_T0)
		{
			//MessageBox(NULL,L"T0",L"",MB_OK);
			pstPCSCInfo->pioPCI = (LPSCARD_IO_REQUEST)SCARD_PCI_T0;
		}
		else
		{
			//MessageBox(NULL,L"T1",L"",MB_OK);
			pstPCSCInfo->pioPCI = (LPSCARD_IO_REQUEST)SCARD_PCI_T1;
		}

		// Assign the reader
		pstPCSCInfo->lpstrReader = szReader;

        // Return the pointer to the PC/SC information structure.
        return SCARD_S_SUCCESS;
    }

	PrintDebugInfo(FILE_LOG, L"ActivatePCSC()::SCardConnect() failed: 0x%X", hr);

    // Release the memory block automatically allocated for the multi-string of readers.
//#ifndef WINCE
//	SCardFreeMemory(pstPCSCInfo->hContext, pstPCSCInfo->lpstrReader);
//#endif

    // Release the resource manager context.
    SCardReleaseContext(pstPCSCInfo->hContext);

    return hr;
}
//TX_PCSC_INFO* ActivatePCSC(LPTSTR szReader)
//{
//	DWORD	eActiveProtocol;
//	HRESULT	hr = 0;
//
//	// Create a PC/SC information structure.
//    TX_PCSC_INFO *pstPCSCInfo = new TX_PCSC_INFO;
//
//	PrintDebugInfo(FILE_LOG, L"ActivatePCSC()::szReader: %s",szReader);
//
//    // Establish the resource manager context.
//    if (SCARD_S_SUCCESS != (hr = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &pstPCSCInfo->hContext)))
//    {
//		//PrintDebugInfo(FILE_LOG, L"ActivatePCSC()::SCardEstablishContext() failed: %X", hr);
//        return NULL;
//    }
//
//	PrintDebugInfo(FILE_LOG, L"ActivatePCSC()::SCardEstablishContext() success");
//
//	// Try to connect to a card in the current reader. The preferred protocol is T=1.
//    if (SCARD_S_SUCCESS == (hr = SCardConnect(pstPCSCInfo->hContext, szReader, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &pstPCSCInfo->hCard, &eActiveProtocol)))
//    {
//		PrintDebugInfo(FILE_LOG, L"ActivatePCSC()::SCardConnect() success");
//
//        // Assign the standard protocol header structure for the T=1 protocol.
//        pstPCSCInfo->pioPCI = (LPSCARD_IO_REQUEST)SCARD_PCI_T1;
//
//		// Assign the reader
//		pstPCSCInfo->lpstrReader = szReader;
//
//        // Return the pointer to the PC/SC information structure.
//        return pstPCSCInfo;
//    }
//
//	PrintDebugInfo(FILE_LOG, L"ActivatePCSC()::SCardConnect() failed: %X", hr);
//
//    // Release the memory block automatically allocated for the multi-string of readers.
//    SCardFreeMemory(pstPCSCInfo->hContext, pstPCSCInfo->lpstrReader);
//
//    // Release the resource manager context.
//    SCardReleaseContext(pstPCSCInfo->hContext);
//
//    return NULL;
//}

void DeactivatePCSC(TX_PCSC_INFO * (& pstPCSCInfo))
{
	DWORD dwRet = 0;

    if (pstPCSCInfo != NULL)
    {
        // Disonnect from the card.
		if(pstPCSCInfo->hCard != 0)
		{
			dwRet = SCardDisconnect(pstPCSCInfo->hCard, SCARD_LEAVE_CARD);
			//SCARD_LEAVE_CARD
			//SCARD_RESET_CARD
			//SCARD_UNPOWER_CARD
			//SCARD_EJECT_CARD 

			//dwRet = SCardDisconnect(pstPCSCInfo->hCard, SCARD_RESET_CARD);
			PrintDebugInfo(FILE_LOG, L"DeactivatePCSC()::SCardDisconnect()=0x%.2X", dwRet);
		}

		// Release the resource manager context.
		if(pstPCSCInfo->hContext != 0)
			SCardReleaseContext(pstPCSCInfo->hContext);

        delete pstPCSCInfo;
        pstPCSCInfo = NULL;
    }
}


//void PCSCErrorMsg(LONG lReturn1, TCHAR* szFunction, SCARDCONTEXT hCtx)
void PCSCErrorMsg(LONG lReturn1, TCHAR* szFunction)
{
	TCHAR	szOutput[256] = {0}, 
			szStatusByte[3] = {0},
			szError[64] = {0};
	int		iOutputSize = sizeof(szOutput)/sizeof(szOutput[0]), 
			iErrSize = sizeof(szError)/sizeof(szError[0]);

	if (lReturn1 < 0)
	{
		switch(lReturn1)
		{
			case 0x80100001:
				#ifndef WINCE
					wcscpy_s(szError,iErrSize,L"SCARD_F_INTERNAL_ERROR");
				#else
					wcscpy(szError,L"SCARD_F_INTERNAL_ERROR");
				#endif
				break;
			case 0x80100002:
				#ifndef WINCE
					wcscpy_s(szError,iErrSize,L"SCARD_E_CANCELLED");
				#else
					wcscpy(szError,L"SCARD_E_CANCELLED");
				#endif
				break;

			case 0x80100003:
				#ifndef WINCE
					wcscpy_s(szError,iErrSize,L"SCARD_E_INVALID_HANDLE");
				#else
					wcscpy(szError,L"SCARD_E_INVALID_HANDLE");
				#endif
				break;
			case 0x80100004:
				#ifndef WINCE
				   wcscpy_s(szError,iErrSize,L"SCARD_E_INVALID_PARAMETER");
				#else
					wcscpy(szError,L"SCARD_E_INVALID_PARAMETER");
				#endif
					break;

			case 0x80100005:
				#ifndef WINCE
					wcscpy_s(szError,iErrSize,L"SCARD_E_INVALID_TARGET");
				#else
					wcscpy(szError,L"SCARD_E_INVALID_TARGET");
				#endif				
				break;

			case 0x80100006:
				#ifndef WINCE
				   wcscpy_s(szError,iErrSize,L"SCARD_E_NO_MEMORY");
				#else
					wcscpy(szError,L"SCARD_E_NO_MEMORY");
				#endif	
				break;

			case 0x80100007:
				#ifndef WINCE
					wcscpy_s(szError,iErrSize,L"SCARD_F_WAITED_TOO_LONG");
				#else
					wcscpy(szError,L"SCARD_F_WAITED_TOO_LONG");
				#endif	
				break;

			case 0x80100008:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_INSUFFICIENT_BUFFER");
				#else
					wcscpy(szError,L"SCARD_E_INSUFFICIENT_BUFFER");
				#endif	
				break;

			case 0x80100009:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_UNKNOWN_READER");
				#else
					wcscpy(szError,L"SCARD_E_UNKNOWN_READER");
				#endif	
				break;

			case 0x8010000A:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_TIMEOUT");
				#else
					wcscpy(szError,L"SCARD_E_TIMEOUT");
				#endif	
				break;

			case 0x8010000B:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_SHARING_VIOLATION");
				#else
					wcscpy(szError,L"SCARD_E_SHARING_VIOLATION");
				#endif	
				break;

			case 0x8010000C:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_NO_SMARTCARD");
				#else
					wcscpy(szError,L"SCARD_E_NO_SMARTCARD");
				#endif	
				break;

			case 0x8010000D:
				#ifndef WINCE	
					wcscpy_s(szError,iErrSize,L"SCARD_E_UNKNOWN_CARD");
				#else
					wcscpy(szError,L"SCARD_E_UNKNOWN_CARD");
				#endif	
				break;

			case 0x8010000E:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_CANT_DISPOSE");
				#else
					wcscpy(szError,L"SCARD_E_CANT_DISPOSE");
				#endif	
				break;

			case 0x8010000F:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_PROTO_MISMATCH");
				#else
					wcscpy(szError,L"SCARD_E_PROTO_MISMATCH");
				#endif	
				break;

			case 0x80100010:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_NOT_READY");
				#else
					wcscpy(szError,L"SCARD_E_NOT_READY");
				#endif
				break;

			case 0x80100011:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_INVALID_VALUE");
				#else
					wcscpy(szError,L"SCARD_E_INVALID_VALUE");
				#endif
				break;

			case 0x80100012:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_SYSTEM_CANCELLED");
				#else
					wcscpy(szError,L"SCARD_E_SYSTEM_CANCELLED");
				#endif
				break;

			case 0x80100013:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_F_COMM_ERROR");
				#else
					wcscpy(szError,L"SCARD_F_COMM_ERROR");
				#endif
				break;

			case 0x80100014:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_F_UNKNOWN_ERROR");
				#else
					wcscpy(szError,L"SCARD_F_UNKNOWN_ERROR");
				#endif
				break;

			case 0x80100015:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_INVALID_ATR");
				#else
					wcscpy(szError,L"SCARD_E_INVALID_ATR");
				#endif
				break;

			case 0x80100016:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_NOT_TRANSACTED");
				#else
					wcscpy(szError,L"SCARD_E_NOT_TRANSACTED");
				#endif
				break;

			case 0x80100017:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_READER_UNAVAILABLE");
				#else
					wcscpy(szError,L"SCARD_E_READER_UNAVAILABLE");
				#endif
				break;

			case 0x80100018:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_P_SHUTDOWN");
				#else
					wcscpy(szError,L"SCARD_P_SHUTDOWN");
				#endif
				break;

			case 0x80100019:
				#ifndef WINCE				   
					wcscpy_s(szError,iErrSize,L"SCARD_E_PCI_TOO_SMALL");
				#else
					wcscpy(szError,L"SCARD_E_PCI_TOO_SMALL");
				#endif
				break;

			case 0x8010001A:
	
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_READER_UNSUPPORTED");
				#else
					wcscpy(szError,L"SCARD_E_READER_UNSUPPORTED");
				#endif
				break;

			case 0x8010001B:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_DUPLICATE_READER");
				#else
					wcscpy(szError,L"SCARD_E_DUPLICATE_READER");
				#endif
				break;

			case 0x8010001C:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_CARD_UNSUPPORTED");
				#else
					wcscpy(szError,L"SCARD_E_CARD_UNSUPPORTED");
				#endif
				break;

			case 0x8010001D:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_NO_SERVICE");
				#else
					wcscpy(szError,L"SCARD_E_NO_SERVICE");
				#endif
				break;

			case 0x8010001E:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_SERVICE_STOPPED");
				#else
					wcscpy(szError,L"SCARD_E_SERVICE_STOPPED");
				#endif
				break;

			case 0x8010001F:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_UNEXPECTED");
				#else
					wcscpy(szError,L"SCARD_E_UNEXPECTED");
				#endif
				break;

			case 0x80100020:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_ICC_INSTALLATION");
				#else
					wcscpy(szError,L"SCARD_E_ICC_INSTALLATION");
				#endif
				break;

			case 0x80100021:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_ICC_CREATEORDER");
				#else
					wcscpy(szError,L"SCARD_E_ICC_CREATEORDER");
				#endif
				break;

			case 0x80100022:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_UNSUPPORTED_FEATURE");
				#else
					wcscpy(szError,L"SCARD_E_UNSUPPORTED_FEATURE");
				#endif
				break;

			case 0x80100023:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_DIR_NOT_FOUND");
				#else
					wcscpy(szError,L"SCARD_E_DIR_NOT_FOUND");
				#endif
				break;

			case 0x80100024:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_FILE_NOT_FOUND");
				#else
					wcscpy(szError,L"SCARD_E_FILE_NOT_FOUND");
				#endif
				break;

			case 0x80100025:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_NO_DIR");
				#else
					wcscpy(szError,L"SCARD_E_NO_DIR");
				#endif
				break;

			case 0x80100026:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_NO_FILE");
				#else
					wcscpy(szError,L"SCARD_E_NO_FILE");
				#endif
				break;

			case 0x80100027:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_NO_ACCESS");
				#else
					wcscpy(szError,L"SCARD_E_NO_ACCESS");
				#endif
				break;

			case 0x80100028:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_WRITE_TOO_MANY");
				#else
					wcscpy(szError,L"SCARD_E_WRITE_TOO_MANY");
				#endif
				break;

			case 0x80100029:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_BAD_SEEK");
				#else
					wcscpy(szError,L"SCARD_E_BAD_SEEK");
				#endif
				break;

			case 0x8010002A:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_INVALID_CHV");
				#else
					wcscpy(szError,L"SCARD_E_INVALID_CHV");
				#endif
				break;

			case 0x8010002B:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_UNKNOWN_RES_MNG");
				#else
					wcscpy(szError,L"SCARD_E_UNKNOWN_RES_MNG");
				#endif
				break;

			case 0x8010002C:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_NO_SUCH_CERTIFICATE");
				#else
					wcscpy(szError,L"SCARD_E_NO_SUCH_CERTIFICATE");
				#endif
				break;

			case 0x8010002D:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_CERTIFICATE_UNAVAILABLE");
				#else
					wcscpy(szError,L"SCARD_E_CERTIFICATE_UNAVAILABLE");
				#endif
				break;

			case 0x8010002E:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_E_NO_READERS_AVAILABLE");
				#else
					wcscpy(szError,L"SCARD_E_NO_READERS_AVAILABLE");
				#endif
				break;

			case 0x80100065:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_W_UNSUPPORTED_CARD");
				#else
					wcscpy(szError,L"SCARD_W_UNSUPPORTED_CARD");
				#endif
				break;

			case 0x80100066:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_W_UNRESPONSIVE_CARD");
				#else
					wcscpy(szError,L"SCARD_W_UNRESPONSIVE_CARD");
				#endif
				break;

			case 0x80100067:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_W_UNPOWERED_CARD");
				#else
					wcscpy(szError,L"SCARD_W_UNPOWERED_CARD");
				#endif
				break;

			case 0x80100068:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_W_RESET_CARD");
				#else
					wcscpy(szError,L"SCARD_W_RESET_CARD");
				#endif
				break;

			case 0x80100069:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_W_REMOVED_CARD");
				#else
					wcscpy(szError,L"SCARD_W_REMOVED_CARD");
				#endif
				break;

			case 0x8010006A:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_W_SECURITY_VIOLATION");
				#else
					wcscpy(szError,L"SCARD_W_SECURITY_VIOLATION");
				#endif
				break;

			case 0x8010006B:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_W_WRONG_CHV");
				#else
					wcscpy(szError,L"SCARD_W_WRONG_CHV");
				#endif
				break;

			case 0x8010006C:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_W_CHV_BLOCKED");
				#else
					wcscpy(szError,L"SCARD_W_CHV_BLOCKED");
				#endif
				break;

			case 0x8010006D:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_W_EOF");
				#else
					wcscpy(szError,L"SCARD_W_EOF");
				#endif
				break;

			case 0x8010006E:
				#ifndef WINCE			
					wcscpy_s(szError,iErrSize,L"SCARD_W_CANCELLED_BY_USER");
				#else
					wcscpy(szError,L"SCARD_W_CANCELLED_BY_USER");
				#endif					
				break;

			default:
				#ifndef WINCE
					wcscpy_s(szError,iErrSize,L"Invalid PCSC Error Code");
				#else
					wcscpy(szError,L"Invalid PCSC Error Code");
				#endif
		}

		#ifndef WINCE
			swprintf_s(szOutput,iOutputSize,L"%s failed with %s",szFunction,szError);
		#else
			swprintf(szOutput,L"%s failed with %s",szFunction,szError);
		#endif

	}
	else 
	{
		#ifndef WINCE
			swprintf_s(szOutput,iOutputSize,L"%s failed with 0x%.2X",szFunction,lReturn1);
		#else
			swprintf(szOutput,L"%s failed with 0x%.2X",szFunction,lReturn1);
		#endif
	}
	
	PrintDebugInfo(FILE_LOG, szOutput);
}

//void APDUErrorMsg(LONG lReturn1, LONG lReturn2, TCHAR* szFunction, SCARDCONTEXT hCtx)
void APDUErrorMsg(LONG lReturn1, LONG lReturn2, TCHAR* szFunction)
{
	
	TCHAR szOutput[256] = {0}, szStatusByte[3] = {0};
	TCHAR szError[256] = {0};

	int iOutputSize = sizeof(szOutput)/sizeof(szOutput[0]), iErrSize = sizeof(szError)/sizeof(szError[0]);

	//SCardReleaseContext(hCtx);
	
	if (lReturn1 == 0)	// no certs found
	{
		#ifndef WINCE
			swprintf_s(szOutput,iOutputSize,L"%s",szFunction);
		#else
			swprintf(szOutput,L"%s",szFunction);
		#endif

	}
	else if (lReturn1 == 0x63 && lReturn1 != 0x00)	// pin error
	{
		if (lReturn2 > 0xC0)	// PIV
			#ifndef WINCE
				swprintf_s(szOutput,iOutputSize,L"PIN verification failed.  %d tries remaining.",lReturn2 & 0x0F);	// only display the 2nd nibble
			#else
				swprintf(szOutput,L"PIN verification failed.  %d tries remaining.",lReturn2 & 0x0F);	// only display the 2nd nibble
			#endif
		else	// CAC
			#ifndef WINCE
				swprintf_s(szOutput,iOutputSize,L"PIN verification failed.  %d tries remaining.",lReturn2);	
			#else
				swprintf(szOutput,L"PIN verification failed.  %d tries remaining.",lReturn2);	
			#endif
	}
	else				// apdu error
	{
		unsigned long ulError = lReturn1 * 256 + lReturn2;

		switch(ulError)
		{
			
			case SW_VERIFY_FAILED:
				#ifndef WINCE
					wcscpy_s(szError,iErrSize,L"SW_VERIFY_FAILED");
				#else
					wcscpy(szError,L"SW_VERIFY_FAILED");
				#endif
				break;

			case SW_WARNING:
				#ifndef WINCE
					wcscpy_s(szError,iErrSize,L"SW_WARNING");
				#else
					wcscpy(szError,L"SW_WARNING");
				#endif
				break;

			case SW_COMMUNICATION_ERROR:
				#ifndef WINCE
				   wcscpy_s(szError,iErrSize,L"SW_COMMUNICATION_ERROR");
				#else
					wcscpy(szError,L"SW_COMMUNICATION_ERROR");
				#endif
				break;

			case SW_SECURITY_CONDITION_NOT_SATISFIED:
				#ifndef WINCE
				   wcscpy_s(szError,iErrSize,L"SW_SECURITY_CONDITION_NOT_SATISFIED");
				#else
					wcscpy(szError,L"SW_SECURITY_CONDITION_NOT_SATISFIED");
				#endif
				break;

			case SW_AUTHENTICATION_METHOD_BLOCKED:
				#ifndef WINCE
				   wcscpy_s(szError,iErrSize,L"SW_WARNING");
				#else
					wcscpy(szError,L"SW_WARNING");
				#endif
				break;

			case SW_CONDITIONS_OF_USE_NOT_SATISFIED:
				#ifndef WINCE
				   wcscpy_s(szError,iErrSize,L"SW_CONDITIONS_OF_USE_NOT_SATISFIED");
				#else
					wcscpy(szError,L"SW_CONDITIONS_OF_USE_NOT_SATISFIED");
				#endif
				break;

			case SW_INCORRECT_PARAMETER_IN_DATA_FIELD:
				#ifndef WINCE
				   wcscpy_s(szError,iErrSize,L"SW_INCORRECT_PARAMETER_IN_DATA_FIELD");
				#else
					wcscpy(szError,L"SW_INCORRECT_PARAMETER_IN_DATA_FIELD");
				#endif
				break;

			case SW_FUNCTION_NOT_SUPPORTED:
				#ifndef WINCE
				   wcscpy_s(szError,iErrSize,L"SW_FUNCTION_NOT_SUPPORTED");
				#else
					wcscpy(szError,L"SW_FUNCTION_NOT_SUPPORTED");
				#endif
				break;

			case SW_DATA_OBJECT_NOT_FOUND:
				#ifndef WINCE
				   wcscpy_s(szError,iErrSize,L"SW_DATA_OBJECT_NOT_FOUND");
				#else
					wcscpy(szError,L"SW_DATA_OBJECT_NOT_FOUND");
				#endif
				break;

			case SW_NOT_ENOUGH_MEMORY:
				#ifndef WINCE
				   wcscpy_s(szError,iErrSize,L"SW_NOT_ENOUGH_MEMORY");
				#else
					wcscpy(szError,L"SW_NOT_ENOUGH_MEMORY");
				#endif
				break;

			case SW_INCORRECT_PARAMETER_IN_P1_OR_P2:
				#ifndef WINCE
					wcscpy_s(szError,iErrSize,L"SW_WARNING");
				#else
					wcscpy(szError,L"SW_WARNING");
				#endif
				break;

			case SW_KEY_REFERENCE_NOT_FOUND:
				#ifndef WINCE
					wcscpy_s(szError,iErrSize,L"SW_KEY_REFERENCE_NOT_FOUND");
				#else
					wcscpy(szError,L"SW_KEY_REFERENCE_NOT_FOUND");
				#endif
				break;
			
			case SW_UNKNOWN_INSTRUCTION:
				#ifndef WINCE
					wcscpy_s(szError,iErrSize,L"SW_UNKNOWN_INSTRUCTION");
				#else
					wcscpy(szError,L"SW_UNKNOWN_INSTRUCTION");
				#endif
				break;

			default:
				#ifndef WINCE
					swprintf_s(szError,iOutputSize,L"0x%.2X%.2X",lReturn1,lReturn2);
				#else
					swprintf(szError,L"0x%.2X%.2X",lReturn1,lReturn2);
				#endif
		
		}

		#ifndef WINCE
			swprintf_s(szOutput,iOutputSize,L"%s failed with %s",szFunction,szError);
		#else
			swprintf(szOutput,L"%s failed with %s",szFunction,szError);
		#endif
	}
	
	PrintDebugInfo(FILE_LOG, szOutput);
	//ImpersonateProcessCardEvent((LPTSTR)szOutput, strlen(szOutput), OPERATION_TYPE_ERR);
	//MessageBox(NULL,szOutput,"APDU Error",MB_ICONERROR);
}

void PrintDebugInfo(int iFile, wchar_t *string,...)
{
	int iLog = 0;
	
#	ifndef WINCE
	iLog = ReadIntFromINI(SECTION_SETUP, INI_DEBUG);
#	endif

	if(bLogging || iLog == 1 || iFile == FILE_OUTPUT) //is debugging on?
	{
		va_list		argList;
		wchar_t		bufTime[100];
		SYSTEMTIME	systemDateTime;
		wchar_t		szPath[MAX_PATH];

		debugFile_txcrdwrppr = NULL;

#		ifndef WINCE
			if(iFile == FILE_OUTPUT)	// for outputting card UID to separate text file for the FTC
			{
				wchar_t* szLogPath;

				szLogPath = ReadStringFromINI(SECTION_SETUP, INI_CARD_LOG_PATH);
				//wcscpy_s(szPath, L"c:\\cardlog.txt");
				//SHGetFolderPath (NULL, CSIDL_DESKTOP, NULL, SHGFP_TYPE_DEFAULT, szPath);
				swprintf(szPath, MAX_PATH, L"%s\\cardlog.txt", szLogPath);
				//wcscpy_s(szPath, L"c:\\cardlog.txt");

				//PrintDebugInfo(FILE_LOG, L"desktop path=%s", szPath);
				//string = L"%s";
			}
			else if(iFile == FILE_SVC_LOG)
			{
				//GetCurrentDirectory(MAX_PATH, szPath);
				//swprintf(szPath, MAX_PATH,L"%s\\txsvclog.txt", &szPath);
				wcscpy_s(szPath, L"c:\\txsvclog.txt");
			}
			else
			{
				//GetCurrentDirectory(MAX_PATH, szPath);
				//swprintf(szPath, MAX_PATH,L"%s\\txlog.txt", &szPath);
				if(wcscmp(szGPath, L"") == 0)
					wcscpy_s(szPath, L"c:\\txlog.txt");
				else
					wcscpy_s(szPath, szGPath);
			}

			_wfopen_s(&debugFile_txcrdwrppr, szPath, L"a");
#		else
			WCHAR* wszTemp = NULL;
			WCHAR  wszDirectory[MAX_PATH] = L"\0";

			GetModuleFileNameW(NULL,wszDirectory,MAX_PATH);

			wszTemp = wcsrchr(wszDirectory,L'\\'); // Finds last occurence and replace it with NULL.

			*(wszTemp+1) = L'\0';

			//swprintf(szPath, MAX_PATH, L"%s\\txlog.txt", wszDirectory);
			swprintf(szPath, L"%s\\txlog.txt", wszDirectory);
			debugFile_txcrdwrppr = _wfopen(szPath, L"a");
#		endif

		if (NULL == debugFile_txcrdwrppr)
		{
			debugFile_txcrdwrppr = NULL;
			return;
		}
		
		GetLocalTime(&systemDateTime);

		// put local time into buffer
		#ifndef WINCE
			swprintf(bufTime, sizeof(bufTime), L"%d.%d.%d - %d:%d:%d - ",systemDateTime.wYear, systemDateTime.wMonth, systemDateTime.wDay, systemDateTime.wHour, systemDateTime.wMinute, systemDateTime.wSecond);

			if (NULL != debugFile_txcrdwrppr)
				fwprintf_s(debugFile_txcrdwrppr,bufTime);
		#else
			swprintf(bufTime, L"%d.%d.%d - %d:%d:%d - ",systemDateTime.wYear, systemDateTime.wMonth, systemDateTime.wDay, systemDateTime.wHour, systemDateTime.wMinute, systemDateTime.wSecond);
			if (NULL != debugFile_txcrdwrppr)
				fwprintf(debugFile_txcrdwrppr,bufTime);
		#endif

		// prepare argument list
		va_start(argList,string);

		// write string from calling function
		#ifndef WINCE
			if (NULL != string)
				vfwprintf_s(debugFile_txcrdwrppr,string,argList);
			if (NULL != debugFile_txcrdwrppr)
				fwprintf_s(debugFile_txcrdwrppr,L"\n");
		#else
			vfwprintf(debugFile_txcrdwrppr,string,argList);
			if (NULL != debugFile_txcrdwrppr)
				fwprintf(debugFile_txcrdwrppr,L"\n");
		#endif
		
		fflush(debugFile_txcrdwrppr);
		if (NULL != debugFile_txcrdwrppr)
			fclose(debugFile_txcrdwrppr);
	} 

	debugFile_txcrdwrppr = NULL;
}

bool IsNumeric(LPSTR szTesting)
{
   bool bNumeric = true;
   LPSTR szText = szTesting; // Saves CString's array access overheads

   while (bNumeric && *szText)
   {
      //if (!isdigit(*szText) && *szText != '-' && *szText != '.')
	   if (!isdigit(*szText))
         bNumeric = false;
      szText++;
   }

   return bNumeric;
} 

#ifndef WINCE
int ReadIntFromINI(wchar_t* szSection, wchar_t* szKey)
{
	wchar_t buffer[MAX_PATH];

    if(0 != GetModuleFileName(NULL, buffer, MAX_PATH))
	{
		if (PathRemoveFileSpec(buffer))
		{
			// create path to setup.ini
			wcscat_s(buffer,FILENAME_INI);

			// instansiate the INI class
			CIniReader iniReader(buffer);

			// read the barcode port
			return iniReader.ReadInteger(szSection, szKey, 0); 
		}
		else
			return 0;
	}
	else 
		return 0;
}

wchar_t* ReadStringFromINI(wchar_t* szSection, wchar_t* szKey)
{
	wchar_t buffer[MAX_PATH];

    if(0 != GetModuleFileName(NULL, buffer, MAX_PATH))
	{
		if (PathRemoveFileSpec(buffer))
		{
			// create path to setup.ini
			wcscat_s(buffer,FILENAME_INI);

			// instansiate the INI class
			CIniReader iniReader(buffer);

			// read the barcode port
			return iniReader.ReadString(szSection, szKey, L""); 
		}
		else
			return L"";
	}
	else 
		return L"";
}

void WriteStringToINI(wchar_t* szSection, wchar_t* szKey, wchar_t* szValue)
{
	wchar_t buffer[MAX_PATH];

    if(0 != GetModuleFileName(NULL, buffer, MAX_PATH))
	{
		if (PathRemoveFileSpec(buffer))
		{
			// create path to setup.ini
			wcscat_s(buffer,FILENAME_INI);
			CIniWriter iniWriter(buffer);
			iniWriter.WriteString(szSection, szKey, szValue); 
		}
	}
}

BOOL WriteIntToINI(wchar_t* szSection, wchar_t* szKey, int iNum)
{
	BOOL bRet = false;

	wchar_t buffer[MAX_PATH];

    if(0 != GetModuleFileName(NULL, buffer, MAX_PATH))
	{
		if (PathRemoveFileSpec(buffer))
		{
			// create path to setup.ini
			wcscat_s(buffer,FILENAME_INI);
			CIniWriter iniWriter(buffer);
			bRet = iniWriter.WriteInteger(szSection,  szKey, iNum); 
		}
	}

	return bRet;
}
#endif