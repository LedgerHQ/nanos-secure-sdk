
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

#ifdef HAVE_PBKDF2

#include <string.h>

#include "errors.h"
#include "exceptions.h"
#include "cx_pbkdf2.h"
#include "cx_hash.h"
#include "cx_ram.h"

cx_err_t cx_pbkdf2_hmac(cx_md_t        md_type,
                        const uint8_t *password,
                        size_t         password_len,
                        const uint8_t *salt,
                        size_t         salt_len,
                        uint32_t       iterations,
                        uint8_t *      key,
                        size_t         key_len) {
  cx_hmac_t *hmac_ctx = &G_cx.pbkdf2.hmac_ctx;
  uint8_t        counter[4];
  uint8_t *      work = G_cx.pbkdf2.work;
  size_t         work_size;
  uint8_t *      md1 = G_cx.pbkdf2.md1;
  size_t         copy_len;
  size_t         digest_size;

  if (password == NULL || salt == NULL || key == NULL) {
    return CX_INVALID_PARAMETER;
  }

  const cx_hash_info_t *info = cx_hash_get_info(md_type);
  digest_size                = info->output_size;

  memset(counter, 0, sizeof(counter));
  counter[sizeof(counter) - 1] = 1;

  while (key_len) {
    cx_hmac_init(hmac_ctx, md_type, password, password_len);
    cx_hmac_update(hmac_ctx, salt, salt_len);
    cx_hmac_update(hmac_ctx, counter, 4);

    work_size = digest_size;
    cx_hmac_final(hmac_ctx, work, &work_size);

    memcpy(md1, work, digest_size);
    for (uint32_t i = 1; i < iterations; i++) {
      cx_hmac_init(hmac_ctx, md_type, password, password_len);
      cx_hmac_update(hmac_ctx, md1, digest_size);
      work_size = digest_size;
      cx_hmac_final(hmac_ctx, md1, &work_size);

      for (unsigned int j = 0; j < digest_size; j++) {
        work[j] ^= md1[j];
      }
    }

    if (key_len < digest_size) {
      copy_len = key_len;
    } else {
      copy_len = digest_size;
    }
    memcpy(key, work, copy_len);
    key += copy_len;
    key_len -= copy_len;

    // Increment counter
    for (int i = 3; i >= 0; i--) {
      if (++counter[i] != 0) {
        break;
      }
    }
  }
  return CX_OK;
}


cx_err_t cx_pbkdf2_no_throw(cx_md_t md_type,
                   const uint8_t *password,
                   size_t         password_len,
                   uint8_t *      salt,
                   size_t         salt_len,
                   uint32_t       iterations,
                   uint8_t *      out,
                   size_t         out_len) {
  return cx_pbkdf2_hmac(md_type, password, password_len, salt, salt_len, iterations, out, out_len);
}

#endif // HAVE_PBKDF2
