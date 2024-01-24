/*
  Copyright (C) 2016 Rory Walsh

  Cabbage is free software; you can redistribute it
  and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  Cabbage is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA
*/

#include "CabbageCheckbox.h"
#include "../Audio/Plugins/CabbagePluginEditor.h"


//==============================================================================
// custom checkbox component with optional surrounding groupbox
//==============================================================================
CabbageCheckbox::CabbageCheckbox (ValueTree wData, CabbagePluginEditor* _owner)
    : ToggleButton (""),
    CabbageWidgetBase(_owner),
    corners (CabbageWidgetData::getNumProp (wData, CabbageIdentifierIds::corners)),
    owner(_owner),
    isRect (CabbageWidgetData::getStringProp (wData, CabbageIdentifierIds::shape).equalsIgnoreCase ("square")),
    name (CabbageWidgetData::getStringProp (wData, CabbageIdentifierIds::name)),
    buttonText (CabbageWidgetData::getStringProp (wData, CabbageIdentifierIds::text)),
    widgetData (wData)
{
    widgetData.addListener (this);
    setButtonText (buttonText);
    setTooltip (tooltipText = CabbageWidgetData::getStringProp (wData, CabbageIdentifierIds::popuptext));

    const String radioId = CabbageWidgetData::getStringProp (wData, CabbageIdentifierIds::radiogroup);
    setRadioGroupId (0);
    if (radioId.isNotEmpty())
    {
        const int id = owner->radioGroups.getWithDefault(radioId, -1);
        CabbageUtilities::debug(id);
        if(id != -1)
            setRadioGroupId (id);
        else{
            owner->radioGroups.set(radioId, owner->radioGroups.size()+100);
            setRadioGroupId (owner->radioGroups.getWithDefault(radioId, -1));
            CabbageUtilities::debug(owner->radioGroups.getWithDefault(radioId, -1).toString());
        }
    }

    this->setWantsKeyboardFocus (false);

    lookAndFeel.customFont = owner->customFont;
    setLookAndFeel(&lookAndFeel);
    
    setColour (TextButton::ColourIds::textColourOffId, Colour::fromString (CabbageWidgetData::getStringProp (wData, CabbageIdentifierIds::fontcolour)));
    setColour (TextButton::ColourIds::textColourOnId, Colour::fromString (CabbageWidgetData::getStringProp (wData, CabbageIdentifierIds::onfontcolour)));
    setColour (TextButton::buttonColourId, Colour::fromString (CabbageWidgetData::getStringProp (wData, CabbageIdentifierIds::colour)));
    setColour (TextButton::buttonOnColourId, Colour::fromString (CabbageWidgetData::getStringProp (wData, CabbageIdentifierIds::oncolour)));
    getProperties().set (CabbageIdentifierIds::shape, CabbageWidgetData::getStringProp (wData, CabbageIdentifierIds::shape).equalsIgnoreCase ("square"));
    getProperties().set (CabbageIdentifierIds::corners, CabbageWidgetData::getNumProp (wData, CabbageIdentifierIds::corners));

    initialiseCommonAttributes (this, wData);
    auto csdPath = owner->getProcessor().getCsdFile().getFullPathName();

    setImgProperties (*this, wData, csdPath, "buttonon");
    setImgProperties (*this, wData, csdPath, "buttonoff");
    setToggleState (getCurrentValue() == 1 ? true : false, dontSendNotification);
}

//==============================================================================
void CabbageCheckbox::valueTreePropertyChanged (ValueTree& valueTree, const Identifier& prop)
{

    if (prop == CabbageIdentifierIds::value)
    {
        const int newValue = CabbageWidgetData::getNumProp(valueTree, CabbageIdentifierIds::value)  > 0.9 ? 1 : 0;
        bool state = newValue == 1 ? true : false;
        if(getRadioGroupId() != 0)
            setToggleState(state, sendNotification);
        else
            setToggleState(state, dontSendNotification);
    }

    else
    {
        handleCommonUpdates (this, valueTree, false, prop);
        setColour (TextButton::ColourIds::textColourOffId, Colour::fromString (CabbageWidgetData::getStringProp (valueTree, CabbageIdentifierIds::fontcolour)));
        setColour (TextButton::ColourIds::textColourOnId, Colour::fromString (CabbageWidgetData::getStringProp (valueTree, CabbageIdentifierIds::onfontcolour)));
        setColour (TextButton::buttonColourId, Colour::fromString (CabbageWidgetData::getStringProp (valueTree, CabbageIdentifierIds::colour)));
        setColour (TextButton::buttonOnColourId, Colour::fromString (CabbageWidgetData::getStringProp (valueTree, CabbageIdentifierIds::oncolour)));
        getProperties().set (CabbageIdentifierIds::shape, CabbageWidgetData::getStringProp (valueTree, CabbageIdentifierIds::shape).equalsIgnoreCase ("square"));
        getProperties().set (CabbageIdentifierIds::corners, CabbageWidgetData::getNumProp (valueTree, CabbageIdentifierIds::corners));

        setButtonText (getCurrentText (valueTree));
        setTooltip (getCurrentPopupText (valueTree));
    }

    repaint();

}



