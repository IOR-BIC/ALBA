/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMeshDeformation
 Authors: Josef Kohout
 
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

#include "albaOpMeshDeformation.h"
#include "wx/busyinfo.h"

#include "albaGUIDialog.h"
#include "albaRWIBase.h"
#include "albaRWI.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaEvent.h"

#include "albaGUIButton.h"
#include "albaGUIValidator.h"
#include "albaVMESurface.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaVME.h"
#include "albaVME.h"
#include "albaVMEPolyLine.h"
#include "albaVMEPolylineGraph.h"
#include "albaPolylineGraph.h"

#include "vtkRenderWindow.h"
#include "vtkPolyDataMapper.h"
#include "vtkAppendPolyData.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
#include "vtkGlyph3D.h"
#include "vtkTubeFilter.h"
#include "vtkSphereSource.h"
#include "vtkProperty.h"
#include "vtkCellArray.h"
#include "vtkCellPicker.h"

#include "vtkMath.h"
#include "vtkALBAPolyDataDeformation.h"
#include "vtkALBAPolyDataDeformation_M1.h"
#include "vtkALBAPolyDataDeformation_M2.h"

#ifdef DEBUG_albaOpMeshDeformation
#include "vtkCharArray.h"
#include "vtkPointData.h"
#include "vtkLookupTable.h"
#include "vtkColorTransferFunction.h"
#include "vtkMassProperties.h"
#endif

#include "albaMemDbg.h"
#include "albaDbg.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpMeshDeformation);
//----------------------------------------------------------------------------

#define CHANGE_OC   1
#define CHANGE_DC   2
#define CHANGE_CC   4
#define CHANGE_ALL  CHANGE_OC | CHANGE_DC | CHANGE_CC 
#define MAKE_ACTION(edm_code, change_flags) ((change_flags) << 16 | (edm_code))
#define GET_EDM_CODE(action) ((short)((action) & 0xFFFF))
#define GET_CHANGE_FLAGS(action) ((action) >> 16)

#define SELECT_NONE     -1
#define SELECT_OC       0
#define SELECT_DC       1
#define SELECT_OC_VERT  2
#define SELECT_DC_VERT  3
#define SELECT_CC_COR   4

//----------------------------------------------------------------------------
albaOpMeshDeformation::albaOpMeshDeformation(const wxString &label) : albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo	= false;  
  m_InputPreserving = true;	

  m_NumberOfCurves = 0;
  m_OriginalCurves = NULL;
  m_DeformedCurves = NULL;
  m_CurvesCorrespondence = NULL;

  m_OCToAdd = m_DCToAdd = NULL;
  for (int i = 0; i < 2; i++)
  {
    m_Meshes[i] = NULL;
    m_Spheres[i] = NULL;
    m_MeshesVisibility[i] = 1;
    m_CurvesVisibility[i] = 1;
  }
  m_CurvesVisibility[2] = 1;

  m_SphereRadius[0] = 75;
  m_SphereRadius[1] = 100;

  m_EditMode = 0;
  m_Picker = NULL;
  m_SelectedCurve = NULL;
  m_SelectedObjType = -1;
  m_SelectedObjId = -1;

  m_DeformationMode = 2;
  m_SaveODC[0] = m_SaveODC[1] = 0;

  m_BPointMoveActive = false;
  m_BCorrespondenceActive = false;
  m_BDoNotCreateUndo = false;
}
//----------------------------------------------------------------------------
albaOpMeshDeformation::~albaOpMeshDeformation()
//----------------------------------------------------------------------------
{  
  for (int i = 0; i < m_NumberOfCurves; i++)
  {
    albaDEL(m_OriginalCurves[i]);
    albaDEL(m_DeformedCurves[i]);
    vtkDEL(m_CurvesCorrespondence[i]);
  }

  delete[] m_DeformedCurves;
  delete[] m_OriginalCurves;
  delete[] m_CurvesCorrespondence;

  m_DeformedCurves = NULL;
  m_OriginalCurves = NULL;
  m_CurvesCorrespondence = NULL;

  albaDEL(m_Output);
}
//----------------------------------------------------------------------------
bool albaOpMeshDeformation::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsA("albaVMESurface"));
}
//----------------------------------------------------------------------------
albaOp *albaOpMeshDeformation::Copy()   
//----------------------------------------------------------------------------
{
  return (new albaOpMeshDeformation(m_Label));
}

//----------------------------------------------------------------------------
void albaOpMeshDeformation::OpRun()   
//----------------------------------------------------------------------------
{ 
  //create internal structures for the visualization
  if (!CreateInternalStructures())
  {
    albaEventMacro(albaEvent(this, OP_RUN_CANCEL)); 
    return;
  }
 
  // interface:	
  CreateOpDialog();	

  ////TODO: to be removed
  //TestCode();

  int result = m_Dialog->ShowModal() == wxID_OK ? OP_RUN_OK : OP_RUN_CANCEL;
  DeleteOpDialog();

  DeleteInternalStructures();
  albaEventMacro(albaEvent(this,result));
}

