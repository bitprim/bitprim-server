/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SERVER_ROUTE
#define LIBBITCOIN_SERVER_ROUTE

#include <cstddef>
#include <string>
#include <boost/functional/hash_fwd.hpp>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/server/define.hpp>

namespace libbitcoin {
namespace server {

/// This class is not thread safe.
/// The route is fixed in compliance with v2/v3 limitations.
class BCS_API route
{
public:
    /// Construct a route.
    route();

    /// A printable address for logging only.
    std::string display() const;

    /// Equality operator.
    bool operator==(const route& other) const;

    /// The message requires a secure port.
    bool secure;

    /// The message route is delimited using an empty frame.
    bool delimited;

    /// The first address.
    data_chunk address1;

    /// The second address.
    data_chunk address2;
};

} // namespace server
} // namespace libbitcoin

namespace std
{
    template<>
    struct hash<bc::server::route>
    {
        size_t operator()(const bc::server::route& value) const
        {
            size_t seed = 0;
            boost::hash_combine(seed, value.secure);
            ////boost::hash_combine(seed, value.delimited);
            boost::hash_combine(seed, value.address1);
            ////boost::hash_combine(seed, value.address2);
            return seed;
        }
    };
} // namespace std

#endif
