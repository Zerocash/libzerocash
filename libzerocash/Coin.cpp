/** @file
 *****************************************************************************

 Implementation of interfaces for the class Coin.

 See coin.h .

 *****************************************************************************
 * @author     This file is part of libzerocash, developed by the Zerocash
 *             project and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#include <cryptopp/osrng.h>
using CryptoPP::AutoSeededRandomPool;

#include <cryptopp/eccrypto.h>
using CryptoPP::ECP;
using CryptoPP::ECIES;

#include <cryptopp/oids.h>
namespace ASN1 = CryptoPP::ASN1;

#include <cryptopp/filters.h>
using CryptoPP::StringSink;
using CryptoPP::StringStore;

#include <stdexcept>

#include "Zerocash.h"
#include "Coin.h"

namespace libzerocash {

Coin::Coin(): addr_pk(), cm(), rho(rho_size), r(zc_r_size), coinValue(v_size) {

}

Coin::Coin(const std::string bucket, Address& addr): addr_pk(), cm(), rho(rho_size), r(zc_r_size), coinValue(v_size) {
    // Retreive and decode the private key
    ECIES<ECP>::PrivateKey decodedPrivateKey;
    decodedPrivateKey.Load(StringStore(addr.getEncryptionSecretKey()).Ref());

    // Create the decryption session
    AutoSeededRandomPool prng;
    ECIES<ECP>::Decryptor decrypt(decodedPrivateKey);

    // Convert the input string into a vector of bytes
    std::vector<byte> bucket_bytes(bucket.begin(), bucket.end());

    // Construct a temporary object to store the plaintext, large enough
    // to store the plaintext if it were extended beyond the real size.
    std::vector<unsigned char> plaintext;
    // Size as needed, filling with zeros.
    plaintext.resize(decrypt.MaxPlaintextLength(decrypt.CiphertextLength(v_size + zc_r_size + rho_size)), 0);

    // Perform the decryption
    decrypt.Decrypt(prng,
                    &bucket_bytes[0],
                    decrypt.CiphertextLength(v_size + zc_r_size + rho_size),
                    &plaintext[0]);

    // Grab the byte vectors
    std::vector<unsigned char> value_v(plaintext.begin(),
                                       plaintext.begin() + v_size);
    std::vector<unsigned char>     r_v(plaintext.begin() + v_size,
                                       plaintext.begin() + v_size + zc_r_size);
    std::vector<unsigned char>   rho_v(plaintext.begin() + v_size + zc_r_size,
                                       plaintext.begin() + v_size + zc_r_size + rho_size);

    this->coinValue = value_v;
    this->r = r_v;
    this->rho = rho_v;
    this->addr_pk = addr.getPublicAddress();

    std::vector<unsigned char> a_pk = addr.getPublicAddress().getPublicAddressSecret();

    this->computeCommitments(a_pk);
}

Coin::Coin(const PublicAddress& addr, uint64_t value): addr_pk(addr), cm(), rho(rho_size), r(zc_r_size), k(k_size), coinValue(v_size)
{
    convertIntToBytesVector(value, this->coinValue);

    std::vector<unsigned char> a_pk = addr.getPublicAddressSecret();

    unsigned char rho_bytes[rho_size];
    getRandBytes(rho_bytes, rho_size);
    convertBytesToBytesVector(rho_bytes, this->rho);

    unsigned char r_bytes[zc_r_size];
    getRandBytes(r_bytes, zc_r_size);
    convertBytesToBytesVector(r_bytes, this->r);

	this->computeCommitments(a_pk);
}


Coin::Coin(const PublicAddress& addr, uint64_t value,
		   const std::vector<unsigned char>& rho, const std::vector<unsigned char>& r): addr_pk(addr), rho(rho), r(r), k(k_size), coinValue(v_size)
{
    convertIntToBytesVector(value, this->coinValue);

    std::vector<unsigned char> a_pk = addr.getPublicAddressSecret();

	this->computeCommitments(a_pk);
}

void
Coin::computeCommitments(std::vector<unsigned char>& a_pk)
{
    std::vector<unsigned char> k_internal;
    std::vector<unsigned char> k_internalhash_trunc(16);

    std::vector<unsigned char> k_internalhash_internal;
    concatenateVectors(a_pk, this->rho, k_internalhash_internal);

    std::vector<unsigned char> k_internalhash(k_size);
    hashVector(k_internalhash_internal, k_internalhash);

    copy(k_internalhash.begin(), k_internalhash.begin()+16, k_internalhash_trunc.begin());
    concatenateVectors(this->r, k_internalhash_trunc, k_internal);
    hashVector(k_internal, this->k);

    CoinCommitment com(this->coinValue, this->k);
    this->cm = com;
}

bool Coin::operator==(const Coin& rhs) const {
	return ((this->cm == rhs.cm) && (this->rho == rhs.rho) && (this->r == rhs.r) && (this->k == rhs.k) && (this->coinValue == rhs.coinValue) && (this->addr_pk == rhs.addr_pk));
}

bool Coin::operator!=(const Coin& rhs) const {
	return !(*this == rhs);
}

const PublicAddress& Coin::getPublicAddress() const {
	return this->addr_pk;
}

const CoinCommitment& Coin::getCoinCommitment() const {
	return this->cm;
}

const std::vector<unsigned char>& Coin::getInternalCommitment() const {
	return this->k;
}

const std::vector<unsigned char>& Coin::getRho() const {
    return this->rho;
}

const std::vector<unsigned char>& Coin::getR() const {
    return this->r;
}

uint64_t Coin::getValue() const {
    return convertBytesVectorToInt(this->coinValue);
}

} /* namespace libzerocash */
