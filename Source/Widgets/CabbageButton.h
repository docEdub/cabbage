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

#ifndef CABBAGEBUTTON_H_INCLUDED
#define CABBAGEBUTTON_H_INCLUDED

#include "../CabbageCommonHeaders.h"
#include "CabbageWidgetBase.h"
#include "../LookAndFeel/FlatButtonLookAndFeel.h"
#include "../Audio/Plugins/CabbagePluginEditor.h"

class CabbageButton : public TextButton, public ValueTree::Listener, public CabbageWidgetBase
{
	CabbagePluginEditor* owner;
	String tooltipText;
public:

	CabbageButton(ValueTree wData, CabbagePluginEditor* owner);
	~CabbageButton() override {
		widgetData.removeListener(this);
		setLookAndFeel(nullptr);
	}

	//ValueTree::Listener virtual methods....
	void valueTreePropertyChanged(ValueTree& valueTree, const Identifier&) override;
	void valueTreeChildAdded(ValueTree&, ValueTree&)override {}
	void valueTreeChildRemoved(ValueTree&, ValueTree&, int) override {}
	void valueTreeChildOrderChanged(ValueTree&, int, int) override {}
	void valueTreeParentChanged(ValueTree&) override {}

	void setLookAndFeelColours(ValueTree wData);
	void visibilityChanged() override;
    CabbageLookAndFeel2 lookAndFeel;
	FlatButtonLookAndFeel flatLookAndFeel;
	
	ValueTree widgetData;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CabbageButton)
};


#endif  // CABBAGEBUTTON_H_INCLUDED
