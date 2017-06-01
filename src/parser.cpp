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
#include <bitcoin/server/parser.hpp>

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <bitcoin/node.hpp>
#include <bitcoin/server/define.hpp>
#include <bitcoin/server/parser.hpp>
#include <bitcoin/server/settings.hpp>

BC_DECLARE_CONFIG_DEFAULT_PATH("libbitcoin" / "bs.cfg")

// TODO: localize descriptions.

namespace libbitcoin {
namespace server {

using namespace boost::filesystem;
using namespace boost::program_options;
using namespace bc::config;
using namespace bc::network;

// Initialize configuration by copying the given instance.
parser::parser(const configuration& defaults)
  : configured(defaults)
{
}

// Initialize configuration using defaults of the given context.
parser::parser(bc::config::settings context)
  : configured(context)
{
    // Logs will slow things if not rotated.
    configured.network.rotation_size = 10000000;

    // With block-first sync the count should be low until complete.
    configured.network.outbound_connections = 2;

    // A node allows 1000 host names by default.
    configured.network.host_pool_capacity = 1000;

    // A node exposes full node (1) network services by default.
    configured.network.services = message::version::service::node_network;

    // TODO: set this independently on each public endpoint.
    configured.protocol.message_size_limit = max_block_size + 100;
}

options_metadata parser::load_options()
{
    options_metadata description("options");
    description.add_options()
    (
        BS_CONFIG_VARIABLE ",c",
        value<path>(&configured.file),
        "Specify path to a configuration settings file."
    )
    (
        BS_HELP_VARIABLE ",h",
        value<bool>(&configured.help)->
            default_value(false)->zero_tokens(),
        "Display command line options."
    )

#if !defined(WITH_REMOTE_BLOCKCHAIN) && !defined(WITH_REMOTE_DATABASE)
    (
        "initchain,i",
        value<bool>(&configured.initchain)->
            default_value(false)->zero_tokens(),
        "Initialize blockchain in the configured directory."
    )
#endif // !defined(WITH_REMOTE_BLOCKCHAIN) && !defined(WITH_REMOTE_DATABASE)

    (
        BS_SETTINGS_VARIABLE ",s",
        value<bool>(&configured.settings)->
            default_value(false)->zero_tokens(),
        "Display all configuration settings."
    )
    (
        BS_VERSION_VARIABLE ",v",
        value<bool>(&configured.version)->
            default_value(false)->zero_tokens(),
        "Display version information."
    );

    return description;
}

arguments_metadata parser::load_arguments()
{
    arguments_metadata description;
    return description
        .add(BS_CONFIG_VARIABLE, 1);
}

options_metadata parser::load_environment()
{
    options_metadata description("environment");
    description.add_options()
    (
        // For some reason po requires this to be a lower case name.
        // The case must match the other declarations for it to compose.
        // This composes with the cmdline options and inits to system path.
        BS_CONFIG_VARIABLE,
        value<path>(&configured.file)->composing()
            ->default_value(config_default_path()),
        "The path to the configuration settings file."
    );

    return description;
}

options_metadata parser::load_settings()
{
    options_metadata description("settings");
    description.add_options()
    /* [log] */
    (
        "log.debug_file",
        value<path>(&configured.network.debug_file),
        "The debug log file path, defaults to 'debug.log'."
    )
    (
        "log.error_file",
        value<path>(&configured.network.error_file),
        "The error log file path, defaults to 'error.log'."
    )
    (
        "log.archive_directory",
        value<path>(&configured.network.archive_directory),
        "The log archive directory, defaults to 'archive'."
    )
    (
        "log.rotation_size",
        value<size_t>(&configured.network.rotation_size),
        "The size at which a log is archived, defaults to 10000000 (0 disables)."
    )
    (
        "log.minimum_free_space",
        value<size_t>(&configured.network.minimum_free_space),
        "The minimum free space required in the archive directory, defaults to 0."
    )
    (
        "log.maximum_archive_size",
        value<size_t>(&configured.network.maximum_archive_size),
        "The maximum combined size of archived logs, defaults to 0 (maximum)."
    )
    (
        "log.maximum_archive_files",
        value<size_t>(&configured.network.maximum_archive_files),
        "The maximum number of logs to archive, defaults to 0 (maximum)."
    )
    (
        "log.statistics_server",
        value<config::authority>(&configured.network.statistics_server),
        "The address of the statistics collection server, defaults to none."
    )
    (
        "log.verbose",
        value<bool>(&configured.network.verbose),
        "Enable verbose logging, defaults to false."
    )
    /* [network] */
    (
        "network.threads",
        value<uint32_t>(&configured.network.threads),
        "The minimum number of threads in the network threadpool, defaults to 0 (physical cores)."
    )
    (
        "network.protocol_maximum",
        value<uint32_t>(&configured.network.protocol_maximum),
        "The maximum network protocol version, defaults to 70013."
    )
    (
        "network.protocol_minimum",
        value<uint32_t>(&configured.network.protocol_minimum),
        "The minimum network protocol version, defaults to 31402."
    )
    (
        "network.services",
        value<uint64_t>(&configured.network.services),
        "The services exposed by network connections, defaults to 1 (full node)."
    )
    (
        "network.validate_checksum",
        value<bool>(&configured.network.validate_checksum),
        "Validate the checksum of network messages, defaults to false."
    )
    (
        "network.identifier",
        value<uint32_t>(&configured.network.identifier),
        "The magic number for message headers, defaults to 3652501241."
    )
    (
        "network.inbound_port",
        value<uint16_t>(&configured.network.inbound_port),
        "The port for incoming connections, defaults to 8333."
    )
    (
        "network.inbound_connections",
        value<uint32_t>(&configured.network.inbound_connections),
        "The target number of incoming network connections, defaults to 0."
    )
    (
        "network.outbound_connections",
        value<uint32_t>(&configured.network.outbound_connections),
        "The target number of outgoing network connections, defaults to 2."
    )
    (
        "network.manual_attempt_limit",
        value<uint32_t>(&configured.network.manual_attempt_limit),
        "The attempt limit for manual connection establishment, defaults to 0 (forever)."
    )
    (
        "network.connect_batch_size",
        value<uint32_t>(&configured.network.connect_batch_size),
        "The number of concurrent attempts to establish one connection, defaults to 5."
    )
    (
        "network.connect_timeout_seconds",
        value<uint32_t>(&configured.network.connect_timeout_seconds),
        "The time limit for connection establishment, defaults to 5."
    )
    (
        "network.channel_handshake_seconds",
        value<uint32_t>(&configured.network.channel_handshake_seconds),
        "The time limit to complete the connection handshake, defaults to 30."
    )
    (
        "network.channel_heartbeat_minutes",
        value<uint32_t>(&configured.network.channel_heartbeat_minutes),
        "The time between ping messages, defaults to 5."
    )
    (
        "network.channel_inactivity_minutes",
        value<uint32_t>(&configured.network.channel_inactivity_minutes),
        "The inactivity time limit for any connection, defaults to 30."
    )
    (
        "network.channel_expiration_minutes",
        value<uint32_t>(&configured.network.channel_expiration_minutes),
        "The age limit for any connection, defaults to 60."
    )
    (
        "network.channel_germination_seconds",
        value<uint32_t>(&configured.network.channel_germination_seconds),
        "The time limit for obtaining seed addresses, defaults to 30."
    )
    (
        "network.host_pool_capacity",
        value<uint32_t>(&configured.network.host_pool_capacity),
        "The maximum number of peer hosts in the pool, defaults to 1000."
    )
    (
        "network.hosts_file",
        value<path>(&configured.network.hosts_file),
        "The peer hosts cache file path, defaults to 'hosts.cache'."
    )
    (
        "network.self",
        value<config::authority>(&configured.network.self),
        "The advertised public address of this node, defaults to none."
    )
    (
        "network.blacklist",
        value<config::authority::list>(&configured.network.blacklists),
        "IP address to disallow as a peer, multiple entries allowed."
    )
    (
        "network.peer",
        value<config::endpoint::list>(&configured.network.peers),
        "A persistent peer node, multiple entries allowed."
    )
    (
        "network.seed",
        value<config::endpoint::list>(&configured.network.seeds),
        "A seed node for initializing the host pool, multiple entries allowed."
    )

    /* [database] */
    (
        "database.directory",
        value<path>(&configured.database.directory),
        "The blockchain database directory, defaults to 'blockchain'."
    )
    (
        "database.flush_writes",
        value<bool>(&configured.database.flush_writes),
        "Flush each write to disk, defaults to false."
    )
    (
        "database.file_growth_rate",
        value<uint16_t>(&configured.database.file_growth_rate),
        "Full database files increase by this percentage, defaults to 50."
    )
    (
        "database.block_table_buckets",
        value<uint32_t>(&configured.database.block_table_buckets),
        "Block hash table size, defaults to 650000."
    )
    (
        "database.transaction_table_buckets",
        value<uint32_t>(&configured.database.transaction_table_buckets),
        "Transaction hash table size, defaults to 110000000."
    )
    (
        "database.transaction_unconfirmed_table_buckets",
        value<uint32_t>(&configured.database.transaction_unconfirmed_table_buckets),
        "Unconfirmed Transaction hash table size, defaults to 10000."
    )
    (
        "database.spend_table_buckets",
        value<uint32_t>(&configured.database.spend_table_buckets),
        "Spend hash table size, defaults to 250000000."
    )
    (
        "database.history_table_buckets",
        value<uint32_t>(&configured.database.history_table_buckets),
        "History hash table size, defaults to 107000000."
    )
    (
        "database.cache_capacity",
        value<uint32_t>(&configured.database.cache_capacity),
        "The maximum number of entries in the unspent outputs cache, defaults to 10000."
    )
#if defined(WITH_REMOTE_DATABASE)    
    (
        "database.replier",
        value<config::endpoint>(&configured.database.replier),
        "Database-blockchain connection, defaults to 127.0.0.1:5568."
    )    
#endif //defined(WITH_REMOTE_DATABASE)    

    /* [blockchain] */
    (
        "blockchain.cores",
        value<uint32_t>(&configured.chain.cores),
        "The number of cores dedicated to block validation, defaults to 0 (physical cores)."
    )
    (
        "blockchain.priority",
        value<bool>(&configured.chain.priority),
        "Use high thread priority for block validation, defaults to true."
    )
    (
        "blockchain.use_libconsensus",
        value<bool>(&configured.chain.use_libconsensus),
        "Use libconsensus for script validation if integrated, defaults to false."
    )
    (
        "blockchain.reorganization_limit",
        value<uint32_t>(&configured.chain.reorganization_limit),
        "The maximum reorganization depth, defaults to 256 (0 for unlimited)."
    )
    (
        "blockchain.checkpoint",
        value<config::checkpoint::list>(&configured.chain.checkpoints),
        "A hash:height checkpoint, multiple entries allowed."
    )
#if defined(WITH_REMOTE_BLOCKCHAIN)
    (
        "blockchain.replier",
        value<config::endpoint>(&configured.chain.replier),
        "Blockchain Replier connect() endpoint."
    )
#endif // defined(WITH_REMOTE_BLOCKCHAIN)

    /* [fork] */
    (
        "fork.easy_blocks",
        value<bool>(&configured.chain.easy_blocks),
        "Allow minimum difficulty blocks, defaults to false."
    )
    (
        "fork.bip16",
        value<bool>(&configured.chain.bip16),
        "Add pay-to-script-hash processing, defaults to true (soft fork)."
    )
    (
        "fork.bip30",
        value<bool>(&configured.chain.bip30),
        "Disallow collision of unspent transaction hashes, defaults to true (hard fork)."
    )
    (
        "fork.bip34",
        value<bool>(&configured.chain.bip34),
        "Coinbase input must include block height, defaults to true (soft fork)."
    )
    (
        "fork.bip66",
        value<bool>(&configured.chain.bip66),
        "Require strict signature encoding, defaults to true (soft fork)."
    )
    (
        "fork.bip65",
        value<bool>(&configured.chain.bip65),
        "Add check locktime verify op code, defaults to true (soft fork)."
    )
    (
        "fork.bip90",
        value<bool>(&configured.chain.bip90),
        "Assume bip34, bip65, and bip66 activation if enabled, defaults to true (hard fork)."
    )



    /* [node] */
    ////(
    ////    "node.sync_peers",
    ////    value<uint32_t>(&configured.node.sync_peers),
    ////    "The maximum number of initial block download peers, defaults to 0 (physical cores)."
    ////)
    ////(
    ////    "node.sync_timeout_seconds",
    ////    value<uint32_t>(&configured.node.sync_timeout_seconds),
    ////    "The time limit for block response during initial block download, defaults to 5."
    ////)
    (
        "node.block_latency_seconds",
        value<uint32_t>(&configured.node.block_latency_seconds),
        "The time to wait for a requested block, defaults to 60."
    )
    (
        /* Internally this is blockchain, but it is conceptually a node setting. */
        "node.notify_limit_hours",
        value<uint32_t>(&configured.chain.notify_limit_hours),
        "Disable relay when top block age exceeds, defaults to 24 (0 disables)."
    )
    (
        /* Internally this is blockchain, but it is conceptually a node setting. */
        "node.byte_fee_satoshis",
        value<float>(&configured.chain.byte_fee_satoshis),
        "The minimum fee per byte, cumulative for conflicts, defaults to 1."
    )
    (
        /* Internally this is blockchain, but it is conceptually a node setting. */
        "node.sigop_fee_satoshis",
        value<float>(&configured.chain.sigop_fee_satoshis),
        "The minimum fee per sigop, additional to byte fee, defaults to 100."
    )
    (
        /* Internally this is blockchain, but it is conceptually a node setting. */
        "node.minimum_output_satoshis",
        value<uint64_t>(&configured.chain.minimum_output_satoshis),
        "The minimum output value, defaults to 500."
    )
    (
        /* Internally this is network, but it is conceptually a node setting. */
        "node.relay_transactions",
        value<bool>(&configured.network.relay_transactions),
        "Request that peers relay transactions, defaults to false."
    )
    (
        "node.refresh_transactions",
        value<bool>(&configured.node.refresh_transactions),
        "Request transactions on each channel start, defaults to true."
    )

    /* [server] */
    (
        /* Internally this is database, but it applies to server and not node. */
        "server.index_start_height",
        value<uint32_t>(&configured.database.index_start_height),
        "The lower limit of address and spend indexing, defaults to 0."
    )
    /* Internally this is protocol, but application to server is more intuitive. */
    (
        "server.send_high_water",
        value<uint32_t>(&configured.protocol.send_high_water),
        "Drop messages at this outgoing backlog level, defaults to 100."
    )
    /* Internally this is protocol, but application to server is more intuitive. */
    (
        "server.receive_high_water",
        value<uint32_t>(&configured.protocol.receive_high_water),
        "Drop messages at this incoming backlog level, defaults to 100."
    )
    /* Internally this is protocol, but application to server is more intuitive. */
    (
        "server.handshake_seconds",
        value<uint32_t>(&configured.protocol.handshake_seconds),
        "The time limit to complete the connection handshake, defaults to 30."
    )
    (
        "server.secure_only",
        value<bool>(&configured.server.secure_only),
        "Disable public endpoints, defaults to false."
    )
    (
        "server.query_workers",
        value<uint16_t>(&configured.server.query_workers),
        "The number of query worker threads per endpoint, defaults to 1 (0 disables service)."
    )
    (
        "server.subscription_limit",
        value<uint32_t>(&configured.server.subscription_limit),
        "The maximum number of query subscriptions, defaults to 1000 (0 disables subscribe)."
    )
    (
        "server.subscription_expiration_minutes",
        value<uint32_t>(&configured.server.subscription_expiration_minutes),
        "The query subscription expiration time, defaults to 10 (0 disables expiration)."
    )
    (
        "server.heartbeat_service_seconds",
        value<uint32_t>(&configured.server.heartbeat_service_seconds),
        "The heartbeat service interval, defaults to 5 (0 disables service)."
    )
    (
        "server.block_service_enabled",
        value<bool>(&configured.server.block_service_enabled),
        "Enable the block publishing service, defaults to false."
    )
    (
        "server.transaction_service_enabled",
        value<bool>(&configured.server.transaction_service_enabled),
        "Enable the transaction publishing service, defaults to false."
    )
    (
        "server.secure_query_endpoint",
        value<endpoint>(&configured.server.secure_query_endpoint),
        "The secure query endpoint, defaults to 'tcp://*:9081'."
    )
    (
        "server.secure_heartbeat_endpoint",
        value<endpoint>(&configured.server.secure_heartbeat_endpoint),
        "The secure heartbeat endpoint, defaults to 'tcp://*:9082'."
    )
    (
        "server.secure_block_endpoint",
        value<endpoint>(&configured.server.secure_block_endpoint),
        "The secure block publishing endpoint, defaults to 'tcp://*:9083'."
    )
    (
        "server.secure_transaction_endpoint",
        value<endpoint>(&configured.server.secure_transaction_endpoint),
        "The secure transaction publishing endpoint, defaults to 'tcp://*:9084'."
    )
    (
        "server.public_query_endpoint",
        value<endpoint>(&configured.server.public_query_endpoint),
        "The public query endpoint, defaults to 'tcp://*:9091'."
    )
    (
        "server.public_heartbeat_endpoint",
        value<endpoint>(&configured.server.public_heartbeat_endpoint),
        "The public heartbeat endpoint, defaults to 'tcp://*:9092'."
    )
    (
        "server.public_block_endpoint",
        value<endpoint>(&configured.server.public_block_endpoint),
        "The public block publishing endpoint, defaults to 'tcp://*:9093'."
    )
    (
        "server.public_transaction_endpoint",
        value<endpoint>(&configured.server.public_transaction_endpoint),
        "The public transaction publishing endpoint, defaults to 'tcp://*:9094'."
    )
    (
        "server.server_private_key",
        value<config::sodium>(&configured.server.server_private_key),
        "The Z85-encoded private key of the server, enables secure endpoints."
    )
    (
        "server.client_public_key",
        value<config::sodium::list>(&configured.server.client_public_keys),
        "Allowed Z85-encoded public key of the client, multiple entries allowed."
    )
    (
        "server.client_address",
        value<config::authority::list>(&configured.server.client_addresses),
        "Allowed client IP address, multiple entries allowed."
    )
    (
        "server.blacklist",
        value<config::authority::list>(&configured.server.blacklists),
        "Blocked client IP address, multiple entries allowed."
    )
#ifdef WITH_LOCAL_MINING
    /* [mining] */
    (
            "mining.cores",
            value<uint32_t>(&configured.mining.cores),
            //"The number of threads in the mining threadpool, defaults to 50."
            "The number of cores dedicated to ????? validation, defaults to 0 (physical cores)."
    )

    (
            "mining.block_timeout_seconds",
            value<uint32_t>(&configured.mining.block_timeout_seconds),
            "The time limit for block receipt during initial block download, defaults to 5."
    )
    (
            "mining.initial_connections",
            value<uint32_t>(&configured.mining.initial_connections),
            "The maximum number of connections for initial block download, defaults to 8."
    )
    (
            "mining.transaction_pool_refresh",
            value<bool>(&configured.mining.transaction_pool_refresh),
            "Refresh the transaction pool on reorganization and channel start, defaults to true."
    )
    (
            "mining.use_testnet_rules",
            value<bool>(&configured.mining.use_testnet_rules),
            "Use testnet rules for determination of work required, defaults to false."
    )
    (
            "mining.rpc_port",
            value<uint32_t>(&configured.mining.rpc_port),
            "TCP port for the HTTP-JSON-RPC connection, default to 8332 (8332 mainnet, 9332 testnet)."
    )
    (
            "mining.real_consensus_enabled",
            value<bool>(&configured.mining.real_consensus_enabled),
            "Enable real consensus mining."
    )
    (
            "mining.real_consensus_endpoint",
            value<bc::config::endpoint>(&configured.mining.real_consensus_endpoint)->default_value({ "localhost:19999" }),
            "The real consensus client endpoint."
    )
    (
            "mining.real_consensus_user",
            value<std::string>(&configured.mining.real_consensus_user),
            "The real consensus client user."
    )
    (
            "mining.real_consensus_pass",
            value<std::string>(&configured.mining.real_consensus_pass),
            "The real consensus client authorization pass."
    )

    /* [server] */
    (
            "server.url",
            value<bc::config::endpoint>(&configured.mining.server_url)->default_value({ "tcp://127.0.0.1:9091" }),
            "The URL of the Libbitcoin/Obelisk server."
    )
    (
            "server.socks_proxy",
            value<bc::config::authority>(&configured.mining.server_socks_proxy)->default_value({ "0.0.0.0:0" }),
            "The address of a SOCKS5 proxy to use, defaults to none."
    )
    (
            "server.connect_retries",
            value<mining::config::byte>(&configured.mining.server_connect_retries)->default_value(0),
            "The number of times to retry contacting a server, defaults to 0."
    )
    (
            "server.connect_timeout_seconds",
            value<uint16_t>(&configured.mining.server_connect_timeout_seconds)->default_value(5),
            "The time limit for connection establishment, defaults to 5."
    )
    (
            "server.server_public_key",
            value<bc::config::sodium>(&configured.mining.server_server_public_key),
            "The Z85-encoded public key of the server."
    )
    (
            "server.client_private_key",
            value<bc::config::sodium>(&configured.mining.server_client_private_key),
            "The Z85-encoded private key of the client."
    )
#endif
    ;

    return description;
}

bool parser::parse(int argc, const char* argv[], std::ostream& error)
{
    try
    {
        auto file = false;
        variables_map variables;
        load_command_variables(variables, argc, argv);
        load_environment_variables(variables, BS_ENVIRONMENT_VARIABLE_PREFIX);

        // Don't load the rest if any of these options are specified.
        if (!get_option(variables, BS_VERSION_VARIABLE) &&
            !get_option(variables, BS_SETTINGS_VARIABLE) &&
            !get_option(variables, BS_HELP_VARIABLE))
        {
            // Returns true if the settings were loaded from a file.
            file = load_configuration_variables(variables, BS_CONFIG_VARIABLE);
        }

        // Update bound variables in metadata.settings.
        notify(variables);

        // Clear the config file path if it wasn't used.
        if (!file)
            configured.file.clear();
    }
    catch (const boost::program_options::error& e)
    {
        // This is obtained from boost, which circumvents our localization.
        error << format_invalid_parameter(e.what()) << std::endl;
        return false;
    }

    return true;
}

} // namespace server
} // namespace libbitcoin
