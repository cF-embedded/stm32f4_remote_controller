/**
 * @file hm_10_init_commands.h
 * @author cF-embedded (cf@embedded.pl)
 * @brief hm-10 init at commands as controller
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _HM_10_INIT_COMMANDS_H_
#define _HM_10_INIT_COMMANDS_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /* compatible with at-09 standard */

#define AT_COMMANDS_INIT_SIZE (sizeof(at_commands_init) / sizeof(at_commands_init[0]))

    /* if you want to change hm-10 module settings , edit only this array */
    const char* at_commands_init[] = {
        "DEFAULT",        /* Set defualt settings */
        "NAMECONTROLLER", /* Name = CONTROLLER */
        "ROLE1",          /* Role = MASTER */
        "IMME0",          /* Transparent mode */
        "INQ",            /* Searching a device to connect */
        "CONADDR_MAC"     /* Set MAC Adress of other hm-10 or bluetooth device */
    };

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _HM_10_INIT_COMMANDS_H_ */
