//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

#ifndef FRAME_H_
#define FRAME_H_

#include "eyeGUI.h"
#include "Elements/Elements.h"

#include <memory>
#include <map>
#include <set>

namespace eyegui
{
    // Forward declaration
    class Layout;

    class Frame
    {
    public:

        // Constructor
        Frame(
            Layout * const pLayout,
            float relativePositionX,
            float relativePositionY,
            float relativeSizeX,
            float relativeSizeY);

        // Destuctor
        virtual ~Frame();

        // Updating
        void update(float tpf, float alpha, Input* pInput);

        // Drawing
        void draw() const;

        // Resize function
        void resize(bool force = false);

        // Attach root
        void attachRoot(std::unique_ptr<Element> upElement);

        // Remove root
        std::unique_ptr<Element> replaceRoot(std::unique_ptr<Element> upElement);

        // Register front element for early update and late drawing
        void registerFrontElementForUpdateAndDraw(Element* pElement, bool visible);

        // Set alpha value of front element
        void setFrontElementAlpha(Element* pElement, float alpha);

        // Commit dying replaced element
        void commitDyingReplacedElement(std::unique_ptr<Element> upElement);

        // Set visibility
        void setVisibility(bool visible, bool setImmediately);

        // Get first interactive element. Returns null, if none found
        InteractiveElement* getFirstInteractiveElement() const;

        // Reset elements
        void resetElements();

        // Remove front elements of a element
        void removeFrontElementsOfElement(Element* pTarget);

		// Get all elements' ids
		std::set<std::string> getAllElementsIds() const;

		// Set frame's alpha
		void setFrameAlpha(float alpha);

		// Get frame's alpha
		float getFrameAlpha() const;

		// Set removed
		void setRemoved();

		// Is removed?
		bool isRemoved() const;

    private:

        // Members
        Layout const * mpLayout;
        std::unique_ptr<Element> mupRoot;
        float mFrameAlpha;
		float mCombinedAlpha;
        bool mVisible;
        std::vector<Element*> mFrontElements;
        std::map<Element*, float> mFrontElementAlphas;
        std::vector<std::unique_ptr<Element> > mDyingReplacedElements;
        bool mResizeNecessary;
        float mRelativePositionX;
        float mRelativePositionY;
        float mRelativeSizeX;
        float mRelativeSizeY;
		bool mRemoved;
    };
}


#endif // FRAME_H_
