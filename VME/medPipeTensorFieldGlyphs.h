/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: medPipeTensorFieldGlyphs.h,v $ 
  Language: C++ 
  Date: $Date: 2010-06-16 07:04:40 $ 
  Version: $Revision: 1.1.2.5 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#ifndef medPipeTensorFieldGlyphs_h__
#define medPipeTensorFieldGlyphs_h__

#include "medPipeTensorField.h"
#include <wx/listctrl.h>

class mafGUI;
class mafGUIPicButton;
class mafVMESurface;

class vtkPolyDataMapper;
class vtkDataSet;
class vtkTensorGlyph;
class vtkSphereSource;
class vtkAxes;
class vtkArrowSource;
class vtkLookupTable;
class vtkActor;
class vtkScalarBarActor;

class vtkPolyData;
class mafGUIButton;
class mafGUIDialog;
class vtkFloatArray;

/** 
class name: medPipeTensorFieldGlyphs
Class for Volumes with compound pipes which show tensor field using glyphs.
 */
class medPipeTensorFieldGlyphs : public medPipeTensorField
{
public:
  /** RTTI macro*/
  mafTypeMacro(medPipeTensorFieldGlyphs, medPipeTensorField);

protected:
  /** IDs for the GUI */
  enum PIPE_VIEWFLOW_WIDGET_ID
  {
    ID_TENSORFIELD = Superclass::ID_LAST,     
    ID_SCALARFIELD,
    ID_GLYPH_TYPE,    
    ID_GLYPH_RADIUS,
    ID_GLYPH_RESOLUTION,    
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
	//------------
	ID_REMOVEITEM2,
	ID_ADDITEM2,
	ID_SHOWITEM2,
	ID_RANGE_NAME2,
	ID_RANGE_VALUE1_2,
	ID_RANGE_VALUE2_2,
	ID_ITEM_OK2,
	ID_ITEM_CANCEL2,
	ID_CHOOSE_ANDOR,
	
	ID_SHOWITEM_ASSOCIATE,
	//------------

	ID_SHOW_ALL,
    ID_LAST,
  };  

  enum GLYPH_TYPES
  {
    GLYPH_AXES,
	GLYPH_ELLIPSOID,    
    GLYPH_ARROWS,
  };
  const static char* FILTER_LINK_NAME;
  const static char* FILTER_LINK_NAME2;
  static int count ;

  int m_GlyphType;                  ///<type of glyph (one of GLYPH_TYPES)  
  double m_GlyphRadius;             ///<radius of the glyph
  int m_GlyphRes;                   ///<resolution of the glyph
  int m_GlyphScaling;               ///<non-zero, if glyphs should be scaled by corresponding eigen values
  
  mafVMESurface* m_GlyphMaterial;     ///<material used for glyphs  
  vtkLookupTable* m_ColorMappingLUT;  ///<lookup table used for the mapping
  int m_UseColorMapping;              ///<non-zero, if colors should be mapped using LUT
  int m_UseSFColorMapping;            ///<non-zero, if scalars should be used for mapping  
  int m_ShowMap;                      ///<non-zero, if the mapping should be displayed in the main view
  
  vtkSphereSource* m_GlyphEllipsoid;  ///<line glyph representing a tensor  
  vtkAxes*  m_GlyphAxes;
  vtkArrowSource* m_GlyphArrow;       ///<arrow glyph representing a tensor
  
  vtkTensorGlyph* m_Glyphs;           ///<filter for the visualization of tensors
  vtkPolyDataMapper* m_GlyphsMapper;  ///<mapper for glyphs
  vtkActor* m_GlyphsActor;            ///<actor for glyphs
  vtkScalarBarActor* m_SFActor;       ///<actor that displays the mapping bat

  vtkPolyData* m_Output;              //output data for m_Glyphs.
  wxListCtrl* m_RangeCtrl;            //control filter range
  wxListCtrl* m_RangeCtrl2;
  wxButton* m_BttnRemoveItem;         //remove list item
    wxButton* m_BttnRemoveItem2;         //remove list item
  wxButton* m_BttnAddItem;            //add list item
  wxButton* m_BttnAddItem2;
  wxButton* m_BttnShow;               //show result of this filter
  wxButton* m_BttnShow2;               //show result of second filter
  mafGUIButton* m_ItemOK;
    mafGUIButton* m_ItemOK2;
  mafGUIDialog *m_AddItemDlg;         //dlg for input name and value;
  wxString m_FilterName;
  wxString m_FilterValue1;
  wxString m_FilterValue2;
  double m_Sr[2];
  double m_Sr2[2];
  wxButton* m_BttnShowAssociate;      //show result of two filter
  int m_AndOr;

  int m_ShowAll;                     //1 do not use filter,0 use filter
  vtkFloatArray* m_DataScale_Copy;


#pragma region GUI controls
  mafGUIPicButton* m_GlyphMaterialButton;  ///<glyph material button  
  wxStaticText* m_GlyphMaterialLabel;      ///<glyph material label
#pragma endregion GUI controls

public:
  /** constrouctor */
  medPipeTensorFieldGlyphs();
  /** destructor */
  virtual ~medPipeTensorFieldGlyphs();

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

  /** gui creation*/
  /*virtual*/ mafGUI  *CreateGui();

  /** Constructs VTK pipeline. */
  /*virtual*/ void CreateVTKPipe();

  /** Updates VTK pipeline (setting radius, etc.). */
  /*virtual*/ void UpdateVTKPipe();

  /** Add an item in the list */
  void OnAddItem(int idx);
  /** create a dialog*/
  void CreateAddItemDlg(int idx);
  /** insert an item into list if values are valid */
  bool AddItem();
  /** insert an item into list if values are valid */
  bool AddItem2();
  /** update data structure and update view */	
  void OnShowFilter(int mode);
  /** remove an item from range list */
  void OnRemoveItem();
  /** remove an item from range list */
  void OnRemoveItem2();
  /** use filter */
  void DoFilter(int mode ,double *rangeValue,double *rangeValue2);
  /** store filter data */
  void StoreFilterLinks();
  /** store filter data */
  void StoreFilterLinks2();
  /** init filter list  */
  void InitFilterList(int nScalars);
  /** to test if this point in range */
  bool DoCondition(int mode ,double tensorValue,double scaleValue,double *rangeValue1,double *rangeValue2);

  /** Handles change of material. */
  virtual void OnChangeMaterial(); 
};
#endif // medPipeTensorFieldGlyphs_h__