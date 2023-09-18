/*****************************************************************************
 *   Ledger App Boilerplate.
 *   (c) 2020 Ledger SAS.
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
 *****************************************************************************/

#include <stddef.h>   // size_t
#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool

#include "parser.h"
#include "offsets.h"

bool apdu_parser(command_t *cmd, uint8_t *buf, size_t buf_len)
{
    // Check minimum length, CLA / INS / P1 and P2 are mandatory
    if (buf_len < OFFSET_LC) {
        return false;
    }

    if (buf_len == OFFSET_LC) {
        // Lc field not specified, implies lc = 0
        cmd->lc = 0;
    }
    else {
        // Lc field specified, check value against received length
        cmd->lc = buf[OFFSET_LC];
        if (buf_len - OFFSET_CDATA != cmd->lc) {
            return false;
        }
    }

    cmd->cla  = buf[OFFSET_CLA];
    cmd->ins  = buf[OFFSET_INS];
    cmd->p1   = buf[OFFSET_P1];
    cmd->p2   = buf[OFFSET_P2];
    cmd->data = (cmd->lc > 0) ? buf + OFFSET_CDATA : NULL;

    return true;
}
