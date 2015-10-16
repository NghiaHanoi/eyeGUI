//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "ElementParser.h"

#include "Defines.h"
#include "Layout.h"
#include "OperationNotifier.h"

#include <uchar.h>

namespace eyegui
{
    namespace element_parser
    {
        std::unique_ptr<elementsAndIds> parse(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, tinyxml2::XMLElement const * xmlElement, Element* pParent, std::string filepath)
        {
            // Create map for ids
            std::unique_ptr<idMap> upIdMap = std::unique_ptr<idMap>(new std::map<std::string, Element*>);

            // Parse elements
            std::unique_ptr<Element> upElement = std::move(parseElement(pLayout, pFrame, pAssetManager, xmlElement, pParent, filepath, *(upIdMap.get())));

            // Create and fill pair
            std::unique_ptr<elementsAndIds> upPair = std::unique_ptr<elementsAndIds>(new elementsAndIds);
            upPair->first = std::move(upElement);
            upPair->second = std::move(upIdMap);

            // Return pair
            return std::move(upPair);
        }

        std::unique_ptr<Element> parseElement(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, tinyxml2::XMLElement const * xmlElement, Element* pParent, std::string filepath, idMap& rIdMap)
        {
            // Name of style of element (if xmlElement == NULL, get style of parent if available)
            std::string styleName = parseStyleName(xmlElement, pParent, pLayout->getNamesOfAvailableStyles(), filepath);

            if (xmlElement == NULL)
            {
                // Fallback
                return(std::move(parseBlank(pLayout, pFrame, pAssetManager, EMPTY_STRING_ATTRIBUTE, styleName, 1, 0, NULL, pParent, filepath, rIdMap)));
            }

            // Unique pointer to element
            std::unique_ptr<Element> upElement;

            // Id of element
            std::string id = parseStringAttribute("id", xmlElement);

            // Relative scale of element
            float relativeScale = parseRelativeScale(xmlElement);

            // Border of element
            float border = parsePercentAttribute("border", xmlElement);

            std::string value = std::string(xmlElement->Value());
            if (value == "grid")
            {
                upElement = std::move(parseGrid(pLayout, pFrame, pAssetManager, id, styleName, relativeScale, border, xmlElement, pParent, filepath, rIdMap));
            }
            else if (value == "blank")
            {
                upElement = std::move(parseBlank(pLayout, pFrame, pAssetManager, id, styleName, relativeScale, border, xmlElement, pParent, filepath, rIdMap));
            }
            else if (value == "block")
            {
                upElement = std::move(parseBlock(pLayout, pFrame, pAssetManager, id, styleName, relativeScale, border, xmlElement, pParent, filepath, rIdMap));
            }
            else if (value == "picture")
            {
                upElement = std::move(parsePicture(pLayout, pFrame, pAssetManager, id, styleName, relativeScale, border, xmlElement, pParent, filepath, rIdMap));
            }
            else if (value == "stack")
            {
                upElement = std::move(parseStack(pLayout, pFrame, pAssetManager, id, styleName, relativeScale, border, xmlElement, pParent, filepath, rIdMap));
            }
            else if (value == "textblock")
            {
                upElement = std::move(parseTextBlock(pLayout, pFrame, pAssetManager, id, styleName, relativeScale, border, xmlElement, pParent, filepath, rIdMap));
            }
            else if (value == "circlebutton")
            {
                upElement = std::move(parseCircleButton(pLayout, pFrame, pAssetManager, id, styleName, relativeScale, border, xmlElement, pParent, filepath, rIdMap));
            }
            else if (value == "boxbutton")
            {
                upElement = std::move(parseBoxButton(pLayout, pFrame, pAssetManager, id, styleName, relativeScale, border, xmlElement, pParent, filepath, rIdMap));
            }
            else if (value == "sensor")
            {
                upElement = std::move(parseSensor(pLayout, pFrame, pAssetManager, id, styleName, relativeScale, border, xmlElement, pParent, filepath, rIdMap));
            }
            else if (value == "dropbutton")
            {
                upElement = std::move(parseDropButton(pLayout, pFrame, pAssetManager, id, styleName, relativeScale, border, xmlElement, pParent, filepath, rIdMap));
            }
            else
            {
                throwError(OperationNotifier::Operation::PARSING, "Unknown element found: " + std::string(xmlElement->Value()), filepath);
            }

            // Return element, but save id before
            if (checkElementId(rIdMap, id, filepath))
            {
                insertElementId(rIdMap, id, upElement.get());
                return (std::move(upElement));
            }
            else
            {
                return NULL;
            }
        }