//----------------------------------------------------------------------------
void albaOpMeshDeformation::OpDo()
//----------------------------------------------------------------------------
{
  if (m_Output != NULL)
    m_Output->ReparentTo(m_Input);

  albaVME** pCVMEs[2] = { m_OriginalCurves, m_DeformedCurves };
  for (int i = 0; i < 2; i++)
  {
    if (m_SaveODC[i] == 0)
      continue; //this type of curves is not to be saved

    for (int j = 0; j < m_NumberOfCurves; j++) 
    {
      if (pCVMEs[i][j] != NULL)
        pCVMEs[i][j]->ReparentTo(m_Output);
    }
  }
  
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaOpMeshDeformation::OpUndo()
//----------------------------------------------------------------------------
{
  if (m_Output != NULL)
  {
    m_Output->ReparentTo(NULL);
    albaDEL(m_Output);
  }

  albaVME** pCVMEs[2] = { m_OriginalCurves, m_DeformedCurves };
  for (int i = 0; i < 2; i++)
  {
    if (m_SaveODC[i] == 0)
      continue; //this type of curves is not to be saved

    for (int j = 0; j < m_NumberOfCurves; j++) 
    {
      if (pCVMEs[i][j] != NULL)
      {
        pCVMEs[i][j]->ReparentTo(NULL);
        albaDEL(pCVMEs[i][j]);
      }
    }
  }

	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaOpMeshDeformation::OpStop(int result)
//----------------------------------------------------------------------------
{
  albaEventMacro(albaEvent(this,result));        
}

//----------------------------------------------------------------------------
//Destroys GUI
void albaOpMeshDeformation::CreateOpDialog()
//----------------------------------------------------------------------------
{
  wxBusyCursor wait;

  //===== setup interface ====
  m_Dialog = new albaGUIDialog("Mesh Deformation", albaCLOSEWINDOW | albaRESIZABLE);  
  m_Dialog->SetWindowStyle(m_Dialog->GetWindowStyle() | wxMAXIMIZE_BOX);

  //rendering window
  m_Rwi = new albaRWI(m_Dialog,ONE_LAYER,false);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(CAMERA_PERSPECTIVE);
  m_Rwi->m_RenderWindow->SetDesiredUpdateRate(0.0001f);
  m_Rwi->SetSize(0,0,400,400);
  m_Rwi->Show(true);
  m_Rwi->m_RwiBase->SetMouse(m_Mouse);


  //The following code was originally generated using wxFormBuilder
  //and modified here to work with ALBA
#pragma region //wxFormBuilder Component Construction
  wxBoxSizer* bSizer1;
  bSizer1 = new wxBoxSizer( wxHORIZONTAL );

  //Left panel - RWI
  wxBoxSizer* bSizer18;
  bSizer18 = new wxBoxSizer( wxVERTICAL );
  bSizer18->Add( m_Rwi->m_RwiBase, 1, wxEXPAND | wxALL, 5 );

  //Left panel - Buttons
  wxBoxSizer* bSizer21;
  bSizer21 = new wxBoxSizer( wxHORIZONTAL );

  m_BttnReset = new wxButton( m_Dialog, ID_RESET, wxT("Reset"), wxDefaultPosition, wxDefaultSize, 0 );
  m_BttnReset->SetToolTip( wxT("Resets every change to control curves.") );
  m_BttnReset->Enable(false);

  bSizer21->Add( m_BttnReset, 0, wxALL, 5 );

  m_BttnUndo = new wxButton( m_Dialog, ID_UNDO, wxT("Undo"), wxDefaultPosition, wxDefaultSize, 0 );
  m_BttnUndo->SetToolTip( wxT("Undoes the last change to control curves.") );
  m_BttnUndo->Enable(false);

  bSizer21->Add( m_BttnUndo, 0, wxALL, 5 );

  m_BttnPreview = new wxButton( m_Dialog, ID_PREVIEW, wxT("Preview"), wxDefaultPosition, wxDefaultSize, 0 );
  m_BttnPreview->SetToolTip( wxT("Deforms the mesh according to the current configuration of control curves.") );
  m_BttnPreview->Enable(false);

  bSizer21->Add( m_BttnPreview, 0, wxALL, 5 );
  bSizer18->Add( bSizer21, 0, wxEXPAND, 5 );
  bSizer1->Add( bSizer18, 1, wxEXPAND, 5 );

  //Right panel  
  wxBoxSizer* bSizer19;
  bSizer19 = new wxBoxSizer( wxVERTICAL );

  //Right panel - Create Control Curves
  wxBoxSizer* bSizer3;
  bSizer3 = new wxBoxSizer( wxVERTICAL );

  wxStaticBoxSizer* sbSizer2;
  sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_Dialog, wxID_ANY, 
    wxT("Create Control Curves") ), wxVERTICAL );
  
  m_BttnGenCurves = new wxButton( m_Dialog, ID_CREATE_CURVES, wxT("Generate Curves"), 
    wxDefaultPosition, wxDefaultSize, 0 );
  m_BttnGenCurves->SetToolTip( wxT("Automatically creates all control curves for the mesh.") );

  sbSizer2->Add( m_BttnGenCurves, 0, wxALL, 5 );

  wxStaticBoxSizer* sbSizer1;
  sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_Dialog, wxID_ANY, 
    wxT("Add Control Curve") ), wxVERTICAL );

  wxBoxSizer* bSizer5;
  bSizer5 = new wxBoxSizer( wxHORIZONTAL );  
  bSizer5->Add(new wxStaticText( m_Dialog, wxID_ANY, wxT("Original Curve:"), 
    wxDefaultPosition, wxSize( 85,-1 ), wxALIGN_RIGHT ), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

  m_OCNameCtrl = new wxStaticText( m_Dialog, wxID_ANY, wxEmptyString,
    wxDefaultPosition, wxDefaultSize, 0|wxSTATIC_BORDER );  
  bSizer5->Add( m_OCNameCtrl, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

  wxButton* bttnSelOC = new wxButton( m_Dialog, ID_SELECT_OC, 
    wxT("Select"), wxDefaultPosition, wxDefaultSize, 0 );
  bttnSelOC->SetToolTip( wxT("Selects VME with the control curve (or graph) related to the original mesh.\n\n"
    "Note: the mesh deformation is governed by the difference between the original "
    "and deformed control curve.") );

  bSizer5->Add( bttnSelOC, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
  sbSizer1->Add( bSizer5, 0, wxEXPAND, 5 );

  wxBoxSizer* bSizer51;
  bSizer51 = new wxBoxSizer( wxHORIZONTAL );
  bSizer51->Add( new wxStaticText( m_Dialog, wxID_ANY, wxT("Deformed Curve:"), 
    wxDefaultPosition, wxSize( 85,-1 ), wxALIGN_RIGHT ), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

  m_DCNameCtrl = new wxStaticText( m_Dialog, wxID_ANY, wxEmptyString, 
    wxDefaultPosition, wxDefaultSize, 0|wxSTATIC_BORDER );  
  bSizer51->Add( m_DCNameCtrl, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

  m_BttnSelDC = new wxButton( m_Dialog, ID_SELECT_DC, wxT("Select"), wxDefaultPosition, wxDefaultSize, 0 );
  m_BttnSelDC->SetToolTip( wxT("[OPTIONAL] Selects VME with the corresponding deformed curve (or graph). "
    "The deformed curve may contain different number of vertices. If the deformed curve is not specified, "
    "it is automatically created from the original curve. \n\nNote: the mesh deformation is governed by "
    "the difference between the original and deformed control curve.") );
  m_BttnSelDC->Enable(false);
  bSizer51->Add( m_BttnSelDC, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
  sbSizer1->Add( bSizer51, 0, wxEXPAND, 5 );

  wxBoxSizer* bSizer511;
  bSizer511 = new wxBoxSizer( wxHORIZONTAL );

  bSizer511->Add( new wxStaticText( m_Dialog, wxID_ANY, wxT("Correspondence:"), 
    wxDefaultPosition, wxSize( 85,-1 ), wxALIGN_RIGHT ), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

  m_CCCtrl = new wxTextCtrl( m_Dialog, ID_CORRESPONDENCE, wxEmptyString, 
    wxDefaultPosition, wxDefaultSize, 0 );
  m_CCCtrl->SetToolTip( wxT("[OPTIONAL] Specifies the correspondence between vertices of original "
    "and deformed curves in the form: \"id1_oc,id1_dc;id2_oc,id2_dc\", e.g., \"0,3;1,12\" "
    "means that the vertex with the index 0 in the original curve corresponds to the vertex 3 "
    "in the deformed curve and the vertex 1 corresponds to the vertex 12. N.B. It is not "
    "necessary to specify the correspondence for every vertex.\n\nIf correspondence is ommitted, "
    "it is automatically detected.\n") );
  m_CCCtrl->Enable(false);
  bSizer511->Add( m_CCCtrl, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
  sbSizer1->Add( bSizer511, 0, wxEXPAND, 5 );

  wxBoxSizer* bSizer12;
  bSizer12 = new wxBoxSizer( wxHORIZONTAL );  
  bSizer12->Add( new wxPanel( m_Dialog, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
    wxTAB_TRAVERSAL ), 1, wxALL|wxEXPAND, 5 );

  m_BttnResetCurve = new wxButton( m_Dialog, ID_RESETCURVE, wxT("Reset"), wxDefaultPosition, wxDefaultSize, 0 );
  bSizer12->Add( m_BttnResetCurve, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

  m_BttnAddCurve = new wxButton( m_Dialog, ID_ADDCURVE, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
  bSizer12->Add( m_BttnAddCurve, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
  m_BttnAddCurve->Enable(false);

  sbSizer1->Add( bSizer12, 0, wxEXPAND|wxALIGN_RIGHT, 5 );
  sbSizer2->Add( sbSizer1, 0, wxEXPAND, 5 );
  bSizer3->Add( sbSizer2, 0, wxEXPAND|wxALIGN_RIGHT, 5 );

  //Right panel - Edit Control Curves
  wxStaticBoxSizer* sbSizer3;
  sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_Dialog, wxID_ANY, wxT("Edit Control Curves") ), wxVERTICAL );

  wxBoxSizer* bSizer13;
  bSizer13 = new wxBoxSizer( wxHORIZONTAL );

  bSizer13->Add( new wxStaticText( m_Dialog, wxID_ANY, wxT("Edit Mode:"), 
    wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

  wxComboBox* editMode = new wxComboBox( m_Dialog, ID_EDITMODE, wxEmptyString, 
    wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );

  //this must correspond to EDIT_MODES
  editMode->Append( wxT("select") );
  editMode->Append( wxT("move point") );
  editMode->Append( wxT("add point") );
  editMode->Append( wxT("delete point") );
  editMode->Append( wxT("add correspondence") );
  editMode->Append( wxT("delete correspondence") );
  editMode->Append( wxT("delete curve") );
#ifdef DEBUG_albaOpMeshDeformation
  editMode->Append( wxT("select mesh vertex") );
#endif
  editMode->SetToolTip( wxT("Selects edit mode. ") );

  bSizer13->Add( editMode, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
  sbSizer3->Add( bSizer13, 0, wxEXPAND, 5 );

  m_EditModeHelp = new wxTextCtrl( m_Dialog, ID_EDITMODE_HELP, wxEmptyString, 
    wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP );
  m_EditModeHelp->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INACTIVEBORDER ) );  
  sbSizer3->Add( m_EditModeHelp, 0, wxALL|wxEXPAND, 5 );

  m_EditModeInfo = new wxStaticText( m_Dialog, ID_EDITMODE_INFO, wxEmptyString,
    wxDefaultPosition, wxDefaultSize, 0/*|wxSIMPLE_BORDER */);  
  sbSizer3->Add( m_EditModeInfo, 0, wxALL|wxEXPAND, 5 );

  m_ChckEditOC = new wxCheckBox( m_Dialog, ID_ENABLE_OC_EDIT, 
    wxT("Enable Editing of Original Curves"), wxDefaultPosition, wxDefaultSize, 0 );

  m_ChckEditOC->SetToolTip( wxT("If checked, the user can modify also the original control curves.") );

  sbSizer3->Add( m_ChckEditOC, 0, wxALL, 5 );
  bSizer3->Add( sbSizer3, 0, wxEXPAND, 5 );

  //Right panel - Visual Options
  wxStaticBoxSizer* sbSizer4;
  sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_Dialog, wxID_ANY, wxT("Visual Options") ), wxVERTICAL );

  wxBoxSizer* bSizer14;
  bSizer14 = new wxBoxSizer( wxHORIZONTAL );

  wxCheckBox* chckShowOC = new wxCheckBox( m_Dialog, ID_SHOW_OC, 
    wxT("Show Original Curves"), wxDefaultPosition, wxDefaultSize, 0 );
  chckShowOC->SetValue(true);

  bSizer14->Add( chckShowOC, 0, wxALL, 5 );

  wxCheckBox* chckShowDC = new wxCheckBox( m_Dialog, ID_SHOW_DC, 
    wxT("Show Deformed Curves"), wxDefaultPosition, wxDefaultSize, 0 );
  chckShowDC->SetValue(true);

  bSizer14->Add( chckShowDC, 0, wxALL, 5 );
  sbSizer4->Add( bSizer14, 0, wxEXPAND, 5 );

  wxCheckBox* chckShowCC = new wxCheckBox( m_Dialog, ID_SHOW_CC, 
    wxT("Show Correspondences"), wxDefaultPosition, wxDefaultSize, 0 );
  chckShowCC->SetValue(true);

  sbSizer4->Add( chckShowCC, 0, wxALL, 5 );

  wxBoxSizer* bSizer141;
  bSizer141 = new wxBoxSizer( wxHORIZONTAL );

  wxCheckBox* chckShowOM = new wxCheckBox( m_Dialog, ID_SHOW_OM, 
    wxT("Show Original Mesh"), wxDefaultPosition, wxDefaultSize, 0 );
  chckShowOM->SetValue(true);

  bSizer141->Add( chckShowOM, 0, wxALL, 5 );

  wxCheckBox* chckShowDM = new wxCheckBox( m_Dialog, ID_SHOW_DM, 
    wxT("Show Deformed Mesh"), wxDefaultPosition, wxDefaultSize, 0 );
  chckShowDM->SetValue(true);

  bSizer141->Add( chckShowDM, 0, wxALL, 5 );
  sbSizer4->Add( bSizer141, 1, wxEXPAND, 5 );
  bSizer3->Add( sbSizer4, 0, wxEXPAND, 5 );

  //Right panel - Deformation mode
  wxStaticBoxSizer* bSizer41;
  bSizer41 = new wxStaticBoxSizer( new wxStaticBox( m_Dialog, wxID_ANY, 
    wxT("Deformation Options") ), wxVERTICAL );

  wxBoxSizer* bSizer131;
  bSizer131 = new wxBoxSizer( wxHORIZONTAL );

  wxStaticText* staticText201 = new wxStaticText( m_Dialog, wxID_ANY, wxT("Method:"), 
    wxDefaultPosition, wxDefaultSize, 0 );
  staticText201->Wrap( -1 );
  bSizer131->Add( staticText201, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

  wxComboBox* defMode = new wxComboBox( m_Dialog, ID_DEFORMATIONMODE, 
    wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
  defMode->Append( wxT("orthogonal mapping (M1)") );
  defMode->Append( wxT("accurate ROI mapping (M2)") );
  defMode->Append( wxT("simple warping (M3)") );
  bSizer131->Add( defMode, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

  bSizer41->Add( bSizer131, 0, wxEXPAND, 5 );

  m_DeformationModeHelp = new wxTextCtrl( m_Dialog, ID_DEFORMATIONMODE_HELP, 
    wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_WORDWRAP );
  m_DeformationModeHelp->SetBackgroundColour( 
    wxSystemSettings::GetColour( wxSYS_COLOUR_INACTIVEBORDER ) );

  bSizer41->Add( m_DeformationModeHelp, 0, wxALL|wxEXPAND, 5 );
	bSizer3->Add( bSizer41, 0, wxEXPAND, 5 );

  bSizer19->Add( bSizer3, 1, wxEXPAND, 5 );

  //Right panel - Buttons
  wxBoxSizer* bSizer211;
  bSizer211 = new wxBoxSizer( wxVERTICAL );
  
  wxStaticBoxSizer* sbSizer5;
  sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( m_Dialog, wxID_ANY, 
    wxT("Save options") ), wxVERTICAL );

  wxBoxSizer* bSizer15;
  bSizer15 = new wxBoxSizer( wxHORIZONTAL );

  wxCheckBox* chckSaveOC = new wxCheckBox( m_Dialog, ID_SAVEOC, wxT("Save Original Curves"),
    wxDefaultPosition, wxDefaultSize, 0 );

  chckSaveOC->SetToolTip( 
    wxT("If checked, a new VME (reparent to the deformed mesh VME) is "
    "constructed for every original curve when OK button is clicked. \n"
    "N.B. This option is NOT automatically checked when the user generates original curves or modifies them.") );

  bSizer15->Add( chckSaveOC, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

  wxCheckBox* chckSaveDC = new wxCheckBox( m_Dialog, ID_SAVEDC, wxT("Save Defomed Curves"), 
    wxDefaultPosition, wxDefaultSize, 0 );

  chckSaveDC->SetToolTip( 
    wxT("If checked, a new VME (reparent to the deformed mesh VME) is "
    "constructed for every deformed curve when OK button is clicked. \n"
    "N.B. This option is automatically checked when the user generates deformed curves or modifies them.") );

  bSizer15->Add( chckSaveDC, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

  sbSizer5->Add( bSizer15, 1, wxEXPAND, 5 );

  bSizer211->Add( sbSizer5, 0, wxEXPAND, 5 );

  wxBoxSizer* bSizer142;
  bSizer142 = new wxBoxSizer( wxVERTICAL );

  wxBoxSizer* bSizer16;
  bSizer16 = new wxBoxSizer( wxHORIZONTAL );
  
  m_BttnOK = new wxButton( m_Dialog, ID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
  m_BttnOK->SetToolTip( wxT("Deforms the mesh according to the current configuration of control "
    "curves and creates new VMEs for the deformed mesh and optionally for deformed control curves.") );  
  m_BttnOK->Enable(false);
  bSizer16->Add( m_BttnOK, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

  wxButton* bttnCancel = new wxButton( m_Dialog, ID_CANCEL, wxT("Cancel"), 
    wxDefaultPosition, wxDefaultSize, 0 );
  bSizer16->Add( bttnCancel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
  bSizer142->Add( bSizer16, 1, wxEXPAND, 5 );
  bSizer211->Add( bSizer142, 1, wxEXPAND, 5 );
  bSizer19->Add( bSizer211, 0, wxEXPAND|wxALIGN_RIGHT, 5 );
  bSizer1->Add( bSizer19, 0, wxEXPAND, 5 );
#pragma endregion //wxFormBuilder

  //validators for Create Control Curves
  m_BttnGenCurves->SetValidator(albaGUIValidator(this, ID_CREATE_CURVES, m_BttnGenCurves));
  bttnSelOC->SetValidator(albaGUIValidator(this, ID_SELECT_OC, bttnSelOC));
  m_BttnSelDC->SetValidator(albaGUIValidator(this, ID_SELECT_DC, m_BttnSelDC));
  m_BttnResetCurve->SetValidator(albaGUIValidator(this, ID_RESETCURVE, m_BttnResetCurve));
  m_BttnAddCurve->SetValidator(albaGUIValidator(this, ID_ADDCURVE, m_BttnAddCurve));
  m_BttnReset->SetValidator(albaGUIValidator(this, ID_RESET, m_BttnReset));
  m_BttnUndo->SetValidator(albaGUIValidator(this, ID_UNDO, m_BttnUndo));
  m_BttnPreview->SetValidator(albaGUIValidator(this, ID_PREVIEW, m_BttnPreview));

  //validators for EditMode & Deformation Mode
  editMode->SetValidator(albaGUIValidator(this, ID_EDITMODE, editMode, &m_EditMode));
  OnEditMode(); //to force Help info

  defMode->SetValidator(albaGUIValidator(this, ID_DEFORMATIONMODE, defMode, &m_DeformationMode));
  OnDeformationMode(); //to force Help info

  //validators for visual options
  chckShowOM->SetValidator(albaGUIValidator(this, ID_SHOW_XX_CHANGE, chckShowOM, &m_MeshesVisibility[0]));
  chckShowDM->SetValidator(albaGUIValidator(this, ID_SHOW_XX_CHANGE, chckShowDM, &m_MeshesVisibility[1]));
  chckShowOC->SetValidator(albaGUIValidator(this, ID_SHOW_XX_CHANGE, chckShowOC, &m_CurvesVisibility[0]));
  chckShowDC->SetValidator(albaGUIValidator(this, ID_SHOW_XX_CHANGE, chckShowDC, &m_CurvesVisibility[1]));
  chckShowCC->SetValidator(albaGUIValidator(this, ID_SHOW_XX_CHANGE, chckShowCC, &m_CurvesVisibility[2]));

  chckSaveOC->SetValidator(albaGUIValidator(this, ID_SAVEOC, chckSaveOC, &m_SaveODC[0]));
  chckSaveDC->SetValidator(albaGUIValidator(this, ID_SAVEDC, chckSaveDC, &m_SaveODC[1]));

  m_BttnOK->SetValidator(albaGUIValidator(this, ID_OK, m_BttnOK));
  bttnCancel->SetValidator(albaGUIValidator(this, ID_CANCEL, bttnCancel));

  m_Dialog->Add(bSizer1, 1, wxEXPAND);

  int x_pos,y_pos,w,h;
  albaGetFrame()->GetPosition(&x_pos,&y_pos);
  m_Dialog->GetSize(&w,&h);
  m_Dialog->SetSize(x_pos+5,y_pos+5,w,h);

  //create the picker
  albaNEW(m_Picker);
  m_Picker->SetListener(this);
  m_Mouse->AddObserver(m_Picker, MCH_INPUT);

  //and initialize the renderer window
  double bounds[6];	
  m_Meshes[0]->pPoly->GetBounds(bounds);

  m_Rwi->m_RenFront->AddActor(m_Meshes[0]->pActor);	
  m_Rwi->m_RenFront->ResetCamera(bounds);

  UpdateVisibility();  
}

//----------------------------------------------------------------------------
//Creates GUI including renderer window
void albaOpMeshDeformation::DeleteOpDialog()
//----------------------------------------------------------------------------
{
  m_Mouse->RemoveObserver(m_Picker);
  albaDEL(m_Picker);

  //remove all actors
  RemoveAllActors();    

  cppDEL(m_Rwi); 
  cppDEL(m_Dialog);
}

//----------------------------------------------------------------------------
void albaOpMeshDeformation::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
    case ID_CREATE_CURVES:
      OnCreateCurves();
      break;

    case ID_SELECT_OC:
      OnSelectOC();
      break;

    case ID_SELECT_DC:
      OnSelectDC();
      break;

    case ID_RESETCURVE:
      OnResetCurve();
      break;

    case ID_ADDCURVE:
      OnAddCurve();
      break;

    case ID_EDITMODE:
      OnEditMode();
      break;

    case ID_DEFORMATIONMODE:
      OnDeformationMode();
      break;

    case ID_SHOW_XX_CHANGE:
      //visibility options has changed
      UpdateVisibility();
      break;

    case VME_PICKING:
      if (m_EditMode == EDM_MOVE_POINTS)
      {
        if (!e->GetBool())
          OnPick((vtkCellPicker*)e->GetVtkObj());   //start movement  
        else         
          MovePoint(((vtkPoints*)e->GetVtkObj())->GetPoint(0));        
      }
      break;
            
    case VME_PICKED:
      if (m_EditMode == EDM_MOVE_POINTS)
        EndMovePoint();
      else
        OnPick((vtkCellPicker*)e->GetVtkObj());
      break;

    case ID_UNDO:
      OnUndo();
      break;

    case ID_RESET:
      OnReset();
      break;

    case ID_PREVIEW:
      OnPreview();
      break;
      
    case ID_OK:
      OnOk();
      m_Dialog->EndModal(wxID_OK);
      break;

    case ID_CANCEL:
      m_Dialog->EndModal(wxID_CANCEL);
      break;
    }
  }
}

#pragma region GUI Handlers
//------------------------------------------------------------------------
//Generates control curves for the mesh.
//All existing control curves are removed
/*virtual*/ void albaOpMeshDeformation::OnCreateCurves()
//------------------------------------------------------------------------
{
  //TODO: automatically detect skeleton of mesh
  //this will require the computation of medial axis
  //which can be obtained via Voronoi diagram of points from input mesh
  double bounds[6];
  m_Meshes[0]->pPoly->GetBounds(bounds);
  
  //detect the longest dimension
  int iMaxDim = 0;
  double dblMaxDim = fabs(bounds[1] - bounds[0]);
  for (int i = 2; i < 6; i += 2)
  {
    double dblDim = fabs(bounds[i + 1] - bounds[i]);
    if (dblDim > dblMaxDim)
    {
      dblMaxDim = dblDim;
      iMaxDim = i;
    }
  }
  
  //compute points positions
  vtkPoints* points = vtkPoints::New();  

  double v1[3], v2[3];
  for (int i = 0, j = 0; i < 6; i += 2, j++)
  {
    if (i != iMaxDim) {
      v2[j] = v1[j] = (bounds[i + 1] + bounds[i]) / 2;
    }
    else
    {
      v1[j] = bounds[i];
      v2[j] = bounds[i + 1];
    }    
  }
  
  //create the polydata
  points->InsertNextPoint(v1);
  points->InsertNextPoint(v2);
  
  vtkIdList* ids = vtkIdList::New();  
  ids->InsertNextId(0);
  ids->InsertNextId(1);  

  vtkCellArray* lines = vtkCellArray::New();
  lines->InsertNextCell(ids);

  vtkPolyData* pPoly = vtkPolyData::New();
  pPoly->SetPoints(points);
  pPoly->SetLines(lines);  

  //create graphs from the polydata
  albaPolylineGraph* pOC = new albaPolylineGraph();
  albaPolylineGraph* pDC = new albaPolylineGraph();
  if (!pOC->CopyFromPolydata(pPoly) ||
      !pDC->CopyFromPolydata(pPoly))
  {
    _ASSERT(false);

    cppDEL(pOC);
    cppDEL(pDC);
  }

  //delete polydata, they are not neededed anymore
  pPoly->Delete();
  lines->Delete();
  ids->Delete();
  points->Delete();

  //if everything is OK, create control curves
  if (pOC != NULL)
    AddControlCurve(pOC, pDC, NULL);  

  //update check box
  m_SaveODC[1] = 1;
  m_Dialog->Update();
}

//------------------------------------------------------------------------
//Opens VMEChoose dialog and selects new original curve.
/*virtual*/ void albaOpMeshDeformation::OnSelectOC()
//------------------------------------------------------------------------
{
  albaVME* vme = SelectCurveVME();
  if (vme != NULL)
  {
    m_OCToAdd = vme;

    m_OCNameCtrl->SetLabel(m_OCToAdd->GetName());
    m_BttnSelDC->Enable(true);  
    m_BttnAddCurve->Enable(true);
  }      
}

//------------------------------------------------------------------------
//Opens VMEChoose dialog and selects new deformed curve.
/*virtual*/ void albaOpMeshDeformation::OnSelectDC()
//------------------------------------------------------------------------
{
  albaVME* vme = SelectCurveVME();
  if (vme != NULL)
  {
    m_DCToAdd = vme;

    m_DCNameCtrl->SetLabel(m_DCToAdd->GetName());
    m_CCCtrl->Enable(true);  
  } 
}

//------------------------------------------------------------------------
//Selects VME of supported type for control curve
//Returns NULL, if the user cancels the selection.
/*virtual*/ albaVME* albaOpMeshDeformation::SelectCurveVME()
//------------------------------------------------------------------------
{
  albaString title = "Choose VME with control curve";
  
  albaEvent ev(this, VME_CHOOSE);   
  ev.SetString(&title);
  ev.SetPointer(&albaOpMeshDeformation::SelectCurveVMECallback);

  albaEventMacro(ev);
  return ev.GetVme();
}

//------------------------------------------------------------------------
//Callback for VME_CHOOSE that accepts polylines only
/*static*/ bool albaOpMeshDeformation::SelectCurveVMECallback(albaVME *vme) 
//------------------------------------------------------------------------
{
  return vme != NULL && vme->GetOutput() && vme->GetOutput()->IsA("albaVMEOutputPolyline");
}

//------------------------------------------------------------------------
//Clears everything form OC, DC and CC edits.
/*virtual*/ void albaOpMeshDeformation::OnResetCurve()
//------------------------------------------------------------------------
{
  m_OCNameCtrl->SetLabel(wxEmptyString);
  m_DCNameCtrl->SetLabel(wxEmptyString);
  m_CCCtrl->SetValue(wxEmptyString);

  m_BttnSelDC->Enable(false);
  m_CCCtrl->Enable(false);
  m_BttnAddCurve->Enable(false);

  m_OCToAdd = m_DCToAdd = NULL;
}

//------------------------------------------------------------------------
//Adds new control curve.
/*virtual*/ void albaOpMeshDeformation::OnAddCurve()
//------------------------------------------------------------------------
{ 
  albaPolylineGraph *pOC, *pDC;
  vtkIdList* pCC = NULL;

  if (m_DCToAdd == NULL)
  {
    //the deformation curve is not specified => create it
    pOC = CreatePolylineGraph(m_OCToAdd);
    pDC = CreateCopyOfPolylineGraph(pOC);
  }
  else
  {
    //if the deformation curve is specified, 
    //we need to perform few checks       
    wxString szCor = m_CCCtrl->GetValue().Trim();
    if (!szCor.IsEmpty())
    {
      //the user has specified correspondence
      //we need to check every value         
      vtkIdList* pCC2 = vtkIdList::New();
      pCC2->Allocate(10);       //they should not be many

      char* szBuf =  new char[szCor.Len() + 1];
      strcpy(szBuf, szCor.ToAscii());

      const static char delims[] = {',',';'};
      int iNextDelim = 0;

      char* szStr = szBuf;
      while (*szStr != '\0')
      {
        //search for the next delimiter
        char* szDel = strchr(szStr, delims[iNextDelim]);      
        if (szDel != NULL)  //if it exists, terminates the string there
          *szDel = '\0';

        //convert the next value into number
        unsigned long nVal = strtoul(szStr, &szStr, 10);
        if (*szStr != '\0')
        {
          //error => display an error and continue
          wxMessageBox(_("Invalid format of the specified correspondence string."), 
            _("Error"), wxOK | wxICON_EXCLAMATION);  

          pCC2->Delete();
          return;
        }             

        pCC2->InsertNextId(nVal);  //add value
        iNextDelim = (iNextDelim + 1) % 2;
        
        if (szDel != NULL)
          szStr++;  //if there is another value
      } //end while
      
      delete[] szBuf;  
      pCC2->Squeeze();

      //we should check now, if values are valid
      int nCount = (int)pCC2->GetNumberOfIds();
      if ((nCount % 2) != 0)
      {
        //error => display an error and continue
        wxMessageBox(_("Incomplete list of correspondences.\nOne index is missing."), 
          _("Error"), wxOK | wxICON_EXCLAMATION);  

        pCC2->Delete();
        return;
      }

      //get point sets for both curves
      vtkPolyData* pPoly;
      int VertCount[2];
            
      _VERIFY_RET(NULL != (pPoly = vtkPolyData::SafeDownCast(
        m_OCToAdd->GetOutput()->GetVTKData()))); 
      pPoly->Update();
      VertCount[0] = pPoly->GetPoints()->GetNumberOfPoints();
      
      _VERIFY_RET(NULL != (pPoly = vtkPolyData::SafeDownCast(
        m_DCToAdd->GetOutput()->GetVTKData())));
      pPoly->Update();
      VertCount[1] = pPoly->GetPoints()->GetNumberOfPoints();

      pCC = vtkIdList::New();
      vtkIdType* ids = pCC2->GetPointer(0);
      for (int i = 0; i < nCount; i += 2)
      {
        if (ids[i] < VertCount[0] && ids[i + 1] < VertCount[1])
        {
          pCC->InsertNextId(ids[i]);
          pCC->InsertNextId(ids[i + 1]);
        }
        else
        {
          //error => display an error and continue
          if (wxMessageBox(albaString::Format(
            _("The %s curve does not contain vertex with index %d.\n"
            "Do you want to ignore this problem?"),
            ((i % 2) == 0 ? _("original") : _("deformed")), ids[i]),
            _("Warning"), wxYES_NO | wxICON_EXCLAMATION) == wxNO)
          {
            pCC->Delete();
            pCC2->Delete();
            return;
          }
        } 
      } //end for      

      pCC2->Delete();
    } //end if (!szCor.IsEmpty())

    pOC = CreatePolylineGraph(m_OCToAdd);
    pDC = CreatePolylineGraph(m_DCToAdd);
  }

  //create the curve and add it into the system
  AddControlCurve(pOC, pDC, pCC);
  OnResetCurve();  
}

//------------------------------------------------------------------------
//Called when the edit mode changes.
//Updates the EditModeHelp text
/*virtual*/ void albaOpMeshDeformation::OnEditMode()
//------------------------------------------------------------------------
{
  //this must correspond to EDIT_MODES
  const static char* szHelp[] = {
    "CTRL + click on an object to select it.",
    "CTRL + drag a vertex to its new position.",
    "CTRL + click on an original or a deformed curve, where a new vertex should be inserted.",
    "CTRL + click on a vertex you want to remove.",
    "CTRL + click on a vertex of an original (a deformed) curve and "
      "then CTRL + click on a vertex of the corresponding deformed (original) curve to "
      "create correspondence between these curves.",
    "CTRL + click on a tube representing the correspondence to delete this correspondence.",
    "CTRL + click on a vertex, an original or a deformed curve or a correspondence tube "
    "to remove the underlaying control curve.",
#ifdef DEBUG_albaOpMeshDeformation
    "CTRL + click on a mesh vertex to get information",    
#endif
  };

  _VERIFY_RET(m_EditMode >= 0 && m_EditMode < sizeof(szHelp));    
  m_EditModeHelp->SetValue(szHelp[m_EditMode]);

  //reset correspondence construction
  m_BCorrespondenceActive = false;

#ifdef DEBUG_albaOpMeshDeformation
  if (m_EditMode == EDM_SELECT_MESH_VERTEX)
  {
    m_Meshes[0]->pActor->PickableOn();
    m_Meshes[1]->pActor->PickableOn();
  }
  else
  {
    m_Meshes[0]->pActor->PickableOff();
    m_Meshes[1]->pActor->PickableOff();
  }
#endif
}

//------------------------------------------------------------------------
//Called when the deformation mode changes.
//Updates the DeformationModeHelp text
/*virtual*/ void albaOpMeshDeformation::OnDeformationMode()
//------------------------------------------------------------------------
{
  //this must correspond to EDIT_MODES
  const static char* szHelp[] = {

    "Blanco's method with LF (tangent vectors are computed to be parallel "
    "with edge directions) set to avoid self-intersection, blending "
    "between edges connected to joint (no blending between simple "
    "connected edges), ROI refined according to distance, infinite edges "
    "have lower priority.\nWorks well for small deformations.",

    "Modified Blanco's method. LF (tangent vectors are computed to be "
    "parallel with edge directions) ARE NOT set to avoid "
    "self-intersection. No blending at all, ROI of edges defined precisely "
    "by separating planes constructed between every pair of connected "
    "edges. A vertex is mapped to just one edge that is closest in the "
    "geodesic meaning (slow intersection and Dijkstra shortest path "
    "test).\nAlthough regions are better defined, still strange behaviour "
    "occurs for larger or awkward deformations.",
    
    "Simple warping technique. Mesh vertices are mapped to all edges but "
    "those mapping have different weights. The weight depends on the "
    "distance of vertex from the edge; if the vertex is not in area "
    "defined by perpendicular planes in end-points of the edge, the weight "
    "is much smaller.\nWorks even for large deformation but self-intersection "
    "may appear and it may deform also distant parts that should not be changed.",
    
  };

  _VERIFY_RET(m_DeformationMode >= 0 && m_DeformationMode < sizeof(szHelp));    
  m_DeformationModeHelp->SetValue(szHelp[m_DeformationMode]);
}

//------------------------------------------------------------------------
//Called when the user picks something. 
/*virtual*/ void albaOpMeshDeformation::OnPick(vtkCellPicker* cellPicker)
//------------------------------------------------------------------------
{  
  double pickedPos[3];    //place where the pick was done
  vtkDataSet* pickedObj;  //picked VTK object
      
  //cellPicker->GetPickPosition(pickedPos);  
  cellPicker->GetPickedPositions()->GetPoint(0, pickedPos);
  pickedObj = cellPicker->GetDataSet();

  //search every objects in order to find the curve to be picked
  int nCount = (int)m_Curves.size();
  for (int i = 0; i < nCount; i++)
  {
    CONTROL_CURVE* pCurve = m_Curves[i];
    for (int j = 0; j < 3; j++)
    {
      vtkDataSet* pDS = pCurve->pActors[j]->GetMapper()->GetInput();
      if (pickedObj == pDS)
      {
        //the current pCurve was picked at picketObj
        if (m_EditMode != EDM_MOVE_POINTS && m_BPointMoveActive)
          EndMovePoint(); //this is to prevent wrong behavior
        if (m_EditMode != EDM_ADD_CORRESPONDENCE && m_BCorrespondenceActive)
          m_BCorrespondenceActive = false;  //fix

        switch(m_EditMode)
        {
        case EDM_DELETE_CURVE:
          DeleteControlCurve(i);
          break;

        case EDM_SELECT:        
          SelectControlCurve(pCurve, j, pickedPos);
          break;        

        case EDM_MOVE_POINTS:
          if (!m_BPointMoveActive)
            BeginMovePoint(i, j , pickedPos);
          else
            EndMovePoint();
          break;  

        case EDM_ADD_POINT:
          AddPoint(i, j, pickedPos);
          break;

        case EDM_DELETE_POINT:
          DeletePoint(i, j, pickedPos);
          break;

        case EDM_ADD_CORRESPONDENCE:
          if (!m_BCorrespondenceActive)
            BeginAddCorrespondence(i, j, pickedPos);
          else
            EndAddCorrespondence(i, j, pickedPos);
          break;

        case EDM_DELETE_CORRESPONDENCE:
          if (j == 2) //only CC is valid
            DeleteCorrespondence(i, pickedPos);
          break;

        } //end switch
        
        return;
      }
    }
  }

#ifdef DEBUG_albaOpMeshDeformation
  if (m_EditMode == EDM_SELECT_MESH_VERTEX)
  {
    vtkPolyData* pPoly = vtkPolyData::SafeDownCast(pickedObj);
    if (pPoly != NULL)
    {
      vtkIdType ptID = pPoly->FindPoint(pickedPos);
      double* pcoords = pPoly->GetPoint(ptID);

      if (pcoords == NULL)
        m_EditModeInfo->SetLabel(wxEmptyString);
      else
      {
        vtkDataArray* pDA = pPoly->GetPointData()->GetScalars();
        if (pDA == NULL)        
          m_EditModeInfo->SetLabel(albaString::Format(_("Selected vertex: #%d [%g, %g, %g]"),
          ptID, pcoords[0], pcoords[1], pcoords[2]));
        else
        {          
          m_EditModeInfo->SetLabel(albaString::Format(_("Selected vertex: #%d [%g, %g, %g] PD: %g"),
          ptID, pcoords[0], pcoords[1], pcoords[2], pDA->GetTuple1(ptID)));
        }
      }

      return;
    }
  }
#endif

  //nothing has been found, some error
  _ASSERT(false);
}


//------------------------------------------------------------------------
//Undoes every action performed after the last adding/deletion of control curve 
//It undoes movement of points, adding/deletion of vertices and correspondences
//N.B. The default implementation calls OnUndo 
/*virtual*/ void albaOpMeshDeformation::OnReset()
//------------------------------------------------------------------------
{
  while (!m_UndoStack.empty())
  {
    UNDO_ITEM* pUndo = m_UndoStack[m_UndoStack.size() - 1];
    int nAction = GET_EDM_CODE(pUndo->nAction);
    if (nAction == EDM_ADD_CURVE || nAction == EDM_DELETE_CURVE)
      return; //nothing to undone

    OnUndo();
  }
}

//------------------------------------------------------------------------
//Undoes the last action
/*virtual*/ void albaOpMeshDeformation::OnUndo()
//------------------------------------------------------------------------
{
  _VERIFY_RET(!m_UndoStack.empty());
  UNDO_ITEM* pUndo = m_UndoStack[m_UndoStack.size() - 1];
  m_UndoStack.pop_back(); //remove it from the stack

  int nAction = GET_EDM_CODE(pUndo->nAction);
  int nFlags = GET_CHANGE_FLAGS(pUndo->nAction);
  if (nAction == EDM_ADD_CURVE)
  {
    //we will remove the curve and everything related to it
    m_BDoNotCreateUndo = true;
    DeleteControlCurve(pUndo->nCurveIndex);
    m_BDoNotCreateUndo = false;
  }
  else if (nAction == EDM_DELETE_CURVE)
  {
    //we will add the curve at the proper position
    m_BDoNotCreateUndo = true;
    AddControlCurve(pUndo->pOC_DC[0], pUndo->pOC_DC[1], pUndo->pCC);
    m_BDoNotCreateUndo = false;

    pUndo->pOC_DC[0] = pUndo->pOC_DC[1] = NULL;
    pUndo->pCC = NULL;

    //now the curve is at the last => we need to swap it
    CONTROL_CURVE* pCurve = m_Curves[m_Curves.size() - 1];
    m_Curves.pop_back();

    m_Curves.insert(m_Curves.begin() + pUndo->nCurveIndex, pCurve);
  }
  else
  {
    //this is the simplest operation
    //all that is needed is to modify
    CONTROL_CURVE* pCurve = m_Curves[pUndo->nCurveIndex];
    if ((nFlags & CHANGE_OC) != 0)
    {
      cppDEL(pCurve->pPolyLines[0]);
      pCurve->pPolyLines[0] = pUndo->pOC_DC[0];
      pUndo->pOC_DC[0] = NULL;
    }

    if ((nFlags & CHANGE_DC) != 0)
    {
      cppDEL(pCurve->pPolyLines[1]);
      pCurve->pPolyLines[1] = pUndo->pOC_DC[1];
      pUndo->pOC_DC[1] = NULL;
    }

    if ((nFlags & CHANGE_CC) != 0)
    {
      vtkDEL(pCurve->pCCList);
      pCurve->pCCList = pUndo->pCC;
      pUndo->pCC = NULL;
    }

    UpdateControlCurve(pCurve, nFlags | CHANGE_CC); //correspondence must be updated always
    UpdateControlCurveVisibility(pCurve);
  }

  //delete the undo things
  cppDEL(pUndo->pOC_DC[0]);
  cppDEL(pUndo->pOC_DC[1]);
  vtkDEL(pUndo->pCC);

  delete pUndo;           //delete the item

  //update buttons
  if (!m_UndoStack.empty())
  {
    m_BttnUndo->Enable(true);

    pUndo = m_UndoStack[m_UndoStack.size() - 1];
    nAction = GET_EDM_CODE(pUndo->nAction);
    m_BttnReset->Enable(nAction != EDM_ADD_CURVE &&
      nAction != EDM_DELETE_CURVE);
  }
  else
  {
    m_BttnUndo->Enable(false);
    m_BttnReset->Enable(false);        
  }  
  
  bool bOK = !m_Curves.empty();
  m_BttnOK->Enable(bOK);
  m_BttnPreview->Enable(bOK);
  m_BttnGenCurves->Enable(!bOK);
}

//------------------------------------------------------------------------
//Performs the deformation using the current settings.
/*virtual*/ void albaOpMeshDeformation::OnPreview()
//------------------------------------------------------------------------
{  
  DeformMesh();
  
#ifdef DEBUG_albaOpMeshDeformation
 /* CONTROL_CURVE* pCurve = m_Curves[0];
  CreateUndo(0, EDM_ADD_CORRESPONDENCE, CHANGE_ALL);
  
  vtkDEL(pCurve->pCCList);
  pCurve->pCCList = md->m_MATCHED_FULLCC; 
  pCurve->pCCList->Register(NULL);

  pCurve->pPolyLines[0]->Clear();
  pCurve->pPolyLines[0]->CopyFromPolydata(md->m_MATCHED_POLYS[0]);

  pCurve->pPolyLines[1]->Clear();
  pCurve->pPolyLines[1]->CopyFromPolydata(md->m_MATCHED_POLYS[1]);  

  UpdateControlCurve(pCurve, CHANGE_ALL);
  UpdateControlCurveVisibility(pCurve);
*/
  m_Meshes[0]->pPoly->Update();
  vtkCharArray* scalars = vtkCharArray::SafeDownCast(
    m_Meshes[0]->pPoly->GetPointData()->GetScalars());
  if (scalars != NULL)
  {
    vtkLookupTable* table = vtkLookupTable::New();

    double sr[2];
    scalars->GetRange(sr);
    table->SetTableRange(sr[0], sr[1]) ;    

    unsigned char RGB_T[] = {      
      255, 0, 0,
      0, 255, 0,
      0, 0, 255,

      255, 255, 0,
      255, 0, 255,
      0, 255, 255,

      128, 128, 128,
      192, 192, 192,
      255, 255, 255,

      128, 0, 0,
      0, 128, 0,
      0, 0, 128,

      128, 128, 0,
      128, 0, 128,
      0, 128, 128,

      0xCC, 0xCC, 0x80,
      0xCC, 0x80, 0xCC,
      0x80, 0xCC, 0xCC,

      0xCC, 0x80, 0x80,
    };

    table->SetNumberOfTableValues(sizeof(RGB_T) / sizeof(RGB_T[0]) / 3);
    for (int i = 0; i < sizeof(RGB_T) / sizeof(RGB_T[0]); i += 3) {    
      table->SetTableValue(i / 3, RGB_T[i] / 255.0, 
        RGB_T[i + 1] / 255.0, RGB_T[i + 2] / 255.0);
    }    
    
    for (int i = 0; i < 2; i++)
    {
      m_Meshes[i]->pMapper->ImmediateModeRenderingOn();
      m_Meshes[i]->pMapper->ScalarVisibilityOn();
      m_Meshes[i]->pMapper->SetColorModeToMapScalars();
      m_Meshes[i]->pMapper->SetScalarModeToUsePointData();  
      m_Meshes[i]->pMapper->ColorByArrayComponent(0, 0);
      m_Meshes[i]->pMapper->SetLookupTable(table);
      m_Meshes[i]->pMapper->SetUseLookupTableScalarRange(1);

      m_Meshes[i]->pMapper->Update();
      m_Meshes[i]->pActor->GetProperty()->SetOpacity(1.0);            
    }

    m_Meshes[0]->pActor->GetProperty()->SetLineWidth(3);
    m_Meshes[0]->pActor->GetProperty()->SetRepresentationToWireframe();
    
    m_Meshes[1]->pActor->GetProperty()->SetLineWidth(1.5);
    m_Meshes[1]->pActor->GetProperty()->SetRepresentationToWireframe();
    //m_Meshes[1]->pActor->GetProperty()->SetInterpolationToFlat();
    

    //m_Meshes[1]->pActor->GetProperty()->SetRepresentationToSurface();
    //m_Meshes[1]->pActor->GetProperty()->SetOpacity(0.9);  
    //m_Meshes[1]->pActor->GetProperty()->BackfaceCullingOff();
    //m_Meshes[1]->pActor->GetProperty()->FrontfaceCullingOff();       

        
    table->Delete();
  }
#endif

//Volume preservation feature
#ifdef DEBUG_albaOpMeshDeformation
  vtkMassProperties* props = vtkMassProperties::New();
  props->SetInput(m_Meshes[0]->pPoly); //GetVolume calls Update    
  double dblOrigVolume = props->GetVolume();

  props->SetInput(m_Meshes[1]->pPoly);  //GetVolume calls Update      
  double dblNewVolume = props->GetVolume();
  props->Delete();

  wxString szMsg = albaString::Format("Deformation done. Volume shrinkage: %.4f (Orig = %.2f, New = %.2f)\n", dblNewVolume / dblOrigVolume, dblOrigVolume, dblNewVolume);

  albaLogMessage(szMsg.ToAscii());
#ifdef _RPT0
  _RPT0(_CRT_WARN, szMsg.ToAscii());
#endif
#endif

  this->m_Rwi->CameraUpdate(); 
}

//------------------------------------------------------------------------
//Performs the deformation and creates outputs
/*virtual*/ void albaOpMeshDeformation::OnOk()
//------------------------------------------------------------------------
{
  DeformMesh();

  //create VME  
  albaVMESurface* surface;

  albaNEW(surface);
  surface->SetName(albaString::Format("Deformed %s", m_Input->GetName()));
  surface->SetData(m_Meshes[1]->pPoly, 0, albaVMEGeneric::ALBA_VME_REFERENCE_DATA);

  albaDEL(m_Output);
  m_Output = surface;

  //check, if we need to save also curves
  if (m_SaveODC[0] != 0 || m_SaveODC[1] != 0)
  {
    //convert curves from internal format into VMEs
    int nCount = (int)m_Curves.size();
    SetNumberOfControlCurves(nCount);

    wxChar chLabels[2] = { wxT('O'), wxT('D') };
    albaVME** pCVMEs[2] = { m_OriginalCurves, m_DeformedCurves };
    for (int i = 0; i < 2; i++)
    {
      if (m_SaveODC[i] == 0)
        continue; //this type of curves is not to be saved

      for (int j = 0; j < nCount; j++)
      {
        albaVMEPolylineGraph* vme;
        albaNEW(vme);

        wxString szOldName;
        if (pCVMEs[i][j] == NULL) //if the curve was not generated, it must have name
          szOldName = wxT("generated");
        else
        {
          //we have some curve
          szOldName = pCVMEs[i][j]->GetName();
          if (
            szOldName.Matches(wxT("*_OC#?*")) || 
            szOldName.Matches(wxT("*_DC#?*"))
            )
          {
            //if the name contains special suffix, remove it
            szOldName = szOldName.BeforeLast(wxT('_'));
          }
        }

        vme->SetName(albaString::Format(wxT("%s_%cC#%d"), szOldName.ToAscii(), chLabels[i], j));
        vme->SetData(m_Curves[j]->pPolys[i], 0, albaVMEGeneric::ALBA_VME_REFERENCE_DATA);

        albaDEL(pCVMEs[i][j]);    //remove the previous VME
        pCVMEs[i][j] = vme;
      }//end for j
    } //end for i
  }//end if
}

#pragma endregion //GUI Handlers

#pragma region //Edit operations
//------------------------------------------------------------------------
//Creates an undo item for the given curve.
void albaOpMeshDeformation::CreateUndo(int index, int nAction, int nFlags)
{
  if (m_BDoNotCreateUndo)
    return; //the construction of undo item is forbidden (typically because of OnUndo)

  CONTROL_CURVE* pCurve = m_Curves[index];

  UNDO_ITEM* pUndo = new UNDO_ITEM;
  pUndo->nAction = MAKE_ACTION(nAction, nFlags);
  pUndo->nCurveIndex = index;
  for (int i = 0; i < 2; i++)
  {
    if ((nFlags & (1 << i)) == 0 || nAction == EDM_ADD_CURVE)
      pUndo->pOC_DC[i] = NULL;
    else
    {
      //pPolys should reflect the current state of the curve
      pUndo->pOC_DC[i] = new albaPolylineGraph();
      pUndo->pOC_DC[i]->CopyFromPolydata(pCurve->pPolys[i]);
    }
  }

  //process correspondences
  if ((nFlags & (1 << 2)) == 0 || pCurve->pCCList == NULL)      
    pUndo->pCC = NULL;
  else
  {
    pUndo->pCC = vtkIdList::New();
    pUndo->pCC->DeepCopy(pCurve->pCCList);
  }

  m_UndoStack.push_back(pUndo);

  m_BttnUndo->Enable(true);  
  if (nAction == EDM_ADD_CURVE || nAction == EDM_DELETE_CURVE)
    m_BttnReset->Enable(false);
  else
  {
    m_BttnReset->Enable(true);        

    if ((nFlags & CHANGE_DC) != 0) 
    {
      m_SaveODC[1] = 1;
      m_Dialog->Update();
    }
  }
}

//------------------------------------------------------------------------
//Adds a new control curve.
//N.B. this does everything. It creates VTK pipeline, internal structures,
//undo item for this operation and also updates the visibility.
void albaOpMeshDeformation::AddControlCurve(albaPolylineGraph* pOC, 
                     albaPolylineGraph* pDC, vtkIdList* pCC)
//------------------------------------------------------------------------
{
  _ASSERT(pOC != NULL);

  CONTROL_CURVE* pCurve = CreateControlCurve(pOC, pDC, pCC);
  UpdateControlCurveVisibility(pCurve);
  m_Curves.push_back(pCurve);  

  CreateUndo((int)m_Curves.size() - 1, EDM_ADD_CURVE, CHANGE_ALL);
  
  m_BttnPreview->Enable(true);
  m_BttnOK->Enable(true);
  m_BttnGenCurves->Enable(false);
}

//------------------------------------------------------------------------
//Deletes the control curve at the given index.
//N.B. this does everything. It destroys VTK pipeline and
//creates undo item for this operation.
void albaOpMeshDeformation::DeleteControlCurve(int index)
//------------------------------------------------------------------------
{
  //create undo item
  CreateUndo(index, EDM_DELETE_CURVE, CHANGE_ALL);  

  CONTROL_CURVE* pCurve = m_Curves[index];
  m_Curves.erase(m_Curves.begin() + index);

  for (int i = 0; i < 3; i++)
  {
    m_Rwi->m_RenFront->RemoveActor(pCurve->pActors[i]);

    vtkDEL(pCurve->pPolys[i]);
    vtkDEL(pCurve->pTubes[i]);
    vtkDEL(pCurve->pActors[i]);
  }

  vtkDEL(pCurve->pGlyphs[0]);
  vtkDEL(pCurve->pGlyphs[1]); 

  cppDEL(pCurve->pPolyLines[0]);
  cppDEL(pCurve->pPolyLines[1]);
  vtkDEL(pCurve->pCCList);

  bool bNoCurve = m_Curves.empty();
  m_BttnPreview->Enable(!bNoCurve);
  m_BttnOK->Enable(!bNoCurve);
  m_BttnGenCurves->Enable(bNoCurve);  
  delete pCurve;

  //update visualization
  if (m_SelectedCurve == pCurve)
  {
    m_SelectedCurve = NULL;
    m_Rwi->m_RenFront->RemoveActor(m_SelPointActor);
  }
  this->m_Rwi->CameraUpdate();
}

//------------------------------------------------------------------------
//Finds the point on iType curve (0 for OC, 1 for DC) closest to the given position.
//Returns -1, if there is no point within the glyph tolerance
int albaOpMeshDeformation::FindPoint(CONTROL_CURVE* pCurve, int iType, double pos[3])
//------------------------------------------------------------------------
{
  if (iType == 2)
    return -1;      //correspondences have no points

  vtkPolyData* pPoly = pCurve->pPolys[iType];
  int nRetID = pPoly->FindPoint(pos);
  if (nRetID >= 0)
  {
    double* pCoords = pPoly->GetPoint(nRetID);

    //check if the distance is within the glyph radius
    double dblDist = vtkMath::Distance2BetweenPoints(pos, pCoords);
    double dblR = m_Spheres[iType]->GetRadius();
    if (dblDist > dblR*dblR*1.02) //2% tolerance
      return -1;  //no, then we are sorry but no point was found
  }

  return nRetID;
}

//------------------------------------------------------------------------
//Selects an original curve, a deformed curve, a vertex or a correspondence.  
//The selection is done for the given control curve and iType is 0 for OC,
//1 for DC and 2 for CC; pos is the position where the pick event happened
void albaOpMeshDeformation::SelectControlCurve(CONTROL_CURVE* pCurve, 
                                              int iType, double pos[3])
//------------------------------------------------------------------------
{  
  if (iType == 2)
    return; //TODO: selection of correspondences is not supported yet
 
  int nID = FindPoint(pCurve, iType, pos);
  if (nID >= 0)
  {        
    //vertex has been selected
    m_SelectedCurve = pCurve;
    m_SelectedObjId = nID;    

    if (iType == 0)
    {
      m_SelectedObjType = SELECT_OC_VERT;
      m_SelPointGlyph->SetRadius(m_SphereRadius[0] * 1.2);
    }
    else
    {
      m_SelectedObjType = SELECT_DC_VERT;
      m_SelPointGlyph->SetRadius(m_SphereRadius[1] * 1.2);
    }

    m_SelPointGlyph->SetCenter(pCurve->pPolys[iType]->GetPoint(m_SelectedObjId));
    m_SelPointGlyph->Update();
    UpdateSelectionVisibility();   
  }
}

//------------------------------------------------------------------------
//Initializes the movement of the point denoted by pos. 
//The curve is defined by its index and iType is 0 for OC, 1 for DC
//After the calling of this routine, any number of MovePoint routines
//is called to move the point. The process ends when EndMovePoint is called
void albaOpMeshDeformation::BeginMovePoint(int index, int iType, double pos[3])
//------------------------------------------------------------------------
{ 
  if (iType == 2 || (iType == 0 && !m_ChckEditOC->IsChecked()))
    return; //prevent modification of original curves

  CONTROL_CURVE* pCurve = m_Curves[index];
  int nPointId = FindPoint(pCurve, iType, pos);
  if (nPointId >= 0)
  {
    //there is a point => we can start with moving
    //so select the vertex that will be moved, if it is not already selected
    bool bMustUndo = false;
    int nUndoAction = MAKE_ACTION(EDM_MOVE_POINTS, (1 << iType));

    if (m_SelectedCurve != pCurve || m_SelectedObjId != nPointId ||
      m_SelectedObjType != SELECT_OC_VERT + iType)
    {
      //if not selected, select it
      bMustUndo = true;
      SelectControlCurve(pCurve, iType, pos);
    }
    else
    {
      //check if we have already undo for the movement of this point
      if (m_UndoStack.empty())
        bMustUndo = true;
      else
      {
        UNDO_ITEM* pLastUndo = m_UndoStack[m_UndoStack.size() - 1];
        bMustUndo = (pLastUndo->nAction != nUndoAction || 
          pLastUndo->nCurveIndex != index);        
      }
    }

    if (bMustUndo)
    {
      //create undo item
      //NOTE: the previous checks do not ensure the proper construction of
      //undo item for every point, however, it ensures the undo of curve
      CreateUndo(index, EDM_MOVE_POINTS, (1 << iType));     
    }
    
    double* pcoords = pCurve->pPolys[iType]->GetPoint(nPointId);
    for (int i = 0; i < 3; i++) {
      m_PointMoveCorrection[i] = pcoords[i] - pos[i];
    }
    m_BPointMoveActive = true;
    m_Picker->SetContinuousPicking(true);
  }  
}

//------------------------------------------------------------------------
//Finalizes the movement of the point
//N.B. see BeginMovePoint
void albaOpMeshDeformation::EndMovePoint()
//------------------------------------------------------------------------
{
  m_Picker->SetContinuousPicking(false);
  m_BPointMoveActive = false;
}

//------------------------------------------------------------------------
//Moves the current point into the new position
//N.B. see BeginMovePoint
void albaOpMeshDeformation::MovePoint(double pos[3])
//------------------------------------------------------------------------
{
  _VERIFY_RET(m_BPointMoveActive && m_SelectedCurve != NULL);    

  double ptPos[3];
  for (int i = 0; i < 3; i++) {
    ptPos[i] = pos[i] + m_PointMoveCorrection[i];
  }

  albaPolylineGraph* pGraph = m_SelectedCurve->pPolyLines[m_SelectedObjType - SELECT_OC_VERT];
  pGraph->SetVertexCoords(m_SelectedObjId, ptPos);

  m_SelPointGlyph->SetCenter(ptPos);
  m_SelPointGlyph->Update();

  int nFlags = (1 << (m_SelectedObjType - SELECT_OC_VERT));
  if (FindCorrespondence(m_SelectedCurve->pCCList, 
    m_SelectedObjId, m_SelectedObjType - SELECT_OC_VERT) >= 0)
    nFlags |= CHANGE_CC;

  UpdateControlCurve(m_SelectedCurve, nFlags);
  UpdateControlCurveVisibility(m_SelectedCurve);   
}

//------------------------------------------------------------------------
//Adds a new point with the specified coordinates on a curve. 
//The curve is defined by its index and iType is 0 for OC, 1 for DC
//The point is inserted into the closest segment
void albaOpMeshDeformation::AddPoint(int index, int iType, double pos[3])
//------------------------------------------------------------------------
{
  if (iType == 2 || (iType == 0 && !m_ChckEditOC->IsChecked()))
    return; //cannot modify correspondence or OC (if it is not allowed)

  CONTROL_CURVE* pCurve = m_Curves[index];
  if (FindPoint(pCurve, iType, pos) >= 0)
    return;   //this point already exists on the curve

  double dblMinDist = DBL_MAX;
  int nMinEdgeID = -1;

  albaPolylineGraph* pGraph = pCurve->pPolyLines[iType];
  int nCount = pGraph->GetNumberOfEdges();
  for (int i = 0; i < nCount; i++)
  {
    const albaPolylineGraph::Edge* pEdge = pGraph->GetConstEdgePtr(i);
    
    //compute the coordinates of the point on the edge
    //that is closest to the given position
    double A[3], B[3], Q[3], u[3], v[3];
    pGraph->GetConstVertexPtr(pEdge->GetVertexId(0))->GetCoords(A);
    pGraph->GetConstVertexPtr(pEdge->GetVertexId(1))->GetCoords(B);

    for (int j = 0; j < 3; j++) 
    {
      u[j] = B[j] - A[j];
      v[j] = pos[j] - A[j];
    }

    //vtkMath::Normalize(u);
    double t = vtkMath::Dot(u, v) / vtkMath::Dot(u, u);

    //perpendicular point may lie outside the edge
    if (t < 0.0)
      t = 0.0;
    else if (t > 1.0)
      t = 1.0;
    
    for (int j = 0; j < 3; j++) {
      Q[j] = A[j] + u[j]*t;      
    }
  
    //compute distance between these two points
    double dblDist = vtkMath::Distance2BetweenPoints(Q, pos);
    if (dblDist < dblMinDist)
    {
      dblMinDist = dblDist;
      nMinEdgeID = i;
    }
  }

  if (nMinEdgeID >= 0)
  {
    //we have found the edge, now add a new point
    //but first, create UNDO item
    CreateUndo(index, EDM_ADD_POINT, (1 << iType));

    const albaPolylineGraph::Edge* pEdge = pGraph->GetConstEdgePtr(nMinEdgeID);
    int branchId = pEdge->GetBranchId();
    int V0 = pEdge->GetVertexId(0);
    int V1 = pEdge->GetVertexId(1);

    //now remove the original edge, which creates new branch
    pGraph->DeleteEdge(nMinEdgeID);
    
    //add new vertex
    pGraph->AddNewVertex(pos);
    int V2 = pGraph->GetMaxVertexId();    
        
    //and create new edges
    pGraph->AddNewEdge(V0, V2);
    pGraph->AddExistingEdgeToBranch(branchId, pGraph->GetMaxEdgeId());

    pGraph->AddNewEdge(V2, V1);
    pGraph->AddExistingEdgeToBranch(branchId, pGraph->GetMaxEdgeId());

    //avoid branches expansion
    //pGraph->MergeBranches(branchId, pGraph->GetMaxBranchId());
    pGraph->MergeSimpleJoinedBranches();

    //now, the graph is modified => update everything
    UpdateControlCurve(pCurve, (1 << iType));
    UpdateControlCurveVisibility(pCurve);   
  }
}

//------------------------------------------------------------------------
//Removes the existing point with the specified coordinates from a curve
//The curve is defined by its index and iType is 0 for OC, 1 for DC
void albaOpMeshDeformation::DeletePoint(int index, int iType, double pos[3])
//------------------------------------------------------------------------
{
  if (iType == 2 || (iType == 0 && !m_ChckEditOC->IsChecked()))
    return; //cannot modify correspondence or OC (if it is not allowed)

  CONTROL_CURVE* pCurve = m_Curves[index];
  int nVertices = pCurve->pPolyLines[iType]->GetNumberOfVertices();
  if (nVertices <= 2)
    return; //no more points to be deleted

  int nPtID = FindPoint(pCurve, iType, pos);
  if (nPtID >= 0)
  {
    //we have valid point to be removed
    //check, if the point or any of its successors has any correspondence
    bool bUpdateCC = false;
    if (pCurve->pCCList != NULL)
    {
      int nCount = pCurve->pCCList->GetNumberOfIds();
      vtkIdType* pIds = pCurve->pCCList->GetPointer(iType);
      
      for (int i = 0; i < nCount; i += 2)
      {        
        if (pIds[i] == nPtID || pIds[i] == nVertices - 1) {
          bUpdateCC = true; break;
        }
      }
    }

    //create undo item
    CreateUndo(index, EDM_DELETE_POINT, (1 << iType) | (bUpdateCC ? CHANGE_CC : 0));
    
    albaPolylineGraph* pGraph = pCurve->pPolyLines[iType];
    const albaPolylineGraph::Vertex* pVertex = pGraph->GetConstVertexPtr(nPtID);    
    int nDegree = pVertex->GetDegree();
    if (nDegree == 1)
    {
      //this is the end point, delete the edge and point,
      //which creates a new branch => remove new branch after that
      pGraph->DeleteEdge(pVertex->GetEdgeId(0));      
    }
    else if (nDegree == 2)
    {
      //this is a point on curve, it will be easier
      //first, construct a new edge between neighboring points       
      pGraph->AddNewEdge(pVertex->GetVertexId(0), pVertex->GetVertexId(1));
      pGraph->AddNewBranch(pVertex->GetVertexId(0)); //create also a new branch for this new edge
      pGraph->AddExistingEdgeToBranch(pGraph->GetMaxBranchId(), pGraph->GetMaxEdgeId());

      //and now delete the original edge and vertex
      pGraph->DeleteEdge(pVertex->GetEdgeId(0));         
      pGraph->DeleteEdge(pVertex->GetEdgeId(0));
    }
    else
    {
      //bifurcation
      
      //create Q = set of points connected with the vertex 
      //for every point from Q find the point from Q with the longest distance
      //and if there is no edge between these two points, construct it
      //remove original edges
      vtkALBASmartPointer< vtkIdList > ids;
      pVertex->GetVerticesIdList(ids);        

      int nCount = ids->GetNumberOfIds();
      vtkIdType* pPtr = ids->GetPointer(0);
      for (int i = 0; i < nCount; i++)
      {
        double A[3], B[3];
        double dblMaxDist = 0.0;
        int jPos = -1;

        const albaPolylineGraph::Vertex* pVA = pGraph->GetConstVertexPtr(pPtr[i]);
        pVA->GetCoords(A);     
        for (int j = i + 1; j < nCount; j++)
        {
          //check if the edge pPtr[i] and pPtr[j] does not already exist
          if (pVA->FindVertexId(pPtr[j]) >= 0)
            continue;

          pGraph->GetVertexCoords(pPtr[j], B);
          double dblDist = vtkMath::Distance2BetweenPoints(A, B);
          if (dblDist > dblMaxDist)
          {
            dblMaxDist = dblDist;
            jPos = j;
          }
        } //end for j

        if (jPos >= 0)
        {
          //we must construct a new edge between points 
          //pPtr[i] and pPtr[jPos]
          pGraph->AddNewEdge(pPtr[i], pPtr[jPos]);
          pGraph->AddNewBranch(pPtr[i]); //create also a new branch for this new edge
          pGraph->AddExistingEdgeToBranch(pGraph->GetMaxBranchId(), pGraph->GetMaxEdgeId());            
        }
      } //end for i  

      //now delete the original edges and the point
      //which will create another explosion of branches
      for (int i = 0; i < nDegree; i++)
      {
        pGraph->DeleteEdge(pVertex->GetEdgeId(0));
      }      
    } //end if (nDegree ...)

    pGraph->DeleteVertex(nPtID); 

    //avoid branches expansion
    pGraph->MergeSimpleJoinedBranches();

    //if there is any correspondence to be updated, do it
    if (bUpdateCC)
    {      
      //first, remove the correspondence for the deleted vertex
      int nCorPos = FindCorrespondence(pCurve->pCCList, nPtID, iType);
      if (nCorPos >= 0)
        DeleteCorrespondence(pCurve->pCCList, nCorPos);

      //and then update the indices for other points
      //as albaPolyGraph::DeleteVertex() swaps the last vertex and
      //the vertex to be deleted and then deletes the currently last vertex,
      //indices remain the same except for the former last vertex
      int nCount = pCurve->pCCList->GetNumberOfIds();        
      vtkIdType* pIds = pCurve->pCCList->GetPointer(iType);

      for (int i = 0; i < nCount; i += 2)
      {
        if (pIds[i] == nVertices - 1) {
          pIds[i] = nPtID; break;
        }
      }        
    }

    //now, the graph is modified => update everything
    UpdateControlCurve(pCurve, (1 << iType) | (bUpdateCC ? CHANGE_CC : 0));
    UpdateControlCurveVisibility(pCurve);  
  } //end if nPtID >= 0
}

//------------------------------------------------------------------------
//Finds a correspondence for the given point from the specified curve
//It returns -1, if there is no such correspondence; index to CC list otherwise
int albaOpMeshDeformation::FindCorrespondence(vtkIdList* pCL, int ptIndex, int iCurve)
//------------------------------------------------------------------------
{
  if (pCL == NULL)
    return -1;

  int nCount = pCL->GetNumberOfIds();
  vtkIdType* pPtr = pCL->GetPointer(0);
  for (int i = 0; i < nCount; i += 2)
  {
    if (pPtr[i + iCurve] == ptIndex)
      return i;
  }

  return -1;
}

//------------------------------------------------------------------------
//Deletes the correspondence at the given index
void albaOpMeshDeformation::DeleteCorrespondence(vtkIdList*& pCL, int index)
//------------------------------------------------------------------------
{
  _ASSERT(pCL != NULL);

  int nCount = pCL->GetNumberOfIds();  
  if (nCount == 2)  //if there is no other correspondence
    vtkDEL(pCL);
  else
  {
    vtkIdType* pIds = pCL->GetPointer(0);
    if (index < nCount - 2)
    {
      //if it is not the last correspondence, swap it 
      pIds[index] = pIds[nCount - 2];
      pIds[index + 1] = pIds[nCount - 1];
    }

    //remove the last two ids
    pCL->SetNumberOfIds(nCount - 2);
  }
}

//------------------------------------------------------------------------
//Initializes the correspondence construction
void albaOpMeshDeformation::BeginAddCorrespondence( int index, int iType, double pos[3] )
//------------------------------------------------------------------------
{
  CONTROL_CURVE* pCurve = m_Curves[index];
  int nPointId = FindPoint(pCurve, iType, pos);
  if (nPointId >= 0)
  {
    //check whether this point has no correspondence 
    if (FindCorrespondence(pCurve->pCCList, nPointId, iType) >= 0)
      return; //already has a correspondence

    //there is a point => we can start the construction
    SelectControlCurve(pCurve, iType, pos);
    m_BCorrespondenceActive = true;
  }
}

//------------------------------------------------------------------------
//Finalizes the correspondence construction
void albaOpMeshDeformation::EndAddCorrespondence( int index, int iType, double pos[3] )
//------------------------------------------------------------------------
{
  _VERIFY_RET(m_BCorrespondenceActive && m_SelectedCurve != NULL);  
  if (iType == m_SelectedObjType - SELECT_OC_VERT)
    return; //the second point must lie on the other curve

  CONTROL_CURVE* pCurve = m_Curves[index];
  int nPointId = FindPoint(pCurve, iType, pos);
  if (nPointId >= 0)
  {
    //check whether this point has no correspondence 
    if (FindCorrespondence(pCurve->pCCList, nPointId, iType) >= 0)
      return; //already has a correspondence

    //everything is OK, so let us create undo item
    CreateUndo(index, EDM_ADD_CORRESPONDENCE, CHANGE_CC);

    //and add new correspondence
    int nOCPtID, nDCPtID;
    if (iType == 0)
    {
      nOCPtID = nPointId;
      nDCPtID = m_SelectedObjId;
    }
    else
    {
      nOCPtID = m_SelectedObjId;
      nDCPtID = nPointId;      
    }    

    if (pCurve->pCCList == NULL)
      pCurve->pCCList = vtkIdList::New();
    
    pCurve->pCCList->InsertNextId(nOCPtID);
    pCurve->pCCList->InsertNextId(nDCPtID);
    m_BCorrespondenceActive = false;

    UpdateControlCurve(pCurve, CHANGE_CC);
    UpdateControlCurveVisibility(pCurve);
  }
}

//------------------------------------------------------------------------
//Deletes the correspondence close to the given point coordinates
//The control curve to be modified is denoted by the given index
void albaOpMeshDeformation::DeleteCorrespondence(int index, double pos[3])
//------------------------------------------------------------------------
{  
  CONTROL_CURVE* pCurve = m_Curves[index];
  if (pCurve->pCCList == NULL)
    return; //no correspondence to delete

  double dblMinDist = DBL_MAX;
  int nMinCorrespondencePos = -1;
  
  int nCount = pCurve->pCCList->GetNumberOfIds();
  vtkIdType* pIds = pCurve->pCCList->GetPointer(0);
  for (int i = 0; i < nCount; i += 2)
  {    
    //compute the coordinates of the point on the edge
    //that is closest to the given position
    double A[3], B[3], Q[3], u[3], v[3];
    pCurve->pPolyLines[0]->GetConstVertexPtr(pIds[i])->GetCoords(A);
    pCurve->pPolyLines[1]->GetConstVertexPtr(pIds[i + 1])->GetCoords(B);

    for (int j = 0; j < 3; j++) 
    {
      u[j] = B[j] - A[j];
      v[j] = pos[j] - A[j];
    }

    //vtkMath::Normalize(u);
    double t = vtkMath::Dot(u, v) / vtkMath::Dot(u, u);

    //perpendicular point may lie outside the edge
    if (t < 0.0)
      t = 0.0;
    else if (t > 1.0)
      t = 1.0;

    for (int j = 0; j < 3; j++) {
      Q[j] = A[j] + u[j]*t;      
    }

    //compute distance between these two points
    double dblDist = vtkMath::Distance2BetweenPoints(Q, pos);
    if (dblDist < dblMinDist)
    {
      dblMinDist = dblDist;
      nMinCorrespondencePos = i;
    }
  }

  if (nMinCorrespondencePos >= 0)
  {
    //we have found the edge, now add a new point
    //but first, create UNDO item
    CreateUndo(index, EDM_DELETE_CORRESPONDENCE, CHANGE_CC);
    DeleteCorrespondence(pCurve->pCCList, nMinCorrespondencePos);    

    //now, the graph is modified => update everything
    UpdateControlCurve(pCurve, CHANGE_CC);
    UpdateControlCurveVisibility(pCurve);   
  }
}

#pragma endregion  //Edit operations

//------------------------------------------------------------------------
//Deforms the input mesh producing output mesh
void albaOpMeshDeformation::DeformMesh()
//------------------------------------------------------------------------
{
  switch (m_DeformationMode)
  {
  case DEM_BLANCO:
    DeformMeshT< vtkALBAPolyDataDeformation_M1 >(); 
    break;
  case DEM_SEPPLANES:
    DeformMeshT< vtkALBAPolyDataDeformation_M2 >(); 
    break;
  default:
    DeformMeshT< vtkALBAPolyDataDeformation >(); 
    break;
  }
}

//------------------------------------------------------------------------
//Template for various methods
template < class T >
void albaOpMeshDeformation::DeformMeshT()
//------------------------------------------------------------------------
{
  vtkALBASmartPointer< T > md;

  md->SetInput(m_Meshes[0]->pPoly);    
  md->SetOutput(m_Meshes[1]->pPoly);

  int nCount = (int)m_Curves.size();
  md->SetNumberOfSkeletons(nCount);
  for (int i = 0; i < nCount; i++)
  {
    CONTROL_CURVE* pCurve = m_Curves[i];
    md->SetNthSkeleton(i, pCurve->pPolys[0], pCurve->pPolys[1], pCurve->pCCList);
  }

  md->Update();
  md->SetOutput(NULL);    //disconnect output from the filter
}

//------------------------------------------------------------------------
//Creates internal data structures used in the editor.
//Returns false, if an error occurs (e.g. unsupported input)
/*virtual*/ bool albaOpMeshDeformation::CreateInternalStructures()
//------------------------------------------------------------------------
{
  albaVMESurface* surface = albaVMESurface::SafeDownCast(m_Input); 
  _VERIFY_RETVAL(surface != NULL, false);  

  vtkPolyData* pPoly = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  _VERIFY_CMD_RPT(pPoly != NULL, false);
  pPoly->Register(NULL);  //this prevents its destruction
    
  m_Meshes[0] = CreateMesh(pPoly);                //original mesh
  m_Meshes[1] = CreateMesh(vtkPolyData::New());   //deformed mesh, it is empty now
  m_Meshes[1]->pActor->GetProperty()->SetRepresentationToWireframe();
  m_Meshes[1]->pActor->GetProperty()->SetColor(1, 0, 0); //red

  double bounds[6];
  pPoly->GetBounds(bounds);
  double dblMaxDim = fabs(bounds[1] - bounds[0]);
  for (int i = 2; i < 6; i += 2)
  {
    double dblDim = fabs(bounds[i + 1] - bounds[i]);
    if (dblDim > dblMaxDim)
      dblMaxDim = dblDim;
  }

  //Compute optimal radius
  m_SphereRadius[0] = 3*dblMaxDim / 400;  //original curves are smaller (75%)
  m_SphereRadius[1] = dblMaxDim / 100;    //deformed curves

  for (int i = 0; i < 2; i++)
  {
    vtkNEW(m_Spheres[i]);
    m_Spheres[i]->SetRadius(m_SphereRadius[i]);  
  }

  vtkNEW(m_SelPointGlyph);
  vtkPolyDataMapper* pMapper = vtkPolyDataMapper::New();
  pMapper->SetInput(m_SelPointGlyph->GetOutput());

  vtkNEW(m_SelPointActor);
  m_SelPointActor->SetMapper(pMapper);
  m_SelPointActor->GetProperty()->SetColor(1, 0, 0);  //red
  m_SelPointActor->PickableOff();
  pMapper->Delete();  //mapper is no longer needed

  for (int i = 0; i < m_NumberOfCurves; i++)
  {
    albaPolylineGraph* pOC = CreatePolylineGraph(m_OriginalCurves[i]);
    _VERIFY_CMD(pOC != NULL, continue);           //it should be compatible

    CONTROL_CURVE* pCurve = CreateControlCurve(pOC, CreatePolylineGraph(
      m_DeformedCurves[i]), m_CurvesCorrespondence[i]);
    m_Curves.push_back(pCurve);
  }

  return true;
}

//------------------------------------------------------------------------
//Destroys internal data structures created by CreateInternalStructures
/*virtual*/ void albaOpMeshDeformation::DeleteInternalStructures()
//------------------------------------------------------------------------
{
  //destroy undo stack
  int nCount = (int)m_UndoStack.size();
  for(int i = 0; i < nCount;i++)
  {
    UNDO_ITEM* pItem = m_UndoStack[i];
    cppDEL(pItem->pOC_DC[0]);
    cppDEL(pItem->pOC_DC[1]);
    vtkDEL(pItem->pCC);

    delete pItem;
  }
  m_UndoStack.clear();

  //destroy curves
  nCount = (int)m_Curves.size();
  for(int i = 0; i < nCount;i++)
  {
    CONTROL_CURVE* pItem = m_Curves[i];
    for (int j = 0; j < 3; j++)
    {
      vtkDEL(pItem->pPolys[j]);
      vtkDEL(pItem->pTubes[j]);
      vtkDEL(pItem->pActors[j]);
    }

    vtkDEL(pItem->pGlyphs[0]);
    vtkDEL(pItem->pGlyphs[1]);
    vtkDEL(pItem->pCCList);
    cppDEL(pItem->pPolyLines[0]);
    cppDEL(pItem->pPolyLines[1]);

    delete pItem;
  }
  m_Curves.clear();

  //and destroy meshes
  for (int i = 0; i < 2; i++)
  {
    vtkDEL(m_Meshes[i]->pActor);
    vtkDEL(m_Meshes[i]->pMapper);
    vtkDEL(m_Meshes[i]->pPoly);
    cppDEL(m_Meshes[i]);
  }

  //and destroys spheres
  vtkDEL(m_Spheres[0]);
  vtkDEL(m_Spheres[1]);
  vtkDEL(m_SelPointGlyph);
  vtkDEL(m_SelPointActor);
}


//------------------------------------------------------------------------
//Creates albaPolyLineGraph for the given vme
albaPolylineGraph* albaOpMeshDeformation::CreatePolylineGraph(albaVME* vme)
//------------------------------------------------------------------------
{
  if (vme == NULL)
    return NULL;  

  vtkPolyData* pPoly = vtkPolyData::SafeDownCast(
    vme->GetOutput()->GetVTKData());  

  _VERIFY_RETVAL(NULL != pPoly, NULL);  
  pPoly->Update();  //to force construction of vtkPoints

  albaPolylineGraph* pRet = new albaPolylineGraph();
  if (pRet->CopyFromPolydata(pPoly))
  {
    pRet->MergeSimpleJoinedBranches();
    return pRet;  //successfully constructed
  }

  _ASSERT(false);
  delete pRet;
  return NULL;
}

//------------------------------------------------------------------------
//Creates a deep copy of polylinegraph 
albaPolylineGraph* albaOpMeshDeformation::CreateCopyOfPolylineGraph(albaPolylineGraph* input)
//------------------------------------------------------------------------
{
  if (input == NULL)
    return NULL;

  vtkPolyData* pPoly = vtkPolyData::New();
  input->CopyToPolydata(pPoly);

  albaPolylineGraph* pRet = new albaPolylineGraph();
  bool bOK = pRet->CopyFromPolydata(pPoly);
  pPoly->Delete();  //no longer needed

  if (bOK)  
    pRet->MergeSimpleJoinedBranches();
  else
  {
  
    _ASSERT(false);
    cppDEL(pRet);
  }

  return pRet;  
}

//------------------------------------------------------------------------
//This method creates the internal mesh structure.
//It constructs also the VTK pipeline.
albaOpMeshDeformation::MESH* albaOpMeshDeformation::CreateMesh(vtkPolyData* pMesh)
//------------------------------------------------------------------------
{
  MESH* pRet = new MESH;
  memset(pRet, 0, sizeof(MESH));

  pRet->pPoly = pMesh;
  pRet->pMapper = vtkPolyDataMapper::New();
  pRet->pActor = vtkActor::New();
  pRet->pActor->SetMapper(pRet->pMapper);
  pRet->pActor->PickableOff();

  UpdateMesh(pRet);
  return pRet;
}

//------------------------------------------------------------------------
//Creates the control curve structure.
//It constructs also the VTK pipeline.
albaOpMeshDeformation::CONTROL_CURVE* albaOpMeshDeformation::
  CreateControlCurve(albaPolylineGraph* pOC, albaPolylineGraph* pDC, vtkIdList* pCC)
//------------------------------------------------------------------------
{
  _ASSERT(m_Spheres[0] != NULL && m_Spheres[1] != NULL);

  CONTROL_CURVE* pRet = new CONTROL_CURVE;
  memset(pRet, 0, sizeof(CONTROL_CURVE));

  pRet->pPolyLines[0] = pOC;
  pRet->pPolyLines[1] = pDC;
  pRet->pCCList = pCC;
    
  for (int i = 0; i < 3; i++)
  {
    pRet->pTubes[i] = vtkTubeFilter::New();
    pRet->pTubes[i]->UseDefaultNormalOff();        
    pRet->pTubes[i]->SetCapping(true);
    pRet->pTubes[i]->SetNumberOfSides(5);

    vtkPolyDataMapper* pMapper = vtkPolyDataMapper::New();
    if (i == 2) 
    {
      //correspondences are just tubes no spheres
      pRet->pTubes[i]->SetRadius(m_Spheres[0]->GetRadius() / 4);
      pMapper->SetInput(pRet->pTubes[i]->GetOutput());
    }
    else
    {
      pRet->pTubes[i]->SetRadius(m_Spheres[i]->GetRadius() / 2);

      pRet->pGlyphs[i] = vtkGlyph3D::New();
      pRet->pGlyphs[i]->SetSource(m_Spheres[i]->GetOutput());
      pRet->pGlyphs[i]->SetScaleModeToDataScalingOff();
      pRet->pGlyphs[i]->SetRange(0.0,1.0);

      vtkAppendPolyData* pAppendPoly = vtkAppendPolyData::New();
      pAppendPoly->AddInput(pRet->pTubes[i]->GetOutput());
      pAppendPoly->AddInput(pRet->pGlyphs[i]->GetOutput());
      pMapper->SetInput(pAppendPoly->GetOutput());
      pAppendPoly->Delete();  //this is no longer needed
    }

    pRet->pActors[i] = vtkActor::New();
    pRet->pActors[i]->SetMapper(pMapper);
    pMapper->Delete();        //this is no longer needed
  }
   
  pRet->pActors[0]->GetProperty()->SetColor(0.0,0.4,0.0); //original curve
  pRet->pActors[1]->GetProperty()->SetColor(1.0,0.8,0.0); //deformed curve
  pRet->pActors[2]->GetProperty()->SetColor(0.5,0.6,1.0); //correspondences

  UpdateControlCurve(pRet);
  return pRet;
}

//------------------------------------------------------------------------
//Updates the VTK pipeline for the given curve.
//N.B. It does not connects actors into the renderer.
void albaOpMeshDeformation::UpdateMesh(MESH* pMesh)
//------------------------------------------------------------------------
{
  if (pMesh->pPoly == NULL)
    pMesh->pActor->SetVisibility(0);
  else
  {
    pMesh->pMapper->SetInput(pMesh->pPoly);
    pMesh->pMapper->Update();

    pMesh->pActor->SetVisibility(1);
  }  
}

//------------------------------------------------------------------------
// Updates the VTK pipeline for the given curve
//Flags define what needs to be updated, bit 0 corresponds to the
//original curve, 1 to deformed and 2 to correspondence 
//N.B. It does not connects actors into the renderer.
void albaOpMeshDeformation::UpdateControlCurve(CONTROL_CURVE* pCurve, int flags)
//------------------------------------------------------------------------
{
  for (int i = 0; i < 3; i++)
  {
    if ((flags & (1 << i)) == 0)
      continue; //this is not to be updated
    
    if (i != 2)
    {
      //curves
      if (pCurve->pPolyLines[i] == NULL) {
        vtkDEL(pCurve->pPolys[i]);
      }
      else
      {
        //if no polydata exists, create it
        if (pCurve->pPolys[i] == NULL)
          pCurve->pPolys[i] = vtkPolyData::New();

        _VERIFY(pCurve->pPolyLines[i]->CopyToPolydata(pCurve->pPolys[i]));        
      }
    }
    else
    {
      //correspondence
      if (pCurve->pCCList != NULL && pCurve->pCCList->GetNumberOfIds() == 0) {
        vtkDEL(pCurve->pCCList);  //no correspondence detected
      }

      if (pCurve->pCCList == NULL) {
        vtkDEL(pCurve->pPolys[i]);
      }
      else
      {
        _ASSERT(pCurve->pPolyLines[0] != NULL && 
          pCurve->pPolyLines[1] != NULL);

        //if no polydata exists, create it
        if (pCurve->pPolys[i] == NULL)
          pCurve->pPolys[i] = vtkPolyData::New();

        //and now, we will need to construct the data
        vtkPoints *points = vtkPoints::New() ;
        vtkCellArray *lines = vtkCellArray::New() ;
        vtkIdList *idlist = vtkIdList::New() ;
        
        int nCount = pCurve->pCCList->GetNumberOfIds();
				vtkIdType* ptIdx = pCurve->pCCList->GetPointer(0);
        for (int j = 0; j < nCount; j += 2)
        {
          const albaPolylineGraph::Vertex* pV0 = pCurve->pPolyLines[0]->GetConstVertexPtr(ptIdx[j]);
          const albaPolylineGraph::Vertex* pV1 = pCurve->pPolyLines[1]->GetConstVertexPtr(ptIdx[j + 1]);

          if (pV0 != NULL && pV1 != NULL)
          {
            double coords[3];
            pV0->GetCoords(coords);
            points->InsertNextPoint(coords);

            pV1->GetCoords(coords);
            points->InsertNextPoint(coords);
          }
        }

        //points are inserted, now creates connectivity
        nCount = points->GetNumberOfPoints();
        idlist->SetNumberOfIds(2);
        ptIdx = idlist->GetPointer(0);
        for (int j = 0; j < nCount; j += 2)
        {
          ptIdx[0] = j; ptIdx[1] = j + 1;
          lines->InsertNextCell(idlist);
        }

        pCurve->pPolys[i]->SetPoints(points);
        pCurve->pPolys[i]->SetLines(lines);

        idlist->Delete() ;
        lines->Delete() ;
        points->Delete() ;
      }
    }

    //now let us update VTK
    if (pCurve->pPolys[i] == NULL)
      pCurve->pActors[i]->SetVisibility(0);
    else
    {
      //set tubes and glyphs      
      if (i != 2)
        pCurve->pGlyphs[i]->SetInput(pCurve->pPolys[i]);
      pCurve->pTubes[i]->SetInput(pCurve->pPolys[i]);
      pCurve->pActors[i]->GetMapper()->Update();      
      pCurve->pActors[i]->SetVisibility(1);
    }
  }  
}

//------------------------------------------------------------------------
//Updates the visibility of the given control curve. 
//It adds/removes actors from the renderer according to the status of curves
//and the visual options specified by the user in the GUI.
//N.B. This is typically called when curve was updated. 
void albaOpMeshDeformation::UpdateControlCurveVisibility(CONTROL_CURVE* pCurve)
//------------------------------------------------------------------------
{
  for (int j = 0; j < 3; j++)
  {
    m_Rwi->m_RenFront->RemoveActor(pCurve->pActors[j]);
    if (m_CurvesVisibility[j] != 0 && pCurve->pPolys[j] != NULL)
      m_Rwi->m_RenFront->AddActor(pCurve->pActors[j]);
  }

  if (m_SelectedCurve == pCurve)
    UpdateSelectionVisibility();
  else
    this->m_Rwi->CameraUpdate();
}

//------------------------------------------------------------------------
//Updates the visibility of meshes and control curves
//It adds/removes actors from the renderer according to the
//status of their associated data and the visual options
//specified by the user in the GUI.
//This method is typically calls after UpdateMesh or 
//UpdateControlCurve is finished
void albaOpMeshDeformation::UpdateVisibility()
//------------------------------------------------------------------------
{
  //remove all actors and 
  RemoveAllActors();

  //readd actors according to the visual options
  for (int i = 0; i < 2; i++)
  {
    if (m_MeshesVisibility[i] != 0)
      m_Rwi->m_RenFront->AddActor(m_Meshes[i]->pActor);
  }
  
  int nCount = (int)m_Curves.size();
  for (int i = 0; i < nCount; i++)
  {
    CONTROL_CURVE* pCurve = m_Curves[i]; 
    int nVisCount = 0;
    for (int j = 0; j < 2; j++)
    {
      if (m_CurvesVisibility[j] != 0 && pCurve->pPolys[j] != NULL) {
        m_Rwi->m_RenFront->AddActor(pCurve->pActors[j]); nVisCount++;
      }
    }

    //correspondence requires both curves to be on
    if (m_CurvesVisibility[2] != 0 && 
      pCurve->pPolys[2] != NULL && nVisCount == 2)      
      m_Rwi->m_RenFront->AddActor(pCurve->pActors[2]);
  }

  if (m_SelectedCurve != NULL)
    UpdateSelectionVisibility();
  else
    this->m_Rwi->CameraUpdate();  
}

//------------------------------------------------------------------------
//Updates the visibility of the selected items.
//Called automatically from UpdateVisibility and UpdateControlCurveVisibility
void albaOpMeshDeformation::UpdateSelectionVisibility()
//------------------------------------------------------------------------
{
  m_Rwi->m_RenFront->RemoveActor(m_SelPointActor);  

  double* coords = NULL;
  switch(m_SelectedObjType)
  {
  case SELECT_OC_VERT:
    if (m_CurvesVisibility[0] != 0 && m_SelectedCurve->pPolys[0] != NULL)
      coords = m_SelectedCurve->pPolys[0]->GetPoint(m_SelectedObjId);
    break;

  case SELECT_DC_VERT:
    if (m_CurvesVisibility[1] != 0 && m_SelectedCurve->pPolys[1] != NULL)
      coords = m_SelectedCurve->pPolys[1]->GetPoint(m_SelectedObjId);
    break;  
  }

  if (coords == NULL)
    m_EditModeInfo->SetLabel(wxEmptyString);
  else
  {
    m_EditModeInfo->SetLabel(albaString::Format(_("Selected point: #%d [%g, %g, %g]"),
      m_SelectedObjId, coords[0], coords[1], coords[2]));
    m_SelPointGlyph->SetCenter(coords);
    m_Rwi->m_RenFront->AddActor(m_SelPointActor);
  }
  this->m_Rwi->CameraUpdate();  
}  

//------------------------------------------------------------------------
//Removes all actors from the renderer.
void albaOpMeshDeformation::RemoveAllActors()
//------------------------------------------------------------------------
{  
  m_Rwi->m_RenFront->RemoveActor(m_Meshes[0]->pActor);
  m_Rwi->m_RenFront->RemoveActor(m_Meshes[1]->pActor);

  int nCount = (int)m_Curves.size();
  for (int i = 0; i < nCount; i++)
  {
    CONTROL_CURVE* pCurve = m_Curves[i];    
    m_Rwi->m_RenFront->RemoveActor(pCurve->pActors[0]);
    m_Rwi->m_RenFront->RemoveActor(pCurve->pActors[1]);
    m_Rwi->m_RenFront->RemoveActor(pCurve->pActors[2]);
  }

  m_Rwi->m_RenFront->RemoveActor(m_SelPointActor);
}



#pragma region //Input Control Curves
//------------------------------------------------------------------------
// Sets the number of control curves.
//Sets the number of control VMEs (polylines or polylinegraphs).
//Old VMEs are copied (and preserved) 
/*virtual*/ void albaOpMeshDeformation::SetNumberOfControlCurves(int nCount)
//------------------------------------------------------------------------
{
  _VERIFY_RET(nCount >= 0);

  if (nCount == m_NumberOfCurves)
    return; //nothing to be changed

  //if the number of curves is going to be decreased, we
  //need to delete some curves
  while (m_NumberOfCurves > nCount)
  {
    --m_NumberOfCurves;
    albaDEL(m_OriginalCurves[m_NumberOfCurves]);
    albaDEL(m_DeformedCurves[m_NumberOfCurves]);
    vtkDEL(m_CurvesCorrespondence[m_NumberOfCurves]);
  }

  albaVME** pNewOC = NULL;
  albaVME** pNewDC = NULL;
  vtkIdList** pNewCC = NULL;

  if (nCount > 0)
  {    
    pNewOC = new albaVME*[nCount];
    pNewDC = new albaVME*[nCount];
    pNewCC = new vtkIdList*[nCount];

    //copy existing curves
    for (int i = 0; i < m_NumberOfCurves; i++)
    {
      pNewOC[i] = m_OriginalCurves[i];
      pNewDC[i] = m_DeformedCurves[i];
      pNewCC[i] = m_CurvesCorrespondence[i];
    }

    //and reset others
    while (m_NumberOfCurves != nCount)
    {
      pNewOC[m_NumberOfCurves] = NULL;
      pNewDC[m_NumberOfCurves] = NULL;
      pNewCC[m_NumberOfCurves] = NULL;

      m_NumberOfCurves++;
    }    
  }

  delete[] m_DeformedCurves;
  delete[] m_OriginalCurves;
  delete[] m_CurvesCorrespondence;

  m_OriginalCurves = pNewOC;
  m_DeformedCurves = pNewDC;
  m_CurvesCorrespondence = pNewCC;
}

//------------------------------------------------------------------------
//Specifies the n-th control curve in its original (undeformed) state. 
/*virtual*/ void albaOpMeshDeformation::SetNthOriginalControlCurve(int num, albaVME* input)
//------------------------------------------------------------------------
{
  _VERIFY_RET(num >= 0);

  if (num >= GetNumberOfControlCurves())
    SetNumberOfControlCurves(num + 1);

  if (m_OriginalCurves[num] != input)
  {
    _VERIFY_RET(input == NULL || 
      input->GetOutput()->IsA("albaVMEOutputPolyline"));  

    if (NULL != m_OriginalCurves[num])
      m_OriginalCurves[num]->Delete();

    if (NULL != (m_OriginalCurves[num] = input))
      m_OriginalCurves[num]->Register(this);
  }  
}

//------------------------------------------------------------------------
//Specifies the n-th control curve in its deformed state. 
/*virtual*/ void albaOpMeshDeformation::SetNthDeformedControlCurve(int num, albaVME* input)
//------------------------------------------------------------------------
{
  _VERIFY_RET(num >= 0);
  if (num >= GetNumberOfControlCurves())
    SetNumberOfControlCurves(num + 1);

  if (m_DeformedCurves[num] != input)
  {
    _VERIFY_RET(input == NULL || 
      input->GetOutput()->IsA("albaVMEOutputPolyline"));  

    if (NULL != m_DeformedCurves[num])
      m_DeformedCurves[num]->Delete();

    if (NULL != (m_DeformedCurves[num] = input))
      m_DeformedCurves[num]->Register(this);
  } 
}

//Sets the correspondence between the n-th original and deformed curve.
//The list contains pairs of indices of vertices of original-deformed 
//curve that correspond to each other.
//N.B. if not specified, it is assumed that first vertices correspond. 
/*virtual*/ void albaOpMeshDeformation::SetNthControlCurveCorrespondence(int num, vtkIdList* matchlist)
//------------------------------------------------------------------------
{
  _VERIFY_RET(num >= 0);
  if (num >= GetNumberOfControlCurves())
    SetNumberOfControlCurves(num + 1);

  if (m_CurvesCorrespondence[num] != matchlist)
  {
    if (NULL != m_CurvesCorrespondence[num])
      m_CurvesCorrespondence[num]->Delete();

    if (NULL != (m_CurvesCorrespondence[num] = matchlist))
      m_CurvesCorrespondence[num]->Register(NULL);
  } 
}
#pragma endregion //Input Control Curves
