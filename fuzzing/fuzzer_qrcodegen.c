#include <string.h>

#include "qrcodegen.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    uint8_t             dataAndTemp[qrcodegen_BUFFER_LEN_FOR_VERSION(10)];
    uint8_t             qrcode[qrcodegen_BUFFER_LEN_FOR_VERSION(10)];
    enum qrcodegen_Ecc  ecc;
    enum qrcodegen_Mask mask;

    if (size < 2 || size + 2 > qrcodegen_BUFFER_LEN_FOR_VERSION(10)) {
        return 0;
    }

    memcpy(dataAndTemp, data + 2, size - 2);
    ecc  = data[0] % 4;
    mask = data[1] % 8;

    qrcodegen_encodeBinary(dataAndTemp, size - 2, qrcode, ecc, 4, 10, mask, true);
    return 0;
}
