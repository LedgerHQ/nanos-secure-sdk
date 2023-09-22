
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

#if defined(HAVE_POLY1305)

#include "cx_poly1305.h"
#include "cx_ram.h"
#include "cx_utils.h"
#include "os_math.h"
#include "os_utils.h"
#include "ox_bn.h"
#include <stddef.h>
#include <string.h>

#define POLY1305_BLOCK_SIZE        16
#define POLY1305_PADDED_BLOCK_SIZE (POLY1305_BLOCK_SIZE + 1)

/* 2^130 - 5 */
const uint8_t MODULUS[] = {0x3,
                           0xff,
                           0xff,
                           0xff,
                           0xff,
                           0xff,
                           0xff,
                           0xff,
                           0xff,
                           0xff,
                           0xff,
                           0xff,
                           0xff,
                           0xff,
                           0xff,
                           0xff,
                           0xfb};

static void cx_le_bytes(const uint8_t *buf, uint8_t *le_buf, size_t len)
{
    size_t i;
    for (i = 0; i < len; i++) {
        le_buf[i] = buf[len - 1 - i];
    }
}

static cx_err_t cx_poly1305_process(cx_poly1305_context_t *ctx,
                                    size_t                 nblocks,
                                    const uint8_t         *input,
                                    uint8_t                needs_padding)
{
    size_t   offset = 0;
    size_t   i;
    cx_bn_t  acc, r, p, d, res;
    uint8_t  buf[POLY1305_PADDED_BLOCK_SIZE];
    cx_err_t error;

    CX_CHECK(cx_bn_lock(32, 0));
    CX_CHECK(cx_bn_alloc_init(&p, 32, MODULUS, sizeof(MODULUS)));
    CX_CHECK(cx_bn_alloc(&acc, 32));
    CX_CHECK(cx_bn_alloc(&r, 32));
    CX_CHECK(cx_bn_alloc(&d, 32));
    CX_CHECK(cx_bn_alloc(&res, 64));

    /* Process full blocks */
    for (i = 0; i < nblocks; i++) {
        /* The input block is treated as a 128-bit little-endian integer */
        buf[0] = needs_padding;
        cx_le_bytes(input + offset, buf + 1, POLY1305_BLOCK_SIZE);
        CX_CHECK(cx_bn_init(d, buf, POLY1305_PADDED_BLOCK_SIZE));

        buf[0] = (uint8_t) ctx->acc[4];
        U4BE_ENCODE(buf, 1, ctx->acc[3]);
        U4BE_ENCODE(buf + 4 + 1, 0, ctx->acc[2]);
        U4BE_ENCODE(buf + 8 + 1, 0, ctx->acc[1]);
        U4BE_ENCODE(buf + 12 + 1, 0, ctx->acc[0]);
        CX_CHECK(cx_bn_init(acc, buf, sizeof(buf)));

        /* Compute: acc += (padded) block as a 130-bit integer */
        CX_CHECK(cx_bn_add(acc, acc, d));

        U4BE_ENCODE(buf, 0, ctx->r[3]);
        U4BE_ENCODE(buf + 4, 0, ctx->r[2]);
        U4BE_ENCODE(buf + 8, 0, ctx->r[1]);
        U4BE_ENCODE(buf + 12, 0, ctx->r[0]);
        CX_CHECK(cx_bn_init(r, buf, POLY1305_BLOCK_SIZE));

        /* Compute: acc *= r */
        CX_CHECK(cx_bn_mul(res, acc, r));

        /* Compute: acc %= (2^130 - 5) */
        CX_CHECK(cx_bn_reduce(acc, res, p));

        CX_CHECK(cx_bn_export(acc, buf, sizeof(MODULUS)));
        ctx->acc[0] = U4BE(buf, 12 + 1);
        ctx->acc[1] = U4BE(buf, 8 + 1);
        ctx->acc[2] = U4BE(buf, 4 + 1);
        ctx->acc[3] = U4BE(buf, 1);
        ctx->acc[4] = buf[0];

        offset += POLY1305_BLOCK_SIZE;
    }

end:
    cx_bn_unlock();
    return error;
}

