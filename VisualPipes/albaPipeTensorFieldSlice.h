/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: albaPipeTensorFieldSlice.h,v $ 
  Language: C++ 
  Date: $Date: 2012-04-06 10:05:09 $ 
  Version: $Revision: 1.1.2.4 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#ifndef albaPipeTensorFieldSlice_h__
#define albaPipeTensorFieldSlice_h__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipeTensorFieldSurface.h"
#include "albaPipeSlice.h"

//----------------------------------------------------------------------------
// Forward declarations:
//----------------------------------------------------------------------------
class albaGUI;
class albaGUIFloatSlider;

class vtkPlane;

/** 
class name: albaPipeTensorFieldSlice
Displays slice of input VME colored according to X,Y,Z component 
or magnitude of the associated tensor field. 
Note: this class should be derived from albaPipeSlice, but
it would lead into multiple inheritance, which causes lot of troubles*/
class ALBA_EXPORT albaPipeTensorFieldSlice : public albaPipeTensorFieldSurface
{
public:
  /** RTTI macro */
  albaTypeMacro(albaPipeTensorFieldSlice, albaPipeTensorFieldSurface);

protected:
  /** IDs for the GUI */
  enum PIPE_VIEWFLOW_WIDGET_ID
  {
    ID_SLICE_DIRECTION = Superclass::ID_LAST,
    ID_SLICE_POS_SLIDER,
    ID_SLICE_POS,
    ID_LAST,
  };    

  enum SLICE_DIRECTIONS
  {
    DIR_X,
    DIR_Y,
    DIR_Z,
  };
  
  int m_SliceDirection;             ///<selected slice direction
  double m_SlicePos;                ///<slice position  

  vtkPlane* m_CutPlane;             ///<plane used to cut the input VME  

  albaGUIFloatSlider* m_SliderSlice; ///<widget for m_SlicePos
  wxTextCtrl* m_TextSlice;          ///<widget for m_SlicePos

public:
  /** constructor */
  albaPipeTensorFieldSlice();
  /** destructor */
  virtual ~albaPipeTensorFieldSlice();

public:  
  /** Processes events coming from GUI */
  /*virtual*/ void OnEvent(albaEventBase *alba_event);

protected:
  /** gui creation */
  /*virtual*/ albaGUI  *CreateGui();

  /** Constructs VTK pipeline. */
  virtual void CreateVTKPipe();

  /** Updates VTK pipeline (setting radius, etc.). */
  virtual void UpdateVTKPipe();  

  /** Updates the range of slice position.
  If the current slice position is outside the range, it is changed. 
  Sets new validators for slice position widgets.*/
  virtual void UpdateSlicePosRange();
};
#endif // albaPipeTensorFieldSlice_h__