#ifndef INIWRITER_H
#define INIWRITER_H
class CIniWriter
{
public:
	CIniWriter(wchar_t* szFileName); 
	//CIniWriter(); 
	void SetWriterPath(wchar_t* szFileName);
	BOOL WriteInteger(wchar_t* szSection, wchar_t* szKey, int iValue);
	//BOOL WriteFloat(wchar_t* szSection, wchar_t* szKey, float fltValue);
	//BOOL WriteBoolean(wchar_t* szSection, wchar_t* szKey, bool bolValue);
	BOOL WriteString(wchar_t* szSection, wchar_t* szKey, wchar_t* szValue);
	//void WriteInteger(char* szSection, char* szKey, int iValue);
	//void WriteFloat(char* szSection, char* szKey, float fltValue);
	//void WriteBoolean(char* szSection, char* szKey, bool bolValue);
	//void WriteString(char* szSection, char* szKey, char* szValue);
private:
	wchar_t m_szFileName[255];
};
#endif //INIWRITER_H