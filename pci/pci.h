#ifndef PCI_H
#define PCI_H
#include "kernel/kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PCI_MAX_DEVICES 32

typedef struct
{
    uint8_t  bus;
    uint8_t  slot;
    uint8_t  func;
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t  class_code;
    uint8_t  subclass;
    uint8_t  prog_if;
    uint8_t  revision;
    uint32_t bar[6];
} PACKED pci_dev_t;

void pci_init(void);
uint32_t pci_read(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void     pci_write(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t val);
pci_dev_t *pci_find_device(uint16_t vendor, uint16_t device);
pci_dev_t *pci_find_class(uint8_t class_code, uint8_t subclass);
pci_dev_t *pci_get_device(uint32_t index);
uint32_t   pci_device_count(void);

#ifdef __cplusplus
}
#endif
#endif