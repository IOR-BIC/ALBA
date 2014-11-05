/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: medPipeTensorFieldSlice.h,v $ 
  Language: C++ 
  Date: $Date: 2012-04-06 10:05:09 $ 
  Version: $Revision: 1.1.2.4 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#ifndef medPipeTensorFieldSlice_h__
#define medPipeTensorFieldSlice_h__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medVMEDefines.h"
#include "medPipeTensorFieldSurface.h"
#include "mafPipeSlice.h"

//----------------------------------------------------------------------------
// Forward declarations:
//----------------------------------------------------------------------------
class mafGUI;
class mafGUIFloatSlider;

class vtkPlane;

/** 
class name: medPipeTensorFieldSlice
Displays slice of input VME colored according to X,Y,Z component 
or magnitude of the associated tensor field. 
Note: this class should be derived from mafPipeSlice, but
it would lead into multiple inheritance, which causes lot of troubles*/
class MED_VME_EXPORT medPipeTensorFieldSlice : public medPipeTensorFieldSurface
{
public:
  /** RTTI macro */
  mafTypeMacro(medPipeTensorFieldSlice, medPipeTensorFieldSurface);

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

  mafGUIFloatSlider* m_SliderSlice; ///<widget for m_SlicePos
  wxTextCtrl* m_TextSlice;          ///<widget for m_SlicePos

public:
  /** constructor */
  medPipeTensorFieldSlice();
  /** destructor */
  virtual ~medPipeTensorFieldSlice();

public:  
  /** Processes events coming from GUI */
  /*virtual*/ void OnEvent(mafEventBase *maf_event);

protected:
  /** gui creation */
  /*virtual*/ mafGUI  *CreateGui();

  /** Constructs VTK pipeline. */
  virtual void CreateVTKPipe();

  /** Updates VTK pipeline (setting radius, etc.). */
  virtual void UpdateVTKPipe();  

  /** Updates the range of slice position.
  If the current slice position is outside the range, it is changed. 
  Sets new validators for slice position widgets.*/
  virtual void UpdateSlicePosRange();
};
#endif // medPipeTensorFieldSlice_h__