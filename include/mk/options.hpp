// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software under the BSD license. See AUTHORS
// and LICENSE for more information on the copying conditions.
#ifndef MK_OPTIONS_HPP
#define MK_OPTIONS_HPP

// The `mk/options.hpp` header defines all the available MK options. Rather
// than hardcoding option names, you should use the corresponding defines.
//
// Changing the defines names will bump the API. Changing the corresponding
// strings instead will bump the ABI.

// The `MK_DNS_NAMERSERVER_HINT` option is used to indicate what nameserver
// should be used to resolve hostnames. Depending on the DNS engine in use, it
// may not be possible to honour this option. In such case, a warning will be
// emitted, but the execution will contonue anyway.
#define MK_DNS_NAMESERVER_HINT "dns/nameserver"

// The `MK_DNS_ENGINE` option allows you to specify the engine to use. If the
// requested engine is not available, all DNS queries will fail.
#define MK_DNS_ENGINE "dns/engine"

/* TODO: define more options */

#endif
