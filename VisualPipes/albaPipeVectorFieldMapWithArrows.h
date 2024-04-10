/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVectorFieldMapWithArrows
 Authors: Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef albaPipeVectorFieldMapWithArrows_h__
#define albaPipeVectorFieldMapWithArrows_h__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipeVectorFieldSurface.h"

//----------------------------------------------------------------------------
// Forward declarations:
//----------------------------------------------------------------------------
class albaGUI;
class vtkArrowSource;
class vtkLineSource;
class vtkConeSource;
class vtkGlyph3D;
class vtkPolyDataMapper;
class vtkDataSet;
class vtkLookupTable;
class vtkActor;
class vtkScalarBarActor;
class vtkAssignAttribute;

/** Displays the surface of input VME using vector or scalar fields.
- SCALARS: surface color is mapped according to the scalar value (selected in the gui); arrows length and color are mapped according to scalar value; arrows direction is normal to the point or cell.
- VECTORS: surface and arrows color are mapped according to X,Y,Z or magnitude of associated vector value (selected in the gui); arrows are mapped according to the vector (magnitude and direction).
NB: The pipe supports time-varying VME, but to update correctly scalars and vectors one must call the UpdateVTKPipe method IN THE ASSOCIATED VIEW!
NB: The pipe looks for scalar and vectors in the point data; if it finds them, only point data fields are displayed! If they are not found, the pipe looks to the cell data.
TO DO: Implement a way to display both cell data and point data.
*/
class ALBA_EXPORT albaPipeVectorFieldMapWithArrows : public albaPipeVectorField
{
public:
  albaTypeMacro(albaPipeVectorFieldMapWithArrows, albaPipeVectorField);

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
    ID_ENABLE_MAP,
    ID_GLYPH_RADIUS,
    ID_GLYPH_RESOLUTION,
    ID_GLYPH_LENGTH,
    ID_GLYPH_TYPE,
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

  enum GLYPH_TYPES
  {
    GLYPH_LINES,
    GLYPH_ARROWS,
  };

  enum DATA_TYPES
  {
    POINT_DATA,
    CELL_DATA,
  };

  int m_DataType;                     ///< Type of data to which map are associated (points, cells, fields)  

  int m_GlyphType;                    ///< Type of glyph (one of GLYPH_TYPES)
  int m_ScalarColorMappingMode;       ///< Color mapping mode for scalars (dummy)
  int m_VectorColorMappingMode;       ///< Color mapping mode for vectors
  int m_ActivateScalars;              ///< Activate scalar fields list
  int m_ActivateVectors;              ///< Activate vector fields list
  vtkLookupTable* m_ColorMappingLUT;  ///< Lookup table used for the mapping    
  int m_ShowMapping;                  ///< Non-zero, if the mapping should be displayed in the main view
  int m_ShowSurface;                  ///< Non-zero, if the surface should be displayed in the main view
  int m_ShowGlyphs;                   ///< Non-zero, if the glyphs should be displayed in the main view
  int m_EnableMap;                    ///< Non-zero, if surface and glyphs should be colored in the main view
  int m_ScalingValue;                 ///< Scaling value for glyphs
  float m_GlyphRadius;                ///< Glyph radius
  int m_GlyphRes;                     ///< Glyph resolution
  float m_GlyphLength;                ///< Glyph tip length
  
  vtkScalarBarActor* m_MappingActor;  ///< Actor that displays the mapping bar

  vtkPolyDataMapper* m_SurfaceMapper; ///< Mapper for surface
  vtkActor* m_SurfaceActor;           ///< Actor for surface  

  vtkLineSource* m_Line;              ///< Line glyph representing a vector
  vtkArrowSource *m_Arrow;            ///< Arrow glyph representing a vector

  vtkGlyph3D* m_Glyph;                ///< Glyph3D filter
	vtkAssignAttribute *m_AttributeAssigner; ////< Attribute Assigner for vector selection
  vtkPolyDataMapper* m_GlyphMapper;   ///< Mapper for glyphs
  vtkActor* m_GlyphActor;             ///< Actor for glyphs  

  wxComboBox* m_ComboField_s;         ///< Combo box for scalar fields GUI
  wxComboBox* m_ComboField_v;         ///< Combo box for vector fields GUI
  wxComboBox* m_ComboColorBy_s;       ///< Combo box for scalar fields mapping criteria
  wxComboBox* m_ComboColorBy_v;       ///< Combo box for vector fields mapping criteria

  wxTextCtrl* m_EdLength;             ///< Editor for tip length
  wxTextCtrl* m_EdRes;                ///< Editor for tip resolution
  wxTextCtrl* m_EdRadius;             ///< Editor for tip radius

public:	
  albaPipeVectorFieldMapWithArrows();
  virtual ~albaPipeVectorFieldMapWithArrows();

public:  
  /** Processes events coming from GUI */
  /*virtual*/ void OnEvent(albaEventBase *alba_event);

  /** Updates VTK pipeline (setting radius, etc.). 
  NB: This function needs to be declared public in order to update correctly views during timebar shifts. */
  virtual void UpdateVTKPipe();  

protected:
  /*virtual*/ albaGUI  *CreateGui();

  /** Constructs VTK pipeline. */
  virtual void CreateVTKPipe();

  /** Populates the combo box by names of scalar/vector fields */
  virtual void PopulateCombo(wxComboBox* combo, bool bVectors);

  /** Returns the number of available scalars/vectors. */
  virtual int GetNumberOfFields(bool bVectors = true); 

  /** 
  Returns the name of field (scalar or vectors depending on
  bVectors parameter) at the specified index. 
  The routine returns NULL, if it cannot find appropriate field. */
  virtual const char* GetFieldName(int nIndex, bool bVectors = true);

  /** 
  Returns the index of specified field (scalar or vectors depending on
  bVectors parameter). If it cannot be found, the index of currently active
  (scalar or vector) field is returned.
  The routine returns -1, if it cannot find appropriate field. */
  virtual int GetFieldIndex(const char* szName = NULL, bool bVectors = true);

};
#endif // albaPipeVectorFieldSurface_h__
