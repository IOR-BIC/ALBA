/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: albaPipeVectorFieldSurface.h,v $ 
  Language: C++ 
  Date: $Date: 2012-04-06 10:06:18 $ 
  Version: $Revision: 1.1.2.3 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#ifndef albaPipeVectorFieldSurface_h__
#define albaPipeVectorFieldSurface_h__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipeVectorField.h"

//----------------------------------------------------------------------------
// Forward declarations:
//----------------------------------------------------------------------------
class albaGUI;

class vtkPolyDataMapper;
class vtkDataSet;
class vtkLookupTable;
class vtkActor;
class vtkScalarBarActor;

/** Displays the surface of input VME (even, if it is volume),
using color mapping according to X,Y,Z or magnitude of associated
vector field (selected in the gui). */
class ALBA_EXPORT albaPipeVectorFieldSurface : public albaPipeVectorField
{
public:
  albaTypeMacro(albaPipeVectorFieldSurface, albaPipeVectorField);

protected:
  /** IDs for the GUI */
  enum PIPE_VIEWFLOW_WIDGET_ID
  {
    ID_VECTORFIELD = Superclass::ID_LAST,     
    ID_COLOR_MAPPING_MODE,
    ID_COLOR_MAPPING_LUT,
    ID_SHOW_COLOR_MAPPING,
    ID_LAST,
  };    

  enum COLOR_MAPPING_MODES
  {
    CMM_MAGNITUDE,
    CMM_X,
    CMM_Y,
    CMM_Z,    
  };
  
  int m_ColorMappingMode;             ///<color mapping mode    
  vtkLookupTable* m_ColorMappingLUT;  ///<lookup table used for the mapping    
  int m_ShowMapping;                  ///<non-zero, if the mapping should be displayed in the main view
  
  vtkScalarBarActor* m_MappingActor;  ///<actor that displays the mapping bar

  vtkPolyDataMapper* m_SurfaceMapper;  ///<mapper for glyphs
  vtkActor* m_SurfaceActor;            ///<actor for glyphs  

public:	
  albaPipeVectorFieldSurface();
  virtual ~albaPipeVectorFieldSurface();

public:  
  /** Processes events coming from GUI */
  /*virtual*/ void OnEvent(albaEventBase *alba_event);

protected:
  /*virtual*/ albaGUI  *CreateGui();

  /** Constructs VTK pipeline. */
  virtual void CreateVTKPipe();

  /** Updates VTK pipeline (setting radius, etc.). */
  virtual void UpdateVTKPipe();  
};
#endif // albaPipeVectorFieldSurface_h__