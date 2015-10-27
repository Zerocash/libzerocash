/** @file
 *****************************************************************************

 Declaration of interfaces for the classes Address and PublicAddress.

 *****************************************************************************
 * @author     This file is part of libzerocash, developed by the Zerocash
 *             project and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef ADDRESS_H_
#define ADDRESS_H_

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

#include <vector>
#include <string>

#include "serialize.h"

namespace libzerocash {

/***************************** Private address ********************************/

class PrivateAddress {

    friend class Address;
    friend class Coin;
    friend class PourTransaction;

public:
    PrivateAddress();

	bool operator==(const PrivateAddress& rhs) const;
	bool operator!=(const PrivateAddress& rhs) const;

	ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
	    READWRITE(a_sk);
	    READWRITE(sk_enc);
    }

    void createPrivateAddress(const std::vector<unsigned char> a_sk, const std::string sk_enc);

private:
    std::vector<unsigned char> a_sk;
    std::string sk_enc;

    const std::vector<unsigned char>& getAddressSecret() const;
    const std::string getEncryptionSecretKey() const;
};

/***************************** Public address ********************************/

class PublicAddress {

friend class Address;
friend class Coin;
friend class PourTransaction;

public:
	PublicAddress();
    PublicAddress(const std::vector<unsigned char>& a_sk, const std::string sk_enc);

	bool operator==(const PublicAddress& rhs) const;
	bool operator!=(const PublicAddress& rhs) const;

	ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
	    READWRITE(a_pk);
	    READWRITE(pk_enc);
    }

private:
	std::vector<unsigned char> a_pk;
	std::string pk_enc;

    void createPublicAddress(const std::vector<unsigned char>& a_sk, const std::string sk_enc);

    const std::vector<unsigned char>& getPublicAddressSecret() const;

	const std::string getEncryptionPublicKey() const;
};

/******************************** Address ************************************/

class Address {

friend class PourTransaction;
friend class Coin;

public:
	Address();
	Address(PrivateAddress&);

	const PublicAddress& getPublicAddress() const;
	const PrivateAddress& getPrivateAddress() const;

	bool operator==(const Address& rhs) const;
	bool operator!=(const Address& rhs) const;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
	    READWRITE(addr_pk);
        READWRITE(addr_sk);
    }

private:
	PublicAddress addr_pk;
    PrivateAddress addr_sk;

    const std::vector<unsigned char>& getAddressSecret() const;
	const std::string getEncryptionSecretKey() const;
};

} /* namespace libzerocash */

#endif /* ADDRESS_H_ */
