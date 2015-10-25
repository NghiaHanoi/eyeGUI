//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "TextBlock.h"

#include "Layout.h"
#include "OperationNotifier.h"

namespace eyegui
{
    TextBlock::TextBlock(
        std::string id,
        std::string styleName,
        Element* pParent,
        Layout const * pLayout,
        Frame* pFrame,
        AssetManager* pAssetManager,
        NotificationQueue* pNotificationQueue,
        float relativeScale,
        float border,
		bool dimmable,
        FontSize fontSize,
        TextFlowAlignment alignment,
        TextFlowVerticalAlignment verticalAlignment,
        std::u16string content,
        std::string key,
        float innerBorder) : Block(
            id,
            styleName,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
            pNotificationQueue,
            relativeScale,
            border,
			dimmable)
    {
        mType = Type::TEXT_BLOCK;

        // Fill members
        mInnerBorder = innerBorder;
        mKey = key;

        // Create text flow
        if (mKey != EMPTY_STRING_ATTRIBUTE)
        {
            std::u16string localization = mpLayout->getContentFromLocalization(mKey);
            if (localization == LOCALIZATION_NOT_FOUND)
            {
                throwWarning(
                    OperationNotifier::Operation::RUNTIME,
                    "No localization used or one found for following key: " + mKey + ". Element has following id: " + getId());

                mupTextFlow = std::move(mpAssetManager->createTextFlow(fontSize, alignment, verticalAlignment, content));
            }
            else
            {
                mupTextFlow = std::move(mpAssetManager->createTextFlow(fontSize, alignment, verticalAlignment, localization));
            }
        }
        else
        {
            mupTextFlow = std::move(mpAssetManager->createTextFlow(fontSize, alignment, verticalAlignment, content));
        }
    }

    TextBlock::~TextBlock()
    {
        // Nothing to do
    }

    void TextBlock::specialDraw() const
    {
        // Super call
        Block::specialDraw();

        // Draw text (emulation of shader bevavior for color mixing)
		glm::vec4 color = getStyle()->fontColor;
		color.a *= mAlpha;
		color *= (1.0f - mDimming.getValue()) + (mDimming.getValue() * getStyle()->dimColor);
        mupTextFlow->draw(1.0f, color);
    }

    void TextBlock::specialTransformAndSize()
    {
        // Super call
        Block::specialTransformAndSize();

        // Use inner border (Copy code from Stack :-/) -> TODO: move to block? (grid could also use it)
        int usedBorder;
        int innerX, innerY, innerWidth, innerHeight;
        if (getOrientation() == Element::Orientation::HORIZONTAL)
        {
            usedBorder = (int)((float)mHeight * mInnerBorder);
        }
        else
        {
            usedBorder = (int)((float)mWidth * mInnerBorder);
        }
        innerX = mX + usedBorder / 2;
        innerY = mY + usedBorder / 2;
        innerWidth = mWidth - usedBorder;
        innerHeight = mHeight - usedBorder;

        // Tell text flow about transformation
        mupTextFlow->transformAndSize(innerX, innerY, innerWidth, innerHeight);
    }
}