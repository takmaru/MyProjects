#include "stdafx.h"
#include "ODBCLibUtil.h"

std::wstring ODBCLib::Datetime2String(const std::vector<unsigned char>& timestamp) {
	unsigned int dateCount = *(unsigned int*)(&timestamp[0]);
	unsigned int timeCount = *(unsigned int*)(&timestamp[4]);
	std::wostringstream oss;
	oss << L"date:" << dateCount << L" time:" << timeCount;
	return oss.str();
}

bool ODBCLib::GetDiagFieldInfo_Value(SQLSMALLINT type, SQLHANDLE handle, SQLSMALLINT record, SQLSMALLINT diagId, SQLPOINTER value, SQLSMALLINT valuetype) {
	bool result = false;
	SQLRETURN ret = ::SQLGetDiagFieldW(type, handle, record, diagId, value, valuetype, NULL);
	if(ret == SQL_SUCCESS) {
		result = true;
	} else {
		std::wcerr << L"ODBCLibUtil::GetDiagFieldInfo_Value() SQLGetDiagFieldW(ID:" << diagId << ") error(" << ret << L")" << std::endl;
	}
	return result;
}

bool ODBCLib::GetDiagFieldInfo_String(SQLSMALLINT type, SQLHANDLE handle, SQLSMALLINT record, SQLSMALLINT diagId, std::wstring& value) {
	bool result = false;

	SQLRETURN ret = SQL_SUCCESS;
	SQLSMALLINT bytes = 0;
	std::vector<unsigned char> fieldStrBuffer;
	ret = ::SQLGetDiagFieldW(type, handle, record, diagId, NULL, 0, &bytes);
	if(ret == SQL_SUCCESS_WITH_INFO) {
		if(bytes > 0) {
			// SQL_SUCCESS_WITH_INFOでデータ長が返ってきていれば、文字列取得
			bytes += sizeof(wchar_t);	// NULL文字分、追加
			fieldStrBuffer.resize(bytes, L'\0');
			wchar_t* bufferPointer = reinterpret_cast<wchar_t*>(&(*fieldStrBuffer.begin()));
			if(ODBCLib::GetDiagFieldInfo_String(type, handle, record, diagId, bufferPointer, bytes)) {
				value = bufferPointer;
				result = true;
			}
		} else {
			std::wcerr << L"ODBCLibUtil::GetDiagFieldInfo_String() SQLGetDiagFieldW(ID:" << diagId << ") getLength=0" << std::endl;
		}
	} else {
		// getLength error
		std::wcerr << L"ODBCLibUtil::GetDiagFieldInfo_String() SQLGetDiagFieldW(ID:" << diagId << ") getLength error(" << ret << L")" << std::endl;
	}

	return result;
}

bool ODBCLib::GetDiagFieldInfo_String(SQLSMALLINT type, SQLHANDLE handle, SQLSMALLINT record, SQLSMALLINT diagId, wchar_t* value, SQLSMALLINT valuelength) {
	bool result = false;
	SQLSMALLINT infolength = 0;
	SQLRETURN ret = ::SQLGetDiagFieldW(type, handle, record, diagId, value, valuelength, &infolength);
	if(ret == SQL_SUCCESS) {
		result = true;
	} else {
		std::wcerr << L"ODBCLibUtil::GetDiagFieldInfo_String() SQLGetDiagFieldW(ID:" << diagId << ") error(" << ret << L")" << std::endl;
	}
	return result;
}