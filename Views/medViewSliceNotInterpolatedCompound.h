/*=========================================================================

 Program: MAF2Medical
 Module: medViewSliceNotInterpolatedCompound
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medViewSliceNotInterpolatedCompound_H__
#define __medViewSliceNotInterpolatedCompound_H__

#include "medViewCompoundWindowing.h"
#include "medViewSliceNotInterpolated.h"
#include "mafRWI.h"

class wxBoxSizer;
class mafGUIFloatSlider;
class vtkLookupTable;
class medPipeVolumeSliceNotInterpolated;
class mafVMEVolumeGray;
class mafGUILutSwatch;

//----------------------------------------------------------------------------
class MED_VIEWS_EXPORT medViewSliceNotInterpolatedCompound : public medViewCompoundWindowing
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
  mafTypeMacro(medViewSliceNotInterpolatedCompound, medViewCompoundWindowing);

  /** ctor */
  medViewSliceNotInterpolatedCompound(wxString label = "View Slice not interpolated", bool show_ruler = false);

  /** dtor */
  ~medViewSliceNotInterpolatedCompound(); 

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

  medViewSliceNotInterpolated *m_ViewSlice;                        //< Slice view

private:

};

#endif //#ifndef __medViewSliceNotInterpolatedCompound_H__