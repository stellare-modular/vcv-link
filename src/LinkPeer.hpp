/*

 VCV-Link by Stellare Modular
 Copyright (C) 2017-2020 - Vincenzo Pietropaolo, Sander Baan
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
*/

#pragma once

// Macros named "defer", "debug" and "info" are defined both in Rack and ASIO
// standalone headers, here we undefine the Rack definitions which stay unused.
#undef defer
#undef debug
#undef info

#if LINK_PLATFORM_WINDOWS
#include <stdint.h>
#include <stdlib.h>
#define htonll(x) _byteswap_uint64(x)
#define ntohll(x) _byteswap_uint64(x)
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-override"
#include <ableton/Link.hpp>
#pragma GCC diagnostic pop

// Handling a single instance of ableton::Link
// for all instancess of Link and Link2 modules

namespace LinkPeer {

    ableton::Link* get();

    void attachModule();
    void detachModule();

} // namespace LinkPeer
