// runtime/entities/prop_static.h

#pragma once
#include "imdl_model_runtime.h"

struct PropStatic {
    IMDLModel model;
    Vec3 origin;

    bool LoadFromEntity(const nlohmann::json& ent);
    void Draw();
};
