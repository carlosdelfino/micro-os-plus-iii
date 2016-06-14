/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2016 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The initial CMSIS++ RTOS API was inspired by CMSIS RTOS API v1.x,
 * Copyright (c) 2013 ARM LIMITED.
 */

/*
 * This file is part of the CMSIS++ proposal, intended as a CMSIS
 * replacement for C++ applications, and provides a C API.
 *
 * The code is inspired by ARM CMSIS cmsis_os.h file, v1.02,
 * and tries to remain functionally close to the CMSIS specifications.
 */

#ifndef CMSIS_PLUS_RTOS_OS_C_API_H_
#define CMSIS_PLUS_RTOS_OS_C_API_H_

// ----------------------------------------------------------------------------

// Include the CMSIS++ C API structures declarations.
#include <cmsis-plus/rtos/os-c-decls.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// ----------------------------------------------------------------------------

#ifdef  __cplusplus
extern "C"
{
#endif

  /**
   * @addtogroup cmsis-plus-rtos-c
   * @{
   */

  // --------------------------------------------------------------------------
  /**
   * @name Return codes
   * @{
   */

  /**
   * @brief Return codes, in addition to POSIX codes.
   */
  enum
  {
    os_ok = 0, ///< function completed; no error or event occurred.
  };

  /**
   * @}
   */

  // --------------------------------------------------------------------------
  /**
   * @name Main thread function
   * @{
   */

  /**
   * @brief Application entry point, running on the main thread context.
   * @param argc Count of arguments.
   * @param argv Array of string arguments.
   * @return 0 for success, non-zero for error.
   *
   * @details
   * If the application does not define a main() function but defines
   * os_main(),
   * the CMSIS++ RTOS will automatically provide a main() function
   * that starts the main thread and calls os_main() within this context.
   */
  int
  os_main (int argc, char* argv[]);

  /**
   * @}
   */

  // --------------------------------------------------------------------------
  /**
   * @name Scheduler functions
   * @{
   */

  /**
   * @brief Initialise the RTOS scheduler.
   * @par Parameters
   *  None
   * @retval os_ok The scheduler was initialised.
   * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
   */
  os_result_t
  os_sched_initialize (void);

  /**
   * @brief Start the RTOS scheduler.
   * @par Parameters
   *  None
   * @par Returns
   *  Nothing.
   */
  void
  __attribute__((noreturn))
  os_sched_start (void);

  /**
   * @brief Check if the scheduler was started.
   * @par Parameters
   *  None
   * @retval true The scheduler was started.
   * @retval false The scheduler was not started.
   */
  bool
  os_sched_is_started (void);

  /**
   * @brief Lock the scheduler.
   * @param [in] status The new status of the scheduler (true for locked).
   * @return The previous status of the scheduler.
   */
  os_sched_status_t
  os_sched_lock (os_sched_status_t status);

  /**
   * @brief Unlock the scheduler.
   * @param [in] status The new status of the scheduler (false for unlocked).
   * @par Returns
   *  Nothing.
   */
  void
  os_sched_unlock (os_sched_status_t status);

  /**
   * @brief Check if the scheduler is locked.
   * @par Parameters
   *  None
   * @retval true The scheduler is locked.
   * @retval false The scheduler is running (not locked).
   */
  bool
  os_sched_is_locked (void);

  /**
   * @}
   */

  // --------------------------------------------------------------------------
  /**
   * @name Scheduler statistics functions
   * @{
   */

#if defined(OS_INCLUDE_RTOS_STATISTICS_THREAD_CONTEXT_SWITCHES)

  os_statistics_counter_t
  os_sched_stat_get_context_switches (void);

#endif /* defined(OS_INCLUDE_RTOS_STATISTICS_THREAD_CONTEXT_SWITCHES) */

#if defined(OS_INCLUDE_RTOS_STATISTICS_THREAD_CPU_CYCLES)

  os_statistics_duration_t
  os_sched_stat_get_cpu_cycles (void);

#endif /* defined(OS_INCLUDE_RTOS_STATISTICS_THREAD_CPU_CYCLES) */

  /**
   * @}
   */

  // --------------------------------------------------------------------------
  /**
   * @name Interrupts functions
   * @{
   */

  /**
   * @brief Check if the CPU is in handler mode.
   * @par Parameters
   *  None
   * @retval true Execution is in an exception handler context.
   * @retval false Execution is in a thread context.
   */
  bool
  os_irq_in_handler_mode (void);

  /**
   * @brief Enter interrupts critical section.
   * @par Parameters
   *  None
   * @return The current interrupts status register.
   */
  os_irq_status_t
  os_irq_critical_enter (void);

  /**
   * @brief Exit interrupts critical section.
   * @param status The value to restore the interrupts status register.
   * @par Returns
   *  Nothing.
   */
  void
  os_irq_critical_exit (os_irq_status_t status);

  // --------------------------------------------------------------------------

  /**
   * @brief Enter interrupts uncritical section.
   * @par Parameters
   *  None
   * @return The current interrupts status register.
   */
  os_irq_status_t
  os_irq_uncritical_enter (void);

  /**
   * @brief Exit interrupts uncritical section.
   * @param status The value to restore the interrupts status register.
   * @par Returns
   *  Nothing.
   */
  void
  os_irq_uncritical_exit (os_irq_status_t status);

  /**
   * @}
   */

  // --------------------------------------------------------------------------
  /**
   * @name Current thread functions
   * @{
   */

  /**
   * @brief Get the current running thread.
   * @par Parameters
   *  None
   * @return Pointer to the current running thread.
   */
  os_thread_t*
  os_this_thread (void);

  /**
   * @brief Suspend the current running thread to wait for an event.
   * @par Parameters
   *  None
   * @par Returns
   *  Nothing.
   */
  void
  os_this_thread_wait (void);

  /**
   * @brief Terminate the current running thread.
   * @param [in] exit_ptr Pointer to object to return. (Optional).
   * @par Returns
   *  Nothing.
   */
  void
  __attribute__((noreturn))
  os_this_thread_exit (void* exit_ptr);

  /**
   * @brief Wait for signal flags.
   * @param [in] mask The expected flags (OR-ed bit-mask);
   *  may be zero.
   * @param [out] oflags Pointer where to store the current flags;
   *  may be `nullptr`.
   * @param [in] mode Mode bits to select if either all or any flags
   *  are expected, and if the flags should be cleared.
   * @retval os_ok All expected flags are raised.
   * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
   * @retval EINVAL The mask is outside of the permitted range.
   * @retval EINTR The operation was interrupted.
   * @retval ENOTRECOVERABLE Wait failed.
   */
  os_result_t
  os_this_thread_sig_wait (os_flags_mask_t mask, os_flags_mask_t* oflags,
                           os_flags_mode_t mode);

  /**
   * @brief Try to wait for signal flags.
   * @param [in] mask The expected flags (OR-ed bit-mask);
   *  may be zero.
   * @param [out] oflags Pointer where to store the current flags;
   *  may be `nullptr`.
   * @param [in] mode Mode bits to select if either all or any flags
   *  are expected, and if the flags should be cleared.
   * @retval os_ok All expected flags are raised.
   * @retval EINVAL The mask is outside of the permitted range.
   * @retval EWOULDBLOCK The expected condition did not occur.
   * @retval ENOTRECOVERABLE Wait failed.
   */
  os_result_t
  os_this_thread_try_sig_wait (os_flags_mask_t mask, os_flags_mask_t* oflags,
                               os_flags_mode_t mode);

  /**
   * @brief Timed wait for signal flags.
   * @param [in] mask The expected flags (OR-ed bit-mask);
   *  may be zero.
   * @param [out] oflags Pointer where to store the current flags;
   *  may be `nullptr`.
   * @param [in] mode Mode bits to select if either all or any flags
   *  are expected, and if the flags should be cleared.
   * @param [in] timeout Timeout to wait, in clock units (ticks or seconds).
   * @retval os_ok All expected flags are raised.
   * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
   * @retval ETIMEDOUT The expected condition did not occur during the
   *  entire timeout duration.
   * @retval EINVAL The mask is outside of the permitted range.
   * @retval EINTR The operation was interrupted.
   * @retval ENOTRECOVERABLE Wait failed.
   */
  os_result_t
  os_this_thread_timed_sig_wait (os_flags_mask_t mask,
                                 os_clock_duration_t timeout,
                                 os_flags_mask_t* oflags, os_flags_mode_t mode);

  /**
   * @}
   */

  // --------------------------------------------------------------------------
  /**
   * @name Thread functions
   * @{
   */

  void
  os_thread_attr_init (os_thread_attr_t* attr);

  void
  os_thread_create (os_thread_t* thread, const char* name,
                    os_thread_func_t func, const os_thread_func_args_t args,
                    const os_thread_attr_t* attr);

  void
  os_thread_destroy (os_thread_t* thread);

  const char*
  os_thread_get_name (os_thread_t* thread);

  os_thread_prio_t
  os_thread_get_prio (os_thread_t* thread);

  os_result_t
  os_thread_set_prio (os_thread_t* thread, os_thread_prio_t prio);

  /**
   * @brief Wait for thread termination.
   * @param [in] thread Pointer to terminating thread.
   * @param [in] exit_ptr Pointer to object to return. (Optional).
   * @retval os_ok The thread was terminated.
   * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
   */
  os_result_t
  os_thread_join (os_thread_t* thread, void** exit_ptr);

  void
  os_thread_resume (os_thread_t* thread);

  os_result_t
  os_thread_sig_raise (os_thread_t* thread, os_flags_mask_t mask,
                       os_flags_mask_t* oflags);

  os_result_t
  os_thread_sig_clear (os_thread_t* thread, os_flags_mask_t mask,
                       os_flags_mask_t* oflags);

  os_flags_mask_t
  os_thread_sig_get (os_thread_t* thread, os_flags_mask_t mask,
                     os_flags_mode_t mode);

  os_thread_state_t
  os_thread_get_sched_state (os_thread_t* thread);

  os_thread_user_storage_t*
  os_thread_get_user_storage (os_thread_t* thread);

  os_thread_stack_t*
  os_thread_get_stack (os_thread_t* thread);

  /**
   * @}
   */

  // --------------------------------------------------------------------------
  /**
   * @name Thread stack functions
   * @{
   */

  size_t
  os_thread_stack_get_default_size (void);

  size_t
  os_thread_stack_set_default_size (size_t size_bytes);

  size_t
  os_thread_stack_get_min_size (void);

  size_t
  os_thread_stack_set_min_size (size_t size_bytes);

  size_t
  os_thread_stack_get_size (os_thread_stack_t* stack);

  size_t
  os_thread_stack_get_available (os_thread_stack_t* stack);

  /**
   * @}
   */

  // --------------------------------------------------------------------------
  /**
   * @name Thread statistics functions
   * @{
   */

#if defined(OS_INCLUDE_RTOS_STATISTICS_THREAD_CONTEXT_SWITCHES)

  os_statistics_counter_t
  os_thread_stat_get_context_switches (os_thread_t* thread);

#endif /* defined(OS_INCLUDE_RTOS_STATISTICS_THREAD_CONTEXT_SWITCHES) */

#if defined(OS_INCLUDE_RTOS_STATISTICS_THREAD_CPU_CYCLES)

  os_statistics_duration_t
  os_thread_stat_get_cpu_cycles (os_thread_t* thread);

#endif /* defined(OS_INCLUDE_RTOS_STATISTICS_THREAD_CPU_CYCLES) */

  /**
   * @}
   */

  // --------------------------------------------------------------------------
  /**
   * @name Thread children iterator functions
   * @{
   */

  os_iterator_t
  os_children_threads_iter_begin (os_thread_t* thread);

  os_iterator_t
  os_children_threads_iter_end (os_thread_t* thread);

  os_thread_t*
  os_children_threads_iter_get (os_iterator_t iterator);

  os_iterator_t
  os_children_threads_iter_next (os_iterator_t iterator);

  /**
   * @}
   */

  // --------------------------------------------------------------------------
  /**
   * @name Clock functions
   * @{
   */

  const char*
  os_clock_name (os_clock_t* clock);

  os_clock_timestamp_t
  os_clock_now (os_clock_t* clock);

  os_clock_timestamp_t
  os_clock_steady_now (os_clock_t* clock);

  os_result_t
  os_clock_sleep_for (os_clock_t* clock, os_clock_duration_t duration);

  os_result_t
  os_clock_sleep_until (os_clock_t* clock, os_clock_timestamp_t timestamp);

  os_result_t
  os_clock_wait_for (os_clock_t* clock, os_clock_duration_t duration);

  os_clock_offset_t
  os_clock_get_offset (os_clock_t* clock);

  os_clock_offset_t
  os_clock_set_offset (os_clock_t* clock, os_clock_offset_t offset);

  os_clock_t*
  os_clock_get_sysclock (void);

  os_clock_t*
  os_clock_get_rtclock (void);

  // --------------------------------------------------------------------------

  os_clock_timestamp_t
  os_sysclock_now (void);

  os_clock_timestamp_t
  os_sysclock_steady_now (void);

  os_result_t
  os_sysclock_sleep_for (os_clock_duration_t duration);

  os_result_t
  os_sysclock_sleep_until (os_clock_timestamp_t timestamp);

  os_result_t
  os_sysclock_wait_for (os_clock_duration_t duration);

  os_clock_timestamp_t
  os_sysclock_now_details (os_sysclock_current_t* details);

#pragma GCC diagnostic push
#if defined(__cplusplus)
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

  inline os_clock_duration_t
  __attribute__((always_inline))
  os_sysclock_ticks_cast (uint32_t microsec)
  {
    return (os_clock_duration_t) ((((microsec)
        * ((uint32_t) OS_INTEGER_SYSTICK_FREQUENCY_HZ)) + (uint32_t) 1000000ul
        - 1) / (uint32_t) 1000000ul);
  }

  inline os_clock_duration_t
  __attribute__((always_inline))
  os_sysclock_ticks_cast_long (uint64_t microsec)
  {
    return (os_clock_duration_t) ((((microsec)
        * ((uint64_t) OS_INTEGER_SYSTICK_FREQUENCY_HZ)) + (uint64_t) 1000000ul
        - 1) / (uint64_t) 1000000ul);
  }

#pragma GCC diagnostic pop

  /**
   * @}
   */

  // --------------------------------------------------------------------------
  /**
   * @name Timer functions
   * @{
   */

  void
  os_timer_attr_init (os_timer_attr_t* attr);

  void
  os_timer_attr_init_periodic (os_timer_attr_t* attr);

  const os_timer_attr_t*
  os_timer_attr_get_periodic (void);

  void
  os_timer_create (os_timer_t* timer, const char* name, os_timer_func_t func,
                   os_timer_func_args_t args, const os_timer_attr_t* attr);

  void
  os_timer_destroy (os_timer_t* timer);

  const char*
  os_timer_get_name (os_timer_t* timer);

  os_result_t
  os_timer_start (os_timer_t* timer, os_clock_duration_t duration);

  os_result_t
  os_timer_stop (os_timer_t* timer);

  /**
   * @}
   */

  // --------------------------------------------------------------------------
  /**
   * @name Mutex functions
   * @{
   */

  void
  os_mutex_attr_init (os_mutex_attr_t* attr);

  void
  os_mutex_attr_init_recursive (os_mutex_attr_t* attr);

  const os_mutex_attr_t*
  os_mutex_attr_get_recursive (void);

  void
  os_mutex_create (os_mutex_t* mutex, const char* name,
                   const os_mutex_attr_t* attr);

  void
  os_mutex_destroy (os_mutex_t* mutex);

  const char*
  os_mutex_get_name (os_mutex_t* mutex);

  os_result_t
  os_mutex_lock (os_mutex_t* mutex);

  os_result_t
  os_mutex_try_lock (os_mutex_t* mutex);

  os_result_t
  os_mutex_timed_lock (os_mutex_t* mutex, os_clock_duration_t timeout);

  os_result_t
  os_mutex_unlock (os_mutex_t* mutex);

  os_thread_prio_t
  os_mutex_get_prio_ceiling (os_mutex_t* mutex);

  os_result_t
  os_mutex_set_prio_ceiling (os_mutex_t* mutex, os_thread_prio_t prio_ceiling,
                             os_thread_prio_t* old_prio_ceiling);

  os_result_t
  os_mutex_mark_consistent (os_mutex_t* mutex);

  os_thread_t*
  os_mutex_get_owner (os_mutex_t* mutex);

  os_result_t
  os_mutex_reset (os_mutex_t* mutex);

  /**
   * @}
   */

  // --------------------------------------------------------------------------
  /**
   * @name Condition variable functions
   * @{
   */

  void
  os_condvar_attr_init (os_condvar_attr_t* attr);

  void
  os_condvar_create (os_condvar_t* condvar, const char* name,
                     const os_condvar_attr_t* attr);

  void
  os_condvar_destroy (os_condvar_t* condvar);

  const char*
  os_condvar_get_name (os_condvar_t* condvar);

  os_result_t
  os_condvar_signal (os_condvar_t* condvar);

  os_result_t
  os_condvar_broadcast (os_condvar_t* condvar);

  os_result_t
  os_condvar_wait (os_condvar_t* condvar, os_mutex_t* mutex);

  os_result_t
  os_condvar_timed_wait (os_condvar_t* condvar, os_mutex_t* mutex,
                         os_clock_duration_t timeout);

  /**
   * @}
   */

  // --------------------------------------------------------------------------
  /**
   * @name Semaphore functions
   * @{
   */

  void
  os_semaphore_attr_init (os_semaphore_attr_t* attr);

  void
  os_semaphore_attr_init_binary (os_semaphore_attr_t* attr);

  const os_semaphore_attr_t*
  os_semaphore_attr_get_binary (void);

  void
  os_semaphore_create (os_semaphore_t* semaphore, const char* name,
                       const os_semaphore_attr_t* attr);

  void
  os_semaphore_destroy (os_semaphore_t* semaphore);

  const char*
  os_semaphore_get_name (os_semaphore_t* semaphore);

  os_result_t
  os_semaphore_post (os_semaphore_t* semaphore);

  os_result_t
  os_semaphore_wait (os_semaphore_t* semaphore);

  os_result_t
  os_semaphore_try_wait (os_semaphore_t* semaphore);

  os_result_t
  os_semaphore_timed_wait (os_semaphore_t* semaphore,
                           os_clock_duration_t timeout);

  os_semaphore_count_t
  os_semaphore_get_value (os_semaphore_t* semaphore);

  os_result_t
  os_semaphore_reset (os_semaphore_t* semaphore);

  os_semaphore_count_t
  os_semaphore_get_initial_value (os_semaphore_t* semaphore);

  os_semaphore_count_t
  os_semaphore_get_max_value (os_semaphore_t* semaphore);

  /**
   * @}
   */

  // --------------------------------------------------------------------------
  /**
   * @name Memory pool functions
   * @{
   */

  void
  os_mempool_attr_init (os_mempool_attr_t* attr);

  void
  os_mempool_create (os_mempool_t* mempool, const char* name, size_t blocks,
                     size_t block_size_bytes, const os_mempool_attr_t* attr);

  void
  os_mempool_destroy (os_mempool_t* mempool);

  const char*
  os_mempool_get_name (os_mempool_t* mempool);

  void*
  os_mempool_alloc (os_mempool_t* mempool);

  void*
  os_mempool_try_alloc (os_mempool_t* mempool);

  void*
  os_mempool_timed_alloc (os_mempool_t* mempool, os_clock_duration_t timeout);

  os_result_t
  os_mempool_free (os_mempool_t* mempool, void* block);

  size_t
  os_mempool_get_capacity (os_mempool_t* mempool);

  size_t
  os_mempool_get_count (os_mempool_t* mempool);

  size_t
  os_mempool_get_block_size (os_mempool_t* mempool);

  bool
  os_mempool_is_empty (os_mempool_t* mempool);

  bool
  os_mempool_is_full (os_mempool_t* mempool);

  os_result_t
  os_mempool_reset (os_mempool_t* mempool);

  void*
  os_mempool_get_pool (os_mempool_t* mempool);

  /**
   * @}
   */

  // --------------------------------------------------------------------------
  /**
   * @name Message queue functions
   * @{
   */

  void
  os_mqueue_attr_init (os_mqueue_attr_t* attr);

  void
  os_mqueue_create (os_mqueue_t* mqueue, const char* name, size_t msgs,
                    size_t msg_size_bytes, const os_mqueue_attr_t* attr);

  void
  os_mqueue_destroy (os_mqueue_t* mqueue);

  const char*
  os_mqueue_get_name (os_mqueue_t* mqueue);

  os_result_t
  os_mqueue_send (os_mqueue_t* mqueue, const void* msg, size_t nbytes,
                  os_mqueue_prio_t mprio);

  os_result_t
  os_mqueue_try_send (os_mqueue_t* mqueue, const void* msg, size_t nbytes,
                      os_mqueue_prio_t mprio);

  os_result_t
  os_mqueue_timed_send (os_mqueue_t* mqueue, const void* msg, size_t nbytes,
                        os_clock_duration_t timeout, os_mqueue_prio_t mprio);

  os_result_t
  os_mqueue_receive (os_mqueue_t* mqueue, void* msg, size_t nbytes,
                     os_mqueue_prio_t* mprio);

  os_result_t
  os_mqueue_try_receive (os_mqueue_t* mqueue, void* msg, size_t nbytes,
                         os_mqueue_prio_t* mprio);

  os_result_t
  os_mqueue_timed_receive (os_mqueue_t* mqueue, void* msg, size_t nbytes,
                           os_clock_duration_t timeout,
                           os_mqueue_prio_t* mprio);

  size_t
  os_mqueue_get_capacity (os_mqueue_t* mqueue);

  size_t
  os_mqueue_get_length (os_mqueue_t* mqueue);

  size_t
  os_mqueue_get_msg_size (os_mqueue_t* mqueue);

  bool
  os_mqueue_is_empty (os_mqueue_t* mqueue);

  bool
  os_mqueue_is_full (os_mqueue_t* mqueue);

  os_result_t
  os_mqueue_reset (os_mqueue_t* mqueue);

  /**
   * @}
   */

  // --------------------------------------------------------------------------
  /**
   * @name Event flags functions
   * @{
   */

  void
  os_evflags_attr_init (os_evflags_attr_t* attr);

  void
  os_evflags_create (os_evflags_t* evflags, const char* name,
                     const os_evflags_attr_t* attr);

  void
  os_evflags_destroy (os_evflags_t* evflags);

  const char*
  os_evflags_get_name (os_evflags_t* evflags);

  os_result_t
  os_evflags_wait (os_evflags_t* evflags, os_flags_mask_t mask,
                   os_flags_mask_t* oflags, os_flags_mode_t mode);

  os_result_t
  os_evflags_try_wait (os_evflags_t* evflags, os_flags_mask_t mask,
                       os_flags_mask_t* oflags, os_flags_mode_t mode);

  os_result_t
  os_evflags_timed_wait (os_evflags_t* evflags, os_flags_mask_t mask,
                         os_clock_duration_t timeout, os_flags_mask_t* oflags,
                         os_flags_mode_t mode);

  os_result_t
  os_evflags_raise (os_evflags_t* evflags, os_flags_mask_t mask,
                    os_flags_mask_t* oflags);

  os_result_t
  os_evflags_clear (os_evflags_t* evflags, os_flags_mask_t mask,
                    os_flags_mask_t* oflags);

  os_flags_mask_t
  os_evflags_get (os_evflags_t* evflags, os_flags_mask_t mask,
                  os_flags_mode_t mode);

  bool
  os_evflags_are_waiting (os_evflags_t* evflags);

  /**
   * @}
   */

  // --------------------------------------------------------------------------
  /**
   * @name Clock handlers
   * @{
   */

  // Internal functions, to be defined by the user.
  // (not to be called directly).
  void
  os_systick_handler (void);

  void
  os_rtc_handler (void);

/**
 * @}
 */

// --------------------------------------------------------------------------
/**
 * @}
 */

#ifdef  __cplusplus
}
#endif

// ----------------------------------------------------------------------------

#endif /* CMSIS_PLUS_RTOS_OS_C_API_H_ */
