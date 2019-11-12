
/*******************************************************************************
*   Ledger Nano S - Secure firmware
*   (c) 2019 Ledger
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

#ifdef CX_COMPLIANCE_141
#undef CX_COMPLIANCE_141

#include "os.h"
#include "cx.h"








int  cx_rng_rfc6979_X(unsigned char *rnd ,
                      unsigned int hashID, unsigned char *h1 ,
                      unsigned char *x ,unsigned int x_len, 
                      unsigned char *q , unsigned int q_len, 
                      unsigned char *V , unsigned int V_len) {
    unsigned int hsz = 0;
    switch(hashID) {
    case CX_SHA256:
        hsz =  32;
        break;
    case CX_SHA512:
        hsz = 64;
        break;
    default:
        THROW(INVALID_PARAMETER);
        return 0;
    }

    return cx_rng_rfc6979(rnd, q_len, hashID, h1, hsz, x, x_len,q,q_len,V,V_len);
}

int cx_hash_X(cx_hash_t *hash ,
              int mode,
              unsigned char WIDE *in , unsigned int len,
              unsigned char *out) {
   unsigned int hsz = 0;

    switch (hash->algo) {
    case CX_RIPEMD160:
        hsz = 20;
        break;
    case CX_SHA224:
        hsz = 28;
        break;
    case CX_SHA256:
        hsz = 32;
        break;
    case CX_SHA384:
        hsz = 48;
        break;
    case CX_SHA512:
        hsz = 64;
        break;
    case CX_SHA3:
    case CX_KECCAK:
    case CX_SHAKE256:
        hsz =   ((cx_sha3_t*)hash)->output_size;
        break;
    case CX_GROESTL:
        hsz =   ((cx_groestl_t*)hash)->output_size;
        break;  
    case CX_BLAKE2B:
        hsz =   ((cx_blake2b_t*)hash)->output_size;
        break;
    default:
        THROW(INVALID_PARAMETER);
        return 0;
    }

    return cx_hash(hash, mode, in, len, out, hsz);

}

int cx_hash_sha256_X(unsigned char WIDE *in, unsigned int len, unsigned char *out){
    return cx_hash_sha256(in, len, out, 32);
}

int cx_hash_sha512_X(unsigned char WIDE *in, unsigned int len, unsigned char *out){
    return cx_hash_sha512(in, len, out, 64);
}

int cx_hmac_X(cx_hmac_t *hmac, int mode,unsigned char WIDE *in, unsigned int len, unsigned char *mac) {
    unsigned int hsz = 0;
    switch(((cx_hash_t*)hmac)->algo) {
    case CX_RIPEMD160:
        hsz = 20;
        break;
    case CX_SHA256:
        hsz =  32;
        break;
    case CX_SHA512:
        hsz = 64;
        break;
    default:
        THROW(INVALID_PARAMETER);
        return 0;
    }
    return cx_hmac(hmac, mode, in, len, mac, hsz);
}

int cx_hmac_sha256_X(unsigned char WIDE *key, unsigned int key_len, unsigned char WIDE *in, unsigned int len, unsigned char *out){
    return cx_hmac_sha256(key, key_len, in, len, out, 32);
}

int cx_hmac_sha512_X(unsigned char WIDE *key, unsigned int key_len, unsigned char WIDE *in, unsigned int len, unsigned char *out){
    return cx_hmac_sha512(key, key_len, in, len, out, 64);
}


int cx_des_iv_X(cx_des_key_t WIDE *key,  int mode,unsigned char  WIDE *iv ,unsigned char  WIDE *in,unsigned int len,unsigned char *out) {
    return cx_des_iv(key, mode, iv, 8, in, len, out, UPPER_ALIGN(len+7, 8, unsigned int));
}
int cx_des_X(cx_des_key_t WIDE *key , int mode, unsigned char  WIDE *in ,unsigned int len,  unsigned char *out){
    return cx_des(key, mode, in, len, out, UPPER_ALIGN(len+7, 8, unsigned int));
}

int cx_aes_iv_X(cx_aes_key_t WIDE *key,  int mode,unsigned char  WIDE *iv ,unsigned char  WIDE *in,unsigned int len,unsigned char *out){
    return cx_aes_iv(key, mode, iv, 16, in, len, out, UPPER_ALIGN(len+15, 16, unsigned int));
}
int cx_aes_X(cx_aes_key_t WIDE *key , int mode, unsigned char  WIDE *in ,unsigned int len,  unsigned char *out) {
    return cx_aes(key, mode, in, len, out, UPPER_ALIGN(len+15, 16, unsigned int));
}


int cx_rsa_init_public_key_X(unsigned char  WIDE *exponent , unsigned char  WIDE *modulus , unsigned int modulus_len,  cx_rsa_public_key_t  *key) {
    return cx_rsa_init_public_key(exponent, 4, modulus, modulus_len, key);

}
int cx_rsa_init_private_key_X(unsigned char  WIDE *exponent, unsigned char  WIDE *modulus, unsigned int modulus_len, cx_rsa_private_key_t  *key){
    return cx_rsa_init_private_key(exponent, modulus_len,  modulus, modulus_len, key);

}

int cx_rsa_generate_pair_X(unsigned int modulus_len,cx_rsa_public_key_t  *public_key, cx_rsa_private_key_t *private_key, unsigned long int pub_exponent, const unsigned char *externalPQ) {
    unsigned char exponent[4];
    if (pub_exponent == 0) {
        pub_exponent = 0x00010001;
    }
    exponent[0] = pub_exponent>>24;
    exponent[1] = pub_exponent>>16;
    exponent[2] = pub_exponent>>8;
    exponent[3] = pub_exponent>>0;

    return cx_rsa_generate_pair(modulus_len, public_key, private_key, exponent, 4, externalPQ);
}

static unsigned int cx_ecfp_get_domain_length(cx_curve_t curve) {
    switch(curve) {
    case CX_CURVE_FRP256V1:
        return 32;
    case CX_CURVE_SECP256K1:
        return 32;
    case CX_CURVE_SECP256R1:
        return 32;
    case CX_CURVE_SECP384R1:
        return 48;
    case CX_CURVE_SECP521R1:
        return 66;
    case CX_CURVE_BrainPoolP256R1:
        return 32;
    case CX_CURVE_BrainPoolP256T1:
        return 32;
    case CX_CURVE_BrainPoolP320R1:
        return 40;
    case CX_CURVE_BrainPoolP320T1:
        return 40;
    case CX_CURVE_BrainPoolP384R1:
        return 48;
    case CX_CURVE_BrainPoolP384T1:
        return 48;
    case CX_CURVE_BrainPoolP512R1:
        return 64;
    case CX_CURVE_BrainPoolP512T1:
        return 64;
    case CX_CURVE_Ed25519:
        return 32;
    case CX_CURVE_Ed448:
        return 57;
    case CX_CURVE_Curve25519:
        return 32;
    case CX_CURVE_Curve448:
        return 56;
    default:
        break;
    }
    THROW(INVALID_PARAMETER);
    return 0;
}

int cx_ecfp_is_valid_point_X(cx_curve_t curve, unsigned char WIDE *point) {
    const unsigned int  domain_length =  cx_ecfp_get_domain_length(curve);
    return cx_ecfp_is_valid_point(curve, point, 1+2*domain_length);
}

int cx_ecfp_is_cryptographic_point_X(cx_curve_t curve, unsigned char WIDE *point) {
    const unsigned int  domain_length =  cx_ecfp_get_domain_length(curve);
    return cx_ecfp_is_cryptographic_point(curve, point, 1+2*domain_length);
}

int cx_ecfp_add_point_X(cx_curve_t curve, unsigned char *R ,unsigned char WIDE *P, unsigned char WIDE *Q) {
    const unsigned int  domain_length =  cx_ecfp_get_domain_length(curve);
    return   cx_ecfp_add_point(curve, R, P, Q, 1+2*domain_length);
}

int cx_ecfp_scalar_mult_X(cx_curve_t curve,  unsigned char *P, unsigned char WIDE *k, unsigned int k_len) {
    const unsigned int  domain_length =  cx_ecfp_get_domain_length(curve);
    return   cx_ecfp_scalar_mult(curve, P, 1+2*domain_length, k, k_len);
}

int cx_ecschnorr_sign_X(cx_ecfp_private_key_t WIDE *pv_key, int mode,  cx_md_t hashID,  unsigned char *msg , unsigned int msg_len,  unsigned char *sig , unsigned int *info) {
    const unsigned int  domain_length =  cx_ecfp_get_domain_length(pv_key->curve);
    return cx_ecschnorr_sign(pv_key, mode, hashID, msg, msg_len, sig, 6+2*(domain_length+1), info);
}

void cx_edward_decompress_point_X(cx_curve_t curve, unsigned char *P) {
    const unsigned int  domain_length =  cx_ecfp_get_domain_length(curve);
    return cx_edward_decompress_point(curve, P, 1+2*domain_length);
}

void cx_edward_compress_point_X(cx_curve_t curve, unsigned char *P) {
    const unsigned int  domain_length =  cx_ecfp_get_domain_length(curve);
    cx_edward_compress_point(curve, P, 1+2*domain_length);
}

void cx_eddsa_get_public_key_X(cx_ecfp_private_key_t WIDE *pv_key, cx_md_t hashID, cx_ecfp_public_key_t *pu_key, unsigned char* a, unsigned char* h) {
    const unsigned int  domain_length =  cx_ecfp_get_domain_length(pu_key->curve);
    return cx_eddsa_get_public_key(pv_key, hashID, pu_key, a, domain_length, h, domain_length);
}

int cx_eddsa_sign_X(cx_ecfp_private_key_t WIDE *pv_key, 
                    int mode,  cx_md_t hashID,  unsigned char  WIDE *hash, unsigned int hash_len,
                    unsigned char  WIDE *ctx, unsigned int ctx_len,
                    unsigned char *sig, unsigned int *info) {
    const unsigned int  domain_length =  cx_ecfp_get_domain_length(pv_key->curve);
    return cx_eddsa_sign(pv_key, mode, hashID, hash, hash_len, ctx, ctx_len, sig,  6+2*(domain_length+1), info);
}

int cx_ecdsa_sign_X(cx_ecfp_private_key_t WIDE *pv_key,
                    int mode,  cx_md_t hashID, unsigned char  WIDE *hash, unsigned int hash_len,
                    unsigned char *sig ,
                    unsigned int *info) {
    const unsigned int  domain_length =  cx_ecfp_get_domain_length(pv_key->curve);
    return cx_ecdsa_sign(pv_key, mode, hashID, hash, hash_len, sig,  6+2*(domain_length+1), info);
}

int cx_ecdh_X(cx_ecfp_private_key_t WIDE *key, int mode, unsigned char WIDE *P, unsigned char *secret) {
    unsigned int sz;
    const unsigned int  domain_length =  cx_ecfp_get_domain_length(key->curve);
    switch  (mode & CX_MASK_EC) { 
    case CX_ECDH_POINT:           
        sz = 1+2*domain_length;
       break;
    case CX_ECDH_X:
        sz = domain_length;
       break;
    default :
        THROW(INVALID_PARAMETER);
        return 0;
    }   
    return cx_ecdh(key, mode, P, 1+2*domain_length, secret, sz);
}







#endif



