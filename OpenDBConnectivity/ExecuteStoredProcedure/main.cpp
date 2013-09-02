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
	std::wcout << L"�ڑ�������=[" << session.connectionString() << L"]" << std::endl;
	if(session.startSession()) {
		std::wcout << L"�ڑ� ����" << std::endl;
		if(session.endSession()) {
			std::wcout << L"�ؒf ����" << std::endl;
		} else {
			std::wcout << L"�ؒf ���s" << std::endl;
		}
	} else {
		std::wcout << L"�ڑ� ���s" << std::endl;
	}
/*
	ODBCLib::CEnvironmentHandle envHandle;
	ODBCLib::CConnectionHandle connectionHandle(envHandle);
	if(connectionHandle.connect(connectionString.c_str())) {
		std::wcout << L"�ڑ� ����" << std::endl;

		ExecuteStatement(ODBCLib::CStatementHandle(connectionHandle), sql);
	} else {
		std::wcout << L"�ڑ� ���s" << std::endl;
	}
*/
	return 0;
}
/*
void ExecuteStatement(ODBCLib::CStatementHandle& statementHandle, const std::wstring& sql) {

//	std::wcout << L"SQL=[" << sql << L"]" << std::endl;

	// Prepare:SQL�X�e�[�g�����g�̏���
	SQLRETURN res = statementHandle.Prepare(sql.c_str());
	if(!SQL_SUCCEEDED(res)) {
		std::wcout << L"Prepare ���s(" << res << L")" << std::endl;
	}

	// TODO:�p�����[�^�o�C���h

	// Execute:SQL�̎��s
	std::wcout << L"SQL Execute" << std::endl;
	res = statementHandle.Execute();

	// ���ʏ������[�v
	while(res != SQL_NO_DATA) {
		switch(res) {
		case SQL_SUCCESS:
			// ���ʃZ�b�g�̎擾
			GetResultSet(statementHandle);
			break;
		case SQL_SUCCESS_WITH_INFO:
			std::wcout << L"���" << std::endl <<
				ODBCLib::CErrorInfo(SQL_HANDLE_STMT, (SQLHSTMT)statementHandle).description() << std::endl;
			break;
		case SQL_ERROR:
			std::wcout << L"�G���[" << std::endl <<
				ODBCLib::CErrorInfo(SQL_HANDLE_STMT, (SQLHSTMT)statementHandle).description() << std::endl;
			break;
		default:
			std::wcout << L"���̑�(" << res << L")" << std::endl;
			break;
		}

		// ���̌��ʃZ�b�g�擾
		res = statementHandle.MoreResults();
	}
}

void GetResultSet(ODBCLib::CStatementHandle& statementHandle) {

	std::wcout << L"����" << std::endl;

	ODBCLib::CResultSet resultSet(statementHandle);
	if(resultSet.columnCount() > 0) {
		// �s���H�{���ʃZ�b�g
		std::wcout << L"RowCount=" << statementHandle.GetRowCount() << std::endl <<
			resultSet.description() << std::endl;

		// �t�F�b�`���[�v
		bool bLoop = true;
		while(bLoop) {
			SQLRETURN ret = resultSet.Fetch();
			switch(ret) {
			case SQL_SUCCESS:
				std::wcout << L"���ʃZ�b�g�̃t�F�b�` ����" << std::endl <<
					resultSet.description_resultset() << std::endl;
				break;
			case SQL_NO_DATA:
				std::wcout << L"���ʃZ�b�g�̃t�F�b�` ����" << std::endl;
				break;
			default:
				std::wcout << L"���ʃZ�b�g�̃t�F�b�`�ŃG���[" << std::endl <<
					ODBCLib::CErrorInfo(SQL_HANDLE_STMT, (SQLHSTMT)statementHandle).description() << std::endl;
				break;
			}
			bLoop = (ret == SQL_SUCCESS);
		}
	} else {
		// �����s��
		SQLLEN rowCount = statementHandle.GetRowCount();
		if(rowCount == 0) {
			if(statementHandle.IsNoCount()) {
				std::wcout << L"�X�e�[�g�����g���������܂����B" << std::endl;
			} else {
				std::wcout << L"�������ꂽ�s�͂���܂���B" << std::endl;
			}
		} else {
			std::wcout << rowCount << L" �s ��������܂����B" << std::endl;
		}
	}
}
*/
