#include "core_init.h"
#include "platform_specific.h"

int main(void)
{
    core_init();
    /* Place your initialisation code here. */
    vTaskStartScheduler();

    /* Place your application code here. */

    while(1)
        ;
}
