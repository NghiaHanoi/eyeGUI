//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "TextSimple.h"

#include "src/GUI.h"
#include "src/Rendering/AssetManager.h"
#include "src/Utilities/OperationNotifier.h"
#include "externals/GLM/glm/gtc/matrix_transform.hpp"

namespace eyegui
{
    TextSimple::TextSimple(
        GUI const * pGUI,
        AssetManager* pAssetManager,
        Font const * pFont,
        FontSize fontSize,
        float scale,
        std::u16string content) : Text(
            pGUI,
            pAssetManager,
            pFont,
            fontSize,
            scale,
            content)
    {
       // After construction, one can already construct the text
       calculateMesh();
    }

    TextSimple::~TextSimple()
    {
        // Delete vertex array object
        glDeleteVertexArrays(1, &mVertexArrayObject);

        // Delete buffers
        glDeleteBuffers(1, &mVertexBuffer);
        glDeleteBuffers(1, &mTextureCoordinateBuffer);
    }

    void TextSimple::evaluateSize(
        int& rWidth,
        int& rHeight) const
    {
        // Just use the values given by calculating mesh
        rWidth = mUsedWidth;
        rHeight = mUsedHeight;
    }

    void TextSimple::transform(int x, int y)
    {
        // Remind it a drawing
        mX = x;
        mY = y;
    }

    void TextSimple::draw(glm::vec4 color) const
    {
        // TODO: maybe merge somehow with TextFlow's draw?
        mpShader->bind();
        glBindVertexArray(mVertexArrayObject);

        // Calculate transformation matrix
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(mX, mpGUI->getWindowHeight() - mY, 0)); // Change coordinate system and translate to position
        matrix = glm::ortho(0.0f, (float)(mpGUI->getWindowWidth() - 1), 0.0f, (float)(mpGUI->getWindowHeight() - 1)) * matrix; // Pixel to world space

        // Bind atlas texture
        if (mScale == 1.0f)
        {
            mpFont->bindAtlasTexture(mFontSize, 0, false);
        }
        else
        {
            mpFont->bindAtlasTexture(mFontSize, 0, true);
        }

        // Fill uniforms (TODO: get rid of this madness)
        mpShader->fillValue("matrix", matrix);
        mpShader->fillValue("color", color);
        mpShader->fillValue("alpha", 1);
        mpShader->fillValue("activity", 0);
        mpShader->fillValue("dimColor", glm::vec4(1,1,1,1));
        mpShader->fillValue("dim", 0);
        mpShader->fillValue("markColor", glm::vec4(1,1,1,1));
        mpShader->fillValue("mark", 0);

        // Draw flow
        glDrawArrays(GL_TRIANGLES, 0, mVertexCount);
    }

    void TextSimple::specialCalculateMesh(
            std::u16string streamlinedContent,
            float lineHeight, std::vector<glm::vec3>& rVertices,
            std::vector<glm::vec2>& rTextureCoordinates)
    {
        // OpenGL setup done in calling method

        // Go over lines
        std::u16string delimiter = u"\n";
        size_t pos = 0;
        std::u16string line;
        float yPixelPen = -lineHeight; // First line should be also inside element
        float maxPixelWidth = 0;
        float pixelHeight = 0;
        while ((pos = streamlinedContent.find(delimiter)) != std::u16string::npos)
        {
            line = streamlinedContent.substr(0, pos);
            streamlinedContent.erase(0, pos + delimiter.length());

            // Just do whole line as one big word
            Word word = calculateWord(line, mScale);

            // Assuming, that the count of vertices and texture coordinates is equal
            for (uint i = 0; i < word.spVertices->size(); i++)
            {
                const glm::vec3& rVertex = word.spVertices->at(i);
                rVertices.push_back(glm::vec3(rVertex.x, rVertex.y + yPixelPen, rVertex.z));
                const glm::vec2& rTextureCoordinate = word.spTextureCoordinates->at(i);
                rTextureCoordinates.push_back(glm::vec2(rTextureCoordinate.s, rTextureCoordinate.t));
            }

            // Advance yPen
            yPixelPen -= lineHeight;

            // Remember that line for evaluate size
            maxPixelWidth = std::max(maxPixelWidth, word.pixelWidth);
            pixelHeight += lineHeight;
        }

        // Save used width and height
        mUsedWidth = (int)maxPixelWidth;
        mUsedHeight = (int)pixelHeight;
    }
}
