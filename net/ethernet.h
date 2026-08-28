#ifndef ETHERNET_H
#define ETHERNET_H
#include "kernel/kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETH_ALEN 6
#define ETH_TYPE_IP 0x0800
#define ETH_TYPE_ARP 0x0806

typedef struct {
    uint8_t dst[ETH_ALEN];
    uint8_t src[ETH_ALEN];
    uint16_t type;
} PACKED eth_header_t;

#ifdef __cplusplus
}
#endif
#endif