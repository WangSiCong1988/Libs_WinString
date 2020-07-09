#pragma once
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include <iostream>		// Two libs for "new (std::nothrow)"
#include <new>

/*** Predefined Values ***/
// Code Page from https://docs.microsoft.com/en-us/windows/win32/intl/code-page-identifiers
#define Win32String_TYPE_WIDE_CHARACTER						1
#define Win32String_TYPE_ASCII								0
#define Win32String_TYPE_UTF8								65001
#define Win32String_TYPE_UTF16_LittleEndian					1200	// Unicode - code page 1200	(it is the default utf16 in Windows except Windows NT 3.51 & Windows NT 4.0)
#define Win32String_TYPE_UTF16_BigEndian					1201	//
#define Win32String_TYPE_ISO_8859_1							28591	// 1 bytes
#define Win32String_TYPE_GB18030							54936	// Windows XP and later: GB18030 Simplified Chinese (4 byte); Chinese Simplified (GB18030)
#define Win32String_TYPE_GB2312								936		// ANSI/OEM ���룬��Ӣ�ļ�Ӣ�ķ��Ų���ASCII���ֽڱ��룬���ĺ����ķ�����GB2312˫�ֽڱ���

#define Win32String_DWORD_BYTESNUM							2

// This STRING class is designed to transfer Unicode into UTF-8
// Chinese Character in ASCII is actually coded in GB2312
class Win32String{
private:
	//-------------------------------------------------------------------------------
	// Assitance Functions
	//-------------------------------------------------------------------------------
	size_t _STR_WCharsNum(const wchar_t * lpWChars);												// include '\0\0'
	size_t _STR_WCharsNumWithoutEOS(const wchar_t * lpWChars);								
	size_t _STR_BytesNum(const char * bytes, UINT type);											// include '\0', '\0\?'(? means any value) or '\0\0'
	size_t _STR_BytesNumWithoutEOS(const char * bytes, UINT type);

	void _STR_WCharsCpy(wchar_t * dest, const wchar_t * source, size_t len);						// @len: must include '\0\0'
	void _STR_WCharsCat(wchar_t * dest, const wchar_t * source, size_t beginIdx, size_t len);		// @len: must include '\0\0'
	void _STR_BytesCpy(char * dest, const char * source, size_t len);								// @len: must include '\0' or '\0\0'
	void _STR_BytesCat(char * dest, const char * source, size_t beginIdx, size_t len);				// @len: must include '\0' or '\0\0'
	// ת��������������������ֹʧ��ʱ�ռ�û���ͷţ������ط�NULL�ɹ�
	wchar_t * _PARSE_Bytes2WChar(const char * bytes, UINT type);									// @byte: must incldue '\0' or '\0\0',	@return: include '\0\0'
	char * _PARSE_WChar2Bytes(const wchar_t * wchars, UINT type);									// @wchars: must include '\0\0',		@return: include '\0' or '\0\0'

	//-------------------------------------------------------------------------------
	// ˽������
	//-------------------------------------------------------------------------------
	static UINT defaultType;
	UINT type;

	// Pointers to storage different coding
	// They are only released in the deconstruction method
	wchar_t * lpWChar;			// WideChar Type (It is for any character in any language but only used in the memory)
	char * lpBytes;				// Bytes of the string (based on what code you choose)

	//-------------------------------------------------------------------------------
	// ˽�з���
	//-------------------------------------------------------------------------------
	/**
	 * Construction
	 */
	Win32String(){
		this->lpWChar = NULL;
		this->lpBytes = NULL;
		this->type = Win32String::defaultType;	// The default type
	}

	/**
	 * Init
	 * @description: init an empty string, set it to ASCII
	 */
	bool Init(){
		this->lpBytes = new (std::nothrow) char[1];
		// Fail for OOM
		if(!this->lpBytes){
			return false;
		}
		// No OOM
		this->lpBytes[0] = '\0';
		this->type = Win32String_TYPE_ASCII;	// The empty string is ALWAYS ASCII
		return true;
	}
	/**
	 * Init
	 * @description: init a string from given bytes (thoses bytes are supposed to be the same as the default type)
	 */
	bool Init(const char * str){
		// Empty Input, so an empty string
		if(!str){
			return Init();
		}
		
		size_t bytesNum = this->_STR_BytesNum(str, this->type);	// Space calculation (we can't get 0 here because NULL is handled before)
		this->lpBytes = new (std::nothrow) char[bytesNum];
		// Fail for OOM
		if(!this->lpBytes){
			return false;
		}
		// No OOM
		this->_STR_BytesCpy(this->lpBytes, str, bytesNum);
		return true;
	}

	
public:
	/*** Coding Types ***/
	const static UINT TYPE_ASCII;
	const static UINT TYPE_WIDE_CHARACTER;
	const static UINT TYPE_UTF8;
	const static UINT TYPE_UTF16_LittleEndian;	// utf16, little endian
	const static UINT TYPE_UNICODE_PAGE1200;		// utf16, little endian
	const static UINT TYPE_UNICODE;				// utf16, little endian
	
