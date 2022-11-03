/* @BANNER@ */

/**
 * @file    lcx_cipher.h
 * @brief   API for block ciphers.
 *
 * This file contains the functions which enable to use standard block ciphers
 * like AES, DES and 3-DES for encryption and decryption in a specific mode of
 * operation.
 */
#pragma once
#include "cx_errors.h"
#include "lcx_wrappers.h"
#include "lcx_common.h"
#include "lcx_aes.h"
#include "lcx_des.h"
#include <stddef.h>

/** Maximum length of the initialization vector in bytes */
#define MAX_IV_LENGTH    16
/** Maximum block size in bytes */
#define MAX_BLOCK_LENGTH 16

#ifdef HAVE_CMAC
/** Maximum block length for CMAC */
#define CMAC_MAX_BLOCK_LENGTH (16)

/** CMAC context */
typedef struct {
  uint8_t state[CMAC_MAX_BLOCK_LENGTH];
  uint8_t unprocessed_block[CMAC_MAX_BLOCK_LENGTH];
  size_t  unprocessed_len;
} cx_cmac_context_t;
#endif // HAVE_CMAC

/** Supported cipher identifiers */
typedef enum {
    CX_CIPHER_NONE = 0,    ///< No cipher
    CX_CIPHER_AES_128,     ///< AES with a 128-bit key
    CX_CIPHER_AES_192,     ///< AES with a 192-bit key
    CX_CIPHER_AES_256,     ///< AES with a 256-bit key
    CX_CIPHER_DES_64,      ///< DES with a 64-bit key
    CX_CIPHER_3DES_128,    ///< 3DES with two keys
    CX_CIPHER_3DES_192,    ///< 3DES with three keys
} cx_cipher_id_t ;

/** Generic key structure */
typedef struct {
  unsigned int  size;
  unsigned char keys[32];
} cipher_key_t;



/** Base cipher information */
typedef struct {
    cx_err_t (*enc_func)(const cipher_key_t *ctx_key, const uint8_t *in_block, uint8_t *out_block); ///< Encryption function
    cx_err_t (*dec_func)(const cipher_key_t *ctx_key, const uint8_t *in_block, uint8_t *out_block); ///< Decryption function
    cx_err_t (*ctr_func)(const cipher_key_t *ctx_key, size_t len, size_t *nc_off, uint8_t *nonce_counter,
                         uint8_t *stream_block, const uint8_t *input, uint8_t *output);             ///< Encryption in CTR mode
    cx_err_t (*setkey_func)(const cipher_key_t *ctx_key, uint32_t operation, const uint8_t *key,
                            uint32_t key_bitlen);                                                   ///< Set key for encryption or decryption
    cx_err_t (*ctx_reset)(void);                                                                    ///< Reset
} cx_cipher_base_t;

/** Cipher information */
typedef struct {
    uint32_t                key_bitlen; ///< Key size
    uint32_t                iv_size;    ///< Initialization vector size
    uint32_t                block_size; ///< Block size
    const cx_cipher_base_t *base;       /// Structure for base cipher

} cx_cipher_info_t;

/** Generic cipher context */
typedef struct {
    const cx_cipher_info_t *cipher_info;                                       ///< Cipher information
    uint32_t                key_bitlen;                                        ///< Key size in bits
    uint32_t                operation;                                         ///< Operation: encryption or decryption
    void (*add_padding)(uint8_t *output, size_t out_len, size_t data_len);     ///< Padding function
    cx_err_t (*get_padding)(uint8_t *input, size_t in_len, size_t *data_len);  ///< Check the padding
    uint8_t                 unprocessed_data[MAX_BLOCK_LENGTH];                ///< Data to process
    size_t                  unprocessed_len;                                   ///< Length of data to process
    uint8_t                 iv[MAX_IV_LENGTH];                                 ///< Initiaization vector
    size_t                  iv_size;                                           ///< Length of the initialization vector
    uint32_t                mode;                                              ///< Mode of operation: ECB, CBC, CTR
    uint8_t                 sig[MAX_BLOCK_LENGTH];                             ///< Last block to be verified
    const cipher_key_t     *cipher_key;                                        ///< Cipher-specific context
#ifdef HAVE_CMAC
    cx_cmac_context_t      *cmac_ctx;
#endif // HAVE_CMAC
} cx_cipher_context_t;


/**
 * @brief   Initialize a cipher context as NONE.
 *
 * @details This function must be called first.
 *
 * @param[in] ctx Pointer to the context. This must not be NULL.
 *
 * @return        Error code:
 *                - CX_OK on success
 *                - CX_INVALID_PARAMETER
 */
cx_err_t cx_cipher_init(cx_cipher_context_t *ctx);

/**
 * @brief   Initialize and fill the context structure given the cipher info.
 *
 * @param[in] ctx  Pointer to the context.
 *
 * @param[in] type Cipher to use:
 *                 - CX_CIPHER_AES_128
 *                 - CX_CIPHER_AES_192
 *                 - CX_CIPHER_AES_256
 *                 - CX_CIPHER_DES_64
 *                 - CX_CIPHER_3DES_128
 *                 - CX_CIPHER_3DES_192
 *
 * @param[in] mode Mode of operation:
 *                 - ECB
 *                 - CBC
 *                 - CTR
 *
 * @return         Error code:
 *                 - CX_OK on success
 *                 - CX_INVALID_PARAMETER
 *                 - CX_INVALID_PARAMETER_VALUE
 */
cx_err_t cx_cipher_setup(cx_cipher_context_t *ctx, const cx_cipher_id_t type, uint32_t mode);

