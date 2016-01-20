//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Key.h"

#include "Layout.h"
#include "AssetManager.h"
#include "Defines.h"

namespace eyegui
{
    Key::Key(Layout const * pLayout, AssetManager* pAssetManager)
    {
        // Initialize members
        mpLayout = pLayout;
        mpAssetManager = pAssetManager;

        // TransformAndSize has to be called before usage
        mX = 0;
        mY = 0;
        mSize = 0;
        mFocused = false;
        mFocus.setValue(0);

        // Fetch render item for key circle
        mpCirlceRenderItem = mpAssetManager->fetchRenderItem(
            shaders::Type::KEY,
            meshes::Type::QUAD);
    }

    Key::~Key()
    {
        // Nothing to do
    }

    void Key::transformAndSize(int x, int y, int size)
    {
        mX = x;
        mY = y;
        mSize = size;

        // Draw matrix for cirlce
        mCircleMatrix = Element::calculateDrawMatrix(
                mpLayout,
                mX - mSize/2,
                mY - mSize/2,
                mSize,
                mSize);
    }

    void Key::update(float tpf)
    {
        mFocus.update(tpf, !mFocused);
    }

    void Key::reset()
    {
        mFocused = false;
        mFocus.setValue(0);
    }

    void Key::setFocus(bool focused)
    {
        mFocused = focused;
    }

    glm::vec2 Key::getPosition() const
    {
        return glm::vec2(mX, mY);
    }

    void Key::drawCircle(
            int oglStencilX,
            int oglSencilY,
            int oglStencilWidth,
            int oglStencilHeight,
            glm::vec4 color,
            float alpha) const
    {
        // Bind and fill render item
        mpCirlceRenderItem->bind();

        // Fill color
        glm::vec4 circleColor = color;
        circleColor.a *= alpha;
        mpCirlceRenderItem->getShader()->fillValue("color", circleColor);

        // Fill other uniforms
        mpCirlceRenderItem->getShader()->fillValue("matrix", mCircleMatrix); // Matrix is updated in transform and size
        mpCirlceRenderItem->getShader()->fillValue("focus", mFocus.getValue());
        mpCirlceRenderItem->getShader()->fillValue("stencil", glm::vec4(oglStencilX, oglSencilY, oglStencilWidth, oglStencilHeight));

        // Drawing
        mpCirlceRenderItem->draw();
    }

