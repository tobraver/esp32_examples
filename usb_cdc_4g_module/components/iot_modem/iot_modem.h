#ifndef __IOT_MODEM_H__
#define __IOT_MODEM_H__

#include "stdio.h"

#ifdef __cplusplus
extern "C" {
#endif

int iot_modem_init(void);
int iot_modem_deinit(void);
void iot_modem_info(void);

#ifdef __cplusplus
}
#endif
#endif // !__IOT_MODEM_H__
