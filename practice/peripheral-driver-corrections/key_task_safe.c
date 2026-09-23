#include "key_task_safe.h"

bit KeyTask_Update(bit scan_due, uchar *led_bit)
{
    uchar key_value = 0;
    if(!scan_due || led_bit == 0) {
        return 0;
    }
    key_value = Read_4Key();
    if(key_value >= 4 && key_value <= 7) {
        *led_bit = key_value;
        return 1;
    }
    return 0;
}
