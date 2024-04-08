/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMELabeledVolume
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaVMELabeledVolume.h"

#include <wx/tokenzr.h>

#include "albaGUI.h"
#include "albaGUIDialogPreview.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "albaVME.h"
#include "albaTransform.h"
#include "albaGUIButton.h"
#include "albaGUIValidator.h"
#include "albaGUICheckListBox.h"
#include "albaInteractorExtractIsosurface.h"
#include "albaRWIBase.h"
#include "albaRWI.h"
#include "albaGUIFloatSlider.h"
#include "albaString.h"
#include "albaDataPipeCustom.h"
#include "albaStorageElement.h"
#include "albaVMEItemVTK.h" 
#include "albaTagItem.h"
#include "albaTagArray.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEOutputVolume.h"

#include "vtkWindowLevelLookupTable.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkTexture.h"
#include "vtkPolyDataMapper.h"
#include "vtkCamera.h"
#include "vtkProbeFilter.h"
#include "vtkPlaneSource.h"
#include "vtkProperty.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkDataSet.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

#include <list>
#include <vector>

#define OUTRANGE_SCALAR -1000

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMELabeledVolume);
//-------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaVMELabeledVolume::albaVMELabeledVolume()
//------------------------------------------------------------------------------
{
  m_LabelCheckBox = NULL;
  m_EditMode = false;
  m_DataCopied = false;
  m_CheckMin = 0; 
  m_CheckMax = 0;
  m_MaxValue = 0;
  m_MinValue = 0;
  m_CheckListId = 0;

  m_VolumeLink = NULL;
  m_Dlg = NULL;
  m_Rwi = NULL;	
  m_SliceSlider = NULL;
  m_MinSlider = NULL;  
  m_MaxSlider = NULL;   
  m_LabelNameCtrl = NULL;
  m_LabelValueCtrl = NULL;
  m_LookUpTable = NULL;
  m_LookUpTableColor = NULL;
  m_ProbeFilter = NULL;  
  m_SP= NULL;
  m_RG = NULL;
  m_Texture = NULL;
  m_SMapper = NULL;
  m_ActorSlice = NULL;  

  m_LabelNameValue = wxEmptyString;
  m_LabelValueValue = wxEmptyString;

  albaNEW(m_Transform);
  albaVMEOutputVolume *output = albaVMEOutputVolume::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);
 
  DependsOnLinkedNodeOn();

  // attach a data pipe which creates a bridge between VTK and ALBA
  albaDataPipeCustom *dpipe = albaDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);
  dpipe->SetInput(NULL);
   
  for(int i=0; i<6; i++) 
    m_BBox[i] = 0;
}
//------------------------------------------------------------------------------
albaVMELabeledVolume::~albaVMELabeledVolume()
//------------------------------------------------------------------------------
{
  albaDEL(m_Transform);
  if (m_DataCopied)
    albaDEL(m_Dataset);
  m_VolumeLink = NULL;
  m_CheckedVector.clear();
  m_LabelNameVector.clear();

  if ( m_Dlg )
    DeleteOpDialog();
}

//-------------------------------------------------------------------------
int albaVMELabeledVolume::DeepCopy(albaVME *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a) == ALBA_OK)
  {
    albaVMELabeledVolume *lab_volume = albaVMELabeledVolume::SafeDownCast(a);
    albaVME *linked_node = lab_volume->GetLink("VolumeLink");
    if (linked_node)
    {
      SetVolumeLink(linked_node);
    }
    m_Transform->SetMatrix(lab_volume->m_Transform->GetMatrix());

    return ALBA_OK;
  }  
  return ALBA_ERROR;
}
//-------------------------------------------------------------------------
bool albaVMELabeledVolume::Equals(albaVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    ret = m_Transform->GetMatrix() == ((albaVMELabeledVolume *)vme)->m_Transform->GetMatrix() && \
      GetLink("VolumeLink") == ((albaVMELabeledVolume *)vme)->GetLink("VolumeLink");
  }
  return ret;
}
//----------------------------------------------------------------------------
void albaVMELabeledVolume::DeleteOpDialog()
//----------------------------------------------------------------------------
{
  m_Rwi->m_RenFront->RemoveActor( m_ActorSlice );  

  vtkDEL( m_LookUpTable );
  vtkDEL( m_LookUpTableColor );
  vtkDEL( m_SP);
  vtkDEL( m_ProbeFilter );
  vtkDEL( m_Texture ); 
  vtkDEL( m_SMapper );
  vtkDEL( m_ActorSlice );  
  cppDEL( m_Dlg ); 
}

//-------------------------------------------------------------------------
void albaVMELabeledVolume::SetVolumeLink(albaVME *n)
//-------------------------------------------------------------------------
{
  SetLink("VolumeLink", n);
  CopyDataset();
  Modified();
}

