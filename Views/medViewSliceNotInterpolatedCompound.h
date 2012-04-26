/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medViewSliceNotInterpolatedCompound.h,v $
  Language:  C++
  Date:      $Date: 2012-04-26 12:53:59 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Alberto Losi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
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