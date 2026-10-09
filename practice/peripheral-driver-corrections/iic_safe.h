#ifndef __IIC_SAFE_H__
#define __IIC_SAFE_H__

#include <library.h>

/* Returns a PCF8591_* status from peripheral_policy.h. On failure *value
 * remains unchanged. Electrical recovery still requires CT107D validation. */
uchar PCF8591_ReadAdcSafe(uchar channel, uchar *value);

#endif
