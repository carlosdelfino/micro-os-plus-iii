/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2016 Liviu Ionescu.
 * Copyright (c) 2013 ARM LIMITED
 *
 * µOS++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, version 3.
 *
 * µOS++ is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file os.h
 * @brief CMSIS++ RTOS definitions
 * @details
 *
 * This file is part of the CMSIS++ proposal, intended as a CMSIS
 * replacement for C++ applications.
 *
 * The code is inspired by ARM CMSIS `<cmsis_os.h>` file, v1.02,
 * and tries to remain functionally close to the CMSIS specifications.
 *
 * References are to C++ Standard ISO/IEC 14882:2011(E)
 * Third edition (2011-09-01).
 *
 * Major improvements:
 * - no more macros required to define objects
 * - allow static memory allocations for all objects
 * - very close to POSIX ([IEEE Std 1003.1, 2013 Edition](http://pubs.opengroup.org/onlinepubs/9699919799/nframe.html))
 * - specifically designed to facilitate the implementation of
 *   C++ standard thread library (ISO/IEC 14882:2011)
 * - standard POSIX errors definitions used
 * - improved usability, by defining both simple (using defaults)
 *   and complex (using attributes) object constructors (feature
 *   inspired by POSIX threads attributes)
 * - improved readability with explicit three-fold waiting functions
 *   (for example: lock(), try-lock(), timed-lock(), similar to POSIX threads)
 * - POSIX condition variable added
 * - versatile clocks added (Systick_clock, Realtime_clock)
 */

/*
 * TODO:
 * - make Thread virtual, to allow create_hook/delete_hook functionality
 * - make most classes virtual, to allow post_hook functionality
 * - event timestamps: add derived classes that capture the event timestamp
 * - add object type in base class
 * - add Wait_list in base class
 * - add libc/newlib errno() function
 * - add a separate Stack object
 *
 * Notes:
 * - try_wait(), try_sig_wait() are probably not very inspired.
 */

#ifndef CMSIS_PLUS_RTOS_OS_H_
#define CMSIS_PLUS_RTOS_OS_H_

// ----------------------------------------------------------------------------

#if defined(__cplusplus)

/**
 * @brief Inform that CMSIS++ is in use.
 * @details
 * Macro to inform including files that CMSIS++ RTOS
 * definitions are available.
 */
#define OS_USE_CMSIS_PLUS

// Include the CMSIS++ OS implementation declarations. This might further
// include os-config.h, for the application specific definitions.
#include <cmsis-plus/rtos/port/os-decls.h>

#include <cmsis-plus/iso/system_error>

#include <cstdint>
#include <cstddef>
#include <cerrno>

// ----------------------------------------------------------------------------

/**
 * @namespace os
 * @brief System namespace.
 */
namespace os
{
  /**
   * @brief RTOS namespace.
   * @details
   * The `os::rtos` namespace groups all RTOS specific declarations,
   * either directly or via nested namespaces.
   */
  namespace rtos
  {
    // ------------------------------------------------------------------------

    /**
     * @brief Type of values returned by RTOS functions.
     * @details
     * For error processing reasons, most CMSIS++ RTOS functions
     * return a numeric result, which, according to POSIX,
     * when the call was successful, must be `0`
     * (`result::ok`) or an error code defined in `<errno.h>` otherwise.
     */
    using result_t = uint32_t;

    /**
     * @namespace os::rtos::result
     * @brief Values returned by RTOS functions.
     * @details
     * This namespace is dedicated to grouping all
     * status code values returned by CMSIS++ RTOS functions.
     *
     * However, CMSIS++ favours POSIX error codes, so,
     * except a few enumerated values, most of them are exactly those
     * defined by POSIX, in the
     * `<errno.h>` header, and are not redefined here.
     *
     * Currently in use are:
     * - `EPERM` - Operation not permitted. An attempt was made to perform
     * an operation limited to processes with appropriate privileges
     * or to the owner of a file or other resource. In CMSIS++ this
     * usually means that the call is not available in handler mode.
     * - `EINVAL` - Invalid argument. Some invalid argument was supplied;
     * - `EAGAIN` - Resource temporarily unavailable. This is a temporary
     * condition and later calls to the same routine may complete normally.
     * In CMSIS++ case, this usually means that a call to `try_xxx()`
     * found the resource busy.
     * - `ENOTRECOVERABLE` - State not recoverable. In CMSIS++ this
     * usually means an unrecoverable error occurred.
     * - `EDEADLOCK` - Resource deadlock would occur. An attempt was made
     * to lock a system resource that would have resulted in a deadlock
     * situation.
     * - `EMSGSIZE` - Message too large. A message sent on a transport
     * provider was larger than an internal message buffer or some other
     * network limit, or inappropriate message buffer length.
     * - `EBADMSG` - Bad message. The implementation has detected a
     * corrupted message.
     * - `EINTR` - Interrupted function call. In CMSIS++ this
     * usually mens that a thread waiting for a message is waken
     * before the event or the timeout occurred, at user request.
     * - `ETIMEDOUT` - Operation timed out. The time limit associated
     * with the operation was exceeded before the operation completed.
     * - `EOWNERDEAD` - Previous owner died. The owner of a robust mutex
     * terminated while holding the mutex lock.
     *
     * @par Example
     *
     * @code{.cpp}
     * void
     * func (void)
     * {
     *    Mutex mx;
     *    ...
     *    result_t res = mx.try_lock();
     *    if (res == result::ok)
     *      {
     *        // All is well, mutex locked.
     *      }
     *    else if (res == EAGAIN)
     *      {
     *        // Mutex busy, try again later.
     *      }
     * }
     * @endcode
     *
     */
    namespace result
    {
      /**
       * @brief Custom enumerated values.
       * @details
       * There are not many custom values returned by
       * CMSIS++ RTOS functions, currently there is only one,
       * `ok`, represented by `0`.
       *
       * If more custom codes will be needed and are not present in POSIX,
       * this is the place where to add them. Just be sure their numeric
       * values do not overlap POSIX values (check for a definition with
       * the last allocated POSIX error number).
       */
      enum
        : result_t
          {
            /**
             * @brief Function completed; no errors or events occurred.
             */
            ok = 0,

      };
    } /* namespace result */

    // ------------------------------------------------------------------------

    /**
     * @brief Type of variables holding timer ticks.
     * @details
     * A numeric type intended to hold a number of SysTick ticks.
     */
    using systicks_t = uint32_t;

    /**
     * @brief Type of variables holding timer durations.
     * @details
     * A numeric type intended to hold a generic duration, either in ticks
     * or in seconds.
     */
    using duration_t = uint32_t;

    // ------------------------------------------------------------------------

    /**
     * @namespace os::rtos::scheduler
     * @brief Scheduler namespace.
     * @details
     * The `os::rtos::scheduler` namespace groups scheduler types
     * and functions.
     */
    namespace scheduler
    {
      /**
       * @brief Type of a variable holding scheduler status codes.
       * @details
       * Usually a boolean telling if the scheduler is
       * locked or not, but for recursive locks it might also be a
       * numeric counter.
       */
      using status_t = bool;

      /**
       * @brief The scheduler status.
       * @details
       * Modified by @ref `lock()` and restored to previous value
       * by @ref `unlock()`.
       */
      extern status_t is_locked_;

      /**
       * @brief Variable set to true after the scheduler is started.
       * @details
       * No further changes allowed, the scheduler cannot be stopped,
       * in can be only locked.
       */
      extern bool is_started_;

      /**
       * @brief Initialise the RTOS scheduler.
       * @par Parameters
       *  None
       * @retval result::ok The scheduler was initialised.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       */
      result_t
      initialize (void);

      /**
       * @brief Start the RTOS scheduler.
       * @par Parameters
       *  None
       * @retval result::ok The scheduler was started.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       */
      result_t
      start (void);

      /**
       * @brief Check if the scheduler was started.
       * @par Parameters
       *  None
       * @retval true The scheduler was started.
       * @retval false The scheduler was not started.
       */
      bool
      started (void);

      /**
       * @brief Check if the scheduler is locked.
       * @par Parameters
       *  None
       * @retval true The scheduler is locked.
       * @retval false The scheduler is running (not locked).
       */
      bool
      locked (void);

      /**
       * @brief Lock the scheduler.
       * @par Parameters
       *  None
       * @return The previous status of the scheduler.
       */
      status_t
      lock (void);

      /**
       * @brief Unlock the scheduler.
       * @param [in] status The new status of the scheduler.
       * @return  Nothing.
       */
      void
      unlock (status_t status);

      /**
       * @brief Check if the CPU is in handler mode.
       * @par Parameters
       *  None
       * @retval true Execution is in an exception handler context.
       * @retval false Execution is in a thread context.
       */
      bool
      in_handler_mode (void);

      // ----------------------------------------------------------------------

      /**
       * @class Critical_section
       * @brief Scheduler critical section [RAII](https://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) helper.
       *
       * @details
       * Use this class to define a critical section
       * protected to scheduler switches. The beginning of the
       * critical section is exactly the place where this class is
       * instantiated (the constructor will lock
       * the scheduler). The end of the critical
       * section is the end of the surrounding block (the destructor will
       * unlock the scheduler).
       *
       * @note Can be nested as many times as required without problems,
       * only the outer call will unlock the scheduler.
       *
       * @par Example
       *
       * @code{.cpp}
       * void
       * func(void)
       * {
       *    // Do something
       *
       *    {
       *      scheduler::Critical_section cs;  // Critical section begins here.
       *
       *      // Inside the critical section.
       *      // No scheduler switches will happen here.
       *
       *    } // Critical section ends here.
       *
       *    // Do something else.
       * }
       * @endcode
       *
       * @headerfile os.h <cmsis-plus/rtos/os.h>
       */
      class Critical_section
      {
      public:

        /**
         * @name Constructors & Destructor
         * @{
         */

        /**
         * @brief Enter a critical section.
         * @par Parameters
         *  None
         */
        Critical_section ();

        /**
         * @cond ignore
         */
        Critical_section (const Critical_section&) = delete;
        Critical_section (Critical_section&&) = delete;
        Critical_section&
        operator= (const Critical_section&) = delete;
        Critical_section&
        operator= (Critical_section&&) = delete;
        /**
         * @endcond
         */

        /**
         * @brief Exit a critical section.
         */
        ~Critical_section ();

        /**
         * @}
         */
      protected:

        /**
         * @name Private Member Variables
         * @{
         */

        /**
         * @brief Variable to store the initial scheduler status.
         * @details
         * The variable is constant, after being set by the constructor no
         * further changes are possible.
         *
         * The variable type usually is a `bool`, but a counter is also
         * possible if the scheduler uses a recursive lock.
         */
        const status_t status_;

        /**
         * @}
         */
      };

      /**
       * @class Lock
       * @brief Scheduler standard locker.
       * @details
       * Locker meeting the standard `Lockable` requirements (30.2.5.3).
       *
       * @headerfile os.h <cmsis-plus/rtos/os.h>
       */
      class Lock
      {
      public:

        /**
         * @name Constructors & Destructor
         * @{
         */

        /**
         * @brief Create a lock.
         * @par Parameters
         *  None
         */
        constexpr
        Lock ();

        /**
         * @brief Destroy the lock.
         */
        ~Lock ();

        /**
         * @cond ignore
         */
        Lock (const Lock&) = delete;
        Lock (Lock&&) = delete;
        Lock&
        operator= (const Lock&) = delete;
        Lock&
        operator= (Lock&&) = delete;
        /**
         * @endcond
         */

        /**
         * @}
         * @name Public Member Functions
         * @{
         */

        /**
         * @brief Lock the scheduler.
         * @par Parameters
         *  None
         * @return  Nothing.
         */
        void
        lock (void);

        /**
         * @brief Try to lock the scheduler.
         * @details
         * Somehow redundant, since the lock will always succeed;
         * but used to meet the Lockable requirements.
         *
         * @par Parameters
         *  None
         * @retval true The scheduler was locked.
         */
        bool
        try_lock (void);

        /**
         * @brief Unlock the scheduler.
         * @par Parameters
         *  None
         * @return  Nothing.
         */
        void
        unlock (void);

        /**
         * @}
         */

      protected:

        /**
         * @name Private Member Variables
         * @{
         */

        /**
         * @brief Variable to store the initial scheduler status.
         * @details
         * The variable type usually is a `bool`, but a counter is also
         * possible if the scheduler uses a recursive lock.
         */
        status_t status_ = 0;

        /**
         * @}
         */
      };

    // ------------------------------------------------------------------------
    } /* namespace scheduler */

    /**
     * @namespace os::rtos::interrupts
     * @brief Interrupts namespace.
     * @details
     * The os::rtos::interrupts namespace groups interrupts related
     *  types and enumerations.
     */
    namespace interrupts
    {
      /**
       * @brief Type of a variable holding interrupts status.
       * @details
       * Usually an integer large enough to hold the CPU status register
       * where the interrupt status is stored.
       *
       * It is used to temporarily store the CPU status register
       * during critical sections.
       */
      using status_t = uint32_t;

      // TODO: define all levels of critical sections
      // (kernel, real-time(level), complete)

      // TODO: make template, parameter IRQ level

      /**
       * @class Critical_section
       * @brief Interrupts critical section [RAII](https://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) helper.
       *
       * @details
       * Use this class to define a critical section
       * protected to interrupts service routines. The begining of the
       * critical section is exactly the place where this class is
       * instantiated (the constructor will disable interrupts below
       * the scheduler priority). The end of the critical
       * section is the end of the surrounding block (the destructor will
       * enable the interrupts).
       *
       * @note Can be nested as many times as required without problems,
       * only the outer call will re-enable the interrupts.
       *
       * @par Example
       *
       * @code{.cpp}
       * void
       * func(void)
       * {
       *    // Do something
       *
       *    {
       *      interrupts::Critical_section cs;  // Critical section begins here.
       *
       *      // Inside the critical section.
       *      // No scheduler switches will happen here.
       *
       *    } // Critical section ends here.
       *
       *    // Do something else.
       * }
       * @endcode
       *
       * @headerfile os.h <cmsis-plus/rtos/os.h>
       */
      class Critical_section
      {
      public:

        /**
         * @name Constructors & Destructor
         * @{
         */

        /**
         * @brief Enter interrupts critical section.
         * @par Parameters
         *  None
         */
        Critical_section ();

        /**
         * @cond ignore
         */
        Critical_section (const Critical_section&) = delete;
        Critical_section (Critical_section&&) = delete;
        Critical_section&
        operator= (const Critical_section&) = delete;
        Critical_section&
        operator= (Critical_section&&) = delete;
        /**
         * @endcond
         */

        /**
         * @brief Exit interrupts critical section.
         */
        ~Critical_section ();

        /**
         * @}
         * @name Public Member Functions
         * @{
         */

        /**
         * @brief Enter interrupts critical section.
         * @par Parameters
         *  None
         * @return The current interrupts status register.
         */
        static status_t
        enter (void);

        /**
         * @brief Exit interrupts critical section.
         * @param status The value to restore the interrupts status register.
         * @return  Nothing.
         */
        static void
        exit (status_t status);

        /**
         * @}
         */

      protected:

        /**
         * @name Private Member Variables
         * @{
         */

        /**
         * @brief Variable to store the initial interrupts status.
         * @details
         * The variable is constant, after being set by the constructor no
         * further changes are possible.
         *
         * The variable type usually is an unsigned integer where
         * the status register is saved.
         */
        const status_t status_;

        /**
         * @}
         */
      };

      /**
       * @class Lock
       * @brief Interrupts standard locker.
       * @details Locker meeting the standard `Lockable` requirements (30.2.5.3).
       *
       * @headerfile os.h <cmsis-plus/rtos/os.h>
       */
      class Lock
      {
      public:

        /**
         * @name Constructors & Destructor
         * @{
         */

        /**
         * @brief Create an interrupts lock.
         * @par Parameters
         *  None
         */
        constexpr
        Lock ();

        /**
         * @brief Destroy the interrupts lock.
         */
        ~Lock ();

        /**
         * @cond ignore
         */
        Lock (const Lock&) = delete;
        Lock (Lock&&) = delete;
        Lock&
        operator= (const Lock&) = delete;
        Lock&
        operator= (Lock&&) = delete;
        /**
         * @endcond
         */

        /**
         * @}
         * @name Public Member Functions
         * @{
         */

        /**
         * @brief Lock the interrupts.
         * @par Parameters
         *  None
         * @return  Nothing.
         */
        void
        lock (void);

        /**
         * @brief Try to lock the interrupts.
         * @details
         * Somehow redundant, since the lock will always succeed;
         * but used to meet the Lockable requirements.
         * @par Parameters
         *  None
         * @retval true The interrupts were locked.
         */
        bool
        try_lock (void);

        /**
         * @brief Unlock the interrupts.
         * @par Parameters
         *  None
         * @return  Nothing.
         */
        void
        unlock (void);

        /**
         * @}
         */

      protected:

        /**
         * @name Private Member Variables
         * @{
         */

        /**
         * @brief Variable to store the initial interrupts status.
         * @details
         * The variable type usually is an unsigned integer where
         * the status register is saved.
         */
        status_t status_;

        /**
         * @}
         */

      };

    } /* namespace interrupts */

    // ----------------------------------------------------------------------

    // Forward reference to Thread.
    class Thread;

    /**
     * @namespace os::rtos::flags
     * @brief Generic flags namespace.
     * @details
     * The os::rtos::flags namespace groups event types and enumerations.
     */
    namespace flags
    {
      /**
       * @brief Type of a variable holding a flags mask.
       * @details
       * An unsigned type large enough to store all the flags, usually
       * 32-bits wide.
       *
       * Both thread signal flags and event flags use this definition.
       */
      using mask_t = uint32_t;
      /**
       * @brief Type of a variable holding the flags mode.
       * @details
       * An unsigned type used to hold the mode bits passed to
       * functions returning flags.
       *
       * Both thread signal flags and event flags use this definition.
       */
      using mode_t = uint32_t;

      /**
       * @namespace os::rtos::flags::mode
       * @brief Flags modes.
       * @details
       * Container for generic flags enumerations.
       */
      namespace mode
      {
        enum
          : mode_t
            {
              /**
               * @brief Return when all flags are set.
               */
              all = 1,

              /**
               * @brief Return when at least one flag is set.
               */
              any = 2,

              /**
               * @brief Ask for flags to be cleared after read.
               */
              clear = 4
        };
      } /* namespace mode */
    } /* namespace flags */

    /**
     * @namespace os::rtos::thread
     * @brief Thread namespace.
     * @details
     * The os::rtos::thread namespace groups thread types, enumerations,
     * attributes and initialisers.
     */
    namespace thread
    {
      /**
       * @brief Type of a variable holding thread priorities.
       * @details
       * A numeric type used to hold thread priorities, affecting the thread
       * behaviour, like scheduling and thread wakeup due to events;
       * usually an unsigned 8-bits type.
       *
       * Higher values represent higher priorities.
       */
      using priority_t = uint8_t;

      /**
       * @namespace os::rtos::thread::priority
       * @brief Thread priority namespace.
       * @details
       * The os::rtos::thread::priority namespace is a container for
       * priorities not restricted to an enumeration.
       */
      namespace priority
      {
        /**
         * @brief Priorities pre-scaler.
         * @details
         * Increasing this value widens the range of allowed
         * priorities. It is recommended to keep it low to give the
         * scheduler a chance to optimise accesses to the ready list
         * with an array of priorities, which will require some
         * pointers and counters for each priority level.
         *
         * The default value of 0 gives 16 priorities; increasing it to
         * 1 gives 32 priorities, 2 gives 64 priorities, 3 gives 128
         * priorities.
         */
        constexpr uint32_t shift = 0;

        enum
          : priority_t
            {
              /**
               * Undefined, thread not initialised.
               */
              none = 0,

              /**
               * System reserved for IDLE thread.
               */
              idle = 1,

              /**
               * Lowest available for user code.
               */
              lowest = 2,

              low = (2 << shift),

              below_normal = (4 << shift),

              /**
               * Default priority.
               */
              normal = (6 << shift),

              above_normal = (8 << shift),

              high = (10 << shift),

              realtime = (12 << shift),

              /**
               * Highest available for user code.
               */
              highest = ((16 << shift) - 3),

              /**
               * System reserved for ISR deferred thread.
               */
              isr = ((16 << shift) - 2),

              /**
               * Error.
               */
              error = ((16 << shift) - 1)
        };
      } /* namespace priority */

      /**
       * @brief Type of a variable holding the thread state.
       * @details
       * An enumeration with the possible thread states. The enumeration
       * is restricted to one of these values.
       */
      using state_t = enum class state : uint8_t
        {
          /**
           * @brief Used to catch uninitialised threads.
           */
          undefined = 0,
          inactive = 1,
          ready = 2,
          running = 3,
          waiting = 4,
          /**
           * @brief Reuse possible if terminated or higher.
           */
          terminated = 5,      // Test for here up for reuse
          destroyed = 6
        };

      /**
       * @brief Type of a variable holding a signal set.
       * @details
       * An unsigned type large enough to store all the signal flags,
       * actually a reuse of the more generic flags mask type
       * @ref flags::mask_t.
       */
      using sigset_t = flags::mask_t;

      /**
       * @namespace os::rtos::thread::sig
       * @brief Thread signals namespace.
       * @details
       * The os::rtos::thread::sig namespace is a container for
       * signal flags masks, which cannot be restricted to an enumeration..
       */
      namespace sig
      {
        enum
          : sigset_t
            {
              /**
               * @brief Special signal mask to represent any flag.
               */
              any = 0,
              /**
               * Special signal mask to represent all flags.
               */
              all = 0xFFFFFFFF,
        };
      } /* namespace sig */

      /**
       * @brief Thread function arguments.
       * @details
       * Type of thread function arguments.
       */
      using func_args_t = void*;

      /**
       * @brief Thread function.
       * @details
       * Type of thread functions. Useful to cast other similar types
       * to silence the compiler warnings.
       */
      using func_t = void* (*) (func_args_t args);

    } /* namespace thread */

    /**
     * @namespace os::rtos::stack
     * @brief Stack namespace.
     * @details
     * The os::rtos::stack namespace groups declarations related to
     * the thread stack.
     */
    namespace stack
    {
      /**
       * @brief Type of a stack element.
       * @details
       * For alignment reasons, the stack is allocated in
       * larger chunks, usually 8-bytes long on Cortex-M cores.
       */
      using element_t = os::rtos::port::stack::element_t;

    } /* namespace stack */

    /**
     * @namespace os::rtos::this_thread
     * @brief The current running thread namespace.
     * @details
     * The os::rtos::this_thread namespace groups functions related to
     * the current thread.
     */
    namespace this_thread
    {
      /**
       * @brief Get the current running thread.
       * @par Parameters
       *  None
       * @return Reference to the current running thread.
       */
      Thread&
      thread (void);

      /**
       * @brief Yield execution to the next thread ready thread.
       * @par Parameters
       *  None
       * @return Nothing.
       */
      void
      yield (void);

      /**
       * @brief Suspend the current running thread.
       * @par Parameters
       *  None
       * @return Nothing.
       */
      void
      suspend (void);

      /**
       * @brief Terminate the current running thread.
       * @param [in] exit_ptr Pointer to object to return. (Optional).
       * @return Nothing.
       */
      void
      exit (void* exit_ptr = nullptr);

      /**
       * @brief Check if the wake-up is due to a timeout.
       * @par Parameters
       *  None
       * @retval true The previous sleep returned after the entire duration.
       * @retval false The previous sleep returned due to an event.
       */
      bool
      is_timeout (void);

      /**
       * @brief Wait for signal flags.
       * @param [in] mask The expected flags (OR-ed bit-mask);
       *  may be zero.
       * @param [out] oflags Pointer where to store the current flags;
       *  may be `nullptr`.
       * @param [in] mode Mode bits to select if either all or any flags
       *  are expected, and if the flags should be cleared.
       * @retval result::ok All expected flags are raised.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval EINVAL The mask is outside of the permitted range.
       * @retval EINTR The operation was interrupted.
       * @retval ENOTRECOVERABLE Wait failed.
       */
      result_t
      sig_wait (thread::sigset_t mask, thread::sigset_t* oflags,
                flags::mode_t mode);

      /**
       * @brief Try to wait for signal flags.
       * @param [in] mask The expected flags (OR-ed bit-mask);
       *  may be zero.
       * @param [out] oflags Pointer where to store the current flags;
       *  may be `nullptr`.
       * @param [in] mode Mode bits to select if either all or any flags
       *  are expected, and if the flags should be cleared.
       * @retval result::ok All expected flags are raised.
       * @retval EINVAL The mask is outside of the permitted range.
       * @retval EAGAIN The expected condition did not occur.
       * @retval ENOTRECOVERABLE Wait failed.
       */
      result_t
      try_sig_wait (thread::sigset_t mask, thread::sigset_t* oflags,
                    flags::mode_t mode);

      /**
       * @brief Timed wait for signal flags.
       * @param [in] mask The expected flags (OR-ed bit-mask);
       *  may be zero.
       * @param [out] oflags Pointer where to store the current flags;
       *  may be `nullptr`.
       * @param [in] mode Mode bits to select if either all or any flags
       *  are expected, and if the flags should be cleared.
       * @param ticks The number of ticks to wait.
       * @retval result::ok All expected flags are raised.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval ETIMEDOUT The expected condition did not occur during the
       *  entire timeout duration.
       * @retval EINVAL The mask is outside of the permitted range.
       * @retval EINTR The operation was interrupted.
       * @retval ENOTRECOVERABLE Wait failed.
       */
      result_t
      timed_sig_wait (thread::sigset_t mask, thread::sigset_t* oflags,
                      flags::mode_t mode, systicks_t ticks);

    } /* namespace this_thread */

    // ========================================================================

    /**
     * @class Named_object
     * @brief Base class for named objects.
     *
     * @details
     * This class serves as a base class for all objects that have a
     * name (most of the RTOS classes do have a name).
     *
     * @headerfile os.h <cmsis-plus/rtos/os.h>
     */
    class Named_object
    {
    public:

      /**
       * @name Constructors & Destructor
       * @{
       */

      /**
       * @brief Create a named object.
       * @param [in] name Null terminated name. If `nullptr`, "-" is assigned.
       */
      Named_object (const char* name);

      /**
       * @cond ignore
       */
      Named_object (const Named_object&) = default;
      Named_object (Named_object&&) = default;
      Named_object&
      operator= (const Named_object&) = default;
      Named_object&
      operator= (Named_object&&) = default;
      /**
       * @endcond
       */

      /**
       * @brief Destroy the named object.
       */
      ~Named_object () = default;

      /**
       * @}
       * @name Public Member Functions
       * @{
       */

      /**
       * @brief Get name.
       * @par Parameters
       *  None.
       * @return A null terminated string.
       */
      const char*
      name (void) const;

      /**
       * @}
       */

    protected:

      /**
       * @name Private Member Variables
       * @{
       */

      /**
       * @brief Pointer to name.
       * @details
       * To save space, the null terminated string passed to the
       * constructor is not copied locally. Instead, the pointer to
       * the string is copied, so the
       * caller must ensure that the pointer life cycle
       * is at least as long as the object life cycle. A constant
       * string (stored in flash) is preferred.
       */
      const char* const name_;

      /**
       * @}
       */

    };

    // ========================================================================

    namespace thread
    {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

      /**
       * @class Attributes
       * @brief Thread attributes.
       * @details
       * Allow to assign a name and custom attributes (like stack address,
       * stack size, priority) to the thread.
       *
       * To simplify access, the member variables are public and do not
       * require accessors or mutators.
       *
       * @par POSIX compatibility
       *  Inspired by `pthread_attr_t` from [<pthread.h>](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html)
       *  (IEEE Std 1003.1, 2013 Edition).
       *
       * @headerfile os.h <cmsis-plus/rtos/os.h>
       */
      class Attributes : public Named_object
      {
      public:

        /**
         * @name Constructors & Destructor
         * @{
         */

        /**
         * @brief Create thread attributes.
         * @param [in] name Null terminated name. If `nullptr`, "-" is assigned.
         */
        Attributes (const char* name);

        /**
         * @cond ignore
         */
        Attributes (const Attributes&) = default;
        Attributes (Attributes&&) = default;
        Attributes&
        operator= (const Attributes&) = default;
        Attributes&
        operator= (Attributes&&) = default;
        /**
         * @endcond
         */

        /**
         * @brief Destroy thread attributes.
         */
        ~Attributes () = default;

        /**
         * @}
         */

      public:

        /**
         * @name Public Member Variables
         * @{
         */

        // Public members, no accessors and mutators required.
        // Warning: must match the type & order of the C file header.
        /**
         * @brief Thread user stack address attribute.
         */
        void* th_stack_address;

        /**
         * @brief Thread user stack size attribute.
         */
        std::size_t th_stack_size_bytes;

        /**
         * @brief Thread priority attribute.
         */
        priority_t th_priority;

        // Add more attributes.

        /**
         * @}
         */

      };

#pragma GCC diagnostic pop

      /**
       * @brief Default thread initialiser.
       */
      extern const Attributes initializer;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

#if !defined(OS_INCLUDE_RTOS_CUSTOM_THREAD_USER_STORAGE)
    /**
     * @brief Default empty thread user storage.
     *
     * If the application requires to store some additional data
     * to each thread, redefine this structure in the `<os-app-conf.h>`
     * file.
     *
     * To get the address of the user storage associated with a
     * given thread, use @ref Thread::user_storage().
     */
    typedef struct
      {
        ;
      }os_thread_user_storage_t;
#endif

    /**
     * @class Thread
     * @brief POSIX compliant thread.
     * @details
     * Supports terminating functions and a simplified version of
     * signal flags.
     *
     * @par POSIX compatibility
     *  Inspired by `pthread_t` from [<pthread.h>](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html)
     *  (IEEE Std 1003.1, 2013 Edition).
     *
     * @headerfile os.h <cmsis-plus/rtos/os.h>
     */
    class Thread : public Named_object
    {
    public:

      /**
       * @name Constructors & Destructor
       * @{
       */

      /**
       * @brief Create a thread with default settings.
       * @param [in] function Pointer to thread function.
       * @param [in] args Pointer to arguments.
       */
      Thread (thread::func_t function, thread::func_args_t args);

      /**
       * @brief Create a thread with custom settings.
       * @param [in] attr Reference to attributes.
       * @param [in] function Pointer to thread function.
       * @param [in] args Pointer to arguments.
       */
      Thread (const thread::Attributes& attr, thread::func_t function,
              thread::func_args_t args);

      /**
       * @cond ignore
       */
      Thread (const Thread&) = delete;
      Thread (Thread&&) = delete;
      Thread&
      operator= (const Thread&) = delete;
      Thread&
      operator= (Thread&&) = delete;
      /**
       * @endcond
       */

      /**
       * @brief Destroy the thread.
       */
      ~Thread ();

      /**
       * @}
       * @name Operators
       * @{
       */

      /**
       * @brief Compare threads.
       * @retval true The given thread is the same as this thread.
       * @retval false The threads are different.
       */
      bool
      operator== (const Thread& rhs) const;

      /**
       * @}
       * @name Public Member Functions
       * @{
       */

      /**
       * @brief Cancel thread execution.
       * @par Parameters
       *  None.
       * @retval result::ok The cancel request was sent to the thread.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       */
      result_t
      cancel (void);

      /**
       * @brief Wait for thread termination.
       * @retval result::ok The thread was terminated.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       */
      result_t
      join (void** exit_ptr = nullptr);

      /**
       * @brief Detach a thread.
       * @par Parameters
       *  None
       * @retval result::ok The thread was detached.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       */
      result_t
      detach (void);

      // Accessors & mutators.

      /**
       * @brief Set dynamic scheduling priority.
       * @retval result::ok The priority was set.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval EINVAL The value of prio is invalid for the
       *  scheduling policy of the specified thread.
       */
      result_t
      sched_prio (thread::priority_t prio);

      /**
       * @brief Get the current scheduling priority.
       * @par Parameters
       *  None.
       * @return The thread priority.
       */
      thread::priority_t
      sched_prio (void);

#if 0
      // ???
      result_t
      set_cancel_state (int, int*);
      result_t
      set_cancel_type (int, int*);

      result_t
      get_sched_param (int*, struct sched_param*);
      result_t
      set_sched_param (int, const struct sched_param*);

      //void test_cancel(void);
#endif

      // TODO: study how to integrate signals and POSIX cancellation.
      /**
       * @brief Check if interrupted.
       * @par Parameters
       *  None
       * @retval true The thread was interrupted.
       * @retval false The thread was not interrupted.
       */
      bool
      interrupted (void);

      /**
       * @brief Get scheduler status of this thread.
       * @par Parameters
       *  None
       * @return Thread scheduler state.
       */
      thread::state_t
      sched_state (void) const;

      /**
       * @brief Wake-up the thread.
       * @par Parameters
       *  None
       * @return  Nothing.
       *
       * @note Can be invoked from Interrupt Service Routines.
       */
      void
      wakeup (void);

      /**
       * @brief Get the thread function arguments.
       * @par Parameters
       *  None.
       * @return Pointer to arguments.
       */
      void*
      function_args (void) const;

#if 0
      // Maybe make it a structure.
      result_t
      wakeup_reason (void) const;
#endif

      /**
       * @brief Get user storage.
       * @par Parameters
       *  None
       * @return The address of the thread user storage.
       */
      os_thread_user_storage_t*
      user_storage (void);

      /**
       * @brief Raise thread signal flags.
       * @param [in] mask The OR-ed flags to raise.
       * @param [out] oflags Optional pointer where to store the
       *  previous flags; may be `nullptr`.
       * @retval result::ok The flags were raised.
       * @retval EINVAL The mask is zero.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       */
      result_t
      sig_raise (thread::sigset_t mask, thread::sigset_t* oflags);

      /**
       * @brief Clear thread signal flags.
       * @param [out] oflags Optional pointer where to store the
       *  previous flags; may be `nullptr`.
       * @param [in] mask The OR-ed flags to clear.
       * @retval result::ok The flags were cleared.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval EINVAL The mask is zero.
       */
      result_t
      sig_clear (thread::sigset_t mask, thread::sigset_t* oflags);

      /**
       * @brief Get/clear thread signal flags.
       * @param [in] mask The OR-ed flags to get/clear; may be zero.
       * @param [in] mode Mode bits to select if the flags should be
       *  cleared (the other bits are ignored).
       * @retval flags The selected bits from the current thread
       *  signal flags mask.
       * @retval sig::all Cannot be invoked from an Interrupt Service Routines.
       */
      thread::sigset_t
      sig_get (thread::sigset_t mask, flags::mode_t mode);

      /**
       * @brief Force thread termination.
       * @par Parameters
       *  None
       * @retval result::ok The tread was terminated.
       */
      result_t
      kill (void);

      /**
       * @}
       */

    protected:

      /**
       * @name Private Friends
       * @{
       */

      friend void
      this_thread::suspend (void);

      friend void
      this_thread::exit (void* exit_ptr);

      friend result_t
      this_thread::sig_wait (thread::sigset_t mask, thread::sigset_t* oflags,
                             flags::mode_t mode);

      friend result_t
      this_thread::try_sig_wait (thread::sigset_t mask,
                                 thread::sigset_t* oflags, flags::mode_t mode);
      friend result_t
      this_thread::timed_sig_wait (thread::sigset_t mask,
                                   thread::sigset_t* oflags, systicks_t ticks,
                                   flags::mode_t mode);

      /**
       * @}
       * @name Private Member Functions
       * @{
       */

      /**
       * @brief Suspend this thread.
       * @par Parameters
       *  None
       * @return  Nothing.
       */
      void
      suspend (void);

      /**
       * @brief Terminate thread by itself.
       * @param [in] exit_ptr Pointer to object to return (optional).
       * @return  Nothing.
       */
      void
      exit (void* exit_ptr = nullptr);

      /**
       * @brief Invoke terminating thread function.
       * @param [in] thread The static `this`.
       * @return  Nothing.
       */
      static void
      _invoke_with_exit (Thread* thread);

      /**
       * @brief Wait for signal flags.
       * @param [in] mask The expected flags (OR-ed bit-mask);
       *  may be zero.
       * @param [out] oflags Pointer where to store the current flags;
       *  may be `nullptr`.
       * @param [in] mode Mode bits to select if either all or any flags
       *  are expected, and if the flags should be cleared.
       * @retval result::ok All expected flags are raised.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval EINVAL The mask is outside of the permitted range.
       * @retval EINTR The operation was interrupted.
       * @retval ENOTRECOVERABLE Wait failed.
       */
      result_t
      sig_wait (thread::sigset_t mask, thread::sigset_t* oflags,
                flags::mode_t mode);

      /**
       * @brief Try to wait for signal flags.
       * @param [in] mask The expected flags (OR-ed bit-mask);
       *  may be zero.
       * @param [out] oflags Pointer where to store the current flags;
       *  may be `nullptr`.
       * @param [in] mode Mode bits to select if either all or any flags
       *  are expected, and if the flags should be cleared.
       * @retval result::ok All expected flags are raised.
       * @retval EINVAL The mask is outside of the permitted range.
       * @retval EAGAIN The expected condition did not occur.
       * @retval ENOTRECOVERABLE Wait failed.
       */
      result_t
      try_sig_wait (thread::sigset_t mask, thread::sigset_t* oflags,
                    flags::mode_t mode);

      /**
       * @brief Timed wait for signal flags.
       * @param [in] mask The expected flags (OR-ed bit-mask);
       *  may be zero.
       * @param [out] oflags Pointer where to store the current flags;
       *  may be `nullptr`.
       * @param [in] mode Mode bits to select if either all or any flags
       *  are expected, and if the flags should be cleared.
       * @param ticks The number of ticks to wait.
       * @retval result::ok All expected flags are raised.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval ETIMEDOUT The expected condition did not occur during the
       *  entire timeout duration.
       * @retval EINVAL The mask is outside of the permitted range.
       * @retval EINTR The operation was interrupted.
       * @retval ENOTRECOVERABLE Wait failed.
       */
      result_t
      timed_sig_wait (thread::sigset_t mask, thread::sigset_t* oflags,
                      systicks_t ticks, flags::mode_t mode);

      /**
       * @brief Internal wait for signal.
       * @param [in] mask The expected flags (OR-ed bit-mask);
       *  may be zero.
       * @param [out] oflags Pointer where to store the current flags;
       *  may be `nullptr`.
       * @param [in] mode Mode bits to select if either all or any flags
       *  are expected, and if the flags should be cleared.
       * @retval result::ok All expected flags are raised.
       * @retval EINVAL The mask is outside of the permitted range.
       * @retval EAGAIN The expected condition did not occur.
       * @retval ENOTRECOVERABLE Wait failed.
       */
      result_t
      _try_wait (thread::sigset_t mask, thread::sigset_t* oflags,
                 flags::mode_t mode);

      /**
       * @brief The actual destructor, also called from exit() and kill().
       * @par Parameters
       *  None
       * @return  Nothing.
       */
      void
      _destroy (void);

      /**
       * @}
       */

    protected:

      /**
       * @name Private Member Variables
       * @{
       */

      // TODO: group them in a Stack object
      void* stack_addr_;
      thread::func_t func_;
      thread::func_args_t func_args_;
      void* func_result_;

      // Implementation
#if defined(OS_INCLUDE_PORT_RTOS_THREAD)
      friend class port::Thread;
      os_thread_port_data_t port_;
#endif

      Thread* joiner_;

      std::size_t stack_size_bytes_;
      thread::state_t sched_state_;
      thread::priority_t prio_;

      result_t wakeup_reason_;

      // volatile, but used in critical sections.
      thread::sigset_t sig_mask_;

      os_thread_user_storage_t user_storage_;

      // Add other internal data

      /**
       * @}
       */
    };

#pragma GCC diagnostic pop

    // ========================================================================

    /**
     * @class Systick_clock
     * @brief SysTick derived clock.
     *
     * @details
     * This clock counts SysTick interrupts since startup.
     *
     * The SysTick clock should be a steady clock, i.e. the total
     * count of ticks should be monotone ascending (in other words no
     * adjustments to the past should be performed).
     *
     * For Cortex-M implementations using the standard SysTick, this
     * clock is able to provide accuracy at CPU cycle level, by
     * sampling the SysTick internal counter. For a CPU clock of 100 MHz,
     * this gives a 10 ns resolution, quite high for accurate timing.
     *
     * @par Example
     *
     * @code{.cpp}
     * void
     * func(void)
     * {
     *    // Do something
     *
     *    // Get the current ticks counter.
     *    Systick_clock::rep ticks = Systick_clock::now();
     *
     *    // Put the current thread to sleep for a given number of ticks.
     *    Systick_clock::sleep_for(7);
     *
     *    // Put the current thread to sleep for a given number of microseconds.
     *    // For a 1000 Hz clock, the actual value is 4 ticks.
     *    Systick_clock::sleep_for(Systick_clock::ticks_cast(3500));
     *
     *    // Do something else.
     * }
     * @endcode
     *
     * @headerfile os.h <cmsis-plus/rtos/os.h>
     */
    class Systick_clock
    {
    public:

      /**
       * @name Types & Constants
       * @{
       */

      /**
       * @brief SysTick frequency in Hz.
       */
      static constexpr uint32_t frequency_hz = OS_INTEGER_SYSTICK_FREQUENCY_HZ;

      /**
       * @brief Type of ticks counter.
       */
      using rep = uint64_t;

      /**
       * @brief Type of duration in ticks.
       */
      using sleep_rep = duration_t;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

      /**
       * @brief SysTick detailed timestamp.
       *
       * @details
       * When an accurate timestamp is needed, the current SysTick
       * counter can be sampled to get the count of CPU cycles inside
       * the tick. For a 100 MHz clock, this gives a 10 ns resolution.
       *
       * To simplify further processing of this timestamp, the
       * structure also includes the CPU clock and the SysTick divider.
       */
      using current_t = struct
        {
          /**
           * @brief Count of SysTick ticks since core reset.
           */
          uint64_t ticks;

          /**
           * @brief Count of SysTick cycles since timer reload (24 bits).
           */
          uint32_t cycles;

          /**
           * @brief SysTick reload value (24 bits).
           */
          uint32_t divisor;

          /**
           * @brief CPU clock frequency Hz.
           */
          uint32_t core_frequency_hz;
        };

#pragma GCC diagnostic pop

      /**
       * @}
       * @name Public Member Functions
       * @{
       */

      /**
       * @brief Tell the current time.
       * @par Parameters
       *  None
       * @return The number of SysTick ticks since startup.
       */
      static rep
      now (void);

      /**
       * @brief Tell the detailed current time.
       * @param [out] details Pointer to structure to store the clock details.
       * @return The number of SysTick ticks since startup.
       */
      static rep
      now (current_t* details);

      /**
       * @brief Convert microseconds to ticks.
       * @tparam Rep_T Type of input, auto deduced (usually uint32_t or uin64_t)
       * @param [in] microsec The number of microseconds.
       * @return The number of ticks.
       */
      template<typename Rep_T>
        static constexpr uint32_t
        ticks_cast (Rep_T microsec);

      /**
       * @brief Sleep a number of ticks.
       * @param [in] ticks The number of ticks to sleep.
       * @retval ETIMEDOUT The sleep lasted the entire duration.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval EINTR The sleep was interrupted.
       */
      static result_t
      sleep_for (sleep_rep ticks);

      /**
       * @brief Wait for an event.
       * @param [in] ticks The timeout in ticks.
       * @retval result::ok An event occurred before the timeout.
       * @retval ETIMEDOUT The wait lasted the entire duration.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval EINTR The sleep was interrupted.
       */
      static result_t
      wait (sleep_rep ticks);

      /**
       * @}
       */

    protected:

      /**
       * @name Private Member Functions
       * @{
       */

      /**
       * @brief Internal wait.
       * @param ticks
       * @retval result::ok An event occurred before the timeout.
       * @retval ETIMEDOUT The wait lasted the entire duration.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval EINTR The sleep was interrupted.
       */
      static result_t
      _wait (sleep_rep ticks);

      /**
       * @}
       */

    };

    /**
     * @class Realtime_clock
     * @brief Real time clock.
     *
     * @details
     * This clock counts seconds since epoch or startup.
     *
     * The real time clock should be derived from a battery powered
     * second counting RTC, initialised at startup with the number
     * of seconds since the standard POSIX epoch (January 1st, 1970).
     *
     * As any usual clock, it might occasionally be adjusted to match
     * a reference clock, so i cannot be a steady clock.
     *
     * For systems that do not have a hardware RTC, it can be derived from
     * SysTick, but in this case it must be externally initialised with
     * the epoch.
     *
     * @par Example
     *
     * @code{.cpp}
     * void
     * func(void)
     * {
     *    // Do something
     *
     *    // Get the current seconds counter.
     *    Realtime_clock::rep seconds = Realtime_clock::now();
     *
     *    // Put the current thread to sleep for a given number of seconds.
     *    Realtime_clock::sleep_for(7);
     *
     *    // Do something else.
     * }
     * @endcode
     *
     * @headerfile os.h <cmsis-plus/rtos/os.h>
     */
    class Realtime_clock
    {
    public:

      /**
       * @name Types & Constants
       * @{
       */

      /**
       * @brief Real time clock frequency in Hz.
       */
      static constexpr uint32_t frequency_hz = 1;

      /**
       * @brief Type of seconds counter.
       */
      using rep = uint64_t;

      /**
       * @brief Type of duration in seconds.
       */
      using sleep_rep = duration_t;

      /**
       * @}
       * @name Public Member Functions
       * @{
       */

      /**
       * @brief Tell the absolute time now.
       * @par Parameters
       *  None
       * @return The number of seconds since January 1st, 1970 00:00:00.
       */
      static rep
      now (void);

      /**
       * @brief Sleep a number of seconds.
       * @param [in] secs The number of seconds to sleep.
       * @retval ETIMEDOUT The sleep lasted the entire duration.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval EINTR The sleep was interrupted.
       */
      static result_t
      sleep_for (sleep_rep secs);

      /**
       * @brief Initialise RTC.
       * @par Parameters
       *  None
       * @retval result::ok   The real time clock was initialised.
       * @retval ENOTRECOVERABLE Could not initialise real time clock.
       */
      static result_t
      initialize (void);

      /**
       * @}
       */
    };

    // ==================--====================================================

    /**
     * @namespace os::rtos::timer
     * @brief User timer namespace.
     * @details
     * The os::rtos::timer namespace groups timer types, enumerations,
     * attributes and initialisers.
     */
    namespace timer
    {
      /**
       * @brief Timer call back function arguments.
       */
      using func_args_t = void*;

      /**
       * @brief Entry point of a timer call back function.
       */
      using func_t = void (*) (func_args_t args);

      /**
       * @brief Timer run type.
       */
      using type_t = enum class run : uint8_t
        {
          /**
           * @brief Run only once.
           */
          once = 0,

          /**
           * @brief Run periodically.
           */
          periodic = 1      //
        };

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

      /**
       * @class Attributes
       * @brief Timer attributes.
       * @details
       * Allow to assign a name to the timer.
       *
       * To simplify access, the member variables are public and do not
       * require accessors or mutators.
       *
       * @par POSIX compatibility
       *  No POSIX similar functionality identified, but inspired by POSIX
       *  attributes used in [<pthread.h>](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html)
       *  (IEEE Std 1003.1, 2013 Edition).
       *
       * @headerfile os.h <cmsis-plus/rtos/os.h>
       */
      class Attributes : public Named_object
      {
      public:

        /**
         * @name Constructors & Destructor
         * @{
         */

        /**
         * @brief Create timer attributes.
         * @param [in] name Null terminated name. If `nullptr`, "-" is assigned.
         */
        Attributes (const char* name);

        /**
         * @cond ignore
         */
        Attributes (const Attributes&) = default;
        Attributes (Attributes&&) = default;
        Attributes&
        operator= (const Attributes&) = default;
        Attributes&
        operator= (Attributes&&) = default;
        /**
         * @endcond
         */

        /**
         * @brief Destroy timer attributes.
         */
        ~Attributes () = default;

        /**
         * @}
         */

      public:

        /**
         * @name Public Member Variables
         * @{
         */

        // Public members, no accessors and mutators required.
        // Warning: must match the type & order of the C file header.
        /**
         * @brief Timer type attribute.
         */
        type_t tm_type;

        // Add more attributes.

        /**
         * @}
         */
      };

#pragma GCC diagnostic pop

      /**
       * @brief Default one shot timer initialiser.
       */
      extern const Attributes once_initializer;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

      /**
       * @class Periodic_attributes
       * @brief Periodic timer attributes.
       * @details
       * Allow to assign a name to the timer.
       *
       * @headerfile os.h <cmsis-plus/rtos/os.h>
       */
      class Periodic_attributes : public Attributes
      {
      public:

        /**
         * @name Constructors & Destructor
         * @{
         */

        /**
         * @brief Create periodic timer attributes.
         * @param [in] name Null terminated name. If `nullptr`, "-" is assigned.
         */
        Periodic_attributes (const char* name);

        /**
         * @cond ignore
         */
        Periodic_attributes (const Periodic_attributes&) = default;
        Periodic_attributes (Periodic_attributes&&) = default;
        Periodic_attributes&
        operator= (const Periodic_attributes&) = default;
        Periodic_attributes&
        operator= (Periodic_attributes&&) = default;
        /**
         * @endcond
         */

        /**
         * @brief Destroy timer attributes.
         */
        ~Periodic_attributes () = default;

        /**
         * @}
         */

      };

#pragma GCC diagnostic pop

      /**
       * @brief Default periodic timer initialiser.
       */
      extern const Periodic_attributes periodic_initializer;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

    /**
     * @class Timer
     * @brief User timer.
     * @details
     * TODO
     *
     * @par POSIX compatibility
     *  No POSIX similar functionality identified.
     *
     * @headerfile os.h <cmsis-plus/rtos/os.h>
     */
    class Timer : public Named_object
    {
    public:

      /**
       * @name Constructors & Destructor
       * @{
       */

      /**
       * @brief Create a timer with default settings.
       * @param [in] function Pointer to timer function.
       * @param [in] args Pointer to arguments.
       */
      Timer (timer::func_t function, timer::func_args_t args);

      /**
       * @brief Create a timer with custom settings.
       * @param [in] attr Reference to attributes.
       * @param [in] function Pointer to timer function.
       * @param [in] args Pointer to arguments.
       */
      Timer (const timer::Attributes& attr, timer::func_t function,
             timer::func_args_t args);

      /**
       * @cond ignore
       */
      Timer (const Timer&) = delete;
      Timer (Timer&&) = delete;
      Timer&
      operator= (const Timer&) = delete;
      Timer&
      operator= (Timer&&) = delete;
      /**
       * @endcond
       */

      /**
       * @brief Destroy the timer.
       */
      ~Timer ();

      /**
       * @}
       * @name Operators
       * @{
       */

      /**
       * @brief Compare timers.
       * @retval true The given timer is the same as this timer.
       * @retval false The timers are different.
       */
      bool
      operator== (const Timer& rhs) const;

      /**
       * @}
       * @name Public Member Functions
       * @{
       */

      /**
       * @brief Start or restart the timer.
       * @param [in] ticks The timer period, in ticks.
       * @retval result::ok The timer has been started or restarted.
       * @retval ENOTRECOVERABLE Timer could not be started.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       */
      result_t
      start (systicks_t ticks);

      /**
       * @brief Stop the timer.
       * @par Parameters
       *  None
       * @retval result::ok The timer has been stopped.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval EAGAIN The timer is not yet started.
       * @retval ENOTRECOVERABLE Timer could not be stopped.
       */
      result_t
      stop (void);

      /**
       * @}
       */

    protected:

      /**
       * @name Private Member Variables
       * @{
       */

      timer::func_t func_;
      timer::func_args_t func_args_;

#if defined(OS_INCLUDE_PORT_RTOS_TIMER)
      friend class port::Timer;
      os_timer_port_data_t port_;
#endif

      timer::type_t type_;

      // Add more internal data.

      /**
       * @}
       */
    };

#pragma GCC diagnostic pop

    // ========================================================================

    /**
     * @namespace os::rtos::mutex
     * @brief Mutex namespace.
     * @details
     * The os::rtos::mutex namespace groups mutex types, enumerations,
     * attributes and initialisers.
     */
    namespace mutex
    {
      /**
       * @brief Type of mutex protocol.
       */
      using protocol_t = enum class protocol : uint8_t
        {
          /**
           * @brief Undefined value.
           */
          none = 0,

          /**
           * @brief Inherit.
           * @details
           * TODO: add
           */
          inherit = 1,

          /**
           * @brief Protect.
           * @details
           * TODO: add
           */
          protect = 2
        };

      /**
       * @brief Type of mutex robustness.
       */
      using robustness_t = enum class robustness : uint8_t
        {
          /**
           * @brief Normal robustness.
           */
          stalled = 0,
          /**
           * @brief Enhanced robustness.
           */
          robust = 1
        };

      /**
       * @brief Type of mutex behaviour.
       */
      using type_t = enum class type : uint8_t
        {
          /**
           * @brief Normal mutex behaviour.
           */
          normal = 0,
          /**
           * @brief Check mutex behaviour.
           */
          errorcheck = 1,
          /**
           * @brief Recursive mutex behaviour.
           */
          recursive = 2,
        };

      /**
       * @brief Type of mutex recursion counter.
       */
      using count_t = uint16_t;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

      /**
       * @class Attributes
       * @brief Mutex attributes.
       * @details
       * Allow to assign a name and custom attributes (like priority ceiling,
       * robustness, etc) to the mutex.
       *
       * To simplify access, the member variables are public and do not
       * require accessors or mutators.
       *
       * @par POSIX compatibility
       *  Inspired by `pthread_mutexattr_t` from [<pthread.h>](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html)
       *  (IEEE Std 1003.1, 2013 Edition).
       *
       * @headerfile os.h <cmsis-plus/rtos/os.h>
       */
      class Attributes : public Named_object
      {
      public:

        /**
         * @name Constructors & Destructor
         * @{
         */

        /**
         * @brief Create mutex attributes.
         * @param [in] name Null terminated name. If `nullptr`, "-" is assigned.
         */
        Attributes (const char* name);

        /**
         * @cond ignore
         */
        Attributes (const Attributes&) = default;
        Attributes (Attributes&&) = default;
        Attributes&
        operator= (const Attributes&) = default;
        Attributes&
        operator= (Attributes&&) = default;
        /**
         * @endcond
         */

        /**
         * @brief Destroy a mutex attributes.
         */
        ~Attributes () = default;

        /**
         * @}
         */

      public:

        /**
         * @name Public Member Variables
         * @{
         */

        // Public members, no accessors and mutators required.
        // Warning: must match the type & order of the C file header.
        /**
         * @brief Mutex priority ceiling.
         * @details
         * The @ref mx_priority_ceiling attribute defines the priority
         * ceiling of initialised mutexes, which is the minimum priority
         * level at which the critical section guarded by the mutex is
         * executed. In order to avoid priority inversion, the priority
         * ceiling of the mutex shall be set to a priority higher than
         * or equal to the highest priority of all the threads that may
         * lock that mutex. The values of @ref mx_priority_ceiling are
         * within the maximum range of priorities defined under the
         * SCHED_FIFO scheduling policy.
         *
         * @par POSIX compatibility
         *  Inspired by `pthread_mutexattr_setprioceiling()` from
         *  [<pthread.h>](http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutexattr_getprioceiling.html)
         *  (IEEE Std 1003.1, 2013 Edition).
         */
        thread::priority_t mx_priority_ceiling;

        /**
         * @brief Mutex protocol attribute.
         * @details
         * @details
         * The default value of the attribute shall be PTHREAD_PRIO_NONE.
         *
         * When a thread owns a mutex with the PTHREAD_PRIO_NONE
         * protocol attribute, its priority and scheduling shall
         * not be affected by its mutex ownership.
         *
         * When a thread is blocking higher priority threads
         * because of owning one or more robust mutexes with the
         * PTHREAD_PRIO_INHERIT protocol attribute, it shall execute
         * at the higher of its priority or the priority of the highest
         * priority thread waiting on any of the robust mutexes owned
         * by this thread and initialised with this protocol.
         *
         * When a thread is blocking higher priority threads because
         * of owning one or more non-robust mutexes with the
         * PTHREAD_PRIO_INHERIT protocol attribute, it shall execute
         * at the higher of its priority or the priority of the
         * highest priority thread waiting on any of the non-robust
         * mutexes owned by this thread and initialised with this protocol.
         *
         * When a thread owns one or more robust mutexes initialised
         * with the PTHREAD_PRIO_PROTECT protocol, it shall execute
         * at the higher of its priority or the highest of the priority
         * ceilings of all the robust mutexes owned by this thread and
         * initialised with this attribute, regardless of whether other
         * threads are blocked on any of these robust mutexes or not.
         *
         * When a thread owns one or more non-robust mutexes initialised
         * with the PTHREAD_PRIO_PROTECT protocol, it shall execute at
         * the higher of its priority or the highest of the priority
         * ceilings of all the non-robust mutexes owned by this thread
         * and initialised with this attribute, regardless of whether
         * other threads are blocked on any of these non-robust mutexes
         * or not.
         *
         * While a thread is holding a mutex which has been initialised
         * with the PTHREAD_PRIO_INHERIT or PTHREAD_PRIO_PROTECT protocol
         * attributes, it shall not be subject to being moved to the tail
         * of the scheduling queue at its priority in the event that its
         * original priority is changed, such as by a call to
         * sched_setparam(). Likewise, when a thread unlocks a mutex
         * that has been initialised with the PTHREAD_PRIO_INHERIT or
         * PTHREAD_PRIO_PROTECT protocol attributes, it shall not be
         * subject to being moved to the tail of the scheduling queue
         * at its priority in the event that its original priority is changed.
         *
         * If a thread simultaneously owns several mutexes initialised
         * with different protocols, it shall execute at the highest of
         * the priorities that it would have obtained by each of these
         * protocols.
         *
         * When a thread makes a call to Mutex::lock(), the mutex
         * was initialised with the protocol attribute having the
         * value PTHREAD_PRIO_INHERIT, when the calling thread is
         * blocked because the mutex is owned by another thread, that
         * owner thread shall inherit the priority level of the calling
         * thread as long as it continues to own the mutex. The
         * implementation shall update its execution priority to
         * the maximum of its assigned priority and all its
         * inherited priorities. Furthermore, if this owner thread
         * itself becomes blocked on another mutex with the protocol
         * attribute having the value PTHREAD_PRIO_INHERIT, the same
         * priority inheritance effect shall be propagated to this
         * other owner thread, in a recursive manner.
         *
         * @par POSIX compatibility
         *  Inspired by `pthread_mutexattr_setprotocol()` from
         *  [<pthread.h>](http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutexattr_getprotocol.html)
         *  (IEEE Std 1003.1, 2013 Edition).
         */
        mutex::protocol_t mx_protocol;

        /**
         * @brief Mutex protocol attribute.
         * @details
         *
         * Valid values for robust include:
         * - PTHREAD_MUTEX_STALLED
         *
         *   No special actions are taken if the owner of the mutex
         *  is terminated while holding the mutex lock. This can
         *  lead to deadlocks if no other thread can unlock the mutex.
         *  This is the default value.
         *
         * - PTHREAD_MUTEX_ROBUST
         *
         *   If the process containing the owning thread of a robust
         *   mutex terminates while holding the mutex lock, the next
         *   thread that acquires the mutex shall be notified about
         *   the termination by the return value [EOWNERDEAD] from
         *   the locking function. If the owning thread of a robust
         *   mutex terminates while holding the mutex lock, the next
         *   thread that acquires the mutex may be notified about the
         *   termination by the return value [EOWNERDEAD]. The notified
         *   thread can then attempt to mark the state protected by
         *   the mutex as consistent again by a call to
         *   pthread_mutex_consistent(). After a subsequent
         *   successful call to pthread_mutex_unlock(), the mutex
         *   lock shall be released and can be used normally by
         *   other threads. If the mutex is unlocked without a
         *   call to pthread_mutex_consistent(), it shall be in a
         *   permanently unusable state and all attempts to lock
         *   the mutex shall fail with the error [ENOTRECOVERABLE].
         *   The only permissible operation on such a mutex is
         *   pthread_mutex_destroy().
         *
         * @par POSIX compatibility
         *  Inspired by `pthread_mutexattr_setrobust()` from
         *  [<pthread.h>](http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutexattr_getrobust.html)
         *  (IEEE Std 1003.1, 2013 Edition).
         */
        mutex::robustness_t mx_robustness;

        /**
         * @brief Mutex type attribute.
         * @details
         * The default value of the type attribute is PTHREAD_MUTEX_DEFAULT.
         *
         * The type of mutex is contained in the type attribute of
         * the mutex attributes. Valid mutex types include:
         *
         * - PTHREAD_MUTEX_NORMAL
         * - PTHREAD_MUTEX_ERRORCHECK
         * - PTHREAD_MUTEX_RECURSIVE
         * - PTHREAD_MUTEX_DEFAULT
         *
         * The mutex type affects the behaviour of calls which lock
         * and unlock the mutex. See @ref Mutex::lock() for details.
         * An implementation may map PTHREAD_MUTEX_DEFAULT to one of
         * the other mutex types.
         *
         * @par POSIX compatibility
         *  Inspired by `pthread_mutexattr_settype()` from
         *  [<pthread.h>](http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutexattr_gettype.html)
         *  (IEEE Std 1003.1, 2013 Edition).
         */
        mutex::type_t mx_type;

        // Add more attributes.

        /**
         * @}
         */
      };

#pragma GCC diagnostic pop

      /**
       * @brief Default normal mutex initialiser.
       */
      extern const Attributes normal_initializer;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

      /**
       * @class Recursive_attributes
       * @brief Recursive mutex attributes.
       * @details
       * Allow to assign a name and custom attributes (like priority ceiling,
       * robustness, etc) to the mutex.
       *
       * @par POSIX compatibility
       *  Inspired by `pthread_mutexattr_t` from [<pthread.h>](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html)
       *  (IEEE Std 1003.1, 2013 Edition).
       *
       * @headerfile os.h <cmsis-plus/rtos/os.h>
       */
      class Recursive_attributes : public Attributes
      {
      public:

        /**
         * @name Constructors & Destructor
         * @{
         */

        /**
         * @brief Create recursive mutex attributes.
         * @param [in] name Null terminated name. If `nullptr`, "-" is assigned.
         */
        Recursive_attributes (const char* name);

        /**
         * @cond ignore
         */
        Recursive_attributes (const Recursive_attributes&) = default;
        Recursive_attributes (Recursive_attributes&&) = default;
        Recursive_attributes&
        operator= (const Recursive_attributes&) = default;
        Recursive_attributes&
        operator= (Recursive_attributes&&) = default;
        /**
         * @endcond
         */

        /**
         * @brief Destroy a recursive mutex attributes.
         */
        ~Recursive_attributes () = default;

        /**
         * @}
         */
      };

#pragma GCC diagnostic pop

      /**
       * @brief Default recursive mutex initialiser.
       */
      extern const Recursive_attributes recursive_initializer;

    } /* namespace mutex */

    // ========================================================================

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

    /**
     * @class Mutex
     * @brief POSIX compliant mutex.
     *
     * @par POSIX compatibility
     *  Inspired by `pthread_mutex_t` from [<pthread.h>](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html)
     *  (IEEE Std 1003.1, 2013 Edition).
     *
     * @headerfile os.h <cmsis-plus/rtos/os.h>
     */
    class Mutex : public Named_object
    {
    public:

      /**
       * @name Constructors & Destructor
       * @{
       */

      /**
       * @brief Create a mutex with default settings.
       * @par Parameters
       *  None
       */
      Mutex ();
      /**
       * @brief Create a mutex with custom settings.
       * @param [in] attr Reference to attributes.
       */
      Mutex (const mutex::Attributes& attr);

      /**
       * @cond ignore
       */
      Mutex (const Mutex&) = delete;
      Mutex (Mutex&&) = delete;
      Mutex&
      operator= (const Mutex&) = delete;
      Mutex&
      operator= (Mutex&&) = delete;
      /**
       * @endcond
       */

      /**
       * @brief Destroy the mutex.
       */
      ~Mutex ();

      /**
       * @}
       * @name Operators
       * @{
       */

      /**
       * @brief Compare mutexes.
       * @retval true The given mutex is the same as this mutex.
       * @retval false The mutexes are different.
       */
      bool
      operator== (const Mutex& rhs) const;

      /**
       * @}
       * @name Public Member Functions
       * @{
       */

      /**
       * @brief Lock the mutex.
       * @par Parameters
       *  None
       * @retval result::ok The mutex was locked.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval ENOTRECOVERABLE The state protected by the mutex is
       *  not recoverable..
       * @retval EAGAIN The mutex could not be acquired because the maximum
       *  number of recursive locks for mutex has been exceeded.
       * @retval EINVAL The mutex was created with the protocol
       *  attribute having the value PTHREAD_PRIO_PROTECT and the
       *  calling thread's priority is higher than the mutex's
       *  current priority ceiling.
       * @retval EOWNERDEAD The mutex is a robust mutex and the process
       *  containing the previous owning thread terminated while holding
       *  the mutex lock. The mutex lock shall be acquired by the calling
       *  thread and it is up to the new owner to make the state consistent.
       * @retval EDEADLK The mutex type is PTHREAD_MUTEX_ERRORCHECK and
       *  the current thread already owns the mutex.
       */
      result_t
      lock (void);

      /**
       * @brief Try to lock the mutex.
       * @par Parameters
       *  None
       * @retval result::ok The mutex was locked.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval ENOTRECOVERABLE The state protected by the mutex is
       *  not recoverable..
       * @retval EAGAIN The mutex could not be acquired because the maximum
       *  number of recursive locks for mutex has been exceeded.
       * @retval EINVAL The mutex was created with the protocol
       *  attribute having the value PTHREAD_PRIO_PROTECT and the
       *  calling thread's priority is higher than the mutex's
       *  current priority ceiling.
       * @retval EOWNERDEAD The mutex is a robust mutex and the process
       *  containing the previous owning thread terminated while holding
       *  the mutex lock. The mutex lock shall be acquired by the calling
       *  thread and it is up to the new owner to make the state consistent.
       * @retval EDEADLK The mutex type is PTHREAD_MUTEX_ERRORCHECK and
       *  the current thread already owns the mutex.
       * @retval EBUSY The mutex could not be acquired because it was
       *  already locked.
       */
      result_t
      try_lock (void);

      /**
       * @brief Timed attempt to lock the mutex.
       * @param [in] ticks Number of ticks to wait.
       * @retval result::ok The mutex was locked.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval ETIMEDOUT The mutex could not be locked before the
       *  specified timeout expired.
       * @retval ENOTRECOVERABLE The state protected by the mutex
       *  is not recoverable.
       * @retval EAGAIN The mutex could not be acquired because the
       *  maximum number of recursive locks for mutex has been exceeded.
       * @retval EDEADLK The mutex type is PTHREAD_MUTEX_ERRORCHECK
       *  and the current thread already owns the mutex.
       * @retval EINVAL The process or thread would have blocked, and
       *  the abstime parameter specified a nanoseconds field value
       *  less than zero or greater than or equal to 1000 million.
       * @retval EOWNERDEAD The mutex is a robust mutex and the process
       *  containing the previous owning thread terminated while holding
       *  the mutex lock. The mutex lock shall be acquired by the
       *  calling thread and it is up to the new owner to make the
       *  state consistent.
       */
      result_t
      timed_lock (systicks_t ticks);

      /**
       * @brief Unlock the mutex.
       * @par Parameters
       *  None
       * @retval result::ok The mutex was unlocked.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routine;
       *  the mutex type is PTHREAD_MUTEX_ERRORCHECK or
       *  PTHREAD_MUTEX_RECURSIVE, or the mutex is a robust mutex,
       *  and the current thread does not own the mutex.
       * @retval ENOTRECOVERABLE The mutex was not unlocked.
       */
      result_t
      unlock (void);

      /**
       * @brief Get the priority ceiling of a mutex.
       * @par Parameters
       *  None
       * @return The priority ceiling.
       */
      thread::priority_t
      prio_ceiling (void) const;

      /**
       * @brief Change the priority ceiling of a mutex.
       * @param [in] prio_ceiling new priority.
       * @param [out] old_prio_ceiling pointer to location where to
       *  store the previous priority; may be `nullptr`.
       * @retval result::ok The priority was changed.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       */
      result_t
      prio_ceiling (thread::priority_t prio_ceiling,
                    thread::priority_t* old_prio_ceiling = nullptr);

      /**
       * @brief Mark mutex as consistent.
       * @par Parameters
       *  None
       * @retval result::ok The mutex was marked as consistent.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval EINVAL The mutex object referenced by mutex is not robust
       *  or does not protect an inconsistent state.
       */
      result_t
      consistent (void);

      /**
       * @brief Get owner thread.
       * @par Parameters
       *  None
       * @return Pointer to thread or `nullptr` if not owned.
       */
      Thread*
      owner (void);

      /**
       * @brief Reset mutex.
       * @par Parameters
       *  None
       * @retval result::ok The mutex was reset.
       */
      result_t
      reset (void);

      /**
       * @}
       */

    protected:

      /**
       * @name Private Member Variables
       * @{
       */

      // Can be updated in different thread contexts.
      Thread* volatile owner_;

#if defined(OS_INCLUDE_PORT_RTOS_MUTEX)
      friend class port::Mutex;
      os_mutex_port_data_t port_;
#endif

      // Can be updated in different thread contexts.
      volatile mutex::count_t count_;

      // Can be updated in different thread contexts.
      volatile thread::priority_t prio_ceiling_;

      // Constants set during construction.
      const mutex::type_t type_; // normal, errorcheck, recursive
      const mutex::protocol_t protocol_; // none, inherit, protect
      const mutex::robustness_t robustness_; // stalled, robust

      // Add more internal data.

      /**
       * @}
       */
    };

#pragma GCC diagnostic pop

    // ========================================================================

    /**
     * @namespace os::rtos::condvar
     * @brief Condition variable namespace.
     * @details
     * The `os::rtos::condvar` namespace groups condition variable attributes
     * and initialisers.
     */
    namespace condvar
    {

      /**
       * @class Attributes
       * @brief Condition variable attributes.
       * @details
       * Allow to assign a name to the condition variable.
       *
       * @par POSIX compatibility
       *  Inspired by `pthread_condattr_t` from [<pthread.h>](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html)
       *  (IEEE Std 1003.1, 2013 Edition).
       *
       * @headerfile os.h <cmsis-plus/rtos/os.h>
       */
      class Attributes : public Named_object
      {
      public:

        /**
         * @name Constructors & Destructor
         * @{
         */

        /**
         * @brief Create condition variable attributes.
         * @param [in] name Null terminated name. If `nullptr`, "-" is assigned.
         */
        Attributes (const char* name);

        /**
         * @cond ignore
         */
        Attributes (const Attributes&) = default;
        Attributes (Attributes&&) = default;
        Attributes&
        operator= (const Attributes&) = default;
        Attributes&
        operator= (Attributes&&) = default;
        /**
         * @endcond
         */

        /**
         * @brief Destroy condition variable attributes.
         */
        ~Attributes () = default;

        /**
         * @}
         */

      public:

        /**
         * @name Public Member Variables
         * @{
         */

        // Public members, no accessors and mutators required.
        // Warning: must match the type & order of the C file header.
        //
        // Add more attributes.
        /**
         * @}
         */
      };

      /**
       * @brief Default condition variable initialiser.
       */
      extern const Attributes initializer;

    } /* namespace condvar */

    // ========================================================================

    /**
     * @class Condition_variable
     * @brief POSIX compliant condition variable.
     *
     * @par POSIX compatibility
     *  Inspired by `pthread_cond_t` from [<pthread.h>](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html)
     *  (IEEE Std 1003.1, 2013 Edition).
     *
     * @headerfile os.h <cmsis-plus/rtos/os.h>
     */
    class Condition_variable : public Named_object
    {
    public:

      /**
       * @name Constructors & Destructor
       * @{
       */

      /**
       * @brief Create a condition variable with default settings.
       * @par Parameters
       *  None
       */
      Condition_variable ();
      /**
       * @brief Create a condition variable with custom settings.
       * @param [in] attr Reference to attributes.
       */
      Condition_variable (const condvar::Attributes& attr);

      /**
       * @cond ignore
       */
      Condition_variable (const Condition_variable&) = delete;
      Condition_variable (Condition_variable&&) = delete;
      Condition_variable&
      operator= (const Condition_variable&) = delete;
      Condition_variable&
      operator= (Condition_variable&&) = delete;
      /**
       * @endcond
       */

      /**
       * @brief Destroy a condition variable.
       */
      ~Condition_variable ();

      /**
       * @}
       * @name Operators
       * @{
       */

      /**
       * @brief Compare condition variables.
       * @retval true The given condition variable is the same as
       *  this condition variable.
       * @retval false The condition variables are different.
       */
      bool
      operator== (const Condition_variable& rhs) const;

      /**
       * @}
       * @name Public Member Functions
       * @{
       */

      /**
       * @brief Signal a condition variable variable.
       * @par Parameters
       *  None
       * @retval result::ok The thread was signaled.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       */
      result_t
      signal (void);

      /**
       * @brief Broadcast a condition variable.
       * @par Parameters
       *  None
       * @retval result::ok All waiting threads signaled.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       */
      result_t
      broadcast (void);

      /**
       * @brief Wait on a condition variable.
       * @param [in] mutex Reference to the associated mutex.
       * @retval result::ok The condition change was signaled.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       */
      result_t
      wait (Mutex& mutex);

      // Neither POSIX nor ISO define a try_wait(), so... do we need one?

      /**
       * @brief Timed wait on a condition variable.
       * @param [in] mutex Reference to the associated mutex.
       * @param [in] ticks Ticks to wait.
       * @retval result::ok The condition change was signaled.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       */
      result_t
      timed_wait (Mutex& mutex, systicks_t ticks);

      /**
       * @}
       */

    protected:

      /**
       * @name Private Member Variables
       * @{
       */

      // Add more internal data.
      /**
       * @}
       */

    };

    // ========================================================================

    /**
     * @namespace os::rtos::semaphore
     * @brief Semaphore namespace.
     * @details
     * The os::rtos::semaphore namespace groups semaphore types,
     * attributes and initialisers.
     */
    namespace semaphore
    {
      /**
       * @brief Type of semaphore counter.
       * @details
       * A numeric value enough to hold the semaphore counter,
       * usually a 16-bits signed value.
       */
      using count_t = int16_t;

      /**
       * @brief Maximum semaphore value.
       * @details
       * Used to validate the semaphore initial count and max count.
       */
      constexpr count_t max_count_value = 0x7FFF;

      /**
       * @class Attributes
       * @brief Semaphore attributes.
       * @details
       * Allow to assign a name and custom attributes (like initial count,
       * max count) to the semaphore.
       *
       * To simplify access, the member variables are public and do not
       * require accessors or mutators.
       *
       * @par POSIX compatibility
       *  No POSIX similar functionality identified, but inspired by POSIX
       *  attributes used in [<pthread.h>](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html)
       *  (IEEE Std 1003.1, 2013 Edition).
       *
       * @headerfile os.h <cmsis-plus/rtos/os.h>
       */
      class Attributes : public Named_object
      {
      public:

        /**
         * @name Constructors & Destructor
         * @{
         */

        /**
         * @brief Create semaphore attributes.
         * @param [in] name Null terminated name. If `nullptr`, "-" is assigned.
         */
        Attributes (const char* name);

        /**
         * @cond ignore
         */
        Attributes (const Attributes&) = default;
        Attributes (Attributes&&) = default;
        Attributes&
        operator= (const Attributes&) = default;
        Attributes&
        operator= (Attributes&&) = default;
        /**
         * @endcond
         */

        /**
         * @brief Destroy semaphore attributes.
         */
        ~Attributes () = default;

        /**
         * @}
         */

      public:

        /**
         * @name Public Member Variables
         * @{
         */

        // Public members, no accessors and mutators required.
        // Warning: must match the type & order of the C file header.
        /**
         * @brief Semaphore initial count.
         * @details
         * This values represents the number of resources initially
         * available to the semaphore.
         */
        count_t sm_initial_count;

        /**
         * @brief Semaphore max count.
         * @details
         * This values represents the maximum number of resources
         * available to the semaphore.
         */
        count_t sm_max_count;

        // Add more attributes.

        /**
         * @}
         */
      };

      /**
       * @brief Default counting semaphore initialiser.
       */
      extern const Attributes counting_initializer;

      /**
       * @class Binary_attributes
       * @brief Binary semaphore attributes.
       * @details
       * Allow to assign a name and custom attributes to the semaphore.
       *
       * @par POSIX compatibility
       *  No POSIX similar functionality identified, but inspired by POSIX
       *  attributes used in [<pthread.h>](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html)
       *  (IEEE Std 1003.1, 2013 Edition).
       *
       * @headerfile os.h <cmsis-plus/rtos/os.h>
       */
      class Binary_attributes : public Attributes
      {
      public:

        /**
         * @name Constructors & Destructor
         * @{
         */

        /**
         * @brief Create binary semaphore attributes.
         * @param [in] name Null terminated name. If `nullptr`, "-" is assigned.
         */
        Binary_attributes (const char* name);

        /**
         * @cond ignore
         */
        Binary_attributes (const Binary_attributes&) = default;
        Binary_attributes (Binary_attributes&&) = default;
        Binary_attributes&
        operator= (const Binary_attributes&) = default;
        Binary_attributes&
        operator= (Binary_attributes&&) = default;
        /**
         * @endcond
         */

        /**
         * @brief Destroy semaphore attributes.
         */
        ~Binary_attributes () = default;

        /**
         * @}
         */
      };

      /**
       * @brief Default binary semaphore initialiser.
       */
      extern const Binary_attributes binary_initializer;

    } /* namespace semaphore */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

    /**
     * @class Semaphore
     * @brief POSIX compliant semaphore.
     * @details
     * Supports both counting and binary semaphores.
     *
     * Semaphores should generally be used to synchronise with
     * events occuring on interrupts.
     *
     * For inter-thread synchronisation, to avoid cases of priority
     * inversion, more suitable are mutexes.
     *
     * @par POSIX compatibility
     *  Inspired by `sem_t` from [<semaphore.h>](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/semaphore.h.html)
     *  (IEEE Std 1003.1, 2013 Edition).
     *
     * @headerfile os.h <cmsis-plus/rtos/os.h>
     */
    class Semaphore : public Named_object
    {
    public:

      /**
       * @name Constructors & Destructor
       * @{
       */

      /**
       * @brief Create a semaphore with default settings.
       * @par Parameters
       *  None
       */
      Semaphore ();

      /**
       * @brief Create a semaphore with custom settings.
       * @param [in] attr Reference to attributes.
       */
      Semaphore (const semaphore::Attributes& attr);

      /**
       * @cond ignore
       */
      Semaphore (const Semaphore&) = delete;
      Semaphore (Semaphore&&) = delete;
      Semaphore&
      operator= (const Semaphore&) = delete;
      Semaphore&
      operator= (Semaphore&&) = delete;
      /**
       * @endcond
       */

      /**
       * @brief Destroy the semaphore.
       */
      ~Semaphore ();

      /**
       * @}
       * @name Operators
       * @{
       */

      /**
       * @brief Compare semaphores.
       * @retval true The given semaphore is the same as this semaphore.
       * @retval false The semaphores are different.
       */
      bool
      operator== (const Semaphore& rhs) const;

      /**
       * @}
       * @name Public Member Functions
       * @{
       */

      /**
       * @brief Post (unlock) the semaphore.
       * @par Parameters
       *  None
       * @retval result::ok The semaphore was posted.
       * @retval EOVERFLOW The max count was exceeded.
       * @retval ENOTRECOVERABLE The semaphore could not be posted
       *  (extension to POSIX).
       */
      result_t
      post (void);

      /**
       * @brief Lock the semaphore, possibly waiting.
       * @par Parameters
       *  None
       * @retval result::ok The calling process successfully
       *  performed the semaphore lock operation.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval ENOTRECOVERABLE Semaphore wait failed (extension to POSIX).
       * @retval EDEADLK A deadlock condition was detected.
       * @retval EINTR The operation was interrupted.
       */
      result_t
      wait (void);

      /**
       * @brief Try to lock  the semaphore.
       * @par Parameters
       *  None
       * @retval result::ok The calling process successfully
       *  performed the semaphore lock operation.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval EAGAIN The semaphore was already locked.
       * @retval ENOTRECOVERABLE Semaphore wait failed (extension to POSIX).
       * @retval EDEADLK A deadlock condition was detected.
       * @retval EINTR The operation was interrupted.
       */
      result_t
      try_wait (void);

      /**
       * @brief Timed wait to lock the semaphore.
       * @param [in] ticks Number of ticks to wait.
       * @retval result::ok The calling process successfully
       *  performed the semaphore lock operation.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval EINVAL Invalid timeout (POSIX limits the timeout
       *  to 1000 million ns)
       * @retval ETIMEDOUT The semaphore could not be locked before
       *  the specified timeout expired.
       * @retval ENOTRECOVERABLE Semaphore wait failed (extension to POSIX).
       * @retval EDEADLK A deadlock condition was detected.
       * @retval EINTR The operation was interrupted.
       */
      result_t
      timed_wait (systicks_t ticks);

      /**
       * @brief Get the semaphore value.
       * @par Parameters
       *  None
       * @return The semaphore value.
       */
      semaphore::count_t
      value (void) const;

      /**
       * @brief Reset the semaphore.
       * @par Parameters
       *  None
       * @retval result::ok The semaphore was reset.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       */
      result_t
      reset (void);

      /**
       * @brief Get the semaphore initial value.
       * @par Parameters
       *  None
       * @return The numeric value set from attributes.
       */
      semaphore::count_t
      initial_value (void) const;

      /**
       * @brief Get the semaphore max value.
       * @par Parameters
       *  None
       * @return The numeric value set from attributes.
       */
      semaphore::count_t
      max_value (void) const;

      /**
       * @}
       */

    protected:

      /**
       * @name Private Member Variables
       * @{
       */

#if !defined(OS_INCLUDE_PORT_RTOS_SEMAPHORE)
      port::Tasks_list list_;
#endif

#if defined(OS_INCLUDE_PORT_RTOS_SEMAPHORE)
      friend class port::Semaphore;
      os_semaphore_port_data_t port_;
#endif

      const semaphore::count_t initial_count_;

      // Can be updated in different contexts (interrupts or threads)
      volatile semaphore::count_t count_;

      // Constant set during construction.
      const semaphore::count_t max_count_;

      // Add more internal data.

      /**
       * @}
       */
    };

#pragma GCC diagnostic pop

    // ========================================================================

    /**
     * @namespace os::rtos::mempool
     * @brief Memory pool namespace.
     * @details
     * The os::rtos::mempool namespace groups memory pool attributes
     * and initialisers.
     */
    namespace mempool
    {
      /**
       * @brief Type of memory pool size.
       * @details
       * A numeric value that can hold the maximum size of the
       * memory pool, usually a 16-bits unsigned value.
       */
      using size_t = uint16_t;

      /**
       * @brief Maximum pool size.
       * @details
       * A constant numeric value used to validate the pool size.
       */
      constexpr size_t max_size = (0 - 1);

      /**
       * @class Attributes
       * @brief Memory pool attributes.
       * @details
       * Allow to assign a name and custom attributes (like a static
       * address) to the memory pool.
       *
       * To simplify access, the member variables are public and do not
       * require accessors or mutators.
       *
       * @par Example
       *
       * Define an array of structures and
       * pass its address and size via the attributes.
       *
       * @code{.cpp}
       * // Define the type of one pool block.
       * typedef struct {
       *   uint32_t length;
       *   uint32_t width;
       *   uint32_t height;
       *   uint32_t weight;
       * } properties_t;
       *
       * // Define the pool size.
       * constexpr uint32_t pool_size = 10;
       *
       * // Allocate static storage for the pool.
       * properties_t pool[pool_size];
       *
       * void
       * func(void)
       * {
       *    // Do something
       *
       *    // Define pool attributes.
       *    mempool::Attributes attr { "properties" };
       *    attr.mp_pool_address = pool;
       *    attr.mp_pool_size_bytes = sizeof(pool);
       *
       *    // Create the pool object.
       *    Memory_pool mp { attr, pool_size, sizeof(properties_t) };
       *
       *    // Do something else.
       * }
       * @endcode
       *
       * @par POSIX compatibility
       *  No POSIX similar functionality identified, but inspired by POSIX
       *  attributes used in [<pthread.h>](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html)
       *  (IEEE Std 1003.1, 2013 Edition).
       *
       * @headerfile os.h <cmsis-plus/rtos/os.h>
       */
      class Attributes : public Named_object
      {
      public:

        /**
         * @name Constructors & Destructor
         * @{
         */

        /**
         * @brief Create memory pool attributes.
         * @param [in] name Null terminated name. If `nullptr`, "-" is assigned.
         */
        Attributes (const char* name);

        /**
         * @cond ignore
         */
        Attributes (const Attributes&) = default;
        Attributes (Attributes&&) = default;
        Attributes&
        operator= (const Attributes&) = default;
        Attributes&
        operator= (Attributes&&) = default;
        /**
         * @endcond
         */

        /**
         * @brief Destroy the memory pool attributes.
         */
        ~Attributes () = default;

        /**
         * @}
         */

      public:

        /**
         * @name Public Member Variables
         * @{
         */

        // Public members, no accessors and mutators required.
        // Warning: must match the type & order of the C file header.
        /**
         * @brief User defined memory pool address.
         * @details
         * Set this variable to a user defined memory area large enough
         * to store the memory pool. Usually this is a statically
         * allocated array of structures.
         *
         * The default value is `nullptr`, which means there is no
         * user defined memory pool.
         */
        void* mp_pool_address;

        /**
         * @brief User defined memory pool size.
         * @details
         * The memory pool size must match exactly the allocated size. It is
         * used for validation; when the memory pool is initialised,
         * this size must be large enough to accommodate the desired
         * memory pool.
         *
         * If the @ref mp_pool_address is `nullptr`, this variable is not
         * checked, but it is recommended to leave it zero.
         */
        size_t mp_pool_size_bytes;

        // Add more attributes.

        /**
         * @}
         */
      };

      /**
       * @brief Default memory pool initialiser.
       */
      extern const Attributes initializer;

    } /* namespace mempool */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

    /**
     * @class Memory_pool
     * @brief Synchronised memory pool.
     * @details
     * Manage a pool of same size blocks. Fast and deterministic allocation
     * and dealocation behaviour, suitable for use even in ISRs.
     *
     * @par POSIX compatibility
     *  No POSIX similar functionality identified.
     *
     * @note There is no equivalent of calloc(); to initialise memory, use:
     * @code{.cpp}
     * memset (block, 0, pool.block_size ());
     * @endcode
     *
     * @headerfile os.h <cmsis-plus/rtos/os.h>
     */
    class Memory_pool : public Named_object
    {
    public:

      /**
       * @name Constructors & Destructor
       * @{
       */

      /**
       * @brief Create a memory pool with default settings.
       * @param [in] blocks The maximum number of items in the pool.
       * @param [in] block_size_bytes The size of an item, in bytes.
       */
      Memory_pool (mempool::size_t blocks, mempool::size_t block_size_bytes);

      /**
       * @brief Create a memory pool with custom settings.
       * @param [in] attr Reference to attributes.
       * @param [in] blocks The maximum number of items in the pool.
       * @param [in] block_size_bytes The size of an item, in bytes.
       */
      Memory_pool (const mempool::Attributes& attr, mempool::size_t blocks,
                   mempool::size_t block_size_bytes);

      /**
       * @cond ignore
       */
      Memory_pool (const Memory_pool&) = delete;
      Memory_pool (Memory_pool&&) = delete;
      Memory_pool&
      operator= (const Memory_pool&) = delete;
      Memory_pool&
      operator= (Memory_pool&&) = delete;
      /**
       * @endcond
       */

      /**
       * @brief Destroy the memory pool.
       */
      ~Memory_pool ();

      /**
       * @}
       * @name Operators
       * @{
       */

      /**
       * @brief Compare memory pools.
       * @retval true The given memory pool is the same as this memory pool.
       * @retval false The memory pools are different.
       */
      bool
      operator== (const Memory_pool& rhs) const;

      /**
       * @}
       * @name Public Member Functions
       * @{
       */

      /**
       * @brief Allocate a memory block.
       * @par Parameters
       *  None
       * @return Pointer to memory block.
       */
      void*
      alloc (void);

      /**
       * @brief Allocate a memory block.
       * @par Parameters
       *  None
       * @return Pointer to memory block, or `nullptr` if no memory available.
       */
      void*
      try_alloc (void);

      /**
       * @brief Allocate a memory block.
       * @param [in] ticks The number of SysTick tick to wait.
       * @return Pointer to memory block, or `nullptr` if timeout.
       */
      void*
      timed_alloc (systicks_t ticks);

      /**
       * @brief Free the memory block.
       * @par Parameters
       *  None
       * @retval result::ok The memory block was released.
       * @retval EINVAL The block does not belong to the memory pool.
       */
      result_t
      free (void* block);

      /**
       * @brief Get memory pool capacity.
       * @par Parameters
       *  None
       * @return The max number of blocks in the pool.
       */
      std::size_t
      capacity (void) const;

      /**
       * @brief Get blocks count.
       * @par Parameters
       *  None
       * @return The number of blocks used from the queue.
       */
      std::size_t
      count (void) const;

      /**
       * @brief Get block size.
       * @par Parameters
       *  None
       * @return The block size, in bytes.
       */
      std::size_t
      block_size (void) const;

      /**
       * @brief Check if the memory pool is empty.
       * @par Parameters
       *  None
       * @retval true The memory pool has no allocated blocks.
       * @retval false The memory pool has allocated blocks.
       */
      bool
      empty (void) const;

      /**
       * @brief Check if the memory pool is full.
       * @par Parameters
       *  None
       * @retval true All memory blocks are allocated.
       * @retval false There are still memory blocks that can be allocated.
       */
      bool
      full (void) const;

      /**
       * @brief Reset the memory pool.
       * @par Parameters
       *  None
       * @retval result::ok The memory pool was reset.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       */
      result_t
      reset (void);

      /**
       *
       * @par Parameters
       *  None
       */
      void*
      pool (void);

      /**
       * @}
       */

    protected:

      /**
       * @name Private Member Functions
       * @{
       */

      /**
       * @brief Internal function used to get the first linked block.
       * @par Parameters
       *  None
       * @return Pointer to block or `nullptr` if no more blocks available.
       */
      void*
      _try_first (void);

      /**
       * @}
       */

    protected:

      /**
       * @name Private Member Variables
       * @{
       */

#if !defined(OS_INCLUDE_PORT_RTOS_MEMORY_POOL)
      port::Tasks_list list_;
#endif
      char* pool_addr_;

#if defined(OS_INCLUDE_PORT_RTOS_MEMORY_POOL)
      friend class port::Memory_pool;
      os_mempool_port_data_t port_;
#endif

      const mempool::size_t blocks_;
      const mempool::size_t block_size_bytes_;

      volatile mempool::size_t count_;

      // All accesses will be done inside a critical section,
      // the volatile may not be needed, the variable remains stable
      // during the critical section and no loops wait for this variable.
      void* volatile first_;

      uint8_t flags_;
      enum
        : uint8_t
          {
            flags_allocated = 1
      };
      // Add more internal data.

      /**
       * @}
       */
    };

#pragma GCC diagnostic pop

    // ========================================================================

    /**
     * @namespace os::rtos::mqueue
     * @brief Message queue namespace.
     * @details
     * The os::rtos::mqueue namespace groups message queue attributes
     * and initialisers.
     */
    namespace mqueue
    {
      /**
       * @brief Type of queue size.
       * @details
       * A numeric value to hold the message queue size, usually
       * a 16-bits value.
       */
      using size_t = uint16_t;

      /**
       * @brief Type of message priority.
       * @details
       * A numeric value to hold the message priority, which
       * controls the order in which messages are added to the
       * queue (higher values represent higher priorities).
       */
      using priority_t = uint8_t;

      /**
       * @class Attributes
       * @brief Message queue attributes.
       * @details
       * Allow to assign a name and custom attributes (like a static
       * address) to the message queue.
       *
       * To simplify access, the member variables are public and do not
       * require accessors or mutators.
       *
       * @par POSIX compatibility
       *  Inspired by `mq_attr` from [<mqueue.h>](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/mqueue.h.html)
       *  (IEEE Std 1003.1, 2013 Edition).
       *
       * @headerfile os.h <cmsis-plus/rtos/os.h>
       */
      class Attributes : public Named_object
      {
      public:

        /**
         * @name Constructors & Destructor
         * @{
         */

        /**
         * @brief Create message queue attributes.
         * @param [in] name Null terminated name. If `nullptr`, "-" is assigned.
         */
        Attributes (const char* name);

        /**
         * @cond ignore
         */
        Attributes (const Attributes&) = default;
        Attributes (Attributes&&) = default;
        Attributes&
        operator= (const Attributes&) = default;
        Attributes&
        operator= (Attributes&&) = default;
        /**
         * @endcond
         */

        /**
         * @brief Destroy the message queue attributes.
         */
        ~Attributes () = default;

        /**
         * @}
         */

      public:

        /**
         * @name Public Member Variables
         * @{
         */

        // Public members, no accessors and mutators required.
        // Warning: must match the type & order of the C file header.
        /**
         * @brief User defined message queue address.
         * @details
         * Set this variable to a user defined memory area large enough
         * to store the message queue. Usually this is a statically
         * allocated array of structures.
         *
         * The default value is `nullptr`, which means there is no
         * user defined message queue.
         */
        void* mq_queue_address;

        /**
         * @brief User defined message queue size.
         * @details
         * The message queue size must match exactly the allocated size. It is
         * used for validation; when the message queue is initialised,
         * this size must be large enough to accommodate the desired
         * message queue.
         *
         * If the @ref mq_queue_address is `nullptr`, this variable is not
         * checked, but it is recommended to leave it zero.
         */
        std::size_t mq_queue_size_bytes;

        // Add more attributes.

        /**
         * @}
         */
      };

      /**
       * @brief Default message queue initialiser.
       */
      extern const Attributes initializer;

    } /* namespace mqueue */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

    /**
     * @class Message_queue
     * @brief POSIX compliant message queue.
     * @details
     * Priority based, fixed size FIFO.
     *
     * @par POSIX compatibility
     *  Inspired by `mqd_t` from [<mqueue.h>](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/mqueue.h.html)
     *  (IEEE Std 1003.1, 2013 Edition).
     *
     * @headerfile os.h <cmsis-plus/rtos/os.h>
     */
    class Message_queue : public Named_object
    {
    public:

      /**
       * @name Constructors & Destructor
       * @{
       */

      /**
       * @brief Create a message queue with default settings.
       * @param [in] msgs The number of messages.
       * @param [in] msg_size_bytes The message size, in bytes.
       */
      Message_queue (mqueue::size_t msgs, mqueue::size_t msg_size_bytes);

      /**
       * @brief Create a message queue with custom settings.
       * @param [in] attr Reference to attributes.
       * @param [in] msgs The number of messages.
       * @param [in] msg_size_bytes The message size, in bytes.
       */
      Message_queue (const mqueue::Attributes&attr, mqueue::size_t msgs,
                     mqueue::size_t msg_size_bytes);

      /**
       * @cond ignore
       */
      Message_queue (const Message_queue&) = delete;
      Message_queue (Message_queue&&) = delete;
      Message_queue&
      operator= (const Message_queue&) = delete;
      Message_queue&
      operator= (Message_queue&&) = delete;
      /**
       * @endcond
       */

      /**
       * @brief Destroy the message queue.
       */
      ~Message_queue ();

      /**
       * @}
       * @name Operators
       * @{
       */

      /**
       * @brief Compare memory queues.
       * @retval true The given memory queue is the same as this memory queue.
       * @retval false The memory queues are different.
       */
      bool
      operator== (const Message_queue& rhs) const;

      /**
       * @}
       * @name Public Member Functions
       * @{
       */

      /**
       * @brief Send a message to the queue.
       * @param [in] msg The address of the message to enqueue.
       * @param [in] nbytes The length of the message. Must be not
       *  higher than the value used when creating the queue.
       * @param [in] mprio The message priority.
       * @retval result::ok The message was enqueued.
       * @retval EINVAL A parameter is invalid or outside of a permitted range.
       * @retval EMSGSIZE The specified message length, nbytes,
       *  exceeds the message size attribute of the message queue.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval ENOTRECOVERABLE The message could not be enqueue
       *  (extension to POSIX).
       * @retval EINTR The operation was interrupted.
       */
      result_t
      send (const char* msg, std::size_t nbytes, mqueue::priority_t mprio);

      /**
       * @brief Try to send a message to the queue.
       * @param [in] msg The address of the message to enqueue.
       * @param [in] nbytes The length of the message. Must be not
       *  higher than the value used when creating the queue.
       * @param [in] mprio The message priority.
       * @retval result::ok The message was enqueued.
       * @retval EAGAIN The specified message queue is full.
       * @retval EINVAL A parameter is invalid or outside of a permitted range.
       * @retval EMSGSIZE The specified message length, nbytes,
       *  exceeds the message size attribute of the message queue.
       * @retval ENOTRECOVERABLE The message could not be enqueue
       *  (extension to POSIX).
       */
      result_t
      try_send (const char* msg, std::size_t nbytes, mqueue::priority_t mprio);

      /**
       * @brief Send a message to the queue with timeout.
       * @param [in] msg The address of the message to enqueue.
       * @param [in] nbytes The length of the message. Must be not
       *  higher than the value used when creating the queue.
       * @param [in] mprio The message priority.
       * @param [in] ticks The timeout duration, in SysTick ticks.
       * @retval result::ok The message was enqueued.
       * @retval EINVAL A parameter is invalid or outside of a permitted range.
       * @retval EMSGSIZE The specified message length, nbytes,
       *  exceeds the message size attribute of the message queue.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval ETIMEDOUT The timeout expired before the message
       *  could be added to the queue.
       * @retval ENOTRECOVERABLE The message could not be enqueue
       *  (extension to POSIX).
       * @retval EINTR The operation was interrupted.
       */
      result_t
      timed_send (const char* msg, std::size_t nbytes, mqueue::priority_t mprio,
                  systicks_t ticks);

      /**
       * @brief Receive a message from the queue.
       * @param [out] msg The address where to store the dequeued message.
       * @param [in] nbytes The size of the destination buffer. Must
       *  be lower than the value used when creating the queue.
       * @param [out] mprio The address where to store the message
       *  priority. May be `nullptr`.
       * @retval result::ok The message was received.
       * @retval EINVAL A parameter is invalid or outside of a permitted range.
       * @retval EMSGSIZE The specified message length, nbytes, is
       *  greater than the message size attribute of the message queue.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval ENOTRECOVERABLE The message could not be dequeued
       *  (extension to POSIX).
       * @retval EBADMSG The implementation has detected a data corruption
       *  problem with the message.
       * @retval EINTR The operation was interrupted.
       */
      result_t
      receive (char* msg, std::size_t nbytes, mqueue::priority_t* mprio);

      /**
       * @brief Try to receive a message from the queue.
       * @param [out] msg The address where to store the dequeued message.
       * @param [in] nbytes The size of the destination buffer. Must
       *  be lower than the value used when creating the queue.
       * @param [out] mprio The address where to store the message
       *  priority. May be `nullptr`.
       * @retval result::ok The message was received.
       * @retval EINVAL A parameter is invalid or outside of a permitted range.
       * @retval EMSGSIZE The specified message length, nbytes, is
       *  greater than the message size attribute of the message queue.
       * @retval ENOTRECOVERABLE The message could not be dequeued
       *  (extension to POSIX).
       * @retval EBADMSG The implementation has detected a data corruption
       *  problem with the message.
       * @retval EAGAIN The specified message queue is empty.
       */
      result_t
      try_receive (char* msg, std::size_t nbytes, mqueue::priority_t* mprio);

      /**
       * @brief Receive a message from the queue with timeout.
       * @retval result::ok The message was received.
       * @retval EINVAL A parameter is invalid or outside of a permitted range.
       * @retval EMSGSIZE The specified message length, nbytes, is
       *  greater than the message size attribute of the message queue.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval ENOTRECOVERABLE The message could not be dequeued
       *  (extension to POSIX).
       * @retval EBADMSG The implementation has detected a data corruption
       *  problem with the message.
       * @retval EINTR The operation was interrupted.
       * @retval ETIMEDOUT No message arrived on the queue before the
       *  specified timeout expired.
       */
      result_t
      timed_receive (char* msg, std::size_t nbytes, mqueue::priority_t* mprio,
                     systicks_t ticks);

      /**
       * @brief Get queue capacity.
       * @par Parameters
       *  None
       * @return The max number of messages that can be queued.
       */
      std::size_t
      capacity (void) const;

      /**
       * @brief Get queue length.
       * @par Parameters
       *  None
       * @return The number of messages in the queue.
       */
      std::size_t
      length (void) const;

      /**
       * @brief Get message size.
       * @par Parameters
       *  None
       * @return The message size, in bytes.
       */
      std::size_t
      msg_size (void) const;

      /**
       * @brief Check if the queue is empty.
       * @par Parameters
       *  None
       * @retval true The queue has no messages.
       * @retval false The queue has some messages.
       */
      bool
      empty (void) const;

      /**
       * @brief Check if the queue is full.
       * @par Parameters
       *  None
       * @retval true The queue is full.
       * @retval false The queue is not full.
       */
      bool
      full (void) const;

      /**
       * @brief Reset the message queue.
       * @par Parameters
       *  None
       * @retval result::ok The queue was reset.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       */
      result_t
      reset (void);

      /**
       * @}
       */

    protected:

      /**
       * @name Private Member Variables
       * @{
       */

      // Keep these in sync with the structure declarations in os-c-decl.h.
#if !defined(OS_INCLUDE_PORT_RTOS_MESSAGE_QUEUE)
      port::Tasks_list send_list_;
      port::Tasks_list receive_list_;

#endif

      void* queue_addr_;

#if defined(OS_INCLUDE_PORT_RTOS_MESSAGE_QUEUE)
      friend class port::Message_queue;
      os_mqueue_port_data_t port_;
#endif

      std::size_t queue_size_bytes_;

      const mqueue::size_t msgs_;
      const mqueue::size_t msg_size_bytes_;

      mqueue::size_t count_;

      /**
       * @}
       */
    };

#pragma GCC diagnostic pop

    // ========================================================================

    /**
     * @namespace os::rtos::evflags
     * @brief Event flags namespace.
     * @details
     * The os::rtos::evflags namespace groups event flags attributes
     * and initialisers.
     */
    namespace evflags
    {
      /**
       * @class Attributes
       * @brief Event flags attributes.
       * @details
       * Allow to assign a name to the event flags.
       *
       * @par POSIX compatibility
       *  No POSIX similar functionality identified, but inspired by POSIX
       *  attributes used in [<pthread.h>](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html)
       *  (IEEE Std 1003.1, 2013 Edition).
       *
       * @headerfile os.h <cmsis-plus/rtos/os.h>
       */
      class Attributes : public Named_object
      {
      public:

        /**
         * @name Constructors & Destructor
         * @{
         */

        /**
         * @brief Create event flags attributes.
         * @param [in] name Null terminated name. If `nullptr`, "-" is assigned.
         */
        Attributes (const char* name);

        /**
         * @cond ignore
         */
        Attributes (const Attributes&) = default;
        Attributes (Attributes&&) = default;
        Attributes&
        operator= (const Attributes&) = default;
        Attributes&
        operator= (Attributes&&) = default;
        /**
         * @endcond
         */

        /**
         * @brief Destroy the event flags attributes.
         */
        ~Attributes () = default;

        /**
         * @}
         */

      public:

        /**
         * @name Public Member Variables
         * @{
         */

        // Public members, no accessors and mutators required.
        // Warning: must match the type & order of the C file header.
        //
        // Add more attributes.
        /**
         * @}
         */
      };

      /**
       * @brief Default event flags initialiser.
       */
      extern const Attributes initializer;

    } /* namespace mqueue */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

    /**
     * @class Event_flags
     * @brief Event flags.
     *
     * @details
     * Synchronised set of flags that can be used to notify events
     * between threads or between ISRs and threads.
     *
     * @par POSIX compatibility
     *  No POSIX similar functionality identified.
     *
     * @headerfile os.h <cmsis-plus/rtos/os.h>
     */
    class Event_flags : public Named_object
    {
    public:

      /**
       * @name Constructors & Destructor
       * @{
       */

      /**
       * @brief Create event flags with default attributes.
       * @par Parameters
       *  None
       */
      Event_flags ();

      /**
       * @brief Create event flags with custom settings.
       * @param [in] attr Reference to attributes.
       */
      Event_flags (const evflags::Attributes& attr);

      /**
       * @cond ignore
       */
      Event_flags (const Event_flags&) = delete;
      Event_flags (Event_flags&&) = delete;
      Event_flags&
      operator= (const Event_flags&) = delete;
      Event_flags&
      operator= (Event_flags&&) = delete;
      /**
       * @endcond
       */

      /**
       * @brief Destroy the event flags.
       */
      ~Event_flags ();

      /**
       * @}
       * @name Operators
       * @{
       */

      /**
       * @brief Compare event flags.
       * @retval true The given event flags object is the same as this
       *  event flags.
       * @retval false The event flags are different.
       */
      bool
      operator== (const Event_flags& rhs) const;

      /**
       * @}
       * @name Public Member Functions
       * @{
       */

      /**
       * @brief Wait for event flags.
       * @param [in] mask The expected flags (OR-ed bit-mask);
       *  may be zero.
       * @param [out] oflags Pointer where to store the current flags;
       *  may be `nullptr`.
       * @param [in] mode Mode bits to select if either all or any flags
       *  are expected, and if the flags should be cleared.
       * @retval result::ok All expected flags are raised.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval EINVAL The mask is outside of the permitted range.
       * @retval EINTR The operation was interrupted.
       * @retval ENOTRECOVERABLE Wait failed.
       */
      result_t
      wait (flags::mask_t mask, flags::mask_t* oflags, flags::mode_t mode);

      /**
       * @brief Try to wait for event flags.
       * @param [in] mask The expected flags (OR-ed bit-mask);
       *  may be zero.
       * @param [out] oflags Pointer where to store the current flags;
       *  may be `nullptr`.
       * @param [in] mode Mode bits to select if either all or any flags
       *  are expected, and if the flags should be cleared.
       * @retval result::ok All expected flags are raised.
       * @retval EINVAL The mask is outside of the permitted range.
       * @retval EAGAIN The expected condition did not occur.
       * @retval ENOTRECOVERABLE Wait failed.
       */
      result_t
      try_wait (flags::mask_t mask, flags::mask_t* oflags, flags::mode_t mode);

      /**
       * @brief Timed wait for signal flags.
       * @param [in] mask The expected flags (OR-ed bit-mask);
       *  may be zero.
       * @param [out] oflags Pointer where to store the current flags;
       *  may be `nullptr`.
       * @param [in] mode Mode bits to select if either all or any flags
       *  are expected, and if the flags should be cleared.
       * @param [in] ticks Ticks to wait.
       * @retval result::ok All expected flags are raised.
       * @retval EPERM Cannot be invoked from an Interrupt Service Routines.
       * @retval ETIMEDOUT The expected condition did not occur during the
       *  entire timeout duration.
       * @retval EINVAL The mask is outside of the permitted range.
       * @retval EINTR The operation was interrupted.
       * @retval ENOTRECOVERABLE Wait failed.
       */
      result_t
      timed_wait (flags::mask_t mask, flags::mask_t* oflags, flags::mode_t mode,
                  systicks_t ticks);

      /**
       * @brief Raise event flags.
       * @param [in] mask The OR-ed flags to raise.
       * @param [out] oflags Optional pointer where to store the
       *  new value of the flags; may be `nullptr`.
       * @retval result::ok The flags were raised.
       * @retval EINVAL The mask is zero.
       * @retval ENOTRECOVERABLE Raise failed.
       */
      result_t
      raise (flags::mask_t mask, flags::mask_t* oflags);

      /**
       * @brief Clear event flags.
       * @param [in] mask The OR-ed flags to clear.
       * @param [out] oflags Optional pointer where to store the
       *  previous value of the flags; may be `nullptr`.
       * @retval result::ok The flags were cleared.
       * @retval EINVAL The mask is zero.
       */
      result_t
      clear (flags::mask_t mask, flags::mask_t* oflags);

      /**
       * @brief Get/clear event flags.
       * @param [in] mask The OR-ed flags to get/clear; may be zero.
       * @param [in] mode Mode bits to select if the flags should be
       *  cleared (the other bits are ignored).
       * @return The selected bits from the flags mask.
       */
      flags::mask_t
      get (flags::mask_t mask, flags::mode_t mode);

      /**
       * @brief Check if some thread is waiting.
       * @par Parameters
       *  None
       * @retval true There are threads waiting.
       * @retval false There are no threads waiting.
       */
      bool
      waiting (void);

      /**
       * @}
       */

    protected:

      /**
       * @name Private Member Functions
       * @{
       */

      /**
       * @brief Internal function to check the flags condition.
       * @param [in] mask The expected flags (OR-ed bit-mask);
       *  may be zero.
       * @param [out] oflags Pointer where to store the current flags;
       *  may be `nullptr`.
       * @param [in] mode Mode bits to select if either all or any flags
       *  are expected, and if the flags should be cleared.
       * @retval result::ok All expected flags are raised.
       * @retval EINVAL The mask is outside of the permitted range.
       * @retval EAGAIN The expected condition did not occur.
       * @retval ENOTRECOVERABLE Wait failed.
       */
      result_t
      _try_wait (flags::mask_t mask, flags::mask_t* oflags, flags::mode_t mode);

      /**
       * @}
       */

    protected:

      /**
       * @name Private Member Variables
       * @{
       */

#if !defined(OS_INCLUDE_PORT_RTOS_EVENT_FLAGS)
      port::Tasks_list list_;
#endif

#if defined(OS_INCLUDE_PORT_RTOS_EVENT_FLAGS)
      friend class port::Event_flags;
      os_evflags_port_data_t port_;
#endif

      /**
       * @brief The event flags.
       */
      flags::mask_t flags_;

      /**
       * @}
       */
    };

#pragma GCC diagnostic pop

  } /* namespace rtos */
} /* namespace os */

// ===== Inline & template implementations ====================================

namespace os
{
  namespace rtos
  {
    // ========================================================================

    namespace scheduler
    {
      /**
       * @details
       * Check if the scheduler was started, i.e. if scheduler::start()
       * was called.
       */
      inline bool
      started (void)
      {
        return is_started_;
      }

      /**
       * @details
       * Check if the scheduler is locked on the current thread or
       * is switching threads from the ready list.
       */
      inline bool
      locked (void)
      {
        return is_locked_;
      }

      /**
       * @details
       * Lock the scheduler and remember the initial scheduler status.
       */
      inline
      Critical_section::Critical_section () :
          status_ (lock ())
      {
        ;
      }

      /**
       * @details
       * Restore the initial scheduler status and possibly unlock
       * the scheduler.
       */
      inline
      Critical_section::~Critical_section ()
      {
        unlock (status_);
      }

      constexpr
      Lock::Lock () :
          status_ (0)
      {
        ;
      }

      inline
      Lock::~Lock ()
      {
        ;
      }

      inline void
      Lock::lock (void)
      {
        status_ = scheduler::lock ();
      }

      inline bool
      Lock::try_lock (void)
      {
        status_ = scheduler::lock ();
        return true;
      }

      inline void
      Lock::unlock (void)
      {
        scheduler::unlock (status_);
      }

    } /* namespace scheduler */

    /**
     * @details
     * Round up the microseconds value and convert to a number of
     * ticks, using the SysTick frequency in Hz.
     */
    template<typename Rep_T>
      constexpr systicks_t
      Systick_clock::ticks_cast (Rep_T microsec)
      {
        // TODO: add some restrictions to match only numeric types
        return (systicks_t) ((((microsec) * ((Rep_T) frequency_hz))
            + (Rep_T) 999999ul) / (Rep_T) 1000000ul);
      }

    // ========================================================================

#if 0
    inline
    Named_object::Named_object (const char* name) :
    name_ (name != nullptr ? name : "-")
      {
        ;
      }
#endif

    /**
     * @details
     * All objects return a non-null string; anonymous objects
     * return `"-"`.
     */
    inline const char*
    Named_object::name (void) const
    {
      return name_;
    }

    // ========================================================================

    namespace interrupts
    {
      inline
      Critical_section::Critical_section () :
          status_ (enter ())
      {
        ;
      }

      inline
      Critical_section::~Critical_section ()
      {
        exit (status_);
      }

      constexpr
      Lock::Lock () :
          status_ (0)
      {
        ;
      }

      inline
      Lock::~Lock ()
      {
        ;
      }

      inline void
      Lock::lock (void)
      {
        status_ = Critical_section::enter ();
      }

      inline bool
      Lock::try_lock (void)
      {
        status_ = Critical_section::enter ();
        return true;
      }

      inline void
      Lock::unlock (void)
      {
        Critical_section::exit (status_);
      }

    }

    // ========================================================================

    namespace this_thread
    {
      /**
       * @details
       * Remove the current running thread from the ready list and pass
       * control to the next thread that is in **READY** state.
       *
       * @warning Cannot be invoked from Interrupt Service Routines.
       */
      inline void
      suspend (void)
      {
        this_thread::thread ().suspend ();
      }

      /**
       * @details
       *
       * @warning Cannot be invoked from Interrupt Service Routines.
       */
      inline result_t
      sig_wait (thread::sigset_t mask, thread::sigset_t* oflags,
                flags::mode_t mode)
      {
        return this_thread::thread ().sig_wait (mask, oflags, mode);
      }

      /**
       * @details
       *
       * @warning Cannot be invoked from Interrupt Service Routines.
       */
      inline result_t
      try_sig_wait (thread::sigset_t mask, thread::sigset_t* oflags,
                    flags::mode_t mode)
      {
        return this_thread::thread ().try_sig_wait (mask, oflags, mode);
      }

      /**
       * @details
       *
       * @warning Cannot be invoked from Interrupt Service Routines.
       */
      inline result_t
      timed_sig_wait (thread::sigset_t mask, thread::sigset_t* oflags,
                      flags::mode_t mode, systicks_t ticks)
      {
        return this_thread::thread ().timed_sig_wait (mask, oflags, mode, ticks);
      }

      inline void
      exit (void* exit_ptr)
      {
        return this_thread::thread ().exit (exit_ptr);
      }

    } /* namespace this_thread */

    namespace thread
    {
      inline
      Attributes::Attributes (const char* name) :
          Named_object (name)
      {
        th_stack_address = nullptr;
        th_stack_size_bytes = 0;
        th_priority = thread::priority::normal;
      }
    } /* namespace thread */

    /**
     * @details
     * Identical threads should have the same memory address.
     *
     * Compatible with POSIX pthread_equal().
     * http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_equal.html
     */
    inline bool
    Thread::operator== (const Thread& rhs) const
    {
      return this == &rhs;
    }

    inline thread::state_t
    Thread::sched_state (void) const
    {
      return sched_state_;
    }

    inline void*
    Thread::function_args (void) const
    {
      return func_args_;
    }

#if 0
    inline result_t
    Thread::wakeup_reason (void) const
      {
        return wakeup_reason_;
      }
#endif

    /**
     * @details
     * The user storage is a custom structure defined in `<os-app-config.h>`,
     * which is added to each and every thread storage. Applications
     * can store here any data.
     *
     * Inspired by (actually a generalisation of) µC-OS III task user
     * registers and FreeRTOS thread local storage, which proved useful
     * when implementing CMSIS+ over FreeRTOS.
     */
    inline os_thread_user_storage_t*
    Thread::user_storage (void)
    {
      return &user_storage_;
    }

    // ========================================================================

    namespace timer
    {
      inline
      Attributes::Attributes (const char* name) :
          Named_object
            { name }
      {
        this->tm_type = run::once;
      }

      // ======================================================================

      inline
      Periodic_attributes::Periodic_attributes (const char* name) :
          Attributes
            { name }
      {
        this->tm_type = run::periodic;
      }
    } /* namespace timer */

    // ========================================================================

    /**
     * @details
     * Identical timers should have the same memory address.
     */
    inline bool
    Timer::operator== (const Timer& rhs) const
    {
      return this == &rhs;
    }

    // ========================================================================

    namespace mutex
    {
      inline
      Attributes::Attributes (const char* name) :
          Named_object (name)
      {
        mx_priority_ceiling = thread::priority::highest;
        mx_protocol = protocol::none;
        mx_robustness = robustness::stalled;
        mx_type = type::normal;
      }

      inline
      Recursive_attributes::Recursive_attributes (const char* name) :
          Attributes (name)
      {
        mx_type = type::recursive;
      }

    }

    // ========================================================================

    /**
     * @details
     * Identical mutexes should have the same memory address.
     */
    inline bool
    Mutex::operator== (const Mutex& rhs) const
    {
      return this == &rhs;
    }

    inline Thread*
    Mutex::owner (void)
    {
      return owner_;
    }

    // ========================================================================

    namespace condvar
    {
      inline
      Attributes::Attributes (const char* name) :
          Named_object
            { name }
      {
        ;
      }
    } /* namespace condvar */

    // ========================================================================

    /**
     * @details
     * Identical condition variables should have the same memory address.
     */
    inline bool
    Condition_variable::operator== (const Condition_variable& rhs) const
    {
      return this == &rhs;
    }

    // ========================================================================

    namespace semaphore
    {
      inline
      Attributes::Attributes (const char* name) :
          Named_object
            { name }
      {
        sm_initial_count = 0;
        sm_max_count = max_count_value;
      }

      inline
      Binary_attributes::Binary_attributes (const char* name) :
          Attributes (name)
      {
        sm_initial_count = 0;
        sm_max_count = 1;
      }

    } /* namespace semaphore */

    // ========================================================================

    /**
     * @details
     * Identical semaphores should have the same memory address.
     */
    inline bool
    Semaphore::operator== (const Semaphore& rhs) const
    {
      return this == &rhs;
    }

    /**
     * @details
     * If positive, the semaphore value reflects the number of
     * available resources.
     *
     * If negative, it counts the waiting threads.
     */
    inline semaphore::count_t
    Semaphore::value (void) const
    {
      return count_;
    }

    inline semaphore::count_t
    Semaphore::initial_value (void) const
    {
      return initial_count_;
    }

    inline semaphore::count_t
    Semaphore::max_value (void) const
    {
      return max_count_;
    }

    // ========================================================================

    namespace mempool
    {
      inline
      Attributes::Attributes (const char* name) :
          Named_object (name)
      {
        mp_pool_address = nullptr;
      }

    } /* namespace mempool */

    // ========================================================================

    /**
     * @details
     * Identical memory pools should have the same memory address.
     */
    inline bool
    Memory_pool::operator== (const Memory_pool& rhs) const
    {
      return this == &rhs;
    }

    inline std::size_t
    Memory_pool::capacity (void) const
    {
      return blocks_;
    }

    inline std::size_t
    Memory_pool::block_size (void) const
    {
      return block_size_bytes_;
    }

    inline std::size_t
    Memory_pool::count (void) const
    {
      return count_;
    }

    inline bool
    Memory_pool::empty (void) const
    {
      return (count () == 0);
    }

    inline bool
    Memory_pool::full (void) const
    {
      return (count () == capacity ());
    }

    inline void*
    Memory_pool::pool (void)
    {
      return pool_addr_;
    }

    // ========================================================================

    namespace mqueue
    {
      inline
      Attributes::Attributes (const char* name) :
          Named_object (name)
      {
        queue_address = nullptr;
        queue_size_bytes = 0;
      }

    } /* namespace mqueue */

    // ========================================================================

    /**
     * @details
     * Identical message queues should have the same memory address.
     */
    inline bool
    Message_queue::operator== (const Message_queue& rhs) const
    {
      return this == &rhs;
    }

    inline std::size_t
    Message_queue::length (void) const
    {
      return count_;
    }

    inline std::size_t
    Message_queue::capacity (void) const
    {
      return msgs_;
    }

    inline std::size_t
    Message_queue::msg_size (void) const
    {
      return msg_size_bytes_;
    }

    inline bool
    Message_queue::empty (void) const
    {
      return (length () == 0);
    }

    inline bool
    Message_queue::full (void) const
    {
      return (length () == capacity ());
    }

    // ========================================================================

    namespace evflags
    {
      inline
      Attributes::Attributes (const char* name) :
          Named_object (name)
      {
        ;
      }

    } /* namespace evflags */

  // ------------------------------------------------------------------------

  } /* namespace rtos */
} /* namespace os */

// ----------------------------------------------------------------------------

extern "C"
{
  /**
   * @brief Main thread.
   * @param argc Count of arguments.
   * @param argv Array of pointers to arguments.
   * @retval 0 The proram terminated normally.
   * @retval 1 The program terminated with an error.
   *
   * @details
   * The standard `main()` creates a dedicated thread to run this function.
   *
   * The returned value is used in semihosted tests, to inform the
   * host on the result of the test.
   */
  int
  os_main (int argc, char* argv[]);

  /**
   * @brief SysTick interrupt handler.
   * @details
   * Must be called from the physical interrupt handler.
   */
  void
  os_systick_handler (void);

  /**
   * @brief SysTick implementation hook.
   * @details
   * It is called from @ref `os_systick_handler()` after the
   * scheduler was started.
   */
  void
  os_impl_systick_handler (void);

  /**
   * @brief RTC interrupt handler.
   * @details
   * Must be called from the physical RTC interrupt handler.
   */
  void
  os_rtc_handler (void);

  /**
   * @brief RTC implementation hook.
   */
  void
  os_impl_rtc_handler (void);
}

// ----------------------------------------------------------------------------

/**
 * @brief Assert or return an error.
 * @details
 * As required by the ANSI standards, if `NDEBUG` is defined,
 * the assertion is disabled and if the condition is true, the
 * given error code is returned.
 */
#ifdef NDEBUG           /* ANSI standard */
#define os_assert_err(__e, __er) \
  do { if (!(__e)) return __er; } while (false)
#else
#define os_assert_err(__e, __er) assert(__e)
#endif

/**
 * @brief Assert or throw a system error exception.
 * @details
 * As required by the ANSI standards, if `NDEBUG` is defined,
 * the assertion is disabled and if the condition is true, a
 * ssytem error exception is thrown (which is replaced by an
 * `abort()` if exceptions are disabled).
 */
#ifdef NDEBUG           /* ANSI standard */
#define os_assert_throw(__e, __er) \
  do { if (!(__e)) os::estd::__throw_system_error(__er, #__e); } while (false)
#else
#define os_assert_throw(__e, __er) assert(__e)
#endif

// ----------------------------------------------------------------------------

#if !defined(OS_INTEGER_SYSTICK_FREQUENCY_HZ)
/**
 * @brief Default definition for the SysTick frequency, in Hz.
 * @details
 * Redefine it in `<os-app-config.h>` to the actual value.
 */
#define OS_INTEGER_SYSTICK_FREQUENCY_HZ                     (1000)
#endif

#if !defined(OS_INTEGER_RTOS_MAIN_STACK_SIZE_BYTES)
/**
 * @brief Default definition for the `main()` stack size, in bytes.
 * @details
 * Redefine it in `<os-app-config.h>` to the actual value.
 */
#define OS_INTEGER_RTOS_MAIN_STACK_SIZE_BYTES               (400)
#endif

// ----------------------------------------------------------------------------

#endif /* __cplusplus */

#endif /* CMSIS_PLUS_RTOS_OS_H_ */
