#include "core_init.h"
#include "hm_10.h"
#include "joystick_controller.h"

void system_init(void)
{
    core_init();

    joystick_control_task_init();

    hm_10_task_init();
}