        std::unique_ptr<Grid> parseGrid(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, std::string id, std::string styleName, float relativeScale, float border, tinyxml2::XMLElement const * xmlGrid, Element* pParent, std::string filepath, idMap& rIdMap)
        {
            // Get first row
            tinyxml2::XMLElement const * xmlRow = xmlGrid->FirstChildElement();
            if (!validateElement(xmlRow, "row"))
            {
                throwError(OperationNotifier::Operation::PARSING, "Row node expected but not found", filepath);
            }

            // Count of rows
            int rows = 0;
            tinyxml2::XMLElement const * xmlRowCounter = xmlRow;

            while (xmlRowCounter != NULL)
            {
                // Checking whether it is really a row element is done later
                rows++;
                xmlRowCounter = xmlRowCounter->NextSiblingElement();
            }

            // Create grid
            std::unique_ptr<Grid> upGrid = std::unique_ptr<Grid>(new Grid(id, styleName, pParent, pLayout, pFrame, pAssetManager, relativeScale, border, rows));

            // Go over rows
            for (int i = 0; i < rows; i++)
            {
                // Get first column
                tinyxml2::XMLElement const * xmlColumn = xmlRow->FirstChildElement();
                if (!validateElement(xmlColumn, "column"))
                {
                    throwError(OperationNotifier::Operation::PARSING, "Column node expected but not found", filepath);
                }

                // Get column count
                int columns = 0;
                tinyxml2::XMLElement const * xmlColumnCounter = xmlColumn;

                while (xmlColumnCounter != NULL)
                {
                    // Checking whether it is really a column element is done later
                    columns++;
                    xmlColumnCounter = xmlColumnCounter->NextSiblingElement();
                }

                // Tell grid about column count
                upGrid->prepareColumns(i, columns);

                // Set height of row
                upGrid->setRelativeHeightOfRow(i, parsePercentAttribute("size", xmlRow));

                // Go over columns
                for (int j = 0; j < columns; j++)
                {
                    // Set width of column
                    upGrid->setRelativeWidthOfCell(i, j, parsePercentAttribute("size", xmlColumn));

                    // Determine element in cell
                    tinyxml2::XMLElement const * xmlElement = xmlColumn->FirstChildElement();
                    upGrid->attachElement(i, j, std::move(parseElement(pLayout, pFrame, pAssetManager, xmlElement, upGrid.get(), filepath, rIdMap)));

                    // Get column sibling if necessary
                    xmlColumn = xmlColumn->NextSiblingElement();
                    if (j < columns - 1)
                    {
                        if (!validateElement(xmlColumn, "column"))
                        {
                            throwError(OperationNotifier::Operation::PARSING, "Column node expected but not found", filepath);
                        }
                    }
                    else
                    {
                        if (xmlColumn != NULL)
                        {
                            throwError(OperationNotifier::Operation::PARSING, "Too many column nodes found", filepath);
                        }
                    }
                }

                // Get row sibling if necessary
                xmlRow = xmlRow->NextSiblingElement();
                if (i < rows - 1)
                {
                    if (!validateElement(xmlRow, "row"))
                    {
                        throwError(OperationNotifier::Operation::PARSING, "Row node expected but not found", filepath);
                    }
                }
                else
                {
                    if (xmlRow != NULL)
                    {
                        throwError(OperationNotifier::Operation::PARSING, "Too many row nodes found", filepath);
                    }
                }
            }

            // In the end, check whether sum of sizes are ok
            if (!upGrid->gridCompletelyFilledBySizes())
            {
                throwError(OperationNotifier::Operation::PARSING, "Sum of relative sizes in grid are not 100 percent", filepath);
            }

            // Return grid and give ownership
            return std::move(upGrid);
        }