//-------------------------------------------------------------------------
void albaVMELabeledVolume::InternalPreUpdate()
//-------------------------------------------------------------------------
{
  m_VolumeLink = GetVolumeLink();
  EnableWidgets(m_VolumeLink != NULL);

  if (m_VolumeLink == NULL)
  {
    return;
  }

  if (!m_DataCopied)
  {
    CopyDataset();
  }
  GenerateLabeledVolume(); 
}
//-------------------------------------------------------------------------
void albaVMELabeledVolume::CopyDataset()
//-------------------------------------------------------------------------
{
  m_VolumeLink = GetVolumeLink();
  if (m_VolumeLink)
  {
    vtkDataSet *data = m_VolumeLink->GetOutput()->GetVTKData();
    m_Dataset = data->NewInstance();
    m_Dataset->DeepCopy(data);
    ((albaDataPipeCustom *)GetDataPipe())->SetInput(m_Dataset); 
    RetrieveTag();
    m_DataCopied = true;

    double scalarRange[2];

    //Set the scalar values  of the labeled volume to OUTRANGE_SCALAR
    vtkDataArray *originalScalars;
    originalScalars = m_Dataset->GetPointData()->GetScalars();
    originalScalars->FillComponent(0, OUTRANGE_SCALAR);
    originalScalars->Modified();
    originalScalars->GetRange(scalarRange);

    mmaVolumeMaterial *volMaterial = GetMaterial();
//    albaNEW(volMaterial);  //@@@@@@@@ Do the code below needs for something??
    volMaterial->DeepCopy(((albaVMEVolumeGray *)m_VolumeLink)->GetMaterial());
    volMaterial->UpdateFromTables();

    mmaVolumeMaterial *labelMaterial = ((albaVMEOutputVolume *)this->GetOutput())->GetMaterial();
    if  (labelMaterial)
    {
      ((albaVMEOutputVolume *)this->GetOutput())->GetMaterial()->DeepCopy(volMaterial);
      labelMaterial->m_ColorLut->SetTableRange(scalarRange);
      labelMaterial->UpdateFromTables();
    }
    else
    {
      ((albaVMEOutputVolume *)this->GetOutput())->SetMaterial(volMaterial);
      ((albaVMEOutputVolume *)this->GetOutput())->Update();
      mmaVolumeMaterial *NewlabelMaterial = ((albaVMEOutputVolume *)this->GetOutput())->GetMaterial();
      NewlabelMaterial->m_ColorLut->SetTableRange(scalarRange);
      NewlabelMaterial->UpdateFromTables();
    }
  }
}

//-------------------------------------------------------------------------
void albaVMELabeledVolume::SetMatrix(const albaMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
void albaVMELabeledVolume::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}
//-------------------------------------------------------------------------
albaVME *albaVMELabeledVolume::GetVolumeLink()
//-------------------------------------------------------------------------
{
  return GetLink("VolumeLink");
}

//-------------------------------------------------------------------------
void albaVMELabeledVolume::UpdateScalars()
//-------------------------------------------------------------------------
{
  m_VolumeLink = GetVolumeLink();
  EnableWidgets(m_VolumeLink != NULL);
  if (m_VolumeLink)
  {
    vtkDataSet *data = m_VolumeLink->GetOutput()->GetVTKData();
    m_Dataset->GetPointData()->GetScalars()->DeepCopy(data->GetPointData()->GetScalars());
    m_Dataset->GetPointData()->GetScalars()->Modified();
  }
  else
  {
    //Set the scalar values of the labeled volume to OUTRANGE_SCALAR
    vtkDataArray *originalScalars;

    if ( m_Dataset->IsA( "vtkImageData" ) )
    {
      vtkImageData *sp = (vtkImageData*) m_Dataset;
      originalScalars = sp->GetPointData()->GetScalars();  
    }
    else if ( m_Dataset->IsA( "vtkRectilinearGrid" ) )
    {    
      vtkRectilinearGrid *rg = (vtkRectilinearGrid*) m_Dataset;
      originalScalars = rg->GetPointData()->GetScalars();  
    }

    int not = originalScalars->GetNumberOfTuples();
    for ( int i = 0; i < not; i++ )
    {
      originalScalars->SetTuple1( i, OUTRANGE_SCALAR ); 
    }
    originalScalars->Modified();
    m_Dataset->GetPointData()->SetScalars(originalScalars);
    m_Dataset->Modified();
  }
}

//-------------------------------------------------------------------------
void albaVMELabeledVolume::GenerateLabeledVolume()
//-------------------------------------------------------------------------
{
  UpdateScalars();

  if (m_VolumeLink)
  {
    vtkDataArray *labelScalars;
    vtkDataArray *volumeScalars;

    if ( m_Dataset->IsA( "vtkImageData" ) )
    {
      vtkImageData *sp = (vtkImageData*) m_Dataset;
      labelScalars = sp->GetPointData()->GetScalars();
      volumeScalars = sp->GetPointData()->GetScalars();  
    }
    else if ( m_Dataset->IsA( "vtkRectilinearGrid" ) )
    {    
      vtkRectilinearGrid *rg = (vtkRectilinearGrid*) m_Dataset;
      labelScalars = rg->GetPointData()->GetScalars();
      volumeScalars = rg->GetPointData()->GetScalars();  
    }
    int numberC = m_TagLabel->GetNumberOfComponents();
    bool lastComponent = false;
    int numberChecked = 0;

    std::vector<int> minVector;
    std::vector<int> maxVector;
    std::vector<int> labelIntVector;

    int counter= 0;
    //Fill the vectors of range and label value
    for (int c = 0; c < m_CheckedVector.size(); c++)
    {
      if (m_CheckedVector.at(c))
      {
        wxString label = m_LabelNameVector.at(c);
        wxStringTokenizer tkz(label,wxT(' '),wxTOKEN_RET_EMPTY_ALL);
        albaString labelName = tkz.GetNextToken();
        albaString labelIntStr = tkz.GetNextToken();
        m_LabelIntValue = atoi(labelIntStr);
        labelIntVector.push_back(m_LabelIntValue);
        albaString minStr = tkz.GetNextToken();
        m_MinValue = atof(minStr);
        minVector.push_back(m_MinValue);
        albaString maxStr = tkz.GetNextToken();
        m_MaxValue = atof(maxStr);
        maxVector.push_back(m_MaxValue);
        counter++;
      }
    }
    
    int labelVlaue;
    if (counter != 0)
    {
      int not = volumeScalars->GetNumberOfTuples();
      for ( int i = 0; i < not; i++ )
      {
        bool modified = false;
        double scalarValue = volumeScalars->GetComponent( i, 0 );
        for (int c = 0; c < labelIntVector.size(); c++)
        {
          if ( scalarValue >= minVector.at(c) && scalarValue <= maxVector.at(c))
          { 
            labelVlaue = labelIntVector.at(c);
            labelScalars->SetTuple1( i, labelVlaue ); 
            modified = true;
          }
        }
        if (!modified)
        {
          labelScalars->SetTuple1( i, OUTRANGE_SCALAR); 
        }
      }
      
      labelScalars->Modified();
      m_Dataset->GetPointData()->SetScalars(labelScalars);
      m_Dataset->Modified();
    }
    else
    {
      int not = volumeScalars->GetNumberOfTuples();
      for ( int i = 0; i < not; i++ )
      {
        labelScalars->SetTuple1( i, OUTRANGE_SCALAR); 
      }
      labelScalars->Modified();
      m_Dataset->GetPointData()->SetScalars(labelScalars);
      m_Dataset->Modified();
    }

    double scalarRange[2];
    labelScalars->GetRange(scalarRange);

    mmaVolumeMaterial *labelMaterial = ((albaVMEOutputVolume *)this->GetOutput())->GetMaterial();
    labelMaterial->m_ColorLut->SetTableRange(scalarRange);
    labelMaterial->UpdateFromTables();

    ForwardUpEvent(&albaEvent(this, CAMERA_UPDATE));
  }
}

