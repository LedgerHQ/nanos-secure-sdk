#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "parser.h"
#include "base58.h"
#include "bip32.h"
#include "qrcodegen.h"
#include "os_io.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    uint8_t   apdu_message[IO_APDU_BUFFER_SIZE];
    command_t cmd;

    if (size > IO_APDU_BUFFER_SIZE) {
        return 0;
    }

    memcpy(apdu_message, data, size);
    apdu_parser(&cmd, apdu_message, size);
    return 0;
}
