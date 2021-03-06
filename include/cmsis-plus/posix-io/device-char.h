/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2015 Liviu Ionescu.
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

#ifndef CMSIS_PLUS_POSIX_IO_DEVICE_CHAR_H_
#define CMSIS_PLUS_POSIX_IO_DEVICE_CHAR_H_

#if defined(__cplusplus)

// ----------------------------------------------------------------------------

#include <cmsis-plus/posix-io/io.h>
#include <cmsis-plus/utils/lists.h>

// ----------------------------------------------------------------------------

#if ! defined(OS_STRING_POSIX_DEVICE_PREFIX)
#define OS_STRING_POSIX_DEVICE_PREFIX "/dev/"
#endif

// ----------------------------------------------------------------------------

namespace os
{
  namespace posix
  {
    // ------------------------------------------------------------------------

    /**
     * @brief Char device class.
     * @headerfile device-char.h <cmsis-plus/posix-io/device-char.h>
     * @ingroup cmsis-plus-posix-io-base
     */
    class device_char : public io
    {
      // ----------------------------------------------------------------------

      /**
       * @cond ignore
       */

      friend io*
      vopen (const char* path, int oflag, std::va_list args);

      /**
       * @endcond
       */

      // ----------------------------------------------------------------------
      /**
       * @name Constructors & Destructor
       * @{
       */

    public:

      device_char (const char* name);

      /**
       * @cond ignore
       */

      // The rule of five.
      device_char (const device_char&) = delete;
      device_char (device_char&&) = delete;
      device_char&
      operator= (const device_char&) = delete;
      device_char&
      operator= (device_char&&) = delete;

      /**
       * @endcond
       */

      virtual
      ~device_char ();

      /**
       * @}
       */

      /**
       * @name Public Member Functions
       * @{
       */

    public:

      int
      ioctl (int request, ...);

      int
      vioctl (int request, std::va_list args);

      // ----------------------------------------------------------------------

      virtual bool
      match_name (const char* name) const;

      const char*
      name (void) const;

      static const char*
      device_prefix (void);

      /**
       * @}
       */

      // ----------------------------------------------------------------------
      /**
       * @name Private Member Functions
       * @{
       */

    protected:

      virtual int
      do_vopen (const char* path, int oflag, std::va_list args) = 0;

      virtual int
      do_vioctl (int request, std::va_list args);

      virtual int
      do_isatty (void) override;

      /**
       * @}
       */

      // ----------------------------------------------------------------------
    public:

      /**
       * @cond ignore
       */

      // Intrusive node used to link this device to the registry list.
      // Must be public.
      utils::double_list_links registry_links_;

      /**
       * @endcond
       */

    protected:

      /**
       * @cond ignore
       */

      const char* name_;

      /**
       * @endcond
       */

    };
  } /* namespace posix */
} /* namespace os */

// ===== Inline & template implementations ====================================

namespace os
{
  namespace posix
  {
    // ------------------------------------------------------------------------

    inline const char*
    device_char::device_prefix (void)
    {
      return OS_STRING_POSIX_DEVICE_PREFIX;
    }

    inline const char*
    device_char::name (void) const
    {
      return name_;
    }

  } /* namespace posix */
} /* namespace os */

// ----------------------------------------------------------------------------

#endif /* __cplusplus */

#endif /* CMSIS_PLUS_POSIX_IO_DEVICE_CHAR_H_ */
