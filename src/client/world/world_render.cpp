// world_render.cpp - Source-style world rendering pass
#include "mathlib.h"
#include "prop_static.h"
#include <vector>

// Global list of static props in the world
std::vector<PropStatic> g_StaticProps;

void World_AddStaticProp(const PropStatic& prop) {
    g_StaticProps.push_back(prop);
}

void World_Render(const Mat4& view, const Mat4& proj) {
    for (auto& prop : g_StaticProps) {
        prop.Draw(view, proj);
    }
}