//-----------------------------------------------------------------------
void albaVMELabeledVolume::RetrieveTag()
//----------------------------------------------------------------------
{ 
  bool tagPresent = this->GetTagArray()->IsTagPresent("LABELS");
  if (!tagPresent)
  {
    albaTagItem tag_Item;
    tag_Item.SetName("LABELS");
    this->GetTagArray()->SetTag(tag_Item);
  }
  m_TagLabel = new albaTagItem;
  m_TagLabel = this->GetTagArray()->GetTag( "LABELS" );
}

//-----------------------------------------------------------------------
int albaVMELabeledVolume::InternalStore(albaStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==ALBA_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
    return ALBA_OK;
  }
  return ALBA_ERROR;
}
//-----------------------------------------------------------------------
int albaVMELabeledVolume::InternalRestore(albaStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==ALBA_OK)
  {
    albaMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==ALBA_OK)
    {
      m_Transform->SetMatrix(matrix);
      return ALBA_OK;
    }
  }

  return ALBA_ERROR;
}

//----------------------------------------------------------------------------
albaGUI* albaVMELabeledVolume::CreateGui()
//----------------------------------------------------------------------------
{
  m_VolumeLink = GetVolumeLink();
  if (m_VolumeLink)
  {
    CopyDataset();
  }
 
  // GUI
  m_Gui = albaVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this); 

  // Settings for the buttons that will control the list box
	m_Gui->TwoButtons(ID_INSERT_LABEL, ID_REMOVE_LABEL, "Add Label", "Remove Label");
	m_Gui->Button(ID_EDIT_LABEL, _("Edit label"), "", _("Edit a label"));

  if (m_VolumeLink)
  {
    EnableWidgets(true);
  }

  m_Gui->Divider(2);  
  m_LabelCheckBox = m_Gui->CheckList(ID_LABELS,_("Labels"),360,_("Chose label to visualize"));
 
  typedef std::list< wxString > LIST;
  LIST myList;     
  LIST::iterator myListIter;   

  // If there already is a tag named "LABELS" then I have to load the old labels in the correct position in the listbox

  if (m_VolumeLink)
  {
    int noc = m_TagLabel->GetNumberOfComponents();
    if(noc != 0)
    {
      for ( unsigned int i = 0; i < noc; i++ )
      {
        wxString label = m_TagLabel->GetValue( i );
        if ( label != "" )
        {
          myList.push_back( label );
        }
      }     

      for( myListIter = myList.begin(); myListIter != myList.end(); myListIter++ )
      {
        for ( unsigned int j = 0; j < noc; j++ )
        {
          wxString component = m_TagLabel->GetValue( j );
          if ( component != "" )
          {
            wxString labelName = *myListIter;
            if ( component == labelName )
            {
              m_LabelCheckBox->AddItem(m_CheckListId, component, false);
              FillLabelVector(component, false);
              m_CheckListId++;
            }
          }
        }  
      }
    }
  }
    
  m_Gui->Update(); 
  
  return m_Gui;
}

//----------------------------------------------------------------------------
void albaVMELabeledVolume::FillLabelVector(wxString name, bool checked)
//----------------------------------------------------------------------------
{
  m_LabelNameVector.push_back(name);
  m_CheckedVector.push_back(checked);
}

//----------------------------------------------------------------------------
void albaVMELabeledVolume::ModifyLabelVector(int n, wxString name, bool checked)
//----------------------------------------------------------------------------
{
  if (n < m_LabelNameVector.size() )
  {
    m_LabelNameVector[n] = name;
    m_CheckedVector[n] = checked;
  }
  else
    return;
}

//----------------------------------------------------------------------------
void albaVMELabeledVolume::RemoveItemLabelVector(int n)
//----------------------------------------------------------------------------
{
  if (n < m_LabelNameVector.size() )
  {
    m_LabelNameVector.erase(m_LabelNameVector.begin() + n);
    m_CheckedVector.erase(m_CheckedVector.begin() + n);
  }
  else
    return;
}

