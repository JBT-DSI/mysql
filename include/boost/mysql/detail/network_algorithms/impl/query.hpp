//
// Copyright (c) 2019-2023 Ruben Perez Hidalgo (rubenperez038 at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_MYSQL_DETAIL_NETWORK_ALGORITHMS_IMPL_QUERY_HPP
#define BOOST_MYSQL_DETAIL_NETWORK_ALGORITHMS_IMPL_QUERY_HPP

#pragma once

#include <boost/mysql/detail/network_algorithms/query.hpp>
#include <boost/mysql/detail/network_algorithms/read_all_rows.hpp>
#include <boost/mysql/detail/network_algorithms/start_query.hpp>

namespace boost {
namespace mysql {
namespace detail {

template <class Stream>
struct query_op : boost::asio::coroutine
{
    channel<Stream>& chan_;
    error_info& output_info_;
    boost::string_view query_;
    resultset& output_;

    query_op(
        channel<Stream>& chan,
        error_info& output_info,
        boost::string_view q,
        resultset& output
    ) noexcept
        : chan_(chan), output_info_(output_info), query_(q), output_(output)
    {
    }

    template <class Self>
    void operator()(Self& self, error_code err = {})
    {
        // Error checking
        if (err)
        {
            self.complete(err);
            return;
        }

        // Normal path
        BOOST_ASIO_CORO_REENTER(*this)
        {
            BOOST_ASIO_CORO_YIELD
            async_start_query(chan_, query_, output_.state(), output_info_, std::move(self));

            BOOST_ASIO_CORO_YIELD async_read_all_rows(
                chan_,
                output_.state(),
                output_.mutable_rows(),
                output_info_,
                std::move(self)
            );

            self.complete(error_code());
        }
    }
};

}  // namespace detail
}  // namespace mysql
}  // namespace boost

template <class Stream>
void boost::mysql::detail::query(
    channel<Stream>& channel,
    boost::string_view query,
    resultset& output,
    error_code& err,
    error_info& info
)
{
    start_query(channel, query, output.state(), err, info);
    if (err)
        return;

    read_all_rows(channel, output.state(), output.mutable_rows(), err, info);
}

template <
    class Stream,
    BOOST_ASIO_COMPLETION_TOKEN_FOR(void(::boost::mysql::error_code)) CompletionToken>
BOOST_ASIO_INITFN_AUTO_RESULT_TYPE(CompletionToken, void(boost::mysql::error_code))
boost::mysql::detail::async_query(
    channel<Stream>& chan,
    boost::string_view query,
    resultset& output,
    error_info& info,
    CompletionToken&& token
)
{
    return boost::asio::async_compose<CompletionToken, void(boost::mysql::error_code)>(
        query_op<Stream>(chan, info, query, output),
        token,
        chan
    );
}

#endif /* INCLUDE_BOOST_MYSQL_DETAIL_NETWORK_ALGORITHMS_IMPL_EXECUTE_QUERY_HPP_ */
