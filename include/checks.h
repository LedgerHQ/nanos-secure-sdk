/* @BANNER@ */

#if !defined(CHECKS_H)
#define CHECKS_H

#if !defined(HAVE_BOLOS)

#define CHECK_NOT_AUDITED_TLV_TAG           0x9F
#define CHECK_NOT_AUDITED_TLV_VAL           0x01
#define CHECK_NOT_AUDITED_MAX_LEN           0x40

void check_audited_app(void);
#endif // !defined(HAVE_BOLOS)

#endif
