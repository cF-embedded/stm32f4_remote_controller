/**
 * @file hm_10.h
 * @author cF-embedded (cf@embedded.pl)
 * @brief hm-10 driver with at commands
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _HM_10_H_
#define _HM_10_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "platform_specific.h"

    /**
     * Send buffer through hm-10.
     *
     * @param buf           Buffer to send.
     * @param len           Length of buffer.
     *
     * @return              Error code.
     */
    int32_t hm_10_send_buf(uint8_t* buf, const int32_t len);

    /**
     * Send buffer through hm-10.
     *
     * @param buf           Buffer to send.
     * @param len           Length of buffer.
     *
     * @return              Error code.
     */
    int32_t hm_10_send_buf(uint8_t* buf, const int32_t len);

    /**
     * @brief Initialize hm-10 to work
     *
     */
    void hm_10_task_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _HM_10_H_ */
