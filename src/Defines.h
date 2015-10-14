//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Stuff used all around the library.

#ifndef DEFINES_H_
#define DEFINES_H_

#include <string>

namespace eyegui
{
    // Typedefs
    typedef unsigned int uint;
    typedef unsigned long ulong;
    typedef unsigned char uchar;

    // Constants
    static const std::string EMPTY_STRING_ATTRIBUTE = "";
    static const int EMPTY_INT_ATTRIBUTE = 0;
    static const float EMPTY_PERCENTAGE_ATTRIBUTE = 0;
    static const std::string DEFAULT_STYLE_NAME = "default";
    static const std::string LAYOUT_EXTENSION = "xeyegui";
    static const std::string BRICK_EXTENSION = "beyegui";
    static const std::string CONFIG_EXTENSION = "ceyegui";
    static const std::string STYLESHEET_EXTENSION = "seyegui";
    static const float SVG_DPI = 96;
    static const int NOTIFICATION_MAX_LOOP_COUNT = 10;
    static const float ACCUMULATED_TIME_PERIOD = 1800.0f;
    static const float FONT_CHARACTER_PADDING = 0.02; // Percentage of height
    static const int FONT_MINIMAL_CHARACTER_PADDING = 1;
    static const float FONT_TALL_SCREEN_HEIGHT = 0.1f;
    static const float FONT_MEDIUM_SCREEN_HEIGHT = 0.05f;
    static const float FONT_SMALL_SCREEN_HEIGHT = 0.025f;

}

#endif // DEFINES_H_
