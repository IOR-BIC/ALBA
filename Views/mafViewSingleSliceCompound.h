/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewSingleSliceCompound.h,v $
  Language:  C++
  Date:      $Date: 2008-02-19 10:59:04 $
  Version:   $Revision: 1.2 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafViewSingleSliceCompound_H__
#define __mafViewSingleSliceCompound_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewCompound.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafSceneGraph;
class mmgGui;
class mafRWIBase;
class mafViewSingleSlice;
class mmgLutSlider;
class mmgLutSwatch;
class mmgFloatSlider;
class vtkLookupTable;

//----------------------------------------------------------------------------
// mafViewSingleSliceCompound :
//----------------------------------------------------------------------------
class mafViewSingleSliceCompound: public mafViewCompound
{
public:
  mafViewSingleSliceCompound(wxString label = "View Single Slice Compound", int num_row = 1, int num_col = 1);
  virtual ~mafViewSingleSliceCompound(); 

  mafTypeMacro(mafViewSingleSliceCompound, mafViewCompound);

	/** IDs for the view GUI */
	enum VIEW_SLICE_WIDGET_ID
	{
		ID_LUT_CHOOSER = Superclass::ID_LAST,
		ID_LAST
	};

  /** 
  Redefine this method to package the compounded view */
  virtual void PackageView();

  virtual mafView *Copy(mafObserver *Listener);
  
  /** 
  Create the GUI on the bottom of the compounded view. */
  virtual void     CreateGuiView();
  
  virtual void     OnEvent(mafEventBase *maf_event);

	/** 
  Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(mafNode *node, bool show);

	virtual void VmeSelect(mafNode *node, bool select);

  virtual void OnLayout();

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same pannel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual mmgGui  *CreateGui();

	void EnableWidgets(bool enable);

	void UpdateWindowing(bool enable,mafNode *node);

	mafViewSingleSlice	*m_ViewSingleSlice;

	mmgLutSwatch		*m_LutWidget; ///< LUT widget in view side panel 
	mmgLutSlider		*m_LutSlider;

	vtkLookupTable	*m_ColorLUT;
};
#endif