//----------------------------------------------------------------------------
void albaVMELabeledVolume::CreateOpDialog()
//----------------------------------------------------------------------------
{
  double b[6];
  m_Dataset->GetBounds(b);
  m_SliceMin = b[4];
  m_SliceMax = b[5];
  m_Slice = ( m_SliceMin + m_SliceMax) * 0.5;  

  int x_pos,y_pos,w,h;
  albaGetFrame()->GetPosition(&x_pos,&y_pos);
  m_Dlg = new albaGUIDialogPreview("Insert Label",albaCLOSEWINDOW | albaRESIZABLE | albaUSERWI);
  m_Dlg->GetSize(&w,&h);
  m_Dlg->SetSize(x_pos+5,y_pos+5,w,h); 

  m_Rwi = m_Dlg->GetRWI();
  //m_Rwi->m_RwiBase->SetMouse(m_Mouse);

  wxPoint p = wxDefaultPosition;

  // slice interface
  wxStaticText *lab_slice  = new wxStaticText(m_Dlg,-1, "slice position: ");
  wxStaticText *foo_slice  = new wxStaticText(m_Dlg,-1, "");
  wxTextCtrl   *text_slice = new wxTextCtrl  (m_Dlg,ID_SLICE, "",	p, wxSize(50, 16 ), wxNO_BORDER );
  m_SliceSlider						 = new albaGUIFloatSlider( m_Dlg, ID_SLICE_SLIDER, m_Slice, m_SliceMin, m_SliceMax, p, wxSize( 235, 20 ) );
  m_SliceSlider->SetNumberOfSteps( 200 );
  m_SliceStep = m_SliceSlider->GetStep();
  m_SliceMin  = m_SliceSlider->GetMin();
  m_SliceMax  = m_SliceSlider->GetMax();
  m_Slice     = m_SliceSlider->GetValue();

  albaGUIButton * b_incr_slice  = new albaGUIButton(m_Dlg,ID_INCREASE_SLICE, ">", p, wxSize( 25, 20) );
  albaGUIButton * b_decr_slice  = new albaGUIButton(m_Dlg,ID_DECREASE_SLICE, "<", p, wxSize( 25, 20) );

  // slice interface validator
  text_slice->SetValidator(albaGUIValidator(this,ID_SLICE,text_slice,&m_Slice,m_SliceMin,m_SliceMax));
  m_SliceSlider->SetValidator(albaGUIValidator(this,ID_SLICE_SLIDER,m_SliceSlider,&m_Slice,text_slice));
  b_incr_slice->SetValidator(albaGUIValidator(this,ID_INCREASE_SLICE,b_incr_slice));
  b_decr_slice->SetValidator(albaGUIValidator(this,ID_DECREASE_SLICE,b_decr_slice));

  // slice sizer
  wxBoxSizer * h_sizer2= new wxBoxSizer( wxHORIZONTAL );
  h_sizer2->Add(lab_slice,     0, wxLEFT);	
  h_sizer2->Add(text_slice,    0, wxLEFT);	
  h_sizer2->Add(b_decr_slice,  0, wxLEFT);
  h_sizer2->Add(m_SliceSlider, 1, wxEXPAND);
  h_sizer2->Add(b_incr_slice,  0, wxLEFT);	

  wxStaticText * lab_labelName  = new wxStaticText( m_Dlg, -1, "Label name: ");
  m_LabelNameCtrl = new wxTextCtrl  ( m_Dlg, ID_D_LABEL_NAME, m_LabelNameValue, p, wxSize( 100, 16 ), wxNO_BORDER );    
  wxStaticText * lab_labelValue  = new wxStaticText( m_Dlg, -1, "Label value: ");
  m_LabelValueCtrl = new wxTextCtrl  ( m_Dlg, ID_D_LABEL_VALUE, m_LabelValueValue, p, wxSize( 100, 16 ), wxNO_BORDER );       
  albaGUIButton  * bFit   = new albaGUIButton(m_Dlg,ID_FIT, "reset camera", p, wxSize(80,20));  

  // other controls validator  
  m_LabelValueCtrl->SetValidator( albaGUIValidator( this, ID_D_LABEL_VALUE, m_LabelValueCtrl, &m_LabelIntValue, m_MinMin, m_MaxMax ) );
  bFit->SetValidator(albaGUIValidator(this,ID_FIT,bFit));  

  // other controls sizer
  wxBoxSizer * h_sizer3 = new wxBoxSizer( wxHORIZONTAL );
  h_sizer3->Add(lab_labelName, 0, wxLEFT );	
  h_sizer3->Add(m_LabelNameCtrl, 0, wxLEFT );
  h_sizer3->Add(lab_labelValue, 0, wxLEFT );	
  h_sizer3->Add(m_LabelValueCtrl, 0, wxLEFT );  
  h_sizer3->Add(foo_slice,     1,wxEXPAND);	
  h_sizer3->Add(bFit,   0,wxRIGHT);  

  // Min and Max interface
  wxStaticText * lab_min  = new wxStaticText( m_Dlg, -1, "Min: ");
  wxStaticText * minFooText  = new wxStaticText( m_Dlg, -1, "");
  wxTextCtrl   * text_min = new wxTextCtrl  ( m_Dlg, ID_D_MIN, "",	p, wxSize(50, 16 ), wxNO_BORDER );  
  m_MinSlider  = new wxSlider( m_Dlg, ID_SLIDER_MIN, m_Min, m_MinMin, m_MinMax, p, wxSize( 235, 20 ) );
  m_MinMin  = m_MinSlider->GetMin();
  m_MinMax  = m_MinSlider->GetMax();
  m_Min     = m_MinSlider->GetValue();

  wxStaticText * lab_max  = new wxStaticText( m_Dlg, -1, "Max: ");
  wxStaticText * maxFooText  = new wxStaticText( m_Dlg, -1, "");
  wxTextCtrl   * text_max = new wxTextCtrl  ( m_Dlg, ID_D_MAX, "",	p, wxSize(50, 16 ), wxNO_BORDER );
  m_MaxSlider	= new wxSlider ( m_Dlg, ID_SLIDER_MAX, m_Max, m_MaxMin, m_MaxMax, p, wxSize( 235, 20 ) );
  m_MaxMin  = m_MaxSlider->GetMin();
  m_MaxMax  = m_MaxSlider->GetMax();
  m_Max     = m_MaxSlider->GetValue();

  // Min and Max interface validator
  m_MinSlider->SetValidator( albaGUIValidator( this, ID_SLIDER_MIN, m_MinSlider, &m_Min, text_min ) );
  text_min->SetValidator( albaGUIValidator( this,ID_D_MIN, text_min, &m_Min, m_MinSlider, m_MinMin, m_MinMax ) );  
  m_MaxSlider->SetValidator(albaGUIValidator(this,ID_SLIDER_MAX, m_MaxSlider, &m_Max, text_max ) );
  text_max->SetValidator(albaGUIValidator(this,ID_D_MAX, text_max, &m_Max, m_MaxMin, m_MaxMax ) );

  // Min and Max sizer
  wxBoxSizer * h_sizer5 = new wxBoxSizer( wxHORIZONTAL );
  h_sizer5->Add( lab_min,     0, wxLEFT);	
  h_sizer5->Add( text_min,    0, wxLEFT);	  
  h_sizer5->Add( m_MinSlider, 1, wxEXPAND);

  wxBoxSizer * h_sizer6 = new wxBoxSizer( wxHORIZONTAL );
  h_sizer6->Add( lab_max,     0, wxLEFT);	
  h_sizer6->Add( text_max,    0, wxLEFT);	  
  h_sizer6->Add( m_MaxSlider, 1, wxEXPAND);  

  albaGUIButton  * b_ok    = new albaGUIButton( m_Dlg, ID_OK, _("ok"), p, wxSize( 200, 20 ) );
  albaGUIButton  * b_cancel= new albaGUIButton( m_Dlg, ID_CANCEL, _("cancel"), p, wxSize( 200, 20 ) );
  b_ok->SetValidator( albaGUIValidator( this, ID_OK, b_ok) );
  b_cancel->SetValidator( albaGUIValidator( this, ID_CANCEL, b_cancel) );
  wxBoxSizer *h_sizer4 = new wxBoxSizer(wxHORIZONTAL);
  h_sizer4->Add( b_ok,     0, wxRIGHT);
  h_sizer4->Add( b_cancel, 0, wxRIGHT);  

 // wxBoxSizer * main_sizer = new wxBoxSizer( wxVERTICAL ); 
  m_Dlg->Add( h_sizer2 );
  m_Dlg->Add( h_sizer3 );  
  m_Dlg->Add( h_sizer5 );  
  m_Dlg->Add( h_sizer6 ); 
  m_Dlg->Add( h_sizer4 );

  CreateSlicePipeline();  
  UpdateLookUpTable();
  this->m_Rwi->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaVMELabeledVolume::CreateSlicePipeline()
//----------------------------------------------------------------------------
{
  // slicing the volume
  double srange[2],w,l, xspc = 0.33, yspc = 0.33;

  double min = m_MinAbsolute;
  double max = m_MaxAbsolute;

  m_Dataset->GetBounds(m_Bounds);
  m_Dataset->GetScalarRange(srange);
  w = srange[1] - srange[0];
  l = (srange[1] + srange[0]) * 0.5;

  m_Dataset->GetCenter(m_Origin);
  m_Slice = m_Origin[2];
  m_XVect[0] = 1.0;
  m_XVect[1] = 0.0;
  m_XVect[2] = 0.0;
  m_YVect[0] = 0.0;
  m_YVect[1] = 1.0;
  m_YVect[2] = 0.0;

  m_LookUpTable = vtkWindowLevelLookupTable::New();  
  m_LookUpTable->SetTableRange( srange[0], srange[1] );
  m_LookUpTable->SetWindow( srange[1] - srange[0] );
  float level = ( srange[1] + srange[0] ) / 2.0;
  m_LookUpTable->SetLevel( level );
  m_LookUpTable->Build();

  m_LookUpTableColor = vtkWindowLevelLookupTable::New();
  m_ProbeFilter = vtkProbeFilter::New(); 

  // Get the volume dimensions
  int dimensions[3];
  if ( m_Dataset->IsA( "vtkImageData" ) )
  {
    vtkImageData *sp = (vtkImageData*) m_Dataset;
    sp->GetDimensions( dimensions );

    // Compute the spacing values
    float xSpacing = ( m_Bounds[1] - m_Bounds[0] ) / ( dimensions[0] - 1 );
    float ySpacing = ( m_Bounds[3] - m_Bounds[2] ) / ( dimensions[1] - 1 );
    
    // Create vtkImageData with dimensions and spacing computed before
    m_SP = vtkImageData::New();
    m_SP->SetDimensions( dimensions[0], dimensions[1], 1 );  
    m_SP->SetSpacing( xSpacing, ySpacing, 1.0f);
    m_SP->SetOrigin( m_Bounds[0], m_Bounds[2], m_Slice );
    m_SP->Modified();

    // Create a probe filter with the volume as source and a structuredPoints as input
    m_ProbeFilter->SetSourceData( sp );
    m_ProbeFilter->SetInputData( m_SP );
  }
  else if ( m_Dataset->IsA( "vtkRectilinearGrid" ) )
  {    
    vtkRectilinearGrid *rg = (vtkRectilinearGrid*) m_Dataset;
    rg->GetDimensions( dimensions ); 

    // Compute the spacing values
    float xSpacing = ( m_Bounds[1] - m_Bounds[0] ) / ( dimensions[0] - 1 );
    float ySpacing = ( m_Bounds[3] - m_Bounds[2] ) / ( dimensions[1] - 1 );

    // Create vtkImageData with dimensions and spacing computed before
    m_SP = vtkImageData::New();
    m_SP->SetDimensions( dimensions[0], dimensions[1], 1 );  
    m_SP->SetSpacing( xSpacing, ySpacing, 1.0f);
    m_SP->SetOrigin( m_Bounds[0], m_Bounds[2], m_Slice );
    m_SP->Modified();

    // Create a probe filter with the volume as source and a structuredPoints as input
    m_ProbeFilter->SetSourceData( rg );
    m_ProbeFilter->SetInputData( m_SP );
  }

  // Create a texture with the default m_LookUpTable and with input the probeFilter output
  m_Texture = vtkTexture::New();	
  m_Texture->SetInputConnection( m_ProbeFilter->GetOutputPort() );
  m_Texture->InterpolateOn();
  m_Texture->MapColorScalarsThroughLookupTableOn();//
  m_Texture->SetLookupTable( m_LookUpTable );

  vtkPlaneSource * planeSource = vtkPlaneSource::New();
  planeSource->SetOrigin( m_Bounds[0], m_Bounds[3], m_Slice );
  planeSource->SetPoint1( m_Bounds[1], m_Bounds[3], m_Slice );
  planeSource->SetPoint2( m_Bounds[0], m_Bounds[2], m_Slice );
  
  float xCenter = ( m_Bounds[1] + m_Bounds[0] ) / 2.0;
  float yCenter = ( m_Bounds[3] + m_Bounds[2] ) / 2.0;
  planeSource->SetCenter( xCenter, yCenter, m_Slice );

  m_SMapper	= vtkPolyDataMapper::New();
  m_SMapper->SetInputConnection( planeSource->GetOutputPort() );
  vtkDEL( planeSource );

  m_ActorSlice= vtkActor::New();
  m_ActorSlice->SetMapper( m_SMapper );
  m_ActorSlice->SetTexture( m_Texture );
  m_ActorSlice->GetProperty()->SetAmbient(1.f);
  m_ActorSlice->GetProperty()->SetDiffuse(0.f);
  m_ActorSlice->PickableOff();

  m_Rwi->m_RenFront->AddActor(m_ActorSlice);
  m_Rwi->m_RenFront->ResetCamera(); 
}

//----------------------------------------------------------------------------
void albaVMELabeledVolume::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {	
      case ID_INSERT_LABEL:
      {     
        UpdateScalars();
        if (m_VolumeLink)
        {
          m_EditMode = false;
          double sr[2];
          m_Dataset->GetScalarRange(sr);
          m_MinAbsolute = sr[0]; 
          m_MaxAbsolute = sr[1]; 

          // To initialize slider values
          m_MinMin = m_MinAbsolute;
          m_MinMax = m_MaxAbsolute;
          m_Min = ( m_MinMin + m_MinMax ) * 0.5;
          m_MaxMin = m_MinAbsolute;  
          m_MaxMax = m_MaxAbsolute;
          m_Max = ( m_MaxMin + m_MaxMax ) * 0.5;
          m_LabelNameValue = wxEmptyString;
          m_LabelIntValue = 1;
          CreateOpDialog();
          m_Dlg->ShowModal();      
        }
      }
      break;
      case ID_REMOVE_LABEL:
      {
        m_LabelCheckBox->RemoveItem(m_ItemSelected);
        RemoveItemLabelVector(m_ItemSelected);
        int noc = m_TagLabel->GetNumberOfComponents();
        for ( unsigned int w = 0; w < noc; w++ )
        {
          wxString component = m_TagLabel->GetValue( w );
          if ( m_ItemLabel == component )
          {
            RemoveLabelTag(w);
          }
        }
        EnableWidgets();
        GenerateLabeledVolume();
      }
      break;
      case ID_EDIT_LABEL:
      {
        if (m_VolumeLink)
        {
          m_EditMode = true;
          wxString componentName;
          int noc = m_TagLabel->GetNumberOfComponents();
          for ( unsigned int w = 0; w < noc; w++ )
          {
            componentName = m_TagLabel->GetValue( w );
            if ( m_ItemLabel == componentName )
            {
              wxStringTokenizer tkz(componentName,wxT(' '),wxTOKEN_RET_EMPTY_ALL);
              albaString labelName = tkz.GetNextToken();
              albaString labelIntStr = tkz.GetNextToken();
              int labelValue = atof(labelIntStr);
              albaString minStr = tkz.GetNextToken();
              double min = atof(minStr);
              albaString maxStr = tkz.GetNextToken();
              double max = atof(maxStr);

              UpdateScalars();
              double sr[2];
              m_Dataset->GetScalarRange(sr);
              m_MinAbsolute = sr[0]; 
              m_MaxAbsolute = sr[1]; 

              // To initialize slider values
              m_MinMin = m_MinAbsolute;
              m_MinMax = m_MaxAbsolute;
              m_MaxMin = m_MinAbsolute;  
              m_MaxMax = m_MaxAbsolute;
         
              m_LabelNameValue = labelName;
              m_LabelValueValue = labelIntStr;
              m_LabelIntValue = labelValue;
              m_Min = min;
              m_Max = max;
              CreateOpDialog();
              m_Dlg->ShowModal();  
            }        
          }
        }  
        else
          break;
      }
      break;
      case ID_LABELS:
      {
        int itemId = e->GetArg();
        m_ItemLabel = m_LabelCheckBox->GetItemLabel(itemId);
        m_ItemSelected = m_LabelCheckBox->FindItemIndex(itemId);
        for (int i = 0; i < m_CheckedVector.size(); i++)
        {
          if (m_CheckedVector.at(i) != m_LabelCheckBox->IsItemChecked(i))
          {
            m_CheckedVector[i] = m_LabelCheckBox->IsItemChecked(i);
            GenerateLabeledVolume();
            InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);
            break;
          }
        } 
      }
      break;
      case ID_D_MAX:
      {  
        m_Dlg->TransferDataToWindow();  
        m_Max = m_MaxSlider->GetValue();
        m_Dlg->TransferDataToWindow();  
        //wxLogMessage("max | m_CheckMax %d | m_Max %f | m_MaxValue %f", m_CheckMax, m_Max, m_MaxValue );

        if ( m_CheckMax  && m_Max >= m_MaxValue )      
        {
          wxMessageBox( "You have inserted a Max value bigger than the value permitted", "Error", wxOK | wxICON_ERROR  );       
          m_Max = m_MaxValue; m_MaxSlider->SetValue( m_Max ); m_Dlg->TransferDataToWindow();       
        }  

        UpdateLookUpTable();
      }
      break;
      case ID_D_MIN:
      {
        m_Dlg->TransferDataToWindow();  
        m_Min = m_MinSlider->GetValue();   
        m_Dlg->TransferDataToWindow();
      
        if ( m_CheckMin  && m_Min <= m_MinValue )      
        {
          wxMessageBox( "You have inserted a Min value smaller than the value permitted", "Error", wxOK | wxICON_ERROR  );                
          m_Min = m_MinValue; m_MinSlider->SetValue( m_Min ); m_Dlg->TransferDataToWindow(); 
        }      
        UpdateLookUpTable();
      }
      break; 
      case ID_SLIDER_MIN:      
      {       
        if ( m_CheckMin  && m_Min <= m_MinValue ) 
        {
          m_Min = m_MinValue; m_MinSlider->SetValue( m_Min ); 
        }      
        UpdateLookUpTable();
      }
      break;
      case ID_SLIDER_MAX:      
      {      
        if ( m_CheckMax  && m_Max >= m_MaxValue )      
        {
          m_Max = m_MaxValue; m_MaxSlider->SetValue( m_Max ); 
        }
        UpdateLookUpTable();
      }      
      break;
      case ID_OK:
        EnableWidgets();
        UpdateLabel();
      break;
      case ID_CANCEL:
        {
         // UpdateLabel();
          GenerateLabeledVolume();
          m_Dlg->EndModal(wxID_CANCEL);
        }
        break;
      case ID_FIT:
        m_Rwi->m_RenFront->ResetCamera(m_BBox);
        m_Rwi->m_Camera->Dolly(1.2);
        m_Rwi->m_RenFront->ResetCameraClippingRange();
        m_Rwi->m_RenFront->ResetCamera();
        m_Rwi->m_RenderWindow->Render();
      break;    
      case ID_SLICE:
        m_Dlg->TransferDataToWindow();  
        m_Slice = m_SliceSlider->GetValue();
        m_Dlg->TransferDataToWindow();  
        UpdateSlice();
      break;
      case ID_SLICE_SLIDER:
        UpdateSlice();
      break;
      case ID_INCREASE_SLICE:
        if(m_Slice<m_SliceMax) m_Slice += m_SliceStep;
        m_Dlg->TransferDataToWindow();  
        UpdateSlice();
      break;
      case ID_DECREASE_SLICE:
        if(m_Slice>m_SliceMin) m_Slice -= m_SliceStep;
        m_Dlg->TransferDataToWindow();  
        UpdateSlice();
      break;
      case VME_PICKED:
      {
        vtkDataSet *vol = m_VolumeLink->GetOutput()->GetVTKData();
        double pos[3];
        vtkPoints *pts = NULL; 
        pts = (vtkPoints *)e->GetVtkObj();
        pts->GetPoint(0,pos);
        int pid = vol->FindPoint(pos);
        vtkDataArray *scalars = vol->GetPointData()->GetScalars();

      }
      break;
      default:
        albaVME::OnEvent(alba_event);
    }
  }
  else
  {
    Superclass::OnEvent(alba_event);
  }
}
//----------------------------------------------------------------------------
void albaVMELabeledVolume::UpdateLabel()
//----------------------------------------------------------------------------
{      
  if ( m_Max < m_Min )      
  {
    wxMessageBox( "Wrong values for Min and Max parameters", "Error", wxOK | wxICON_ERROR );
    return;
  }
  wxString labelName = "";

  if (m_LabelNameCtrl) //In test mode this is not present
  {
    labelName = m_LabelNameCtrl->GetValue();
  }
 
  if ( labelName == wxEmptyString )
    labelName = "no_name";
  else
    labelName.Replace( " ", "_" );

  // Check if another name for the label already exists
  if( m_TagLabel && m_EditMode == false)
  {
    int noc = m_TagLabel->GetNumberOfComponents();
    for ( unsigned int i = 0; i < noc; i++ )
    {
      wxString component = m_TagLabel->GetValue( i );

      // Check if another name for the label already exists
      wxString currentLabelName = component.BeforeFirst( ' ' );       
      if ( currentLabelName == labelName )
      {          
        int answer = wxMessageBox( "The name of this label already exists - Continue?", "Warning", wxYES_NO | wxICON_ERROR, NULL);
        if (answer == wxNO)
        {            
          m_LabelNameCtrl->SetValue( wxEmptyString );            
          return;
        }
        else
          break; //exit from loop for only
      }
      // Check if another value for the label already exists
      int currentLabelValue = GetLabelValue( currentLabelName );
      if ( currentLabelValue == m_LabelIntValue )
      {
        int answer = wxMessageBox( "The value for this label already exists - Continue?", "Warning", wxYES_NO | wxICON_ERROR, NULL);
        if (answer == wxNO)
        {            
          m_LabelIntValue = 1;
          m_Dlg->TransferDataToWindow();
          return;
        }
        else
          break; //exit from loop for only
      }
    }
  }
  wxString labelLine; 
  labelLine.Printf( "%s %d %d %d", labelName, m_LabelIntValue, m_Min, m_Max ); 

  if (m_Dlg)
  {
    m_Dlg->EndModal(wxID_CANCEL);
  }
   
  if (m_EditMode == false)
  {
    m_LabelCheckBox->AddItem(m_CheckListId, labelLine, true); 
    FillLabelVector(labelLine, true);
    m_CheckListId++;
    m_LabelCheckBox->Update();
    int nComp = m_TagLabel->GetNumberOfComponents();
    SetLabelTag(labelLine, nComp);
  }
  else
  {
    m_LabelCheckBox->SetItemLabel(m_ItemSelected, labelLine);
    m_LabelCheckBox->CheckItem(m_ItemSelected, true);
    ModifyLabelVector(m_ItemSelected, labelLine, true);
    m_LabelCheckBox->Update();

    int noc = m_TagLabel->GetNumberOfComponents();
    for ( unsigned int w = 0; w < noc; w++ )
    {
      wxString componentName = m_TagLabel->GetValue( w );
      if ( m_ItemLabel == componentName )
      {
        SetLabelTag(labelLine, w );
      }
    }
  }
  GenerateLabeledVolume();
}

