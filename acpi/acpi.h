#ifndef ACPI_H
#define ACPI_H
#include "kernel/kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

void acpi_init(void);
void acpi_shutdown(void);

#ifdef __cplusplus
}
#endif
#endif