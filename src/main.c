
#include "initialization.h"
#include "platform_specific.h"

int main(void)
{
    /* Initialization whole system */
    system_init();
    /* Place your initialisation code here. */
    vTaskStartScheduler();

    /* Place your application code here. */

    while(1)
        ;
}
