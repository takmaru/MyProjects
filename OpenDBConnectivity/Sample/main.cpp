#include <iostream>
#include <string>
#include <conio.h>

#include <Windows.h>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include <stdlib.h>

bool TransactionSequence(SQLHSTMT& hstmt)
{
	SQLRETURN res = SQL_SUCCESS;

	res = ::SQLPrepareW(hstmt, L"INSERT INTO TestTable(ID, Name, Price) VALUES(?, ?, ?)", SQL_NTS);
	if(!SQL_SUCCEEDED(res))	return false;
	int ID = 0;
	SQLLEN nRes_ID = 0;
	res = ::SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &ID, 0, &nRes_ID);
	if(!SQL_SUCCEEDED(res))	return false;
	SQLWCHAR Name[21] = {'0'};
	SQLLEN nRes_Name = SQL_NTS;
	res = ::SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 20, 0, Name, 0, &nRes_Name);
	if(!SQL_SUCCEEDED(res))	return false;
	int Price = 0;
	SQLLEN nRes_Price = 0;
	res = ::SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &Price, 0, &nRes_Price);
	if(!SQL_SUCCEEDED(res))	return false;

	ID = 3;	wcscpy_s(Name, L"サンドイッチ");	Price = 200;
	res = ::SQLExecute(hstmt);
	if(!SQL_SUCCEEDED(res))	return false;
	ID = 4;	wcscpy_s(Name, L"カップ麺");	Price = 250;
	res = ::SQLExecute(hstmt);
	if(!SQL_SUCCEEDED(res))	return false;

	return true;
}

bool CleanupTranSeq(SQLHSTMT& hstmt)
{
	SQLRETURN res = SQL_SUCCESS;

	res = ::SQLPrepareW(hstmt, L"DELETE FROM TestTable WHERE ID>=?", SQL_NTS);
	if(!SQL_SUCCEEDED(res))	return false;
	int ID = 0;
	SQLLEN nRes_ID = 0;
	res = ::SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &ID, 0, &nRes_ID);
	if(!SQL_SUCCEEDED(res))	return false;

	ID = 3;
	res = ::SQLExecute(hstmt);
	if(!SQL_SUCCEEDED(res))	return false;

	return true;
}

