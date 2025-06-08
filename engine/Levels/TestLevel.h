#ifndef TESTLEVEL_H
#define TESTLEVEL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void LoadTestLevel();
void RenderTestLevel(const glm::mat4& view, const glm::mat4& projection);
void CleanupTestLevel();

#endif