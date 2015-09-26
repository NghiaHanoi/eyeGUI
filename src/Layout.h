//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Layout class owning one root element. Does mapping of ids to element pointer.
// Enqueues notifications of elements occuring during updating and works on them
// before the next update and drawing. Is owner of the stylesheet used by the
// root element and all its children. Elements can register child elements for
// updating and rendering as front elements, which are then updated before the
// other elements and drawn at least by the layout.
// TODO: Frames (ids still here, but no elements...)

#ifndef LAYOUT_H_
#define LAYOUT_H_

#include "eyeGUI.h"
#include "Frame.h"
#include "Input.h"
#include "Style.h"
#include "Config.h"
#include "Parser/StylesheetParser.h"
#include "Parser/BrickParser.h"

#include <memory>
#include <map>
#include <set>
#include <utility>

namespace eyegui
{
    // Forward declaration
    class GUI;

    class Layout
    {
    public:

        // Constructor
        Layout(GUI const * pGUI, AssetManager* pAssetManager, std::string stylesheetFilepath);

        // Destructor
        virtual ~Layout();

        // Updating
        void update(float tpf, Input* pInput);

        // Drawing
        void draw() const;

        // Resize function
        void resize(bool force = false);

        // Attach root
        void attachRoot(
            std::unique_ptr<Element> upElement,
            std::unique_ptr<std::map<std::string, Element*> > upIds);

        // Enqueue notification which is processed before updating
        void enqueueNotification(InteractiveElement* pNotifier, InteractiveElement::Notification notification);

        // Get pointer to config of owning GUI
        Config const * getConfig() const;

        // Get main frame of layout
        Frame* getMainFrame();

        // Get layout resolution
        uint getLayoutWidth() const;
        uint getLayoutHeight() const;

        // Get time from GUI
        float getAccPeriodicTime() const;

        // Get pointer to styles of this layout
        Style const * getStyleFromStylesheet(std::string styleName) const;

        // Returns set of names of the available styles for this layout
        std::set<std::string> getNamesOfAvailableStyles() const;

        // Set visibility
        void setVisibility(bool visible, bool setImmediately);

        // One may want not to use input for this layout
        void useInput(bool useInput);

        // Set element activity
        void setElementActivity(std::string id, bool active, bool setImmediately);

        // Check activity of element
        bool isElementActive(std::string id) const;

        // Set interactive element as highlighted
        void highlightInteractiveElement(std::string id, bool doHighlight);

        // Set icon of interactive element
        void setIconOfInteractiveElement(std::string id, std::string iconFilepath);

        // Hit button
        void hitButton(std::string id);

        // Button down
        void buttonDown(std::string id);

        // Button up
        void buttonUp(std::string id);

        // Is button a switch?
        bool isButtonSwitch(std::string id) const;

        // Penetrate sensor
        void penetrateSensor(std::string id, float amount);

        // Register button listener
        void registerButtonListener(std::string id, std::weak_ptr<ButtonListener> wpListener);

        // Register sensor listener
        void registerSensorListener(std::string id, std::weak_ptr<SensorListener> wpListener);

        // Select interactive element by id
        void selectInteractiveElement(std::string id);

        // Returns, whether successfull
        bool selectInteractiveElement(InteractiveElement* pInteractiveElement);

        // Deselect current element
        void deselectInteractiveElement();

        // Interact with selected interactive element
        void interactWithSelectedInteractiveElement();

        // Select next interactive element
        bool selectNextInteractiveElement();

        // Reset elements
        void resetElements();

        // Check for existence of id
        bool checkForId(std::string id) const;

        // Check whether element is highlighted
        bool isInteractiveElementHighlighted(std::string id) const;

        // Replace any element with block
        void replaceElementWithBlock(std::string id, bool doFading);

        // Replace any element with picture
        void replaceElementWithPicture(std::string id, bool doFading, std::string filepath, PictureAlignment alignment);

        // Replace any element with blank
        void replaceElementWithBlank(std::string id, bool doFading);

        // Replace any element with circle button
        void replaceElementWithCircleButton(std::string id, bool doFading, std::string iconFilepath, bool isSwitch);

        // Replace any element with box button
        void replaceElementWitBoxButton(std::string id, bool doFading, std::string iconFilepath, bool isSwitch);

        // Replace any element with sensor
        void replaceElementWitSensor(std::string id, bool doFading, std::string iconFilepath);

        // Replace any element with a brick of elements
        void replaceElementWithBrick(std::string id, bool doFading, std::string filepath);

        // Add floating frame with brick
        uint addFloatingFrameWithBrick(
            std::string filepath,
            float relativePositionX,
            float relativePositionY,
            float relativeSizeX,
            float relativeSizeY,
            bool doFading,
            bool visible);

        // Set visibilty of floating frame
        void setVisibiltyOfFloatingFrame(uint frameIndex, bool visible, bool setImmediately);

        // Reset elements of floating frame
        void resetFloatingFramesElements(uint frameIndex);

		// Remove floating frame
		void removeFloatingFrame(uint frameIndex, bool fade);

		// Translate floating frame
		void translateFloatingFrame(uint frameIndex, float translateX, float translateY);

		// Scale floating frame
		void scaleFloatingFrame(uint frameIndex, float scaleX, float scaleY);

		// Set position of floating frame
		void setPositionOfFloatingFrame(uint frameIndex, float relativePositionX, float relativePositionY);

		// Set size of floating frame
		void setSizeOfFloatingFrame(uint frameIndex, float relativeSizeX, float relativeSizeY);

		// Move floating frame to front
		void moveFloatingFrameToFront(uint frameIndex);

		// Move floating frame to back
		void moveFloatingFrameToBack(uint frameIndex);

    private:

        // Fetch pointer to element by id
        Element* fetchElement(std::string id) const;

        // Internal replacement helper, returns whether successful
        bool replaceElement(Element* pTarget, std::unique_ptr<Element> upElement, bool doFading);

        // Insert id of element
        void insertId(Element* pElement);

        // Insert map of ids
        void insertIds(std::unique_ptr<idMap> upIdMap);

        // Fetch pointer to frame
        Frame* fetchFloatingFrame(uint frameIndex);

		// Move floating frame by id
		void moveFloatingFrame(int oldIndex, int newIndex);

        // Members
        GUI const * mpGUI;
        AssetManager* mpAssetManager;
        std::unique_ptr<Frame> mupMainFrame;
        std::unique_ptr<std::map<std::string, Element*> > mupIds;
        std::vector<std::unique_ptr<Frame> > mFloatingFrames;
		std::vector<int> mFloatingFramesOrderingIndices;
		std::vector<int> mDyingFloatingFramesIndices;
        float mAlpha;
        bool mVisible;
        bool mResizeNecessary;
        bool mUseInput;
        std::unique_ptr<std::map<std::string, Style> > mStyles;
        InteractiveElement* mpSelectedInteractiveElement;
        std::unique_ptr<std::vector<std::pair<InteractiveElement*, InteractiveElement::Notification> > > mupNotificatons;
        StylesheetParser mStylesheetParser;
        BrickParser mBrickParser;
    };
}

#endif // LAYOUT_H_