//----------------------------------------------------------------------------
void albaVMELabeledVolume::SetLabelTag(albaString label, int component )
//----------------------------------------------------------------------------
{
  m_TagLabel->SetValue(label, component);
}

//----------------------------------------------------------------------------
void albaVMELabeledVolume::RemoveLabelTag(int component)
//----------------------------------------------------------------------------
{
  m_TagLabel->RemoveValue(component);
}

//----------------------------------------------------------------------------
 void albaVMELabeledVolume::UpdateSlice()
//----------------------------------------------------------------------------
{
  m_Origin[2] = m_Slice;
  m_SP->SetOrigin( m_Bounds[0], m_Bounds[2], m_Origin[2] );
  m_ProbeFilter->Update();

  this->m_Rwi->m_RenFront->ResetCameraClippingRange();
  m_Rwi->m_RwiBase->Render();
  m_Rwi->m_RenderWindow->SetDesiredUpdateRate(15.f);  
}

//-------------------------------------------------------------------------
mmaVolumeMaterial * albaVMELabeledVolume::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaVolumeMaterial *material = (mmaVolumeMaterial *)GetAttribute("VolumeMaterialAttributes");
  if (material == NULL)
  {
    material = mmaVolumeMaterial::New();
    SetAttribute("VolumeMaterialAttributes", material);
    if (m_Output)
    {
      ((albaVMEOutputVolume *)m_Output)->SetMaterial(material);
    }
  }
  return material;
}
//----------------------------------------------------------------------------
int albaVMELabeledVolume::GetLabelValue( wxString &item )
//----------------------------------------------------------------------------
{
  wxString tmp = item.AfterFirst( ' ' );  
  wxString value = tmp.BeforeFirst( ' ' );  
  long v = 0;
  value.ToLong( &v );   

  return (int)v;  
}
//----------------------------------------------------------------------------
int albaVMELabeledVolume::GetMin( wxString &item )
//----------------------------------------------------------------------------
{
  wxString tmp = item.AfterFirst( ' ' );   
  tmp = tmp.AfterFirst( ' ' );
  wxString min = tmp.BeforeFirst( ' ' );
  long m = 0;
  min.ToLong( &m );   

  return (int)m;  
}

