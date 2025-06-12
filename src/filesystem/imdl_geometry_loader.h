#pragma once

#include <string>

struct IMDLModel {
    unsigned int vao = 0;
    unsigned int vbo = 0;
    unsigned int ebo = 0;
    int indexCount = 0;
    std::string material;
};

bool LoadIMDL(const std::string& path, IMDLModel& outModel);
void DrawIMDL(const IMDLModel& model);