    CharacterKey::CharacterKey(
        Layout const * pLayout,
        AssetManager* pAssetManager,
        Font const * pFont,
        char16_t character) : Key(pLayout, pAssetManager)
    {
        // Save members
        mpFont = pFont;
        mCharacter = character;

        // Fetch shader to render character
        mpQuadShader = mpAssetManager->fetchShader(shaders::Type::CHARACTER_KEY);

        // Get glyph from font
        mpGlyph = mpFont->getGlyph(FontSize::TALL, mCharacter);

        // Save currently set buffer and vertex array object
        GLint oldBuffer, oldVAO;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &oldBuffer);
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &oldVAO);

        // Initialize mesh buffers and vertex array object
        glGenBuffers(1, &mQuadVertexBuffer);
        glGenBuffers(1, &mQuadTextureCoordinateBuffer);
        glGenVertexArrays(1, &mQuadVertexArrayObject);

        // Fill vertex buffer (in OpenGL space)
        glBindBuffer(GL_ARRAY_BUFFER, mQuadVertexBuffer);
        std::vector<glm::vec3> vertices;
        vertices.push_back(glm::vec3(0,0,0));
        vertices.push_back(glm::vec3(1,0,0));
        vertices.push_back(glm::vec3(1,1,0));
        vertices.push_back(glm::vec3(1,1,0));
        vertices.push_back(glm::vec3(0,1,0));
        vertices.push_back(glm::vec3(0,0,0));
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Texture coordinates are dynamic and filled in transformAndSize method

        // Bind stuff to vertex array object
        glBindVertexArray(mQuadVertexArrayObject);

        // Vertices
        GLuint vertexAttrib = glGetAttribLocation(mpQuadShader->getShaderProgram(), "posAttribute");
        glEnableVertexAttribArray(vertexAttrib);
        glBindBuffer(GL_ARRAY_BUFFER, mQuadVertexBuffer);
        glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        // Texture coordinates
        GLuint uvAttrib = glGetAttribLocation(mpQuadShader->getShaderProgram(), "uvAttribute");
        glEnableVertexAttribArray(uvAttrib);
        glBindBuffer(GL_ARRAY_BUFFER, mQuadTextureCoordinateBuffer);
        glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, 0, NULL);

        // Restore old settings
        glBindBuffer(GL_ARRAY_BUFFER, oldBuffer);
        glBindVertexArray(oldVAO);
    }

    CharacterKey::~CharacterKey()
    {
        // Delete vertex array object
        glDeleteVertexArrays(1, &mQuadVertexArrayObject);

        // Delete buffers
        glDeleteBuffers(1, &mQuadVertexBuffer);
        glDeleteBuffers(1, &mQuadTextureCoordinateBuffer);
    }

    void CharacterKey::transformAndSize(int x, int y, int size)
    {
        // Super call
        Key::transformAndSize(x, y, size);

        // Calculate ratio of glyph representing character
        float ratio = (float)mpGlyph->size.x / (float)mpGlyph->size.y;

        // Calculate size multiplier
        glm::vec2 sizeMultiplier;
        if(ratio > 1)
        {
            // Wider
            sizeMultiplier = glm::vec2(1, 1.0f/ratio);
        }
        else
        {
            // Higher
            sizeMultiplier = glm::vec2(ratio, 1);
        }

        // Fill matrix for rendering quad displaying character
        glm::vec2 quadSize =  sizeMultiplier * (float)mSize * KEY_CIRCLE_CHARACTER_SIZE_RATIO;
        mQuadMatrix = Element::calculateDrawMatrix(
                mpLayout,
                mX - (quadSize.x/2),
                mY - (quadSize.y/2),
                quadSize.x,
                quadSize.y);

        // Set texture coordinates of quad
        GLint oldBuffer;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &oldBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, mQuadTextureCoordinateBuffer);
        std::vector<glm::vec2> textureCoordinates;
        textureCoordinates.push_back(glm::vec2(mpGlyph->atlasPosition.x, mpGlyph->atlasPosition.y));
        textureCoordinates.push_back(glm::vec2(mpGlyph->atlasPosition.z, mpGlyph->atlasPosition.y));
        textureCoordinates.push_back(glm::vec2(mpGlyph->atlasPosition.z, mpGlyph->atlasPosition.w));
        textureCoordinates.push_back(glm::vec2(mpGlyph->atlasPosition.z, mpGlyph->atlasPosition.w));
        textureCoordinates.push_back(glm::vec2(mpGlyph->atlasPosition.x, mpGlyph->atlasPosition.w));
        textureCoordinates.push_back(glm::vec2(mpGlyph->atlasPosition.x, mpGlyph->atlasPosition.y));
        glBufferData(GL_ARRAY_BUFFER, textureCoordinates.size() * 2 * sizeof(float), textureCoordinates.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, oldBuffer);
    }

    void CharacterKey::draw(
            int stencilX,
            int stencilY,
            int stencilWidth,
            int stencilHeight,
            glm::vec4 color,
            glm::vec4 iconColor,
            float alpha) const
    {
        // Convert stencil values to OpenGL coordinate system
        int oglStencilX = stencilX;
        int oglStencilY = mpLayout->getLayoutHeight() - stencilY - stencilHeight;
        int oglStencilWidth = stencilWidth;
        int oglStencilHeight = stencilHeight;

        // Draw circle of key
        drawCircle(oglStencilX, oglStencilY, oglStencilWidth, oglStencilHeight, color, alpha);

        // Render character
        mpQuadShader->bind();
        glBindVertexArray(mQuadVertexArrayObject);

        // Bind atlas texture
        glBindTexture(GL_TEXTURE_2D, mpFont->getAtlasTextureHandle(FontSize::TALL));

        // Fill color
        glm::vec4 characterColor = iconColor;
        characterColor.a *= alpha;
        mpQuadShader->fillValue("color", characterColor);

        // Fill other uniforms
        mpQuadShader->fillValue("matrix", mQuadMatrix); // Matrix is updated in transform and size
        mpQuadShader->fillValue("stencil", glm::vec4(oglStencilX, oglStencilY, oglStencilWidth, oglStencilHeight));

        // Draw character quad (vertex count must be 6)
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    std::u16string CharacterKey::getValue() const
    {
        return std::u16string(&mCharacter);
    }
}