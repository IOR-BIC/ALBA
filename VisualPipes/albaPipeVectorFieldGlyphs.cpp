/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: albaPipeVectorFieldGlyphs.cpp,v $ 
  Language: C++ 
  Date: $Date: 2011-11-15 09:23:14 $ 
  Version: $Revision: 1.1.2.9 $ 
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
#include "albaPipeVectorFieldGlyphs.h"

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
#include "vtkLookupTable.h"
#include "vtkGlyph3D.h"
//#include "vtkHedgeHog.h"
#include "vtkLineSource.h"
#include "vtkConeSource.h"
#include "vtkArrowSource.h"

#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkScalarBarActor.h"
#include "vtkRenderer.h"
#include "vtkImageData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkDoubleArray.h"
#include "albaGUIDialog.h"
#include "albaGUIValidator.h"
#include "albaGUIButton.h"
#include "vtkRectilinearGrid.h"
#include "vtkImageData.h"
#include "wx/busyinfo.h"
#include <wx/tokenzr.h>
#include <wx/slider.h>
#include <wx/defs.h>
#include <float.h>


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeVectorFieldGlyphs);
//----------------------------------------------------------------------------
const  char* albaPipeVectorFieldGlyphs::FILTER_LINK_NAME = "filter-link-vector";
const  char* albaPipeVectorFieldGlyphs::FILTER_LINK_NAME2 = "filter-link-scale";
int albaPipeVectorFieldGlyphs::count;
//----------------------------------------------------------------------------
albaPipeVectorFieldGlyphs::albaPipeVectorFieldGlyphs() : albaPipeVectorField()
//----------------------------------------------------------------------------
{
  //default constructor
  m_GlyphType = GLYPH_LINES;
  m_GlyphRadius = 0.1;
  m_GlyphLength = 1.0;
  m_GlyphRes = 6;
  
  m_GlyphScaling = SCALING_MAGNITUDE;  
  
  m_GlyphMaterial = NULL;
  m_ColorMappingLUT = NULL;

  m_UseColorMapping = 1;
  m_UseSFColorMapping = 0;  
  m_ShowMap = 1;  
  
  m_GlyphLine = NULL;
  m_GlyphCone = NULL;
  m_GlyphArrow = NULL;

  m_Glyphs = NULL;
  m_GlyphsMapper = NULL;
  m_GlyphsActor = NULL;    
  
  m_SFActor = NULL; 
  m_ShowAll = 1;//that means do not use filter
  m_AndOr = 0;
  albaPipeVectorFieldGlyphs::count = 0;
}

//----------------------------------------------------------------------------
albaPipeVectorFieldGlyphs::~albaPipeVectorFieldGlyphs()
//----------------------------------------------------------------------------
{ 
  // default destructor
  m_AssemblyFront->RemovePart(m_GlyphsActor);  
  m_RenFront->RemoveActor2D(m_SFActor);  
  
  // destroy objects
  vtkDEL(m_SFActor);
  
  vtkDEL(m_GlyphsActor);
  vtkDEL(m_GlyphsMapper);
  vtkDEL(m_Glyphs);
  vtkDEL(m_GlyphArrow);
  vtkDEL(m_GlyphCone);
  vtkDEL(m_GlyphLine);
  
  vtkDEL(m_ColorMappingLUT);
  albaDEL(m_GlyphMaterial);    
}


//----------------------------------------------------------------------------
/*virtual*/ void albaPipeVectorFieldGlyphs::ComputeDefaultParameters()
//----------------------------------------------------------------------------
{
  //Default radius, etc. should be calculated in this method, 
  //i.e., inherited classes should always override this method. 
  //The default implementation is to update VME
  Superclass::ComputeDefaultParameters();

  double dblDiag = m_Vme->GetOutput()->GetVTKData()->GetLength();
  m_GlyphRadius = dblDiag / 80;
}

