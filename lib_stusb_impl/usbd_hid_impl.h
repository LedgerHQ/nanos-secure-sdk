
/*******************************************************************************
 *   Ledger Nano S - Secure firmware
 *   (c) 2021 Ledger
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

#ifndef USBD_HID_IMPL_H
#define USBD_HID_IMPL_H

// ================================================
// HIDGEN

#define HID_INTF 0

#define HID_EPIN_ADDR 0x82
#define HID_EPIN_SIZE 0x40

#define HID_EPOUT_ADDR 0x02
#define HID_EPOUT_SIZE 0x40

#ifdef HAVE_IO_U2F
// ================================================
// HID U2F

#define U2F_INTF 1

#define U2F_EPIN_ADDR 0x81
#define U2F_EPIN_SIZE 0x40

#define U2F_EPOUT_ADDR 0x01
#define U2F_EPOUT_SIZE 0x40
#endif  // HAVE_IO_U2F

#ifdef HAVE_WEBUSB

#define WEBUSB_EPIN_ADDR  0x83
#define WEBUSB_EPIN_SIZE  0x40
#define WEBUSB_EPOUT_ADDR 0x03
#define WEBUSB_EPOUT_SIZE 0x40

#ifdef HAVE_USB_CLASS_CCID
#error Unsupported CCID+WEBUSB, not enough endpoints
#endif  // HAVE_USB_CLASS_CCID

#ifdef HAVE_IO_U2F
#define WEBUSB_INTF 2
#else  // HAVE_IO_U2F
#define WEBUSB_INTF 1
#endif  // HAVE_IO_U2F
#endif  // HAVE_WEBUSB

#endif  // USBD_HID_IMPL_H
