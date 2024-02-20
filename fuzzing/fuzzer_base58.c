#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "parser.h"
#include "base58.h"
#include "bip32.h"
#include "qrcodegen.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    uint8_t out[200];
    base58_decode((const char *) data, size, out, 200);
    return 0;
}