//----------------------------------------------------------------------------
int albaVMELabeledVolume::GetMax( wxString &item )
//----------------------------------------------------------------------------
{
  wxString tmp = item.AfterFirst( ' ' );    
  tmp = tmp.AfterFirst( ' ' );    
  tmp = tmp.AfterFirst( ' ' );  
  wxString max = tmp.BeforeFirst( ' ' );
  long M = 0;  
  max.ToLong( &M );    

  return (int)M;
}

//----------------------------------------------------------------------------
void albaVMELabeledVolume::UpdateLookUpTable()
//----------------------------------------------------------------------------
{ 
  m_LookUpTableColor->DeepCopy( m_LookUpTable );
  for ( int i = m_Min; i <= m_Max; i++ )
  {
    vtkIdType index = m_LookUpTableColor->GetIndex( i );                
    m_LookUpTableColor->SetTableValue( index, 1.0f, 0.0f, 0.0f );
  }
  m_Texture->SetLookupTable( m_LookUpTableColor );    
  m_Rwi->m_RwiBase->Render(); 
}  

//----------------------------------------------------------------------------
void albaVMELabeledVolume::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  if (m_Gui)
  {
    if (!enable || !m_VolumeLink)
    {
      m_Gui->Enable(ID_INSERT_LABEL,enable);
      m_Gui->Enable(ID_REMOVE_LABEL,enable);
      m_Gui->Enable(ID_EDIT_LABEL,enable);
    }
    else
    {
      int noc = m_TagLabel->GetNumberOfComponents();
      bool labelPresent = noc != 0;
      m_Gui->Enable(ID_INSERT_LABEL,enable);
      m_Gui->Enable(ID_REMOVE_LABEL,labelPresent);
      m_Gui->Enable(ID_EDIT_LABEL,labelPresent);
    }
    m_Gui->Update();
  }
}
//-------------------------------------------------------------------------
char** albaVMELabeledVolume::GetIcon() 
//-------------------------------------------------------------------------
{
#include "albaVMEVolume.xpm"
  return albaVMEVolume_xpm;
}

//-------------------------------------------------------------------------
bool albaVMELabeledVolume::IsDataAvailable()
//-------------------------------------------------------------------------
{
  albaVME *volume = GetVolumeLink();
  if (volume != NULL)
  {
    return volume->IsDataAvailable();
  }
  return false;
}