        std::unique_ptr<Blank> parseBlank(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, std::string id, std::string styleName, float relativeScale, float border, tinyxml2::XMLElement const * xmlBlank, Element* pParent, std::string filepath, idMap& rIdMap)
        {
            // Create and return blank
            std::unique_ptr<Blank> upBlank = std::unique_ptr<Blank>(new Blank(id, styleName, pParent, pLayout, pFrame, pAssetManager, relativeScale, border));
            return (std::move(upBlank));
        }

        std::unique_ptr<Block> parseBlock(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, std::string id, std::string styleName, float relativeScale, float border, tinyxml2::XMLElement const * xmlBlock, Element* pParent, std::string filepath, idMap& rIdMap)
        {
            // Create block and return
            std::unique_ptr<Block> upBlock = std::unique_ptr<Block>(new Block(id, styleName, pParent, pLayout, pFrame, pAssetManager, relativeScale, border));
            return (std::move(upBlock));
        }

        std::unique_ptr<Picture> parsePicture(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, std::string id, std::string styleName, float relativeScale, float border, tinyxml2::XMLElement const * xmlPicture, Element* pParent, std::string filepath, idMap& rIdMap)
        {
            // Get full path to image file
            std::string imageFilepath = parseStringAttribute("src", xmlPicture);

            // Get alignment
            std::string alignmentValue = parseStringAttribute("alignment", xmlPicture);
            PictureAlignment alignment;
            if (alignmentValue == EMPTY_STRING_ATTRIBUTE || alignmentValue == "original")
            {
                alignment = PictureAlignment::ORIGINAL;
            }
            else if (alignmentValue == "stretched")
            {
                alignment = PictureAlignment::STRETCHED;
            }
            else
            {
                throwError(OperationNotifier::Operation::PARSING, "Unknown alignment used for picture: " + alignmentValue, filepath);
            }

            // Create and return
            std::unique_ptr<Picture> upPicture = std::unique_ptr<Picture>(new Picture(id, styleName, pParent, pLayout, pFrame, pAssetManager, relativeScale, border, imageFilepath, alignment));
            return (std::move(upPicture));
        }

        std::unique_ptr<Stack> parseStack(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, std::string id, std::string styleName, float relativeScale, float border, tinyxml2::XMLElement const * xmlStack, Element* pParent, std::string filepath, idMap& rIdMap)
        {
            // Get inner border
            float innerBorder = parsePercentAttribute("innerborder", xmlStack);

            // Get mode of relative scaling
            std::string relativeScalingValue = parseStringAttribute("relativescaling", xmlStack);
            Stack::RelativeScaling relativeScaling;
            if (relativeScalingValue == EMPTY_STRING_ATTRIBUTE || relativeScalingValue == "mainaxis")
            {
                relativeScaling = Stack::RelativeScaling::MAIN_AXIS;
            }
            else if (relativeScalingValue == "bothaxes")
            {
                relativeScaling = Stack::RelativeScaling::BOTH_AXES;
            }
            else
            {
                throwError(OperationNotifier::Operation::PARSING, "Unknown relative scaling used in stack: " + relativeScalingValue, filepath);
            }

            // Get alignment
            std::string alignmentValue = parseStringAttribute("alignment", xmlStack);
            Stack::Alignment alignment;
            if (alignmentValue == EMPTY_STRING_ATTRIBUTE || alignmentValue == "fill")
            {
                alignment = Stack::Alignment::FILL;
            }
            else if (alignmentValue == "center")
            {
                alignment = Stack::Alignment::CENTER;
            }
            else if (alignmentValue == "head")
            {
                alignment = Stack::Alignment::HEAD;
            }
            else if (alignmentValue == "tail")
            {
                alignment = Stack::Alignment::TAIL;
            }
            else
            {
                throwError(OperationNotifier::Operation::PARSING, "Unknown alignment used in stack: " + alignmentValue, filepath);
            }

            // Get padding
            float padding = parsePercentAttribute("padding", xmlStack);
            if (padding < 0 || padding > 1)
            {
                throwError(OperationNotifier::Operation::PARSING, "Padding value of stack not in range of 0% to 100%", filepath);
            }

            // Get separator
            float separator = parsePercentAttribute("separator", xmlStack);
            if (separator < 0 || separator > 1)
            {
                throwError(OperationNotifier::Operation::PARSING, "Separator value of stack not in range of 0% to 100%", filepath);
            }

            // Create stack
            std::unique_ptr<Stack> upStack = std::unique_ptr<Stack>(new Stack(id, styleName, pParent, pLayout, pFrame, pAssetManager, relativeScale, border, relativeScaling, alignment, padding, innerBorder, separator));

            // Insert elements
            tinyxml2::XMLElement const * xmlElement = xmlStack->FirstChildElement();
            while (xmlElement != NULL)
            {
                // Fetch element
                upStack->attachElement(std::move(parseElement(pLayout, pFrame, pAssetManager, xmlElement, upStack.get(), filepath, rIdMap)));

                // Try to get next sibling element
                xmlElement = xmlElement->NextSiblingElement();
            }

            // Return stack
            return (std::move(upStack));
        }

