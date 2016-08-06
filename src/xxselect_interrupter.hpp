//
// detail/socket_select_interrupter.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_XXSOCKET_SELECT_INTERRUPTER_HPP
#define BOOST_ASIO_DETAIL_XXSOCKET_SELECT_INTERRUPTER_HPP
#include "xxsocket.h"
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#define BOOST_ASIO_DECL

namespace purelib {
namespace inet {

    class xxselect_interrupter
{
public:
  // Constructor.
    BOOST_ASIO_DECL xxselect_interrupter();

  // Destructor.
    BOOST_ASIO_DECL ~xxselect_interrupter();

  // Recreate the interrupter's descriptors. Used after a fork.
  BOOST_ASIO_DECL void recreate();

  // Interrupt the select call.
  BOOST_ASIO_DECL void interrupt();

  // Reset the select interrupt. Returns true if the call was interrupted.
  BOOST_ASIO_DECL bool reset();

  // Get the read descriptor to be passed to select.
  socket_native_type read_descriptor() const
  {
    return read_descriptor_;
  }

private:
  // Open the descriptors. Throws on error.
  BOOST_ASIO_DECL void open_descriptors();

  // Close the descriptors.
  BOOST_ASIO_DECL void close_descriptors();

  // The read end of a connection used to interrupt the select call. This file
  // descriptor is passed to select such that when it is time to stop, a single
  // byte will be written on the other end of the connection and this
  // descriptor will become readable.
  socket_native_type read_descriptor_;

  // The write end of a connection used to interrupt the select call. A single
  // byte may be written to this to wake up the select which is waiting for the
  // other end to become readable.
  socket_native_type write_descriptor_;
};

} // namespace inet
} // namespace purelib


#if 1
// # include "xxselect_interrupter.ipp"
#endif // defined(BOOST_ASIO_HEADER_ONLY)

#endif // BOOST_ASIO_DETAIL_SOCKET_SELECT_INTERRUPTER_HPP
