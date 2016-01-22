/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2016 Liviu Ionescu.
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

#include <cmsis-plus/drivers/serial.h>
#include <cmsis-plus/drivers/usart-wrapper.h>

#include <cassert>

using namespace os::cmsis::driver;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"

class TestSerial : public Serial
{
public:

  TestSerial ();

  virtual
  ~TestSerial () override;

protected:

  virtual const Version&
  do_get_version (void) noexcept override;

  virtual return_t
  do_power (Power state) noexcept override;

  virtual const serial::Capabilities&
  do_get_capabilities (void) noexcept override;

  virtual return_t
  do_send (const void* data, std::size_t num) noexcept override;

  virtual return_t
  do_receive (void* data, std::size_t num) noexcept override;

  virtual return_t
  do_transfer (const void* data_out, void* data_in, std::size_t num)
      noexcept override;

  virtual std::size_t
  do_get_tx_count (void) noexcept override;

  virtual std::size_t
  do_get_rx_count (void) noexcept override;

  virtual return_t
  do_configure (uint32_t ctrl, uint32_t arg) noexcept override;

  virtual return_t
  do_control (serial::control_t ctrl) noexcept override;

  virtual serial::Status&
  do_get_status (void) noexcept override;

  virtual return_t
  do_control_modem_line (serial::Modem_control ctrl) noexcept override;

  virtual serial::Modem_status&
  do_get_modem_status (void) noexcept override;

private:
  Version version_
    { 1, 2 }; //
  serial::Capabilities capabilities_
    { true, 0, true, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, true, 0, true };

};

#pragma GCC diagnostic pop

TestSerial::TestSerial ()
{
}

TestSerial::~TestSerial ()
{
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

const Version&
TestSerial::do_get_version (void) noexcept
{
  return version_;
}

const serial::Capabilities&
TestSerial::do_get_capabilities (void) noexcept
{
  return capabilities_;
}

return_t
TestSerial::do_power (Power state) noexcept
{
  return RETURN_OK;
}

return_t
TestSerial::do_send (const void* data, std::size_t num) noexcept
{
  return RETURN_OK;
}

return_t
TestSerial::do_receive (void* data, std::size_t num) noexcept
{
  return RETURN_OK;
}

return_t
TestSerial::do_transfer (const void* data_out, void* data_in, std::size_t num) noexcept
{
  return RETURN_OK;
}

std::size_t
TestSerial::do_get_tx_count (void) noexcept
{
  return 0;
}

std::size_t
TestSerial::do_get_rx_count (void) noexcept
{
  return 0;
}

return_t
TestSerial::do_configure (serial::config_t cfg, serial::config_arg_t arg) noexcept
{
  return RETURN_OK;
}

return_t
TestSerial::do_control (serial::control_t ctrl) noexcept
{
  return RETURN_OK;
}

serial::Status&
TestSerial::do_get_status (void) noexcept
{
  return status_;
}

return_t
TestSerial::do_control_modem_line (serial::Modem_control ctrl) noexcept
{
  return RETURN_OK;
}

serial::Modem_status&
TestSerial::do_get_modem_status (void) noexcept
{
  return modem_status_;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------

extern ARM_DRIVER_USART test_usart;

void
test_serial (void);

static TestSerial device;

static Usart_wrapper wrap
  { &test_usart, nullptr };

void
test_serial (void)
{
  serial::Capabilities capa;
  capa = device.get_capabilities ();
  assert(capa.asynchronous);
  assert(!capa.synchronous_master);
  assert(capa.synchronous_slave);
  assert(!capa.single_wire);
  assert(!capa.irda);
  assert(!capa.smart_card);
  assert(!capa.smart_card_clock);
  assert(!capa.flow_control_rts);

  assert(!capa.flow_control_cts);
  assert(!capa.event_tx_complete);
  assert(!capa.event_rx_timeout);
  assert(!capa.rts);
  assert(!capa.cts);
  assert(!capa.dtr);
  assert(!capa.dsr);
  assert(!capa.dcd);

  assert(!capa.ri);
  assert(!capa.event_cts);
  assert(capa.event_dsr);
  assert(!capa.event_dcd);
  assert(capa.event_ri);

  const serial::Capabilities& caparef = device.get_capabilities ();
  assert(caparef.asynchronous);
  assert(!caparef.synchronous_master);
  assert(caparef.synchronous_slave);
  assert(!caparef.single_wire);
  assert(!caparef.irda);
  assert(!caparef.smart_card);
  assert(!caparef.smart_card_clock);
  assert(!caparef.flow_control_rts);

  assert(!caparef.flow_control_cts);
  assert(!caparef.event_tx_complete);
  assert(!caparef.event_rx_timeout);
  assert(!caparef.rts);
  assert(!caparef.cts);
  assert(!caparef.dtr);
  assert(!caparef.dsr);
  assert(!caparef.dcd);

  assert(!caparef.ri);
  assert(!caparef.event_cts);
  assert(caparef.event_dsr);
  assert(!caparef.event_dcd);
  assert(caparef.event_ri);

  // --------------------------------------------------------------------------

  Version version = wrap.get_version ();
  assert(version.get_api () == 0x1234);
  assert(version.get_drv () == 0x5678);

  capa = wrap.get_capabilities ();
  assert(capa.asynchronous);
  assert(!capa.synchronous_master);
  assert(capa.synchronous_slave);
  assert(!capa.single_wire);
  assert(!capa.irda);
  assert(capa.smart_card);
  assert(!capa.smart_card_clock);
  assert(capa.flow_control_rts);

  assert(capa.flow_control_cts);
  assert(capa.event_tx_complete);
  assert(capa.event_rx_timeout);
  assert(capa.rts);
  assert(!capa.cts);
  assert(!capa.dtr);
  assert(!capa.dsr);
  assert(!capa.dcd);

  assert(capa.ri);
  assert(!capa.event_cts);
  assert(capa.event_dsr);
  assert(!capa.event_dcd);
  assert(capa.event_ri);

  serial::Status status = wrap.get_status ();

  assert(status.is_tx_busy ());
  assert(!status.is_rx_busy ());
  assert(status.is_tx_underflow ());
  assert(status.is_rx_overflow ());
  assert(!status.is_rx_break ());
  assert(!status.is_rx_framing_error ());
  assert(status.is_rx_parity_error ());

  serial::Modem_status modem_status = wrap.get_modem_status ();
  assert(modem_status.is_cts_active ());
  assert(!modem_status.is_dsr_active ());
  assert(modem_status.is_dcd_active ());
  assert(modem_status.is_ri_active ());

  assert(wrap.power (Power::full) == 11);
  assert(wrap.power (Power::off) == 22);
  assert(wrap.power (Power::low) == 33);

  char buf[10];
  assert(wrap.send (buf, 12) == 12);
  assert(wrap.receive (buf, 13) == 13);
  assert(wrap.transfer (buf, buf, 14) == 14);
  assert(wrap.get_tx_count () == 78);
  assert(wrap.get_rx_count () == 87);
  assert(wrap.configure (22, 33) == 55);
  assert(wrap.control_modem_line (serial::Modem_control::clear_rts) == 21);
  assert(wrap.control_modem_line (serial::Modem_control::set_rts) == 22);
  assert(wrap.control_modem_line (serial::Modem_control::clear_dtr) == 23);
  assert(wrap.control_modem_line (serial::Modem_control::set_dtr) == 24);

}