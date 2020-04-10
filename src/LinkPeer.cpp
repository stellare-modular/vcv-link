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

#include "LinkPeer.hpp"

#include <atomic>

static ableton::Link* g_link = nullptr;
static std::atomic<int> g_instances(0);

namespace LinkPeer {

ableton::Link* get()
{
    return g_link;
}

void attachModule()
{
    if ((++g_instances == 1) && (g_link == nullptr))
    {
        g_link = new ableton::Link(120.0);

        g_link->enable(true);
        g_link->enableStartStopSync(true);
    }
}

void detachModule()
{
    if ((--g_instances == 0) && (g_link != nullptr))
    {
        g_link->enable(false);
        g_link->enableStartStopSync(false);

        delete g_link;
        g_link = nullptr;
    }
}

} // namespace LinkPeer
