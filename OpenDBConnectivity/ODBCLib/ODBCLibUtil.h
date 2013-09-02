#pragma once

namespace ODBCLib {

std::wstring Datetime2String(const std::vector<unsigned char>& timestamp);

// GetDiagField
bool GetDiagFieldInfo_Value(SQLSMALLINT type, SQLHANDLE handle, SQLSMALLINT record, SQLSMALLINT diagId, SQLPOINTER value, SQLSMALLINT valuetype);
bool GetDiagFieldInfo_String(SQLSMALLINT type, SQLHANDLE handle, SQLSMALLINT record, SQLSMALLINT diagId, std::wstring& value);
bool GetDiagFieldInfo_String(SQLSMALLINT type, SQLHANDLE handle, SQLSMALLINT record, SQLSMALLINT diagId, wchar_t* value, SQLSMALLINT valuelength);

};