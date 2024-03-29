#ifndef INIREADER_H
#define INIREADER_H
class CIniReader
{
public:
	CIniReader(wchar_t* szFileName); 
	//CIniReader(); 
	void SetReaderPath(wchar_t* szFileName);
	int ReadInteger(wchar_t* szSection, wchar_t* szKey, int iDefaultValue);
	//float ReadFloat(wchar_t* szSection, wchar_t* szKey, float fltDefaultValue);
	//bool ReadBoolean(wchar_t* szSection, wchar_t* szKey, bool bolDefaultValue);
	wchar_t* ReadString(wchar_t* szSection, wchar_t* szKey, const wchar_t* szDefaultValue);
private:
	wchar_t m_szFileName[255];
};
#endif//INIREADER_H