#include "Link.hpp"

Plugin* plugin;

void init(rack::Plugin* p)
{
    plugin = p;
    p->slug = "StellareModular";

#ifdef VERSION
    p->version = TOSTRING(VERSION);
#endif

    p->addModel(createModel<LinkWidget>("Stellare Modular", "Link", "Link", CLOCK_TAG));
}
