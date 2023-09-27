
/*******************************************************************************
 *   Ledger Nano S - Secure firmware
 *   (c) 2022 Ledger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ********************************************************************************/

#ifdef HAVE_ECSCHNORR

#include "cx_rng.h"
#include "cx_ecfp.h"
#include "cx_eddsa.h"
#include "cx_hash.h"
#include "cx_utils.h"
#include "cx_ram.h"

#include <string.h>

// const char kr[] = {0xe5, 0xa8, 0xd1, 0xd5, 0x29, 0x97, 0x1c, 0x10, 0xca, 0x2a, 0xf3, 0x78, 0x44,
// 0x4f, 0xb5, 0x44, 0xa2, 0x11, 0x70, 0x78, 0x92, 0xc8, 0x89, 0x8f, 0x91, 0xdc, 0xb1, 0x71, 0x58,
// 0x4e, 0x3d, 0xb9};

/* BIP0340 tags for computing the tagged hashes */
const uint8_t BIP0340_challenge[]
    = {'B', 'I', 'P', '0', '3', '4', '0', '/', 'c', 'h', 'a', 'l', 'l', 'e', 'n', 'g', 'e'};
const uint8_t BIP0340_aux[]   = {'B', 'I', 'P', '0', '3', '4', '0', '/', 'a', 'u', 'x'};
const uint8_t BIP0340_nonce[] = {'B', 'I', 'P', '0', '3', '4', '0', '/', 'n', 'o', 'n', 'c', 'e'};

cx_err_t cx_ecschnorr_sign_no_throw(const cx_ecfp_private_key_t *pv_key,
                                    uint32_t                     mode,
                                    cx_md_t                      hashID,
                                    const uint8_t               *msg,
                                    size_t                       msg_len,
                                    uint8_t                     *sig,
                                    size_t                      *sig_len)
{
#define CX_MAX_TRIES 100
#define H            G_cx.sha256

    size_t       size;
    cx_ecpoint_t Q;
    cx_bn_t      bn_k, bn_d, bn_r, bn_s, bn_n;
    uint8_t      R[33];
    uint8_t      S[32];
    uint8_t      P[32];
    int          odd;
    uint8_t      tries;
    cx_err_t     error;
    int          diff;

    CX_CHECK(cx_ecdomain_parameters_length(pv_key->curve, &size));

    // Only secp256k1 is allowed when using CX_ECSCHNORR_BIP0340
    if (((mode & CX_MASK_EC) == CX_ECSCHNORR_BIP0340) && (pv_key->curve != CX_CURVE_SECP256K1)) {
        return CX_EC_INVALID_CURVE;
    }

    // WARN: only accept weierstrass 256 bits curve for now
    if (hashID != CX_SHA256 || size != 32 || !CX_CURVE_RANGE(pv_key->curve, WEIERSTRASS)
        || pv_key->d_len != size) {
        return CX_INVALID_PARAMETER;
    }

    // Schnorr BIP0340 signature is not DER encoded and is 64-byte long.
    if (((mode & CX_MASK_EC) != CX_ECSCHNORR_BIP0340) && (*sig_len < (6 + 2 * (size + 1)))) {
        return CX_INVALID_PARAMETER;
    }

    CX_CHECK(cx_bn_lock(size, 0));
    CX_CHECK(cx_bn_alloc(&bn_n, size));
    CX_CHECK(cx_ecdomain_parameter_bn(pv_key->curve, CX_CURVE_PARAM_Order, bn_n));
    CX_CHECK(cx_bn_alloc(&bn_k, size));
    CX_CHECK(cx_bn_alloc(&bn_d, size));
    CX_CHECK(cx_bn_alloc(&bn_r, size));
    CX_CHECK(cx_bn_alloc(&bn_s, size));
    CX_CHECK(cx_ecpoint_alloc(&Q, pv_key->curve));

    if ((mode & CX_MASK_EC) == CX_ECSCHNORR_BIP0340) {
        // Q = [d].G
        CX_CHECK(cx_ecdomain_generator_bn(pv_key->curve, &Q));
        CX_CHECK(cx_ecpoint_rnd_fixed_scalarmul(&Q, pv_key->d, size));
        // If Qy is even use d otherwise use n-d
        CX_CHECK(cx_bn_init(bn_d, pv_key->d, size));
        CX_CHECK(cx_ecpoint_export(&Q, NULL, 0, P, size));
        odd = P[size - 1] & 1;
        if (odd) {
            CX_CHECK(cx_bn_sub(bn_d, bn_n, bn_d));
        }
        // tag_hash = SHA256("BIP0340/aux")
        // SHA256(tag_hash || tag_hash || aux_rnd)
        cx_sha256_init_no_throw(&H);
        CX_CHECK(
            cx_hash_no_throw((cx_hash_t *) &H, CX_LAST, BIP0340_aux, sizeof(BIP0340_aux), R, size));
        cx_sha256_init_no_throw(&H);
        CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, R, size, NULL, 0));
        CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, R, size, NULL, 0));
        CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, CX_LAST | CX_NO_REINIT, sig, size, R, size));
        // t = d ^ SHA256(tag_hash || tag_hash || aux_rnd)
        CX_CHECK(cx_bn_init(bn_k, R, size));
        CX_CHECK(cx_bn_xor(bn_r, bn_d, bn_k));
        CX_CHECK(cx_bn_export(bn_r, sig, size));
        // tag_hash = SHA256("BIP0340/nonce")
        // SHA256(tag_hash || tag_hash || t || Qx || msg)
        cx_sha256_init_no_throw(&H);
        CX_CHECK(cx_hash_no_throw(
            (cx_hash_t *) &H, CX_LAST, BIP0340_nonce, sizeof(BIP0340_nonce), R, size));
        cx_sha256_init_no_throw(&H);
        CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, R, size, NULL, 0));
        CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, R, size, NULL, 0));
        CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, sig, size, NULL, 0));
        CX_CHECK(cx_ecpoint_export(&Q, P, size, NULL, 0));
        CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, P, size, NULL, 0));
        CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, CX_LAST | CX_NO_REINIT, msg, size, sig, size));
    }

    // generate random
    tries = 0;
