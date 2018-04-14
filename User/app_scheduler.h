/** @file
 *
 * @defgroup app_scheduler Scheduler
 * @{
 * @ingroup app_common
 *
 * @brief The scheduler is used for transferring execution from the interrupt context to the main
 *        context.
 *
 * @details See @ref ble_sdk_apps_seq_diagrams for sequence diagrams illustrating the flow of events
 *          when using the Scheduler.
 *
 * @section app_scheduler_req Requirements:
 *
 * @subsection main_context_logic Logic in main context:
 *
 *   - Define an event handler for each type of event expected.
 *   - Initialize the scheduler by calling the APP_SCHED_INIT() macro before entering the
 *     application main loop.
 *   - Call app_sched_execute() from the main loop each time the application wakes up because of an
 *     event (typically when sd_app_event_wait() returns).
 *
 * @subsection int_context_logic Logic in interrupt context:
 *
 *   - In the interrupt handler, call app_sched_event_put()
 *     with the appropriate data and event handler. This will insert an event into the
 *     scheduler's queue. The app_sched_execute() function will pull this event and call its
 *     handler in the main context.
 *
 * For an example usage of the scheduler, please see the implementations of
 * @ref ble_sdk_app_hids_mouse and @ref ble_sdk_app_hids_keyboard.
 *
 * @image html scheduler_working.jpg The high level design of the scheduler
 */

#ifndef APP_SCHEDULER_H__
#define APP_SCHEDULER_H__
#include <stdio.h>
#include <string.h>

#include "debug.h"

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#define SCHED_MAX_EVENT_DATA_SIZE       60     /**< Maximum size of scheduler events. Note that scheduler BLE stack events do not contain any data, as the events are being pulled from the stack in the event handler. */
#define SCHED_QUEUE_SIZE                5      /**< Maximum number of events in the scheduler queue. */

#define ERR_SCHEDULER_SUCCESS            0
#define ERR_SCHEDULER_PARAM              1
#define ERR_SCHEDULER_NO_MEM             2
#define ERR_SCHEDULER_INVALID_LENGTH     3
#define ERR_SCHEDULER_NOT_FOUND          4


#define APP_SCHED_EVENT_HEADER_SIZE 8       /**< Size of app_scheduler.event_header_t (only for use inside APP_SCHED_BUF_SIZE()). */

/**@brief Compute number of bytes required to hold the scheduler buffer.
 *
 * @param[in] EVENT_SIZE   Maximum size of events to be passed through the scheduler.
 * @param[in] QUEUE_SIZE   Number of entries in scheduler queue (i.e. the maximum number of events
 *                         that can be scheduled for execution).
 *
 * @return    Required scheduler buffer size (in bytes).
 */
#define APP_SCHED_BUF_SIZE(EVENT_SIZE, QUEUE_SIZE)                                                 \
            (((EVENT_SIZE) + APP_SCHED_EVENT_HEADER_SIZE) * ((QUEUE_SIZE) + 1))
            
/**@brief Scheduler event handler type. */
typedef void (*app_sched_event_handler_t)(void * p_event_data, uint16_t event_size);


/**@brief Perform integer division, making sure the result is rounded up.
 *
 * @details One typical use for this is to compute the number of objects with size B is needed to
 *          hold A number of bytes.
 *
 * @param[in]   A   Numerator.
 * @param[in]   B   Denominator.
 *
 * @return      Integer result of dividing A by B, rounded up.
 */
#define CEIL_DIV(A, B)      \
    /*lint -save -e573 */   \
    ((((A) - 1) / (B)) + 1) \
    /*lint -restore */


/**@brief Macro for initializing the event scheduler.
 *
 * @details It will also handle dimensioning and allocation of the memory buffer required by the
 *          scheduler, making sure the buffer is correctly aligned.
 *
 * @param[in] EVENT_SIZE   Maximum size of events to be passed through the scheduler.
 * @param[in] QUEUE_SIZE   Number of entries in scheduler queue (i.e. the maximum number of events
 *                         that can be scheduled for execution).
 *
 * @note Since this macro allocates a buffer, it must only be called once (it is OK to call it
 *       several times as long as it is from the same location, e.g. to do a reinitialization).
 */
#define APP_SCHED_INIT(EVENT_SIZE, QUEUE_SIZE)                                                     \
    do                                                                                             \
    {                                                                                              \
        static uint32_t APP_SCHED_BUF[CEIL_DIV(APP_SCHED_BUF_SIZE((EVENT_SIZE), (QUEUE_SIZE)),     \
                                               sizeof(uint32_t))];                                 \
        uint32_t ERR_CODE = app_sched_init((EVENT_SIZE), (QUEUE_SIZE), APP_SCHED_BUF);             \
    } while (0)

/**@brief Function for initializing the Scheduler.
 *
 * @details It must be called before entering the main loop.
 *
 * @param[in]   max_event_size   Maximum size of events to be passed through the scheduler.
 * @param[in]   queue_size       Number of entries in scheduler queue (i.e. the maximum number of
 *                               events that can be scheduled for execution).
 * @param[in]   p_event_buffer   Pointer to memory buffer for holding the scheduler queue. It must
 *                               be dimensioned using the APP_SCHED_BUFFER_SIZE() macro. The buffer
 *                               must be aligned to a 4 byte boundary.
 *
 * @note Normally initialization should be done using the APP_SCHED_INIT() macro, as that will both
 *       allocate the scheduler buffer, and also align the buffer correctly.
 *
 * @retval      NRF_SUCCESS               Successful initialization.
 * @retval      NRF_ERROR_INVALID_PARAM   Invalid parameter (buffer not aligned to a 4 byte
 *                                        boundary).
 */
uint32_t app_sched_init(uint16_t max_event_size, uint16_t queue_size, void * p_evt_buffer);

/**@brief Function for executing all scheduled events.
 *
 * @details This function must be called from within the main loop. It will execute all events
 *          scheduled since the last time it was called.
 */
void app_sched_execute(void);

/**@brief Function for scheduling an event.
 *
 * @details Puts an event into the event queue.
 *
 * @param[in]   p_event_data   Pointer to event data to be scheduled.
 * @param[in]   p_event_size   Size of event data to be scheduled.
 * @param[in]   handler        Event handler to receive the event.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t app_sched_event_put(void *                    p_event_data,
                             uint16_t                  event_size,
                             app_sched_event_handler_t handler);

static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}

#endif // APP_SCHEDULER_H__

/** @} */
