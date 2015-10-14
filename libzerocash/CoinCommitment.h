/** @file
 *****************************************************************************

 Declaration of interfaces for the class CoinCommitment.

 *****************************************************************************
 * @author     This file is part of libzerocash, developed by the Zerocash
 *             project and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef COINCOMMITMENT_H_
#define COINCOMMITMENT_H_

#include <vector>

#include "serialize.h"

namespace libzerocash {

/****************************** Coin commitment ******************************/

class CoinCommitment {

friend class PourTransaction;

public:
	CoinCommitment();

	CoinCommitment(const std::vector<unsigned char>& val,
                   const std::vector<unsigned char>& k);

	void constructCommitment(const std::vector<unsigned char>& val,
                             const std::vector<unsigned char>& k);

    const std::vector<unsigned char>& getCommitmentValue() const;

	bool operator==(const CoinCommitment& rhs) const;
	bool operator!=(const CoinCommitment& rhs) const;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(commitmentValue);
    }

private:
    std::vector<unsigned char> commitmentValue;
};

} /* namespace libzerocash */

#endif /* COINCOMMITMENT_H_ */
