/** @file
 *****************************************************************************

 Implementation of interfaces for the class ZerocashParams.

 See ZerocashParams.h .

 *****************************************************************************
 * @author     This file is part of libzerocash, developed by the Zerocash
 *             project and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#include <fstream>

#include "Zerocash.h"
#include "ZerocashParams.h"

namespace libzerocash {

ZerocashParams::ZerocashParams(
    const unsigned int tree_depth,
    zerocash_pour_keypair<ZerocashParams::zerocash_pp> *keypair
) :
    treeDepth(tree_depth)
{
    kp_v1 = new zerocash_pour_keypair<zerocash_pp>(*keypair);
}

/*
void ZerocashParams::initV1Params()
{
    ZerocashParams::zerocash_pp::init_public_params();
    kp_v1 = new zerocash_pour_keypair<ZerocashParams::zerocash_pp>(zerocash_pour_ppzksnark_generator<ZerocashParams::zerocash_pp>(this->numPourInputs,
                                                                                                                                  this->numPourOutputs,
                                                                                                                                  this->treeDepth));

	params_pk_v1 = &kp_v1->pk;
	params_vk_v1 = &kp_v1->vk;
}
*/

ZerocashParams::~ZerocashParams()
{
    delete kp_v1;
}

const zerocash_pour_proving_key<ZerocashParams::zerocash_pp>& ZerocashParams::getProvingKey()
{
    return kp_v1->pk;
}

const zerocash_pour_verification_key<ZerocashParams::zerocash_pp>& ZerocashParams::getVerificationKey()
{
    return kp_v1->vk;
}

} /* namespace libzerocash */
