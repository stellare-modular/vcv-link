/*

 VCV-Link by Stellare Modular
 Copyright (C) 2017-2018 - Vincenzo Pietropaolo, Sander Baan

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

#include "Link.hpp"

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

struct Link : Module
{
public:
	enum ParamIds
    {
        SYNC_PARAM = 0,
        OFFSET_PARAM,
        RATIO_PARAM,
        SWING_PARAM,
        NUM_PARAMS
    };

	enum InputIds
    {
        NUM_INPUTS = 0
	};

	enum OutputIds
    {
        CLOCK_OUTPUT_4TH = 0,
        RESET_OUTPUT,
        CLOCK_OUTPUT_2ND,
        BPM_OUTPUT,
        NUM_OUTPUTS
	};

	enum LightIds
    {
        CLOCK_LIGHT_4TH = 0,
        RESET_LIGHT,
        SYNC_LIGHT,
        CLOCK_LIGHT_2ND,
        NUM_LIGHTS
	};

    Link()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configParam(SYNC_PARAM, 0.0, 1.0, 0.0);
        configParam(SWING_PARAM, 0.0, 1.0, 0.0);
        configParam(RATIO_PARAM, 0.0, 6.0, 3.0);
        configParam(OFFSET_PARAM, -1.0, 1.0, 0.0);

        m_link = new ableton::Link(120.0);
        m_link->enable(true);
    }

    ~Link()
    {
        if (m_link)
        {
            m_link->enable(false);
            delete m_link;
        }
    }

    void process(const ProcessArgs& args) override;

private:
    void clampTick(int& tick, int maxTicks);

    ableton::Link* m_link;
    int m_lastTick = -1;
    bool m_synced = false;
};

void Link::clampTick(int& tick, int maxTicks)
{
    if (tick < 0)
        tick += maxTicks;

    tick %= maxTicks;
}

void Link::process(const ProcessArgs& args)
{
    // Tick length = 1 beat / 16ths / 2 (PWM 50%)
    static const double tick_length = (1.0 / 16.0) / 2.0;
    // We assume 4/4 signature
    static const double beats_per_bar = 4.0;
    static const int ticks_per_bar = static_cast<int>(beats_per_bar / tick_length);

	if (params[SYNC_PARAM].getValue() == 1.0)
	{
		m_synced = false;
	}

    double tempo = 0.0;
    double phase = 0.0;

    if (m_link)
    {
        const auto time = m_link->clock().micros();
        const auto timeline = m_link->captureAppSessionState();

        tempo = timeline.tempo();
        phase = timeline.phaseAtTime(time, beats_per_bar);
    }

    const double offset = params[OFFSET_PARAM].getValue() * (5.0 * tick_length);
    int tick = static_cast<int>(std::floor((phase + offset) / tick_length));

    clampTick(tick, ticks_per_bar);

    if (((tick >> 3) % 2) == 1)
    {
        const double max_swing_in_ticks = 3.0;

        const double swing = params[SWING_PARAM].getValue() * (max_swing_in_ticks * tick_length);
        tick = static_cast<int>(std::floor((phase + offset - swing) / tick_length));

        clampTick(tick, ticks_per_bar);
    }

    tick %= ticks_per_bar;

    if ((m_lastTick != tick) || !m_synced)
    {
        if (tick == 0)
            m_synced = true;

        if (m_synced)
        {
            int period_in_ticks = 0;
            switch (static_cast<int>(params[RATIO_PARAM].getValue()))
            {
                case 0: // 1 beat
                    period_in_ticks = 32;
                    break;

                case 1: // 1/2
                    period_in_ticks = 16;
                    break;

                case 2: // 1/4
                    period_in_ticks = 8;
                    break;

                case 3: // 1/6 (TO DO)
                    period_in_ticks = 32;
                    break;

                case 4: // 1/8
                    period_in_ticks = 4;
                    break;

                case 5: // 1/12 (TO DO)
                    period_in_ticks = 32;
                    break;

                case 6: // 1/16
                    period_in_ticks = 2;
                    break;
            }

            // clock has 50% PWM
            const bool clock_4th = ((tick % period_in_ticks) < (period_in_ticks / 2));
            outputs[CLOCK_OUTPUT_4TH].setVoltage(clock_4th ? 10.0 : 0.0);
            lights[CLOCK_LIGHT_4TH].setBrightness(clock_4th ? 1.0 : 0.0);

            outputs[CLOCK_OUTPUT_2ND].setVoltage(0.0);
            lights[CLOCK_LIGHT_2ND].setBrightness(0.0);

            // reset has 25% PWM
            const bool reset = ((tick % ticks_per_bar) < 2);
            outputs[RESET_OUTPUT].setVoltage(reset ? 10.0 : 0.0);
            lights[RESET_LIGHT].setBrightness(reset ? 1.0 : 0.0);

            outputs[BPM_OUTPUT].value = log2f(tempo / 120.f);
        }
        else
        {
            outputs[CLOCK_OUTPUT_4TH].setVoltage(0.0);
            lights[CLOCK_LIGHT_4TH].setBrightness(0.0);

            outputs[CLOCK_OUTPUT_2ND].setVoltage(0.0);
            lights[CLOCK_LIGHT_2ND].setBrightness(0.0);

            outputs[RESET_OUTPUT].setVoltage(0.0);
            lights[RESET_LIGHT].setBrightness(0.0);
        }

        m_lastTick = tick;
    }

    lights[SYNC_LIGHT].setBrightness(m_synced ? 0.0 : 1.0);
}

struct LinkWidget : ModuleWidget
{
    LinkWidget(Link*);
};

LinkWidget::LinkWidget(Link* module)
{
    setModule(module);
    box.size = Vec(60, 380);

    SvgPanel* panel = new SvgPanel();
    panel->box.size = box.size;
    panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Link.svg")));
    addChild(panel);

    addChild(createWidget<StellareScrew>(Vec(0, 0)));
    addChild(createWidget<StellareScrew>(Vec(box.size.x - 15, 365)));

    addParam(createParam<StellarePushButton>(Vec(19.5, 209.5), module, Link::SYNC_PARAM));
    addParam(createParam<StellareKnob01>(Vec(16.2, 57.5), module, Link::OFFSET_PARAM));
    addParam(createParam<StellareKnob01>(Vec(16.2, 108.2), module, Link::SWING_PARAM));
    addParam(createParam<StellareKnobSnap01>(Vec(16.2, 160.9), module, Link::RATIO_PARAM));

    addOutput(createOutput<StellareJack>(Vec(12.7, 305.2), module, Link::CLOCK_OUTPUT_4TH));
    addOutput(createOutput<StellareJack>(Vec(23.6, 266.8), module, Link::BPM_OUTPUT));
    addOutput(createOutput<StellareJack>(Vec(23.6, 342.2), module, Link::RESET_OUTPUT));

    addChild(createLight<SmallLight<BlueLight>>(Vec(27.5, 297.5), module, Link::CLOCK_LIGHT_4TH));
    addChild(createLight<SmallLight<YellowLight>>(Vec(27.5, 333.4), module, Link::RESET_LIGHT));
    addChild(createLight<SmallLight<BlueLight>>(Vec(27, 217), module, Link::SYNC_LIGHT));
}

Model *modelLink = createModel<Link, LinkWidget>("Link");
