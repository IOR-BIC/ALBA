/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: medPipeVectorField.h,v $ 
  Language: C++ 
  Date: $Date: 2009-06-08 15:11:33 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#ifndef medPipeVectorField_h__
#define medPipeVectorField_h__

#include "mafPipe.h"

class mafGUI;
class mafGUIPicButton;
class mafVMESurface;

class vtkPolyDataMapper;
class vtkDataSet;
class vtkGlyph3D;
class vtkLineSource;
class vtkConeSource;
class vtkArrowSource;
class vtkLookupTable;
class vtkActor;
class vtkScalarBarActor;

/** General class for Volumes with compound pipes */
class medPipeVectorField : public mafPipe
{
public:
  mafTypeMacro(medPipeVectorField, mafPipe);

protected:
  /** IDs for the GUI */
  enum PIPE_VIEWFLOW_WIDGET_ID
  {
    ID_VECTORFIELD = Superclass::ID_LAST,     
    ID_SCALARFIELD,
    ID_GLYPH_TYPE,
    ID_GLYPH_RADIUS,
    ID_GLYPH_RESOLUTION,
    ID_GLYPH_LENGTH,
    ID_GLYPH_SCALING,    
    ID_GLYPH_MATERIAL,    
    ID_GLYPH_MATERIAL_LABEL,
    ID_USE_COLOR_MAPPING,    
    ID_USE_SF_MAPPING,    
    ID_SF_MAPPING_LUT,
    ID_SHOW_SF_MAPPING,
    ID_LAST,
  };  

  enum GLYPH_TYPES
  {
    GLYPH_LINES,
    GLYPH_CONES,
    GLYPH_ARROWS,
  };

  enum GLYPH_SCALING
  {
    SCALING_OFF,
    SCALING_MAGNITUDE,
    SCALING_SCALARS,
  };

  int m_VectorFieldIndex;           ///<index of vector field to be processed
  int m_GlyphType;                  ///<type of glyph (one of GLYPH_TYPES)
  double m_GlyphRadius;             ///<radius of the glyph
  double m_GlyphLength;             ///<base length of the glyph
  int m_GlyphRes;                   ///<resolution of the glyph

  int m_GlyphScaling;               ///<glyph scaling mode
  int m_ScalarFieldIndex;        ///<index of scalar field whose values are used to scale glyphs
  
  mafVMESurface* m_GlyphMaterial;     ///<material used for glyphs  
  vtkLookupTable* m_ColorMappingLUT;  ///<lookup table used for the mapping
  int m_UseColorMapping;              ///<non-zero, if colors should be mapped using LUT
  int m_UseSFColorMapping;            ///<non-zero, if scalars should be used for mapping  
  int m_ShowMap;                      ///<non-zero, if the mapping should be displayed in the main view
  
  vtkLineSource* m_GlyphLine;     ///<line glyph representing a vector
  vtkConeSource* m_GlyphCone;         ///<cone glyph representing a vector
  vtkArrowSource* m_GlyphArrow;       ///<arrow glyph representing a vector
  
  vtkGlyph3D* m_Glyphs;               ///<filter for the visualization of vectors
  vtkPolyDataMapper* m_GlyphsMapper;  ///<mapper for glyphs
  vtkActor* m_GlyphsActor;            ///<actor for glyphs
  vtkScalarBarActor* m_SFActor;       ///<actor that displays the mapping bat

#pragma region GUI controls
  mafGUIPicButton* m_GlyphMaterialButton;  ///<glyph material button  
  wxStaticText* m_GlyphMaterialLabel;      ///<glyph material label
#pragma endregion GUI controls

public:	
  medPipeVectorField();
  virtual ~medPipeVectorField();

public:
  /** Creates the VTK rendering pipeline */
  /*virtual*/ void Create(mafSceneNode *n);

  /** Processes events coming from GUI */
  /*virtual*/ void OnEvent(mafEventBase *maf_event);

protected:
  /*virtual*/ mafGUI  *CreateGui();

  /** Constructs VTK pipeline. */
  virtual void CreateVTKPipe();

  /** Updates VTK pipeline (setting radius, etc.). */
  virtual void UpdateVTKPipe();

  /** Handles change of material. */
  virtual void OnChangeMaterial();

  /** Returns name of vector field at the the specified index.  
  Returns NULL, if it fails to find the appropriate field.*/
  inline const char* GetVectorFieldName(int nIndex) {
    return GetFieldName(nIndex, true);
  }

  /** Returns name of scalar field at the the specified index.  
  Returns NULL, if it fails to find the appropriate field.*/
  inline const char* GetScalarFieldName(int nIndex) {
    return GetFieldName(nIndex, false);    
  }

  /** 
  Returns the name of field (scalar or vectors depending on
  bVectors parameter) at the specified index. 
  The routine returns NULL, if it cannot find appropriate field. */
  const char* GetFieldName(int nIndex, bool bVectors = true);

  /** Returns index of vector field with the specified name.  
  Returns -1, if it fails to find the appropriate field.*/
  inline int GetVectorFieldIndex(const char* szName = NULL) {
    return GetFieldIndex(NULL, true);
  }

  /** Returns index of vector field with the specified name.  
  Returns -1, if it fails to find the appropriate field.*/
  inline int GetScalarFieldIndex(const char* szName = NULL) {
    return GetFieldIndex(NULL, false);    
  }

  /** 
  Returns the index of specified field (scalar or vectors depending on
  bVectors parameter). If it cannot be found, the index of currently active
  (scalar or vector) field is returned.
  The routine returns -1, if it cannot find appropriate field. */
  int GetFieldIndex(const char* szName = NULL, bool bVectors = true);
  
  /** Returns the number of available vectors. */
  inline int GetNumberOfVectors() {
    return GetNumberOfFields(true);
  }

  /** Returns the number of available scalars. */
  inline int GetNumberOfScalars() {
    return GetNumberOfFields(false);
  }

  /** Returns the number of available scalars/vectors. */
  int GetNumberOfFields(bool bVectors = true);  

  /** Populates the combo box by names of scalar/vector fields */
  void PopulateCombo(wxComboBox* combo, bool bVectors);
};
#endif // medPipeVectorField_h__