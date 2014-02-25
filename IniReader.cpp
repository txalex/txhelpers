//#include "stdafx.h"
#include <Windows.h>
//#include <iostream>
#include <stdio.h>
#include "IniReader.h"

#pragma warning(disable:4995)

CIniReader::CIniReader(wchar_t* szFileName)
{
	memset(m_szFileName, 0x00, 255);
	memcpy(m_szFileName, szFileName, wcslen(szFileName)*sizeof(wchar_t));
}

//CIniReader::CIniReader()
//{
//}

void CIniReader::SetReaderPath(wchar_t* szFileName)
{
	memset(m_szFileName, 0x00, 255);
	memcpy(m_szFileName, szFileName, wcslen(szFileName)*sizeof(wchar_t));
}


int CIniReader::ReadInteger(wchar_t* szSection, wchar_t* szKey, int iDefaultValue)
{
	int iResult = 0;

#	ifndef WINCE
	iResult = GetPrivateProfileInt(szSection,  szKey, iDefaultValue, m_szFileName); 
#	endif

	return iResult;
}
//
//float CIniReader::ReadFloat(wchar_t* szSection, wchar_t* szKey, float fltDefaultValue)
//{
//	wchar_t szResult[255];
//	wchar_t szDefault[255];
//	float fltResult;
//	swprintf_s(szDefault, L"%f",fltDefaultValue);
//	GetPrivateProfileString(szSection,  szKey, szDefault, szResult, 255, m_szFileName); 
//	fltResult = (float)atof(szResult);
//	return fltResult;
//}
//bool CIniReader::ReadBoolean(wchar_t* szSection, wchar_t* szKey, bool bolDefaultValue)
//{
//	wchar_t szResult[255];
//	wchar_t szDefault[255];
//	bool bolResult;
//	swprintf_s(szDefault, L"%s", bolDefaultValue? "True" : "False");
//	GetPrivateProfileString(
//		szSection,
//		szKey,
//		szDefault,
//		szResult,
//		255,
//		m_szFileName); 
//	bolResult =  (strcmp(szResult, "True") == 0 || 
//		strcmp(szResult, "true") == 0) ? true : false;
//	return bolResult;
//}
wchar_t* CIniReader::ReadString(wchar_t* szSection, wchar_t* szKey, const wchar_t* szDefaultValue)
{
	wchar_t* szResult = new wchar_t[255];
	memset(szResult, 0x00, 255);

#	ifndef WINCE
	GetPrivateProfileString(
		szSection,  
		szKey, 
		szDefaultValue, 
		szResult, 
		255, 
		m_szFileName); 
#	endif
	return szResult;
}