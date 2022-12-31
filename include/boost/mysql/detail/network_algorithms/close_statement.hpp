//
// Copyright (c) 2019-2023 Ruben Perez Hidalgo (rubenperez038 at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MYSQL_DETAIL_NETWORK_ALGORITHMS_CLOSE_STATEMENT_HPP
#define BOOST_MYSQL_DETAIL_NETWORK_ALGORITHMS_CLOSE_STATEMENT_HPP

#include <boost/mysql/error.hpp>
#include <boost/mysql/statement_base.hpp>

#include <boost/mysql/detail/channel/channel.hpp>

namespace boost {
namespace mysql {
namespace detail {

template <class Stream>
void close_statement(
    channel<Stream>& chan,
    statement_base& stmt,
    error_code& code,
    error_info& info
);

template <
    class Stream,
    BOOST_ASIO_COMPLETION_TOKEN_FOR(void(::boost::mysql::error_code)) CompletionToken>
BOOST_ASIO_INITFN_AUTO_RESULT_TYPE(CompletionToken, void(error_code))
async_close_statement(
    channel<Stream>& chan,
    statement_base& stmt,
    CompletionToken&& token,
    error_info& info
);

}  // namespace detail
}  // namespace mysql
}  // namespace boost

#include <boost/mysql/detail/network_algorithms/impl/close_statement.hpp>

#endif /* INCLUDE_BOOST_MYSQL_DETAIL_NETWORK_ALGORITHMS_CLOSE_STATEMENT_HPP_ */
