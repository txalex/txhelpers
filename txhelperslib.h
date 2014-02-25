#ifndef _TXHELPERSLIB_H
#define _TXHELPERSLIB_H

#include <windows.h>
#include <stdio.h>
#include <winscard.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#include "IniReader.h"
#include "IniWriter.h"
#include "defines.h"

#define FILENAME_INI		L"\\setup.ini"
#define SECTION_SETUP		L"SETUP"
#define INI_DEBUG			L"Debug"
#define INI_CARD_LOG_PATH	L"CardLogPath"

#define ERR_TX_CRD_WRPPR	0x100
#define ERR_FASCN_PARSING	ERR_TX_CRD_WRPPR + 1
#define ERR_INVALID_PIN		ERR_TX_CRD_WRPPR + 2
#define ERR_NO_DATA			ERR_TX_CRD_WRPPR + 3
#define ERR_PCSC_FAILED		ERR_TX_CRD_WRPPR + 4
#define ERR_INVALID_OFFSET	ERR_TX_CRD_WRPPR + 5


// callback function message definitions
#define CLLBK_CAC_PIV_INSERTED	1
#define CLLBK_CARD_REMOVED		2
#define CLLBK_CARD_INSERTED		3
#define CLLBK_READ_SUCCESS		4
#define CLLBK_CARD_INVALID		5
#define CLLBK_ERR				6
#define CLLBK_READER_CHANGE		7
#define CLLBK_INVALID_PIN		8

#define FILE_LOG		0
#define FILE_SVC_LOG	1
#define FILE_OUTPUT		2

// contains PCSC reader and connection variables
struct TX_PCSC_INFO
{
    SCARDCONTEXT		hContext;
    SCARDHANDLE			hCard;
    LPSCARD_IO_REQUEST	pioPCI;
	LPTSTR				lpstrReader;
};

// contains PCSC reader and connection variables
struct READER_STATUS
{
	LPTSTR szReader; 
	bool bInserted;
};

// used in reader looping 
inline LPVOID	Allocate(size_t cb);
inline LPVOID	Reallocate(LPVOID pv, size_t cb);
void WINAPI		Free(__in LPVOID pvMem);
LPVOID WINAPI	Alloc(__in SIZE_T cBytes);

#define AllocateStruct(x) (x *)Allocate(sizeof(x))
#define AllocateStructArray(x, n) (x *)Allocate(sizeof(x) * (n))

DWORD		ActivatePCSC(LPTSTR szReader, DWORD dwShareMode, TX_PCSC_INFO* pstPCSCInfo);
void		DeactivatePCSC(TX_PCSC_INFO*(&));
void		PrintDebugInfo(int iFile, wchar_t *string,...);
void		PCSCErrorMsg(LONG lReturn1, TCHAR* szFunction);
void		APDUErrorMsg(LONG lReturn1, LONG lReturn2, TCHAR* szFunction);
void		ToggleLogging(bool bToggle);
void		SetLogPath(wchar_t* szPath);
bool		IsNumeric(LPSTR szTesting);
BYTE*		StringToBytPtr(LPTSTR szText, DWORD dwLen);
int			countBits(unsigned char byte);

#ifndef WINCE
BOOL		WriteIntToINI(wchar_t* szSection, wchar_t* szKey, int iNum);
void		WriteStringToINI(wchar_t*, wchar_t*, wchar_t*);
wchar_t*	ReadStringFromINI(wchar_t*, wchar_t*);
int			ReadIntFromINI(wchar_t*, wchar_t*);
#endif	// end if not WINCE

#endif	// end _TXHELPERSLIB_H
