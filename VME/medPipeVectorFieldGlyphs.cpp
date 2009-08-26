/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: medPipeVectorFieldGlyphs.cpp,v $ 
  Language: C++ 
  Date: $Date: 2009-08-26 14:47:27 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  modify: Hui Wei (beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafDecl.h"
#include "medPipeVectorFieldGlyphs.h"

#include "mafSceneNode.h"
#include "mafPipeFactory.h"
#include "mafGUI.h"
#include "mafGUIValidator.h"
#include "mafGUILutSwatch.h"
#include "mafGUILutPreset.h"
#include "mafGUIPicButton.h"
#include "mafGUIMaterialChooser.h"
#include "mafVME.h"
#include "mafVMESurface.h"
#include "mmaMaterial.h"

#include "vtkMAFAssembly.h"
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
#include "vtkStructuredPoints.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkDoubleArray.h"
#include "mafGUIDialog.h"
#include "mafGUIValidator.h"
#include "mafGUIButton.h"


//#include "vtkStructuredPointsToPolyDataFilter.h"
//#include "medPointsFilter.h"


#include "wx/busyinfo.h"
#include <wx/tokenzr.h>
#include <float.h>


//----------------------------------------------------------------------------
mafCxxTypeMacro(medPipeVectorFieldGlyphs);
//----------------------------------------------------------------------------
const  char* medPipeVectorFieldGlyphs::FILTER_LINK_NAME = "filter-link-vector";
int medPipeVectorFieldGlyphs::count;
//----------------------------------------------------------------------------
medPipeVectorFieldGlyphs::medPipeVectorFieldGlyphs() : medPipeVectorField()
//----------------------------------------------------------------------------
{    
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
  medPipeVectorFieldGlyphs::count = 0;
}

//----------------------------------------------------------------------------
medPipeVectorFieldGlyphs::~medPipeVectorFieldGlyphs()
//----------------------------------------------------------------------------
{  
  m_AssemblyFront->RemovePart(m_GlyphsActor);  
  m_RenFront->RemoveActor2D(m_SFActor);  
  
  vtkDEL(m_SFActor);
  
  vtkDEL(m_GlyphsActor);
  vtkDEL(m_GlyphsMapper);
  vtkDEL(m_Glyphs);
  vtkDEL(m_GlyphArrow);
  vtkDEL(m_GlyphCone);
  vtkDEL(m_GlyphLine);
  
  vtkDEL(m_ColorMappingLUT);
  mafDEL(m_GlyphMaterial);    
}


//----------------------------------------------------------------------------
//Default radius, etc. should be calculated in this method, 
//i.e., inherited classes should always override this method. 
//The default implementation is to update VME
/*virtual*/ void medPipeVectorFieldGlyphs::ComputeDefaultParameters()
//----------------------------------------------------------------------------
{
  Superclass::ComputeDefaultParameters();

  double dblDiag = m_Vme->GetOutput()->GetVTKData()->GetLength();
  m_GlyphRadius = dblDiag / 80;
}

//----------------------------------------------------------------------------
mafGUI *medPipeVectorFieldGlyphs::CreateGui()
//----------------------------------------------------------------------------
{
  int nVectors = GetNumberOfVectors();
  int nScalars = GetNumberOfScalars();

  m_Gui = new mafGUI(this);
     
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
      comboField->SetValidator(mafGUIValidator(this, ID_VECTORFIELD, comboField, &m_VectorFieldIndex));
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
      comboScalars->SetValidator(mafGUIValidator(this, ID_SCALARFIELD, comboScalars, &m_ScalarFieldIndex));
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
    comboGlyphs->SetValidator(mafGUIValidator(this, ID_GLYPH_TYPE, comboGlyphs, &m_GlyphType));
//-----------weih add-----------
#pragma region Glyph range
//-------------------------list----------
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
	m_BttnAddItem->SetValidator(mafGUIValidator(this, ID_ADDITEM, m_BttnAddItem));

	m_BttnRemoveItem = new wxButton( m_Gui, ID_REMOVEITEM, wxT("Remove"), 
		wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_BttnRemoveItem->Enable( false );
	m_BttnRemoveItem->SetToolTip( wxT("Removes selected item.") );
	m_BttnRemoveItem->SetValidator(mafGUIValidator(this, ID_REMOVEITEM, m_BttnRemoveItem));

	m_BttnShow = new wxButton( m_Gui, ID_SHOWITEM, wxT("Show"), 
		wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_BttnShow->Enable( false );
	m_BttnShow->SetToolTip( wxT("Show result.") );
	m_BttnShow->SetValidator(mafGUIValidator(this, ID_SHOWITEM, m_BttnShow));

	bSizer6->Add( m_BttnAddItem, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1 );
	bSizer6->Add( m_BttnRemoveItem, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1 );
	bSizer6->Add( m_BttnShow,0, wxALIGN_CENTER_VERTICAL|wxALL, 1);
	
	sbSizer3->Add( bSizer6, 0, wxEXPAND, 1 );
//------------------------check box-------------
/*	wxBoxSizer* bSizer7 = new wxBoxSizer( wxVERTICAL );  
*/
	wxCheckBox* chckShowAll = new wxCheckBox( m_Gui, 
		ID_SHOW_ALL, _("Show all"), wxDefaultPosition, wxDefaultSize,0 );
	chckShowAll->SetToolTip( _("If checked, glyphs are showed by using original data.") );

	//and validator
	chckShowAll->SetValidator(mafGUIValidator(this, ID_SHOW_ALL,       
		chckShowAll, &m_ShowAll));
	//bSizer7->Add( chckShowAll, 0, wxALL|wxEXPAND, 5 );	
	sbSizer3->Add( chckShowAll, 0, wxALL|wxEXPAND, 5 );

//----------------------------------------------	
	bSizerMain->Add( sbSizer3, 0, wxEXPAND, 1 );
	
#pragma endregion Glyph range
    InitFilterList();

//----------weih add end---------

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
    edRadius->SetValidator(mafGUIValidator(this, ID_GLYPH_RADIUS, edRadius, &m_GlyphRadius, 1e-8, 1000));

    bSizer5->Add( new wxStaticText( m_Gui, wxID_ANY, _("Res:"), 
      wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT ), 1, wxALL, 5 );

    wxTextCtrl* edRes = new wxTextCtrl( m_Gui, ID_GLYPH_RESOLUTION);
    edRes->SetToolTip( _("Specifies the resolution of glyphs, "
      "i.e., number of sides of cones, etc.") );
    bSizer5->Add( edRes, 1, wxALL, 1 );
    sbSizer2->Add( bSizer5, 0, wxEXPAND, 5 );

    //and validator
    edRes->SetValidator(mafGUIValidator(this, ID_GLYPH_RESOLUTION, edRes, &m_GlyphRes, 3, 100));

    wxBoxSizer* bSizer51 = new wxBoxSizer( wxHORIZONTAL );
    bSizer51->Add( new wxStaticText( m_Gui, wxID_ANY, _("Length:"), 
      wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

    wxTextCtrl* edLength = new wxTextCtrl( m_Gui, ID_GLYPH_LENGTH);
    edLength->SetToolTip( _("Specifies the length of glyph, which will "
      "be scaled by the selected scaling component.") );
    bSizer51->Add( edLength, 1, wxALL, 1 );

    //and validator
    edLength->SetValidator(mafGUIValidator(this, ID_GLYPH_LENGTH, edLength, &m_GlyphLength, 1e-8, 1000));    

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
    comboScale->SetValidator(mafGUIValidator(this, ID_GLYPH_SCALING, comboScale, &m_GlyphScaling));

    bSizerMain->Add( sbSizer2, 0, wxEXPAND, 5 );
#pragma endregion Glyph shape

#pragma region Glyph Colors
    wxStaticBoxSizer* sbSizer65 = new wxStaticBoxSizer( 
      new wxStaticBox( m_Gui, wxID_ANY, _("Glyph Color") ), wxVERTICAL );

    wxBoxSizer* bSizer4311 = new wxBoxSizer( wxHORIZONTAL );
    bSizer4311->Add( new wxStaticText( m_Gui, wxID_ANY, _("Material:"), 
      wxDefaultPosition, wxSize( 60,-1 ), 0 ), 0, wxALL, 5 );

    m_GlyphMaterial->GetMaterial()->MakeIcon();
    m_GlyphMaterialButton = new mafGUIPicButton(m_Gui, 
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
    chckShowScalarField1->SetValidator(mafGUIValidator(this, ID_USE_COLOR_MAPPING, 
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
      checkBox52->SetValidator(mafGUIValidator(this, ID_USE_SF_MAPPING,       
        checkBox52, &m_UseSFColorMapping));     
    }

    wxBoxSizer* bSizer491 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* bSizer432 = new wxBoxSizer( wxHORIZONTAL );    
    bSizer432->Add( new wxStaticText( m_Gui, wxID_ANY, _("LUT:")), 0, wxALL, 5 );

    mafGUILutSwatch* luts2 = new mafGUILutSwatch(m_Gui, ID_SF_MAPPING_LUT, 
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
    chckShowSFLegend1->SetValidator(mafGUIValidator(this, ID_SHOW_SF_MAPPING,       
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
void medPipeVectorFieldGlyphs::InitFilterList(){
//----------------------------------------------------------------------------
	wxString cols[2] = { wxT("filter nameV"), wxT("range value") };
	for (int i = 0; i < 2; i++){
		m_RangeCtrl->InsertColumn(i, cols[i]);
	}
	mafNode::mafLinksMap* pLinks =  m_Vme->GetLinks();
	wxString itemName,itemValue1,itemValue2,displayValue;
	double dValue1,dValue2;
	int idx =0;
	for (mafNode::mafLinksMap::iterator i = pLinks->begin(); i != pLinks->end(); i++)
	{
		mafString linkName = i->first;
		if (linkName.StartsWith(FILTER_LINK_NAME))
		{
			//------insert item----format--"filter-link0:aa:0.100:1.214"
			FILTER_ITEM* pItem = new FILTER_ITEM;
			memset(pItem, 0, sizeof(FILTER_ITEM));

			wxStringTokenizer tkz(wxT(linkName.GetCStr()), wxT(":"));
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
			m_RangeCtrl->InsertItem(idx,itemName);
			displayValue =  "["+itemValue1+","+itemValue2+"]";
			m_RangeCtrl->SetItem(idx ,1,displayValue);
			m_RangeCtrl->SetItemState(idx , wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED,
				wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
			m_RangeCtrl->EnsureVisible(idx+1);
			m_RangeCtrl->SetItemData(idx , (long)pItem);
			idx++;

		}
	}
	if (idx>0)
	{
		m_BttnShow->Enable(true);
		m_BttnRemoveItem->Enable(true);
	}

}
//----------------------------------------------------------------------------
void medPipeVectorFieldGlyphs::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{			
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {	
    if (e->GetId() >= Superclass::ID_LAST && e->GetId() < ID_LAST)
    {
	  if (e->GetId() == ID_GLYPH_MATERIAL){
        OnChangeMaterial();
	  }
	  else if (e->GetId() == ID_ADDITEM)
	  {
		OnAddItem();
	  }else if (e->GetId() == ID_SHOWITEM)//show result of this filter
	  {
		  m_ShowAll = 0;
		  OnShowFilter();
		  m_Gui->Update();
	  }else if (e->GetId() == ID_REMOVEITEM)//remove an Item
	  {
		  OnRemoveItem();
		  StoreFilterLinks();
	  }
	  else if (e->GetId()==ID_RANGE_NAME)
	  {
		  m_ItemOK->Enable(true);
	  }else if (e->GetId()==ID_ITEM_OK)//check value
	  {
		  if (AddItem())//if values are valid ,an item inserted
		  {
			  m_AddItemDlg->EndModal(wxID_OK);
			  m_BttnShow->Enable(true);
			  m_BttnRemoveItem->Enable(true);
			  m_Gui->Update();
			  StoreFilterLinks();
		  }	
	  }else if (e->GetId()==ID_ITEM_CANCEL)//close window
	  {
		  m_AddItemDlg->EndModal(wxID_CANCEL);
	  }else if (e->GetId()==ID_SHOW_ALL)
	  {
		  if (m_ShowAll)
		  {
			  m_Glyphs->SetInput(m_Vme->GetOutput()->GetVTKData());
		  }
	  }
        
      UpdateVTKPipe(); 
    
      mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      return;
    }     
  }

  //forward it to our listener to deal with it
  mafEventMacro(*maf_event);
}
//-----------------------------------------------------------------------
//Store Filter values into link 
void medPipeVectorFieldGlyphs::StoreFilterLinks(){
//-----------------------------------------------------------------------

	mafNode::mafLinksMap* pLinks = m_Vme->GetLinks(); 
	//remove old filter of this link
	bool bNeedRestart;  
	do
	{
		bNeedRestart = false;
		for (mafNode::mafLinksMap::iterator i = pLinks->begin(); i != pLinks->end(); i++)
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
	mafString szName;
	wxString itemName;
	int nCount = m_RangeCtrl->GetItemCount();
	if (nCount>0)
	{
		for (int i=0;i<nCount;i++)
		{
			itemName = m_RangeCtrl->GetItemText(i);
			FILTER_ITEM* pItem = (FILTER_ITEM*)m_RangeCtrl->GetItemData(i);
			mafString szName;
			szName = wxString::Format("%s%d",FILTER_LINK_NAME,i);
			szName += ":";
			szName += itemName;
			szName += ":";
			szName +=   wxString::Format("%.4f",pItem->value[0]);//wxString::Format("%.3f%d",szName,pItem->value[0]);
			szName += ":";
			szName +=  wxString::Format("%.4f",pItem->value[1]);//wxString::Format("%s%.3f",szName,pItem->value[1]);
			
			m_Vme->SetLink(szName,m_Vme);
		}
	}
}
//-----------------------------------------------------------------------
//use value of this item to filte data
void medPipeVectorFieldGlyphs::OnShowFilter(){
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
}
//-----------------------------------------------------------------------
//Add a range item
void medPipeVectorFieldGlyphs::OnAddItem(){
//-----------------------------------------------------------------------

	createAddItemDlg();
	
}
//-----------------------------------------------------------------------
//Remove a range item
void medPipeVectorFieldGlyphs::OnRemoveItem(){
//--------------------------------------------------------------------
	int nIndex = m_RangeCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED); 
	if (nIndex >= 0)
	{
		m_RangeCtrl->DeleteItem(nIndex);
	}
}
/*---------------------------------------------*/
//insert item 
//----------------------------------------------
bool medPipeVectorFieldGlyphs::AddItem(){
	
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
	if (fabs(dValue2 - m_sr[1]) < 0.00005)
	{
		dValue2 = m_sr[1];
	}
	if (fabs(dValue1- m_sr[0]) < 0.00005)
	{
		dValue1 = m_sr[0];
	}

	if (dValue1<=dValue2 && dValue1>=m_sr[0] && dValue2<=m_sr[1] )
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
		mafMessage("invalid value,please check");
		rtn = false;
	}
	return rtn;
}
//-----------------------------------------------------------------------
//create dialog
void medPipeVectorFieldGlyphs::createAddItemDlg(){
//-----------------------------------------------------------------------
	/*vtkDataArray *dataArr = m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetScalars();
	double range[2];
	dataArr->GetRange(range);*/

	m_AddItemDlg = new mafGUIDialog("filter editor",mafCLOSEWINDOW | mafRESIZABLE);
	// vertical stacker for the rows of widgets
	wxBoxSizer *vs1 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer *hs_1 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *hs_2 = new wxBoxSizer(wxHORIZONTAL);
	
	wxString label1 = "  name: ";
	wxString label2 = "  range: ";
	wxString label3 = "  <value<  ";

	//-----------------filter name------------------
	wxStaticText *textName  = new wxStaticText(m_AddItemDlg, wxID_ANY, label1, wxPoint(0,0), wxSize(50,20));
	wxTextCtrl   *textNameValue = new wxTextCtrl(m_AddItemDlg , ID_RANGE_NAME, "ddd", wxPoint(0,0), wxSize(50,20), wxNO_BORDER  );
	textNameValue->SetValidator(mafGUIValidator(this, ID_RANGE_NAME, textNameValue, &m_FilterName));

	//-----------------filter value1----------------
	m_FilterValue1 = wxString::Format("%.4f",m_sr[0]);
	wxStaticText *textValue1Name = new wxStaticText(m_AddItemDlg,wxID_ANY,label2,wxPoint(0,0),wxSize(50,20));
	wxTextCtrl   *textValue1Value = new wxTextCtrl(m_AddItemDlg,ID_RANGE_VALUE1,"",wxPoint(0,0), wxSize(50,20),wxNO_BORDER);
	textValue1Value->SetValidator(mafGUIValidator(this, ID_RANGE_VALUE1, textValue1Value, &m_FilterValue1));
	//-----------------filter value2----------------
	m_FilterValue2 = wxString::Format("%.4f",m_sr[1]);
	wxStaticText *textValue2Name = new wxStaticText(m_AddItemDlg,wxID_ANY,label3,wxPoint(0,0),wxSize(50,20));
	wxTextCtrl   *textValue2Value = new wxTextCtrl(m_AddItemDlg,ID_RANGE_VALUE2,"",wxPoint(0,0), wxSize(50,20),wxNO_BORDER);
	textValue2Value->SetValidator(mafGUIValidator(this, ID_RANGE_VALUE2, textValue2Value, &m_FilterValue2));
	//-----------------ok cancel button-------------

	// ok/cancel buttons
	m_ItemOK = new mafGUIButton(m_AddItemDlg, ID_ITEM_OK, "OK", wxPoint(0,0), wxSize(50,20));
	m_ItemOK->SetListener(this);
	m_ItemOK->Enable(false);

	mafGUIButton *b_cancel = new mafGUIButton(m_AddItemDlg, ID_ITEM_CANCEL, "CANCEL", wxPoint(0,0), wxSize(50,20));
	b_cancel->SetListener(this);

	wxBoxSizer *hs_b = new wxBoxSizer(wxHORIZONTAL);
	hs_b->Add(m_ItemOK,0);
	hs_b->Add(b_cancel, 1, wxEXPAND);
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
// Handles change of material.
/*virtual*/ void medPipeVectorFieldGlyphs::OnChangeMaterial()
//------------------------------------------------------------------------
{    
  mafGUIMaterialChooser dlg;
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

//--------------------------------------------------------------------------
//filter begin
//--------------------------------------------------------------------------
void medPipeVectorFieldGlyphs::doFilter(double *rangeValue){

	
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

	vtkStructuredPoints *orgData =vtkStructuredPoints::SafeDownCast(m_Vme->GetOutput()->GetVTKData()) ;

	vtkPointData *allPoints = orgData->GetPointData();
	vtkDataArray *old_scalars = allPoints->GetScalars();
	vtkDataArray *old_vectors = allPoints->GetVectors();
	vtkDataArray *old_tensors = allPoints->GetTensors();

	vtkDataArray *allArray = allPoints->GetScalars();

	int nPoints = allArray->GetSize();
	double origin[3],spacing[3];
	int dim[3],increment[3];
	orgData->GetOrigin(origin) ;
	orgData->GetSpacing(spacing) ;
	orgData->GetDimensions(dim);
	orgData->GetIncrements(increment);

	double *pCoord;
	//mafString logFname2 = "coordFile.txt";//if debug
	//std::ofstream outputFile2(logFname2, std::ios::out|std::ios::app);//if debug
	//outputFile2.clear();//if debug
	//outputFile2<<"-----------------------------------------------"<<std::endl;//if debug

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

					 //outputFile2<< "  tmpScale="<<tmpScale<<"  x:"<<pCoord[0]<<"   y:"<<pCoord[1]<<"  z:"<<pCoord[2]<<std::endl;//if debug

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

	//outputFile2.close();//if debug
	
	m_Output->SetPoints(points) ;
	m_Output->GetPointData()->SetScalars(scalars) ;
	m_Output->GetPointData()->SetVectors(vectors) ;
	m_Output->GetPointData()->SetTensors(tensors);
	//m_Output->GetPointData()->SetActiveScalars("scalars1");
	m_Output->Update();



	m_Glyphs->SetInput(m_Output);
	//m_Glyphs->SelectInputScalars("scalars1");
	//m_Glyphs->Update();
	
   /* vtkGlyph3D *test_Glyphs = vtkGlyph3D::New();
	test_Glyphs->SetInput(m_Output);
	test_Glyphs->SetScaleModeToScaleByVector();
	//test_Glyphs->SetScaleModeToScaleByScalar();
	test_Glyphs->GetOutput()->GetPointData()->SetScalars(scalars);
	test_Glyphs->Update();
	vtkDataArray *testScalars3 = test_Glyphs->GetOutput()->GetPointData()->GetScalars();
	double sr2[2],sr3[2];
	testScalars3->GetRange(sr2);

	m_Output->GetScalarRange(sr3);


	vtkDataArray *testScalars2 = m_Glyphs->GetOutput()->GetPointData()->GetScalars();
	double sr[2];
	testScalars2->GetRange(sr);*/




	
}

//------------------------------------------------------------------------
//Constructs VTK pipeline.
/*virtual*/ void medPipeVectorFieldGlyphs::CreateVTKPipe()
//------------------------------------------------------------------------
{
  //build materials  
  mafNEW(m_GlyphMaterial);

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
  m_GlyphCone->CappingOn();  
  m_GlyphArrow->SetTipLength(0.5);     

  m_Glyphs = vtkGlyph3D::New();

  m_Glyphs->SetInput(m_Vme->GetOutput()->GetVTKData());

  m_Glyphs->SetVectorModeToUseVector();

  m_GlyphsMapper = vtkPolyDataMapper::New();
  m_GlyphsMapper->SetInput(m_Glyphs->GetOutput());
  m_GlyphsMapper->ImmediateModeRenderingOn();
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
/*virtual*/ void medPipeVectorFieldGlyphs::UpdateVTKPipe()
//------------------------------------------------------------------------
{
  
  medPipeVectorFieldGlyphs::count ++; 

  if (m_GlyphType == GLYPH_LINES)
  {
    //m_GlyphLine->SetRadius(m_GlyphRadius);
    //m_GlyphLine->SetHeight(m_GlyphLength);
    //m_GlyphLine->SetResolution(m_GlyphRes);
    //
    m_Glyphs->SetSource(m_GlyphLine->GetOutput());
  }
  else if (m_GlyphType == GLYPH_CONES)
  {
    m_GlyphCone->SetRadius(m_GlyphRadius);
    m_GlyphCone->SetHeight(m_GlyphLength);
    m_GlyphCone->SetResolution(m_GlyphRes);

    m_Glyphs->SetSource(m_GlyphCone->GetOutput());
  }
  else
  {
    m_GlyphArrow->SetShaftRadius(0.3 * m_GlyphRadius);    
    m_GlyphArrow->SetShaftResolution(m_GlyphRes);

    m_GlyphArrow->SetTipRadius(m_GlyphRadius);    
    m_GlyphArrow->SetTipResolution(m_GlyphRes);
    
    m_Glyphs->SetSource(m_GlyphArrow->GetOutput());
  }

  if (m_GlyphScaling == SCALING_OFF)
    m_Glyphs->SetScaleModeToDataScalingOff();
  else if (m_GlyphScaling == SCALING_MAGNITUDE)
    m_Glyphs->SetScaleModeToScaleByVector();
  else
    m_Glyphs->SetScaleModeToScaleByScalar();

  if (m_ShowAll){
	m_Glyphs->SetInput(m_Vme->GetOutput()->GetVTKData());
	m_Glyphs->SelectInputScalars(GetScalarFieldName(m_ScalarFieldIndex));
  }
  
  if (m_UseSFColorMapping == 0)
    m_Glyphs->SetColorModeToColorByVector();
  else
    m_Glyphs->SetColorModeToColorByScalar();
    

  m_Glyphs->Update();
    
  vtkDataArray* da = m_Glyphs->GetOutput()->GetPointData()->GetScalars();


  double sr[2];
  if (da!=NULL)
  {
	  if (medPipeVectorFieldGlyphs::count==1)
	  {
		da->GetRange(m_sr);
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