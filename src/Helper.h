//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Static helper functions.

#ifndef HELPER_H_
#define HELPER_H_

#include <string>
#include <algorithm>

namespace eyegui
{
    static float clamp(float value, float lowerBound, float upperBound)
    {
        return value < lowerBound ? lowerBound : (value > upperBound ? upperBound : value);
    }

    static bool checkFileNameExtension(std::string filepath, std::string expectedExtension)
    {
        // Extract token behind last dot
        std::string delimiter = ".";
        size_t pos = 0;
        std::string token;
        while ((pos = filepath.find(delimiter)) != std::string::npos)
        {
            token = filepath.substr(0, pos);
            filepath.erase(0, pos + delimiter.length());
        }

        // File format case is not relevant
        std::transform(filepath.begin(), filepath.end(), filepath.begin(), ::tolower);

        // Check token
        return (filepath.compare(expectedExtension) == 0);
    }

    static void replaceString(std::string& rInput, const std::string &rTarget, const std::string& rReplacement)
    {
        // Check whether there is a target
        if(rTarget.empty())
        {
            return;
        }

        // Find target and replace it
        size_t startPos = 0;
        while((startPos = rInput.find(rTarget, startPos)) != std::string::npos)
        {
            rInput.replace(startPos, rTarget.length(), rReplacement);
            startPos += rReplacement.length();
        }
    }
}

#endif // HELPER_H_
