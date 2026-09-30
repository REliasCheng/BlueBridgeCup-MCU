#include "key_task_safe.h"
#include "peripheral_policy.h"

bit KeyTask_Update(bit scan_due, uchar *led_bit)
{
    uchar key_value = scan_due ? Read_4Key() : 0;
    return PeripheralPolicy_KeyValue(scan_due, key_value, led_bit) != 0;
}
