#ifndef pci_h
#define pci_h

#include "ktype.h"

#define PCI_ADDRESS_PORT 0xCF8
#define PCI_VALUE_PORT   0xCFC
#define PCI_NONE 0xFFFF

#define PCI_BAR0                 0x10
#define PCI_BAR1                 0x14
#define PCI_BAR2                 0x18
#define PCI_BAR3                 0x1C
#define PCI_BAR4                 0x20
#define PCI_BAR5                 0x24

#define PCI_COMMAND              0x04
#define PCI_SUBCLASS             0x0a
#define PCI_CLASS                0x0b
#define PCI_HEADER_TYPE          0x0e
#define PCI_VENDOR_ID            0x00
#define PCI_DEVICE_ID            0x02

#define PCI_HEADER_TYPE_DEVICE  0
#define PCI_HEADER_TYPE_BRIDGE  1
#define PCI_HEADER_TYPE_CARDBUS 2

#define PCI_TYPE_BRIDGE 0x0604
#define PCI_TYPE_SATA   0x0106


#define PCI_SECONDARY_BUS        0x19
typedef void (*pci_scanner_fn_t)(uint32_t, uint16_t, uint16_t, void *);

uint16_t pci_find_type(uint32_t);
const char *pci_vendor_lookup(uint16_t);
void pci_scan(pci_scanner_fn_t, int, void *);
void print_pci_devices(uint32_t, uint16_t, uint16_t, void *);
uint32_t pci_read_field(uint32_t, int, int);
void pci_write_field(uint32_t, int, int, uint32_t);

#endif
