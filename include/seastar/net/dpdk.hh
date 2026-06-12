/*
 * This file is open source software, licensed to you under the terms
 * of the Apache License, Version 2.0 (the "License").  See the NOTICE file
 * distributed with this work for additional information regarding copyright
 * ownership.  You may not use this file except in compliance with the License.
 *
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (C) 2014 Cloudius Systems, Ltd.
 */

#pragma once

#include <memory>
#include <functional>
#include <seastar/net/config.hh>
#include <seastar/net/net.hh>
#include <seastar/core/sstring.hh>
#include <seastar/util/program-options.hh>

namespace seastar {

namespace net {

/// DPDK configuration.
struct dpdk_options : public program_options::option_group {
    /// DPDK Port Index.
    ///
    /// Default: 0.
    program_options::value<unsigned> dpdk_port_index;
    /// \brief Enable HW Flow Control (on / off).
    ///
    /// Default: \p on.
    program_options::value<std::string> hw_fc;
    /// \brief Extra EAL arguments passed verbatim to rte_eal_init().
    ///
    /// Space-separated list of EAL flags appended after Seastar's own
    /// generated arguments, e.g. "--no-pci --vdev=net_af_xdp0,iface=eth0".
    program_options::value<std::string> dpdk_extra_eal_args;

    /// \cond internal
    dpdk_options(program_options::option_group* parent_group);
    /// \endcond
};

}

/// \cond internal

#ifdef SEASTAR_HAVE_DPDK

std::unique_ptr<net::device> create_dpdk_net_device(
                                    uint16_t port_idx = 0,
                                    uint16_t num_queues = 1,
                                    bool use_lro = true,
                                    bool enable_fc = true);

std::unique_ptr<net::device> create_dpdk_net_device(
                                    const net::hw_config& hw_cfg);

namespace dpdk {
/**
 * @return Number of bytes needed for mempool objects of each QP.
 */
uint32_t qp_mempool_obj_size(bool hugetlbfs_membackend);

/**
 * Install a per-port cross-shard steering callback.  The function receives a
 * TCP destination port (host byte order) and returns the target shard id, or
 * 0xFF (XDP_NO_SHARD) when no steering is needed.  Must be called before the
 * first packet is received.
 */
void set_port_to_shard_fn(std::function<uint8_t(uint16_t)> fn);
}

/// \endcond

#endif // SEASTAR_HAVE_DPDK

}
