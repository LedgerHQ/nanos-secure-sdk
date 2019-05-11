#ifndef USBD_CCID_IMPL_H
#define USBD_CCID_IMPL_H

#define TPDU_EXCHANGE                  0x01
#define SHORT_APDU_EXCHANGE            0x02
#define EXTENDED_APDU_EXCHANGE         0x04
#define CHARACTER_EXCHANGE             0x00

#define EXCHANGE_LEVEL_FEATURE         SHORT_APDU_EXCHANGE
  

#define CCID_BULK_IN_EP       0x83
#define CCID_BULK_EPIN_SIZE   64
#define CCID_BULK_OUT_EP      0x03
#define CCID_BULK_EPOUT_SIZE  64
//#define CCID_INTR_IN_EP       0x81
//#define CCID_INTR_EPIN_SIZE   16

#define CCID_EP0_BUFF_SIZ            64


#endif // USBD_CCID_IMPL_H