        std::unique_ptr<TextBlock> parseTextBlock(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, std::string id, std::string styleName, float relativeScale, float border, tinyxml2::XMLElement const * xmlTextBlock, Element* pParent, std::string filepath, idMap& rIdMap)
        {
            // Get inner border
            float innerBorder = parsePercentAttribute("innerborder", xmlTextBlock);

             // Get font size
            std::string fontSizeValue = parseStringAttribute("fontsize", xmlTextBlock);
            FontSize fontSize;
            if (fontSizeValue == EMPTY_STRING_ATTRIBUTE || fontSizeValue == "medium")
            {
                fontSize = FontSize::MEDIUM;
            }
            else if (fontSizeValue == "tall")
            {
                fontSize = FontSize::TALL;
            }
            else if (fontSizeValue == "small")
            {
                fontSize = FontSize::SMALL;
            }
            else
            {
                throwError(OperationNotifier::Operation::PARSING, "Unknown font size used in text block: " + fontSizeValue, filepath);
            }

            // Get alignment
            std::string alignmentValue = parseStringAttribute("alignment", xmlTextBlock);
            TextFlowAlignment alignment;
            if (alignmentValue == EMPTY_STRING_ATTRIBUTE || alignmentValue == "left")
            {
                alignment = TextFlowAlignment::LEFT;
            }
            else if (alignmentValue == "right")
            {
                alignment = TextFlowAlignment::RIGHT;
            }
            else if (alignmentValue == "center")
            {
                alignment = TextFlowAlignment::CENTER;
            }
            else if (alignmentValue == "justify")
            {
                alignment = TextFlowAlignment::JUSTIFY;
            }
            else
            {
                throwError(OperationNotifier::Operation::PARSING, "Unknown alignment used in text block: " + alignmentValue, filepath);
            }

            // Get vertical alignment
            std::string verticalAlignmentValue = parseStringAttribute("verticalalignment", xmlTextBlock);
            TextFlowVerticalAlignment verticalAlignment;
            if (verticalAlignmentValue == EMPTY_STRING_ATTRIBUTE || verticalAlignmentValue == "top")
            {
                verticalAlignment = TextFlowVerticalAlignment::TOP;
            }
            else if (verticalAlignmentValue == "center")
            {
                verticalAlignment = TextFlowVerticalAlignment::CENTER;
            }
            else if (verticalAlignmentValue == "bottom")
            {
                verticalAlignment = TextFlowVerticalAlignment::BOTTOM;
            }
            else
            {
                throwError(OperationNotifier::Operation::PARSING, "Unknown vertical alignment used in text block: " + verticalAlignmentValue, filepath);
            }

            // Get content (only 8 bit are possible inside the xml layout)
            std::string contentValue = parseStringAttribute("content", xmlTextBlock);
            std::u16string content = u"";
            char16_t c16str[3] = u"\0";
            mbstate_t mbs;
            for (const auto& it: contentValue)
            {
                memset(&mbs, 0, sizeof (mbs));
                memmove(c16str, u"\0\0\0", 3);
                mbrtoc16 (c16str, &it, 3, &mbs);
                content.append(std::u16string(c16str));
            }

            // Create text block
            std::unique_ptr<TextBlock> upTextBlock = std::unique_ptr<TextBlock>(new TextBlock(id, styleName, pParent, pLayout, pFrame, pAssetManager, relativeScale, border, fontSize, alignment, verticalAlignment, content, innerBorder));

            // Return text block
            return std::move(upTextBlock);
        }

