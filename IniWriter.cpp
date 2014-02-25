//#include "stdafx.h"
#include <Windows.h> 
//#include <iostream>
#include <stdio.h>
#include "IniWriter.h"

#pragma warning(disable:4995)

CIniWriter::CIniWriter(wchar_t* szFileName)
{
	memset(m_szFileName, 0x00, 255);
	memcpy(m_szFileName, szFileName, wcslen(szFileName)*sizeof(wchar_t));
}

//CIniWriter::CIniWriter()
//{
//}

void CIniWriter::SetWriterPath(wchar_t* szFileName)
{
	memset(m_szFileName, 0x00, 255);
	memcpy(m_szFileName, szFileName, wcslen(szFileName)*sizeof(wchar_t));
}

BOOL CIniWriter::WriteInteger(wchar_t* szSection, wchar_t* szKey, int iValue)
{
	BOOL bRet = 0;
#	ifndef WINCE
	wchar_t szValue[255];
	swprintf_s(szValue, L"%d", iValue);
	bRet = WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName); 
#	endif
	return bRet;
}
//BOOL CIniWriter::WriteFloat(wchar* szSection, char* szKey, float fltValue)
//{
//	BOOL bRet;
//	char szValue[255];
//	swprintf_s(szValue, "%f", fltValue);
//	bRet = WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName); 
//	return bRet;
//}
//BOOL CIniWriter::WriteBoolean(wchar_t* szSection, wchar_t* szKey, bool bolValue)
//{
//	BOOL bRet;
//	char szValue[255];
//	sprintf_s(szValue, "%s", bolValue ? "True" : "False");
//	bRet = WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName); 
//	return bRet;
//}
BOOL CIniWriter::WriteString(wchar_t* szSection, wchar_t* szKey, wchar_t* szValue)
{
	BOOL bRet = 0;
#ifndef WINCE
	bRet = WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName);
#endif
	return bRet;
}

//void CIniWriter::WriteInteger(char* szSection, char* szKey, int iValue)
//{
//	char szValue[255];
//	sprintf_s(szValue, "%d", iValue);
//	WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName); 
//}
//void CIniWriter::WriteFloat(char* szSection, char* szKey, float fltValue)
//{
//	char szValue[255];
//	sprintf_s(szValue, "%f", fltValue);
//	WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName); 
//}
//void CIniWriter::WriteBoolean(char* szSection, char* szKey, bool bolValue)
//{
//	char szValue[255];
//	sprintf_s(szValue, "%s", bolValue ? "True" : "False");
//	WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName); 
//}
//void CIniWriter::WriteString(char* szSection, char* szKey, char* szValue)
//{
//	WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName);
//}