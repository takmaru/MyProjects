#include "stdafx.h"
#include "DHKeyMaker.h"

#include "DHKExException.h"


DHKEx::CDHKeyMaker::CDHKeyMaker() : m_cryptoProvider(), m_cryptoKey() {
}
DHKEx::CDHKeyMaker::~CDHKeyMaker() {
}

void DHKEx::CDHKeyMaker::createPublicKey() {

	if(m_cryptoProvider.isOpen()) {
		RAISE_DHKExEXCEPTION("createPublicKey provider is opened");
	}
	if(m_cryptoKey.isEnable()) {
		RAISE_DHKExEXCEPTION("createPublicKey publicKey is enabled");
	}

	// Microsoft Diffie-Hellman Crypto Provider を取得
	MyLib::Crypto::CCryptoProvider cryptoProvider;
	if(!::CryptAcquireContext(cryptoProvider, NULL, MS_ENH_DSS_DH_PROV, PROV_DSS_DH, CRYPT_VERIFYCONTEXT)) {
		RAISE_DHKExEXCEPTION("createPublicKey CryptAcquireContext err(%u)", ::GetLastError());
    }

	// 一時的な秘密キーを生成（Prime、Generatorは自動生成）
	MyLib::Crypto::CCryptoKey cryptoKey;
	if(!::CryptGenKey(cryptoProvider, CALG_DH_EPHEM, CRYPT_EXPORTABLE, cryptoKey)) {
		RAISE_DHKExEXCEPTION("createPublicKey CryptGenKey err(%u)", ::GetLastError());
	}

	m_cryptoProvider.attach(cryptoProvider.release());
	m_cryptoKey.attach(cryptoKey.release());
	return;
}

void DHKEx::CDHKeyMaker::createPublicKey(const DHBlob& prime, const DHBlob& generator) {
	if(m_cryptoProvider.isOpen()) {
		RAISE_DHKExEXCEPTION("createPublicKey(P,B) provider is opened");
	}
	if(m_cryptoKey.isEnable()) {
		RAISE_DHKExEXCEPTION("createPublicKey(P,B) publicKey is enabled");
	}
	
	// Microsoft Diffie-Hellman Crypto Provider を取得
	MyLib::Crypto::CCryptoProvider cryptoProvider;
	if(!::CryptAcquireContext(cryptoProvider, NULL, MS_ENH_DSS_DH_PROV, PROV_DSS_DH, CRYPT_VERIFYCONTEXT)) {
		RAISE_DHKExEXCEPTION("createPublicKey(P,B) CryptAcquireContext err(%u)", ::GetLastError());
    }
	// 一時的な秘密キーを生成
	MyLib::Crypto::CCryptoKey cryptoKey;
	if(!::CryptGenKey(cryptoProvider, CALG_DH_EPHEM, CRYPT_EXPORTABLE | CRYPT_PREGEN, cryptoKey)) {
		RAISE_DHKExEXCEPTION("createPublicKey(P,B) CryptGenKey err(%u)", ::GetLastError());
	}
	// Prime セット
	DHBlob prime_work(prime);
	CRYPT_INTEGER_BLOB primeBlob = {prime_work.size(), &prime_work[0]};
	if(!::CryptSetKeyParam(cryptoKey, KP_P, (const BYTE*)&primeBlob, 0)) {
		RAISE_DHKExEXCEPTION("createPublicKey(P,B) CryptSetKeyParam(prime) err(%u)", ::GetLastError());
	}
	// generator セット
	DHBlob generator_work(generator);
	CRYPT_INTEGER_BLOB generatorBlob = {generator_work.size(), &generator_work[0]};
	if(!::CryptSetKeyParam(cryptoKey, KP_G, (const BYTE*)&generatorBlob, 0)) {
		RAISE_DHKExEXCEPTION("createPublicKey(P,B) CryptSetKeyParam(generator) err(%u)", ::GetLastError());
	}
	// 公開キー 作成
	if(!::CryptSetKeyParam(cryptoKey, KP_X, NULL, 0)) {
		RAISE_DHKExEXCEPTION("createPublicKey(P,B) CryptSetKeyParam(PublicKey) err(%u)", ::GetLastError());
	}

	m_cryptoProvider.attach(cryptoProvider.release());
	m_cryptoKey.attach(cryptoKey.release());
	return;
}

