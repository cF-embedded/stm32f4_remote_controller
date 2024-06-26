#include "core_init.h"
#include "display.h"
#include "hm_10.h"

void system_init(void)
{
    core_init();

    hm_10_task_init();

    display_tasks_init();
}