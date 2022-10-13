/* @BANNER@ */

/**
 * @file    lcx_aes_siv.h
 * @brief   Advanced Encryption Standard with Synthetic Initialization Vector (AES-SIV).
 *
 * @section Description
 *          AES-SIV is an algorithm which provides both authentication and encryption.
 *          It uses AES in CMAC mode to compute a synthetic initialization vector
 *          which is further used by the AES in counter mode to encrypt
 *          or decrypt the plaintext or ciphertext. The synthetic initialization vector
 *          along with the encrypted plaintext is the ciphertext.
 *
 * @author  Ledger
 * @version 1.0
 **/

#if defined(HAVE_AES_SIV)

#ifndef LCX_AES_SIV_H
#define LCX_AES_SIV_H

#include "lcx_cipher.h"

#define AES_SIV_MAX_KEY_LEN (32)

/**
 * @brief AES-SIV context.
 * 
 */
typedef struct _cx_aes_siv_context {
  uint8_t              key1[AES_SIV_MAX_KEY_LEN];    ///< Array to store the leftmost bytes of the key
  uint8_t              key2[AES_SIV_MAX_KEY_LEN];    ///< Array to store the rightmost bytes of the key
  size_t               key_len;                      ///< Size of the inner key in bits
  uint8_t              tag_state[CX_AES_BLOCK_SIZE]; ///< Array to store the S2V state
  cx_cipher_id_t       cipher_type;                  ///< Cipher id
  uint32_t             mode;                         ///< Encryption or decryption
  cx_cipher_context_t *cipher_ctx;                   ///< Pointer to the cipher context used for both AES-CMAC and AES-CTR

} cx_aes_siv_context_t;

/**
 * @brief   Initializes the AES-SIV context.
 * @details The cipher context must be initialized beforehand.
 *          This function must be called first and should be followed
 *          by #cx_aes_siv_set_key, #cx_aes_siv_start, multiple calls to 
 *          #cx_aes_siv_update_aad, #cx_aes_siv_finish and finally 
 *          #cx_aes_siv_update.
 * 
 * @param[in] ctx Pointer to the AES-SIV context.
 * @return        Error code.
 */
cx_err_t cx_aes_siv_init(cx_aes_siv_context_t *ctx);

/**
 * @brief   Sets the key to compute AES-SIV.
 * 
 * @details The size of the key is twice the size of 
 *          typical AES key, i.e. the key size is 256 bits
 *          with AES-128, 384 bits with AES-192 and 512 bits
 *          with AES-256.
 *          This function must be called after #cx_aes_siv_init
 *          and the pointer to the key of the cipher context must
 *          be initialized beforehand.
 * 
 * @param[in] ctx        Pointer to the AES-SIV context.
 * @param[in] key        Pointer to the key.
 * @param[in] key_bitlen Size of the key in bits.
 * @return               Error code
 */
cx_err_t cx_aes_siv_set_key(cx_aes_siv_context_t *ctx,
                            const uint8_t *key,
                            size_t key_bitlen);

/**
 * @brief Starts the S2V algorithm following RFC5297 specification.
 * 
 * @param[in] ctx    Pointer to the AES-SIV context.
 * @param[in] mode   Operation to perform: encryption or decryption.
 * @param[in] iv     Initialization vector: either to initialize the
 *                   S2V computation for encryption or the AES-CTR
 *                   computation for decryption.
 * @param[in] iv_len Length of the initialization vector. This must
 *                   be 16 bytes.
 * @return           Error code
 */
cx_err_t cx_aes_siv_start(cx_aes_siv_context_t *ctx,
                          uint32_t mode,
                          const uint8_t *iv,
                          size_t iv_len);

/**
 * @brief Processes additional data.
 * 
 * @param[in] ctx     Pointer to the AES-SIV context.
 * @param[in] aad     Pointer to the additional data.
 * @param[in] aad_len Length of the additional data.
 * @return            Error code.
 */
cx_err_t cx_aes_siv_update_aad(cx_aes_siv_context_t *ctx,
                               const uint8_t *aad,
                               size_t aad_len);

/**
 * @brief Processes plaintext or ciphertext with AES-CTR.
 * 
 * @param[in]  ctx    Pointer to the AES-SIV context.
 * @param[in]  input  Pointer to the input.
 * @param[out] output Pointer to the output. The buffer 
 *                    will contain *len* bytes.
 * @param[in]  len    Length of the input.
 * @return            Error code.
 */
cx_err_t cx_aes_siv_update(cx_aes_siv_context_t *ctx,
                           const uint8_t *input,
                           uint8_t *output,
                           size_t len);

/**
 * @brief Finishes the S2V algorithm and prepares for the 
 *        AES-CTR computation.
 * 
 * @param[in]     ctx    Pointer to the AES-SIV context.
 * @param[in]     input  Pointer to the input.
 * @param[in]     in_len Length of the input.
 * @param[in,out] tag    Pointer to the tag. The value of the tag
 *                       is given by the caller in case of decryption.
 * @return               Error code.
 */
cx_err_t cx_aes_siv_finish(cx_aes_siv_context_t *ctx,
                           const uint8_t *input,
                           size_t in_len,
                           uint8_t *tag);

/**
 * @brief All-in-one encryption.
 * 
 * @param[in]  ctx     Pointer to the AES-SIV context.
 * @param[in]  input   Pointer to the input.
 * @param[in]  in_len  Length of the input.
 * @param[in]  aad     Pointer to the additional data.
 * @param[in]  aad_len Length of the additional data.
 * @param[out] output  Pointer to the output. The buffer
 *                     will contain *in_len* bytes.
 * @param[out] tag     Pointer to the tag. The buffer
 *                     will contain *16* bytes.
 * @return             Error code.
 */
cx_err_t cx_aes_siv_encrypt(cx_aes_siv_context_t *ctx,
                            const uint8_t *input,
                            size_t in_len,
                            const uint8_t *aad,
                            size_t aad_len,
                            uint8_t *output,
                            uint8_t *tag);

/**
 * @brief All-in-one decryption.
 * 
 * @param[in]  ctx     Pointer to the AES-SIV context.
 * @param[in]  input   Pointer to the input.
 * @param[in]  in_len  Length of the input.
 * @param[in]  aad     Pointer to the additional data.
 * @param[in]  aad_len Length of the additional data.
 * @param[out] output  Pointer to the output. The buffer
 *                     will contain *in_len* bytes.
 * @param[in]  tag     Pointer to the tag to verify.
 *                     The buffer must contain *16* bytes.
 * @return             Error code
 */
cx_err_t cx_aes_siv_decrypt(cx_aes_siv_context_t *ctx,
                            const uint8_t *input,
                            size_t in_len,
                            const uint8_t *aad,
                            size_t aad_len,
                            uint8_t *output,
                            uint8_t *tag);

#endif /* LCX_AES_SIV_H */

#endif // HAVE_AES_SIV
