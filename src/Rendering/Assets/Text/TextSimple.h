//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Very simple text asset for rendering.

#ifndef TEXT_SIMPLE_H_
#define TEXT_SIMPLE_H_

#include "src/Rendering/Assets/Text/Text.h"
namespace eyegui
{
    class TextSimple : public Text
    {
    public:

        // Constructor
        TextSimple(
            GUI const * pGUI,
            AssetManager* pAssetManager,
            Font const * pFont,
            FontSize fontSize,
            float scale,
            std::u16string content);

        // Destructor
        virtual ~TextSimple();

        // Evaluate size with given content
        void evaluateSize(
            int& rWidth,
            int& rHeight) const;

        // Set draw position (upper left corner)
        void transform(int x, int y);

        // Draw (TODO: do it in superclass somehow)
        void draw(glm::vec4 color) const;

    protected:

        // Calculate mesh (in pixel coordinates)
        virtual void specialCalculateMesh(
            std::u16string streamlinedContent,
            float lineHeight, std::vector<glm::vec3>& rVertices,
            std::vector<glm::vec2>& rTextureCoordinates);

        // Members
        int mUsedWidth;
        int mUsedHeight;
    };

}

#endif // TEXT_SIMPLE_H_
