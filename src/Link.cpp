/*
 
 VCV-Link by Stellare Modular
 Copyright (C) 2017 - Vincenzo Pietropaolo, Sander Baan
 
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

#include <link-wrapper.h>
#include <iostream>

struct Link : Module
{
public:
	enum ParamIds
    {
        SYNC_PARAM = 0,
        OFFSET_PARAM,
        SWING_PARAM,
        NUM_PARAMS
    };

	enum InputIds
    {
        NUM_INPUTS = 0
	};

	enum OutputIds
    {
        CLOCK_OUTPUT = 0,
		RESET_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightIds
    {
        CLOCK_LIGHT = 0,
        RESET_LIGHT,
        SYNC_LIGHT,
        NUM_LIGHTS
	};

    Link() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
    {
		m_link = create_link_wrapper();
    }

    ~Link()
    {
		delete_link_wrapper(m_link);
    }

    void step() override;

private:
	link_handle* m_link;
    int m_lastTick = -1;
    bool m_synced = false;
};

void Link::step()
{
    // Tick length = 1 beat / 16ths / 2 (PWM 50%)
    static const double tick_length = (1.0 / 16.0) / 2.0;
    // We assume 4/4 signature
    static const double beats_per_bar = 4.0;
    static const int ticks_per_bar = static_cast<int>(beats_per_bar / tick_length);

	if (params[SYNC_PARAM].value == 1.0)
	{
		m_synced = false;
	}

	const double phase = get_link_phase(m_link, beats_per_bar);
    const double offset = params[OFFSET_PARAM].value * (5.0 * tick_length);
    int tick = static_cast<int>(std::floor((phase + offset) / tick_length));

    if (tick < 0)
        tick += ticks_per_bar;

    if (((tick >> 3) % 2) == 1)
    {
        const double max_swing_in_ticks = 2.0;

        const double swing = params[SWING_PARAM].value * (max_swing_in_ticks * tick_length);
        tick = static_cast<int>(std::floor((phase + offset - swing) / tick_length));
    }

    if (tick < 0)
        tick += ticks_per_bar;

    if ((m_lastTick != tick) || !m_synced)
    {
        //std::cout << tick << std::endl;

        if (tick == 0)
            m_synced = true;

        if (m_synced)
        {
            // 8 ticks per 4th of beat, clock has 50% PWM
            const bool clock_4th = ((tick % 8) < 4);
            outputs[CLOCK_OUTPUT].value = (clock_4th ? 10.0 : 0.0);
            lights[CLOCK_LIGHT].setBrightness(clock_4th ? 1.0 : 0.0);

            // reset has 25% PWM
            const bool reset = ((tick % ticks_per_bar) < 2);
            outputs[RESET_OUTPUT].value = (reset ? 10.0 : 0.0);
            lights[RESET_LIGHT].setBrightness(reset ? 1.0 : 0.0);
        }
        else
        {
            outputs[CLOCK_OUTPUT].value = 0.0;
            lights[CLOCK_LIGHT].setBrightness(0.0);

            outputs[RESET_OUTPUT].value = 0.0;
            lights[RESET_LIGHT].setBrightness(0.0);
        }

        m_lastTick = tick;
    }

    lights[SYNC_LIGHT].setBrightness(m_synced ? 0.0 : 1.0);
}

LinkWidget::LinkWidget()
{
    Link* module = new Link();
    setModule(module);
    box.size = Vec(60, 380);

    SVGPanel* panel = new SVGPanel();
    panel->box.size = box.size;
    panel->setBackground(SVG::load(assetPlugin(plugin, "res/LinkBlue.svg")));
    addChild(panel);

    addChild(createScrew<ScrewSilver>(Vec(23, 0)));
    addChild(createScrew<ScrewSilver>(Vec(23, 365)));

    addParam(createParam<BlueSmallButton>(Vec(22, 62), module, Link::SYNC_PARAM, 0.0, 1.0, 0.0));
    addParam(createParam<KnobSimpleWhite>(Vec(16, 114), module, Link::OFFSET_PARAM, -1.0, 1.0, 0.0));
    addParam(createParam<KnobSimpleWhite>(Vec(16, 174), module, Link::SWING_PARAM, 0.0, 1.0, 0.0));

    addOutput(createOutput<PJ301MPort>(Vec(17, 241), module, Link::CLOCK_OUTPUT));
    addOutput(createOutput<PJ301MPort>(Vec(17, 297), module, Link::RESET_OUTPUT));

    addChild(createLight<SmallLight<BlueLight>>(Vec(17, 236), module, Link::CLOCK_LIGHT));
    addChild(createLight<SmallLight<YellowLight>>(Vec(17, 292), module, Link::RESET_LIGHT));
    addChild(createLight<MediumLight<BlueLight>>(Vec(25.4, 65.4), module, Link::SYNC_LIGHT));
}