int wmain(int argc, wchar_t* argv[], wchar_t* arge[])
{
	std::locale::global(std::locale("", std::locale::ctype));

	SQLRETURN res = SQL_SUCCESS;

	bool bError = true;
	std::wstring strErr(L"");
	SQLSMALLINT errHandleType = 0;
	SQLHANDLE errHandle = SQL_NULL_HANDLE;


	SQLHENV henv = SQL_NULL_HANDLE;
	SQLHDBC hdbc = SQL_NULL_HANDLE;
	SQLHSTMT hstmt = SQL_NULL_HANDLE;

	bool bConnect = false;

	// 環境ハンドルの作成＆設定
	res = ::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	if(!SQL_SUCCEEDED(res)){strErr = L"環境ハンドル作成";	errHandleType = SQL_HANDLE_ENV;	errHandle = SQL_NULL_HANDLE;	goto end;}
	res = ::SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 0);
	if(!SQL_SUCCEEDED(res)){strErr = L"環境ハンドル設定";	errHandleType = SQL_HANDLE_ENV;	errHandle = henv;	goto end;}
	// 接続ハンドルの作成
	res = ::SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	if(!SQL_SUCCEEDED(res)){strErr = L"接続ハンドル作成";	errHandleType = SQL_HANDLE_ENV;	errHandle = henv;	goto end;}
	// ODBCドライバへ接続
	res = ::SQLDriverConnect(hdbc, NULL, L"DRIVER={SQL Server Native Client 10.0};SERVER=(local);Trusted_Connection=yes;Database=TestDB", SQL_NTS,
		NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
	if(!SQL_SUCCEEDED(res)){strErr = L"接続";	errHandleType = SQL_HANDLE_DBC;	errHandle = hdbc;	goto end;}
	bConnect = true;
	std::wcout << L"接続成功" << std::endl;

	// ステートメントハンドルを作成
	res = ::SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	if(!SQL_SUCCEEDED(res)){strErr = L"ステートメントハンドル作成";	errHandleType = SQL_HANDLE_DBC;	errHandle = hdbc;	goto end;}

/*	SQLExecute
	// SQL実行
	{	// INSERT
		res = ::SQLPrepareW(hstmt, L"INSERT INTO TestTable(ID, Name, Price) VALUES(?, ?, ?)", SQL_NTS);
		if(!SQL_SUCCEEDED(res)){strErr = L"INSERT 1 Prepare";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		int ID = 0;
		SQLLEN nRes_ID = 0;
		res = ::SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &ID, 0, &nRes_ID);
		if(!SQL_SUCCEEDED(res)){strErr = L"INSERT 1 BindParam 1";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		SQLWCHAR Name[21] = {'0'};
		SQLLEN nRes_Name = SQL_NTS;
		res = ::SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 20, 0, Name, 0, &nRes_Name);
		if(!SQL_SUCCEEDED(res)){strErr = L"INSERT 1 BindParam 2";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		int Price = 0;
		SQLLEN nRes_Price = 0;
		res = ::SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &Price, 0, &nRes_Price);
		if(!SQL_SUCCEEDED(res)){strErr = L"INSERT 1 BindParam 3";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}

		ID = 3;	wcscpy_s(Name, L"サンドイッチ");	Price = 200;
		res = ::SQLExecute(hstmt);
		if(!SQL_SUCCEEDED(res)){strErr = L"INSERT 1 Execute";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		ID = 4;	wcscpy_s(Name, L"カップ麺");	Price = 250;
		res = ::SQLExecute(hstmt);
		if(!SQL_SUCCEEDED(res)){strErr = L"INSERT 2 Execute";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
	}
	{	// UPDATE
		res = ::SQLPrepareW(hstmt, L"UPDATE TestTable SET Price=? WHERE Price=?", SQL_NTS);
		if(!SQL_SUCCEEDED(res)){strErr = L"UPDATE 1 Prepare";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		int Price_After = 0;
		SQLLEN nRes_PriceA = 0;
		res = ::SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &Price_After, 0, &nRes_PriceA);
		if(!SQL_SUCCEEDED(res)){strErr = L"UPDATE 1 BindParam 1";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		int Price_Before = 0;
		SQLLEN nRes_PriceB = 0;
		res = ::SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &Price_Before, 0, &nRes_PriceB);
		if(!SQL_SUCCEEDED(res)){strErr = L"UPDATE 1 BindParam 1";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}

		Price_After = 210;
		Price_Before = 200;
		res = ::SQLExecute(hstmt);
		if(!SQL_SUCCEEDED(res)){strErr = L"UPDATE 1 Execute";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
	}
	{	// DELETE
		res = ::SQLPrepareW(hstmt, L"DELETE FROM TestTable WHERE ID>=?", SQL_NTS);
		if(!SQL_SUCCEEDED(res)){strErr = L"DELETE 1 Prepare";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		int ID = 0;
		SQLLEN nRes_ID = 0;
		res = ::SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &ID, 0, &nRes_ID);
		if(!SQL_SUCCEEDED(res)){strErr = L"DELETE 1 BindParam 1";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}

		ID = 3;
		res = ::SQLExecute(hstmt);
		if(!SQL_SUCCEEDED(res)){strErr = L"DELETE 1 Execute";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
	}
	{	// SELECT
		res = ::SQLPrepareW(hstmt, L"SELECT * FROM TestTable WHERE Price>=?", SQL_NTS);
		if(!SQL_SUCCEEDED(res)){strErr = L"SELECT 1 Prepare";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		int Price = 0;
		SQLLEN nRes_Price = 0;
		res = ::SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &Price, 0, &nRes_Price);
		if(!SQL_SUCCEEDED(res)){strErr = L"SELECT 1 BindParam 1";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}

		Price = 100;
		res = ::SQLExecute(hstmt);
		if(!SQL_SUCCEEDED(res)){strErr = L"SELECT 1 Execute";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		{
			std::wcout << L"SQL実行 SELECT 1" << std::endl;
			SQLINTEGER id = 0;
			SQLWCHAR name[20 + 1] ={L'\0'};
			SQLINTEGER price = 0;
			SQLLEN len1 = 0;
			SQLLEN len2 = 0;
			SQLLEN len3 = 0;
			res = ::SQLBindCol(hstmt, 1, SQL_C_SLONG, &id, sizeof(id), &len1);
			if(!SQL_SUCCEEDED(res)){strErr = L"SELECT 1 Bind 1";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
			res = ::SQLBindCol(hstmt, 2, SQL_C_WCHAR, &name, sizeof(name), &len2);
			if(!SQL_SUCCEEDED(res)){strErr = L"SELECT 1 Bind 2";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
			res = ::SQLBindCol(hstmt, 3, SQL_C_SLONG, &price, sizeof(price), &len3);
			if(!SQL_SUCCEEDED(res)){strErr = L"SELECT 1 Bind 3";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
			while(SQL_SUCCEEDED(::SQLFetch(hstmt)))
				std::wcout << id << L", " << name << L", " << price
					<< L" : (" << len1 << L", " << len2 << L", " << len3 << L')' <<  std::endl;
		}
		res = ::SQLCloseCursor(hstmt);
		if(!SQL_SUCCEEDED(res)){strErr = L"Close カーソル SELECT 1";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
	}
*/
/*	SQLExecDirect
	// INSERT
	res = ::SQLExecDirect(hstmt, L"INSERT INTO TestTable(ID, Name, Price) VALUES(3, 'サンドイッチ', 200)", SQL_NTS);
	if(!SQL_SUCCEEDED(res)){strErr = L"SQL実行 INSERT 1";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
	{
		SQLLEN cnt = 0;
		res = ::SQLRowCount(hstmt, &cnt);
		if(!SQL_SUCCEEDED(res)){strErr = L"SQL実行 INSERT 1 結果列数取得";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		std::wcout << L"SQL実行 INSERT 1 : " << cnt << L" 行  処理されました。" << std::endl;
	}
	// UPDATE
	res = ::SQLExecDirect(hstmt, L"UPDATE TestTable SET Price=250 WHERE Price=200", SQL_NTS);
	if(res == SQL_NO_DATA){std::wcout << L"SQL実行 UPDATE 1 : データがありませんでした。" << std::endl;}
	else if(!SQL_SUCCEEDED(res)){strErr = L"SQL実行 UPDATE 1";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
	else
	{
		SQLLEN cnt = 0;
		res = ::SQLRowCount(hstmt, &cnt);
		if(!SQL_SUCCEEDED(res)){strErr = L"SQL実行 UPDATE 1 結果列数取得";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		std::wcout << L"SQL実行 UPDATE 1 : " << cnt << L" 行  処理されました。" << std::endl;
	}
	// DELETE
	res = ::SQLExecDirect(hstmt, L"DELETE FROM TestTable WHERE Price=250", SQL_NTS);
	if(res == SQL_NO_DATA){std::wcout << L"SQL実行 DELETE 1 : データがありませんでした。" << std::endl;}
	else if(!SQL_SUCCEEDED(res)){strErr = L"SQL実行 DELETE 1";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
	else
	{
		SQLLEN cnt = 0;
		res = ::SQLRowCount(hstmt, &cnt);
		if(!SQL_SUCCEEDED(res)){strErr = L"SQL実行 DELETE 1 結果列数取得";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		std::wcout << L"SQL実行 DELETE 1 : " << cnt << L" 行  処理されました。" << std::endl;
	}
	// SELECT
	res = ::SQLExecDirect(hstmt, L"SELECT * FROM TestTable", SQL_NTS);
	if(!SQL_SUCCEEDED(res)){strErr = L"SQL実行 SELECT 1";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
	{
		std::wcout << L"SQL実行 SELECT 1" << std::endl;
		SQLINTEGER id = 0;
		SQLWCHAR name[20 + 1] ={L'\0'};
		SQLINTEGER price = 0;
		SQLLEN len1 = 0;
		SQLLEN len2 = 0;
		SQLLEN len3 = 0;
		res = ::SQLBindCol(hstmt, 1, SQL_C_SLONG, &id, sizeof(id), &len1);
		if(!SQL_SUCCEEDED(res)){strErr = L"SELECT 1 Bind 1";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		res = ::SQLBindCol(hstmt, 2, SQL_C_WCHAR, &name, sizeof(name), &len2);
		if(!SQL_SUCCEEDED(res)){strErr = L"SELECT 1 Bind 2";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		res = ::SQLBindCol(hstmt, 3, SQL_C_SLONG, &price, sizeof(price), &len3);
		if(!SQL_SUCCEEDED(res)){strErr = L"SELECT 1 Bind 3";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		while(SQL_SUCCEEDED(::SQLFetch(hstmt)))
			std::wcout << id << L", " << name << L", " << price
				<< L" : (" << len1 << L", " << len2 << L", " << len3 << L')' <<  std::endl;
	}
	res = ::SQLCloseCursor(hstmt);
	if(!SQL_SUCCEEDED(res)){strErr = L"Close カーソル SELECT 1";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
*/

	{	// Auto Commit Off
		res = ::SQLSetConnectAttrW(hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_IS_UINTEGER);
		if(!SQL_SUCCEEDED(res)){strErr = L"Connect属性セット 1";	errHandleType = SQL_HANDLE_DBC;	errHandle = hdbc;	goto end;}
		// Rollback
		if(!TransactionSequence(hstmt)){strErr = L"TranSeq 1";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		res = ::SQLEndTran(SQL_HANDLE_DBC, hdbc, SQL_ROLLBACK);
		if(!SQL_SUCCEEDED(res)){strErr = L"Rollback";	errHandleType = SQL_HANDLE_DBC;	errHandle = hdbc;	goto end;}
		// Commit
		if(!TransactionSequence(hstmt)){strErr = L"TranSeq 2";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		res = ::SQLEndTran(SQL_HANDLE_DBC, hdbc, SQL_COMMIT);
		if(!SQL_SUCCEEDED(res)){strErr = L"Commit";	errHandleType = SQL_HANDLE_DBC;	errHandle = hdbc;	goto end;}
		// Cleanup
		if(!CleanupTranSeq(hstmt)){strErr = L"CleanTranSeq 1";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		res = ::SQLEndTran(SQL_HANDLE_DBC, hdbc, SQL_COMMIT);
		if(!SQL_SUCCEEDED(res)){strErr = L"Commit Cleanup 1";	errHandleType = SQL_HANDLE_DBC;	errHandle = hdbc;	goto end;}
	}
	{	// Auto Commit On
		res = ::SQLSetConnectAttrW(hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_IS_UINTEGER);
		if(!SQL_SUCCEEDED(res)){strErr = L"Connect属性セット 2";	errHandleType = SQL_HANDLE_DBC;	errHandle = hdbc;	goto end;}
		// Rollback Sequence
		res = ::SQLExecDirect(hstmt, L"BEGIN TRANSACTION", SQL_NTS);
		if(!SQL_SUCCEEDED(res)){strErr = L"SQL実行 BEGIN TRANSACTION 1";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		if(!TransactionSequence(hstmt)){strErr = L"TranSeq 3";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		res = ::SQLExecDirect(hstmt, L"ROLLBACK TRANSACTION", SQL_NTS);
		if(!SQL_SUCCEEDED(res)){strErr = L"SQL実行 ROLLBACK TRANSACTION 2";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		// Commit
		res = ::SQLExecDirect(hstmt, L"BEGIN TRANSACTION", SQL_NTS);
		if(!SQL_SUCCEEDED(res)){strErr = L"SQL実行 BEGIN TRANSACTION 2";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		if(!TransactionSequence(hstmt)){strErr = L"TranSeq 4";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		res = ::SQLExecDirect(hstmt, L"COMMIT TRANSACTION", SQL_NTS);
		if(!SQL_SUCCEEDED(res)){strErr = L"SQL実行 ROLLBACK TRANSACTION 3";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
		// Cleanup
		if(!CleanupTranSeq(hstmt)){strErr = L"CleanTranSeq 2";	errHandleType = SQL_HANDLE_STMT;	errHandle = hstmt;	goto end;}
	}


	std::wcout << L"SQL実行成功" << std::endl;

	bError = false;
end:
	if(bError)
	{
		SQLWCHAR szState[6] = {L'\0'};
		SQLWCHAR szErrorMsg[1024] = {L'\0'};
		SQLINTEGER  nErrorCode = 0;
		SQLSMALLINT nSize = 0;
		::SQLGetDiagRec(errHandleType, errHandle, 1, szState, &nErrorCode, szErrorMsg, sizeof(szErrorMsg), &nSize);
		std::wcout << L"エラー(" << strErr
			<< L") State=[" << szState
			<< L"] ErrorMsg[" << szErrorMsg << L"]" << std::endl;
	}

	if(hstmt != SQL_NULL_HANDLE)	::SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	if(hdbc != SQL_NULL_HANDLE)
	{
		if(bConnect)	::SQLDisconnect(hdbc);
		::SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	}
	if(henv != SQL_NULL_HANDLE)	::SQLFreeHandle(SQL_HANDLE_ENV, henv);

	std::wcout << L"終了するには何かキーを押してください。" << std::endl;
	_getch();
	return 0;
}