	/**
	 * SetDefaultType
	 * @param type: the code type in your source files 
	 */
	static bool SetDefaultType(const UINT type){
		// ���������֧�ֵ�����ģ����򷵻�false
		if(	type == Win32String_TYPE_ASCII || 
			type == Win32String_TYPE_WIDE_CHARACTER || 
			type == Win32String_TYPE_UTF8 ||
			type == Win32String_TYPE_UTF16_LittleEndian ||
			type == Win32String_TYPE_UTF16_BigEndian || 
			type == Win32String_TYPE_ISO_8859_1 ||
			type == Win32String_TYPE_GB18030 ||
			type == Win32String_TYPE_GB2312
			){
			Win32String::defaultType = type;
			return true;
		}
		return false;
	}

	/**
	 * Deconstruct
	 */
	~Win32String(){
		if(this->lpWChar != NULL){
			delete[] this->lpWChar;
		}
		if(this->lpBytes != NULL){
			delete[] this->lpBytes;
		}
	}

	//------------------------------------------------------------------------------
	// Create
	//
	// Create: An empty ASCII string
	static Win32String * Create(){
		Win32String * lpWin32Str = new (std::nothrow) Win32String();
		// OOM
		if(!lpWin32Str){
			return NULL;
		}
		// Init fails
		if(!lpWin32Str->Init()){
			delete lpWin32Str;
			return NULL;
		}
		// All good
		return lpWin32Str; 
	}
	// Create: An Default String
	static Win32String * Create(const char * str){
		Win32String * lpWin32Str = new (std::nothrow) Win32String();
		// OOM
		if(!lpWin32Str){
			return NULL;
		}
		// Init fails
		if(!lpWin32Str->Init(str)){
			delete lpWin32Str;
			return NULL;
		}
		// All good
		return lpWin32Str;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	/**
	 * Add extra bytes inside
	 * @param bytes: the bytes to be appended
	 * @param type: the extra bytes' type. If it does not fit the type of old bytes, return false
	 */
	bool Append(const char * bytes, UINT type){
		// Refuse the different type coding
		if(type != this->type){
			return false;
		}
		// Empty Bytes alway true
		if(bytes == NULL){
			return true;
		}
		// ����ռ䣬���ʱ��this->lpBytes���ٻ���1��byte�Ŀռ䡣�������wchar_t��ʼ���ᱻǰ��Ĺ��ܾ̾�����Ϊwchar_t����ֱ��׷��char��
		size_t curNeededBytesNum = this->_STR_BytesNumWithoutEOS(this->lpBytes, this->type);	// Not include EOS (end of string)
		size_t extraBytesNum = this->_STR_BytesNum(bytes, this->type);
	
		// Create new space
		char * lpBytesTMP = new (std::nothrow) char[curNeededBytesNum + extraBytesNum];
		if(!lpBytesTMP){
			return false;					// OOM
		}
		// Copy
		this->_STR_BytesCpy(lpBytesTMP, this->lpBytes, curNeededBytesNum);
		this->_STR_BytesCat(lpBytesTMP, bytes, curNeededBytesNum, extraBytesNum);
		// Free old space & link it to the new one
		delete[] this->lpBytes;				// �ͷžɿռ䣨���ﲻ��Ҫ���ָ���Ƿ�Ϊ�գ���Ϊֻ�У�
		this->lpBytes = lpBytesTMP;
		return true;
	}
	/**
	 * Add extra bytes inside
	 * @param dw: a DWORD will be added and it will be shown in Hex digits
	 */
	bool Append(DWORD dw){
		char * lpASCIIBytes			= NULL;
		wchar_t * wcExtra			= NULL;
		wchar_t * lpWCharNew		= NULL;
		char * bytesExtra			= NULL;
		char * lpBytesNew			= NULL;
		size_t oldNeededWCharLen	= 0;
		size_t extraWcharLen		= 0;
		size_t oldNeededBytesLen	= 0;
		size_t extraBytesLen		= 0;
		bool res = false;

		
		lpASCIIBytes = new (std::nothrow) char[2 + Win32String_DWORD_BYTESNUM*2 + 1];							// Apply space for ASCII bytes
		if(lpASCIIBytes){
			sprintf(lpASCIIBytes, "0x%04x", dw);																// Change DWORD to ASCII bytes
			wcExtra = this->_PARSE_Bytes2WChar(lpASCIIBytes, Win32String_TYPE_ASCII);							// Change ASCII bytes to wide char
			if(wcExtra){
				if(this->type == Win32String_TYPE_WIDE_CHARACTER){
					// Wide char
					oldNeededWCharLen = this->_STR_WCharsNumWithoutEOS(this->lpWChar);
					extraWcharLen = this->_STR_WCharsNum(this->lpWChar);
					lpWCharNew = new (std::nothrow) wchar_t[oldNeededWCharLen + extraWcharLen];
					if(lpWCharNew){
						// Copy
						this->_STR_WCharsCpy(lpWCharNew, this->lpWChar, oldNeededWCharLen);
						this->_STR_WCharsCat(lpWCharNew, wcExtra, oldNeededWCharLen, extraWcharLen);
						// Release old space & link it to the new one
						delete[] this->lpWChar;
						this->lpWChar = lpWCharNew;
						lpWCharNew = NULL;
						// Set success
						res = true;
					}
				}else{
					// Other coding
					bytesExtra = this->_PARSE_WChar2Bytes(wcExtra, this->type);
					if(bytesExtra){
						oldNeededBytesLen = this->_STR_BytesNumWithoutEOS(this->lpBytes, this->type);
						extraBytesLen = this->_STR_BytesNum(bytesExtra, this->type);
						lpBytesNew = new (std::nothrow) char[oldNeededBytesLen + extraBytesLen];
						if(lpBytesNew){
							// Copy
							this->_STR_BytesCpy(lpBytesNew, this->lpBytes, oldNeededBytesLen);
							this->_STR_BytesCat(lpBytesNew, bytesExtra, oldNeededBytesLen, extraBytesLen);
							// Release space
							delete[] this->lpBytes;
							this->lpBytes = lpBytesNew;
							lpBytesNew = NULL;
							// Set success
							res = true;
						}
					}
				}
			}
		}
		// Release temporal space
		if(lpASCIIBytes){
			delete[] lpASCIIBytes;
		}
		if(wcExtra){
			delete[] wcExtra;
		}
		if(lpWCharNew){
			delete[] lpWCharNew;
		}
		if(bytesExtra){
			delete[] bytesExtra;
		}
		if(lpBytesNew){
			delete[] lpBytesNew	;
		}
		// Return
		return res;
	}

	/**
	 * Get the bytes
	 */
	char * GetBytes(){
		return this->lpBytes;
	}

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Type
	//
	// Type: Is WideChar
	bool TypeIsWideChar(){
		return this->type == Win32String_TYPE_WIDE_CHARACTER;
	}
	// Type: Is ASCII
	bool TypeIsASCII(){
		return this->type == Win32String_TYPE_ASCII;
	}
	// Type: Is UTF8
	bool TypeIsUTF8(){
		return this->type == Win32String_TYPE_UTF8;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

	/**
	 * Parse to UTF8
	 * @return: NULL, fail; not NULL, succeed
	 */
	char * Parse2UTF8(){
		wchar_t * lpWCTemp = NULL;
		char * lpBytesTemp = NULL;
		char * utf8Bytes = NULL;			// Default is NULL

		// If it is already utf8, just return those bytes
		if(this->TypeIsUTF8()){ 
			return this->lpBytes;
		}	

		// Transfer - based on the type
		if(this->type == Win32String_TYPE_WIDE_CHARACTER){
			// Begin with Wide Chars
			lpBytesTemp = this->_PARSE_WChar2Bytes(this->lpWChar, Win32String_TYPE_UTF8);
			// Succeed!	
			if(lpBytesTemp){
				if(this->lpBytes){ delete[] this->lpBytes; }	// Free old space
				this->lpBytes = lpBytesTemp;					// Link bytes pointer to the new one
				lpBytesTemp = NULL;
				utf8Bytes = this->lpBytes;				// Return pointer has value now
			}
		}else{
			// Begin with other coding bytes (��ʱthis->lpBytes������һ���ַ�'\0')
			lpWCTemp = this->_PARSE_Bytes2WChar(this->lpBytes, this->type);
			if(lpWCTemp){
				lpBytesTemp = this->_PARSE_WChar2Bytes(lpWCTemp, Win32String_TYPE_UTF8);
				// Succeed! 
				if(lpBytesTemp){
					// Free old space & link it to the new one	
					delete[] this->lpBytes;
					this->lpBytes = lpBytesTemp;
					lpBytesTemp = NULL;
					utf8Bytes = this->lpBytes;			// Return pointer has value now
				}	
			}	
		}
		// If succeed, set type to UTF8
		if(utf8Bytes){
			this->type = Win32String_TYPE_UTF8;
		}
		// Release Temperal Variables
		if(lpWCTemp){ delete[] lpWCTemp; }
		if(lpBytesTemp){ delete[] lpBytesTemp; }

		// Return
		return utf8Bytes;
	}

	/**
	 * Parse to WideChar
	 */
	wchar_t * Parse2WideChar(){
		// No transfer if it is
		if(this->TypeIsWideChar()){
			return this->lpWChar;
		}
		// If WChar space exist, delete it
		if(this->lpWChar){ 
			delete[] this->lpWChar;
			this->lpWChar = NULL;
		}
		// Try to change to wide char
		this->lpWChar = this->_PARSE_Bytes2WChar(this->lpBytes, this->type);
		// If succeed, set type to WideChar
		if(this->lpWChar){ this->type = Win32String_TYPE_WIDE_CHARACTER; }
		// Return
		return this->lpWChar;
	}
};


//------------------------------------------------------------------------------------------
// Init static Variables
//------------------------------------------------------------------------------------------
UINT Win32String::defaultType = Win32String_TYPE_ASCII;									// The default type is ASCII

const UINT Win32String::TYPE_ASCII = Win32String_TYPE_ASCII;
const UINT Win32String::TYPE_WIDE_CHARACTER = Win32String_TYPE_WIDE_CHARACTER;
const UINT Win32String::TYPE_UTF8 = Win32String_TYPE_UTF8;
const UINT Win32String::TYPE_UTF16_LittleEndian = Win32String_TYPE_UTF16_LittleEndian;
const UINT Win32String::TYPE_UNICODE_PAGE1200 = Win32String_TYPE_UTF16_LittleEndian;
const UINT Win32String::TYPE_UNICODE = Win32String_TYPE_UTF16_LittleEndian;

//------------------------------------------------------------------------------------------
// Assistance Functions
//------------------------------------------------------------------------------------------
size_t Win32String::_STR_WCharsNum(const wchar_t * lpWChars){
	if(!lpWChars){
		return 0;		// NULL 0 wide chars
	}
	return wcslen(lpWChars) + 1;
}
size_t Win32String::_STR_WCharsNumWithoutEOS(const wchar_t * lpWChars){
	return this->_STR_WCharsNum(lpWChars) - 1;
}

size_t Win32String::_STR_BytesNum(const char * bytes, UINT type){
	// ��ָ�뺬��0��byte
	if(!bytes){
		return 0;
	}

	size_t len = 0;
	
	switch(type){
	case Win32String_TYPE_ASCII:
		return strlen(bytes) + 1;
	
	case Win32String_TYPE_UTF8:
		return strlen(bytes) + 1;

	case Win32String_TYPE_UTF16_LittleEndian:
	case Win32String_TYPE_UTF16_BigEndian:
		while(bytes[len] != '\0' && bytes[len + 1] != '\0'){
			len += 2;
		}
		return len + 2;	// The extra space for '\0\0'
	
	case Win32String_TYPE_ISO_8859_1:
		return strlen(bytes) + 1;
	
	case Win32String_TYPE_GB18030:
		// ���ÿ4��bytes�е�һ��byteΪ0����Ϊ�ַ����������
		while(bytes[len] != '\0'){
			len += 4;
		}
		return len + 4;	// The extra space for '\0\0\0\0'
	
	case Win32String_TYPE_GB2312:
		while(bytes[len] != '\0'){
			len += 1;
		}
		return len + 1;
	}
}
size_t Win32String::_STR_BytesNumWithoutEOS(const char * bytes, UINT type){
	size_t bytesNum = _STR_BytesNum(bytes, type);
	switch(type){
	case Win32String_TYPE_ASCII:
		return bytesNum - 1;
	
	case Win32String_TYPE_UTF8:
		return bytesNum -1;

	case Win32String_TYPE_UTF16_LittleEndian:
	case Win32String_TYPE_UTF16_BigEndian:
		return bytesNum - 2;
	
	case Win32String_TYPE_ISO_8859_1:
		return bytesNum - 1;
	
	case Win32String_TYPE_GB18030:
		return bytesNum - 4;
	
	case Win32String_TYPE_GB2312:
		return bytesNum - 1;
	}
}
void Win32String::_STR_WCharsCpy(wchar_t * dest, const wchar_t * source, size_t len){
	if(!source){
		return;
	}
	size_t i;
	for(i = 0; i < len; i++){
		dest[i] = source[i];
	}
}
void Win32String::_STR_WCharsCat(wchar_t * dest, const wchar_t * source, size_t beginIdx, size_t len){
	if(!source){
		return;
	}
	size_t i;
	for(i = 0; i < len; i++){
		dest[i + beginIdx] = source[i];
	}
}
void Win32String::_STR_BytesCpy(char * dest, const char * source, size_t len){
	if(!source){
		return;
	}
	size_t i;
	for(i = 0; i < len; i++){
		dest[i] = source[i];
	}
}
void Win32String::_STR_BytesCat(char * dest, const char * source, size_t beginIdx, size_t len){
	if(!source){
		return;
	}
	size_t i;
	for(i = 0; i < len; i++){
		dest[i + beginIdx] = source[i];
	}
}
wchar_t * Win32String::_PARSE_Bytes2WChar(const char * bytes, UINT type){
	// NULL for the empty pointer
	if(!bytes){
		return NULL;
	}		
	// NULL for not supported types
	if( type != Win32String_TYPE_ASCII &&
		type != Win32String_TYPE_UTF8 &&
		type != Win32String_TYPE_UTF16_LittleEndian &&
		type != Win32String_TYPE_UTF16_BigEndian &&
		type != Win32String_TYPE_ISO_8859_1 &&
		type != Win32String_TYPE_GB18030 &&
		type != Win32String_TYPE_GB2312){
		return NULL;
	}

	wchar_t * lpWCTemp = NULL;
	int wcharNum = 0;			// include the one more space for '\0\0'
	wcharNum = MultiByteToWideChar(type, 0, bytes, -1, NULL, 0);
	if(!wcharNum){
		return NULL;			// Can't calculate the space
	}
	lpWCTemp = new wchar_t[wcharNum];
	if(!lpWCTemp){
		return NULL;			// OOM
	}
	if(!MultiByteToWideChar(type, 0, bytes, -1, lpWCTemp, wcharNum)){
		delete[] lpWCTemp;		// Transfer fails
		return NULL;
	}
	// Success
	return lpWCTemp;
}
char * Win32String::_PARSE_WChar2Bytes(const wchar_t * wchars, UINT type){
	// NULL for the empty pointer
	if(!wchars){
		return NULL;
	}
	// NULL for not supported types
	if( type != Win32String_TYPE_ASCII &&
		type != Win32String_TYPE_UTF8 &&
		type != Win32String_TYPE_UTF16_LittleEndian &&
		type != Win32String_TYPE_UTF16_BigEndian &&
		type != Win32String_TYPE_ISO_8859_1 &&
		type != Win32String_TYPE_GB18030 &&
		type != Win32String_TYPE_GB2312){
		return NULL;
	}
	
	char * lpBytesTMP = NULL;
	int bytesNum = 0;			// include the one more space for '\0', '\0\0' or other types

	bytesNum = WideCharToMultiByte(type, 0, wchars, -1, NULL, 0, NULL, NULL);
	if(!bytesNum){
		return NULL;			// Space Calculation Fails
	}
	lpBytesTMP = new (std::nothrow) char[bytesNum];
	if(!lpBytesTMP){
		return NULL;			// OOM
	}
	if(!WideCharToMultiByte(type, 0, wchars, -1, lpBytesTMP, bytesNum, NULL, NULL)){
		delete[] lpBytesTMP;
		return NULL;			// Tranfer fails
	}
	// Success
	return lpBytesTMP;
}