        std::unique_ptr<CircleButton> parseCircleButton(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, std::string id, std::string styleName, float relativeScale, float border, tinyxml2::XMLElement const * xmlCircleButton, Element* pParent, std::string filepath, idMap& rIdMap)
        {
            // Extract filepath
            std::string iconFilepath = parseStringAttribute("icon", xmlCircleButton);

            // Is button a switch?
            bool isSwitch = parseBoolAttribute("switch", xmlCircleButton);

            // Create circle button
            std::unique_ptr<CircleButton> upCircleButton = std::unique_ptr<CircleButton>(new CircleButton(id, styleName, pParent, pLayout, pFrame, pAssetManager, relativeScale, border, iconFilepath, isSwitch));

            // Return circle button
            return (std::move(upCircleButton));
        }

        std::unique_ptr<BoxButton> parseBoxButton(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, std::string id, std::string styleName, float relativeScale, float border, tinyxml2::XMLElement const * xmlBoxButton, Element* pParent, std::string filepath, idMap& rIdMap)
        {
            // Extract filepath
            std::string iconFilepath = parseStringAttribute("icon", xmlBoxButton);

            // Is button a switch?
            bool isSwitch = parseBoolAttribute("switch", xmlBoxButton);

            // Create box button
            std::unique_ptr<BoxButton> upBoxButton = std::unique_ptr<BoxButton>(new BoxButton(id, styleName, pParent, pLayout, pFrame, pAssetManager, relativeScale, border, iconFilepath, isSwitch));

            // Return box button
            return (std::move(upBoxButton));
        }

        std::unique_ptr<Sensor> parseSensor(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, std::string id, std::string styleName, float relativeScale, float border, tinyxml2::XMLElement const * xmlSensor, Element* pParent, std::string filepath, idMap& rIdMap)
        {
            // Extract filepath
            std::string iconFilepath = parseStringAttribute("icon", xmlSensor);

            // Create sensor
            std::unique_ptr<Sensor> upSensor = std::unique_ptr<Sensor>(new Sensor(id, styleName, pParent, pLayout, pFrame, pAssetManager, relativeScale, border, iconFilepath));

            // Return sensor
            return (std::move(upSensor));
        }

        std::unique_ptr<DropButton> parseDropButton(Layout* pLayout, Frame* pFrame, AssetManager* pAssetManager, std::string id, std::string styleName, float relativeScale, float border, tinyxml2::XMLElement const * xmlDropButton, Element* pParent, std::string filepath, idMap& rIdMap)
        {
            // Extract filepath
            std::string iconFilepath = parseStringAttribute("icon", xmlDropButton);

            // Get usage of available space
            float space = parsePercentAttribute("space", xmlDropButton);
            if (space < 0 || space > 1)
            {
                throwError(OperationNotifier::Operation::PARSING, "Usage of available space in DropButton not in range of 0% to 100%", filepath);
            }

            // Create drop button
            std::unique_ptr<DropButton> upDropButton = std::unique_ptr<DropButton>(new DropButton(id, styleName, pParent, pLayout, pFrame, pAssetManager, relativeScale, border, iconFilepath, space));

            // Attach inner element
            tinyxml2::XMLElement const * xmlElement = xmlDropButton->FirstChildElement();

            if (xmlElement == NULL)
            {
                throwError(OperationNotifier::Operation::PARSING, "DropButton has no inner element", filepath);
            }
            else if (xmlElement->NextSiblingElement() != NULL)
            {
                throwError(OperationNotifier::Operation::PARSING, "DropButton has more than one inner element", filepath);
            }

            upDropButton->attachInnerElement(std::move(parseElement(pLayout, pFrame, pAssetManager, xmlElement, upDropButton.get(), filepath, rIdMap)));

            // Return drop button
            return (std::move(upDropButton));
        }

