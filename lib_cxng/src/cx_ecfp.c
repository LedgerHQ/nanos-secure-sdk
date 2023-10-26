
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

#ifdef HAVE_ECC

#include "cx_ecfp.h"
#include "libcxng.h"
#include "cx_eddsa.h"
#include "cx_utils.h"
#include "cx_ram.h"

#include <string.h>

/* ========================================================================= */
/* ========================================================================= */
/* ===                                APIs                               === */
/* ========================================================================= */
/* ========================================================================= */

/* ----------------------------------------------------------------------- */
/*                                                                         */
/* ----------------------------------------------------------------------- */
cx_err_t cx_ecfp_init_private_key_no_throw(cx_curve_t             curve,
                                           const uint8_t         *raw_key,
                                           size_t                 key_len,
                                           cx_ecfp_private_key_t *key)
{
    size_t   domain_length;
    cx_err_t error;

    CX_CHECK(cx_ecdomain_parameters_length(curve, &domain_length));

    if (!(((raw_key == NULL) && (key_len == 0)) || ((raw_key) && (key_len == domain_length)))) {
        error = CX_INVALID_PARAMETER;
        goto end;
    }

    if (raw_key) {
        key->d_len = key_len;
        memmove(key->d, raw_key, key_len);
    }

    key->curve = curve;

end:
    return error;
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/* ----------------------------------------------------------------------- */
cx_err_t cx_ecfp_init_public_key_no_throw(cx_curve_t            curve,
                                          const uint8_t        *rawkey,
                                          size_t                key_len,
                                          cx_ecfp_public_key_t *key)
{
    size_t   expected_key_len;
    size_t   expected_compressed_key_len;
    size_t   size;
    cx_err_t error;

    error = cx_ecdomain_parameters_length(curve, &size);
    if (error) {
        return error;
    }

    // SEC: complex assert param dispatched in code....

    memset(key, 0, sizeof(cx_ecfp_public_key_t));

    if (rawkey) {
        if (key_len) {
            expected_key_len            = 0;
            expected_compressed_key_len = 0;

// check key length vs curve
#ifdef HAVE_ECC_WEIERSTRASS
            if (CX_CURVE_RANGE(curve, WEIERSTRASS)) {
                expected_key_len = 1 + (size) *2;
            }
#endif

#ifdef HAVE_ECC_TWISTED_EDWARDS
            if (CX_CURVE_RANGE(curve, TWISTED_EDWARDS)) {
                expected_key_len            = 1 + (size) *2;
                expected_compressed_key_len = 1 + (size);
            }
#endif

#ifdef HAVE_ECC_MONTGOMERY
            if (CX_CURVE_RANGE(curve, MONTGOMERY)) {
                expected_compressed_key_len = 1 + (size);
            }
#endif

            // check key format
            if ((key_len == expected_key_len) && (rawkey[0] == 0x04)) {
                goto OK;
            }
            if ((key_len == expected_compressed_key_len) && (rawkey[0] == 0x02)) {
                goto OK;
            }
        }
        return INVALID_PARAMETER;
    }
    else {
        key_len = 0;
    }

OK:
    // init key
    key->curve = curve;
    key->W_len = key_len;
    memmove(key->W, rawkey, key_len);

    return CX_OK;
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/* ----------------------------------------------------------------------- */
cx_err_t cx_ecfp_generate_pair_no_throw(cx_curve_t             curve,
                                        cx_ecfp_public_key_t  *public_key,
                                        cx_ecfp_private_key_t *private_key,
                                        bool                   keep_private)
{
    return cx_ecfp_generate_pair2_no_throw(curve, public_key, private_key, keep_private, CX_SHA512);
}

cx_err_t cx_ecfp_generate_pair2_no_throw(cx_curve_t             curve,
                                         cx_ecfp_public_key_t  *public_key,
                                         cx_ecfp_private_key_t *private_key,
                                         bool                   keep_private,
                                         cx_md_t                hashID)
{
    // domain used
    size_t       size;
    cx_bn_t      r, a, n;
    cx_ecpoint_t W;
    cx_err_t     error;

#ifndef HAVE_ECC_TWISTED_EDWARDS
    (void) hashID;
#endif

    CX_CHECK(cx_ecdomain_parameters_length(curve, &size));

    // SEC: complex assert param dispatched in code....

    CX_CHECK(cx_bn_lock(size, 0));
    CX_CHECK(cx_bn_alloc(&n, size));
    CX_CHECK(cx_ecdomain_parameter_bn(curve, CX_CURVE_PARAM_Order, n));
    CX_CHECK(cx_bn_alloc(&r, size));

    // generate private key
    if (keep_private) {
        // keep => check private key
        if ((private_key->curve != curve) || (private_key->d_len != size)) {
            error = CX_INVALID_PARAMETER;
            goto end;
        }
        CX_CHECK(cx_bn_init(r, private_key->d, private_key->d_len));
    }
    else {
        CX_CHECK(cx_bn_alloc(&a, 2 * size));
        CX_CHECK(cx_bn_rand(a));
        CX_CHECK(cx_bn_reduce(r, a, n));
        CX_CHECK(cx_bn_destroy(&a));
        CX_CHECK(cx_bn_export(r, private_key->d, size));
        private_key->curve = curve;
        private_key->d_len = size;

#ifdef HAVE_ECC_MONTGOMERY
        if (CX_CURVE_RANGE(curve, MONTGOMERY)) {
            if (curve == CX_CURVE_Curve25519) {
                private_key->d[size - 1] &= 0xF8;
                private_key->d[0] = (private_key->d[0] & 0x7F) | 0x40;
            }
            else /* CX_CURVE_Curve448*/ {
                private_key->d[size - 1] &= 0xFC;
                private_key->d[0] |= 0x80;
            }
        }
#endif
    }

    // generate public key
#ifdef HAVE_ECC_WEIERSTRASS
    if (CX_CURVE_RANGE(curve, WEIERSTRASS)) {
        CX_CHECK(cx_ecpoint_alloc(&W, private_key->curve));
        CX_CHECK(cx_ecdomain_generator_bn(curve, &W));
        // 'cx_ecpoint_rnd_fixed_scalarmul' doesn't support BLS12-381 so far
        // use cx_ecpoint_rnd_scalarmul for now
        if (CX_CURVE_BLS12_381_G1 == private_key->curve) {
            CX_CHECK(cx_ecpoint_rnd_scalarmul(&W, private_key->d, private_key->d_len));
        }
        else {
            CX_CHECK(cx_ecpoint_rnd_fixed_scalarmul(&W, private_key->d, private_key->d_len));
        }
        public_key->curve = curve;
        public_key->W_len = 1 + 2 * size;
        public_key->W[0]  = 0x04;
        CX_CHECK(cx_ecpoint_export(&W, &public_key->W[1], size, &public_key->W[1 + size], size));
        goto end;
    }
#endif

#ifdef HAVE_ECC_TWISTED_EDWARDS
    if (CX_CURVE_RANGE(curve, TWISTED_EDWARDS)) {
        uint8_t scal[114];
        CX_CHECK(cx_eddsa_get_public_key_internal(
            private_key, hashID, public_key, NULL, 0, NULL, 0, scal));
        goto end;
    }
#endif

#ifdef HAVE_ECC_MONTGOMERY
    if (CX_CURVE_RANGE(curve, MONTGOMERY)) {
        CX_CHECK(cx_ecpoint_alloc(&W, private_key->curve));
        CX_CHECK(cx_ecdomain_generator_bn(curve, &W));
        CX_CHECK(cx_ecpoint_rnd_scalarmul(&W, private_key->d, private_key->d_len));
        public_key->curve = curve;
        public_key->W_len = 1 + 2 * size;
        public_key->W[0]  = 0x04;
        CX_CHECK(cx_ecpoint_export(&W, &public_key->W[1], size, &public_key->W[1 + size], size));
        goto end;
    }
#endif

    error = CX_EC_INVALID_POINT;

end:
    cx_bn_unlock();
    return error;
}

cx_err_t cx_ecfp_add_point_no_throw(cx_curve_t           curve,
                                    unsigned char       *R,
                                    const unsigned char *P,
                                    const unsigned char *Q)
{
    size_t       size;
    cx_ecpoint_t ecR, ecP, ecQ;
    cx_err_t     error;

    CX_CHECK(cx_ecdomain_parameters_length(curve, &size));
    CX_CHECK(cx_bn_lock(size, 0));

    CX_CHECK(cx_ecpoint_alloc(&ecP, curve));
    CX_CHECK(cx_ecpoint_alloc(&ecQ, curve));
    CX_CHECK(cx_ecpoint_alloc(&ecR, curve));
    CX_CHECK(cx_ecpoint_init(&ecP, P + 1, size, P + 1 + size, size));
    CX_CHECK(cx_ecpoint_init(&ecQ, Q + 1, size, Q + 1 + size, size));
    CX_CHECK(cx_ecpoint_add(&ecR, &ecP, &ecQ));
    R[0] = 0x04;
    CX_CHECK(cx_ecpoint_export(&ecR, &R[1], size, &R[1 + size], size));

end:
    cx_bn_unlock();
    return error;
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/* ----------------------------------------------------------------------- */
cx_err_t cx_ecfp_scalar_mult_no_throw(cx_curve_t curve, uint8_t *P, const uint8_t *k, size_t k_len)
{
    size_t       size;
    cx_ecpoint_t ecP;
    cx_err_t     error;

    CX_CHECK(cx_ecdomain_parameters_length(curve, &size));
    CX_CHECK(cx_bn_lock(size, 0));

    CX_CHECK(cx_ecpoint_alloc(&ecP, curve));
    CX_CHECK(cx_ecpoint_init(&ecP, P + 1, size, P + 1 + size, size));
    CX_CHECK(cx_ecpoint_rnd_scalarmul(&ecP, k, k_len));
    P[0] = 0x04;
    CX_CHECK(cx_ecpoint_export(&ecP, &P[1], size, &P[1 + size], size));

end:
    cx_bn_unlock();
    return error;
}

#ifdef HAVE_ECC_TWISTED_EDWARDS

cx_err_t cx_edwards_compress_point_no_throw(cx_curve_t curve, uint8_t *P, size_t P_len)
{
    cx_ecpoint_t P_ec;
    cx_err_t     error;
    uint32_t     sign;
    size_t       size;

    UNUSED(P_len);
    CX_CHECK(cx_ecdomain_parameters_length(curve, &size));
    CX_CHECK(cx_bn_lock(size, 0));
    CX_CHECK(cx_ecpoint_alloc(&P_ec, curve));
    CX_CHECK(cx_ecpoint_init(&P_ec, P + 1, size, P + 1 + size, size));
    CX_CHECK(cx_ecpoint_compress(&P_ec, P + 1, size, &sign));
    cx_encode_coord(P + 1, size, sign);
    memmove(P + 1 + size, P + 1, size);
    P[0] = 0x02;

end:
    cx_bn_unlock();
    return error;
}

cx_err_t cx_edwards_decompress_point_no_throw(cx_curve_t curve, uint8_t *P, size_t P_len)
{
    cx_ecpoint_t P_ec;
    cx_err_t     error;
    uint32_t     sign;
    size_t       size;

    UNUSED(P_len);
    CX_CHECK(cx_ecdomain_parameters_length(curve, &size));
    CX_CHECK(cx_bn_lock(size, 0));
    sign = cx_decode_coord(P + 1, size);
    CX_CHECK(cx_ecpoint_alloc(&P_ec, curve));
    memmove(P + 1 + size, P + 1, size);
    CX_CHECK(cx_ecpoint_decompress(&P_ec, P + 1, size, sign));
    CX_CHECK(cx_ecpoint_export(&P_ec, P + 1, size, P + 1 + size, size));
    P[0] = 0x04;

end:
    cx_bn_unlock();
    return error;
}

#endif  // HAVE_ECC_TWISTED_EDWARDS

static size_t asn1_get_encoded_length_size(size_t len)
{
    if (len < 0x80) {  // ..
        return 1;
    }
    if (len < 0x100) {
        // 81 ..
        return 2;
    }
    if (len < 0x10000) {
        // 82 .. ..
        return 3;
    }
    return 0;
}

// return the length of an asn1 integer, aka '02' L V
static size_t asn1_get_encoded_integer_size(uint8_t const *val, size_t len)
{
    size_t l0;

    while (len && (*val == 0)) {
        val++;
        len--;
    }

    if (len == 0) {
        len = 1;
    }
    else if (*val & 0x80u) {
        len++;
    }

    l0 = asn1_get_encoded_length_size(len);
    if (l0 == 0) {
        return 0;
    }
    return 1 + l0 + len;
}

static int asn1_insert_tag(uint8_t **p, const uint8_t *end, unsigned int tag)
{
    if ((end - *p) < 1) {
        return 0;
    }
    **p = tag;
    (*p)++;
    return 1;
}

static int asn1_insert_len(uint8_t **p, const uint8_t *end, size_t len)
{
    if (len < 0x80) {
        if ((end - *p) < 1) {
            return 0;
        }
        (*p)[0] = len & 0xFF;
        (*p) += 1;
        return 1;
    }

    if (len < 0x100) {
        if ((end - *p) < 2) {
            return 0;
        }
        (*p)[0] = 0x81u;
        (*p)[1] = len & 0xFF;
        (*p) += 2;
        return 2;
    }

    if (len < 0x10000) {
        if ((end - *p) < 3) {
            return 0;
        }
        (*p)[0] = 0x82u;
        (*p)[1] = (len >> 8) & 0xFF;
        (*p)[2] = len & 0xFF;
        (*p) += 3;
        return 3;
    }

    return 0;
}

static int asn1_insert_integer(uint8_t **p, const uint8_t *end, const uint8_t *val, size_t len)
{
    while (len && (*val == 0)) {
        val++;
        len--;
    }
    if (!asn1_insert_tag(p, end, 0x02)) {
        return 0;
    }

    // special case for 0
    if (len == 0) {
        if ((end - *p) < 2) {
            return 0;
        }
        (*p)[0] = 0x01u;
        (*p)[1] = 0x00u;
        (*p) += 2;
        return 1;
    }

    // cont with len != 0, so val != 0
    if (!asn1_insert_len(p, end, (*val & 0x80) ? len + 1 : len)) {
        return 0;
    }

    if (*val & 0x80) {
        if ((end - *p) < 1) {
            return 0;
        }
        **p = 0;
        (*p)++;
    }
    if ((end - *p) < (int) len) {
        return 0;
    }
    memmove(*p, val, len);
    (*p) += len;
    return 1;
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/* ----------------------------------------------------------------------- */
size_t cx_ecfp_encode_sig_der(uint8_t       *sig,
                              size_t         sig_len,
                              const uint8_t *r,
                              size_t         r_len,
                              const uint8_t *s,
                              size_t         s_len)
{
    size_t         l0, len;
    const uint8_t *sig_end = sig + sig_len;

    len = 0;

    l0 = asn1_get_encoded_integer_size(r, r_len);
    if (l0 == 0) {
        return 0;
    }
    len += l0;

    l0 = asn1_get_encoded_integer_size(s, s_len);
    if (l0 == 0) {
        return 0;
    }
    len += l0;

    if (!asn1_insert_tag(&sig, sig_end, 0x30) || !asn1_insert_len(&sig, sig_end, len)
        || !asn1_insert_integer(&sig, sig_end, r, r_len)
        || !asn1_insert_integer(&sig, sig_end, s, s_len)) {
        return 0;
    }
    return sig_len - (sig_end - sig);
}

static int asn1_read_len(const uint8_t **p, const uint8_t *end, size_t *len)
{
    /* Adapted from secp256k1 */
    int          lenleft;
    unsigned int b1;
    *len = 0;

    if (*p >= end) {
        return 0;
    }

    b1 = *((*p)++);
    if (b1 == 0xff) {
        /* X.690-0207 8.1.3.5.c the value 0xFF shall not be used. */
        return 0;
    }
    if ((b1 & 0x80u) == 0) {
        /* X.690-0207 8.1.3.4 short form length octets */
        *len = b1;
        return 1;
    }
    if (b1 == 0x80) {
        /* Indefinite length is not allowed in DER. */
        return 0;
    }
    /* X.690-207 8.1.3.5 long form length octets */
    lenleft = b1 & 0x7Fu;
    if (lenleft > end - *p) {
        return 0;
    }
    if (**p == 0) {
        /* Not the shortest possible length encoding. */
        return 0;
    }
    if ((size_t) lenleft > sizeof(size_t)) {
        /* The resulting length would exceed the range of a size_t, so
         * certainly longer than the passed array size.
         */
        return 0;
    }
    while (lenleft > 0) {
        if ((*len >> ((sizeof(size_t) - 1) * 8)) != 0) {
            return 0;
        }
        *len = (*len << 8u) | **p;
        if (*len + lenleft > (size_t) (end - *p)) {
            /* Result exceeds the length of the passed array. */
            return 0;
        }
        (*p)++;
        lenleft--;
    }
    if (*len < 128) {
        /* Not the shortest possible length encoding. */
        return 0;
    }
    return 1;
}

static int asn1_read_tag(const uint8_t **p, const uint8_t *end, size_t *len, int tag)
{
    if ((end - *p) < 1) {
        return 0;
    }

    if (**p != tag) {
        return 0;
    }

    (*p)++;
    return asn1_read_len(p, end, len);
}

static int asn1_parse_integer(const uint8_t **p,
                              const uint8_t  *end,
                              const uint8_t **n,
                              size_t         *n_len)
{
    size_t len;
    int    ret = 0;

    if (!asn1_read_tag(p, end, &len, 0x02)) { /* INTEGER */
        goto end;
    }

    if (len == 0 || len > (size_t) (end - *p)) {
        goto end;
    }

    if (((*p)[0] & 0x80u) == 0x80u) {
        /* Truncated, missing leading 0 (negative number) */
        goto end;
    }

    if ((*p)[0] == 0 && len >= 2 && ((*p)[1] & 0x80u) == 0) {
        /* Zeroes have been prepended to the integer */
        goto end;
    }

    while (len > 0 && **p == 0 && *p != end) { /* Skip leading null bytes */
        (*p)++;
        len--;
    }

    *n     = *p;
    *n_len = len;

    *p += len;
    ret = 1;

end:
    return ret;
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/* ----------------------------------------------------------------------- */

int cx_ecfp_decode_sig_der(const uint8_t  *input,
                           size_t          input_len,
                           size_t          max_size,
                           const uint8_t **r,
                           size_t         *r_len,
                           const uint8_t **s,
                           size_t         *s_len)
{
    size_t         len;
    int            ret       = 0;
    const uint8_t *input_end = input + input_len;

    *s     = NULL;
    *r     = NULL;
    *s_len = 0;
    *r_len = 0;

    const uint8_t *p = (const uint8_t *) input;

    if (!asn1_read_tag(&p, input_end, &len, 0x30)) { /* SEQUENCE */
        goto end;
    }

    if (p + len != input_end) {
        goto end;
    }

    if (!asn1_parse_integer(&p, input_end, r, r_len)
        || !asn1_parse_integer(&p, input_end, s, s_len)) {
        goto end;
    }

    if (p != input_end) { /* Check if bytes have been appended to the sequence */
        goto end;
    }

    if (*r_len > max_size || *s_len > max_size) {
        return 0;
    }
    ret = 1;
end:
    return ret;
}

#endif  // HAVE_ECC
