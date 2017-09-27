# NAME

`mk/options.hpp`

# LIBRARY

measurement-kit (`libmeasurement_kit`, `-lmeasurement_kit`)

# SYNOPSIS

```C++
#ifndef MK_OPTIONS_HPP
#define MK_OPTIONS_HPP

#define MK_DNS_NAMESERVER_HINT "dns/nameserver"

#define MK_DNS_ENGINE "dns/engine"

/* TODO: define more options */

#endif
```

# DESCRIPTION

The `mk/options.hpp` header defines all the available MK options. Rather than hardcoding option names, you should use the corresponding defines. 

Changing the defines names will bump the API. Changing the corresponding strings instead will bump the ABI.

The `MK_DNS_NAMERSERVER_HINT` option is used to indicate what nameserver should be used to resolve hostnames. Depending on the DNS engine in use, it may not be possible to honour this option. In such case, a warning will be emitted, but the execution will contonue anyway.

The `MK_DNS_ENGINE` option allows you to specify the engine to use. If the requested engine is not available, all DNS queries will fail.

