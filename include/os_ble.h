#ifdef HAVE_BLE

#include "bolos_target.h"
#include "decorators.h"

/**
 * Write the Bluetooth security database of the given mac address (NULL if the
 * global security database)
 */
SYSCALL void
os_perso_ble_pairing_db_save(unsigned char *mac_addr PLENGTH(mac_addr_len),
                             unsigned int mac_addr_len, unsigned int db_offset,
                             unsigned char *buffer PLENGTH(buffer_len),
                             unsigned int buffer_len);

/**
 * Read a part of the Bluetooth security database and send it
 */
SYSCALL unsigned int
os_perso_ble_pairing_db_load(unsigned char *mac_addr PLENGTH(mac_addr_len),
                             unsigned int mac_addr_len, unsigned int db_offset,
                             unsigned char *buffer PLENGTH(buffer_len),
                             unsigned int buffer_len);

#endif // HAVE_BLE
