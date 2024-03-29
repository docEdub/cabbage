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

#ifndef CABBAGEWIDGETBASE_H_INCLUDED
#define CABBAGEWIDGETBASE_H_INCLUDED

#include "../CabbageCommonHeaders.h"

class CabbagePluginEditor;

// Simple base class for taking care of some widget housekeeping. This class looks after
// common member variables such as alpha values, tooltiptext, bounds, etc
// Each cabbage widget should inherit from this class and call initialiseCommonAttributes()
// in the derived class's constructor, and handleCommonUpdates() in its valueTreePropertyChanged() method.
class CabbageWidgetBase
{
    int pivotx = 0, pivoty = 0, visible = 0, active = 0, value = 0, valuex = 0, valuey = 0, lineNumber = 0, toFront = 0;
    float rotate = 0.f, alpha = 0.f, currentValue = 0.f;
    String tooltipText = {}, text = {}, channel = {}, csdFile = {}, file = {}, behind ={};
    StringArray channelArray = {};   //can be used if widget supports multiple channels
    StringArray textArray = {};      //can be used used if widget supports multiple text items
    CabbagePluginEditor* editor;
    int customRadioGroupId = 0;
    
public:
    CabbageWidgetBase(CabbagePluginEditor* _owner);
    ~CabbageWidgetBase() {}

    void setCustomRadioGroupId(int radioId)
    {
        customRadioGroupId = radioId;
    }
    int getCustomRadioGroupId()
    {
        return customRadioGroupId;
    }
    int getActive() const
    {
        return active;
    }
    float getAlpha() const
    {
        return alpha;
    }
    const String& getChannel() const
    {
        return channel;
    }
    const StringArray&  getChannelArray() const
    {
        return channelArray;
    }
    const String&  getCsdFile() const
    {
        return csdFile;
    }
    const String&  getFilename() const
    {
        return file;
    }
    float  getCurrentValue() const
    {
        return currentValue;
    }
    int  getPivotx() const
    {
        return pivotx;
    }
    int  getPivoty() const
    {
        return pivoty;
    }
    float  getRotate() const
    {
        return rotate;
    }
    const String getText() const
    {
        return text.replace("\\n", "\n");
    }
    const StringArray&  getTextArray() const
    {
        return textArray;
    }
    const String getTooltipText() const
    {
        return tooltipText.replace("\\n", "\n");
    }
    int  getValue() const
    {
        return value;
    }
    int  getVisible() const
    {
        return visible;
    }
    StringArray getTextArray()
    {
        return textArray;
    }
    StringArray getChannelArray()
    {
        return channelArray;
    }
    void setValue (float val)
    {
        value = val;
    }
    void setWidgetText (String val)
    {
        text = val;
    }
    void setFilename (String val)
    {
        file = val;
    }
    void initialiseCommonAttributes (Component* child, ValueTree valueTree);                        //handles simple attributes on initialisation
    void handleCommonUpdates (Component* child, ValueTree data, const Identifier& prop); //handles all updates from ident channel message

    //see below file for implementation
    template< typename Type >
    void setImgProperties (Type& comp, ValueTree widgetData, String csdFilePath, String type);



    String getCurrentText (ValueTree data);
    String getCurrentPopupText (ValueTree data);

    void populateTextArrays (ValueTree data);
    void setChannel (ValueTree value);
    float getCurrentValue (ValueTree data);
    static int getSVGHeight (File svgFile);
    static int getSVGWidth (File svgFile);

    String createPopupBubbleText(double val, int decimalPlaces,
                                 const String& channel,
                                 const String& escapedPrefix = String(),
                                 const String& escapedPostfix = String());
    
    String createValueText(double val, int decimalPlaces,
                           const String& prefix = String(),
                           const String& postfix = String())
    {
        return prefix + String(val, decimalPlaces) + postfix;
    }

};

//================== setImgProperties ==========================
template< typename Type >
void CabbageWidgetBase::setImgProperties (Type& comp, ValueTree data, String csdFilePath, String imgType)
{

    File imgFile;
    const File csdfile (csdFilePath);
    const File imgPath (CabbageWidgetData::getStringProp (data, CabbageIdentifierIds::imgpath));
    const String fileType (CabbageWidgetData::getStringProp (data, CabbageIdentifierIds::filetype));

    if (imgType == "groupbox")
    {
        imgFile = csdfile.getParentDirectory().getChildFile (CabbageWidgetData::getStringProp (data, CabbageIdentifierIds::imggroupbox));

        if (imgFile.existsAsFile())
        {
            if (imgFile.getFileExtension().contains ("svg"))
            {
                comp.getProperties().set (CabbageIdentifierIds::imggroupbox, imgFile.getFullPathName());
            }
            else
                comp.getProperties().set (CabbageIdentifierIds::imggroupbox, imgFile.getFullPathName());
        }
    }
    else if (imgType == "buttonon")
    {
        imgFile = csdfile.getParentDirectory().getChildFile (CabbageWidgetData::getStringProp (data, CabbageIdentifierIds::imgbuttonon));

        if (imgFile.existsAsFile())
        {

            if (imgFile.getFileExtension().contains ("svg"))
            {
                comp.getProperties().set (CabbageIdentifierIds::imgbuttonon, imgFile.getFullPathName());
            }
            else
                comp.getProperties().set (CabbageIdentifierIds::imgbuttonon, imgFile.getFullPathName());
        }
    }
    else if (imgType == "buttonoff")
    {
        imgFile = csdfile.getParentDirectory().getChildFile (CabbageWidgetData::getStringProp (data, CabbageIdentifierIds::imgbuttonoff));

        if (imgFile.existsAsFile())
        {
            if (imgFile.getFileExtension().contains ("svg"))
            {
                comp.getProperties().set (CabbageIdentifierIds::imgbuttonoff, imgFile.getFullPathName());
            }
            else
                comp.getProperties().set (CabbageIdentifierIds::imgbuttonoff, imgFile.getFullPathName());
        }
    }
    else if (imgType == "buttonover")
    {
        imgFile = csdfile.getParentDirectory().getChildFile (CabbageWidgetData::getStringProp (data, CabbageIdentifierIds::imgbuttonover));
        
        if (imgFile.existsAsFile())
        {
            if (imgFile.getFileExtension().contains ("svg"))
            {
                comp.getProperties().set (CabbageIdentifierIds::imgbuttonover, imgFile.getFullPathName());
            }
            else
                comp.getProperties().set (CabbageIdentifierIds::imgbuttonover, imgFile.getFullPathName());
        }
    }
    else if (imgType.contains ("sliderbg"))
    {
        imgFile = csdfile.getParentDirectory().getChildFile (CabbageWidgetData::getStringProp (data, CabbageIdentifierIds::imgsliderbg));

        if (imgFile.existsAsFile())
        {

            if (imgFile.getFileExtension().contains ("svg"))
            {
                comp.getProperties().set ("imgsliderbg", imgFile.getFullPathName());
            }
            else
                comp.getProperties().set ("imgsliderbg", imgFile.getFullPathName());

        }
    }
    else if (imgType.contains ("slider"))
    {
        imgFile = csdfile.getParentDirectory().getChildFile (CabbageWidgetData::getStringProp (data, CabbageIdentifierIds::imgslider));

        if (imgFile.existsAsFile())
        {

            if (imgFile.getFileExtension().contains ("svg"))
            {
                comp.getProperties().set ("imgslider", imgFile.getFullPathName());
            }
            else
                comp.getProperties().set ("imgslider", imgFile.getFullPathName());
        }
    }
}



#endif  // CABBAGEWIDGETBASE_H_INCLUDED
