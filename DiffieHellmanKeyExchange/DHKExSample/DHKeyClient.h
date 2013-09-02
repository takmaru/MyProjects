#pragma once

#include <DHKExLib/DHKeyMaker.h>

class CDHKeyClient {
public:
	CDHKeyClient();
	~CDHKeyClient();
public:
	void createPublickey(const DHKEx::DHBlob& prime, const DHKEx::DHBlob& generator);
	void setServerKey(const DHKEx::DHBlob& serverKey);

	DHKEx::DHBlob publicKey() const {
		return m_dhkeyMaker.publicKey();
	}

public:
	DHKEx::DHBlob encrypto(const DHKEx::DHBlob& data);
	DHKEx::DHBlob decrypto(const DHKEx::DHBlob& encryptoData);

private:
	DHKEx::CDHKeyMaker m_dhkeyMaker;
	MyLib::Crypto::CCryptoKey m_sessionKey;
};
