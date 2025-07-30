/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: albaPipeTensorFieldGlyphs.cpp,v $ 
  Language: C++ 
  Date: $Date: 2010-06-25 15:01:02 $ 
  Version: $Revision: 1.1.2.6 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  modify: Hui Wei (beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaDecl.h"
#include "albaPipeTensorFieldGlyphs.h"

#include "albaSceneNode.h"
#include "albaPipeFactory.h"
#include "albaGUI.h"
#include "albaGUIValidator.h"
#include "albaGUILutSwatch.h"
#include "albaGUILutPreset.h"
#include "albaGUIPicButton.h"
#include "albaGUIMaterialChooser.h"
#include "albaVME.h"
#include "albaVMESurface.h"
#include "mmaMaterial.h"

#include "vtkALBAAssembly.h"
#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkDataArray.h"
#include "vtkDoubleArray.h"
#include "vtkLookupTable.h"
#include "vtkTensorGlyph.h"

#include "vtkSphereSource.h"
#include "vtkArrowSource.h"

#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkScalarBarActor.h"
#include "vtkRenderer.h"
#include "vtkAxes.h"
#include "vtkFloatArray.h"

#include "albaGUIBusyInfo.h"
#include <float.h>
#include <math.h>
#include <wx/tokenzr.h>
#include "albaGUIButton.h"
#include "albaGUIDialog.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeTensorFieldGlyphs);
//----------------------------------------------------------------------------
const  char* albaPipeTensorFieldGlyphs::FILTER_LINK_NAME = "filter-link-tensor";
const  char* albaPipeTensorFieldGlyphs::FILTER_LINK_NAME2 = "filter-link-scale";
int albaPipeTensorFieldGlyphs::count; 

//----------------------------------------------------------------------------
albaPipeTensorFieldGlyphs::albaPipeTensorFieldGlyphs() : albaPipeTensorField()
//----------------------------------------------------------------------------
{    
  m_GlyphType = GLYPH_AXES;
  m_GlyphRadius = 0.1;  
  m_GlyphRes = 3;
  
  m_GlyphScaling = 1;  
  
  m_GlyphMaterial = NULL;
  m_ColorMappingLUT = NULL;

  m_UseColorMapping = 1;
  m_UseSFColorMapping = 0;  
  m_ShowMap = 1;  
  
  m_GlyphAxes = NULL;
  m_GlyphEllipsoid = NULL;  
  m_GlyphArrow = NULL;

  m_Glyphs = NULL;
  m_GlyphsMapper = NULL;
  m_GlyphsActor = NULL;    
  
  m_SFActor = NULL; 
  m_AndOr = 0;
  m_ShowAll = 1;//that means do not use filter
   albaPipeTensorFieldGlyphs::count = 0;
}

//----------------------------------------------------------------------------
albaPipeTensorFieldGlyphs::~albaPipeTensorFieldGlyphs()
//----------------------------------------------------------------------------
{  
  m_AssemblyFront->RemovePart(m_GlyphsActor);  
  m_RenFront->RemoveActor2D(m_SFActor);  
  
  vtkDEL(m_SFActor);
  
  vtkDEL(m_GlyphsActor);
  vtkDEL(m_GlyphsMapper);
  vtkDEL(m_Glyphs);
  vtkDEL(m_GlyphArrow);
  vtkDEL(m_GlyphEllipsoid);  
  vtkDEL(m_GlyphAxes);
  
  vtkDEL(m_ColorMappingLUT);
  albaDEL(m_GlyphMaterial);   

  vtkDEL(m_DataScale_Copy);
}


//----------------------------------------------------------------------------
//Default radius, etc. should be calculated in this method, 
//i.e., inherited classes should always override this method. 
//The default implementation is to update VME
/*virtual*/ void albaPipeTensorFieldGlyphs::ComputeDefaultParameters()
//----------------------------------------------------------------------------
{
  Superclass::ComputeDefaultParameters();

  vtkDataSet* ds = m_Vme->GetOutput()->GetVTKData();
  double dblDiag = ds->GetLength();
  double nPoints = (double)ds->GetNumberOfPoints();
      
  m_GlyphRadius = dblDiag / (2*pow(nPoints, 1 / 3.0));  
  //should be scaled by 100 for some data
}

//----------------------------------------------------------------------------
albaGUI *albaPipeTensorFieldGlyphs::CreateGui()
//----------------------------------------------------------------------------
{
  int nTensors = GetNumberOfTensors();
  int nScalars = GetNumberOfScalars();

  m_Gui = new albaGUI(this);
     
  wxBoxSizer* bSizerMain = new wxBoxSizer( wxVERTICAL );
  if (nTensors == 0)  //no tensor field available, this pipe does not work
    bSizerMain->Add(new wxStaticText( m_Gui, wxID_ANY, _("No tensor field to visualize.")));
  else
  {
    if (nTensors > 1)      
    {
      wxBoxSizer* bSizer1 = new wxBoxSizer( wxHORIZONTAL );
      bSizer1->Add( new wxStaticText( m_Gui, wxID_ANY, _("Tensor Field:"), 
        wxDefaultPosition, wxSize( 65,-1 ), 0 ), 0, wxALL, 5 );

      wxComboBox* comboField = new wxComboBox( m_Gui, ID_TENSORFIELD, 
        wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
      comboField->SetToolTip(_("Selects the tensor field to be visualized.") );
      PopulateCombo(comboField, true);  //at least one tensor is available
      bSizer1->Add( comboField, 1, wxALL, 1 );
      bSizerMain->Add( bSizer1, 0, wxEXPAND, 0 );

      //and validator
      comboField->SetValidator(albaGUIValidator(this, ID_TENSORFIELD, comboField, &m_TensorFieldIndex));
    }

    if (nScalars > 1)
    {
      wxBoxSizer* bSizer51111 = new wxBoxSizer( wxHORIZONTAL );  
      bSizer51111->Add( new wxStaticText( m_Gui, wxID_ANY, _("Scalar Field:"), 
        wxDefaultPosition, wxSize( 65,-1 ), 0 ), 0, wxALL, 5 );

      wxComboBox* comboScalars = new wxComboBox( m_Gui, ID_SCALARFIELD, 
        wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
      comboScalars->SetToolTip( 
        _("Selects the scalar field to be used for coloring of glyphs.") );
      PopulateCombo(comboScalars, false);
      bSizer51111->Add( comboScalars, 1, wxALL, 1 );
      bSizerMain->Add( bSizer51111, 0, wxEXPAND, 5 );

      //and validator
      comboScalars->SetValidator(albaGUIValidator(this, ID_SCALARFIELD, comboScalars, &m_ScalarFieldIndex));
    }

    wxBoxSizer* bSizer52 = new wxBoxSizer( wxHORIZONTAL );
    bSizer52->Add( new wxStaticText( m_Gui, wxID_ANY, _("Glyph Type:"), 
      wxDefaultPosition, wxSize( 65,-1 ), 0 ), 0, wxALL, 5 );

    wxComboBox* comboGlyphs = new wxComboBox( m_Gui, ID_GLYPH_TYPE, 
      wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
    comboGlyphs->Append(_("axes"));
	comboGlyphs->Append( _("ellipsoid") );
    comboGlyphs->Append( _("arrows") );
    comboGlyphs->SetToolTip( _("Specifies glyph type which should be used "
      "for the visualization of the selected tensor field.") );
    bSizer52->Add( comboGlyphs, 1, wxALL, 0 );
    bSizerMain->Add( bSizer52, 0, wxEXPAND, 5 );

    //and validator
    comboGlyphs->SetValidator(albaGUIValidator(this, ID_GLYPH_TYPE, comboGlyphs, &m_GlyphType));

	//-----------weih add-----------
#pragma region Glyph range
	//-------------------------first filter list----------
	wxStaticBoxSizer* sbSizer3 = new wxStaticBoxSizer( 
		new wxStaticBox( m_Gui, wxID_ANY, wxT("Range") ), wxVERTICAL );
	m_RangeCtrl = new wxListCtrl( m_Gui, ID_LIST_RANGES, wxDefaultPosition, 
		wxDefaultSize, wxLC_NO_SORT_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL );
	m_RangeCtrl->SetColumnWidth(0,200);//wxLIST_AUTOSIZE_USEHEADER
	m_RangeCtrl->SetColumnWidth(1,300);// useless

	sbSizer3->Add( m_RangeCtrl, 1, wxALL|wxEXPAND, 1 );
	//-------------------------buttons---------
	wxBoxSizer* bSizer6 = new wxBoxSizer( wxHORIZONTAL );  
	bSizer6->Add( new wxPanel( m_Gui, wxID_ANY, wxDefaultPosition, 
		wxDefaultSize, wxTAB_TRAVERSAL ), 1, wxALL, 5 );

	m_BttnAddItem = new wxButton(m_Gui,ID_ADDITEM,wxT("Add"),
		wxDefaultPosition,wxSize( 50,-1 ), 0);
	m_BttnAddItem->Enable(true);
	m_BttnAddItem->SetToolTip(wxT("Add an item."));
	m_BttnAddItem->SetValidator(albaGUIValidator(this, ID_ADDITEM, m_BttnAddItem));

	m_BttnRemoveItem = new wxButton( m_Gui, ID_REMOVEITEM, wxT("Remove"), 
		wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_BttnRemoveItem->Enable( false );
	m_BttnRemoveItem->SetToolTip( wxT("Removes selected item.") );
	m_BttnRemoveItem->SetValidator(albaGUIValidator(this, ID_REMOVEITEM, m_BttnRemoveItem));

	m_BttnShow = new wxButton( m_Gui, ID_SHOWITEM, wxT("Show"), 
		wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_BttnShow->Enable( false );
	m_BttnShow->SetToolTip( wxT("Show result.") );
	m_BttnShow->SetValidator(albaGUIValidator(this, ID_SHOWITEM, m_BttnShow));

	bSizer6->Add( m_BttnAddItem, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1 );
	bSizer6->Add( m_BttnRemoveItem, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1 );
	bSizer6->Add( m_BttnShow,0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

	sbSizer3->Add( bSizer6, 0, wxEXPAND, 1 );

	if (nScalars > 0)
	{
	
		//----------------------second filter list----------
		m_RangeCtrl2 = new wxListCtrl( m_Gui, ID_LIST_RANGES, wxDefaultPosition, 
			wxDefaultSize, wxLC_NO_SORT_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL );
		m_RangeCtrl2->SetColumnWidth(0,200);//wxLIST_AUTOSIZE_USEHEADER
		m_RangeCtrl2->SetColumnWidth(1,300);// useless

		sbSizer3->Add(new wxStaticText( m_Gui, wxID_ANY, _("scalar Filter:"), 
			wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );
		sbSizer3->Add( m_RangeCtrl2, 1, wxALL|wxEXPAND, 1 );
		//----------------------buttons----------
		wxBoxSizer* bSizer7 = new wxBoxSizer( wxHORIZONTAL );  
		bSizer7->Add( new wxPanel( m_Gui, wxID_ANY, wxDefaultPosition, 
			wxDefaultSize, wxTAB_TRAVERSAL ), 1, wxALL, 5 );

		m_BttnAddItem2 = new wxButton(m_Gui,ID_ADDITEM2,wxT("Add"),
			wxDefaultPosition,wxSize( 50,-1 ), 0);
		m_BttnAddItem2->Enable(true);
		m_BttnAddItem2->SetToolTip(wxT("Add an item."));
		m_BttnAddItem2->SetValidator(albaGUIValidator(this, ID_ADDITEM2, m_BttnAddItem2));

		m_BttnRemoveItem2 = new wxButton( m_Gui, ID_REMOVEITEM2, wxT("Remove"), 
			wxDefaultPosition, wxSize( 50,-1 ), 0 );
		m_BttnRemoveItem2->Enable( false );
		m_BttnRemoveItem2->SetToolTip( wxT("Removes selected item.") );
		m_BttnRemoveItem2->SetValidator(albaGUIValidator(this, ID_REMOVEITEM2, m_BttnRemoveItem));

		m_BttnShow2 = new wxButton( m_Gui, ID_SHOWITEM2, wxT("Show"), 
			wxDefaultPosition, wxSize( 50,-1 ), 0 );
		m_BttnShow2->Enable( false );
		m_BttnShow2->SetToolTip( wxT("Show result.") );
		m_BttnShow2->SetValidator(albaGUIValidator(this, ID_SHOWITEM2, m_BttnShow));

		bSizer7->Add( m_BttnAddItem2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1 );
		bSizer7->Add( m_BttnRemoveItem2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1 );
		bSizer7->Add( m_BttnShow2,0, wxALIGN_CENTER_VERTICAL|wxALL,1);

		sbSizer3->Add( bSizer7, 0, wxEXPAND, 1 );
		//-----------------------logic widgets-----------------
		wxBoxSizer* bSizer8= new wxBoxSizer( wxHORIZONTAL );  
		//bSizer8->Add( new wxPanel( m_Gui, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL ), 1, wxALL, 5 );
		wxArrayString radioStrings ;
		radioStrings.Add(wxT("AND")) ;
		radioStrings.Add(wxT("OR")) ;


		wxRadioBox* radioBox = new wxRadioBox(m_Gui, ID_CHOOSE_ANDOR, "interaction logic", wxDefaultPosition, wxSize(120,40), radioStrings, 1, wxRA_SPECIFY_ROWS) ;//wxEmptyString
		radioBox->SetValidator(albaGUIValidator(this, ID_CHOOSE_ANDOR, radioBox, &m_AndOr));

		m_BttnShowAssociate = new wxButton( m_Gui, ID_SHOWITEM, wxT("Show"), 
			wxDefaultPosition, wxSize(50,-1 ), 0 );
		m_BttnShowAssociate->Enable(false);
		m_BttnShowAssociate->SetToolTip(wxT("Show result from associated two filter."));
		m_BttnShowAssociate->SetValidator(albaGUIValidator(this, ID_SHOWITEM_ASSOCIATE, m_BttnShowAssociate));

		bSizer8->Add(radioBox,0,wxALIGN_LEFT|wxALL, 1);
		bSizer8->Add(m_BttnShowAssociate,0,wxALIGN_CENTER_VERTICAL|wxALL, 1 );
		sbSizer3->Add( bSizer8, 0, wxEXPAND, 1 );

	}
	//------------------------check box-------------
	/*	wxBoxSizer* bSizer7 = new wxBoxSizer( wxVERTICAL );  
	*/
	wxCheckBox* chckShowAll = new wxCheckBox( m_Gui, 
		ID_SHOW_ALL, _("Show all"), wxDefaultPosition, wxDefaultSize,0 );
	chckShowAll->SetToolTip( _("If checked, glyphs are showed by using original data.") );

	//and validator
	chckShowAll->SetValidator(albaGUIValidator(this, ID_SHOW_ALL,       
		chckShowAll, &m_ShowAll));
	//bSizer7->Add( chckShowAll, 0, wxALL|wxEXPAND, 5 );	
	sbSizer3->Add( chckShowAll, 0, wxALL|wxEXPAND, 5 );

	//----------------------------------------------	
	bSizerMain->Add( sbSizer3, 0, wxEXPAND, 1 );

#pragma endregion Glyph range
	InitFilterList(nScalars);

	//----------weih add end---------

#pragma region Glyph shape
    wxStaticBoxSizer* sbSizer2 = new wxStaticBoxSizer( 
      new wxStaticBox( m_Gui, wxID_ANY, _("Glyph Shape") ), wxVERTICAL );
    wxBoxSizer* bSizer5 = new wxBoxSizer( wxHORIZONTAL );
    bSizer5->Add( new wxStaticText( m_Gui, wxID_ANY, _("Radius:"), 
      wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

    wxTextCtrl* edRadius = new wxTextCtrl( m_Gui, ID_GLYPH_RADIUS);
    edRadius->SetToolTip( _("Specifies the unscaled size of glyph. ") );
    bSizer5->Add( edRadius, 1, wxALL, 1 );

    //and validator
    edRadius->SetValidator(albaGUIValidator(this, ID_GLYPH_RADIUS, edRadius, &m_GlyphRadius, 1e-8, 1000));

    bSizer5->Add( new wxStaticText( m_Gui, wxID_ANY, _("Res:"), 
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT ), 1, wxALL, 5 );

    wxTextCtrl* edRes = new wxTextCtrl( m_Gui, ID_GLYPH_RESOLUTION);
    edRes->SetToolTip( _("Specifies the resolution of glyphs, "
      "i.e., number of sides of cones, etc.") );
    bSizer5->Add( edRes, 1, wxALL, 1 );
    sbSizer2->Add( bSizer5, 0, wxEXPAND, 5 );

    //and validator
    edRes->SetValidator(albaGUIValidator(this, ID_GLYPH_RESOLUTION, edRes, &m_GlyphRes, 3, 100));

    //BES: 12.6.2009 - vtkTensorGlyph version 4.2 contains "Eigen Values Scaling"
    //but does not implement it => we won't use it
    //wxCheckBox* chckScale = new wxCheckBox( m_Gui, ID_GLYPH_SCALING, 
    //  _("Enable Eigen Values Scaling"), wxDefaultPosition, wxDefaultSize, 0 );
    //chckScale->SetToolTip( _("When checked, glyphs are scaled according "
    //  "to the eigen values of tensors.") );
    //sbSizer2->Add( chckScale, 0, wxALL, 5 );

    ////and validator
    //chckScale->SetValidator(albaGUIValidator(this, ID_GLYPH_SCALING, chckScale, &m_GlyphScaling));

    bSizerMain->Add( sbSizer2, 0, wxEXPAND, 5 );
#pragma endregion Glyph shape

#pragma region Glyph Colors
    wxStaticBoxSizer* sbSizer65 = new wxStaticBoxSizer( 
      new wxStaticBox( m_Gui, wxID_ANY, _("Glyph Color") ), wxVERTICAL );

    wxBoxSizer* bSizer4311 = new wxBoxSizer( wxHORIZONTAL );
    bSizer4311->Add( new wxStaticText( m_Gui, wxID_ANY, _("Material:"), 
      wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

    m_GlyphMaterial->GetMaterial()->MakeIcon();
    m_GlyphMaterialButton = new albaGUIPicButton(m_Gui, 
      m_GlyphMaterial->GetMaterial()->m_Icon, ID_GLYPH_MATERIAL, this);
    bSizer4311->Add( m_GlyphMaterialButton, 0, wxALL, 0 );

    m_GlyphMaterialLabel = new wxStaticText( m_Gui, ID_GLYPH_MATERIAL_LABEL, 
      m_GlyphMaterial->GetMaterial()->m_MaterialName.GetCStr(), wxDefaultPosition, wxDefaultSize, 0 );        
    bSizer4311->Add( m_GlyphMaterialLabel, 1, wxALL, 5 );
    sbSizer65->Add( bSizer4311, 0, wxEXPAND, 5 );

    wxCheckBox* chckShowScalarField1 = new wxCheckBox( m_Gui, 
      ID_USE_COLOR_MAPPING, _("Enable Color Mapping"), wxDefaultPosition, wxDefaultSize, 0 );
    chckShowScalarField1->SetToolTip( 
      _("If checked, glyphs are colored using a mapping of the specified LUT.") );
    sbSizer65->Add( chckShowScalarField1, 0, wxALL|wxEXPAND, 5 );

    //and validator
    chckShowScalarField1->SetValidator(albaGUIValidator(this, ID_USE_COLOR_MAPPING, 
      chckShowScalarField1, &m_UseColorMapping));
    

    wxStaticBoxSizer* sbSizer221 = new wxStaticBoxSizer( 
      new wxStaticBox( m_Gui, wxID_ANY, wxEmptyString ), wxVERTICAL );

    //scalar field widgets
    if (nScalars > 0)
    {      
      wxCheckBox* checkBox52 = new wxCheckBox( m_Gui, ID_USE_SF_MAPPING, 
         _("Scalar Field Mapping"), wxDefaultPosition, wxDefaultSize, 0 );    
      checkBox52->SetToolTip( _("If checked, values from the specified scalar "
        "field are used to map colors, otherwise eigen values are used.") );
      sbSizer221->Add( checkBox52, 0, wxALL, 5 );
      
      //and validator
      checkBox52->SetValidator(albaGUIValidator(this, ID_USE_SF_MAPPING,       
        checkBox52, &m_UseSFColorMapping));     
    }

    wxBoxSizer* bSizer491 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* bSizer432 = new wxBoxSizer( wxHORIZONTAL );    
    bSizer432->Add( new wxStaticText( m_Gui, wxID_ANY, _("LUT:")), 0, wxALL, 5 );

    albaGUILutSwatch* luts2 = new albaGUILutSwatch(m_Gui, ID_SF_MAPPING_LUT, 
      wxDefaultPosition, wxSize(140, 18), wxTAB_TRAVERSAL | wxSIMPLE_BORDER );
    luts2->SetLut(m_ColorMappingLUT);
    luts2->SetEditable(true);
    luts2->SetListener(this);
    bSizer432->Add(luts2, 0, 0/*wxEXPAND*/, 5 );
    bSizer491->Add( bSizer432, 1, wxEXPAND, 5 );

    wxCheckBox* chckShowSFLegend1 = new wxCheckBox( m_Gui, ID_SHOW_SF_MAPPING, 
      _("Show Mapping"), wxDefaultPosition, wxDefaultSize, 0 );
    chckShowSFLegend1->SetToolTip( _("If checked, mapping is displayed "
      "as a colored bar in the main view.") );
    bSizer491->Add( chckShowSFLegend1, 0, wxALL|wxEXPAND, 5 );

    //and validator
    chckShowSFLegend1->SetValidator(albaGUIValidator(this, ID_SHOW_SF_MAPPING,       
      chckShowSFLegend1, &m_ShowMap));

    sbSizer221->Add( bSizer491, 1, wxEXPAND, 5 );
    sbSizer65->Add( sbSizer221, 0, wxEXPAND, 5 );
    bSizerMain->Add( sbSizer65, 0, wxEXPAND, 0 );
#pragma endregion Glyph Colors
  }

  m_Gui->Add(bSizerMain);
  return m_Gui;
}

//----------------------------------------------------------------------------
// init items of this gui when load vme--[7/31/2009 weih]
void albaPipeTensorFieldGlyphs::InitFilterList(int nScalars){
	//----------------------------------------------------------------------------
	wxString cols[2] = { wxT("filter name T"), wxT("range value") };
	for (int i = 0; i < 2; i++){
		m_RangeCtrl->InsertColumn(i, cols[i]);
		if (nScalars > 0)
		{
			m_RangeCtrl2->InsertColumn(i,cols[i]);
		}
	}
	albaVME::albaLinksMap* pLinks =  m_Vme->GetLinks();
	wxString itemName,itemValue1,itemValue2,displayValue;
	double dValue1,dValue2;
	int idx1 =0,idx2=0;
	for (albaVME::albaLinksMap::iterator i = pLinks->begin(); i != pLinks->end(); i++)
	{
		albaString linkName = i->first;
		if (linkName.StartsWith(FILTER_LINK_NAME))
		{
			//------insert item----format--"filter-link0:aa:0.100:1.214"
			FILTER_ITEM* pItem = new FILTER_ITEM;
			memset(pItem, 0, sizeof(FILTER_ITEM));

			wxStringTokenizer tkz(linkName.GetCStr(), wxT(":"));
			int j=0;
			while ( tkz.HasMoreTokens() )
			{
				wxString token = tkz.GetNextToken();
				if (j==1)
				{
					itemName = token;
				}else if (j==2)
				{
					itemValue1 = token;
				}else if (j==3)
				{
					itemValue2 = token;
				}
				j++;
			}
			itemValue1.ToDouble(&dValue1);
			itemValue2.ToDouble(&dValue2);
			pItem->value[0] = dValue1;
			pItem->value[1] = dValue2;
			m_RangeCtrl->InsertItem(idx1,itemName);
			displayValue =  "["+itemValue1+","+itemValue2+"]";
			m_RangeCtrl->SetItem(idx1 ,1,displayValue);
			m_RangeCtrl->SetItemState(idx1 , wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED,
				wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
			m_RangeCtrl->EnsureVisible(idx1+1);
			m_RangeCtrl->SetItemData(idx1 , (long)pItem);
			idx1++;

		}else if (linkName.StartsWith(FILTER_LINK_NAME2) &&  nScalars>0)
		{
			//------insert item----format--"filter-link0:aa:0.100:1.214"
			FILTER_ITEM* pItem = new FILTER_ITEM;
			memset(pItem, 0, sizeof(FILTER_ITEM));

			wxStringTokenizer tkz(linkName.GetCStr(), wxT(":"));
			int j=0;
			while ( tkz.HasMoreTokens() )
			{
				wxString token = tkz.GetNextToken();
				if (j==1)
				{
					itemName = token;
				}else if (j==2)
				{
					itemValue1 = token;
				}else if (j==3)
				{
					itemValue2 = token;
				}
				j++;
			}
			itemValue1.ToDouble(&dValue1);
			itemValue2.ToDouble(&dValue2);
			pItem->value[0] = dValue1;
			pItem->value[1] = dValue2;
			m_RangeCtrl2->InsertItem(idx2,itemName);
			displayValue =  "["+itemValue1+","+itemValue2+"]";
			m_RangeCtrl2->SetItem(idx2 ,1,displayValue);
			m_RangeCtrl2->SetItemState(idx2 , wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED,
				wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
			m_RangeCtrl2->EnsureVisible(idx2 + 1);
			m_RangeCtrl2->SetItemData(idx2 , (long)pItem);
			idx2++;

		}
	}
	if (idx1>0)
	{
		m_BttnShow->Enable(true);
		m_BttnRemoveItem->Enable(true);
	}
	if (idx2>0)
	{
		m_BttnShow2->Enable(true);
		m_BttnRemoveItem->Enable(true);
	}
	if (idx1>0 && idx2>0)
	{
		m_BttnShowAssociate->Enable(true);
	}

}
//-----------------------------------------------------------------------
//Store Filter values into link 
void albaPipeTensorFieldGlyphs::StoreFilterLinks2(){
	//-----------------------------------------------------------------------

	albaVME::albaLinksMap* pLinks = m_Vme->GetLinks(); 
	//remove old filter of this link
	bool bNeedRestart;  
	do
	{
		bNeedRestart = false;
		for (albaVME::albaLinksMap::iterator i = pLinks->begin(); i != pLinks->end(); i++)
		{
			if (i->first.StartsWith(FILTER_LINK_NAME2))
			{
				m_Vme->RemoveLink(i->first);
				bNeedRestart = true;
				break;
			}
		}
	}while (bNeedRestart);

	//then add new links
	albaString szName;
	wxString itemName;
	int nCount = m_RangeCtrl2->GetItemCount();
	if (nCount>0)
	{
		for (int i=0;i<nCount;i++)
		{
			itemName = m_RangeCtrl2->GetItemText(i);
			FILTER_ITEM* pItem = (FILTER_ITEM*)m_RangeCtrl2->GetItemData(i);
			albaString szName;
			szName = albaString::Format("%s%d",FILTER_LINK_NAME2,i);
			szName += ":";
			szName += itemName;
			szName += ":";
			szName +=   albaString::Format("%.4f",pItem->value[0]);
			szName += ":";
			szName +=  albaString::Format("%.4f",pItem->value[1]);

			m_Vme->SetLink(szName,m_Vme);
		}
	}
}
//-----------------------------------------------------------------------
//Store Filter values into link 
void albaPipeTensorFieldGlyphs::StoreFilterLinks(){
	//-----------------------------------------------------------------------

	albaVME::albaLinksMap* pLinks = m_Vme->GetLinks(); 
	//remove old filter of this link
	bool bNeedRestart;  
	do
	{
		bNeedRestart = false;
		for (albaVME::albaLinksMap::iterator i = pLinks->begin(); i != pLinks->end(); i++)
		{
			if (i->first.StartsWith(FILTER_LINK_NAME))
			{
				m_Vme->RemoveLink(i->first);
				bNeedRestart = true;
				break;
			}
		}
	}while (bNeedRestart);

	//then add new links
	albaString szName;
	wxString itemName;
	int nCount = m_RangeCtrl->GetItemCount();
	if (nCount>0)
	{
		for (int i=0;i<nCount;i++)
		{
			itemName = m_RangeCtrl->GetItemText(i);
			FILTER_ITEM* pItem = (FILTER_ITEM*)m_RangeCtrl->GetItemData(i);
			albaString szName;
			szName = albaString::Format("%s%d",FILTER_LINK_NAME,i);
			szName += ":";
			szName += itemName;
			szName += ":";
			szName +=   albaString::Format("%.4f",pItem->value[0]);
			szName += ":";
			szName +=  albaString::Format("%.4f",pItem->value[1]);

			m_Vme->SetLink(szName,m_Vme);
		}
	}
}
//-----------------------------------------------------------------------
//use value of this item to filte data
/*void albaPipeTensorFieldGlyphs::OnShowFilter(int mode){
	//-----------------------------------------------------------------------
	int nIndex = m_RangeCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED); 
	if (nIndex >= 0)
	{
		FILTER_ITEM* pItem = (FILTER_ITEM*)m_RangeCtrl->GetItemData(nIndex);
		double valueRange[2];
		valueRange[0] = pItem->value[0] ;
		valueRange[1] = pItem->value[1];

		doFilter(valueRange);

		//valueRange = pItem->value;
		//m_RangeCtrl->DeleteItem(nIndex);
	}
}*/
//-----------------------------------------------------------------------
//use value of this item to filte data
void albaPipeTensorFieldGlyphs::OnShowFilter(int mode){
	//-----------------------------------------------------------------------
	int nIndex1 = -1;
	int nIndex2 = -1;
	nIndex1 = m_RangeCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED); 

	if (mode ==1 && nIndex1>=0)
	{
		FILTER_ITEM* pItem = (FILTER_ITEM*)m_RangeCtrl->GetItemData(nIndex1);
		double valueRange[2];
		valueRange[0] = pItem->value[0] ;
		valueRange[1] = pItem->value[1];
		DoFilter(mode,valueRange,NULL);
	}else if (mode ==2 )
	{
		nIndex2 = m_RangeCtrl2->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED); 
		if( nIndex2>=0){
			FILTER_ITEM* pItem = (FILTER_ITEM*)m_RangeCtrl2->GetItemData(nIndex2);
			double valueRange[2];
			valueRange[0] = pItem->value[0] ;
			valueRange[1] = pItem->value[1];
			DoFilter(mode,NULL,valueRange);		
		}
	}else if (mode ==3 && nIndex1>=0 )
	{
		nIndex2 = m_RangeCtrl2->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED); 
		if( nIndex2>=0){
			double valueRange1[2],valueRange2[2];
			FILTER_ITEM* pItem1 = (FILTER_ITEM*)m_RangeCtrl->GetItemData(nIndex1);
			valueRange1[0] = pItem1->value[0];
			valueRange1[1] = pItem1->value[1];

			FILTER_ITEM* pItem2 = (FILTER_ITEM*)m_RangeCtrl2->GetItemData(nIndex2);
			valueRange2[0] = pItem2->value[0];
			valueRange2[1] = pItem2->value[1];
			DoFilter(mode,valueRange1,valueRange2);	
		}
	}
}
//-----------------------------------------------------------------------
//Add a range item
void albaPipeTensorFieldGlyphs::OnAddItem(int idx){
	//-----------------------------------------------------------------------

	CreateAddItemDlg(idx);

}
//-----------------------------------------------------------------------
//Remove a range item
void albaPipeTensorFieldGlyphs::OnRemoveItem(){
	//--------------------------------------------------------------------
	int nIndex = m_RangeCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED); 
	if (nIndex >= 0)
	{
		m_RangeCtrl->DeleteItem(nIndex);
	}
}
//Remove a range item
void albaPipeTensorFieldGlyphs::OnRemoveItem2(){
	//--------------------------------------------------------------------
	int nIndex = m_RangeCtrl2->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED); 
	if (nIndex >= 0)
	{
		m_RangeCtrl2->DeleteItem(nIndex);
		if (m_RangeCtrl->GetItemCount()<1)
		{
			m_BttnShowAssociate->Enable(false);
		}
	}
}
/*---------------------------------------------*/
//insert item 
//----------------------------------------------
bool albaPipeTensorFieldGlyphs::AddItem2(){

	double dValue1,dValue2;
	bool rtn = false;
	m_FilterValue1.ToDouble(&dValue1);
	m_FilterValue2.ToDouble(&dValue2);

	if (fabs(dValue2 - m_Sr2[1]) < 0.00005)
	{
		dValue2 = m_Sr2[1];
	}
	if (fabs(dValue1- m_Sr2[0]) < 0.00005)
	{
		dValue1 = m_Sr2[0];
	}

	if (dValue1<=dValue2 && dValue1>=m_Sr2[0] && dValue2<=m_Sr2[1] )
	{
		FILTER_ITEM* pItem = new FILTER_ITEM;
		memset(pItem, 0, sizeof(FILTER_ITEM));
		int nListCount = m_RangeCtrl2->GetItemCount();
		wxString itemName = m_FilterName;
		m_RangeCtrl2->InsertItem(nListCount, itemName);
		wxString displayValue = "["+m_FilterValue1+","+m_FilterValue2+"]";
		m_RangeCtrl2->SetItem(nListCount,1,displayValue);
		m_RangeCtrl2->SetItemState(nListCount, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED,
			wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
		m_RangeCtrl2->EnsureVisible(nListCount);

		pItem->value[0] = dValue1;
		pItem->value[1] = dValue2;

		m_RangeCtrl2->SetItemData(nListCount, (long)pItem);
		rtn = true;
	}else{
		albaMessage("invalid value,please check");
		rtn = false;
	}
	return rtn;
}

/*---------------------------------------------*/
//insert item 
//----------------------------------------------
bool albaPipeTensorFieldGlyphs::AddItem(){

	double dValue1,dValue2;
	bool rtn = false;
	m_FilterValue1.ToDouble(&dValue1);
	m_FilterValue2.ToDouble(&dValue2);
	/*if (dValue2 > m_sr[1] && dValue2 -m_sr[1] < 0.00005)//1.2138546 will be ranged to 1.214 after wxString format 
	{
		dValue2 = m_sr[1];
	}
	if (dValue1< m_sr[0] && m_sr[0] - dValue1 < 0.00005)
	{
		dValue1 = m_sr[0];
	}*/
	if (fabs(dValue2 - m_Sr[1]) < 0.00005)
	{
		dValue2 = m_Sr[1];
	}
	if (fabs(dValue1- m_Sr[0]) < 0.00005)
	{
		dValue1 = m_Sr[0];
	}

	if (dValue1<=dValue2 && dValue1>=m_Sr[0] && dValue2<=m_Sr[1] )
	{
		FILTER_ITEM* pItem = new FILTER_ITEM;
		memset(pItem, 0, sizeof(FILTER_ITEM));
		int nListCount = m_RangeCtrl->GetItemCount();
		wxString itemName = m_FilterName;
		m_RangeCtrl->InsertItem(nListCount, itemName);
		wxString displayValue = "["+m_FilterValue1+","+m_FilterValue2+"]";
		m_RangeCtrl->SetItem(nListCount,1,displayValue);
		m_RangeCtrl->SetItemState(nListCount, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED,
			wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
		m_RangeCtrl->EnsureVisible(nListCount);

		pItem->value[0] = dValue1;
		pItem->value[1] = dValue2;

		m_RangeCtrl->SetItemData(nListCount, (long)pItem);
		rtn = true;
	}else{
		albaMessage("invalid value,please check");
		rtn = false;
	}
	return rtn;
}
//-----------------------------------------------------------------------
//create dialog ,same as vector
void albaPipeTensorFieldGlyphs::CreateAddItemDlg(int idx){
	//-----------------------------------------------------------------------
	/*vtkDataArray *dataArr = m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetScalars();
	double range[2];
	dataArr->GetRange(range);*/

	wxString dlgName = "";
	int okBtn =0;
	int canBtn = 0;
	int rName = 0;
	int rValue1 = 0;
	int rValue2 = 0;
	m_FilterName = "";

	if(idx==1){
		dlgName = "magnitude filter editor";
		m_FilterValue1 = albaString::Format("%.4f",m_Sr[0]);
		m_FilterValue2 = albaString::Format("%.4f",m_Sr[1]);
		okBtn = ID_ITEM_OK;
		canBtn = ID_ITEM_CANCEL;
		rName = ID_RANGE_NAME;
		rValue1 = ID_RANGE_VALUE1;
		rValue2 = ID_RANGE_VALUE2;

	}else if (idx==2)
	{
		dlgName = "scalar filter editor";
		m_FilterValue1 = albaString::Format("%.4f",m_Sr2[0]);
		m_FilterValue2 = albaString::Format("%.4f",m_Sr2[1]);
		okBtn = ID_ITEM_OK2;
		canBtn = ID_ITEM_CANCEL2;
		rName = ID_RANGE_NAME2;
		rValue1 = ID_RANGE_VALUE1_2;
		rValue2 = ID_RANGE_VALUE2_2;
	}

	m_AddItemDlg = new albaGUIDialog("filter editor",albaCLOSEWINDOW | albaRESIZABLE);
	// vertical stacker for the rows of widgets
	wxBoxSizer *vs1 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer *hs_1 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *hs_2 = new wxBoxSizer(wxHORIZONTAL);

	wxString label1 = "  name: ";
	wxString label2 = "  range: ";
	wxString label3 = "  <value<  ";

	//-----------------filter name------------------
	wxStaticText *textName  = new wxStaticText(m_AddItemDlg, wxID_ANY, label1, wxPoint(0,0), wxSize(50,20));
	wxTextCtrl   *textNameValue = new wxTextCtrl(m_AddItemDlg , rName, "ddd", wxPoint(0,0), wxSize(50,20), wxNO_BORDER  );
	textNameValue->SetValidator(albaGUIValidator(this, rName, textNameValue, &m_FilterName));

	//-----------------filter value1----------------

	wxStaticText *textValue1Name = new wxStaticText(m_AddItemDlg,wxID_ANY,label2,wxPoint(0,0),wxSize(50,20));
	wxTextCtrl   *textValue1Value = new wxTextCtrl(m_AddItemDlg,rValue1,"",wxPoint(0,0), wxSize(50,20),wxNO_BORDER);
	textValue1Value->SetValidator(albaGUIValidator(this, rValue1, textValue1Value, &m_FilterValue1));
	//-----------------filter value2----------------

	wxStaticText *textValue2Name = new wxStaticText(m_AddItemDlg,wxID_ANY,label3,wxPoint(0,0),wxSize(50,20));
	wxTextCtrl   *textValue2Value = new wxTextCtrl(m_AddItemDlg, rValue2,"",wxPoint(0,0), wxSize(50,20),wxNO_BORDER);
	textValue2Value->SetValidator(albaGUIValidator(this, rValue2, textValue2Value, &m_FilterValue2));
	//-----------------ok cancel button-------------

	wxBoxSizer *hs_b = new wxBoxSizer(wxHORIZONTAL);

	if (idx==1)
	{
		// ok/cancel buttons
		m_ItemOK = new albaGUIButton(m_AddItemDlg, okBtn, "OK", wxPoint(0,0), wxSize(50,20));
		m_ItemOK->SetListener(this);
		m_ItemOK->Enable(false);

		albaGUIButton *b_cancel = new albaGUIButton(m_AddItemDlg, canBtn, "CANCEL", wxPoint(0,0), wxSize(50,20));
		b_cancel->SetListener(this);

		hs_b->Add(m_ItemOK,0);
		hs_b->Add(b_cancel, 1, wxEXPAND);

	}else if (idx==2)
	{
		// ok/cancel buttons
		m_ItemOK2 = new albaGUIButton(m_AddItemDlg, okBtn, "OK", wxPoint(0,0), wxSize(50,20));
		m_ItemOK2->SetListener(this);
		m_ItemOK2->Enable(false);

		albaGUIButton *b_cancel2 = new albaGUIButton(m_AddItemDlg, canBtn, "CANCEL", wxPoint(0,0), wxSize(50,20));
		b_cancel2->SetListener(this);

		hs_b->Add(m_ItemOK2,0);
		hs_b->Add(b_cancel2, 1, wxEXPAND);
	}

	//-----------------------------------------------
	hs_1->Add(textName, 0);
	hs_1->Add(textNameValue, 1, wxEXPAND);

	hs_2->Add(textValue1Name,0);
	hs_2->Add(textValue1Value,1,wxEXPAND);
	hs_2->Add(textValue2Name,1,wxEXPAND);
	hs_2->Add(textValue2Value,1,wxEXPAND);

	vs1->Add(hs_1, 0, wxEXPAND | wxALL, 1);
	vs1->Add(hs_2, 0, wxEXPAND | wxALL, 1);
	vs1->Add(hs_b, 0, wxEXPAND | wxALL, 1);

	vs1->Fit(m_AddItemDlg);

	m_AddItemDlg->Add(vs1) ;  // plug the sizer into the dialog
	m_AddItemDlg->ShowModal();

	cppDEL(m_AddItemDlg);



}
bool albaPipeTensorFieldGlyphs::DoCondition(int mode,double tensorScaleValue,double scaleValue,double *rangeValue1,double *rangeValue2){
	bool rtn = false;
	bool tensorFlag;
	bool scaleFlag;
	if (mode ==1)
	{
		rtn = tensorScaleValue >= rangeValue1[0] && tensorScaleValue <= rangeValue1[1] ;

	}else if (mode ==2)
	{
		rtn = scaleValue >= rangeValue2[0] && scaleValue <= rangeValue2[1];
	}
	else if (mode ==3)
	{
		tensorFlag = tensorScaleValue >= rangeValue1[0] && tensorScaleValue <= rangeValue1[1] ;
		scaleFlag = scaleValue >= rangeValue2[0] && scaleValue <= rangeValue2[1];

		if (m_AndOr ==0)//and
		{
			rtn = tensorFlag && scaleFlag;

		}else if (m_AndOr == 1)//or
		{
			rtn = tensorFlag || scaleFlag;
		}
	}

	return rtn;

}
void albaPipeTensorFieldGlyphs::DoFilter(int mode ,double *rangeValue,double *rangeValue2){
	

	double origin[3],spacing[3];
	int dim[3];
	vtkIdType increment[3];
	m_Output = vtkPolyData::New();
	m_Output->Initialize();
	vtkPoints *points = vtkPoints::New() ;// Create points and attribute arrays (vector or tensor as requested by user)

	vtkDoubleArray *scalars ;
	vtkDoubleArray *vectors ;
	vtkDoubleArray *tensors ;

	scalars = vtkDoubleArray::New() ;
	scalars->SetNumberOfComponents(1) ;
	scalars->SetName("scalars") ;

	vectors = vtkDoubleArray::New() ;
	vectors->SetNumberOfComponents(3) ;
	vectors->SetName("vectors") ;

	tensors = vtkDoubleArray::New() ;
	tensors->SetNumberOfComponents(9) ;
	tensors->SetName("tensors") ;

	vtkDataSet *orgData = m_Vme->GetOutput()->GetVTKData();

	vtkPointData *allPoints = orgData->GetPointData();
	vtkDataArray *old_scalars = allPoints->GetScalars();
	vtkDataArray *old_vectors = allPoints->GetVectors();
	vtkDataArray *old_tensors = allPoints->GetTensors();

	vtkDataArray *pointScalarArray = allPoints->GetScalars();
	int nPoints = old_tensors->GetSize()/old_tensors->GetNumberOfComponents();
	int num=0 ;	

	double dMax=0,dMin=0;//test code


	int pointsNum = m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetNumberOfTuples();
	vtkIdType numScal = m_DataScale_Copy->GetSize();//vtkIdType num1 = da->GetNumberOfTuples();
	int step = numScal /pointsNum;
	int idxPoints = 0;
	int idxScales = 0;

	double tensorScale;
	double pointScale = 0.0;

	double *pCoord;
	double *aTuple;

	int nScalars = GetNumberOfScalars();

	//if (nScalars > 1){
		if(orgData->IsA("vtkRectilinearGrid")){
			//vtkImageData* pImgData = GetImageData(orgDataR);

			albaString logFname2 = "coordFile.txt";//if debug
			std::ofstream outputFile2(logFname2, std::ios::out|std::ios::app);//if debug
			outputFile2.clear();//if debug
			outputFile2<<"---------------begin--------------------------------"<<std::endl;//if debug
			int dim[3];

			vtkRectilinearGrid *orgDataR = vtkRectilinearGrid::SafeDownCast(orgData);
			/*vtkFloatArray *xCoord = vtkFloatArray::SafeDownCast(orgDataR->GetXCoordinates());//@todo
			vtkFloatArray *yCoord = vtkFloatArray::SafeDownCast(orgDataR->GetYCoordinates());
			vtkFloatArray *zCoord = vtkFloatArray::SafeDownCast(orgDataR->GetZCoordinates());*/
			vtkDataArray *xCoord = orgDataR->GetXCoordinates();
			vtkDataArray *yCoord = orgDataR->GetYCoordinates();
			vtkDataArray *zCoord = orgDataR->GetZCoordinates();
			//-----------test code ------------------
			orgDataR->GetDimensions(dim);
			int numberOfTuple = old_scalars->GetNumberOfTuples();//@todo			
			if (m_DataScale_Copy != NULL)
			{

				if (nPoints>0)
				{
					for (int iz=0;iz<dim[2];iz++)
					{
						for (int iy=0;iy<dim[1];iy++)
						{
							for (int ix=0;ix<dim[0];ix++)
							{
								idxPoints = ix+iy*dim[0]+iz*dim[0]*dim[1];//position in whole image
								idxScales = idxPoints * step;
								aTuple = m_DataScale_Copy->GetTuple(idxScales);
								tensorScale = aTuple[0];
								if (pointScalarArray)
								{
									pointScale = pointScalarArray->GetTuple1(idxPoints);
								}
								//if (scaleValue>=rangeValue[0] && scaleValue<=rangeValue[1])//in range
								if (DoCondition(mode,tensorScale,pointScale,rangeValue,rangeValue2))//DoCondition(int mode,double tensorScaleValue,double scaleValue,double *rangeValue1,double *rangeValue2)
								{
									double coord[3];

									coord[0] = xCoord->GetTuple1(ix);
									coord[1] = yCoord->GetTuple1(iy);
									coord[2] = zCoord->GetTuple1(iz);
									
									points->InsertNextPoint(coord);
									vectors->InsertNextTuple(old_vectors->GetTuple3(idxPoints));																		
									scalars->InsertNextTuple(&pointScale);
									tensors->InsertNextTuple(old_tensors->GetTuple9(idxPoints));
									num++;
								}

							}
						}
					}
				}
			}


		}else if (orgData->IsA("vtkImageData") || orgData->IsA("vtkImageData"))
		{
			vtkImageData *orgDataS =vtkImageData::SafeDownCast(m_Vme->GetOutput()->GetVTKData()) ;
			orgDataS->GetOrigin(origin) ;
			orgDataS->GetSpacing(spacing) ;
			orgDataS->GetDimensions(dim);
			orgDataS->GetIncrements(increment);

			
			//vtkDataArray* daScal = Tmp_Glyphs->GetOutput()->GetPointData()->GetScalars();
			double sr[2];

			if (m_DataScale_Copy != NULL)
			{
				m_DataScale_Copy->GetRange(sr);

				
				if (pointsNum>0)
				{

					for (int iz=0;iz<dim[2];iz++)
					{
						for (int iy=0;iy<dim[1];iy++)
						{
							for (int ix=0;ix<dim[0];ix++)
							{
								idxPoints = ix+iy*dim[0]+iz*dim[0]*dim[1];//position in whole image
								idxScales = idxPoints * step;

								aTuple = m_DataScale_Copy->GetTuple(idxScales);
								tensorScale = aTuple[0];
								if (pointScalarArray)
								{
									pointScale = pointScalarArray->GetTuple1(idxPoints);
								}
								if (DoCondition(mode,tensorScale,pointScale,rangeValue,rangeValue2))//in range
								{
									pCoord = allPoints->GetScalars()->GetTuple(idxPoints);

									pCoord[0] = origin[0] + ix * spacing[0];
									pCoord[1] = origin[1] + iy * spacing[1];
									pCoord[2] = origin[2] + iz * spacing[2];

									points->InsertNextPoint(pCoord);
									scalars->InsertNextTuple((float*)&pointScale);

									vectors->InsertNextTuple(old_vectors->GetTuple3(idxPoints));
									tensors->InsertNextTuple(old_tensors->GetTuple9(idxPoints));

									//outputFile2<< "  Scale="<<scaleValue<<" idxPoints="<<idxPoints<<"   idxScales"<<idxScales<<std::endl;//if debug
								}		
							}
						}
					}
				}
			}

		}
		//


		//albaString logFname2 = "coordFile.txt";//if debug
		//std::ofstream outputFile2(logFname2, std::ios::out|std::ios::app);//if debug
		//outputFile2.clear();//if debug
		//outputFile2<<"-----------------------------------------------"<<std::endl;//if debug

		

		scalars->Squeeze();
		vectors->Squeeze();
		tensors->Squeeze();

		m_Output->SetPoints(points) ;
		m_Output->GetPointData()->SetScalars(scalars) ;
		m_Output->GetPointData()->SetVectors(vectors) ;
		m_Output->GetPointData()->SetTensors(tensors);
			
		m_Glyphs->SetInputData(m_Output);
		m_Glyphs->Update();
		//outputFile2<< "  sr[0]="<<sr[0]<<"  sr[1]="<<sr[1]<<std::endl;//if debug
		//outputFile2.close();//if debug
	//}//if nscale


}
//--------------------------------------------------------------------------
//filter begin
//--------------------------------------------------------------------------
/*void albaPipeTensorFieldGlyphs::doFilter(double *rangeValue){


	double dMin = rangeValue[0];
	double dMax = rangeValue[1];

	m_Output = vtkPolyData::New();
	m_Output->Initialize();
	vtkPoints *points = vtkPoints::New() ;// Create points and attribute arrays (vector or tensor as requested by user)

	vtkFloatArray *scalars ;
	vtkFloatArray *vectors ;
	vtkFloatArray *tensors ;

	scalars = vtkFloatArray::New() ;
	scalars->SetNumberOfComponents(1) ;
	scalars->SetName("scalars") ;

	vectors = vtkFloatArray::New() ;
	vectors->SetNumberOfComponents(3) ;
	vectors->SetName("vectors") ;

	tensors = vtkFloatArray::New() ;
	tensors->SetNumberOfComponents(9) ;
	tensors->SetName("tensors") ;

	vtkImageData *orgData =vtkImageData::SafeDownCast(m_Vme->GetOutput()->GetVTKData()) ;

	vtkPointData *allPoints = orgData->GetPointData();
	vtkDataArray *old_scalars = allPoints->GetScalars();
	vtkDataArray *old_vectors = allPoints->GetVectors();
	vtkDataArray *old_tensors = allPoints->GetTensors();

	vtkDataArray* da = m_Glyphs->GetOutput()->GetPointData()->GetScalars();
	double sr[2];
	if (da != NULL)
	{
		da->GetRange(sr);
	}

	vtkDataArray *allArray = allPoints->GetScalars();

	int nPoints = old_tensors->GetNumberOfTuples();
	double origin[3],spacing[3];
	int dim[3],increment[3];
	orgData->GetOrigin(origin) ;
	orgData->GetSpacing(spacing) ;
	orgData->GetDimensions(dim);
	orgData->GetIncrements(increment);
	double scaleValue;
	double *pCoord;
	double *tmpCoord;
	albaString logFname2 = "coordFile.txt";
	std::ofstream outputFile2(logFname2, std::ios::out|std::ios::app);
	outputFile2.clear();
	outputFile2<<"-----------------------------------------------"<<std::endl;

	int idx,num=0 ;
	if (nPoints>0)
	{
		for (int iz=0;iz<dim[2];iz++)
		{
			for (int iy=0;iy<dim[1];iy++)
			{
				for (int ix=0;ix<dim[0];ix++)
				{
					idx = ix+iy*dim[0]+iz*dim[0]*dim[1];//position in whole image
					//double tmpScale = old_scalars->GetTuple1(idx);
					pCoord = allPoints->GetTuple(idx);
					//double tmpScale =  sqrt(pCoord[0]*pCoord[0]+pCoord[1]*pCoord[1]+pCoord[2]*pCoord[2]);	 
					double *vet = old_vectors->GetTuple3(idx);
					double tmpScale =  sqrt(vet[0]*vet[0]+vet[1]*vet[1]+vet[2]*vet[2]);

					if (tmpScale>=dMin && tmpScale<=dMax)
				 {
					 pCoord[0] = origin[0] + pCoord[0] + ix * spacing[0];
					 pCoord[1] = origin[1] + pCoord[1] + iy * spacing[1];
					 pCoord[2] = origin[2] + pCoord[2] + iz * spacing[2];

					 points->InsertNextPoint(pCoord);
					 scalars->InsertNextTuple((float*)&tmpScale);

					 outputFile2<< "  tmpScale="<<tmpScale<<"  x:"<<pCoord[0]<<"   y:"<<pCoord[1]<<"  z:"<<pCoord[2]<<std::endl;

					 vectors->InsertNextTuple(vet);
					 tensors->InsertNextTuple(old_tensors->GetTuple9(idx));
					 num++;
				 }
				}
			}
		}

	}
	scalars->Squeeze();
	vectors->Squeeze();
	tensors->Squeeze();

	outputFile2.close();

	m_Output->SetPoints(points) ;
	m_Output->GetPointData()->SetScalars(scalars) ;
	m_Output->GetPointData()->SetVectors(vectors) ;
	m_Output->GetPointData()->SetTensors(tensors);
	//m_Output->GetPointData()->SetActiveScalars("scalars1");
	m_Output->Update();

	m_Glyphs->SetInput(m_Output);

}
*/
//----------------------------------------------------------------------------
void albaPipeTensorFieldGlyphs::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{			
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {	
    if (e->GetId() >= Superclass::ID_LAST && e->GetId() < ID_LAST)
    {
		if (e->GetId() == ID_GLYPH_MATERIAL){
			OnChangeMaterial();
		}
		else if (e->GetId() == ID_ADDITEM)
		{
			OnAddItem(1);
		}else if (e->GetId()== ID_ADDITEM2)
		{
			OnAddItem(2);
		}else if (e->GetId() == ID_SHOWITEM)//show result of this filter
		{
			m_ShowAll = 0;
			OnShowFilter(1);
			m_Gui->Update();
		}else if(e->GetId() == ID_SHOWITEM2){
			m_ShowAll = 0;
			OnShowFilter(2);
			m_Gui->Update();
		}else if (e->GetId() == ID_SHOWITEM_ASSOCIATE)
		{
			m_ShowAll = 0;
			OnShowFilter(3);
			m_Gui->Update();
		}
		else if (e->GetId() == ID_REMOVEITEM)//remove an Item
		{
			OnRemoveItem();
			if ( m_RangeCtrl2->GetItemCount()<1 || m_RangeCtrl->GetItemCount()<1)
			{
				m_BttnShowAssociate->Enable(false);
			}
			StoreFilterLinks();
		}else if(e->GetId() == ID_REMOVEITEM2)
		{
			OnRemoveItem2();
			if ( m_RangeCtrl2->GetItemCount()<1 || m_RangeCtrl->GetItemCount()<1)
			{
				m_BttnShowAssociate->Enable(false);
			}
			StoreFilterLinks2();
		}
		else if (e->GetId()==ID_RANGE_NAME)
		{
			m_ItemOK->Enable(true);
		}
		else if (e->GetId() == ID_RANGE_NAME2)
		{
			m_ItemOK2->Enable(true);
		}
		else if (e->GetId()==ID_ITEM_OK)//check value
		{
			if (AddItem())//if values are valid ,an item inserted
			{
				m_AddItemDlg->EndModal(wxID_OK);
				m_BttnShow->Enable(true);
				m_BttnRemoveItem->Enable(true);
				if ( GetNumberOfScalars()>0 && m_RangeCtrl2->GetItemCount()>0)
				{
					m_BttnShowAssociate->Enable(true);
				}
				m_Gui->Update();
				StoreFilterLinks();
			}	
		}else if (e->GetId()==ID_ITEM_OK2)//check value
		{
			if (AddItem2())//if values are valid ,an item inserted
			{
				m_AddItemDlg->EndModal(wxID_OK);
				m_BttnShow2->Enable(true);
				m_BttnRemoveItem2->Enable(true);

				if ( m_RangeCtrl->GetItemCount()>0)
				{
					m_BttnShowAssociate->Enable(true);
				}

				m_Gui->Update();
				StoreFilterLinks2();
			}	
		}
		else if (e->GetId()==ID_ITEM_CANCEL)//close window
		{
			m_AddItemDlg->EndModal(wxID_CANCEL);
		}else if (e->GetId()==ID_ITEM_CANCEL2)
		{
			m_AddItemDlg->EndModal(wxID_CANCEL);
		}
		else if (e->GetId()==ID_SHOW_ALL)
		{
			if (m_ShowAll)
			{
				m_Glyphs->SetInputData(m_Vme->GetOutput()->GetVTKData());
			}
		}else if (e->GetId()==ID_CHOOSE_ANDOR)
		{

		}

        
      UpdateVTKPipe(); 
    
			GetLogicManager()->CameraUpdate();
      return;
    }     
  }

  //forward it to our listener to deal with it
  albaEventMacro(*alba_event);
}

//------------------------------------------------------------------------
// Handles change of material.
/*virtual*/ void albaPipeTensorFieldGlyphs::OnChangeMaterial()
//------------------------------------------------------------------------
{    
  albaGUIMaterialChooser dlg;
  if (dlg.ShowChooserDialog(m_GlyphMaterial))
  {
    //set new material label
    mmaMaterial* mat = m_GlyphMaterial->GetMaterial();     
    m_GlyphMaterialLabel->SetLabel(mat->m_MaterialName.GetCStr());

    //and set a new material icon
    cppDEL(mat->m_Icon); mat->MakeIcon();
    m_GlyphMaterialButton->SetBitmapLabel(*mat->m_Icon);
    m_GlyphMaterialButton->Refresh();

    m_Gui->Update();
  }
}

//------------------------------------------------------------------------
//Constructs VTK pipeline.
/*virtual*/ void albaPipeTensorFieldGlyphs::CreateVTKPipe()
//------------------------------------------------------------------------
{
  //build materials  
  albaNEW(m_GlyphMaterial);

  //build LUT
  m_ColorMappingLUT = vtkLookupTable::New();
  lutPreset(12, m_ColorMappingLUT); //initialize LUT to SAR (it has index 12)  
  m_ColorMappingLUT->Build();
  
  m_GlyphAxes = vtkAxes::New();
  m_GlyphEllipsoid = vtkSphereSource::New(); 
  m_GlyphArrow = vtkArrowSource::New(); 
   
  m_GlyphArrow->SetTipLength(0.5);     

  m_Glyphs = vtkTensorGlyph::New();
  m_Glyphs->SetInputData(m_Vme->GetOutput()->GetVTKData());  
  m_Glyphs->SetScaleFactor(1.0);
  m_Glyphs->ClampScalingOff();
  m_Glyphs->SymmetricOff();  
 

  m_GlyphsMapper = vtkPolyDataMapper::New();
  m_GlyphsMapper->SetInputConnection(m_Glyphs->GetOutputPort());
  m_GlyphsMapper->SetScalarModeToUsePointData();
  m_GlyphsMapper->SetColorModeToMapScalars();
  m_GlyphsMapper->SetLookupTable(m_ColorMappingLUT);

  m_GlyphsActor = vtkActor::New();
  m_GlyphsActor->SetMapper(m_GlyphsMapper);
  m_GlyphsActor->SetPickable(0);   //make it faster

  //scalar field map
  m_SFActor = vtkScalarBarActor::New();
  m_SFActor->SetLookupTable(m_GlyphsMapper->GetLookupTable());
  ((vtkActor2D*)m_SFActor)->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  ((vtkActor2D*)m_SFActor)->GetPositionCoordinate()->SetValue(0.1,0.01);
  m_SFActor->SetOrientationToHorizontal();
  m_SFActor->SetWidth(0.8);
  m_SFActor->SetHeight(0.12);  
  m_SFActor->SetLabelFormat("%6.3g");
  m_SFActor->SetPickable(0);   //make it faster

  m_RenFront->AddActor2D(m_SFActor);
  m_AssemblyFront->AddPart(m_GlyphsActor); 
}

//------------------------------------------------------------------------
//Updates VTK pipeline (setting radius, etc.). 
/*virtual*/ void albaPipeTensorFieldGlyphs::UpdateVTKPipe()
//------------------------------------------------------------------------
{
	albaPipeTensorFieldGlyphs::count ++; 
	m_Vme->GetOutput()->GetVTKData()->GetPointData()->
    SetActiveTensors(GetTensorFieldName(m_TensorFieldIndex));

  m_Vme->GetOutput()->GetVTKData()->GetPointData()->
    SetActiveScalars(GetScalarFieldName(m_ScalarFieldIndex));

  if (m_GlyphType == GLYPH_AXES)
  {
	  //m_GlyphAxes
	  m_Glyphs->SetSourceConnection(m_GlyphAxes->GetOutputPort());
	  m_Glyphs->ThreeGlyphsOff();
  }
  else if (m_GlyphType == GLYPH_ELLIPSOID)
  {
//    m_GlyphEllipsoid->SetRadius(m_GlyphRadius);  
    m_GlyphEllipsoid->SetPhiResolution(m_GlyphRes);
    m_GlyphEllipsoid->SetThetaResolution(m_GlyphRes);

    m_Glyphs->SetSourceConnection(m_GlyphEllipsoid->GetOutputPort());
    m_Glyphs->ThreeGlyphsOff();
	 
  }  
  else
  {
    m_GlyphArrow->SetShaftRadius(0.3 * m_GlyphRadius);    
    m_GlyphArrow->SetShaftResolution(m_GlyphRes);

    m_GlyphArrow->SetTipRadius(m_GlyphRadius);    
    m_GlyphArrow->SetTipResolution(m_GlyphRes);
    
    m_Glyphs->SetSourceConnection(m_GlyphArrow->GetOutputPort());
    m_Glyphs->ThreeGlyphsOn();
  }
  
  m_Glyphs->SetScaleFactor(m_GlyphRadius);
  m_Glyphs->SetScaling(m_GlyphScaling); 
  
  if (m_UseSFColorMapping == 0)
    m_Glyphs->SetColorModeToEigenvalues();
  else
    m_Glyphs->SetColorModeToScalars();
   
 
  m_Glyphs->Update();

  vtkDataArray* da = m_Glyphs->GetOutput()->GetPointData()->GetScalars();
  vtkDataArray* daScalar = m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetScalars();

  //-------------test code----------------
 /*vtkDataArray* testTensorArray = m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetTensors();
 int tensorSize = testTensorArray->GetSize();
 int comp = testTensorArray->GetNumberOfComponents();
 int nPoints = tensorSize/comp;
 albaString logFname2 = "testTensorFile.txt";
 std::ofstream outputFile2(logFname2, std::ios::out|std::ios::app);
 outputFile2.clear();
 outputFile2<<"-----------------------------------------------"<<std::endl;
 for (int k=0;k<nPoints;k++)
 {
	 outputFile2<<"tensor:"<<k<<std::endl;
	 for (int i=0;i<9;i++)
	 {
		 outputFile2<<testTensorArray->GetTuple9(k)[i];
	 }
	 outputFile2<<std::endl;
 }
 outputFile2.close();
 */
  //-------------test over----------------
  
  if (albaPipeTensorFieldGlyphs::count==1)
  {
	  m_DataScale_Copy = vtkFloatArray::New();
	  m_DataScale_Copy->DeepCopy(da);
	  m_DataScale_Copy->GetRange(m_Sr);

  }
  if (daScalar)
  {
	  daScalar->GetRange(m_Sr2);
  }
  if (m_UseColorMapping != 0)
  {

    if (da != NULL)
    {
      double sr[2];
      da->GetRange(sr);

      m_ColorMappingLUT->SetTableRange(sr);
      m_GlyphsMapper->SetScalarRange(sr);        
    }
  }

  m_GlyphsMapper->SetScalarVisibility(m_UseColorMapping); 
  m_GlyphsMapper->Update();

  m_GlyphsActor->SetProperty(m_GlyphMaterial->GetMaterial()->m_Prop);
  m_GlyphsActor->Modified();

  m_SFActor->SetVisibility(m_ShowMap != 0 && m_UseColorMapping != 0);  
}