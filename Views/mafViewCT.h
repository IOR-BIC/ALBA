/*=========================================================================

 Program: MAF2
 Module: mafViewCT
 Authors: Daniele Giunchi, Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafViewCT_H__
#define __mafViewCT_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafViewCompound.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
//class mafViewRX;
class mafVMEVolume;
class mafViewVTK;

//----------------------------------------------------------------------------
// mafViewCT :
//----------------------------------------------------------------------------
/**
This view features two Rx views and one compound view made of six CT slices.*/
class MAF_EXPORT mafViewCT : public mafViewCompound
{
public:
  /** constructor */
	mafViewCT(wxString label = "View CT");
  /** destructor */
	virtual ~mafViewCT();
  /** RTTI macro */
	mafTypeMacro(mafViewCT, mafViewCompound);

    /** clone the object*/
	virtual mafView *Copy(mafObserver *Listener, bool lightCopyEnabled = false);
	/** listen to other object events*/
	virtual void OnEvent(mafEventBase *maf_event);

	/** Show/Hide VMEs into plugged sub-views */
	virtual void VmeShow(mafNode *node, bool show);

	/** Remove VME into plugged sub-views */
	virtual void VmeRemove(mafNode *node);

	/** Operations to be performed on vme selection */
	virtual void VmeSelect(mafNode *node, bool select);

	/** Create visual pipe and initialize them to build a RXCT view */
	void PackageView();

	/** IDs for the GUI */
	enum VIEW_RXCT_WIDGET_ID
	{
		ID_LAYOUT_WIDTH = Superclass::ID_LAST,
		ID_LAYOUT_HEIGHT,
		ID_LAST
	};

	/** Create the GUI on the bottom of the compounded view. */
	virtual void CreateGuiView();

protected:
	/**
	Internally used to create a new instance of the GUI. This function should be
	overridden by subclasses to create specialized GUIs. Each subclass should append
	its own widgets and define the enum of IDs for the widgets as an extension of
	the superclass enum. The last id value must be defined as "LAST_ID" to allow the
	subclass to continue the ID enumeration from it. For appending the widgets in the
	same panel GUI, each CreateGUI() function should first call the superclass' one.*/
	virtual mafGUI  *CreateGui();

	/**
	Redefine to arrange views to generate RXCT visualization.*/
	//virtual void LayoutSubViewCustom(int width, int height);

	/**
	Enable/disable view widgets.*/
	void EnableWidgets(bool enable = true);

	double m_BorderColor[10][3];

	mafVMEVolume    *m_CurrentVolume; ///< Current visualized volume

	mafViewCompound *m_ViewCTCompound;

	// this member variables are used by side panel gui view
	int m_LayoutWidth;
	int m_LayoutHeight;

	mafGUI  *m_GuiViews[3];

	mafViewVTK *m_Vs;
};
#endif