/**
 * @brief   Set the key to use.
 *
 * @details This function must be called after the cipher context is initialized with
 *          #cx_cipher_init.
 *
 * @param[in] ctx        Pointer to the context.
 *
 * @param[in] key        Key to use: a buffer of at least *key_bitlen* bits.
 *
 * @param[in] key_bitlen Length of key in bits.
 *
 * @param[in] operation  The operation that the key will be used for: encryption or decryption.
 *
 * @return               Error code:
 *                       - CX_OK on success
 *                       - CX_INVALID_PARAMETER
 *                       - CX_INVALID_PARAMETER_SIZE
 *                       - CX_INVALID_PARAMETER_VALUE
 */
cx_err_t cx_cipher_setkey(cx_cipher_context_t *ctx, const uint8_t *key, uint32_t key_bitlen, uint32_t operation);

/**
 * @brief   Set the initialization vector.
 *
 * @details This function must be called after the cipher context is initialized with
 *          #cx_cipher_init.
 *
 * @param[in] ctx    Pointer to the context.
 *
 * @param[in] iv     Initialization vector: a buffer of at least *iv_len* bytes.
 *
 * @param[in] iv_len Length of the initialization vector in bytes.
 *
 * @return           Error code:
 *                   - CX_OK on success
 *                   - CX_INVALID_PARAMETER
 *                   - CX_INVALID_PARAMETER_VALUE
 */
cx_err_t cx_cipher_setiv(cx_cipher_context_t *ctx, const uint8_t *iv, size_t iv_len);

/**
 * @brief   Set the padding type.
 *
 * @details This function must be called after the cipher context is initialized with
 *          #cx_cipher_init.
 *
 * @param[in] ctx Pointer to the context.
 *
 * @param[in] padding Type of padding:
 *                    - CX_PAD_NONE: no padding
 *                    - CX_PAD_ISO9797M1: pad with zeros only
 *                    - CX_PAD_ISO9797M2: pad with a single one and several zeros.
 *
 * @return            Error code:
 *                    - CX_OK on success
 *                    - CX_INVALID_PARAMETER
 *                    - CX_INVALID_PARAMETER_VALUE
 */
cx_err_t cx_cipher_set_padding(cx_cipher_context_t *ctx, uint32_t padding);

/**
 * @brief   Encrypt or decrypt with the given context.
 *
 * @details This function must be called after the cipher context is initialized with
 *          #cx_cipher_init and set with #cx_cipher_setup. The key must be set with
 *          #cx_cipher_setkey, the padding method to use must be set with
 *          #cx_cipher_set_padding and the Initialization Vector must be set with
 *          #cx_cipher_setiv.
 *          One can call this function multiple times depending on the length of the data
 *          that needed to be processed.
 *
 * @param[in]  ctx     Pointer to the context.
 *
 * @param[in]  input   Input data: buffer of at least *in_len* bytes.
 *
 * @param[in]  in_len  Length of the input in bytes.
 *
 * @param[out] output  Output data: a buffer of at least *in_len + block_size* bytes.
 *
 * @param[out] out_len Length of the data written to the output.
 *
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_INTERNAL_ERROR
 *                     - CX_INVALID_PARAMETER
 *                     - CX_INVALID_PARAMETER_VALUE
 */
cx_err_t cx_cipher_update(cx_cipher_context_t *ctx, const uint8_t *input, size_t in_len, uint8_t *output, size_t *out_len);

/**
 * @brief   Finalize the operation.
 *
 * @details This function must be called after the cipher context is initialized with
 *          #cx_cipher_init and set with #cx_cipher_setup. The key must be set with
 *          #cx_cipher_setkey, the padding method to use must be set with
 *          #cx_cipher_set_padding and the Initialization Vector must be set with
 *          #cx_cipher_setiv.
 *          This function must be called after the last #cx_cipher_update.
 *
 * @param[in]  ctx     Pointer to the context.
 *
 * @param[out] output  Output data: buffer of at least *block_size* bytes.
 *
 * @param[out] out_len Length of the data written to the output.
 *
 * @return             Error code:
 *                     - CX_Ok on success
 *                     - CX_INTERNAL_ERROR
 *                     - CX_INVALID_PARAMETER
 *                     - CX_INVALID_PARAMETER_VALUE
 */
cx_err_t cx_cipher_finish(cx_cipher_context_t *ctx, uint8_t *output, size_t *out_len);

/**
 * @brief   All-in-one encryption or decryption.
 *
 * @details This function must be called after the cipher context is initialized with
 *          #cx_cipher_init and set with #cx_cipher_setup. The key must be set with
 *          #cx_cipher_setkey, the padding method to use must be set with
 *          #cx_cipher_set_padding.
 *
 * @param[in] ctx      Pointer to the context.
 *
 * @param[in] iv       Initialization vector: a buffer of at least *iv_len* bytes.
 *
 * @param[in] iv_len   Length of the initialization vector in bytes.
 *
 * @param[in] input    Input data: a buffer of at least *in_len* bytes.
 *
 * @param[in] in_len   Length of the input in bytes.
 *
 * @param[out] output  Output data: a buffer of at least *in_len + block_size* bytes.
 *
 * @param[out] out_len Length of the output.
 *
 * @return             Error code:
 *                     - CX_OK on success
 *                     - CX_INTERNAL_ERROR
 *                     - CX_INVALID_PARAMETER
 *                     - CX_INVALID_PARAMETER_VALUE
 */
cx_err_t cx_cipher_enc_dec(cx_cipher_context_t *ctx, const uint8_t *iv, size_t iv_len, const uint8_t *input, size_t in_len,
                           uint8_t *output, size_t *out_len);

void cx_cipher_reset(cx_cipher_context_t *ctx);

void add_one_and_zeros_padding(uint8_t *output, size_t out_len, size_t data_len);