RETRY:
    if (tries == CX_MAX_TRIES) {
        goto end;
    }

    switch (mode & CX_MASK_RND) {
        case CX_RND_PROVIDED:
            if (tries) {
                goto end;
            }
            CX_CHECK(cx_bn_init(bn_r, sig, size));
            CX_CHECK(cx_bn_reduce(bn_k, bn_r, bn_n));
            break;

        case CX_RND_TRNG:
            CX_CHECK(cx_bn_rng(bn_k, bn_n));
            break;

        default:
            error = CX_INVALID_PARAMETER;
            goto end;
    }
    if ((mode & CX_MASK_EC) == CX_ECSCHNORR_BIP0340) {
        CX_CHECK(cx_bn_cmp_u32(bn_k, 0, &diff));
        if (diff == 0) {
            error = CX_INVALID_PARAMETER;
            goto end;
        }
    }
    CX_CHECK(cx_bn_export(bn_k, sig, size));

    // sign
    tries++;
RETRY2:
    CX_CHECK(cx_ecdomain_generator_bn(pv_key->curve, &Q));
    CX_CHECK(cx_ecpoint_rnd_fixed_scalarmul(&Q, sig, size));

    switch (mode & CX_MASK_EC) {
        case CX_ECSCHNORR_ISO14888_XY:
        case CX_ECSCHNORR_ISO14888_X:
            // 1. Generate a random k from [1, ..., order-1]
            // 2. Q = G*k
            // 3. r = H(Q.x||Q.y||M)
            // 4. s = (k+r*pv_key.d)%n
            cx_sha256_init_no_throw(&H);
            CX_CHECK(cx_ecpoint_export(&Q, sig, size, NULL, 0));
            CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, sig, size, NULL, 0));
            if ((mode & CX_MASK_EC) == CX_ECSCHNORR_ISO14888_XY) {
                CX_CHECK(cx_ecpoint_export(&Q, NULL, 0, sig, size));
                CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, sig, size, NULL, 0));
            }
            CX_CHECK(cx_hash_no_throw(
                (cx_hash_t *) &H, CX_LAST | CX_NO_REINIT, msg, msg_len, R, sizeof(R)));

            CX_CHECK(cx_bn_init(bn_d, R, 32));
            CX_CHECK(cx_bn_reduce(bn_r, bn_d, bn_n));
            CX_CHECK(cx_bn_cmp_u32(bn_r, 0, &diff));
            if (diff == 0) {
                cx_bn_unlock();
                goto RETRY;
            }

            CX_CHECK(cx_bn_init(bn_d, pv_key->d, pv_key->d_len));
            CX_CHECK(cx_bn_mod_mul(bn_s, bn_d, bn_r, bn_n));
            CX_CHECK(cx_bn_mod_add(bn_s, bn_k, bn_s, bn_n));
            CX_CHECK(cx_bn_set_u32(bn_k, 0));
            CX_CHECK(cx_bn_mod_sub(bn_s, bn_s, bn_k, bn_n));

            CX_CHECK(cx_bn_cmp_u32(bn_s, 0, &diff));
            if (diff == 0) {
                goto RETRY;
            }
            CX_CHECK(cx_bn_export(bn_s, S, 32));
            break;

        case CX_ECSCHNORR_BSI03111:
            // 1. Q = G*k
            // 2. r = H((msg+xQ), and r%n != 0
            // 3. s = (k-r*pv_key.d)%n
            // r = H((msg+xQ), and r%n != 0
            cx_sha256_init_no_throw(&H);
            CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, msg, msg_len, NULL, 0));
            CX_CHECK(cx_ecpoint_export(&Q, sig, size, NULL, 0));
            CX_CHECK(cx_hash_no_throw(
                (cx_hash_t *) &H, CX_LAST | CX_NO_REINIT, sig, size, R, sizeof(R)));

            CX_CHECK(cx_bn_init(bn_d, R, CX_SHA256_SIZE));
            CX_CHECK(cx_bn_reduce(bn_r, bn_d, bn_n));
            CX_CHECK(cx_bn_cmp_u32(bn_r, 0, &diff));
            if (diff == 0) {
                goto RETRY;
            }

            // s = (k-r*pv_key.d)%n
            CX_CHECK(cx_bn_init(bn_d, pv_key->d, pv_key->d_len));
            CX_CHECK(cx_bn_mod_mul(bn_s, bn_d, bn_r, bn_n));
            CX_CHECK(cx_bn_mod_sub(bn_s, bn_k, bn_s, bn_n));

            CX_CHECK(cx_bn_cmp_u32(bn_s, 0, &diff));
            if (diff == 0) {
                goto RETRY;
            }
            CX_CHECK(cx_bn_export(bn_s, S, 32));
            break;

        case CX_ECSCHNORR_Z:
            // https://github.com/Zilliqa/Zilliqa/blob/master/src/libCrypto/Schnorr.cpp#L580
            // https://docs.zilliqa.com/whitepaper.pdf
            // 1. Generate a random k from [1, ..., order-1]
            // 2. Compute the commitment Q = kG, where  G is the base point
            // 3. Compute the challenge r = H(Q, kpub, m) [CME: mod n according to pdf/code, Q and
            // kpub compressed "02|03 x" according to code)
            // 4. If r = 0 mod(order), goto 1
            // 4. Compute s = k - r*kpriv mod(order)
            // 5. If s = 0 goto 1.
            // 5  Signature on m is (r, s)

            // Q
            cx_sha256_init_no_throw(&H);
            CX_CHECK(cx_ecpoint_export(&Q, NULL, 0, sig, size));
            odd = sig[size - 1] & 1;
            CX_CHECK(cx_ecpoint_export(&Q, sig + 1, size, NULL, 0));
            sig[0] = odd ? 0x03 : 0x02;
            CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, sig, 1 + size, NULL, 0));  // Q
            // kpub
            CX_CHECK(cx_ecdomain_generator_bn(pv_key->curve, &Q));
            CX_CHECK(cx_ecpoint_rnd_fixed_scalarmul(&Q, pv_key->d, pv_key->d_len));
            CX_CHECK(cx_ecpoint_export(&Q, NULL, 0, sig, size));
            odd = sig[size - 1] & 1;
            CX_CHECK(cx_ecpoint_export(&Q, sig + 1, size, NULL, 0));
            sig[0] = odd ? 0x03 : 0x02;
            CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, sig, 1 + size, NULL, 0));  // Q
            // m
            CX_CHECK(cx_hash_no_throw(
                (cx_hash_t *) &H, CX_LAST | CX_NO_REINIT, msg, msg_len, R, sizeof(R)));

            // Compute the challenge r = H(Q, kpub, m)
            //[CME: mod n according to pdf/code, Q and kpub compressed "02|03 x" according to code)
            CX_CHECK(cx_bn_init(bn_d, R, CX_SHA256_SIZE));
            CX_CHECK(cx_bn_reduce(bn_r, bn_d, bn_n));
            CX_CHECK(cx_bn_cmp_u32(bn_r, 0, &diff));
            if (diff == 0) {
                goto RETRY;
            }
            CX_CHECK(cx_bn_export(bn_r, R, 32));

            CX_CHECK(cx_bn_init(bn_d, pv_key->d, pv_key->d_len));
            CX_CHECK(cx_bn_mod_mul(bn_s, bn_d, bn_r, bn_n));
            CX_CHECK(cx_bn_mod_sub(bn_s, bn_k, bn_s, bn_n));
            CX_CHECK(cx_bn_cmp_u32(bn_s, 0, &diff));
            if (diff == 0) {
                goto RETRY;
            }
            CX_CHECK(cx_bn_export(bn_s, S, 32));
            break;

        case CX_ECSCHNORR_LIBSECP:
            // Inputs: 32-byte message m, 32-byte scalar key x (!=0), 32-byte scalar nonce k (!=0)
            // 1. Compute point R = k * G. Reject nonce if R's y coordinate is odd (or negate
            // nonce).
            // 2. Compute 32-byte r, the serialization of R's x coordinate.
            // 3. Compute scalar h = Hash(r || m). Reject nonce if h == 0 or h >= order.
            // 4. Compute scalar s = k - h * x.
            // 5. The signature is (r, s).
            // Q = G*k
            CX_CHECK(cx_ecpoint_export(&Q, NULL, 0, sig, size));
            odd = sig[size - 1] & 1;
            if (odd) {
                // if y is odd, k <- -k mod n = n-k,  and retry
                CX_CHECK(cx_bn_mod_sub(bn_k, bn_n, bn_k, bn_n));
                CX_CHECK(cx_bn_export(bn_k, sig, size));
                goto RETRY2;
            }
            // r = xQ
            CX_CHECK(cx_ecpoint_export(&Q, R, size, NULL, 0));
            CX_CHECK(cx_bn_init(bn_d, R, size));
            CX_CHECK(cx_bn_reduce(bn_r, bn_d, bn_n));
            CX_CHECK(cx_bn_cmp_u32(bn_r, 0, &diff));
            if (diff == 0) {
                goto RETRY;
            }
            // h = Hash(r || m).
            cx_sha256_init_no_throw(&H);
            CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, R, size, NULL, 0));
            CX_CHECK(cx_hash_no_throw(
                (cx_hash_t *) &H, CX_LAST | CX_NO_REINIT, msg, msg_len, sig, sizeof(S)));
            // Reject nonce if h == 0 or h >= order.
            CX_CHECK(cx_bn_init(bn_r, sig, 32));
            CX_CHECK(cx_bn_cmp_u32(bn_r, 0, &diff));
            if (diff == 0) {
                goto RETRY;
            }
            CX_CHECK(cx_bn_cmp(bn_r, bn_n, &diff));
            if (diff >= 0) {
                goto RETRY;
            }
            // s = k - h * x.
            CX_CHECK(cx_bn_init(bn_d, pv_key->d, pv_key->d_len));
            CX_CHECK(cx_bn_mod_mul(bn_s, bn_d, bn_r, bn_n));
            CX_CHECK(cx_bn_mod_sub(bn_s, bn_k, bn_s, bn_n));
            CX_CHECK(cx_bn_cmp_u32(bn_s, 0, &diff));
            if (diff == 0) {
                goto RETRY;
            }
            CX_CHECK(cx_bn_export(bn_s, S, 32));
            break;

            /* Schnorr signature with secp256k1 according to BIP0340
            ** https://github.com/bitcoin/bips/blob/master/bip-0340.mediawiki */

        case CX_ECSCHNORR_BIP0340:
            CX_CHECK(cx_ecpoint_export(&Q, NULL, 0, sig, size));
            odd = sig[size - 1] & 1;
            if (odd) {
                CX_CHECK(cx_bn_sub(bn_k, bn_n, bn_k));
                CX_CHECK(cx_bn_export(bn_k, sig, size));
            }
            // Only take the x-coordinate
            CX_CHECK(cx_ecpoint_export(&Q, R, size, NULL, 0));

            // tag_hash = SHA256("BIP0340_challenge")
            // e = SHA256(tag_hash || tag_hash || Rx || Px || msg)
            cx_sha256_init_no_throw(&H);
            CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H,
                                      CX_LAST,
                                      BIP0340_challenge,
                                      sizeof(BIP0340_challenge),
                                      sig,
                                      size));
            cx_sha256_init_no_throw(&H);
            CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, sig, size, NULL, 0));
            CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, sig, size, NULL, 0));
            CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, R, size, NULL, 0));
            CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, P, size, NULL, 0));
            CX_CHECK(cx_hash_no_throw(
                (cx_hash_t *) &H, CX_LAST | CX_NO_REINIT, msg, msg_len, sig, size));

            // e = e % n
            CX_CHECK(cx_bn_init(bn_s, sig, size));
            CX_CHECK(cx_bn_reduce(bn_r, bn_s, bn_n));

            // s = (k + e *d) % n
            CX_CHECK(cx_bn_mod_mul(bn_s, bn_d, bn_r, bn_n));
            CX_CHECK(cx_bn_mod_add(bn_s, bn_k, bn_s, bn_n));
            CX_CHECK(cx_bn_set_u32(bn_k, 0));
            CX_CHECK(cx_bn_mod_sub(bn_s, bn_s, bn_k, bn_n));

            CX_CHECK(cx_bn_export(bn_s, S, size));
            break;

        default:
            error = CX_INVALID_PARAMETER;
            goto end;
    }

