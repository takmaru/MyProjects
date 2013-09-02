#include "stdafx.h"
#include "ParamParser.h"

//void ExecuteStatement(ODBCLib::CStatementHandle& statementHandle, const std::wstring& sql);
//void GetResultSet(ODBCLib::CStatementHandle& statementHandle);

int wmain(int argc, wchar_t* argv[], wchar_t* arge[]) {
	std::locale::global(std::locale("", std::locale::ctype));

	local::CParamPerser params(argc, argv);
	if(params.count() < 2) {
		std::wcout << L"ExecuteStoredProcedure [ <sqlfile> | -q <sql> [<sql> ...] ] -s <servername> -d <dbname>" << std::endl;
		return ERROR_INVALID_PARAMETER;
	}

	std::wstring servername;
	if(params[L"s"].size() > 0) {
		servername = *params[L"s"].begin();
	}
	std::wstring dbname;
	if(params[L"d"].size() > 0) {
		dbname = *params[L"d"].begin();
	}

	std::wstring sql;
	local::CParamPerser::paramlist sqlStrings = params[L"q"];
	if(sqlStrings.size() > 0) {
		local::CParamPerser::paramlist::iterator it;
		for(it = sqlStrings.begin(); it != sqlStrings.end(); ++it) {
			sql += (*it);
		}
	} else {
		std::wifstream fs(params[1]);
		std::wstring line;
		std::wostringstream oss;
		while(std::getline(fs, line)) {
			oss << line << std::endl;
		}
		sql = oss.str();
	}


	ODBCLib::CODBCSession session;
	session.setServerName(servername.c_str());
	session.setDatabaseName(dbname.c_str());
/*
	std::wstringstream oss;
	oss << L"DRIVER={SQL Server Native Client 10.0};SERVER=" << servername << L";" <<
		L"Database=" << dbname << L";" <<
		L"Trusted_Connection=yes";
//	std::wstring connectionString(L"DRIVER={SQL Server Native Client 10.0};SERVER=localhost\\SKYSEAMASTERSVR;Trusted_Connection=yes;Database=scvdb");
	std::wstring connectionString =oss.str();
*/
	std::wcout << L"接続文字列=[" << session.connectionString() << L"]" << std::endl;
	if(session.startSession()) {
		std::wcout << L"接続 成功" << std::endl;
		if(session.endSession()) {
			std::wcout << L"切断 成功" << std::endl;
		} else {
			std::wcout << L"切断 失敗" << std::endl;
		}
	} else {
		std::wcout << L"接続 失敗" << std::endl;
	}
/*
	ODBCLib::CEnvironmentHandle envHandle;
	ODBCLib::CConnectionHandle connectionHandle(envHandle);
	if(connectionHandle.connect(connectionString.c_str())) {
		std::wcout << L"接続 成功" << std::endl;

		ExecuteStatement(ODBCLib::CStatementHandle(connectionHandle), sql);
	} else {
		std::wcout << L"接続 失敗" << std::endl;
	}
*/
	return 0;
}
/*
void ExecuteStatement(ODBCLib::CStatementHandle& statementHandle, const std::wstring& sql) {

//	std::wcout << L"SQL=[" << sql << L"]" << std::endl;

	// Prepare:SQLステートメントの準備
	SQLRETURN res = statementHandle.Prepare(sql.c_str());
	if(!SQL_SUCCEEDED(res)) {
		std::wcout << L"Prepare 失敗(" << res << L")" << std::endl;
	}

	// TODO:パラメータバインド

	// Execute:SQLの実行
	std::wcout << L"SQL Execute" << std::endl;
	res = statementHandle.Execute();

	// 結果処理ループ
	while(res != SQL_NO_DATA) {
		switch(res) {
		case SQL_SUCCESS:
			// 結果セットの取得
			GetResultSet(statementHandle);
			break;
		case SQL_SUCCESS_WITH_INFO:
			std::wcout << L"情報" << std::endl <<
				ODBCLib::CErrorInfo(SQL_HANDLE_STMT, (SQLHSTMT)statementHandle).description() << std::endl;
			break;
		case SQL_ERROR:
			std::wcout << L"エラー" << std::endl <<
				ODBCLib::CErrorInfo(SQL_HANDLE_STMT, (SQLHSTMT)statementHandle).description() << std::endl;
			break;
		default:
			std::wcout << L"その他(" << res << L")" << std::endl;
			break;
		}

		// 次の結果セット取得
		res = statementHandle.MoreResults();
	}
}

void GetResultSet(ODBCLib::CStatementHandle& statementHandle) {

	std::wcout << L"成功" << std::endl;

	ODBCLib::CResultSet resultSet(statementHandle);
	if(resultSet.columnCount() > 0) {
		// 行数？＋結果セット
		std::wcout << L"RowCount=" << statementHandle.GetRowCount() << std::endl <<
			resultSet.description() << std::endl;

		// フェッチループ
		bool bLoop = true;
		while(bLoop) {
			SQLRETURN ret = resultSet.Fetch();
			switch(ret) {
			case SQL_SUCCESS:
				std::wcout << L"結果セットのフェッチ 成功" << std::endl <<
					resultSet.description_resultset() << std::endl;
				break;
			case SQL_NO_DATA:
				std::wcout << L"結果セットのフェッチ 完了" << std::endl;
				break;
			default:
				std::wcout << L"結果セットのフェッチでエラー" << std::endl <<
					ODBCLib::CErrorInfo(SQL_HANDLE_STMT, (SQLHSTMT)statementHandle).description() << std::endl;
				break;
			}
			bLoop = (ret == SQL_SUCCESS);
		}
	} else {
		// 処理行数
		SQLLEN rowCount = statementHandle.GetRowCount();
		if(rowCount == 0) {
			if(statementHandle.IsNoCount()) {
				std::wcout << L"ステートメントが完了しました。" << std::endl;
			} else {
				std::wcout << L"処理された行はありません。" << std::endl;
			}
		} else {
			std::wcout << rowCount << L" 行 処理されました。" << std::endl;
		}
	}
}
*/
