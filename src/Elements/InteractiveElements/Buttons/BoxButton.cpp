//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "BoxButton.h"

namespace eyegui
{
    BoxButton::BoxButton(
        std::string id,
        std::string styleName,
        Element* pParent,
        Layout const * pLayout,
        Frame* pFrame,
        AssetManager* pAssetManager,
        NotificationQueue* pNotificationQueue,
        float relativeScale,
        float border,
        bool dimming,
        bool adaptiveScaling,
        std::string iconFilepath,
        bool isSwitch) : Button(
            id,
            styleName,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
            pNotificationQueue,
            RenderingMask::BOX,
            false,
            relativeScale,
            border,
            dimming,
            adaptiveScaling,
            iconFilepath,
            isSwitch)
    {
        mType = Type::BOX_BUTTON;

        // Fetch stuff for rendering
        mpIconRenderItem = mpAssetManager->fetchRenderItem(
            shaders::Type::BOX_BUTTON,
            meshes::Type::QUAD);
    }

    BoxButton::~BoxButton()
    {
        // Nothing to do here
    }

    void BoxButton::specialDraw() const
    {
        // Scale of icon
        //mpIconRenderItem->getShader()->fillValue("iconUVScale", iconAspectRatioCorrection());

        // Super call (draw highlight etc. on top)
        Button::specialDraw();
    }
}