end:
    cx_hash_destroy((cx_hash_t *) &H);
    cx_bn_unlock();
    if (error == CX_OK) {
        if ((mode & CX_MASK_EC) == CX_ECSCHNORR_BIP0340) {
            *sig_len = 64;
            memcpy(sig, R, 32);
            memcpy(sig + 32, S, 32);
        }
        else {
            // encoding
            *sig_len = cx_ecfp_encode_sig_der(sig, *sig_len, R, size, S, size);
        }
    }
    return error;
}

bool cx_ecschnorr_verify(const cx_ecfp_public_key_t *pu_key,
                         uint32_t                    mode,
                         cx_md_t                     hashID,
                         const uint8_t              *msg,
                         size_t                      msg_len,
                         const uint8_t              *sig,
                         size_t                      sig_len)
{
    size_t         size;
    const uint8_t *r, *s;
    size_t         r_len, s_len;
    cx_ecpoint_t   R, P, Q;
    uint8_t        x[33];
    bool           odd;
    volatile int   verified;
    cx_err_t       error;
    int            diff;
    bool           is_on_curve = false;

    cx_bn_t bn_d, bn_r, bn_s, bn_n;
    cx_bn_t bn_x, bn_y, bn_p;

    CX_CHECK(cx_ecdomain_parameters_length(pu_key->curve, &size));

    // ECSCHNORR BIP0340 is only defined for secp256k1
    if (((mode & CX_MASK_EC) == CX_ECSCHNORR_BIP0340) && (pu_key->curve != CX_CURVE_SECP256K1)) {
        error    = CX_EC_INVALID_CURVE;
        verified = false;
        goto end;
    }

    if (!CX_CURVE_RANGE(pu_key->curve, WEIERSTRASS) || hashID != CX_SHA256 || size != 32
        || pu_key->W_len != 1 + 2 * size) {
        return false;
    }

    if ((mode & CX_MASK_EC) != CX_ECSCHNORR_BIP0340) {
        if (!cx_ecfp_decode_sig_der(sig, sig_len, size, &r, &r_len, &s, &s_len)) {
            return false;
        }
    }

    CX_CHECK(cx_bn_lock(size, 0));
    verified = false;

    CX_CHECK(cx_bn_alloc(&bn_n, size));
    CX_CHECK(cx_ecdomain_parameter_bn(pu_key->curve, CX_CURVE_PARAM_Order, bn_n));
    if ((mode & CX_MASK_EC) == CX_ECSCHNORR_BIP0340) {
        CX_CHECK(cx_bn_alloc_init(&bn_r, size, sig, size));
        CX_CHECK(cx_bn_alloc_init(&bn_s, size, sig + size, size));
    }
    else {
        CX_CHECK(cx_bn_alloc_init(&bn_r, size, r, r_len));
        CX_CHECK(cx_bn_alloc_init(&bn_s, size, s, s_len));
    }

    CX_CHECK(cx_bn_alloc(&bn_d, size));

    CX_CHECK(cx_bn_cmp_u32(bn_r, 0, &diff));
    if (diff == 0) {
        goto end;
    }
    CX_CHECK(cx_bn_cmp_u32(bn_s, 0, &diff));
    if (diff == 0) {
        goto end;
    }

    CX_CHECK(cx_ecpoint_alloc(&P, pu_key->curve));
    CX_CHECK(cx_ecpoint_alloc(&Q, pu_key->curve));
    CX_CHECK(cx_ecpoint_alloc(&R, pu_key->curve));
    CX_CHECK(cx_ecdomain_generator_bn(pu_key->curve, &P));
    CX_CHECK(cx_ecpoint_init(&Q, &pu_key->W[1], size, &pu_key->W[1 + size], size));

    switch (mode & CX_MASK_EC) {
        case CX_ECSCHNORR_ISO14888_XY:
        case CX_ECSCHNORR_ISO14888_X:
            // 1. check...
            // 2. Q = [s]G - [r]W
            //   If Q = 0, output Error and terminate.
            // 3. v = H(Qx||Qy||M).
            // 4. Output True if v = r, and False otherwise.

            // 1.
            CX_CHECK(cx_bn_cmp_u32(bn_r, 0, &diff));
            if (diff == 0) {
                break;
            }
            CX_CHECK(cx_bn_cmp(bn_n, bn_s, &diff));
            if (diff <= 0) {
                break;
            }
            CX_CHECK(cx_bn_cmp_u32(bn_s, 0, &diff));
            if (diff == 0) {
                break;
            }
            CX_CHECK(cx_bn_cmp(bn_n, bn_s, &diff));
            if (diff <= 0) {
                break;
            }

            // 2.
            // sG - rW
            CX_CHECK(cx_ecpoint_neg(&Q));
            CX_CHECK(cx_ecpoint_double_scalarmul_bn(&R, &P, &Q, bn_s, bn_r));
            // 3.
            cx_sha256_init_no_throw(&H);
            CX_CHECK(cx_ecpoint_export(&R, x, size, NULL, 0));
            CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, x, size, NULL, 0));
            if ((mode & CX_MASK_EC) == CX_ECSCHNORR_ISO14888_XY) {
                CX_CHECK(cx_ecpoint_export(&R, NULL, 0, x, size));
                CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, x, size, NULL, 0));
            }
            CX_CHECK(cx_hash_no_throw(
                (cx_hash_t *) &H, CX_LAST | CX_NO_REINIT, msg, msg_len, x, sizeof(x)));
            // 4.
            CX_CHECK(cx_bn_init(bn_s, x, CX_SHA256_SIZE));
            CX_CHECK(cx_bn_cmp(bn_r, bn_s, &diff));
            if (diff == 0) {
                verified = true;
            }
            break;

        case CX_ECSCHNORR_BSI03111:
            // 1. Verify that r in {0, . . . , 2**t - 1} and s in {1, 2, . . . , n - 1}.
            //   If the check fails, output False and terminate.
            // 2. Q = [s]G + [r]W
            //   If Q = 0, output Error and terminate.
            // 3. v = H(M||Qx)
            // 4. Output True if v = r, and False otherwise.

            // 1.
            CX_CHECK(cx_bn_cmp_u32(bn_r, 0, &diff));
            if (diff == 0) {
                break;
            }
            CX_CHECK(cx_bn_cmp_u32(bn_s, 0, &diff));
            if (diff == 0) {
                break;
            }
            CX_CHECK(cx_bn_cmp(bn_n, bn_s, &diff));
            if (diff <= 0) {
                break;
            }

            // 2.
            CX_CHECK(cx_ecpoint_double_scalarmul_bn(&R, &P, &Q, bn_s, bn_r));
            // 3.
            cx_sha256_init_no_throw(&H);
            CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, msg, msg_len, NULL, 0));
            CX_CHECK(cx_ecpoint_export(&R, x, size, NULL, 0));
            CX_CHECK(
                cx_hash_no_throw((cx_hash_t *) &H, CX_LAST | CX_NO_REINIT, x, size, x, sizeof(x)));
            // 4.
            CX_CHECK(cx_bn_init(bn_s, x, CX_SHA256_SIZE));
            CX_CHECK(cx_bn_cmp(bn_r, bn_s, &diff));
            if (diff == 0) {
                verified = true;
            }
            break;

        case CX_ECSCHNORR_Z:
            // The algorithm to check the signature (r, s) on a message m using a public
            // key kpub is as follows
            // 1. Check if r,s is in [1, ..., order-1]
            // 2. Compute Q = sG + r*kpub
            // 3. If Q = O (the neutral point), return 0;
            // 4. r' = H(Q, kpub, m) [CME: mod n and Q and kpub compressed "02|03 x" according to
            // pdf/code]
            // 5. return r' == r

            // r,s is in [1, ..., order-1]
            CX_CHECK(cx_bn_cmp_u32(bn_r, 0, &diff));
            if (diff == 0) {
                break;
            }
            CX_CHECK(cx_bn_cmp(bn_r, bn_n, &diff));
            if (diff >= 0) {
                break;
            }
            CX_CHECK(cx_bn_cmp_u32(bn_s, 0, &diff));
            if (diff == 0) {
                break;
            }
            CX_CHECK(cx_bn_cmp(bn_s, bn_n, &diff));
            if (diff >= 0) {
                break;
            }

            //  Q = sG + r*kpub
            CX_CHECK(cx_ecpoint_double_scalarmul_bn(&R, &P, &Q, bn_s, bn_r));
            // r' = H(Q, kpub, m)
            cx_sha256_init_no_throw(&H);
            // Q
            CX_CHECK(cx_ecpoint_export(&R, NULL, 0, x, size));
            odd = x[size - 1] & 1;
            CX_CHECK(cx_ecpoint_export(&R, x + 1, size, NULL, 0));
            x[0] = odd ? 0x03 : 0x02;
            CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, x, 1 + size, NULL, 0));  // Q
            // kpub
            memmove(x + 1, &pu_key->W[1], size);
            x[0] = (pu_key->W[1 + 2 * size - 1] & 1) ? 0x03 : 0x02;
            CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, x, 1 + size, NULL, 0));  // kpub
            // m
            CX_CHECK(cx_hash_no_throw(
                (cx_hash_t *) &H, CX_LAST | CX_NO_REINIT, msg, msg_len, x, sizeof(x)));  // m

            CX_CHECK(cx_bn_init(bn_d, x, CX_SHA256_SIZE));
            CX_CHECK(cx_bn_reduce(bn_s, bn_d, bn_n));
            CX_CHECK(cx_bn_cmp(bn_r, bn_s, &diff));
            if (diff == 0) {
                verified = true;
            }
            break;

        case CX_ECSCHNORR_LIBSECP:
            // Verification:
            // Inputs: 32-byte message m, public key point Q, signature: (32-byte r, scalar s)
            // 1. Signature is invalid if s >= order.
            // 2. Signature is invalid if r >= p.
            // 3. Compute scalar h = Hash(r || m). Signature is invalid if h == 0 or h >= order.
            // 4. Option 1 (faster for single verification):
            // 5. Compute point R = h * Q + s * G. Signature is invalid if R is infinity or R's y
            // coordinate is odd.
            // 6. Signature is valid if the serialization of R's x coordinate equals r.
            // s < order and r < field.

            // 1. 2.
            CX_CHECK(cx_bn_cmp_u32(bn_r, 0, &diff));
            if (diff == 0) {
                verified = false;
                break;
            }
            CX_CHECK(cx_bn_cmp_u32(bn_s, 0, &diff));
            if (diff == 0) {
                verified = false;
                break;
            }
            CX_CHECK(cx_bn_cmp(bn_n, bn_s, &diff));
            if (diff <= 0) {
                verified = false;
                break;
            }
            // h = Hash(r||m), and h!=0, and h<order
            cx_sha256_init_no_throw(&H);
            CX_CHECK(cx_bn_export(bn_r, x, size));
            CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, x, size, NULL, 0));
            CX_CHECK(cx_hash_no_throw(
                (cx_hash_t *) &H, CX_LAST | CX_NO_REINIT, msg, msg_len, x, sizeof(x)));
            CX_CHECK(cx_bn_init(bn_s, x, CX_SHA256_SIZE));
            CX_CHECK(cx_bn_cmp_u32(bn_s, 0, &diff));
            if (diff == 0) {
                break;
            }
            CX_CHECK(cx_bn_cmp(bn_s, bn_n, &diff));
            if (diff >= 0) {
                break;
            }
            // R = h*W + s*G, and Ry is NOT odd, and Rx=r
            CX_CHECK(cx_bn_init(bn_d, s, s_len));
            CX_CHECK(cx_ecpoint_double_scalarmul_bn(&R, &P, &Q, bn_d, bn_s));
            CX_CHECK(cx_ecpoint_export_bn(&R, &bn_s, &bn_d));
            CX_CHECK(cx_bn_is_odd(bn_d, &odd));
            if (odd) {
                break;
            }
            CX_CHECK(cx_bn_cmp(bn_r, bn_s, &diff));
            if (diff == 0) {
                verified = true;
            }
            break;

            /* Schnorr signature verification with secp256k1 according to BIP0340
            ** https://github.com/bitcoin/bips/blob/master/bip-0340.mediawiki */

        case CX_ECSCHNORR_BIP0340:
            // Q = lift_x(pu_key)
            CX_CHECK(cx_bn_alloc(&bn_x, size));
            CX_CHECK(cx_bn_alloc(&bn_y, size));
            CX_CHECK(cx_bn_alloc(&bn_p, size));
            CX_CHECK(cx_ecdomain_parameter_bn(pu_key->curve, CX_CURVE_PARAM_Field, bn_p));
            CX_CHECK(cx_ecpoint_export_bn(&Q, &bn_x, &bn_y));
            // c = x^3 + 7 mod p
            CX_CHECK(cx_bn_set_u32(bn_s, 3));
            CX_CHECK(cx_bn_mod_pow_bn(bn_r, bn_x, bn_s, bn_p));
            CX_CHECK(cx_bn_set_u32(bn_s, 7));
            CX_CHECK(cx_bn_mod_add(bn_r, bn_r, bn_s, bn_p));
            // y = c^(p+1)/4 mod p
            CX_CHECK(cx_bn_copy(bn_s, bn_p));
            CX_CHECK(cx_bn_set_u32(bn_d, 1));
            CX_CHECK(cx_bn_add(bn_s, bn_s, bn_d));
            CX_CHECK(cx_bn_shr(bn_s, 2));
            CX_CHECK(cx_bn_mod_pow_bn(bn_y, bn_r, bn_s, bn_p));

            CX_CHECK(cx_bn_is_odd(bn_y, &odd));
            // If y is even, Qy = y otherwise Qy = p - y
            if (odd) {
                CX_CHECK(cx_bn_sub(bn_y, bn_p, bn_y));
            }
            CX_CHECK(cx_ecpoint_init_bn(&Q, bn_x, bn_y));
            CX_CHECK(cx_ecpoint_is_on_curve(&Q, &is_on_curve));
            if (!is_on_curve) {
                error = CX_EC_INVALID_POINT;
                goto end;
            }
            // tag_hash = SHA256("BIP0340/challenge")
            // e = SHA256(tag_hash || tag_hash || r || pu_key || msg)
            cx_sha256_init_no_throw(&H);
            CX_CHECK(cx_hash_no_throw(
                (cx_hash_t *) &H, CX_LAST, BIP0340_challenge, sizeof(BIP0340_challenge), x, size));
            cx_sha256_init_no_throw(&H);
            CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, x, size, NULL, 0));
            CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, x, size, NULL, 0));
            CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, sig, size, NULL, 0));
            CX_CHECK(cx_hash_no_throw((cx_hash_t *) &H, 0, &pu_key->W[1], size, NULL, 0));
            CX_CHECK(
                cx_hash_no_throw((cx_hash_t *) &H, CX_LAST | CX_NO_REINIT, msg, msg_len, x, size));

            // e = e % n
            CX_CHECK(cx_bn_init(bn_x, x, size));
            CX_CHECK(cx_bn_reduce(bn_r, bn_x, bn_n));
            // n - e
            CX_CHECK(cx_bn_sub(bn_r, bn_n, bn_r));
            // s
            CX_CHECK(cx_bn_init(bn_s, sig + size, size));
            // [s].P + [n-e].Q where P is the generator of the curve
            // and Q is the point representing the public key.
            CX_CHECK(cx_ecpoint_double_scalarmul_bn(&R, &P, &Q, bn_s, bn_r));
            CX_CHECK(cx_ecpoint_export_bn(&R, &bn_x, &bn_y));
            CX_CHECK(cx_bn_is_odd(bn_y, &odd));
            // r
            CX_CHECK(cx_bn_init(bn_r, sig, size));
            CX_CHECK(cx_bn_cmp(bn_x, bn_r, &diff));

            if (odd || (diff != 0)) {
                error = CX_INVALID_PARAMETER;
                goto end;
            }
            verified = true;
            break;

        default:
            error = CX_INVALID_PARAMETER;
            goto end;
    }

end:
    cx_bn_unlock();
    return error == CX_OK && verified;
}

#endif  // HAVE_ECSCHNORR
