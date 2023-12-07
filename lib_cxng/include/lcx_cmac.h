/* @BANNER@ */

/**
 * @file    lcx_cmac.h
 * @brief   CMAC (Cipher-based Message Authentication Code).
 */

#ifdef HAVE_CMAC

#ifndef LCX_CMAC_H
#define LCX_CMAC_H

#include "lcx_cipher.h"
#include "cx_errors.h"

WARN_UNUSED_RESULT cx_err_t cx_cmac_start(cx_cipher_context_t *ctx,
                                          const uint8_t       *key,
                                          size_t               key_bitlen);

WARN_UNUSED_RESULT cx_err_t cx_cmac_update(cx_cipher_context_t *ctx,
                                           const uint8_t       *input,
                                           size_t               in_len);

WARN_UNUSED_RESULT cx_err_t cx_cmac_finish(cx_cipher_context_t *ctx, uint8_t *output);

WARN_UNUSED_RESULT cx_err_t cx_cmac(const cx_cipher_id_t type,
                                    const uint8_t       *key,
                                    size_t               key_bitlen,
                                    const uint8_t       *input,
                                    size_t               in_len,
                                    uint8_t             *output);

#endif /* LCX_CMAC_H */

#endif  // HAVE_CMAC
