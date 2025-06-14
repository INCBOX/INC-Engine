// runtime/entities/prop_static.h

#pragma once
#include "imdl_model.h"

struct PropStatic {
    IMDL_GPU model;
    Vec3 origin;

    bool LoadFromEntity(const nlohmann::json& ent);
    void Draw();
};
