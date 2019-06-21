/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewCT
 Authors: Daniele Giunchi, Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaViewCT_H__
#define __albaViewCT_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaViewCompound.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
//class albaViewRX;
class albaVMEVolume;
class albaViewVTK;

//----------------------------------------------------------------------------
// albaViewCT :
//----------------------------------------------------------------------------
/**
This view features two Rx views and one compound view made of six CT slices.*/
class ALBA_EXPORT albaViewCT : public albaViewCompound
{
public:
  /** constructor */
	albaViewCT(wxString label = "View CT");
  /** destructor */
	virtual ~albaViewCT();
  /** RTTI macro */
	albaTypeMacro(albaViewCT, albaViewCompound);

    /** clone the object*/
	virtual albaView *Copy(albaObserver *Listener, bool lightCopyEnabled = false);
	/** listen to other object events*/
	virtual void OnEvent(albaEventBase *alba_event);

	/** Show/Hide VMEs into plugged sub-views */
	virtual void VmeShow(albaVME *vme, bool show);

	/** Remove VME into plugged sub-views */
	virtual void VmeRemove(albaVME *vme);

	/** Operations to be performed on vme selection */
	virtual void VmeSelect(albaVME *vme, bool select);

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
	virtual albaGUI  *CreateGui();


	/**
	Enable/disable view widgets.*/
	void EnableWidgets(bool enable = true);

	double m_BorderColor[10][3];

	albaVMEVolume    *m_CurrentVolume; ///< Current visualized volume

	albaViewCompound *m_ViewCTCompound;

	// this member variables are used by side panel gui view
	int m_LayoutWidth;
	int m_LayoutHeight;

	albaGUI  *m_GuiViews[3];

	albaViewVTK *m_Vs;
};
#endif