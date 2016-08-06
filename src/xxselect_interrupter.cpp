//
// detail/impl/socket_select_interrupter.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_IMPL_XXSOCKET_SELECT_INTERRUPTER_IPP
#define BOOST_ASIO_DETAIL_IMPL_XXSOCKET_SELECT_INTERRUPTER_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)
#include "xxselect_interrupter.hpp"

namespace purelib {
    namespace inet {

        xxselect_interrupter::xxselect_interrupter()
        {
            open_descriptors();
        }

        void xxselect_interrupter::open_descriptors()
        {
            xxsocket acceptor(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            acceptor.set_optval(SOL_SOCKET, SO_REUSEADDR, 1);

            ip::endpoint ep("127.0.0.1", 0);

            acceptor.bind(ep);
            ep = acceptor.local_endpoint();
            // Some broken firewalls on Windows will intermittently cause getsockname to
            // return 0.0.0.0 when the socket is actually bound to 127.0.0.1. We
            // explicitly specify the target address here to work around this problem.
            // addr.sin_addr.s_addr = socket_ops::host_to_network_long(INADDR_LOOPBACK);
            // cp.address("127.0.0.1");
            ep.address("127.0.0.1");
            acceptor.listen();

            xxsocket client(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            client.connect(ep);

            auto server = acceptor.accept();

            client.set_nonblocking(true);
            client.set_optval(IPPROTO_TCP, TCP_NODELAY, 1);

            server.set_nonblocking(true);
            client.set_optval(IPPROTO_TCP, TCP_NODELAY, 1);

            read_descriptor_ = server.release();
            write_descriptor_ = client.release();
        }

        xxselect_interrupter::~xxselect_interrupter()
        {
            close_descriptors();
        }

        void xxselect_interrupter::close_descriptors()
        {
            /*boost::system::error_code ec;
            socket_ops::state_type state = socket_ops::internal_non_blocking;
            if (read_descriptor_ != invalid_socket)
                socket_ops::close(read_descriptor_, state, true, ec);
            if (write_descriptor_ != invalid_socket)
                socket_ops::close(write_descriptor_, state, true, ec);*/

            if (read_descriptor_ != invalid_socket)
                ::closesocket(read_descriptor_);

            if (write_descriptor_ != invalid_socket)
                ::closesocket(write_descriptor_);
        }

        void xxselect_interrupter::recreate()
        {
            close_descriptors();

            write_descriptor_ = invalid_socket;
            read_descriptor_ = invalid_socket;

            open_descriptors();
        }

        void xxselect_interrupter::interrupt()
        {
            /*char byte = 0;
            socket_ops::buf b;
            socket_ops::init_buf(b, &byte, 1);
            boost::system::error_code ec;
            socket_ops::send(write_descriptor_, &b, 1, 0, ec);*/
            xxsocket::send_i(write_descriptor_, "\0", 1);
        }

        bool xxselect_interrupter::reset()
        {
            char buffer[1024];
            /*socket_ops::buf b;
            socket_ops::init_buf(b, data, sizeof(data));
            boost::system::error_code ec;*/
            int bytes_read = xxsocket::recv_i(read_descriptor_, buffer, sizeof(buffer), 0);
            bool was_interrupted = (bytes_read > 0);
            while (bytes_read == sizeof(buffer))
                bytes_read = xxsocket::recv_i(read_descriptor_, buffer, sizeof(buffer), 0);
            return was_interrupted;
        }

    } // namespace detail
} // namespace asio

#endif // BOOST_ASIO_DETAIL_IMPL_XXSOCKET_SELECT_INTERRUPTER_IPP
