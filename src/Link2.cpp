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

#include "Link2.hpp"
#include "LinkPeer.hpp"
#include "UIWidgets.hpp"

struct Link2 : Module
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

    Link2()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configParam(SYNC_PARAM, 0.0, 1.0, 0.0);
        configParam(SWING_PARAM, 0.0, 1.0, 0.0);
        configParam(RATIO_PARAM, 0.0, 4.0, 2.0);
        configParam(OFFSET_PARAM, -1.0, 1.0, 0.0);

        LinkPeer::attachModule();
    }

    ~Link2()
    {
        LinkPeer::detachModule();
    }

    void process(const ProcessArgs& args) override;

	json_t* dataToJson() override;
	void dataFromJson(json_t*) override;

    bool startStopEnabled();
    void setStartStopEnabled(bool value);

private:
    void clampTick(int& tick, int maxTicks);

    int m_lastTick = -1;
    bool m_synced = false;
    bool m_lastPlayingState = false;
    bool m_startStopEnabled = false;
};

void Link2::clampTick(int& tick, int maxTicks)
{
    if (tick < 0)
        tick += maxTicks;

    tick %= maxTicks;
}

void Link2::process(const ProcessArgs& args)
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
    bool playing = true;

    auto linkPeer = LinkPeer::get();

    if (linkPeer)
    {
        const auto time = linkPeer->clock().micros();
        const auto timeline = linkPeer->captureAppSessionState();

        tempo = timeline.tempo();
        phase = timeline.phaseAtTime(time, beats_per_bar);

        if (linkPeer->isStartStopSyncEnabled())
        {
            if (m_startStopEnabled)
            {
                playing = timeline.isPlaying();

                if (playing && !m_lastPlayingState)
                {
                    m_synced = false;
                }
            }

            m_lastPlayingState = playing;
        }
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

        if (m_synced && playing)
        {
            int period_in_ticks = 0;
            switch (static_cast<int>(params[RATIO_PARAM].getValue()))
            {
                case 0: // 4 beats
                    period_in_ticks = 128;
                    break;

                case 1: // 2 beats
                    period_in_ticks = 64;
                    break;

                case 2: // 1 beat
                    period_in_ticks = 32;
                    break;

                case 3: // 1/2 beat
                    period_in_ticks = 16;
                    break;

                case 4: // 1/4 beat
                    period_in_ticks = 8;
                    break;
            }

            // clock has 50% PWM
            const bool clock_4th = ((tick % period_in_ticks) < 2);
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

json_t* Link2::dataToJson()
{
	json_t* root = json_object();

	// Enable Start/Stop
    json_object_set_new(root, "enable_start_stop", json_boolean(startStopEnabled()));

	return root;
}

void Link2::dataFromJson(json_t* root)
{
	// Enable Start/Stop
    json_t* enable_start_stop = json_object_get(root, "enable_start_stop");

    if (enable_start_stop)
	{
		setStartStopEnabled(json_boolean_value(enable_start_stop));
	}
}

bool Link2::startStopEnabled()
{
    return m_startStopEnabled;
}

void Link2::setStartStopEnabled(bool value)
{
    m_startStopEnabled = value;
}

// -------------------------------------------------------------

struct Link2StartStopMenuItem : rack::MenuItem
{
	Link2* module;

    void onAction(const rack::event::Action&) override
    {
        const bool enable = module->startStopEnabled();
        module->setStartStopEnabled(!enable);
    }

    void step() override
    {
        rightText = (module->startStopEnabled() ? "✔" : "");
        rack::MenuItem::step();
    }
};

struct Link2Widget : ModuleWidget
{
    Link2Widget(Link2*);
    void appendContextMenu(rack::ui::Menu*) override;
};

Link2Widget::Link2Widget(Link2* module)
{
    setModule(module);
    box.size = Vec(60, 380);

    SvgPanel* panel = new SvgPanel();
    panel->box.size = box.size;
    panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Link2.svg")));
    addChild(panel);

    addChild(createWidget<StellareScrew>(Vec(0, 0)));
    addChild(createWidget<StellareScrew>(Vec(box.size.x - 15, 365)));

    addParam(createParam<StellarePushButton>(Vec(19.5, 209.5), module, Link2::SYNC_PARAM));
    addParam(createParam<StellareKnob01>(Vec(16.2, 57.5), module, Link2::OFFSET_PARAM));
    addParam(createParam<StellareKnob01>(Vec(16.2, 108.2), module, Link2::SWING_PARAM));
    addParam(createParam<StellareKnobSnap01>(Vec(16.2, 160.9), module, Link2::RATIO_PARAM));

    addOutput(createOutput<StellareJack>(Vec(12.7, 305.2), module, Link2::CLOCK_OUTPUT_4TH));
    addOutput(createOutput<StellareJack>(Vec(23.6, 266.8), module, Link2::BPM_OUTPUT));
    addOutput(createOutput<StellareJack>(Vec(23.6, 342.2), module, Link2::RESET_OUTPUT));

    addChild(createLight<SmallLight<BlueLight>>(Vec(27.5, 297.5), module, Link2::CLOCK_LIGHT_4TH));
    addChild(createLight<SmallLight<YellowLight>>(Vec(27.5, 333.4), module, Link2::RESET_LIGHT));
    addChild(createLight<SmallLight<BlueLight>>(Vec(27, 217), module, Link2::SYNC_LIGHT));
}

void Link2Widget::appendContextMenu(rack::ui::Menu* menu)
{
	Link2* module = dynamic_cast<Link2*>(this->module);

	menu->addChild(construct<MenuSeparator>());
	menu->addChild(construct<Link2StartStopMenuItem>(&Link2StartStopMenuItem::text, "Enable Link Start/Stop", &Link2StartStopMenuItem::module, module));
}

Model *modelLink2 = createModel<Link2, Link2Widget>("Link2");
