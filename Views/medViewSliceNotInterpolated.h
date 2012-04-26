/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medViewSliceNotInterpolated.h,v $
  Language:  C++
  Date:      $Date: 2012-04-26 12:53:59 $
  Version:   $Revision: 1.1.2.2 $
  Authors:   Alberto Losi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medViewSliceNotInterpolated_H__
#define __medViewSliceNotInterpolated_H__

#include "mafViewVTK.h"
#include "medViewsDefines.h"

class wxBoxSizer;
class mafGUIFloatSlider;
class vtkLookupTable;
class medPipeVolumeSliceNotInterpolated;
class mafVMEVolumeGray;
class mafGUILutSwatch;

//----------------------------------------------------------------------------
class MED_VIEWS_EXPORT medViewSliceNotInterpolated : public mafViewVTK
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
  mafTypeMacro(medViewSliceNotInterpolated, mafViewVTK);

  /** ctor */
  medViewSliceNotInterpolated(wxString label = "View Slice not interpolated", bool show_ruler = false);

  /** dtor */
  ~medViewSliceNotInterpolated(); 

  /** Create visual pipe and initialize them */
  virtual void PackageView();

  /** Create the GUI on the bottom of the compounded view. */
  /*virtual void CreateGuiView(){Superclass::CreateGuiView();};*/

  /** Function that clones instance of the object. */
  virtual mafView* Copy(mafObserver *Listener, bool lightCopyEnabled = false);

  /** Create VME visual pipe */
  virtual void VmeShow(mafNode *vme, bool show);

  /** Process events coming from gui */
  virtual void OnEvent(mafEventBase * event);

  /** Set the axis along slice */
  void SetSliceAxis(int axis);

  /** Set the slice position */
  void SetSlice(double slicePosition);

  /** Set the slice position */
  void SetSlice(double origin[3]);

protected:

  /** Create view Gui */
  mafGUI *CreateGui();

  /** Set the axis along slice */
  void SetSliceAxis();

  int m_SliceAxis;                                //> Slicer direction
  double m_Bounds[6];                             //> Input volume bounds
  double m_CurrentSlice;                          //> Current slice coordinate
  mafGUIFloatSlider *m_SliceSlider;               //> Slice coordinate slider
  mafGUILutSwatch *m_LutSwatch;                   //> Lut swatch widget
  std::map<mafNode*,medPipeVolumeSliceNotInterpolated*> m_PipesSlice; //> Slice visual pipe
  vtkLookupTable *m_ColorLUT;

private:

  /** Enable disable gui widgets */
  void EnableGuiWidgets(bool enable);
  
  /** Fill volume parameters as bounds and lut */
  void GetVolumeParameters(mafVMEVolumeGray *volume);

  /** Update slice visualization */
  void UpdateSlice();
};

#endif //#ifndef __medViewSliceNotInterpolated_H__