DHKEx::DHBlob DHKEx::CDHKeyMaker::publicKey() const {
	// 相手に渡す公開キーを取得
	DWORD length = 0;
	if(!::CryptExportKey(m_cryptoKey, NULL, PUBLICKEYBLOB, 0, NULL, &length)) {
		RAISE_DHKExEXCEPTION("createPublicKey CryptExportKey(getLength) err(%u)", ::GetLastError());
	}
	DHKEx::DHBlob publicKey(length, 0);
	if(!::CryptExportKey(m_cryptoKey, NULL, PUBLICKEYBLOB, 0, &publicKey[0], &length)) {
		RAISE_DHKExEXCEPTION("createPublicKey CryptExportKey err(%u)", ::GetLastError());
	}
	return publicKey;
}

DHKEx::DHBlob DHKEx::CDHKeyMaker::prime() const {
	if(!m_cryptoProvider.isOpen()) {
		RAISE_DHKExEXCEPTION("prime provider isn't opened");
	}
	if(!m_cryptoKey.isEnable()) {
		RAISE_DHKExEXCEPTION("prime publicKey isn't enabled");
	}

	DWORD length = 0;
	if(!::CryptGetKeyParam(m_cryptoKey, KP_P, NULL, &length, 0)) {
		RAISE_DHKExEXCEPTION("prime CryptGetKeyParam(getLength) err(%u)", ::GetLastError());
	}
	DHKEx::DHBlob primeValue(length, 0);
	if(!::CryptGetKeyParam(m_cryptoKey, KP_P, &primeValue[0], &length, 0)) {
		RAISE_DHKExEXCEPTION("prime CryptGetKeyParam err(%u)", ::GetLastError());
	}
	return primeValue;
}

DHKEx::DHBlob DHKEx::CDHKeyMaker::generator() const {
	if(!m_cryptoProvider.isOpen()) {
		RAISE_DHKExEXCEPTION("generator provider isn't opened");
	}
	if(!m_cryptoKey.isEnable()) {
		RAISE_DHKExEXCEPTION("generator publicKey isn't enabled");
	}

	DWORD length = 0;
	if(!::CryptGetKeyParam(m_cryptoKey, KP_G, NULL, &length, 0)) {
		RAISE_DHKExEXCEPTION("generator CryptGetKeyParam(getLength) err(%u)", ::GetLastError());
	}
	DHKEx::DHBlob generatorValue(length, 0);
	if(!::CryptGetKeyParam(m_cryptoKey, KP_G, &generatorValue[0], &length, 0)) {
		RAISE_DHKExEXCEPTION("generator CryptGetKeyParam err(%u)", ::GetLastError());
	}
	return generatorValue;
}
HCRYPTKEY DHKEx::CDHKeyMaker::createSecretKey(const DHBlob& publicKey) {
	if(!m_cryptoProvider.isOpen()) {
		RAISE_DHKExEXCEPTION("createSecretKey provider isn't opened");
	}
	if(!m_cryptoKey.isEnable()) {
		RAISE_DHKExEXCEPTION("createSecretKey publicKey isn't enabled");
	}

	DHBlob publicKey_work(publicKey);
	HCRYPTKEY secretKey = NULL;
	if(!::CryptImportKey(m_cryptoProvider, &publicKey_work[0], publicKey_work.size(), m_cryptoKey, 0, &secretKey)) {
		RAISE_DHKExEXCEPTION("createSecretKey CryptImportKey err(%u)", ::GetLastError());
	}
	return secretKey;
}
