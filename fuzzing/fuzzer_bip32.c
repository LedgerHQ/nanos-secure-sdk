#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "parser.h"
#include "base58.h"
#include "bip32.h"
#include "qrcodegen.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    uint32_t out[MAX_BIP32_PATH];
    bip32_path_read(data, size, out, MAX_BIP32_PATH);
    return 0;
}
