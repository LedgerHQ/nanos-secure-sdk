/*
*******************************************************************************
*   Portable FIDO U2F implementation
*   (c) 2016 Ledger
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*   limitations under the License.
********************************************************************************/

#ifndef __U2F_PROCESSING_H__

#define __U2F_PROCESSING_H__

#include "u2f_service.h"

/**
 * Function called when a complete message is received
 */
void u2f_message_complete(u2f_service_t *service);

/**
 * Function to be called upon timer event to notify the U2F library of potential 
 */
void u2f_timeout(u2f_service_t *service);


/**
 * Function to be called by the user when the reply to a previously completed message is ready to be sent
 */
void u2f_message_reply(u2f_service_t *service, uint8_t cmd, uint8_t *buffer, uint16_t length);


#endif
