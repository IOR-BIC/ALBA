/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeVectorFieldMapWithArrows.h,v $
  Language:  C++
  Date:      $Date: 2010-12-09 16:43:02 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Simone Brazzale
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef medPipeVectorFieldMapWithArrows_h__
#define medPipeVectorFieldMapWithArrows_h__

#include "medPipeVectorFieldSurface.h"

class mafGUI;

class vtkGlyph3D;
class vtkPolyDataMapper;
class vtkDataSet;
class vtkLookupTable;
class vtkActor;
class vtkScalarBarActor;

/** Displays the surface of input VME (even, if it is volume),
using 
1) color mapping according to X,Y,Z or magnitude of associated
vector field (selected in the gui)
2) vector mappins accordinf to the vector field (selected in the gui)
*/
class medPipeVectorFieldMapWithArrows : public medPipeVectorField
{
public:
  mafTypeMacro(medPipeVectorFieldMapWithArrows, medPipeVectorField);

protected:
  /** IDs for the GUI */
  enum PIPE_VIEWFLOW_WIDGET_ID
  {
    ID_VECTORFIELD = Superclass::ID_LAST,   
    ID_SCALARFIELD,  
    ID_SCALAR_COLOR_MAPPING_MODE,
    ID_VECTOR_COLOR_MAPPING_MODE,
    ID_COLOR_MAPPING_LUT,
    ID_SHOW_COLOR_MAPPING,  
    ID_SHOW_SURFACE,
    ID_SHOW_GLYPHS,
    ID_ACTIVATE_SCALARS,
    ID_ACTIVATE_VECTORS,
    ID_SCALESLIDER,
    ID_LAST,
  };   

  enum COLOR_MAPPING_MODES
  {
    CMM_MAGNITUDE,
    CMM_X,
    CMM_Y,
    CMM_Z, 
    CMM_LAST,
  };
  
  int m_ScalarColorMappingMode;       ///< Color mapping mode for scalars (dummy)
  int m_VectorColorMappingMode;       ///< Color mapping mode for vectors
  int m_ActivateScalars;              ///< Activate scalar fields list
  int m_ActivateVectors;              ///< Activate vector fields list
  vtkLookupTable* m_ColorMappingLUT;  ///< Lookup table used for the mapping    
  int m_ShowMapping;                  ///< Non-zero, if the mapping should be displayed in the main view
  int m_ShowSurface;                  ///< Non-zero, if the surface should be displayed in the main view
  int m_ShowGlyphs;                   ///< Non-zero, if the glyphs should be displayed in the main view
  int m_ScalingValue;                 ///< Scaling value for glyphs
  
  vtkScalarBarActor* m_MappingActor;  ///<actor that displays the mapping bar*/

  vtkPolyDataMapper* m_SurfaceMapper;  ///< Mapper for surface
  vtkActor* m_SurfaceActor;            ///< Actor for surface  

  vtkGlyph3D* m_Glyph;                ///< Glyph3D filter
  vtkPolyDataMapper* m_GlyphMapper;   ///< Mapper for glyphs
  vtkActor* m_GlyphActor;             ///< Actor for glyphs  

  wxComboBox* m_ComboField_s;         ///< Combo box for scalar fields GUI
  wxComboBox* m_ComboField_v;         ///< Combo box for vector fields GUI
  wxComboBox* m_ComboColorBy_s;       ///< Combo box for scalar fields mapping criteria
  wxComboBox* m_ComboColorBy_v;       ///< Combo box for vector fields mapping criteria

public:	
  medPipeVectorFieldMapWithArrows();
  virtual ~medPipeVectorFieldMapWithArrows();

public:  
  /** Processes events coming from GUI */
  /*virtual*/ void OnEvent(mafEventBase *maf_event);

protected:
  /*virtual*/ mafGUI  *CreateGui();

  /** Constructs VTK pipeline. */
  virtual void CreateVTKPipe();

  /** Updates VTK pipeline (setting radius, etc.). */
  virtual void UpdateVTKPipe();  
};
#endif // medPipeVectorFieldSurface_h__