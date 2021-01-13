
/* @BANNER@ */

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
 * Auto reply user presence required until the real reply is ready to be replied
 */
void u2f_message_set_autoreply_wait_user_presence(u2f_service_t* service, bool enabled);

/**
 * Return true when a message can be replied using ::u2f_message_reply
 */
bool u2f_message_repliable(u2f_service_t* service);

/**
 * Function to be called by the user when the reply to a previously completed message is ready to be sent
 */
void u2f_message_reply(u2f_service_t *service, uint8_t cmd, uint8_t *buffer, uint16_t length);

#endif
