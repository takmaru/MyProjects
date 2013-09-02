#include "stdafx.h"
#include "DHKeyClient.h"

CDHKeyClient::CDHKeyClient() : m_dhkeyMaker(), m_sessionKey() {
}
CDHKeyClient::~CDHKeyClient() {
}

void CDHKeyClient::createPublickey(const DHKEx::DHBlob& prime, const DHKEx::DHBlob& generator) {
	m_dhkeyMaker.createPublicKey(prime, generator);
}

void CDHKeyClient::setServerKey(const DHKEx::DHBlob& serverKey) {
	m_sessionKey.attach(m_dhkeyMaker.createSecretKey(serverKey));
	m_sessionKey.setAlgorithm(CALG_RC4);
}

DHKEx::DHBlob CDHKeyClient::encrypto(const DHKEx::DHBlob& data) {
	return m_sessionKey.encrypto(data);
}
DHKEx::DHBlob CDHKeyClient::decrypto(const DHKEx::DHBlob& encryptoData) {
	return m_sessionKey.decrypto(encryptoData);
}
