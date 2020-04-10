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

#include "rack.hpp"

using namespace rack;

extern Plugin* pluginInstance;

struct StellarePushButton : SvgSwitch
{
    StellarePushButton()
    {
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/stellare_Button.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/stellare_Button_push.svg")));

        sw->wrap();
        momentary = true;
        box.size = sw->box.size;
    }
};

struct StellareKnob01 : SvgKnob
{
    StellareKnob01()
    {
        box.size = Vec(28, 28);
        minAngle = -0.82 * M_PI;
        maxAngle = 0.82 * M_PI;

        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/stellare_Knob_M.svg")));
        shadow->opacity = 0.f;
    }
};

struct StellareKnobSnap01 : SvgKnob
{
    StellareKnobSnap01()
    {
        box.size = Vec(28, 28);
        minAngle = -0.5 * M_PI;
        maxAngle = 0.5 * M_PI;

        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/stellare_Knob_M.svg")));
        shadow->opacity = 0.f;

        snap = true;
        smooth = false;
    }
};

struct StellareJack : SvgPort
{
    StellareJack()
    {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/stellare_Jack.svg")));
        shadow->opacity = 0.f;
    }
};

struct StellareScrew : SvgScrew
{
    StellareScrew()
    {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/stellare_Screw.svg")));
    }
};
