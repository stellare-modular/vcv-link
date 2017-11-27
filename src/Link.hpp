#include "rack.hpp"

using namespace rack;

extern Plugin* plugin;

struct BlueSmallButton : SVGSwitch, MomentarySwitch
{
    BlueSmallButton()
    {
        addFrame(SVG::load(assetPlugin(plugin, "res/BlueSmallButton_0.svg")));
        addFrame(SVG::load(assetPlugin(plugin, "res/BlueSmallButton_1.svg")));

        sw->wrap();
        box.size = sw->box.size;
    }
};

struct SimpleKnobBlack : SVGKnob
{
    SimpleKnobBlack()
    {
        box.size = Vec(39.5, 39.5);
        minAngle = -0.82 * M_PI;
        maxAngle = 0.82 * M_PI;

        setSVG(SVG::load(assetPlugin(plugin, "res/SimpleKnobBlack.svg")));
    }
};

struct SimpleKnobBlackSmall : SimpleKnobBlack
{
	SimpleKnobBlackSmall()
    {
        box.size = Vec(30, 30);
    }
};

struct LinkWidget : ModuleWidget
{
    LinkWidget();
};
