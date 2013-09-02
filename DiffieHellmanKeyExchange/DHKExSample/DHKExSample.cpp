// DHKExSample.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <MyLib/String/StringUtil.h>
#include <MyLib/Data/DataUtil.h>
#include "DHKeyServer.h"
#include "DHKeyClient.h"

std::tstring outputBinaryData(const std::tstring& name, const MyLib::Data::BinaryData& data) {
	std::tostringstream oss;
	oss << name << _T(" size=") << data.size() << std::endl <<
		MyLib::String::toHexStr(&data[0], data.size()) << std::endl;
	return oss.str();
}

int _tmain(int argc, _TCHAR* argv[]) {
	CDHKeyServer dhkeyServer;
	dhkeyServer.createPublickey();
	CDHKeyClient dhkeyClient;
	dhkeyClient.createPublickey(dhkeyServer.prime(), dhkeyServer.generator());
	dhkeyServer.setClientKey(dhkeyClient.publicKey());
	dhkeyClient.setServerKey(dhkeyServer.publicKey());

	MyLib::Data::BinaryData serverData = MyLib::Data::randomData(56);
	MyLib::Data::BinaryData serverEncrytoData = dhkeyServer.encrypto(serverData);
	MyLib::Data::BinaryData clientDecrytoData = dhkeyClient.decrypto(serverEncrytoData);

	MyLib::Data::BinaryData clientData = MyLib::Data::randomData(128);
	MyLib::Data::BinaryData clientEncrytoData = dhkeyClient.encrypto(clientData);
	MyLib::Data::BinaryData serverDecrytoData = dhkeyServer.decrypto(clientEncrytoData);

	std::tcout <<
		outputBinaryData(_T("[Sever Public Key]"), dhkeyServer.publicKey()) <<
		outputBinaryData(_T("[Prime]"), dhkeyServer.prime()) <<
		outputBinaryData(_T("[Generator]"), dhkeyServer.generator()) <<
		outputBinaryData(_T("[Client Public Key]"), dhkeyClient.publicKey()) <<
		outputBinaryData(_T("[Server Message]"), serverData) <<
		outputBinaryData(_T("[Server Encrypto Message]"), serverEncrytoData) <<
		outputBinaryData(_T("[Client Decrypto Message]"), clientDecrytoData) <<
		_T("[Server Message] == [Client Decrypto Message] ? ") << (serverData == clientDecrytoData) << std::endl << std::endl <<
		outputBinaryData(_T("[Client Message]"), clientData) <<
		outputBinaryData(_T("[Client Encrypto Message]"), clientEncrytoData) <<
		outputBinaryData(_T("[Server Decrypto Message]"), serverDecrytoData) <<
		_T("[Client Message] == [Server Decrypto Message] ? ") << (clientData == serverDecrytoData) << std::endl << std::endl;
	return 0;
}

