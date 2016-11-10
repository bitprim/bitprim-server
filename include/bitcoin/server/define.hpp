/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin-server.
 *
 * libbitcoin-server is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SERVER_DEFINE_HPP
#define LIBBITCOIN_SERVER_DEFINE_HPP

#include <bitcoin/bitcoin.hpp>

// We use the generic helper definitions in libbitcoin to define BCS_API 
// and BCS_INTERNAL. BCS_API is used for the public API symbols. It either DLL
// imports or DLL exports (or does nothing for static build) BCS_INTERNAL is 
// used for non-api symbols.

#if defined BCS_STATIC
    #define BCS_API
    #define BCS_INTERNAL
#elif defined BCS_DLL
    #define BCS_API      BC_HELPER_DLL_EXPORT
    #define BCS_INTERNAL BC_HELPER_DLL_LOCAL
#else
    #define BCS_API      BC_HELPER_DLL_IMPORT
    #define BCS_INTERNAL BC_HELPER_DLL_LOCAL
#endif

// Log name.
#define LOG_SERVER "server"

// Avoid namespace conflict between boost::placeholders and std::placeholders. 
#define BOOST_BIND_NO_PLACEHOLDERS

// Include boost only here, so placeholders exclusion works.
#include <boost/filesystem.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/program_options.hpp>
#include <boost/thread.hpp>

namespace libbitcoin {
namespace server {

typedef bc::message::header_message::ptr header_ptr;
typedef bc::message::header_message::const_ptr header_const_ptr;

typedef bc::message::inventory::ptr inventory_ptr;
typedef bc::message::inventory::const_ptr inventory_const_ptr;

typedef bc::message::merkle_block::ptr merkle_block_ptr;
typedef bc::message::merkle_block::const_ptr merkle_block_const_ptr;

typedef bc::message::block_message::ptr block_ptr;
typedef bc::message::block_message::const_ptr block_const_ptr;
typedef bc::message::block_message::ptr_list block_ptr_list;
typedef bc::message::block_message::const_ptr_list block_const_ptr_list;

typedef bc::message::transaction_message::ptr transaction_ptr;
typedef bc::message::transaction_message::const_ptr transaction_const_ptr;


} // namespace server
} // namespace libbitcoin

#endif
