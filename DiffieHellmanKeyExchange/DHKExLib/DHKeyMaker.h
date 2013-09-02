#pragma once

#include <wincrypt.h>

#include <MyLib/Crypto/CryptoProvider.h>
#include <MyLib/Crypto/CryptoKey.h>

#include "DHBlob.h"

namespace DHKEx {

class CDHKeyMaker {
public:
	CDHKeyMaker();
	~CDHKeyMaker();

public:
	void createPublicKey();
	void createPublicKey(const DHBlob& prime, const DHBlob& generator);
	DHBlob publicKey() const;
	DHBlob prime() const;
	DHBlob generator() const;
	HCRYPTKEY createSecretKey(const DHBlob& publicKey);

private:
	MyLib::Crypto::CCryptoProvider m_cryptoProvider;
	MyLib::Crypto::CCryptoKey m_cryptoKey;
};

}
