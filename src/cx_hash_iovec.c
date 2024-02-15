
/*******************************************************************************
 *   (c) 2023 Ledger SAS.
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
#include <stdint.h>   // uint*_t
#include <string.h>   // memset, explicit_bzero
#include <stdbool.h>  // bool

#include "bolos_target.h"
#include "cx.h"
#include "lib_cxng/src/cx_ram.h"

#ifdef TARGET_NANOS
// The G_cx symbol is already defined in some apps.
// In such cases, do not redefine it to avoid conflicts.
__attribute__((weak)) union cx_u G_cx;
#endif

static cx_err_t hash_iovec(cx_hash_t        *hash_ctx,
                           size_t            hash_ctx_size,
                           cx_md_t           hash_id,
                           const cx_iovec_t *iovec,
                           size_t            iovec_len,
                           uint8_t          *digest)
{
    cx_err_t error;

    CX_CHECK(cx_hash_init(hash_ctx, hash_id));
    for (size_t i = 0; i < iovec_len; i++) {
        CX_CHECK(cx_hash_update(hash_ctx, iovec[i].iov_base, iovec[i].iov_len));
    }
    CX_CHECK(cx_hash_final(hash_ctx, digest));

end:
    explicit_bzero(hash_ctx, hash_ctx_size);

    return error;
}

static cx_err_t hash_iovec_ex(cx_hash_t        *hash_ctx,
                              size_t            hash_ctx_size,
                              cx_md_t           hash_id,
                              size_t            hash_digest_len,
                              const cx_iovec_t *iovec,
                              size_t            iovec_len,
                              uint8_t          *digest)
{
    cx_err_t error;

    CX_CHECK(cx_hash_init_ex(hash_ctx, hash_id, hash_digest_len));
    for (size_t i = 0; i < iovec_len; i++) {
        CX_CHECK(cx_hash_update(hash_ctx, iovec[i].iov_base, iovec[i].iov_len));
    }
    CX_CHECK(cx_hash_final(hash_ctx, digest));

end:
    explicit_bzero(hash_ctx, hash_ctx_size);

    return error;
}

#ifdef HAVE_RIPEMD160
cx_err_t cx_ripemd160_hash_iovec(const cx_iovec_t *iovec,
                                 size_t            iovec_len,
                                 uint8_t           digest[static CX_RIPEMD160_SIZE])
{
#ifdef TARGET_NANOS
    cx_ripemd160_t *hash = &G_cx.ripemd160;
#else
    cx_ripemd160_t  ripemd160;
    cx_ripemd160_t *hash = &ripemd160;
#endif

    return hash_iovec(
        &hash->header, sizeof(cx_ripemd160_t), CX_RIPEMD160, iovec, iovec_len, digest);
}
#endif

#ifdef HAVE_SHA224

cx_err_t cx_sha224_hash_iovec(const cx_iovec_t *iovec,
                              size_t            iovec_len,
                              uint8_t           digest[static CX_SHA224_SIZE])
{
#ifdef TARGET_NANOS
    cx_sha256_t *hash = &G_cx.sha256;
#else
    cx_sha256_t     sha256;
    cx_sha256_t    *hash = &sha256;
#endif

    return hash_iovec(&hash->header, sizeof(cx_sha256_t), CX_SHA224, iovec, iovec_len, digest);
}
#endif

#ifdef HAVE_SHA256
cx_err_t cx_sha256_hash_iovec(const cx_iovec_t *iovec,
                              size_t            iovec_len,
                              uint8_t           digest[static CX_SHA256_SIZE])
{
#ifdef TARGET_NANOS
    cx_sha256_t *hash = &G_cx.sha256;
#else
    cx_sha256_t     sha256;
    cx_sha256_t    *hash = &sha256;
#endif

    return hash_iovec(&hash->header, sizeof(cx_sha256_t), CX_SHA256, iovec, iovec_len, digest);
}
#endif

#ifdef HAVE_SHA384
cx_err_t cx_sha384_hash_iovec(const cx_iovec_t *iovec,
                              size_t            iovec_len,
                              uint8_t           digest[static CX_SHA384_SIZE])
{
#ifdef TARGET_NANOS
    cx_sha512_t *hash = &G_cx.sha512;
#else
    cx_sha512_t     sha512;
    cx_sha512_t    *hash = &sha512;
#endif

    return hash_iovec(&hash->header, sizeof(cx_sha512_t), CX_SHA384, iovec, iovec_len, digest);
}
#endif

#ifdef HAVE_SHA512
cx_err_t cx_sha512_hash_iovec(const cx_iovec_t *iovec,
                              size_t            iovec_len,
                              uint8_t           digest[static CX_SHA512_SIZE])
{
#ifdef TARGET_NANOS
    cx_sha512_t *hash = &G_cx.sha512;
#else
    cx_sha512_t     sha512;
    cx_sha512_t    *hash = &sha512;
#endif

    return hash_iovec(&hash->header, sizeof(cx_sha512_t), CX_SHA512, iovec, iovec_len, digest);
}
#endif

#ifdef HAVE_SHA3
#ifdef TARGET_NANOS
#define ALLOCATE_SHA3_HASH() cx_sha3_t *hash = &G_cx.sha3
#else
#define ALLOCATE_SHA3_HASH() \
    cx_sha3_t  sha3;         \
    cx_sha3_t *hash = &sha3
#endif

#define CX_SHA3_BASED_FUNC(func_name, hash_id, digest_len)                                    \
    cx_err_t func_name(                                                                       \
        const cx_iovec_t *iovec, size_t iovec_len, uint8_t digest[static digest_len])         \
    {                                                                                         \
        ALLOCATE_SHA3_HASH();                                                                 \
        return hash_iovec_ex(                                                                 \
            &hash->header, sizeof(cx_sha3_t), hash_id, digest_len, iovec, iovec_len, digest); \
    }

CX_SHA3_BASED_FUNC(cx_sha3_224_hash_iovec, CX_SHA3, CX_SHA3_224_SIZE)
CX_SHA3_BASED_FUNC(cx_sha3_256_hash_iovec, CX_SHA3, CX_SHA3_256_SIZE)
CX_SHA3_BASED_FUNC(cx_sha3_384_hash_iovec, CX_SHA3, CX_SHA3_384_SIZE)
CX_SHA3_BASED_FUNC(cx_sha3_512_hash_iovec, CX_SHA3, CX_SHA3_512_SIZE)

CX_SHA3_BASED_FUNC(cx_keccak_224_hash_iovec, CX_KECCAK, CX_KECCAK_224_SIZE)
CX_SHA3_BASED_FUNC(cx_keccak_256_hash_iovec, CX_KECCAK, CX_KECCAK_256_SIZE)
CX_SHA3_BASED_FUNC(cx_keccak_384_hash_iovec, CX_KECCAK, CX_KECCAK_384_SIZE)
CX_SHA3_BASED_FUNC(cx_keccak_512_hash_iovec, CX_KECCAK, CX_KECCAK_512_SIZE)

cx_err_t cx_shake128_hash_iovec(const cx_iovec_t *iovec,
                                size_t            iovec_len,
                                uint8_t          *digest,
                                size_t            out_length)
{
    ALLOCATE_SHA3_HASH();
    return hash_iovec_ex(
        &hash->header, sizeof(cx_sha3_t), CX_SHAKE128, out_length, iovec, iovec_len, digest);
}

cx_err_t cx_shake256_hash_iovec(const cx_iovec_t *iovec,
                                size_t            iovec_len,
                                uint8_t          *digest,
                                size_t            out_length)
{
    ALLOCATE_SHA3_HASH();
    return hash_iovec_ex(
        &hash->header, sizeof(cx_sha3_t), CX_SHAKE256, out_length, iovec, iovec_len, digest);
}
#endif

#ifdef HAVE_BLAKE2
cx_err_t cx_blake2b_256_hash_iovec(const cx_iovec_t *iovec,
                                   size_t            iovec_len,
                                   uint8_t           digest[static CX_BLAKE2B_256_SIZE])
{
#ifdef TARGET_NANOS
    cx_blake2b_t *hash = &G_cx.blake.blake2b;
#else
    cx_blake2b_t    blake;
    cx_blake2b_t   *hash = &blake;
#endif

    return hash_iovec_ex(&hash->header,
                         sizeof(cx_blake2b_t),
                         CX_BLAKE2B,
                         CX_BLAKE2B_256_SIZE,
                         iovec,
                         iovec_len,
                         digest);
}

cx_err_t cx_blake2b_512_hash_iovec(const cx_iovec_t *iovec,
                                   size_t            iovec_len,
                                   uint8_t           digest[static CX_BLAKE2B_512_SIZE])
{
#ifdef TARGET_NANOS
    cx_blake2b_t *hash = &G_cx.blake.blake2b;
#else
    cx_blake2b_t    blake;
    cx_blake2b_t   *hash = &blake;
#endif

    return hash_iovec_ex(&hash->header,
                         sizeof(cx_blake2b_t),
                         CX_BLAKE2B,
                         CX_BLAKE2B_512_SIZE,
                         iovec,
                         iovec_len,
                         digest);
}
#endif
