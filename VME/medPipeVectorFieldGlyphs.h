/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: medPipeVectorFieldGlyphs.h,v $ 
  Language: C++ 
  Date: $Date: 2009-08-26 14:47:40 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#ifndef medPipeVectorFieldGlyphs_h__
#define medPipeVectorFieldGlyphs_h__

#include "medPipeVectorField.h"
#include <wx/listctrl.h>

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
class vtkPolyData;
class mafGUIDialog;
class mafGUIButton;

/** General class for Volumes with compound pipes */
class medPipeVectorFieldGlyphs : public medPipeVectorField
{
public:
  mafTypeMacro(medPipeVectorFieldGlyphs, medPipeVectorField);

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
	ID_LIST_RANGES,
	ID_REMOVEITEM,
	ID_ADDITEM,
	ID_SHOWITEM,
	ID_RANGE_NAME,
	ID_RANGE_VALUE1,
	ID_RANGE_VALUE2,
	ID_ITEM_OK,
	ID_ITEM_CANCEL,
	ID_SHOW_ALL,
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
  const static char* FILTER_LINK_NAME;
  static int count ;
  
  int m_GlyphType;                  ///<type of glyph (one of GLYPH_TYPES)
  double m_GlyphRadius;             ///<radius of the glyph
  double m_GlyphLength;             ///<base length of the glyph
  int m_GlyphRes;                   ///<resolution of the glyph
  int m_GlyphScaling;               ///<glyph scaling mode
  
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
  vtkPolyData* m_Output;              //output data for m_Glyphs.
  wxListCtrl* m_RangeCtrl;            //control filter range
  wxButton* m_BttnRemoveItem;         //remove list item
  wxButton* m_BttnAddItem;            //add list item
  wxButton* m_BttnShow;               //show result of this filter
  mafGUIButton* m_ItemOK;
  mafGUIDialog *m_AddItemDlg;         //dlg for input name and value;
  wxString m_FilterName;
  wxString m_FilterValue1;
  wxString m_FilterValue2;
  double m_sr[2];
  int m_ShowAll;                     //1 do not use filter,0 use filter

#pragma region GUI controls
  mafGUIPicButton* m_GlyphMaterialButton;  ///<glyph material button  
  wxStaticText* m_GlyphMaterialLabel;      ///<glyph material label
#pragma endregion GUI controls

public:	
  medPipeVectorFieldGlyphs();
  virtual ~medPipeVectorFieldGlyphs();

public:  

  /** Processes events coming from GUI */
  /*virtual*/ void OnEvent(mafEventBase *maf_event);

protected:

	typedef struct FILTER_ITEM 
	{    
		double value[2];

		FILTER_ITEM* pNext;
		FILTER_ITEM* pLast;
	};

  /** Default radius, etc. should be calculated in this method, 
  i.e., inherited classes should always override this method. 
  The default implementation is to update VME*/
  /*virtual*/ void ComputeDefaultParameters();

  /*virtual*/ mafGUI  *CreateGui();

  /** Constructs VTK pipeline. */
  /*virtual*/ void CreateVTKPipe();

  /** Updates VTK pipeline (setting radius, etc.). */
  /*virtual*/ void UpdateVTKPipe();
  /** Add an item in the list */
  void OnAddItem();
  /** create a dialog*/
  void createAddItemDlg();
  /** insert an item into list if values are valid */
  bool AddItem();
  /** update data structure and update view */	
  void OnShowFilter();
  /** remove an item from range list */
  void OnRemoveItem();
  /** use filter */
  void doFilter(double *value);
  /** store filter data */
  void StoreFilterLinks();
  /** init filter list  */
  void InitFilterList();
  /** Handles change of material. */
  virtual void OnChangeMaterial(); 


};
#endif // medPipeVectorFieldGlyphs_h__