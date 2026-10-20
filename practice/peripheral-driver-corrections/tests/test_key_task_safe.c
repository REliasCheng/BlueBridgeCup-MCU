#include "key_task_safe.h"

#include <assert.h>
#include <stdio.h>

static unsigned int read_calls;
static uchar next_key;

uchar Read_4Key(void)
{
    ++read_calls;
    return next_key;
}

int main(void)
{
    uchar led_bit = 5U;

    next_key = 6U;
    assert(!KeyTask_Update(0U, &led_bit));
    assert(read_calls == 0U);
    assert(led_bit == 5U);

    next_key = 0U;
    assert(!KeyTask_Update(1U, &led_bit));
    assert(read_calls == 1U);
    assert(led_bit == 5U);

    next_key = 8U;
    assert(!KeyTask_Update(1U, &led_bit));
    assert(read_calls == 2U);
    assert(led_bit == 5U);

    next_key = 4U;
    assert(KeyTask_Update(1U, &led_bit));
    assert(read_calls == 3U);
    assert(led_bit == 4U);

    next_key = 7U;
    assert(KeyTask_Update(1U, &led_bit));
    assert(read_calls == 4U);
    assert(led_bit == 7U);

    assert(!KeyTask_Update(0U, &led_bit));
    assert(read_calls == 4U);
    assert(led_bit == 7U);

    puts("key task wrapper tests passed");
    return 0;
}
