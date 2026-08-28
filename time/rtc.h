#ifndef RTC_H
#define RTC_H
#include "kernel/kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} rtc_time_t;

void rtc_read(rtc_time_t *t);
uint32_t rtc_to_unix(const rtc_time_t *t);

#ifdef __cplusplus
}
#endif
#endif