static cx_err_t cx_poly1305_compute_mac(cx_poly1305_context_t *ctx, uint8_t *tag)
{
    cx_err_t error;
    cx_bn_t  acc, s;
    uint8_t  buf[POLY1305_PADDED_BLOCK_SIZE];

    CX_CHECK(cx_bn_lock(32, 0));
    CX_CHECK(cx_bn_alloc(&acc, 32));
    CX_CHECK(cx_bn_alloc(&s, 32));

    buf[0] = (uint8_t) ctx->acc[4];
    U4BE_ENCODE(buf, 1, ctx->acc[3]);
    U4BE_ENCODE(buf + 4 + 1, 0, ctx->acc[2]);
    U4BE_ENCODE(buf + 8 + 1, 0, ctx->acc[1]);
    U4BE_ENCODE(buf + 12 + 1, 0, ctx->acc[0]);
    CX_CHECK(cx_bn_init(acc, buf, sizeof(buf)));

    U4BE_ENCODE(buf, 0, ctx->s[3]);
    U4BE_ENCODE(buf + 4, 0, ctx->s[2]);
    U4BE_ENCODE(buf + 8, 0, ctx->s[1]);
    U4BE_ENCODE(buf + 12, 0, ctx->s[0]);
    CX_CHECK(cx_bn_init(s, buf, POLY1305_BLOCK_SIZE));

    /* Compute acc += s */
    CX_CHECK(cx_bn_add(acc, acc, s));

    /* Compute MAC (128 least significant bits of the accumulator) */
    CX_CHECK(cx_bn_export(acc, buf, sizeof(MODULUS)));
    cx_le_bytes(buf + 1, tag, POLY1305_BLOCK_SIZE);

end:
    cx_bn_unlock();
    return error;
}

void cx_poly1305_init(cx_poly1305_context_t *ctx)
{
    memset(ctx, 0, sizeof(cx_poly1305_context_t));
}

void cx_poly1305_set_key(cx_poly1305_context_t *ctx, const uint8_t *key)
{
    /* r &= 0x0ffffffc0ffffffc0ffffffc0fffffff */
    ctx->r[0] = U4LE(key, 0) & 0x0FFFFFFF;
    ctx->r[1] = U4LE(key, 4) & 0x0FFFFFFC;
    ctx->r[2] = U4LE(key, 8) & 0x0FFFFFFC;
    ctx->r[3] = U4LE(key, 12) & 0x0FFFFFFC;
    ctx->s[0] = U4LE(key, 16);
    ctx->s[1] = U4LE(key, 20);
    ctx->s[2] = U4LE(key, 24);
    ctx->s[3] = U4LE(key, 28);
    /* Initial accumulator state */
    ctx->acc[0] = 0;
    ctx->acc[1] = 0;
    ctx->acc[2] = 0;
    ctx->acc[3] = 0;
    ctx->acc[4] = 0;
    memset(ctx->block, 0, sizeof(ctx->block));
    ctx->block_len = 0;
}

cx_err_t cx_poly1305_update(cx_poly1305_context_t *ctx, const uint8_t *input, size_t in_len)
{
    size_t   offset    = 0;
    size_t   remaining = in_len;
    cx_err_t error     = CX_OK;
    size_t   n;

    if ((remaining > 0) && (ctx->block_len > 0)) {
        n = POLY1305_BLOCK_SIZE - ctx->block_len;
        if (in_len < n) {
            /* Not enough data to complete the block.
             * Store this data with the other leftovers.
             */
            memcpy(&ctx->block[ctx->block_len], input, in_len);
            ctx->block_len += in_len;
            remaining = 0;
        }
        else {
            /* Enough data to produce a complete block */
            memcpy(&ctx->block[ctx->block_len], input, n);
            ctx->block_len = 0;
            CX_CHECK(cx_poly1305_process(ctx, 1, ctx->block, 1)); /* add padding bit */
            offset += n;
            remaining -= n;
        }
    }

    if (remaining >= POLY1305_BLOCK_SIZE) {
        n = remaining / POLY1305_BLOCK_SIZE;
        CX_CHECK(cx_poly1305_process(ctx, n, &input[offset], 1));
        offset += n * POLY1305_BLOCK_SIZE;
        remaining %= POLY1305_BLOCK_SIZE;
    }

    if (remaining > 0) {
        /* Store partial block */
        ctx->block_len = remaining;
        memcpy(ctx->block, &input[offset], remaining);
    }

end:
    return error;
}

cx_err_t cx_poly1305_finish(cx_poly1305_context_t *ctx, uint8_t *tag)
{
    cx_err_t error;

    /* Process any leftover data */
    if (ctx->block_len > 0) {
        /* Add padding bit */
        ctx->block[ctx->block_len] = 1;
        ctx->block_len++;
        /* Pad with zeroes */
        memset(&ctx->block[ctx->block_len], 0, POLY1305_BLOCK_SIZE - ctx->block_len);
        CX_CHECK(cx_poly1305_process(ctx, 1, ctx->block, 0));
    }
    CX_CHECK(cx_poly1305_compute_mac(ctx, tag));

end:
    return error;
}

cx_err_t cx_poly1305_mac(const uint8_t *key, const uint8_t *input, size_t in_len, uint8_t *tag)
{
    cx_err_t               error;
    cx_poly1305_context_t *ctx = &G_cx.poly1305;
    cx_poly1305_init(ctx);
    cx_poly1305_set_key(ctx, key);
    CX_CHECK(cx_poly1305_update(ctx, input, in_len));
    CX_CHECK(cx_poly1305_finish(ctx, tag));

end:
    explicit_bzero(ctx, sizeof(cx_poly1305_context_t));
    return error;
}

#endif  // HAVE_POLY1305