//----------------------------------------------------------------------------
albaGUI *albaPipeVectorFieldGlyphs::CreateGui()
//----------------------------------------------------------------------------
{
  // Create the operation GUI
  int nVectors = GetNumberOfVectors();
  int nScalars = GetNumberOfScalars();

  m_Gui = new albaGUI(this);
     
  wxBoxSizer* bSizerMain = new wxBoxSizer( wxVERTICAL );
  if (nVectors == 0)  //no vector field available, this pipe does not work
    bSizerMain->Add(new wxStaticText( m_Gui, wxID_ANY, _("No vector field to visualize.")));
  else
  {
    if (nVectors > 1)      
    {
      wxBoxSizer* bSizer1 = new wxBoxSizer( wxHORIZONTAL );
      bSizer1->Add( new wxStaticText( m_Gui, wxID_ANY, _("Vector Field:"), 
        wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

      wxComboBox* comboField = new wxComboBox( m_Gui, ID_VECTORFIELD, 
        wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
      comboField->SetToolTip(_("Selects the vector field to be visualized.") );
      PopulateCombo(comboField, true);  //at least one vector is available
      bSizer1->Add( comboField, 1, wxALL, 1 );
      bSizerMain->Add( bSizer1, 0, wxEXPAND, 0 );

      //and validator
      comboField->SetValidator(albaGUIValidator(this, ID_VECTORFIELD, comboField, &m_VectorFieldIndex));
    }

    if (nScalars > 1)
    {
      wxBoxSizer* bSizer51111 = new wxBoxSizer( wxHORIZONTAL );  
      bSizer51111->Add( new wxStaticText( m_Gui, wxID_ANY, _("Scalar Field:"), 
        wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

      wxComboBox* comboScalars = new wxComboBox( m_Gui, ID_SCALARFIELD, 
        wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
      comboScalars->SetToolTip( 
        _("Selects the scalar field to be used for scaling and coloring of glyphs.") );
      PopulateCombo(comboScalars, false);
      bSizer51111->Add( comboScalars, 1, wxALL, 1 );
      bSizerMain->Add( bSizer51111, 0, wxEXPAND, 5 );

      //and validator
      comboScalars->SetValidator(albaGUIValidator(this, ID_SCALARFIELD, comboScalars, &m_ScalarFieldIndex));
    }

    wxBoxSizer* bSizer52 = new wxBoxSizer( wxHORIZONTAL );
    bSizer52->Add( new wxStaticText( m_Gui, wxID_ANY, _("Glyph Type:"), 
      wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

    wxComboBox* comboGlyphs = new wxComboBox( m_Gui, ID_GLYPH_TYPE, 
      wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
    comboGlyphs->Append( _("lines") );
    comboGlyphs->Append( _("cones") );
    comboGlyphs->Append( _("arrows") );
    comboGlyphs->SetToolTip( _("Specifies glyph type which should be used "
      "for the visualization of the selected vector field.") );
    bSizer52->Add( comboGlyphs, 1, wxALL, 0 );
    bSizerMain->Add( bSizer52, 0, wxEXPAND, 5 );

    //and validator
    comboGlyphs->SetValidator(albaGUIValidator(this, ID_GLYPH_TYPE, comboGlyphs, &m_GlyphType));
//weih add----------------------
#pragma region Glyph range
//----------------------first filter list----------
	wxStaticBoxSizer* sbSizer3 = new wxStaticBoxSizer( 
		new wxStaticBox( m_Gui, wxID_ANY, wxT("Range") ), wxVERTICAL );
	m_RangeCtrl = new wxListCtrl( m_Gui, ID_LIST_RANGES, wxDefaultPosition, 
		wxDefaultSize, wxLC_NO_SORT_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL );
	m_RangeCtrl->SetColumnWidth(0,200);//wxLIST_AUTOSIZE_USEHEADER
	m_RangeCtrl->SetColumnWidth(1,300);// useless
	
	sbSizer3->Add(new wxStaticText( m_Gui, wxID_ANY, _("magnitude value:"), 
		wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );
	sbSizer3->Add( m_RangeCtrl, 1, wxALL|wxEXPAND, 1 );
//buttons----------------------------------
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
		//second filter list--------------------------------
		m_RangeCtrl2 = new wxListCtrl( m_Gui, ID_LIST_RANGES, wxDefaultPosition, 
			wxDefaultSize, wxLC_NO_SORT_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL );
		m_RangeCtrl2->SetColumnWidth(0,200);//wxLIST_AUTOSIZE_USEHEADER
		m_RangeCtrl2->SetColumnWidth(1,300);// useless

		sbSizer3->Add(new wxStaticText( m_Gui, wxID_ANY, _("scalar Filter:"), 
			wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );
		sbSizer3->Add( m_RangeCtrl2, 1, wxALL|wxEXPAND, 1 );
		//buttons--------------------------------
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

		//logic widgets----------------------------------------
		wxBoxSizer* bSizer8= new wxBoxSizer( wxHORIZONTAL );  
		bSizer8->Add( new wxPanel( m_Gui, wxID_ANY, wxDefaultPosition, 
			wxDefaultSize, wxTAB_TRAVERSAL ), 1, wxALL, 5 );
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
		bSizer8->Add(m_BttnShowAssociate,0, wxALIGN_CENTER_VERTICAL|wxALL, 1 );
		sbSizer3->Add( bSizer8, 0, wxEXPAND, 1 );
	}
//check box-------------------------------------
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

//weih add end-------------------

#pragma region Glyph shape
    wxStaticBoxSizer* sbSizer2 = new wxStaticBoxSizer( 
      new wxStaticBox( m_Gui, wxID_ANY, _("Glyph shape") ), wxVERTICAL );
    wxBoxSizer* bSizer5 = new wxBoxSizer( wxHORIZONTAL );
    bSizer5->Add( new wxStaticText( m_Gui, wxID_ANY, _("Radius:"), 
      wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

    wxTextCtrl* edRadius = new wxTextCtrl( m_Gui, ID_GLYPH_RADIUS);
    edRadius->SetToolTip( _("Specifies the radius of glyph, i.e., "
      "the thickness of lines, radius of cones and arrows. ") );
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

    wxBoxSizer* bSizer51 = new wxBoxSizer( wxHORIZONTAL );
    bSizer51->Add( new wxStaticText( m_Gui, wxID_ANY, _("Length:"), 
      wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

    wxTextCtrl* edLength = new wxTextCtrl( m_Gui, ID_GLYPH_LENGTH);
    edLength->SetToolTip( _("Specifies the length of glyph, which will "
      "be scaled by the selected scaling component.") );
    bSizer51->Add( edLength, 1, wxALL, 1 );

    //and validator
    edLength->SetValidator(albaGUIValidator(this, ID_GLYPH_LENGTH, edLength, &m_GlyphLength, 1e-8, 1000));    

    bSizer51->Add( new wxStaticText( m_Gui, wxID_ANY, _("Scaling:"), 
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT ), 1, wxALL, 5 );

    wxComboBox* comboScale = new wxComboBox( m_Gui, ID_GLYPH_SCALING, 
      wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );  
    comboScale->Append( _("off") );
    comboScale->Append( _("magnitude") );
    if (nScalars != 0)
      comboScale->Append( _("scalar") );    
    comboScale->SetToolTip( _("Specifies how the length of glyph is scaled, "
      "whether by the magnitude of vectors, associated scalar value (from the "
      "selected scalar field) or not scaled at all.") );
    bSizer51->Add( comboScale, 1, wxALL, 0 );
    sbSizer2->Add( bSizer51, 1, wxEXPAND, 0 );

    //and validator
    comboScale->SetValidator(albaGUIValidator(this, ID_GLYPH_SCALING, comboScale, &m_GlyphScaling));

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
        "field are used to map colors, otherwise vector magnitudes are used.") );
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
	//wxCheckBox* chckShowAll1 = new wxCheckBox( m_Gui, ID_SHOW_ALL, _("Show all"), wxDefaultPosition, wxDefaultSize,0 );

    bSizer491->Add( chckShowSFLegend1, 0, wxALL|wxEXPAND, 5 );
	//bSizer491->Add( chckShowAll1,0,wxALL|wxEXPAND, 5);

    //and validator
    chckShowSFLegend1->SetValidator(albaGUIValidator(this, ID_SHOW_SF_MAPPING,       
      chckShowSFLegend1, &m_ShowMap));

    sbSizer221->Add( bSizer491, 1, wxEXPAND, 5 );
    sbSizer65->Add( sbSizer221, 0, wxEXPAND, 5 );
    bSizerMain->Add( sbSizer65, 0, wxEXPAND, 0 );
#pragma endregion Glyph Colors
  }//end of else

  m_Gui->Add(bSizerMain);
  return m_Gui;
}

//----------------------------------------------------------------------------
void albaPipeVectorFieldGlyphs::InitFilterList(int nScalars)
//----------------------------------------------------------------------------
{
  // init items of this gui
  // when load vme--[7/31/2009 weih]
	wxString cols[2] = { wxT("filter name"), wxT("range value") };
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
			//------insert item----
      //format--"filter-link0:aa:0.100:1.214"
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
			m_RangeCtrl->EnsureVisible(idx1 + 1);
			m_RangeCtrl->SetItemData(idx1 , (long)pItem);
			idx1++;

		}else if (linkName.StartsWith(FILTER_LINK_NAME2) &&  nScalars>0)
		{
			//------insert item----
      //format--"filter-link0:aa:0.100:1.214"
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
//----------------------------------------------------------------------------
void albaPipeVectorFieldGlyphs::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  // Processes events coming from GUI
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
	  }
	  else if (e->GetId() == ID_SHOWITEM)//show result of this filter
	  {
		  m_ShowAll = 0;
		  OnShowFilter(1);
		  m_Gui->Update();
	  }
	  else if(e->GetId() == ID_SHOWITEM2){
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
		  if (AddItem())//values are valid ,an item inserted
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
	  }	  
	  else if (e->GetId()==ID_ITEM_OK2)//check value
	  {
		  if (AddItem2())//values are valid ,an item inserted
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
	  }
	  else if (e->GetId()==ID_ITEM_CANCEL2)
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
//-----------------------------------------------------------------------
void albaPipeVectorFieldGlyphs::StoreFilterLinks()
//-----------------------------------------------------------------------
{
  //Store Filter values into link 
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
void albaPipeVectorFieldGlyphs::StoreFilterLinks2()
//-----------------------------------------------------------------------
{
  //Store Filter values into link 
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
void albaPipeVectorFieldGlyphs::OnShowFilter(int mode)
//-----------------------------------------------------------------------
{
	//use value of this item to filter data
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
void albaPipeVectorFieldGlyphs::OnAddItem(int idx)
//-----------------------------------------------------------------------
{
  //Add a range item
	CreateAddItemDlg(idx);
	
}
//-----------------------------------------------------------------------
void albaPipeVectorFieldGlyphs::OnRemoveItem()
//--------------------------------------------------------------------
{
  //Remove a range item
	int nIndex = m_RangeCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED); 
	if (nIndex >= 0)
	{
		m_RangeCtrl->DeleteItem(nIndex);
    // Update the GUI
		if (m_RangeCtrl2->GetItemCount()<1)
		{
			m_BttnShowAssociate->Enable(false);
		}
	}
}
//--------------------------------------------------------------------
void albaPipeVectorFieldGlyphs::OnRemoveItem2()
//--------------------------------------------------------------------
{
  //Remove a range item
	int nIndex = m_RangeCtrl2->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED); 
	if (nIndex >= 0)
	{
		m_RangeCtrl2->DeleteItem(nIndex);
    // Update the GUI
		if (m_RangeCtrl->GetItemCount()<1)
		{
			m_BttnShowAssociate->Enable(false);
		}
	}
}
//----------------------------------------------
bool albaPipeVectorFieldGlyphs::AddItem2()
//----------------------------------------------
{
  //insert item 
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

bool albaPipeVectorFieldGlyphs::AddItem()
{	
	double dValue1,dValue2;
	bool rtn = false;
	m_FilterValue1.ToDouble(&dValue1);
	m_FilterValue2.ToDouble(&dValue2);
	/*if (dValue2 > m_sr[1] && dValue2 -m_sr[1] < 0.0005)//1.2138546 will be ranged to 1.214 after wxString format 
	{
		dValue2 = m_sr[1];
	}
	if (dValue1< m_sr[0] && m_sr[0] - dValue1 < 0.0005)
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
void albaPipeVectorFieldGlyphs::CreateAddItemDlg(int idx)
//-----------------------------------------------------------------------
{
  //create dialog
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

	m_AddItemDlg = new albaGUIDialog(dlgName,albaCLOSEWINDOW | albaRESIZABLE);
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
//------------------------------------------------------------------------
/*virtual*/ void albaPipeVectorFieldGlyphs::OnChangeMaterial()
//------------------------------------------------------------------------
{
  // Handles change of material.
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
bool albaPipeVectorFieldGlyphs::DoCondition(int mode,double vectorValue,double scaleValue,double *rangeValue1,double *rangeValue2)
{
	bool rtn = false;
	bool vectorFlag;
	bool scaleFlag;
	if (mode ==1)
	{
		rtn = vectorValue >= rangeValue1[0] && vectorValue <= rangeValue1[1] ;
		
	}else if (mode ==2)
	{
		rtn = scaleValue >= rangeValue2[0] && scaleValue <= rangeValue2[1];
	}
	else if (mode ==3)
	{
		vectorFlag = vectorValue >= rangeValue1[0] && vectorValue <= rangeValue1[1] ;
		scaleFlag = scaleValue >= rangeValue2[0] && scaleValue <= rangeValue2[1];

		if (m_AndOr ==0)//and
		{
			rtn = vectorFlag && scaleFlag;

		}else if (m_AndOr == 1)//or
		{
			rtn = vectorFlag || scaleFlag;
		}
	}

	return rtn;
}

//--------------------------------------------------------------------------
void albaPipeVectorFieldGlyphs::DoFilter(int mode ,double *rangeValue,double *rangeValue2)
//--------------------------------------------------------------------------
{	
  //filter begin
	m_Output = vtkPolyData::New();
	m_Output->Initialize();
  // Create points and attribute arrays (vector or tensor as requested by user)
	vtkPoints *points = vtkPoints::New() ;

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
	double *pCoord;
	//float xValue,yValue,zValue;
	double xDvalue,yDvalue,zDvalue;

	
	vtkDataSet *orgData = m_Vme->GetOutput()->GetVTKData();
	vtkPointData *allPoints = orgData->GetPointData();
	vtkDataArray *old_scalars = allPoints->GetScalars();
	vtkDataArray *old_vectors = allPoints->GetVectors();
	vtkDataArray *old_tensors = allPoints->GetTensors();

	
	int nPoints = m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetNumberOfTuples();
	int idx,num=0 ;

	if(orgData->IsA("vtkRectilinearGrid")){
		
		
		//vtkImageData* pImgData = GetImageData(orgDataR);
		
		albaString logFname2 = "coordFile.txt";//debug
		std::ofstream outputFile2(logFname2, std::ios::out|std::ios::app);//debug
		outputFile2.clear();//debug
		outputFile2<<"---------------begin--------------------------------"<<std::endl;//debug
		int dim[3];

		vtkRectilinearGrid *orgDataR = vtkRectilinearGrid::SafeDownCast(orgData);
/*		vtkFloatArray *xCoord = vtkFloatArray::SafeDownCast(orgDataR->GetXCoordinates());//@todo
		vtkFloatArray *yCoord = vtkFloatArray::SafeDownCast(orgDataR->GetYCoordinates());
		vtkFloatArray *zCoord = vtkFloatArray::SafeDownCast(orgDataR->GetZCoordinates());
*/
		vtkDataArray *xCoord = orgDataR->GetXCoordinates();
		vtkDataArray *yCoord = orgDataR->GetYCoordinates();
		vtkDataArray *zCoord = orgDataR->GetZCoordinates();

		//-----------test code ------------------
		orgDataR->GetDimensions(dim);
		int numberOfTuple = old_scalars->GetNumberOfTuples();
		//float  fMin,fMax;
		
		if (nPoints>0)
		{
			for (int iz=0;iz<dim[2];iz++)
			{
				for (int iy=0;iy<dim[1];iy++)
				{
					for (int ix=0;ix<dim[0];ix++)
					{
						idx = ix+iy*dim[0]+iz*dim[0]*dim[1];//position in whole image
						
						outputFile2<< "  tmpScale="<<old_scalars->GetTuple1(idx)<<std::endl;//debug
						
						xDvalue = old_vectors->GetTuple3(idx)[0];
						yDvalue = old_vectors->GetTuple3(idx)[1];
						zDvalue = old_vectors->GetTuple3(idx)[2];
						double tmpVectorValue = sqrt(xDvalue*xDvalue + yDvalue*yDvalue + zDvalue*zDvalue);
						double tmpScaleValue = old_scalars->GetTuple1(idx);

						if (DoCondition(mode,tmpVectorValue,tmpScaleValue,rangeValue,rangeValue2))//tmpScale>=dMin && tmpScale<=dMax
						{

							pCoord = allPoints->GetScalars()->GetTuple(idx);

							pCoord[0] = xCoord->GetTuple1(ix);
							pCoord[1] = yCoord->GetTuple1(iy);
							pCoord[2] = zCoord->GetTuple1(iz);

							points->InsertNextPoint(pCoord);
							vectors->InsertNextTuple(old_vectors->GetTuple3(idx));
							scalars->InsertNextTuple((float*)&tmpScaleValue);
							num++;

						}
					}
				}
			}
			
		}
		outputFile2.close();//debug
	}
	else if (orgData->IsA("vtkImageData") || orgData->IsA("vtkImageData"))
	{
		vtkImageData *orgDataS =vtkImageData::SafeDownCast(orgData) ;
		double origin[3],spacing[3];
		int dim[3];
		vtkIdType increment[3];
		orgDataS->GetOrigin(origin) ;
		orgDataS->GetSpacing(spacing) ;
		orgDataS->GetDimensions(dim);
		orgDataS->GetIncrements(increment);

		
		//albaString logFname2 = "coordFile.txt";//if debug
		//std::ofstream outputFile2(logFname2, std::ios::out|std::ios::app);//if debug
		//outputFile2.clear();//if debug
		//outputFile2<<"-----------------------------------------------"<<std::endl;//if debug

		
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
						pCoord = allPoints->GetScalars()->GetTuple(idx);
						//double tmpScale =  sqrt(pCoord[0]*pCoord[0]+pCoord[1]*pCoord[1]+pCoord[2]*pCoord[2]);	 
						double *vet = old_vectors->GetTuple3(idx);
						double tmpVectorValue =  sqrt(vet[0]*vet[0]+vet[1]*vet[1]+vet[2]*vet[2]);
						double tmpScaleValue = NULL;

						if (mode>1)
						{
							tmpScaleValue = old_scalars->GetTuple1(idx);
						}

						if (DoCondition(mode,tmpVectorValue,tmpScaleValue,rangeValue,rangeValue2))
						{
							pCoord[0] = origin[0]  + ix * spacing[0];
							pCoord[1] = origin[1]  + iy * spacing[1];
							pCoord[2] = origin[2]  + iz * spacing[2];
										 
							points->InsertNextPoint(pCoord);
							scalars->InsertNextTuple1(tmpScaleValue);

							//outputFile2<< "  tmpScale="<<tmpScale<<"  x:"<<pCoord[0]<<"   y:"<<pCoord[1]<<"  z:"<<pCoord[2]<<std::endl;//if debug

							vectors->InsertNextTuple(vet);
							if (old_tensors)
							{
								tensors->InsertNextTuple(old_tensors->GetTuple9(idx));
							}
							num++;
						}
					}
				}
			}

		}
	}//end of else if

	scalars->Squeeze();
	vectors->Squeeze();
	tensors->Squeeze();

	//outputFile2.close();//if debug
	
	m_Output->SetPoints(points) ;
	m_Output->GetPointData()->SetScalars(scalars) ;
	m_Output->GetPointData()->SetVectors(vectors) ;
	m_Output->GetPointData()->SetTensors(tensors);

	m_Glyphs->SetInputData(m_Output);
}

//------------------------------------------------------------------------

vtkImageData* albaPipeVectorFieldGlyphs::GetImageData(vtkRectilinearGrid* pInput)
//------------------------------------------------------------------------
{
  //Converts the given rectilinear grid into a regular grid.
  //In case the operation cannot be successfully completed (e.g., because
  //it is not allowed or it would needed sampling of data),
  //it returns NULL, otherwise it constructs a new object

	vtkFloatArray* pXYZ[3];
	/*pXYZ[0] = vtkDoubleArray::SafeDownCast(pInput->GetXCoordinates());
	pXYZ[1] = vtkDoubleArray::SafeDownCast(pInput->GetYCoordinates());
	pXYZ[2] = vtkDoubleArray::SafeDownCast(pInput->GetZCoordinates());*/
	
	pXYZ[0] = vtkFloatArray::SafeDownCast(pInput->GetXCoordinates());
	pXYZ[1] = vtkFloatArray::SafeDownCast(pInput->GetYCoordinates());
	pXYZ[2] = vtkFloatArray::SafeDownCast(pInput->GetZCoordinates());

	double sp[3];     //spacing
	double origin[3]; //origin
	for (int i = 0; i < 3; i++) 
	{
		if (pXYZ[i] == NULL || !DetectSpacing(pXYZ[i], &sp[i]))
			return NULL;  //cannot continue

		origin[i] = *(pXYZ[i]->GetPointer(0));
	}

	//we can convert it to image data
	vtkImageData* pRet = vtkImageData::New();
	pRet->SetDimensions(pInput->GetDimensions());
	pRet->SetOrigin(origin);
	pRet->SetSpacing(sp);

	vtkDataArray *scalars = pInput->GetPointData()->GetScalars();
	pRet->AllocateScalars(scalars->GetDataType(),scalars->GetNumberOfComponents());
	pRet->GetPointData()->SetScalars(scalars);

	return pRet;
}

//------------------------------------------------------------------------
bool albaPipeVectorFieldGlyphs::DetectSpacing(vtkFloatArray* pCoords, double* pOutSpacing)
//------------------------------------------------------------------------
{
  //Detects spacing in the given array of coordinates.
  //It returns false, if the spacing between values is non-uniform
	int nCount = pCoords->GetNumberOfTuples();
	if (nCount <= 1)        //one slice
	{
		*pOutSpacing = 1.0;
		return true;
	}

	//at least 2 slices => detect min and max spacing
	float* pData = pCoords->GetPointer(0);  
	double dblMin, dblMax;
	dblMax = dblMin = pData[1] - pData[0];

	for (int i = 2; i < nCount; i++)
	{
		double dblSp = pData[i] - pData[i - 1];    
		if (dblSp < dblMin)
			dblMin = dblSp;
		else if (dblSp > dblMax)
			dblMax = dblSp;
	}

	//the difference between min and max spacing is insignificant,
	//then we can assume the coordinates have uniform spacing
	*pOutSpacing = (pData[nCount - 1] - pData[0]) / nCount;
	if ((dblMax - dblMin) / *pOutSpacing <= 1e-3)  
		return true;  

	*pOutSpacing = 0.0;
	return false;
}

//------------------------------------------------------------------------
/*virtual*/ void albaPipeVectorFieldGlyphs::CreateVTKPipe()
//------------------------------------------------------------------------
{
  //Constructs VTK pipeline.
  //build materials  
  albaNEW(m_GlyphMaterial);

  //build LUT
  m_ColorMappingLUT = vtkLookupTable::New();
  lutPreset(12, m_ColorMappingLUT); //initialize LUT to SAR (it has index 12)  
  m_ColorMappingLUT->Build();

  m_GlyphLine = vtkLineSource::New();   
  m_GlyphCone = vtkConeSource::New(); 
  m_GlyphArrow = vtkArrowSource::New(); 

  m_GlyphLine->SetPoint1(0.0, 0.0, 0.0);
  m_GlyphLine->SetPoint2(1.0, 0.0, 0.0);
//  m_GlyphLine->CappingOn();
  //Set capping to on
  m_GlyphCone->CappingOn();  
  m_GlyphArrow->SetTipLength(0.5);     

  m_Glyphs = vtkGlyph3D::New();

  m_Glyphs->SetInputData(m_Vme->GetOutput()->GetVTKData());

  m_Glyphs->SetVectorModeToUseVector();

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
/*virtual*/ void albaPipeVectorFieldGlyphs::UpdateVTKPipe()
//------------------------------------------------------------------------
{
  //Updates VTK pipeline
  //(setting radius and other parmeters). 
  albaPipeVectorFieldGlyphs::count ++; 

  if (m_GlyphType == GLYPH_LINES)
  {
    m_Glyphs->SetSourceConnection(m_GlyphLine->GetOutputPort());
  }
  else if (m_GlyphType == GLYPH_CONES)
  {
    m_GlyphCone->SetRadius(m_GlyphRadius);
    m_GlyphCone->SetHeight(m_GlyphLength);
    m_GlyphCone->SetResolution(m_GlyphRes);

    m_Glyphs->SetSourceConnection(m_GlyphCone->GetOutputPort());
  }
  else
  {
    m_GlyphArrow->SetShaftRadius(0.3 * m_GlyphRadius);    
    m_GlyphArrow->SetShaftResolution(m_GlyphRes);

    m_GlyphArrow->SetTipRadius(m_GlyphRadius);    
    m_GlyphArrow->SetTipResolution(m_GlyphRes);
    
    m_Glyphs->SetSourceConnection(m_GlyphArrow->GetOutputPort());
  }

  if (m_GlyphScaling == SCALING_OFF)
    m_Glyphs->SetScaleModeToDataScalingOff();
  else if (m_GlyphScaling == SCALING_MAGNITUDE)
    m_Glyphs->SetScaleModeToScaleByVector();
  else
    m_Glyphs->SetScaleModeToScaleByScalar();

  if (m_ShowAll){
	m_Glyphs->SetInputData(m_Vme->GetOutput()->GetVTKData());
	m_GlyphsMapper->ColorByArrayComponent(GetScalarFieldName(m_ScalarFieldIndex),1);
  }
  
  if (m_UseSFColorMapping == 0)
    m_Glyphs->SetColorModeToColorByVector();
  else
    m_Glyphs->SetColorModeToColorByScalar();
    

  m_Glyphs->Update();
    
  vtkDataArray* da = m_Glyphs->GetOutput()->GetPointData()->GetScalars();

  vtkDataSet *orgData = m_Vme->GetOutput()->GetVTKData();
  vtkPointData *allPoints = orgData->GetPointData();
//  int nPoints = allPoints->GetScalars()->GetSize();
  int nPoints = m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetNumberOfTuples();
  vtkDataArray *old_scalars = allPoints->GetScalars();


  double sr[2];
  if (da!=NULL)
  {
	  if (albaPipeVectorFieldGlyphs::count==1)
	  {
		da->GetRange(m_Sr);//used for vtkStructedPoint

		int nScalars = GetNumberOfScalars();
		if ( nScalars>0)
		{
			double tmpValue; //used for vtkRectilinearGrid
			m_Sr2[0] = old_scalars->GetTuple1(0);
			m_Sr2[1] = old_scalars->GetTuple1(0);

			for (int i=0;i<nPoints;i++)
			{
				tmpValue = old_scalars->GetTuple1(i);
				if (tmpValue>m_Sr2[1])
				{
					m_Sr2[1] = tmpValue;
				}
				if (tmpValue<m_Sr2[0])
				{
					m_Sr2[0] = tmpValue;
				}
			}
		}

	  }
	  
	  da->GetRange(sr);

	  if (m_UseColorMapping != 0)
	  {      
		  m_ColorMappingLUT->SetTableRange(sr);
		  m_GlyphsMapper->SetScalarRange(sr);        
	  }
  }



  m_GlyphsMapper->SetScalarVisibility(m_UseColorMapping); 
  m_GlyphsMapper->Update();

  m_GlyphsActor->SetProperty(m_GlyphMaterial->GetMaterial()->m_Prop);
  m_GlyphsActor->Modified();
//  m_GlyphsActor->SetVisibility();  

//  m_SFActor->SetTitle(scalar_name);
  m_SFActor->SetVisibility(m_ShowMap != 0 && m_UseColorMapping != 0);  
}