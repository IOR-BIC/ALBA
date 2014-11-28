/*=========================================================================

 Program: MAF2
 Module: mafViewSliceNotInterpolatedCompound
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafViewSliceNotInterpolatedCompound_H__
#define __mafViewSliceNotInterpolatedCompound_H__

#include "mafViewCompoundWindowing.h"
#include "mafViewSliceNotInterpolated.h"
#include "mafRWI.h"

class wxBoxSizer;
class mafGUIFloatSlider;
class vtkLookupTable;
class medPipeVolumeSliceNotInterpolated;
class mafVMEVolumeGray;
class mafGUILutSwatch;

//----------------------------------------------------------------------------
class MAF_EXPORT mafViewSliceNotInterpolatedCompound : public mafViewCompoundWindowing
//----------------------------------------------------------------------------
{
public:

  /** Gui widget events id */
  enum GUI_WIDGET_ID
  {
    ID_LUT,
    ID_AXIS,
    ID_SLICE,
    MINID,
  };

  /** RTTI Macro */
  mafTypeMacro(mafViewSliceNotInterpolatedCompound, mafViewCompoundWindowing);

  /** ctor */
  mafViewSliceNotInterpolatedCompound(wxString label = "View Slice not interpolated", bool show_ruler = false);

  /** dtor */
  ~mafViewSliceNotInterpolatedCompound(); 

  /** Create visual pipe and initialize them */
  virtual void PackageView();

  /** Create the GUI on the bottom of the compounded view. */
  virtual void CreateGuiView(){Superclass::CreateGuiView();};

  /** Function that clones instance of the object. */
  virtual mafView* Copy(mafObserver *Listener, bool lightCopyEnabled = false);

  /** Create VME visual pipe */
  virtual void VmeShow(mafNode *vme, bool show);

protected:

  /** Create view Gui */
  mafGUI *CreateGui();

  /** Set the axis along slice */
  void SetSliceAxis();

  mafViewSliceNotInterpolated *m_ViewSlice;                        //< Slice view

private:

};

#endif //#ifndef __mafViewSliceNotInterpolatedCompound_H__