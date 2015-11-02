/** @file
 *****************************************************************************

 Declaration of interfaces for the class ZerocashParams.

 *****************************************************************************
 * @author     This file is part of libzerocash, developed by the Zerocash
 *             project and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef PARAMS_H_
#define PARAMS_H_

#include "Zerocash.h"
#include "libsnark/common/default_types/r1cs_ppzksnark_pp.hpp"
#include "libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp"
#include "zerocash_pour_ppzksnark/zerocash_pour_ppzksnark.hpp"

namespace libzerocash {

class ZerocashParams {

public:
    typedef default_r1cs_ppzksnark_pp zerocash_pp;

    ZerocashParams(
        const unsigned int tree_depth,
        zerocash_pour_keypair<ZerocashParams::zerocash_pp> *keypair
    );

    const zerocash_pour_proving_key<zerocash_pp>& getProvingKey();
    const zerocash_pour_verification_key<zerocash_pp>& getVerificationKey();
    ~ZerocashParams();

    static const size_t numPourInputs = 2;
    static const size_t numPourOutputs = 2;
private:
    int treeDepth;
    zerocash_pour_keypair<zerocash_pp>* kp_v1;
};

} /* namespace libzerocash */

#endif /* PARAMS_H_ */
