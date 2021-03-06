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

#ifndef CMSIS_PLUS_POSIX_IO_DEVICE_CHAR_REGISTRY_H_
#define CMSIS_PLUS_POSIX_IO_DEVICE_CHAR_REGISTRY_H_

#if defined(__cplusplus)

// ----------------------------------------------------------------------------

#include <cmsis-plus/posix-io/device-char.h>

#include <cstddef>
#include <cassert>

// ----------------------------------------------------------------------------

namespace os
{
  namespace posix
  {
    // ------------------------------------------------------------------------

    /**
     * @brief Char devices registry static class.
     * @headerfile device-char-registry.h <cmsis-plus/posix-io/device-char-registry.h>
     * @ingroup cmsis-plus-posix-io-base
     */
    class device_char_registry
    {
      // ----------------------------------------------------------------------

      /**
       * @name Constructors & Destructor
       * @{
       */

    public:

      // Do not allow to create instances of this class.
      device_char_registry () = delete;

      /**
       * @cond ignore
       */

      // The rule of five.
      device_char_registry (const device_char_registry&) = delete;
      device_char_registry (device_char_registry&&) = delete;
      device_char_registry&
      operator= (const device_char_registry&) = delete;
      device_char_registry&
      operator= (device_char_registry&&) = delete;

      /**
       * @endcond
       */

      ~device_char_registry () = delete;

      /**
       * @}
       */

      // ----------------------------------------------------------------------
      /**
       * @name Public Static Member Functions
       * @{
       */

    public:

      static void
      link (device_char* device);

      static device_char*
      identify_device (const char* path);

      /**
       * @}
       */

      // ----------------------------------------------------------------------
    private:

      /**
       * @cond ignore
       */

      // Since devices may be constructed statically, so may ask
      // to be linked here at any time, this list must be initialised
      // before any static constructor.
      // With the order of static constructors unknown, this means it
      // must be allocated in the BSS and will be initialised to 0 by
      // the startup code.
      using device_list = utils::intrusive_list<device_char,
      utils::double_list_links, &device_char::registry_links_>;
      static device_list registry_list__;

      /**
       * @endcond
       */
    };

  } /* namespace posix */
} /* namespace os */

// ----------------------------------------------------------------------------

#endif /* __cplusplus */

#endif /* CMSIS_PLUS_POSIX_IO_DEVICE_CHAR_REGISTRY_H_ */
