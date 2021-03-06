//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Static helper functions.

#ifndef HELPER_H_
#define HELPER_H_

#include "externals/GLM/glm/glm.hpp"
#include "externals/GLM/glm/gtc/matrix_transform.hpp"

#include <string>
#include <algorithm>

namespace eyegui
{
    float clamp(float value, float lowerBound, float upperBound);

    bool checkFileNameExtension(std::string filepath, std::string expectedExtension);

    void replaceString(std::string& rInput, const std::string &rTarget, const std::string& rReplacement);

    // Convert pixel space to drawing space (origin top left)
    glm::mat4 calculateDrawMatrix(int layoutWidth, int layoutHeight, int x, int y, int width, int height);
}

#endif // HELPER_H_