        bool validateElement(tinyxml2::XMLElement const * xmlElement, const std::string& expectedValue)
        {
            if (xmlElement == NULL || std::string(xmlElement->Value()).compare(expectedValue) != 0)
            {
                return false;
            }
            return true;
        }

        float parseRelativeScale(tinyxml2::XMLElement const * xmlElement)
        {
            if (xmlElement == NULL)
            {
                return 1;
            }
            else
            {
                float value = parsePercentAttribute("relativescale", xmlElement);
                if (value <= 0)
                {
                    return 1;
                }
                else
                {
                    return value;
                }
            }
        }

        std::string parseStyleName(tinyxml2::XMLElement const * xmlElement, Element const * pParent, const std::set<std::string>& rNamesOfAvailableStyles, std::string filepath)
        {
            if (xmlElement == NULL)
            {
                return DEFAULT_STYLE_NAME;
            }
            else
            {
                std::string styleName = parseStringAttribute("style", xmlElement);
                if (styleName == EMPTY_STRING_ATTRIBUTE)
                {
                    // No style found, try to get one from parent
                    if (pParent != NULL)
                    {
                        styleName = pParent->getStyleName();
                    }
                    else
                    {
                        // Otherwise, set default as style
                        styleName = DEFAULT_STYLE_NAME;
                    }
                }

                // Check, whether found name is ok
                bool check = false;
                for (const std::string& rName : rNamesOfAvailableStyles)
                {
                    if (rName == styleName)
                    {
                        check = true;
                    }
                }

                if (!check)
                {
                    // Trying to use style, which is not defined
                    throwError(OperationNotifier::Operation::PARSING, "Following style is tried to be used but not defined in stylesheet: " + styleName, filepath);
                }

                return styleName;
            }
        }

        std::string parseStringAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement)
        {
            tinyxml2::XMLAttribute const * xmlAttribute = xmlElement->FindAttribute(attributeName.c_str());
            if (xmlAttribute != NULL)
            {
                return xmlAttribute->Value();
            }
            else
            {
                return EMPTY_STRING_ATTRIBUTE;
            }
        }

        bool parseBoolAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement)
        {
            tinyxml2::XMLAttribute const * xmlAttribute = xmlElement->FindAttribute(attributeName.c_str());
            if (xmlAttribute != NULL)
            {
                std::string value = xmlAttribute->Value();

                // File format case is not relevant
                std::transform(value.begin(), value.end(), value.begin(), ::tolower);

                return value == "true";
            }
            else
            {
                return false;
            }
        }

        int parseIntAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement)
        {
            tinyxml2::XMLAttribute const * xmlAttribute = xmlElement->FindAttribute(attributeName.c_str());
            if (xmlAttribute != NULL)
            {
                return xmlAttribute->IntValue();
            }
            else
            {
                return EMPTY_INT_ATTRIBUTE;
            }
        }

        float parsePercentAttribute(std::string attributeName, tinyxml2::XMLElement const * xmlElement)
        {
            tinyxml2::XMLAttribute const * xmlAttribute = xmlElement->FindAttribute(attributeName.c_str());
            if (xmlAttribute != NULL)
            {
                std::string value = xmlAttribute->Value();
                std::string delimiter = "%";
                std::string token = value.substr(0, value.find(delimiter));
                return (std::stof(token) / 100.0f);
            }
            else
            {
                return EMPTY_PERCENTAGE_ATTRIBUTE;
            }
        }

        bool checkElementId(const idMap& rIdMap, const std::string& rId, std::string filepath)
        {
            auto it = rIdMap.find(rId);

            if (it != rIdMap.end())
            {
                if (it->second != NULL)
                {
                    throwError(OperationNotifier::Operation::PARSING, "Following id not unique: " + rId, filepath);
                    return false;
                }
            }

            return true;
        }


        void insertElementId(idMap& rIdMap, const std::string& rId, Element* pElement)
        {
            if (rId != EMPTY_STRING_ATTRIBUTE)
            {
                rIdMap[rId] = pElement;
            }
        }
    }
}
