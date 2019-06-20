/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: albaPipeVectorFieldSlice.h,v $ 
  Language: C++ 
  Date: $Date: 2012-04-06 10:06:18 $ 
  Version: $Revision: 1.1.2.3 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#ifndef albaPipeVectorFieldSlice_h__
#define albaPipeVectorFieldSlice_h__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipeVectorFieldSurface.h"
#include "albaPipeSlice.h"

//----------------------------------------------------------------------------
// Forward declarations:
//----------------------------------------------------------------------------
class albaGUI;
class albaGUIFloatSlider;
class vtkPlane;

/** Displays slice of input VME colored according to X,Y,Z component 
or magnitude of the associated vector field. 
Note: this class should be derived from albaPipeSlice, but
it would lead into multiple inheritance, which causes lot of troubles*/
class ALBA_EXPORT albaPipeVectorFieldSlice : public albaPipeVectorFieldSurface
{
public:
  albaTypeMacro(albaPipeVectorFieldSlice, albaPipeVectorFieldSurface);

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
  albaPipeVectorFieldSlice();
  virtual ~albaPipeVectorFieldSlice();

public:  
  /** Processes events coming from GUI */
  /*virtual*/ void OnEvent(albaEventBase *alba_event);

protected:
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
#endif // albaPipeVectorFieldSlice_h__