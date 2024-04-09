/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMML3
 Authors: Mel Krokos, Nigel McFarlane
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#define TestingVersion

#include "albaDefines.h"

#include <wx/slider.h>
#include <wx/defs.h>
#include "wx/busyinfo.h"

#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkTextSource.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkActor2D.h"
#include "vtkCamera.h"
#include "vtkLookupTable.h"
#include "vtkProperty2D.h"
#include "vtkTextProperty.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"
#include "vtkActorCollection.h"

#include "albaOp.h"
#include "albaOpMML3.h"
#include "albaGUIDialog.h"
#include "albaGUIButton.h"
#include "albaRWI.h"
#include "albaRWIBase.h"
#include "albaGUIValidator.h"
#include "albaGUILutSlider.h"
#include "albaVMEVolumeGray.h"
#include "albaVMESurface.h"
#include "albaVMEGroup.h"
#include "albaVMELandmark.h"
#include "albaTagArray.h"
#include "albaTagItem.h"
#include "albaGUI.h"

#include "albaOpMML3ParameterView.h"
#include "albaOpMML3ModelView.h"
#include "albaOpMML3ContourWidget.h"
#include "albaOpMML3NonUniformSlicePipe.h"
#include "albaOpMML3GlobalRegistration.h"

#include <fstream>








//----------------------------------------------------------------------------
// MML id's
//----------------------------------------------------------------------------
enum MML_IDS
{
  ID = MINID,

  // set up dlg events
  ID_INPUTS_SURFACE,
  ID_INPUTS_L1,
  ID_INPUTS_L2,
  ID_INPUTS_L3,
  ID_INPUTS_L4,
  ID_INPUTS_AXIS1,
  ID_INPUTS_AXIS2,
  ID_INPUTS_AXIS3,
  ID_INPUTS_SLICEXYZ,
  ID_INPUTS_FLAG3D,
  ID_INPUTS_NONUNIFORM,
  ID_INPUTS_NUMSLICES,
  ID_INPUTS_RESSLICES,
  ID_INPUTS_FAKE, // dummy id with no associated event
  ID_INPUTS_OK,
  ID_INPUTS_CANCEL,

  // non-uniform slice dlg events
  ID_NUSLICES_NUMBER_BEGIN,
  ID_NUSLICES_NUMBER_LAST = ID_NUSLICES_NUMBER_BEGIN + albaOpMML3::m_NumberOfNonUniformSections - 1,
  ID_NUSLICES_TOTAL,
  ID_NUSLICES_OK,
  ID_NUSLICES_CANCEL,

  // registration dlg events
  ID_REG_DISPLAY_MODE, 
  ID_REG_PREVIEW,
  ID_REG_RESET_VIEW,
  ID_REG_P_OPERATION,
  ID_REG_T_OPERATION,
  ID_REG_R_OPERATION,
  ID_REG_S_OPERATION,
  ID_REG_UNDO,
  ID_REG_SLICE,
  ID_REG_MINUS10,
  ID_REG_MINUS1,
  ID_REG_PLUS1,
  ID_REG_PLUS10,
  ID_REG_FAKE, // dummy id with no associated event
  ID_REG_OK,
  ID_REG_CANCEL
};




//----------------------------------------------------------------------------
// Constructor
albaOpMML3::albaOpMML3(const wxString &label) : albaOp(label)
//----------------------------------------------------------------------------
{
  //
  m_OpType = OPTYPE_OP;
  m_Canundo = false;

  m_VolumeVME         = NULL;
  m_SurfaceVME     = NULL;

  m_OpDlg      = NULL; // registration dialog
  
  m_DisplayModeButton = NULL;
  m_PreviewButton = NULL;
  m_ResetViewButton = NULL;
  m_UndoButton = NULL;
  m_OkButton = NULL;
  m_CancelButton = NULL;

  // p/t/r/s operation buttons
  m_PlaceOpButton = NULL;
  m_TranslateOpButton = NULL;
  m_RotateOpButton = NULL;
  m_ScaleOpButton = NULL;

  m_CurrentSlice = 0; // start up slice
  m_ShowAxes = 1; // contour line axes switched on
  m_ContourVisibility = 1; // contour switched on

  // .msf file section names
  m_AtlasMSFSectionName = "ATLAS";
  m_PatientMSFSectionName = "Patient";

  // initial inputs
  m_SurfaceName = "none";
  m_L1Name      = "none";
  m_L2Name      = "none";
  m_L3Name      = "none";
  m_L4Name      = "none";
  m_P1Name      = "none";
  m_P2Name      = "none";
  m_P3Name      = "none";
  m_P4Name      = "none";
  m_Axis1Name      = "none";
  m_Axis2Name      = "none";
  m_Axis3Name      = "none";


  // initial display 2d, no preview
  m_Flag3D = 0 ;
  m_PreviewFlag = 0 ;

  // default axis is z
  m_Slicexyz = 2 ;

  // unregistered
  m_RegistrationStatus = 0;

  // set flags to indicate that no landmarks have been defined
  m_L1Defined = false ;
  m_L2Defined = false ;
  m_L3Defined = false ;
  m_L4Defined = false ;
  m_Axis1Defined = false ;
  m_Axis2Defined = false ;
  m_Axis3Defined = false ;

  // set flag indicating how axis landmarks were created
  m_AxisLandmarksFlag = 1 ;

  m_ScansGrain = 128 ;  // resolution of slices
  m_NumberOfScans = 12 ;  // no. of slices
  m_NonUniformSliceSpacing = 0 ; // switch off non-uniform slicing
  m_AxisRangeFactor = 1.1 ; // range factor for axis
  m_MuscleType = 1 ;     // 1 for simple axis, 2 for piecewise axis

  // timestamps
  m_UserTimeStampSet = false ;
  m_UserTimeStamp = 0.0 ;
  m_InputTimeStamp = 0.0 ;
  m_OutputTimeStamp = 0.0 ;

  // Model alba RWIs
  m_ModelalbaRWI = NULL;

  // parameter alba RWIs
  m_PHalbaRWI = NULL;
  m_PValbaRWI = NULL;
  m_THalbaRWI = NULL;
  m_TValbaRWI = NULL;
  m_RAalbaRWI = NULL;
  m_SNalbaRWI = NULL;
  m_SSalbaRWI = NULL;
  m_SEalbaRWI = NULL;
  m_SWalbaRWI = NULL;

  // parameter views
  m_PH = NULL; // place horizontal
  m_PV = NULL; // place vertical
  m_TH = NULL; // translate horizontal
  m_TV = NULL; // translate vertical
  m_RA = NULL; // rotate
  m_SN = NULL; // scale north
  m_SS = NULL; // scale south
  m_SE = NULL; // scale east
  m_SW = NULL; // scale west

  m_Model = NULL;
  m_Widget = NULL;

  //
  m_VolName     = "none";
  m_InputsOk = NULL;

	vtkNEW(m_Muscle);
	vtkNEW(m_MuscleGlobalReg);
	vtkNEW(m_MuscleOutput);
}


//------------------------------------------------------------------------------
albaOpMML3::~albaOpMML3( ) 
//------------------------------------------------------------------------------
{
	vtkDEL(m_Muscle);
	vtkDEL(m_MuscleGlobalReg);
	vtkDEL(m_MuscleOutput);
}



//------------------------------------------------------------------------------
bool albaOpMML3::InternalAccept(albaVME* vme)
//------------------------------------------------------------------------------
{ 
  return vme != NULL && vme->IsALBAType(albaVMEVolumeGray);
}

//----------------------------------------------------------------------------
albaOp* albaOpMML3::Copy() 
//------------------------------------------------------------------------------
{
  return new albaOpMML3(m_Label);
}

//----------------------------------------------------------------------------
void albaOpMML3::OpDo()  {}

//----------------------------------------------------------------------------
void albaOpMML3::OpUndo() {}

//----------------------------------------------------------------------------
void albaOpMML3::OpStop(int result) {	albaEventMacro(albaEvent(this,result));}



//----------------------------------------------------------------------------
void albaOpMML3::OpRun()   
//----------------------------------------------------------------------------
{
  //----------------------------------------------------------------------------
  // create inputs dialog
  //----------------------------------------------------------------------------
  CreateInputsDlg() ;  

  int returnCode = m_InputsDlg->GetReturnCode() ;
  DeleteInputsDlg() ;

  if (returnCode != wxID_OK){
    // delete dialog and cancel operation
    OpStop(OP_RUN_CANCEL);
    return ;
  }

  if (!m_SurfaceVME){
    // vme not selected so warn and cancel
    wxMessageBox("Not all parameters have been set up","alert",wxICON_WARNING);
    OpStop(OP_RUN_CANCEL);
    return ;
  }


  //----------------------------------------------------------------------------
  // get the vtk inputs
  //----------------------------------------------------------------------------
  GetVtkInputs() ;



  //----------------------------------------------------------------------------
  // Perform the landmark-based global registration.
  // This creates the global-registered versions of the muscle and landmarks
  //----------------------------------------------------------------------------
  ApplyGlobalRegistration() ;



  //----------------------------------------------------------------------------
  // create the non-uniform slices dialog
  //----------------------------------------------------------------------------
  if (m_NonUniformSliceSpacing == 1){
    CreateNonUniformSlicesDlg() ;

    //int returnCode = m_NonUniformSlicesDlg->GetReturnCode() ;
    DeleteNonUniformSlicesDlg() ;

    if (returnCode != wxID_OK){
      // not ok, but no need to stop whole operation
      // just set back to uniform slicing
      m_NonUniformSliceSpacing = false ;
    }
  }


  //----------------------------------------------------------------------------
  // create registration dialog
  //----------------------------------------------------------------------------
  CreateRegistrationDlg(); 

  int res = (m_OpDlg->GetReturnCode() == wxID_OK) ? OP_RUN_OK : OP_RUN_CANCEL;

  // delete dialog and stop
  DeleteRegistrationDlg() ;
  OpStop(res);
}



//----------------------------------------------------------------------------
// Create the input and set-up dialog
void albaOpMML3::CreateInputsDlg() 
//----------------------------------------------------------------------------
{
  // create the dialog
  m_InputsDlg = new albaGUIDialog("SetUp", albaCLOSEWINDOW | albaRESIZABLE); 

  // vertical stacker for the rows of widgets
  wxBoxSizer *vs1 = new wxBoxSizer(wxVERTICAL);

  // select muscle surface vme
  wxStaticText *selectMuscleLabel  = new wxStaticText(m_InputsDlg, wxID_ANY, "Surface", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl   *selectMuscleTxt = new wxTextCtrl(m_InputsDlg , ID_INPUTS_SURFACE, "", wxPoint(0,0), wxSize(150,20), wxNO_BORDER |wxTE_READONLY );
  selectMuscleTxt->SetValidator(albaGUIValidator(this, ID_INPUTS_SURFACE, selectMuscleTxt, &m_SurfaceName));
  albaGUIButton *selectMuscleButton = new albaGUIButton(m_InputsDlg , ID_INPUTS_SURFACE, "select", wxPoint(0,0), wxSize(50,20));
  selectMuscleButton->SetListener(this);

  wxBoxSizer *selectMuscleHBox = new wxBoxSizer(wxHORIZONTAL);
  selectMuscleHBox->Add(selectMuscleLabel, 0);
  selectMuscleHBox->Add(selectMuscleTxt, 1, wxEXPAND);
  selectMuscleHBox->Add(selectMuscleButton,0);
  vs1->Add(selectMuscleHBox, 0, wxEXPAND | wxALL, 2);

  // time stamp
  wxStaticText *TimeStampLabel  = new wxStaticText(m_InputsDlg, wxID_ANY, "Output time stamp", wxPoint(0,0), wxSize(150,20));
  m_TimeStampTxt = new wxTextCtrl(m_InputsDlg, wxID_ANY, "0.0", wxPoint(0,0), wxSize(150,20), wxNO_BORDER );
  m_TimeStampTxt->SetValidator(albaGUIValidator(this, ID_INPUTS_FAKE, m_TimeStampTxt, &m_OutputTimeStamp));

  wxBoxSizer *TimeStampHBox = new wxBoxSizer(wxHORIZONTAL);
  TimeStampHBox->Add(TimeStampLabel, 0);
  TimeStampHBox->Add(m_TimeStampTxt, 1, wxEXPAND);
  vs1->Add(TimeStampHBox, 0, wxEXPAND | wxALL, 2);
  vs1->AddSpacer(20) ;



  // Registration landmarks
  wxStaticText *lab_RegLandmarksTitle = new wxStaticText(m_InputsDlg, -1, "Registration Landmarks (select from atlas)", wxPoint(0,0), wxSize(150,20));
  vs1->Add(lab_RegLandmarksTitle, 0, wxEXPAND | wxALL, 2);

  // landmark 1
  wxStaticText *lab_5  = new wxStaticText(m_InputsDlg, -1, "Landmark 1", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl   *text_5 = new wxTextCtrl(m_InputsDlg ,  -1, "", wxPoint(0,0), wxSize(150,20),wxNO_BORDER |wxTE_READONLY );
  text_5->SetValidator(albaGUIValidator(this,ID_INPUTS_L1,text_5,&m_L1Name));
  albaGUIButton *b_5       = new albaGUIButton(m_InputsDlg ,ID_INPUTS_L1,"select",wxPoint(0,0), wxSize(50,20));
  b_5->SetListener(this);
  wxBoxSizer *hs_5 = new wxBoxSizer(wxHORIZONTAL);
  hs_5->Add(lab_5,0);
  hs_5->Add(text_5,1,wxEXPAND);
  hs_5->Add(b_5,0);
  vs1->Add(hs_5,0,wxEXPAND | wxALL, 2);

  // landmark 2
  wxStaticText *lab_6  = new wxStaticText(m_InputsDlg, -1, "Landmark 2", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl   *text_6 = new wxTextCtrl(m_InputsDlg ,  -1, "", wxPoint(0,0), wxSize(150,20),wxNO_BORDER |wxTE_READONLY );
  text_6->SetValidator(albaGUIValidator(this,ID_INPUTS_L2,text_6,&m_L2Name));
  albaGUIButton *b_6       = new albaGUIButton(m_InputsDlg ,ID_INPUTS_L2,"select",wxPoint(0,0), wxSize(50,20));
  b_6->SetListener(this);
  wxBoxSizer *hs_6 = new wxBoxSizer(wxHORIZONTAL);
  hs_6->Add(lab_6,0);
  hs_6->Add(text_6,1,wxEXPAND);
  hs_6->Add(b_6,0);
  vs1->Add(hs_6,0,wxEXPAND | wxALL, 2);

  // landmark 3
  wxStaticText *lab_7  = new wxStaticText(m_InputsDlg, -1, "Landmark 3", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl   *text_7 = new wxTextCtrl(m_InputsDlg ,  -1, "", wxPoint(0,0), wxSize(150,20),wxNO_BORDER |wxTE_READONLY );
  text_7->SetValidator(albaGUIValidator(this,ID_INPUTS_L3,text_7,&m_L3Name));
  albaGUIButton *b_7       = new albaGUIButton(m_InputsDlg ,ID_INPUTS_L3,"select",wxPoint(0,0), wxSize(50,20));
  b_7->SetListener(this);
  wxBoxSizer *hs_7 = new wxBoxSizer(wxHORIZONTAL);
  hs_7->Add(lab_7,0);
  hs_7->Add(text_7,1,wxEXPAND);
  hs_7->Add(b_7,0);
  vs1->Add(hs_7,0,wxEXPAND | wxALL, 2);

  // landmark 4
  wxStaticText *lab_8  = new wxStaticText(m_InputsDlg, -1, "Landmark 4 (optional)", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl   *text_8 = new wxTextCtrl(m_InputsDlg ,  -1, "", wxPoint(0,0), wxSize(150,20),wxNO_BORDER |wxTE_READONLY );
  text_8->SetValidator(albaGUIValidator(this,ID_INPUTS_L4,text_8,&m_L4Name));
  albaGUIButton *b_8       = new albaGUIButton(m_InputsDlg ,ID_INPUTS_L4,"select",wxPoint(0,0), wxSize(50,20));
  b_8->SetListener(this);
  wxBoxSizer *hs_8 = new wxBoxSizer(wxHORIZONTAL);
  hs_8->Add(lab_8,0);
  hs_8->Add(text_8,1,wxEXPAND);
  hs_8->Add(b_8,0);
  vs1->Add(hs_8,0,wxEXPAND | wxALL, 2);
  vs1->AddSpacer(20) ;



  // Axis slice direction
  wxStaticText *lab_AxisLandmarksTitle = new wxStaticText(m_InputsDlg, -1, "Select Slice Direction", wxPoint(0,0), wxSize(150,20));
  vs1->Add(lab_AxisLandmarksTitle, 0, wxEXPAND | wxALL, 2);

  // Default slice direction x, y or z
  wxArrayString radioStrings ;
  radioStrings.Add("x") ;
  radioStrings.Add("y") ;
  radioStrings.Add("z") ;
  m_Radio_slicexyz = new wxRadioBox(m_InputsDlg, ID_INPUTS_SLICEXYZ, "Default direction", wxPoint(0,0), wxSize(150,50), radioStrings, 1, wxRA_SPECIFY_ROWS) ;
  m_Radio_slicexyz->SetValidator(albaGUIValidator(this, ID_INPUTS_SLICEXYZ, m_Radio_slicexyz, &m_Slicexyz));
  wxBoxSizer *sliceXYZHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
  sliceXYZHorizontalSizer->Add(m_Radio_slicexyz,1,wxEXPAND | wxRIGHT, 3);
  vs1->Add(sliceXYZHorizontalSizer,0,wxEXPAND | wxALL, 2);

  // landmark axis 1
  wxStaticText *lab_axis1 = new wxStaticText(m_InputsDlg, -1, "Landmark Axis 1", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl *text_axis1 = new wxTextCtrl(m_InputsDlg ,  -1, "", wxPoint(0,0), wxSize(150,20),wxNO_BORDER |wxTE_READONLY );
  text_axis1->SetValidator(albaGUIValidator(this, ID_INPUTS_AXIS1, text_axis1, &m_Axis1Name));
  albaGUIButton *button_axis1 = new albaGUIButton(m_InputsDlg, ID_INPUTS_AXIS1, "select", wxPoint(0,0), wxSize(50,20));
  button_axis1->SetListener(this);
  wxBoxSizer *hs_axis1 = new wxBoxSizer(wxHORIZONTAL);
  hs_axis1->Add(lab_axis1,0);
  hs_axis1->Add(text_axis1,1,wxEXPAND);
  hs_axis1->Add(button_axis1,0);
  vs1->Add(hs_axis1,0,wxEXPAND | wxALL, 2);

  // landmark axis 2
  wxStaticText *lab_axis2 = new wxStaticText(m_InputsDlg, -1, "Landmark Axis 2", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl *text_axis2 = new wxTextCtrl(m_InputsDlg ,  -1, "", wxPoint(0,0), wxSize(150,20),wxNO_BORDER |wxTE_READONLY );
  text_axis2->SetValidator(albaGUIValidator(this, ID_INPUTS_AXIS2, text_axis2, &m_Axis2Name));
  albaGUIButton *button_axis2 = new albaGUIButton(m_InputsDlg, ID_INPUTS_AXIS2, "select", wxPoint(0,0), wxSize(50,20));
  button_axis2->SetListener(this);
  wxBoxSizer *hs_axis2 = new wxBoxSizer(wxHORIZONTAL);
  hs_axis2->Add(lab_axis2,0);
  hs_axis2->Add(text_axis2,1,wxEXPAND);
  hs_axis2->Add(button_axis2,0);
  vs1->Add(hs_axis2,0,wxEXPAND | wxALL, 2);

  // landmark axis 3
  wxStaticText *lab_axis3 = new wxStaticText(m_InputsDlg, -1, "Landmark Axis 3", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl *text_axis3 = new wxTextCtrl(m_InputsDlg ,  -1, "", wxPoint(0,0), wxSize(150,20),wxNO_BORDER |wxTE_READONLY );
  text_axis3->SetValidator(albaGUIValidator(this, ID_INPUTS_AXIS3, text_axis3, &m_Axis3Name));
  albaGUIButton *button_axis3 = new albaGUIButton(m_InputsDlg, ID_INPUTS_AXIS3, "select", wxPoint(0,0), wxSize(50,20));
  button_axis3->SetListener(this);
  wxBoxSizer *hs_axis3 = new wxBoxSizer(wxHORIZONTAL);
  hs_axis3->Add(lab_axis3,0);
  hs_axis3->Add(text_axis3,1,wxEXPAND);
  hs_axis3->Add(button_axis3,0);
  vs1->Add(hs_axis3,0,wxEXPAND | wxALL, 2);
  text_axis3->Enable(m_MuscleType == 2) ;   // enable only if muscle has a piecewise axis with 3 landmarks
  button_axis3->Enable(m_MuscleType == 2) ;
  vs1->AddSpacer(20) ;


  // number of slices
  wxStaticText *ScansNumberLab = new wxStaticText(m_InputsDlg, wxID_ANY, "Number of slices (3 or more)", wxPoint(0,0), wxSize(150,20));
  m_NumberOfScansTxt = new wxTextCtrl(m_InputsDlg, wxID_ANY, "", wxPoint(0,0), wxSize(150,20), wxNO_BORDER );
  m_NumberOfScansTxt->SetValidator(albaGUIValidator(this, ID_INPUTS_NUMSLICES, m_NumberOfScansTxt, &m_NumberOfScans, 3, 10000)); // min/max values
  wxBoxSizer *ScansNumberHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
  ScansNumberHorizontalSizer->Add(ScansNumberLab,0);
  ScansNumberHorizontalSizer->Add(m_NumberOfScansTxt,1,wxEXPAND);
  ScansNumberHorizontalSizer->AddSpacer(20) ;

  // memory warning
  m_WarningNumberOfSlices = new wxStaticText(m_InputsDlg, wxID_ANY, "Warning: May exceed memory", wxPoint(0,0), wxSize(150,20));
  wxColour redColour(1,0,0) ;
  m_WarningNumberOfSlices->Enable(false) ;
  ScansNumberHorizontalSizer->Add(m_WarningNumberOfSlices,0);
  ScansNumberHorizontalSizer->AddSpacer(20) ;

  // non-uniform slice spacing check box
  wxStaticText *nonUniformLab = new wxStaticText(m_InputsDlg, wxID_ANY, "Non-uniform slicing", wxPoint(0,0), wxSize(150,20));
  wxCheckBox *nonUniformCheckBox = new wxCheckBox(m_InputsDlg, ID_INPUTS_NONUNIFORM, "", wxPoint(0,0), wxSize(80,20)) ;
  nonUniformCheckBox->SetValidator(albaGUIValidator(this, ID_INPUTS_NONUNIFORM, nonUniformCheckBox, &m_NonUniformSliceSpacing));
  ScansNumberHorizontalSizer->Add(nonUniformLab, 0);
  ScansNumberHorizontalSizer->Add(nonUniformCheckBox,1,wxEXPAND | wxRIGHT, 3);
  vs1->Add(ScansNumberHorizontalSizer,0,wxEXPAND | wxALL, 2);


  // scans resolution
  wxStaticText *ScansGrainLab  = new wxStaticText(m_InputsDlg, wxID_ANY, "Slice resolution (64-1024)", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl *ScansGrainTxt1 = new wxTextCtrl(m_InputsDlg , wxID_ANY, "", wxPoint(0,0), wxSize(75,20), wxNO_BORDER );
  ScansGrainTxt1->SetValidator(albaGUIValidator(this, ID_INPUTS_RESSLICES, ScansGrainTxt1, &m_ScansGrain, 64, 1024));
  wxBoxSizer *ScansGrainHorizontalSizer2 = new wxBoxSizer(wxHORIZONTAL);
  ScansGrainHorizontalSizer2->Add(ScansGrainLab, 0);
  ScansGrainHorizontalSizer2->Add(ScansGrainTxt1,1,wxEXPAND | wxRIGHT, 3);
  vs1->Add(ScansGrainHorizontalSizer2,0,wxEXPAND | wxALL, 2);

  // axis scaling factor
  wxStaticText *AxisScalingLab = new wxStaticText(m_InputsDlg, wxID_ANY, "Axis range factor (1-2)", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl *AxisScalingTxt = new wxTextCtrl(m_InputsDlg, wxID_ANY, "", wxPoint(0,0), wxSize(150,20), wxNO_BORDER );
  AxisScalingTxt->SetValidator(albaGUIValidator(this, ID_INPUTS_FAKE, AxisScalingTxt, &m_AxisRangeFactor, 1.0, 2.0)); // min/max values
  wxBoxSizer *AxisScalingHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
  AxisScalingHorizontalSizer->Add(AxisScalingLab,0);
  AxisScalingHorizontalSizer->Add(AxisScalingTxt,1,wxEXPAND);
  vs1->Add(AxisScalingHorizontalSizer,0,wxEXPAND | wxALL, 2);


  // ok/cancel buttons
  m_InputsOk = new albaGUIButton(m_InputsDlg, ID_INPUTS_OK, "OK", wxPoint(0,0), wxSize(50,20));
  m_InputsOk->SetListener(this);
  m_InputsOk->Enable(false);

  albaGUIButton *b_cancel = new albaGUIButton(m_InputsDlg, ID_INPUTS_CANCEL, "CANCEL", wxPoint(0,0), wxSize(50,20));
  b_cancel->SetListener(this);

  wxBoxSizer *okCancelBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  okCancelBoxSizer->Add(m_InputsOk,0);
  okCancelBoxSizer->Add(b_cancel,0);
  vs1->Add(okCancelBoxSizer,0,wxALIGN_CENTER | wxALL, 2);



  // put the vertical sizer into the dialog and display
  vs1->Fit(m_InputsDlg);	  // fit the window to the min size of the sizer
  m_InputsDlg->Add(vs1) ;  // plug the sizer into the dialog
  m_InputsDlg->ShowModal();

}




//----------------------------------------------------------------------------
// Delete the input and set-up dialog
void albaOpMML3::DeleteInputsDlg() 
//----------------------------------------------------------------------------
{
  delete m_InputsDlg ;
}




//------------------------------------------------------------------------------
// Create the registration dialog
void albaOpMML3::CreateRegistrationDlg()
//------------------------------------------------------------------------------
{ 
  // create dialog
  wxString Title;
  Title = "registration of " + m_SurfaceName;
  m_OpDlg = new albaGUIDialog(Title, albaCLOSEWINDOW | albaRESIZABLE); 

  // display mode button
  m_DisplayModeButton = new albaGUIButton(m_OpDlg, ID_REG_DISPLAY_MODE, "Display 3D", wxPoint(0,0), wxSize(75,20));
  m_DisplayModeButton->SetListener(this);
  m_DisplayModeButton->Enable(true);
  if (m_Flag3D == 0) 
    m_DisplayModeButton->SetLabel("Display 3D");
  else 
    m_DisplayModeButton->SetLabel("Display 2D");


  // preview button
  m_PreviewButton = new albaGUIButton(m_OpDlg, ID_REG_PREVIEW, "Preview On", wxPoint(0,0), wxSize(75,20));
  m_PreviewButton->SetListener(this);
  m_PreviewButton->Enable(true) ;
  if (m_PreviewFlag == 0) 
    m_PreviewButton->SetLabel("Preview On");
  else
    m_PreviewButton->SetLabel("Preview Off");


  // reset view button
  m_ResetViewButton = new albaGUIButton(m_OpDlg, ID_REG_RESET_VIEW, "Reset View", wxPoint(0,0), wxSize(75,20));
  m_ResetViewButton->SetListener(this);
  m_ResetViewButton->Enable(true) ;


  // undo button
  m_UndoButton = new albaGUIButton(m_OpDlg, ID_REG_UNDO, "Undo", wxPoint(0,0), wxSize(75,20));
  m_UndoButton->SetListener(this);
  m_UndoButton->Enable((m_Flag3D == 0) && (m_PreviewFlag == 0)) ;


  // ok button
  m_OkButton = new albaGUIButton(m_OpDlg, ID_REG_OK, "OK", wxPoint(0,0), wxSize(75,20));
  m_OkButton->SetListener(this);

  // cancel button
  m_CancelButton = new albaGUIButton(m_OpDlg, ID_REG_CANCEL, "CANCEL", wxPoint(0,0), wxSize(75,20));
  m_CancelButton->SetListener(this);

  // button background colour
  m_ButtonBackgroundColour = m_CancelButton->GetBackgroundColour();

  // alba check boxes
  //wxBoxSizer *CheckBoxHorizontalBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  //wxCheckBox *c0 = new wxCheckBox(m_Op_dlg,ID_REG_DISPLAY_MODE,"show axes");
  //c0->SetValidator( albaGUIValidator(this,ID_REG_DISPLAY_MODE,c0,&m_Show_axes) );
  //CheckBoxHorizontalBoxSizer->Add(c0,0,wxALL, 5);
  //wxCheckBox *c1 = new wxCheckBox(m_Op_dlg,ID_REG_PREVIEW,"show contour");
  //c1->SetValidator( albaGUIValidator(this,ID_REG_PREVIEW,c1,&m_ContourVisibility));
  //CheckBoxHorizontalBoxSizer->Add(c1,0,wxALL, 5);
  //LeftVerticalBoxSizer->Add(CheckBoxHorizontalBoxSizer);


  // create p/t/r/s operation buttons
  m_PlaceOpButton = new albaGUIButton(m_OpDlg, ID_REG_P_OPERATION, "P", wxPoint(0,0), wxSize(75,20));
  m_PlaceOpButton->SetListener(this);
  m_PlaceOpButton->Enable((m_Flag3D == 0) && (m_PreviewFlag == 0)) ;

  m_TranslateOpButton = new albaGUIButton(m_OpDlg, ID_REG_T_OPERATION,"T", wxPoint(0,0), wxSize(75,20));
  m_TranslateOpButton->SetListener(this);
  m_TranslateOpButton->Enable((m_Flag3D == 0) && (m_PreviewFlag == 0)) ;

  m_RotateOpButton = new albaGUIButton(m_OpDlg, ID_REG_R_OPERATION, "R", wxPoint(0,0), wxSize(75,20));
  m_RotateOpButton->SetListener(this);
  m_RotateOpButton->Enable((m_Flag3D == 0) && (m_PreviewFlag == 0)) ;

  m_ScaleOpButton = new albaGUIButton(m_OpDlg, ID_REG_S_OPERATION, "S", wxPoint(0,0), wxSize(75,20));
  m_ScaleOpButton->SetListener(this);
  m_ScaleOpButton->Enable((m_Flag3D == 0) && (m_PreviewFlag == 0)) ;


  // alba model view RWI
  m_ModelalbaRWI = new albaRWI(m_OpDlg);
  m_ModelalbaRWI->SetListener(this);
  m_ModelalbaRWI->m_RwiBase->SetInteractorStyle(NULL);
  m_ModelalbaRWI->SetSize(0,0,420,420);
  m_ModelalbaRWI->Show(true);
  m_ModelalbaRWI->CameraUpdate();


  // alba lut
  m_Lut = new albaGUILutSlider(m_OpDlg,-1,wxPoint(0,0),wxSize(420,24));
  m_Lut->SetListener(this);

  // create mml model view
  m_Model = new albaOpMML3ModelView(m_ModelalbaRWI->m_RenderWindow, m_ModelalbaRWI->m_RenFront, 
    m_MuscleGlobalReg, m_MuscleOutput, m_Volume, m_NumberOfScans);

  // slice position slider and buttons
  wxStaticText *lab_slicepos  = new wxStaticText(m_OpDlg, -1, "Slice", wxPoint(0,0), wxSize(40, 20));
  m_Text_SlicePos = new wxTextCtrl(m_OpDlg, ID_REG_SLICE, "", wxPoint(0,0), wxSize(40, 20), wxNO_BORDER | wxTE_READONLY );
  albaGUIButton *button_Minus10 = new albaGUIButton(m_OpDlg, ID_REG_MINUS10, "<<", wxPoint(0,0), wxSize(30,20)) ;
  button_Minus10->SetListener(this) ;
  albaGUIButton *button_Minus1 = new albaGUIButton(m_OpDlg, ID_REG_MINUS1, "<", wxPoint(0,0), wxSize(30,20)) ; 
  button_Minus1->SetListener(this) ;
  albaGUIButton *button_Plus1 = new albaGUIButton(m_OpDlg, ID_REG_PLUS1, ">", wxPoint(0,0), wxSize(30,20)) ; 
  button_Plus1->SetListener(this) ;
  albaGUIButton *button_Plus10 = new albaGUIButton(m_OpDlg, ID_REG_PLUS10, ">>", wxPoint(0,0), wxSize(30,20)) ; 
  button_Plus10->SetListener(this) ;
  wxSlider *slider_slicepos  = new wxSlider(m_OpDlg, ID_REG_SLICE, 0, 0, m_NumberOfScans-1, wxPoint(0,0), wxSize(269,-1));
  slider_slicepos->SetValidator(albaGUIValidator(this, ID_REG_SLICE, (wxSlider*)slider_slicepos, &m_CurrentSlice, m_Text_SlicePos));


  // alba parameter views RWI's
  for (int i = 0 ;  i < 9 ;  i++)
    m_TextSource[i] = vtkTextSource::New() ;
  m_PHalbaRWI = CreateParameterViewalbaRWI(m_TextSource[0], "P-H", 1,1,1); // white
  m_PValbaRWI = CreateParameterViewalbaRWI(m_TextSource[1], "P-V", 1,1,1); // white
  m_THalbaRWI = CreateParameterViewalbaRWI(m_TextSource[2], "T-H", 1,1,1); // white
  m_TValbaRWI = CreateParameterViewalbaRWI(m_TextSource[3], "T-V", 1,1,1); // white
  m_RAalbaRWI = CreateParameterViewalbaRWI(m_TextSource[4], "R-A", 1,1,1); // white
  m_SNalbaRWI = CreateParameterViewalbaRWI(m_TextSource[5], "S-N", 1,0,0); // red
  m_SSalbaRWI = CreateParameterViewalbaRWI(m_TextSource[6], "S-S", 0,1,0); // green
  m_SEalbaRWI = CreateParameterViewalbaRWI(m_TextSource[7], "S-E", 0,0,1); // blue
  m_SWalbaRWI = CreateParameterViewalbaRWI(m_TextSource[8], "S-W", 1,0,1); // magenta



  //----------------------------------------------------------------------------
  // layout of dialog
  //----------------------------------------------------------------------------

  // top row of buttons
  wxBoxSizer *TopHorizontalBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  TopHorizontalBoxSizer->Add(m_DisplayModeButton,0,wxALL, 5);
  TopHorizontalBoxSizer->Add(m_PreviewButton,0,wxALL, 5);
  TopHorizontalBoxSizer->Add(m_ResetViewButton,0,wxALL, 5);
  TopHorizontalBoxSizer->Add(m_OkButton,0,wxALL, 5);
  TopHorizontalBoxSizer->Add(m_CancelButton,0, wxALL, 5);

  // second row of operation buttons
  wxBoxSizer *OperationHorizontalBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  OperationHorizontalBoxSizer->Add(m_PlaceOpButton,0,wxALL, 5);
  OperationHorizontalBoxSizer->Add(m_TranslateOpButton,0,wxALL, 5);
  OperationHorizontalBoxSizer->Add(m_RotateOpButton,0,wxALL, 5);
  OperationHorizontalBoxSizer->Add(m_ScaleOpButton,0,wxALL, 5);
  OperationHorizontalBoxSizer->Add(m_UndoButton,0,wxALL, 5);

  // slider label and value
  wxBoxSizer *SliderValueHorizontalBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  SliderValueHorizontalBoxSizer->Add(button_Minus10,  0, wxLEFT);
  SliderValueHorizontalBoxSizer->Add(button_Minus1,  0, wxLEFT);
  SliderValueHorizontalBoxSizer->Add(lab_slicepos,  0, wxLEFT, 5);
  SliderValueHorizontalBoxSizer->Add(m_Text_SlicePos, 0, wxLEFT, 5);
  SliderValueHorizontalBoxSizer->Add(button_Plus1,  0, wxLEFT);
  SliderValueHorizontalBoxSizer->Add(button_Plus10,  0, wxLEFT);


  // left vertical sizer (rows of buttons + render window)
  wxBoxSizer *LeftVerticalBoxSizer = new wxBoxSizer(wxVERTICAL);
  LeftVerticalBoxSizer->Add(TopHorizontalBoxSizer);
  LeftVerticalBoxSizer->Add(OperationHorizontalBoxSizer);
  LeftVerticalBoxSizer->Add(m_ModelalbaRWI->m_RwiBase, 1, wxEXPAND | wxALL, 5);
  LeftVerticalBoxSizer->Add(m_Lut, 0, wxEXPAND, 6);


  // right vertical sizer (slider + param view widgets)
  wxBoxSizer *RightVerticalBoxSizer = new wxBoxSizer(wxVERTICAL) ;
  RightVerticalBoxSizer->Add(SliderValueHorizontalBoxSizer, 0, wxEXPAND | wxALIGN_LEFT | wxTOP | wxLEFT, 3);  // slice pos buttons, label and value
  RightVerticalBoxSizer->Add(slider_slicepos, 0, wxEXPAND | wxALIGN_LEFT | wxTOP, 3); // slice position slider
  RightVerticalBoxSizer->AddSpacer(5) ;
  RightVerticalBoxSizer->Add(m_PHalbaRWI->m_RwiBase, 0, wxLEFT, 10);
  RightVerticalBoxSizer->AddSpacer(3) ;
  RightVerticalBoxSizer->Add(m_PValbaRWI->m_RwiBase, 0, wxLEFT, 10);
  RightVerticalBoxSizer->AddSpacer(3) ;
  RightVerticalBoxSizer->Add(m_THalbaRWI->m_RwiBase, 0, wxLEFT, 10);
  RightVerticalBoxSizer->AddSpacer(3) ;
  RightVerticalBoxSizer->Add(m_TValbaRWI->m_RwiBase, 0, wxLEFT, 10);
  RightVerticalBoxSizer->AddSpacer(3) ;
  RightVerticalBoxSizer->Add(m_RAalbaRWI->m_RwiBase, 0, wxLEFT, 10);
  RightVerticalBoxSizer->AddSpacer(3) ;
  RightVerticalBoxSizer->Add(m_SNalbaRWI->m_RwiBase, 0, wxLEFT, 10);
  RightVerticalBoxSizer->AddSpacer(3) ;
  RightVerticalBoxSizer->Add(m_SSalbaRWI->m_RwiBase, 0, wxLEFT, 10);
  RightVerticalBoxSizer->AddSpacer(3) ;
  RightVerticalBoxSizer->Add(m_SEalbaRWI->m_RwiBase, 0, wxLEFT, 10);
  RightVerticalBoxSizer->AddSpacer(3) ;
  RightVerticalBoxSizer->Add(m_SWalbaRWI->m_RwiBase, 0, wxLEFT, 10);
  RightVerticalBoxSizer->AddSpacer(3) ;


  // registration window horizontal sizer
  wxBoxSizer *WindowHorizontalBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  WindowHorizontalBoxSizer->Add(LeftVerticalBoxSizer,1,wxEXPAND);
  WindowHorizontalBoxSizer->Add(RightVerticalBoxSizer,0,wxALL,0);


  // add to dialog
  m_OpDlg->Add(WindowHorizontalBoxSizer);    
  WindowHorizontalBoxSizer->Fit(m_OpDlg);



  //----------------------------------------------------------------------------
  // MML widget classes
  //----------------------------------------------------------------------------
  // create mml parameter views
  m_PH = new albaOpMML3ParameterView(m_PHalbaRWI->m_RenderWindow, m_PHalbaRWI->m_RenFront); 
  m_PV = new albaOpMML3ParameterView(m_PValbaRWI->m_RenderWindow, m_PValbaRWI->m_RenFront);
  m_TH = new albaOpMML3ParameterView(m_THalbaRWI->m_RenderWindow, m_THalbaRWI->m_RenFront);
  m_TV = new albaOpMML3ParameterView(m_TValbaRWI->m_RenderWindow, m_TValbaRWI->m_RenFront);
  m_RA = new albaOpMML3ParameterView(m_RAalbaRWI->m_RenderWindow, m_RAalbaRWI->m_RenFront);
  m_SN = new albaOpMML3ParameterView(m_SNalbaRWI->m_RenderWindow, m_SNalbaRWI->m_RenFront);
  m_SS = new albaOpMML3ParameterView(m_SSalbaRWI->m_RenderWindow, m_SSalbaRWI->m_RenFront);
  m_SE = new albaOpMML3ParameterView(m_SEalbaRWI->m_RenderWindow, m_SEalbaRWI->m_RenFront);
  m_SW = new albaOpMML3ParameterView(m_SWalbaRWI->m_RenderWindow, m_SWalbaRWI->m_RenFront);

  // create mml contour widget
  m_Widget = albaOpMML3ContourWidget::New();

  // set up the model view, the widget and the parameter views
  SetUpModelView();
  SetUpParameterViews();
  m_Model->Update() ; // update because SetUpParameterViews intializes the transform splines
  SetUpContourWidget();
 
  albaEventMacro(albaEvent(this, ID_REG_T_OPERATION));
  
  m_OpDlg->ShowModal();	
}




//------------------------------------------------------------------------------
// Delete the registration dialog
void albaOpMML3::DeleteRegistrationDlg()
//------------------------------------------------------------------------------
{ 
  // delete the contour widget
  m_Widget->Delete() ;

  // delete the parameter views
  if (m_PH != NULL)
    delete m_PH ;
  if (m_PV != NULL)
    delete m_PV ;
  if (m_TH != NULL)
    delete m_TH ;
  if (m_TV != NULL)
    delete m_TV ;
  if (m_RA != NULL)
    delete m_RA ;
  if (m_SN != NULL)
    delete m_SN ;
  if (m_SS != NULL)
    delete m_SS ;
  if (m_SE != NULL)
    delete m_SE ;
  if (m_SW != NULL)
    delete m_SW ;

  // delete the parameter view render windows
  for (int i = 0 ;  i < 9 ;  i++)
    m_TextSource[i]->Delete() ;
  delete m_PHalbaRWI ;
  delete m_PValbaRWI ;
  delete m_THalbaRWI ;
  delete m_TValbaRWI ;
  delete m_RAalbaRWI ;
  delete m_SNalbaRWI ;
  delete m_SSalbaRWI ;
  delete m_SEalbaRWI ;
  delete m_SWalbaRWI ;

  // delete the model view
  if (m_Model != NULL)
    delete m_Model ;

  // delete the model view render window
  delete m_ModelalbaRWI ;

  // delete the dialog
  delete m_OpDlg ;
}





//------------------------------------------------------------------------------
// Dialog to input non-uniform slice spacing
void albaOpMML3::CreateNonUniformSlicesDlg()
//------------------------------------------------------------------------------
{
  int i, total ;

  // create dialog
  wxString Title;
  Title = "Set distribution of non-uniform slice spacing" ;
  m_NonUniformSlicesDlg = new albaGUIDialog(Title, albaCLOSEWINDOW | albaRESIZABLE); 


  //----------------------------------------------------------------------------
  // initialize the numbers in each section so that they add up to m_NumberOfScans
  // and the distribution is as uniform as possible
  //----------------------------------------------------------------------------
  for (i = 0 ;  i < m_NumberOfNonUniformSections ;  i++)
    m_SlicesInSection[i] = 0 ;
  for (total = 0 ;  total < m_NumberOfScans ;  total++){
    i = total % m_NumberOfNonUniformSections ; // section index
    m_SlicesInSection[i]++ ;
  }
  for (i = 0, total = 0 ;  i < m_NumberOfNonUniformSections ;  i++)
    total += m_SlicesInSection[i] ;
  assert(total == m_NumberOfScans) ;



  //----------------------------------------------------------------------------
  // vertical column containing RWI
  //----------------------------------------------------------------------------

  // window size parameters
  int winSize = 400 ;
  int sectionHeight = winSize / m_NumberOfNonUniformSections ;
  int textHeight = sectionHeight ;
  int textCtrlHeight = sectionHeight/2 ;
  int spacerHeight = (sectionHeight - textCtrlHeight) / 2 ;

  // column title
  wxStaticText *renWinTitleTxt = new wxStaticText(m_NonUniformSlicesDlg, wxID_ANY, "Section view", wxPoint(0,0), wxSize(25,textHeight)) ;

  // create render window
  m_SectionsViewRWI = new albaRWI(m_NonUniformSlicesDlg);
  m_SectionsViewRWI->SetListener(this);
  m_SectionsViewRWI->m_RwiBase->SetInteractorStyle(NULL);
  m_SectionsViewRWI->SetSize(0,0,winSize,winSize);
  m_SectionsViewRWI->Show(true);

  // vertical sizer for rwi 
  wxBoxSizer *RenWinBoxSizer = new wxBoxSizer(wxVERTICAL);
  RenWinBoxSizer->Add(renWinTitleTxt, 0.5, wxEXPAND | wxALL, 1);
  RenWinBoxSizer->Add(m_SectionsViewRWI->m_RwiBase, 0.5, wxEXPAND | wxALL, 1);
  RenWinBoxSizer->AddSpacer(sectionHeight) ;  // space of one section at bottom to allow for total box



  //----------------------------------------------------------------------------
  // column for number entry widgets and total
  //----------------------------------------------------------------------------

  wxBoxSizer *widgetsBoxSizer = new wxBoxSizer(wxVERTICAL);
  m_NumSlicesRowBoxSizer = new (wxBoxSizer* [m_NumberOfNonUniformSections]) ;
  m_NumSlicesLabel = new (wxStaticText* [m_NumberOfNonUniformSections]) ;
  m_NumSlicesEntry = new (wxTextCtrl* [m_NumberOfNonUniformSections]) ;

  // column title
  wxStaticText *numSlicesTitleTxt = new wxStaticText(m_NonUniformSlicesDlg, wxID_ANY, "Number of slices", wxPoint(0,0), wxSize(75,textHeight)) ;
  widgetsBoxSizer->Add(numSlicesTitleTxt, 0, wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL | wxALL, 1);

  // number of slices entry widgets (stacked in reverse order)
  for (int i = m_NumberOfNonUniformSections-1 ;  i >= 0 ;  i--){
    wxString label = albaString::Format("%d",i);

    m_NumSlicesLabel[i] = new wxStaticText(m_NonUniformSlicesDlg, wxID_ANY, label, wxPoint(0,0), wxSize(25,textCtrlHeight));
    m_NumSlicesEntry[i] = new wxTextCtrl(m_NonUniformSlicesDlg , wxID_ANY, "", wxPoint(0,0), wxSize(50,textCtrlHeight), wxNO_BORDER);
    m_NumSlicesEntry[i]->SetValidator(albaGUIValidator(this, (ID_NUSLICES_NUMBER_BEGIN+i), m_NumSlicesEntry[i], &m_SlicesInSection[i]));

    m_NumSlicesRowBoxSizer[i] = new wxBoxSizer(wxHORIZONTAL);
    m_NumSlicesRowBoxSizer[i]->Add(m_NumSlicesLabel[i], 0.5, wxEXPAND | wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL, 1);
    m_NumSlicesRowBoxSizer[i]->Add(m_NumSlicesEntry[i], 0.5, wxEXPAND | wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL, 1);

    widgetsBoxSizer->AddSpacer(spacerHeight) ;
    widgetsBoxSizer->Add(m_NumSlicesRowBoxSizer[i], 0.5, wxEXPAND | wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL | wxALL, 0);
    widgetsBoxSizer->AddSpacer(spacerHeight) ;
  }

  // total box
  wxStaticText *totalSlicesLabel  = new wxStaticText(m_NonUniformSlicesDlg, wxID_ANY, "total", wxPoint(0,0), wxSize(25,textHeight));
  wxTextCtrl   *totalSlicesTxt = new wxTextCtrl(m_NonUniformSlicesDlg , ID_NUSLICES_TOTAL, "", wxPoint(0,0), wxSize(50,textHeight), wxTE_READONLY | wxNO_BORDER );
  totalSlicesTxt->SetValidator(albaGUIValidator(this, ID_NUSLICES_TOTAL, totalSlicesTxt, &m_NumberOfScans));

  wxBoxSizer *totalSlicesBoxSizer = new wxBoxSizer(wxHORIZONTAL) ;
  totalSlicesBoxSizer->Add(totalSlicesLabel, 0.5, wxEXPAND | wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL, 1);
  totalSlicesBoxSizer->Add(totalSlicesTxt, 0.5, wxEXPAND | wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL, 1);

  // memory warning
  m_WarningTotalSlices = new wxStaticText(m_NonUniformSlicesDlg, wxID_ANY, "Warning: May exceed memory", wxPoint(0,0), wxSize(150,20));
  m_WarningTotalSlices->Enable(false) ;
  wxBoxSizer *warningBoxSizer = new wxBoxSizer(wxHORIZONTAL) ;
  warningBoxSizer->Add(m_WarningTotalSlices, 0.5, wxEXPAND | wxALIGN_RIGHT | wxALIGN_CENTRE_VERTICAL, 1);

  widgetsBoxSizer->Add(totalSlicesBoxSizer, 0, wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL | wxALL, 1);
  widgetsBoxSizer->Add(warningBoxSizer, 0, wxALIGN_LEFT | wxALIGN_CENTRE_VERTICAL | wxALL, 1);



  //----------------------------------------------------------------------------
  // ok/cancel buttons
  //----------------------------------------------------------------------------
  albaGUIButton *SliceOk = new albaGUIButton(m_NonUniformSlicesDlg, ID_NUSLICES_OK, "OK", wxPoint(0,0), wxSize(50,20));
  SliceOk->SetListener(this);

  albaGUIButton *b_cancel = new albaGUIButton(m_NonUniformSlicesDlg, ID_NUSLICES_CANCEL, "CANCEL", wxPoint(0,0), wxSize(50,20));
  b_cancel->SetListener(this);

  wxBoxSizer *okCancelBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  okCancelBoxSizer->Add(SliceOk, 0, wxALIGN_CENTRE_HORIZONTAL, 0);
  okCancelBoxSizer->Add(b_cancel, 0, wxALIGN_CENTRE_HORIZONTAL, 0);



  //----------------------------------------------------------------------------
  // put columns together and add to dialog
  //----------------------------------------------------------------------------
  wxBoxSizer *twoColumnsBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  twoColumnsBoxSizer->Add(RenWinBoxSizer, 0, wxEXPAND | wxALL, 1) ;
  twoColumnsBoxSizer->Add(widgetsBoxSizer, 1, wxEXPAND | wxALL, 1) ;

  wxBoxSizer *allBoxSizer = new wxBoxSizer(wxVERTICAL);
  allBoxSizer->Add(twoColumnsBoxSizer, 0, wxEXPAND | wxALL, 1) ;
  allBoxSizer->Add(okCancelBoxSizer, 1, wxEXPAND | wxALIGN_CENTRE_HORIZONTAL | wxALL, 1) ;

  m_NonUniformSlicesDlg->Add(allBoxSizer, 1, wxEXPAND | wxALL, 2);    
  twoColumnsBoxSizer->Fit(m_NonUniformSlicesDlg);



  //----------------------------------------------------------------------------
  // create visual pipe
  //----------------------------------------------------------------------------
  m_NonUniformSlicePipe = new albaOpMML3NonUniformSlicePipe(m_MuscleGlobalReg, m_SectionsViewRWI->m_RenFront, m_NumberOfNonUniformSections) ;
  m_NonUniformSlicePipe->SetEndsOfAxis(m_Axis1Point_PatientCoords, m_Axis2Point_PatientCoords) ;

  m_SectionsViewRWI->CameraUpdate();
  UpdateNonUniformVisualPipe() ; // transfer initial numbers from dialog to visual pipe

  m_NumberOfScansSaved = m_NumberOfScans ;  // save no. of scans so we can undo later if dialog is cancelled

  m_NonUniformSlicesDlg->ShowModal() ;

}




//------------------------------------------------------------------------------
// Delete non-uniform slice spacing dialog
void albaOpMML3::DeleteNonUniformSlicesDlg()
//------------------------------------------------------------------------------
{
  delete m_SectionsViewRWI ;
  delete m_NonUniformSlicePipe ;
  delete m_NonUniformSlicesDlg ;

  delete [] m_NumSlicesRowBoxSizer ;
  delete [] m_NumSlicesLabel ;
  delete [] m_NumSlicesEntry ;
}



//------------------------------------------------------------------------------
// Get vtk data for volume and surface
// Must run inputs dialog first 
void albaOpMML3::GetVtkInputs()
//------------------------------------------------------------------------------
{
  // get alba gray volume 
  m_VolumeVME = albaVMEVolumeGray::SafeDownCast(m_Input);
  assert(!(m_VolumeVME == NULL));

  // get vtk rectilinear grid 
  m_Volume = m_VolumeVME->GetOutput()->GetVTKData();
  assert(!(m_Volume == NULL));

  // polydata muscle should be known already from OnMuscleSelection()
  assert(!(m_Muscle == NULL));
}




//------------------------------------------------------------------------------
// Get absolute position of landmark.
// Required because landmark->GetPoint() only returns pos relative to parent.
void albaOpMML3::GetAbsPosOfLandmark(albaVMELandmark *landmark, double point[3])
//------------------------------------------------------------------------------
{
  vtkMatrix4x4 *mat = landmark->GetOutput()->GetAbsMatrix()->GetVTKMatrix() ;
  point[0] = mat->GetElement(0,3) ;
  point[1] = mat->GetElement(1,3) ;
  point[2] = mat->GetElement(2,3) ;
}



//------------------------------------------------------------------------------
// Enable/Disable buttons which only work in 2d display mode
//------------------------------------------------------------------------------
void albaOpMML3::Set2DButtonsEnable(bool enable)
{
  m_UndoButton->Enable(enable) ;
  m_PlaceOpButton->Enable(enable) ;
  m_TranslateOpButton->Enable(enable) ;
  m_RotateOpButton->Enable(enable) ;
  m_ScaleOpButton->Enable(enable) ;
}




//------------------------------------------------------------------------------
// Transfer the numbers in the non-uniform slice dialog to the visual pipe
void albaOpMML3::UpdateNonUniformVisualPipe()
//------------------------------------------------------------------------------
{
  double *alpha = new double[m_NumberOfScans] ;
  CalculateSlicePositionsAlongAxis(alpha) ;
  m_NonUniformSlicePipe->SetSlicePositions(m_NumberOfScans, alpha) ;
  m_NonUniformSlicePipe->Update() ;
  delete [] alpha ;
}




//------------------------------------------------------------------------------
// Event handler
void albaOpMML3::OnEvent(albaEventBase *alba_event) 
//------------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event)){

    // check for range of id's corresponding to non-uniform slice entries
    if ((e->GetId() >= ID_NUSLICES_NUMBER_BEGIN) && (e->GetId() <= ID_NUSLICES_NUMBER_LAST)){
      OnTextNumberChange(e->GetId());
      return ;
    }

    switch(e->GetId())
    {
    case ID_NUSLICES_OK:// non-uniform slicing dlg OK
      m_NonUniformSlicesDlg->EndModal(wxID_OK);
      break;

    case ID_NUSLICES_CANCEL:// non-uniform slicing dlg cancel
      OnNonUniformCancel() ;
      m_NonUniformSlicesDlg->EndModal(wxID_CANCEL);
      break;

    case ID_INPUTS_NUMSLICES: // number of slices changed
      OnNumberOfSlices();
      break;

    case ID_INPUTS_RESSLICES: // resolution of slices changed
      OnResolutionOfSlices();
      break;

    case ID_INPUTS_SURFACE: // set up dlg muscle selection
      //
      OnMuscleSelection();
      break;

    case ID_INPUTS_L1: // set up dlg L1
      OnLandmark1AtlasPatientSelection();
      break;

    case ID_INPUTS_L2: // set up dlg L2
      OnLandmark2AtlasPatientSelection();
      break;

    case ID_INPUTS_L3: // set up dlg L3
      OnLandmark3AtlasPatientSelection();
      break;

    case ID_INPUTS_L4: // set up dlg L4
      OnLandmark4AtlasPatientSelection();
      break;

    case ID_INPUTS_SLICEXYZ: // radio box to choose default slice direction
      OnSliceXYZ() ;
      break ;

    case ID_INPUTS_AXIS1: // set up dlg to select axis 1
      OnLandmarkAxis1AtlasSelection();
      break;

    case ID_INPUTS_AXIS2: // set up dlg to select axis 2
      OnLandmarkAxis2AtlasSelection();
      break;

    case ID_INPUTS_AXIS3: // set up dlg to select axis 2
      OnLandmarkAxis3AtlasSelection();
      break;

    case ID_INPUTS_OK: // set up dlg ok
      OnInputsOk() ;
      m_InputsDlg->EndModal(wxID_OK);
      break;

    case ID_INPUTS_CANCEL: // set up dlg cancel
      m_InputsDlg->EndModal(wxID_CANCEL);
      break;

    case ID_REG_DISPLAY_MODE: // registration dlg axes on/off
      OnDisplayMode();
      break;

    case ID_REG_PREVIEW: // registration dlg contour on/off
      OnPreview();
      break;

    case ID_REG_RESET_VIEW: // registration dlg reset view
      OnResetView();
      break;

    case ID_REG_OK: // registration dlg ok
      OnRegistrationOK();
      m_OpDlg->EndModal(wxID_OK);
      break;

    case ID_REG_CANCEL: // registration dlg cancel
      OnRegistrationCANCEL();
      m_OpDlg->EndModal(wxID_CANCEL);
      break;

    case ID_REG_P_OPERATION: // registration dlg p-op
      OnPOperationButton();
      break;

    case ID_REG_T_OPERATION: // registration dlg t-op
      OnTOperationButton();
      break;

    case ID_REG_R_OPERATION: // registration dlg r-op
      OnROperationButton();
      break;

    case ID_REG_S_OPERATION: // registration dlg s-op
      OnSOperationButton();
      break;

    case ID_REG_UNDO: // registration dlg undo
      OnUndo();
      break;

    case ID_RANGE_MODIFIED: // registration dlg lut
      OnLut();
      break;

    case ID_REG_SLICE: // registration dlg slice slider
      OnSlider();
      break;

    case ID_REG_MINUS10: // registration dlg slice button
      OnMinus10();
      OnSlider(); // need to call this manually
      break;

    case ID_REG_MINUS1: // registration dlg slice button
      OnMinus1();
      OnSlider(); // need to call this manually
      break;

    case ID_REG_PLUS1: // registration dlg slice button
      OnPlus1();
      OnSlider(); // need to call this manually
      break;

    case ID_REG_PLUS10: // registration dlg slice button
      OnPlus10();
      OnSlider(); // need to call this manually
      break;

    default:
      albaEventMacro(*e); 
      break;
    }
  }
}




//------------------------------------------------------------------------------
// Event handler when no. of slices in a section is changed
void albaOpMML3::OnTextNumberChange(albaID id)
//------------------------------------------------------------------------------
{
  int i ;

  // update the total number of slices
  for (i = 0, m_NumberOfScans = 0; i < albaOpMML3::m_NumberOfNonUniformSections; i++)
    m_NumberOfScans += m_SlicesInSection[i];

  m_NonUniformSlicesDlg->TransferDataToWindow();


  // enable warning if memory threshold exceeded
  double sliceMemory = (double)m_NumberOfScans * (double)(m_ScansGrain*m_ScansGrain) / (1024.0*1024.0) ;

  if (sliceMemory <= m_MemoryThreshold)
    m_WarningTotalSlices->Enable(false) ;
  else
    m_WarningTotalSlices->Enable(true) ;

  // display the slice position on the image
  UpdateNonUniformVisualPipe() ;
}



//------------------------------------------------------------------------------
// Cancel non-uniform slices dialog
// This restores the original no. of slices and sets the op to continue with uniform slicing
void albaOpMML3::OnNonUniformCancel()
//------------------------------------------------------------------------------
{
  m_NumberOfScans = m_NumberOfScansSaved ;
  m_NonUniformSliceSpacing = 0 ;
}


//------------------------------------------------------------------------------
// Number of slices changed
void albaOpMML3::OnNumberOfSlices()
//------------------------------------------------------------------------------
{
  // enable warning if memory threshold exceeded
  double sliceMemory = (double)m_NumberOfScans * (double)(m_ScansGrain*m_ScansGrain) / (1024.0*1024.0) ;

  if (sliceMemory <= m_MemoryThreshold)
    m_WarningNumberOfSlices->Enable(false) ;
  else
    m_WarningNumberOfSlices->Enable(true) ;
}


//------------------------------------------------------------------------------
// Resolution of slices changed
void albaOpMML3::OnResolutionOfSlices()
//------------------------------------------------------------------------------
{
  // enable warning if memory threshold exceeded
  double sliceMemory = (double)m_NumberOfScans * (double)(m_ScansGrain*m_ScansGrain) / (1024.0*1024.0) ;

  if (sliceMemory <= m_MemoryThreshold)
    m_WarningNumberOfSlices->Enable(false) ;
  else
    m_WarningNumberOfSlices->Enable(true) ;
}

//------------------------------------------------------------------------------
// Action if check box UseReg changes
void albaOpMML3::OnSliceXYZ()
//------------------------------------------------------------------------------
{
}



//------------------------------------------------------------------------------
// Inputs dialog ok - tidy up
void albaOpMML3::OnInputsOk()
//------------------------------------------------------------------------------
{
  // apply stretch factor to axis landmarks
  // do this only if axis is calculated from selected landmarks
  if (m_AxisLandmarksFlag == 2)
    ApplyAxisRangeFactor() ;

  // calculate default landmarks if not already defined
  if (!m_L1Defined || !m_L2Defined || !m_L3Defined)
    CreateDefaultRegistrationLandmarks();
  if (!m_Axis1Defined || !m_Axis2Defined || ((m_MuscleType == 2) && !m_Axis3Defined))
    CreateDefaultAxisLandmarks();
}




//----------------------------------------------------------------------------
// Registration ok - create output VME
void albaOpMML3::OnRegistrationOK() 
//----------------------------------------------------------------------------
{
  // apply deformations
  m_Model->ApplyRegistrationOps();



  // create vme and set data
  albaSmartPointer<albaVMESurface> vme ;
  vme->SetData(m_MuscleOutput, m_OutputTimeStamp) ;


  // tag 1: status
  albaTagItem status;
  status.SetName("STATUS_TAG");
  vme->GetTagArray()->SetTag(status);

  // tag 2: number of slices
  albaTagItem noofslices;
  noofslices.SetName("NUMBER_OF_SLICES_TAG");
  noofslices.SetValue(m_Model->GetTotalNumberOfScans());
  vme->GetTagArray()->SetTag(noofslices);

  // tag 4: operations stack
  albaTagItem SliceId_StackTag;
  albaTagItem ZValue_StackTag;
  albaTagItem OperationType_StackTag;
  albaTagItem Parameter1_StackTag;
  albaTagItem Parameter2_StackTag;

  // names
  SliceId_StackTag.SetName("SLICE_ID_STACK_TAG");
  ZValue_StackTag.SetName("Z_VALUE_STACK_TAG");
  OperationType_StackTag.SetName("OPERATION_TYPE_STACK_TAG");
  Parameter1_StackTag.SetName("PARAMETER_1_STACK_TAG");
  Parameter2_StackTag.SetName("PARAMETER_2_STACK_TAG");

  // size
  SliceId_StackTag.SetNumberOfComponents(m_Widget->GetNextOperationId());
  ZValue_StackTag.SetNumberOfComponents(m_Widget->GetNextOperationId());
  OperationType_StackTag.SetNumberOfComponents(m_Widget->GetNextOperationId());
  Parameter1_StackTag.SetNumberOfComponents(m_Widget->GetNextOperationId());
  Parameter2_StackTag.SetNumberOfComponents(m_Widget->GetNextOperationId());

  // values
  double operation_stack[5];
  for(int i = 0; i < m_Widget->GetNextOperationId(); i++)
  {
    m_Model->GetOperationsStackTuple(i, operation_stack);

    SliceId_StackTag.SetComponent(operation_stack[0], i);
    ZValue_StackTag.SetComponent(operation_stack[1], i);
    OperationType_StackTag.SetComponent(operation_stack[2], i);
    Parameter1_StackTag.SetComponent(operation_stack[3], i);
    Parameter2_StackTag.SetComponent(operation_stack[4], i);
  } 

  // add now
  vme->GetTagArray()->SetTag(SliceId_StackTag);
  vme->GetTagArray()->SetTag(ZValue_StackTag);
  vme->GetTagArray()->SetTag(OperationType_StackTag);
  vme->GetTagArray()->SetTag(Parameter1_StackTag);
  vme->GetTagArray()->SetTag(Parameter2_StackTag);

  // tag 5: type of muscle
  albaTagItem typeofmuscle;
  typeofmuscle.SetName("TYPE_OF_MUSCLE_TAG");
  typeofmuscle.SetValue(m_Model->GetTypeOfMuscles());
  vme->GetTagArray()->SetTag(typeofmuscle);

  // root vme
  albaVME *RootVME = m_Input->GetRoot();
  /*

  // patient section vme
  albaVME *PatientSectionVME = RootVME->FindInTreeByName(m_PatientMSFSectionName);
  assert (!(PatientSectionVME == NULL));

  // is there a "registered muscles" groupVME under patient section
  // of .msf file? if no, create it, if yes get a pointer to it
  albaVMEGroup *RegisteredMusclesVME = (albaVMEGroup *) PatientSectionVME->FindInTreeByName("registered muscles");
  if(RegisteredMusclesVME == NULL)
  {
  RegisteredMusclesVME = albaVMEGroup::New();
  RegisteredMusclesVME->SetName("registered muscles");
  PatientSectionVME->AddChild(RegisteredMusclesVME);
  GetLogicManager()->VmeAdd(RegisteredMusclesVME);
  }

  // is there a groupVME under the "registered muscles" groupVME with name identical to
  // that of the muscle under registration? If no create it, if yes get a pointer to it
  albaVMEGroup *OurMuscleVME = (albaVMEGroup *) RegisteredMusclesVME->FindInTreeByName(m_Surface_name);
  if(OurMuscleVME == NULL)
  {
  OurMuscleVME = albaVMEGroup::New();
  OurMuscleVME->SetName(m_Surface_name);
  RegisteredMusclesVME->AddChild(OurMuscleVME);
  GetLogicManager()->VmeAdd(OurMuscleVME);
  }

  // no children?
  long num;
  if (OurMuscleVME->GetNumberOfChildren() == 0)
  {
  num = 0;
  }
  else
  {
  // last child
  albaVME *LastChildVME = OurMuscleVME->GetLastChild();
  wxString Name = LastChildVME->GetName();

  // extension
  wxString ext = Name.AfterLast('.');
  ext.ToLong(&num);
  }

  */
  // set name, extension is no of last child + 1
  //	wxString ext;
  //	ext.Printf("%d", num + 1);

  //
  vme->SetName(m_SurfaceName + ".registered");

  // add to tree (save now)
  //OurMuscleVME->AddChild(vme);
  GetLogicManager()->VmeAdd(vme);
}


//----------------------------------------------------------------------------
void albaOpMML3::OnRegistrationCANCEL()
//----------------------------------------------------------------------------
{

}


//----------------------------------------------------------------------------
void albaOpMML3::OnSlider() 
//----------------------------------------------------------------------------
{
  // force immediate update of text display, otherwise it does it after the processing
  m_Text_SlicePos->Update() ;

  // Set the slice in the model view
  m_Model->SetCurrentIdOfScans(m_CurrentSlice);
  m_Model->Update() ;

  // update line actors
  m_PH->SetLineActorX(m_CurrentSlice);
  m_PV->SetLineActorX(m_CurrentSlice);
  m_TH->SetLineActorX(m_CurrentSlice);
  m_TV->SetLineActorX(m_CurrentSlice);
  m_RA->SetLineActorX(m_CurrentSlice);
  m_SN->SetLineActorX(m_CurrentSlice);
  m_SS->SetLineActorX(m_CurrentSlice);
  m_SE->SetLineActorX(m_CurrentSlice);
  m_SW->SetLineActorX(m_CurrentSlice);


  // Update widget bits
  if (m_Widget->GetRotationMode()){
    // update rotation handle
    m_Widget->UpdateRotationHandle();
    m_Widget->SetRotationHandleVisibility();
  }

  // update - scaling mode
  if (m_Widget->GetScalingMode()){
    // update scaling handles
    m_Widget->UpdateScalingHandles();
  }


  // render parameter views
  m_PH->Render();
  m_PV->Render();
  m_TH->Render();
  m_TV->Render();
  m_RA->Render();
  m_SN->Render();
  m_SS->Render();
  m_SE->Render();
  m_SW->Render();

  // render model view
  m_Model->Render();
}



//------------------------------------------------------------------------------
void albaOpMML3::OnMinus10() 
//------------------------------------------------------------------------------
{
  int tens = m_CurrentSlice / 10 ;
  int units = m_CurrentSlice - 10*tens ;

  // subtract 10, or amount required to go to next lowest 10
  if (units == 0)
    m_CurrentSlice -= 10 ;
  else
    m_CurrentSlice -= units ;

  // clamp the value
  if (m_CurrentSlice < 0)
    m_CurrentSlice = 0 ;

  m_OpDlg->TransferDataToWindow() ;
  m_Text_SlicePos->Update() ;   // force immediate update of text display
}


//------------------------------------------------------------------------------
void albaOpMML3::OnMinus1() 
//------------------------------------------------------------------------------
{
  // subtract 1
  m_CurrentSlice -= 1 ;

  // clamp the value
  if (m_CurrentSlice < 0)
    m_CurrentSlice = 0 ;

  m_OpDlg->TransferDataToWindow() ;
  m_Text_SlicePos->Update() ;   // force immediate update of text display
}


//------------------------------------------------------------------------------
void albaOpMML3::OnPlus1() 
//------------------------------------------------------------------------------
{
  // add 1
  m_CurrentSlice += 1 ;

  // clamp the value
  if (m_CurrentSlice >= m_NumberOfScans)
    m_CurrentSlice = m_NumberOfScans-1 ;

  m_OpDlg->TransferDataToWindow() ;
  m_Text_SlicePos->Update() ;   // force immediate update of text display
}


//------------------------------------------------------------------------------
void albaOpMML3::OnPlus10() 
//------------------------------------------------------------------------------
{
  int tens = m_CurrentSlice / 10 ;
  int units = m_CurrentSlice - 10*tens ;

  // add 10, or amount required to go to next highest 10
  m_CurrentSlice += (10-units) ;

  // clamp the value
  if (m_CurrentSlice >= m_NumberOfScans)
    m_CurrentSlice = m_NumberOfScans-1 ;

  m_OpDlg->TransferDataToWindow() ;
  m_Text_SlicePos->Update() ;   // force immediate update of text display
}



//----------------------------------------------------------------------------
void albaOpMML3::OnLut() 
//----------------------------------------------------------------------------
{
  double Low,High;

  m_Lut->GetSubRange(&Low, &High);
  m_Model->GetVisualPipe2D()->SetLutRange(Low, High) ;
  m_Model->GetVisualPipe3D()->SetLutRange(Low, High) ;
  m_Model->GetVisualPipePreview()->SetLutRange(Low, High) ;
  m_Model->Render();
}




//----------------------------------------------------------------------------
// Select the muscle polydata
// Transform the muscle with the pose matrix.
void albaOpMML3::OnMuscleSelection() 
//----------------------------------------------------------------------------
{
  // raise event to request a VME selection dialog
  albaString title = "Select Muscle (Atlas)";
  albaEvent e(this,VME_CHOOSE);
  e.SetString(&title);
  e.SetPointer(&albaOpMML3::AcceptVMESurface) ; // accept only landmark vme's
  albaEventMacro(e);
  albaVME *vme = e.GetVme();
  if(!vme) return;

  // get vme surface
  m_SurfaceVME = albaVMESurface::SafeDownCast(vme);

  //
  if(m_SurfaceVME == NULL)
  {
    wxMessageBox("Wrong type of vme, a muscle vme is required","alert",wxICON_WARNING);
    return;
  }

  // get vme name
  m_SurfaceName = vme->GetName();

  // get vme time stamp and set output stamp to input
  m_InputTimeStamp = vme->GetTimeStamp() ;
  m_OutputTimeStamp = m_InputTimeStamp ;
  m_InputsDlg->TransferDataToWindow();



  // if muscle registered previously
  // read in relevant tags stored in
  if(vme->GetTagArray()->IsTagPresent("STATUS_TAG"))
  {
    // flag (active)
    m_RegistrationStatus = 1;


    // tag 2: number of slices
    if(vme->GetTagArray()->IsTagPresent("NUMBER_OF_SLICES_TAG") == false)
    {
      wxMessageBox("Number of slices tag in already registered muscle vme is missing!","alert",wxICON_WARNING);
      return;
    }
    albaTagItem *NumberOfSlicesTag;
    NumberOfSlicesTag = vme->GetTagArray()->GetTag("NUMBER_OF_SLICES_TAG");
    m_NumberOfScans = NumberOfSlicesTag->GetComponentAsDouble(0);

    // Disable widget so no. of slices can't be changed
    m_NumberOfScansTxt->Enable(false) ;



    // tag 4: operation stack
    //
    // slice id
    if(vme->GetTagArray()->IsTagPresent("SLICE_ID_STACK_TAG") == false)
    {
      wxMessageBox("Slice id tag in already registered muscle vme is missing!","alert",wxICON_WARNING);
      return;
    }
    m_CurrentSliceIdStackTag = vme->GetTagArray()->GetTag("SLICE_ID_STACK_TAG");

    // z value
    if(vme->GetTagArray()->IsTagPresent("Z_VALUE_STACK_TAG") == false)
    {
      wxMessageBox("Z value tag in already registered muscle vme is missing!","alert",wxICON_WARNING);
      return;
    }
    m_ZValueStackTag = vme->GetTagArray()->GetTag("Z_VALUE_STACK_TAG");

    // operation type
    if(vme->GetTagArray()->IsTagPresent("OPERATION_TYPE_STACK_TAG") == false)
    {
      wxMessageBox("Operation type tag in already registered muscle vme is missing!","alert",wxICON_WARNING);
      return;
    }
    m_OperationTypeStackTag = vme->GetTagArray()->GetTag("OPERATION_TYPE_STACK_TAG");

    // parameter 1
    if(vme->GetTagArray()->IsTagPresent("PARAMETER_1_STACK_TAG") == false)
    {
      wxMessageBox("Parameter 1 tag in already registered muscle vme is missing!","alert",wxICON_WARNING);
      return;
    }
    m_Parameter1StackTag = vme->GetTagArray()->GetTag("PARAMETER_1_STACK_TAG");

    // parameter 2
    if(vme->GetTagArray()->IsTagPresent("PARAMETER_2_STACK_TAG") == false)
    {
      wxMessageBox("Parameter 2 tag in already registered muscle vme is missing!","alert",wxICON_WARNING);
      return;
    }
    m_Parameter2StackTag = vme->GetTagArray()->GetTag("PARAMETER_2_STACK_TAG");


    // tag 5: type of muscle
    if(vme->GetTagArray()->IsTagPresent("TYPE_OF_MUSCLE_TAG") == false)
    {
      wxMessageBox("Type of muscle tag in already registered muscle vme is missing!","alert",wxICON_WARNING);
      return;
    }
    albaTagItem *TypeOfMuscleTag;
    TypeOfMuscleTag = vme->GetTagArray()->GetTag("TYPE_OF_MUSCLE_TAG");
    m_MuscleType = TypeOfMuscleTag->GetComponentAsDouble(0);

    // set m_SurfaceVME to parent vme (original non-registered muscle)
    // to identify landmarks correctly
    wxString ParentVMEName;
    ParentVMEName = m_SurfaceName.BeforeFirst('.');
    m_SurfaceName = ParentVMEName;
  }



  // set up specific set of landmarks (parameters are .msf section names)
  //SetUpLandmarks2(m_AtlasMSFSectionName, m_PatientMSFSectionName);

  //
  albaVME *RootVME;
  albaVME *AtlasSectionVME;
  albaVME *MuscleInAtlasSectionVME;

  if (m_RegistrationStatus == 1)
  {
    // root vme
    RootVME = m_Input->GetRoot();

    // atlas section vme
    AtlasSectionVME = RootVME->FindInTreeByName(m_AtlasMSFSectionName);
    assert (!(AtlasSectionVME == NULL));

    MuscleInAtlasSectionVME = AtlasSectionVME->FindInTreeByName(m_SurfaceName);
    assert (!(MuscleInAtlasSectionVME == NULL));

    //
    m_SurfaceVME = albaVMESurface::SafeDownCast(MuscleInAtlasSectionVME);
  }


  // set muscle data to input * abs pose matrix
  vtkTransform *transform = vtkTransform::New();
  vtkTransformPolyDataFilter *musclepd = vtkTransformPolyDataFilter::New();
  transform->SetMatrix(m_SurfaceVME->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
  musclepd->SetInputData((vtkPolyData*)m_SurfaceVME->GetOutput()->GetVTKData());
  musclepd->SetTransform(transform);
  transform->Delete() ;

  // set muscle as output from transform
  musclepd->Update();
  int numPts = musclepd->GetOutput()->GetPoints()->GetNumberOfPoints() ;
  m_Muscle->DeepCopy(musclepd->GetOutput()) ;
  musclepd->Delete() ;


  //
  //	// if all landmarks chosen
  //	if (m_L1Name.compare("none") != 0 &&
  //		m_L2Name.compare("none") != 0 &&
  //		m_L3Name.compare("none") != 0
  //	   )
  //	{
  //		// activate ok button
  //    m_Choose_ok->Enable(true);
  //	}
  //	else
  //		// de-activate ok button
  //		m_Choose_ok->Enable(false);  


  m_InputsOk->Enable(true);

  // update window
  m_InputsDlg->TransferDataToWindow();
}




//----------------------------------------------------------------------------
void albaOpMML3::OnResetView() 
//----------------------------------------------------------------------------
{
  int n = m_Model->GetRenderer()->GetActors()->GetNumberOfItems() ;
  albaLogMessage("no. of actors = %d", n) ;

  m_Model->GetVisualPipe2D()->Update() ;
  m_Model->GetVisualPipe3D()->Update() ;
  m_Model->GetVisualPipePreview()->Update() ;
  m_Model->ResetCameraPosition() ;
  m_Model->Render();


  // Write debug info to file
  /* std::fstream thing ;
  thing.open("C:/Documents and Settings/Nigel/My Documents/Visual Studio Projects/ALBA/MML3/thing.txt", thing.out | thing.app) ;
  m_Model->PrintSelf(thing, 0) ;
  m_Model->GetVisualPipe2D()->PrintSelf(thing, 0) ;
  m_Widget->PrintSelf(thing, 0) ;
  thing.close() ; */
}




//----------------------------------------------------------------------------
void albaOpMML3::OnUndo() 
//----------------------------------------------------------------------------
{
  int i;
  int FirstLastSliceFlag = 0;

  // 3d mode
  //if (m_3dflag == 1)
  //	return;

  // operations stack empty
  if (m_Widget->GetNextOperationId() == 0)
    return;

  // the operation to undo
  double params_undo[5];
  m_Model->GetOperationsStackTuple(m_Widget->GetNextOperationId() - 1, params_undo);

  int maxsliceid = m_Model->GetTotalNumberOfScans() - 1;

  // total number of data points
  int n = m_PH->GetNumberOfDataPoints();

  // can not undo op on first/last slice if only these are left
  if ((params_undo[0] == 0 || params_undo[0] == maxsliceid))
  {
    FirstLastSliceFlag = 1;
  }

  // must have at least 2 data points
  if (n == 2)
    return;

  // new next op (go back by 1)
  m_Widget->SetNextOperationId(m_Widget->GetNextOperationId() - 1);

  // the new current operation
  double params_undo_again[5];
  m_Model->GetOperationsStackTuple(m_Widget->GetNextOperationId() - 1, params_undo_again);

  // if go back to non-scaling situation reset non-scaling splines colours
  if (m_Model->GetScalingOccured())
  {
    if (m_Model->GetScalingOccuredOperationId() == m_Widget->GetNextOperationId())
    {
      m_Model->SetScalingOccured(false) ;

      // re-draw curves
      m_PH->GetSplineActor()->GetProperty()->SetColor(1.0, 1.0, 1.0);
      m_PH->GetPointsActor()->GetProperty()->SetColor(1.0, 1.0, 1.0);
      m_PH->Render();

      m_PV->GetSplineActor()->GetProperty()->SetColor(1.0, 1.0, 1.0);
      m_PV->GetPointsActor()->GetProperty()->SetColor(1.0, 1.0, 1.0);
      m_PV->Render();

      m_RA->GetSplineActor()->GetProperty()->SetColor(1.0, 1.0, 1.0);
      m_RA->GetPointsActor()->GetProperty()->SetColor(1.0, 1.0, 1.0);
      m_RA->Render();

      m_TH->GetSplineActor()->GetProperty()->SetColor(1.0, 1.0, 1.0);
      m_TH->GetPointsActor()->GetProperty()->SetColor(1.0, 1.0, 1.0);
      m_TH->Render();

      m_TV->GetSplineActor()->GetProperty()->SetColor(1.0, 1.0, 1.0);
      m_TV->GetPointsActor()->GetProperty()->SetColor(1.0, 1.0, 1.0);
      m_TV->Render();
    }
  }


  // undo
  switch ((int) params_undo[2])
  {
  case 0 : // place
    m_PH->RemovePoint(params_undo[0]);
    m_PV->RemovePoint(params_undo[0]);
    m_Model->GetPHSpline()->RemovePoint(params_undo[1]);
    m_Model->GetPVSpline()->RemovePoint(params_undo[1]);

    for(i = m_Widget->GetNextOperationId() - 1; i >= 0 ; i--)
    {
      double params[5];
      m_Model->GetOperationsStackTuple(i, params);
      if (params[0] == params_undo[0] && // same slice
        params[2] == (int) params_undo[2]) // same operation
      {
        m_PH->AddPoint(params_undo[0], params[3]);
        m_PV->AddPoint(params_undo[0], params[4]);
        m_Model->GetPHSpline()->AddPoint(params_undo[1], params[3]);
        m_Model->GetPVSpline()->AddPoint(params_undo[1], params[4]);
        break;
      }
    }

    // first/last slice
    if ((params_undo[0] == 0) || (params_undo[0] == (m_Model->GetTotalNumberOfScans() - 1)))
    {
      if (!(m_PH->GetValue(params_undo[0]) == 0.0))
      {
        m_PH->AddPoint(params_undo[0], 0.0);
        m_Model->GetPHSpline()->AddPoint(params_undo[0], 0.0);
      }

      if (!(m_PV->GetValue(params_undo[0]) == 0.0))
      {
        m_PV->AddPoint(params_undo[0], 0.0);
        m_Model->GetPVSpline()->AddPoint(params_undo[0], 0.0);
      }
    }

    m_PH->Render();
    m_PV->Render();
    break;

  case 1 : // translate
    m_TH->RemovePoint(params_undo[0]);
    m_TV->RemovePoint(params_undo[0]);
    m_Model->GetTHSpline()->RemovePoint(params_undo[1]);
    m_Model->GetTVSpline()->RemovePoint(params_undo[1]);

    for(i = m_Widget->GetNextOperationId() - 1; i >= 0 ; i--)
    {
      double params[5];
      m_Model->GetOperationsStackTuple(i, params);
      if (params[0] == params_undo[0] && // same slice
        params[2] == (int) params_undo[2]) // same operation
      {
        m_TH->AddPoint(params_undo[0], params[3]);
        m_TV->AddPoint(params_undo[0], params[4]);
        m_Model->GetTHSpline()->AddPoint(params_undo[1], params[3]);
        m_Model->GetTVSpline()->AddPoint(params_undo[1], params[4]);
        break;
      }
    }

    // first/last slice
    if ((params_undo[0] == 0) || (params_undo[0] == (m_Model->GetTotalNumberOfScans() - 1)))
    {
      if (!(m_TH->GetValue(params_undo[0]) == 0.0))
      {
        m_TH->AddPoint(params_undo[0], 0.0);
        m_Model->GetTHSpline()->AddPoint(params_undo[0], 0.0);
      }

      if (!(m_TV->GetValue(params_undo[0]) == 0.0))
      {
        m_TV->AddPoint(params_undo[0], 0.0);
        m_Model->GetTVSpline()->AddPoint(params_undo[0], 0.0);
      }
    }
    m_TH->Render();
    m_TV->Render();
    break;

  case 2 : // rotate
    m_RA->RemovePoint(params_undo[0]);
    m_Model->GetRASpline()->RemovePoint(params_undo[1]);

    for(i = m_Widget->GetNextOperationId() - 1; i >= 0 ; i--)
    {
      double params[5];
      m_Model->GetOperationsStackTuple(i, params);
      if (params[0] == params_undo[0] && // same slice
        params[2] == (int) params_undo[2]) // same operation
      {
        m_RA->AddPoint(params_undo[0], params[3]);
        m_Model->GetRASpline()->AddPoint(params_undo[1], params[3]);
        break;
      }
    }

    // first/last slice
    if ((params_undo[0] == 0) || (params_undo[0] == (m_Model->GetTotalNumberOfScans() - 1)))
    {
      if (!(m_RA->GetValue(params_undo[0]) == 0.0))
      {
        m_RA->AddPoint(params_undo[0], 0.0);
        m_Model->GetRASpline()->AddPoint(params_undo[0], 0.0);
      }
    }
    m_RA->Render();
    break;

  case 3 : // scale north
    m_SN->RemovePoint(params_undo[0]);
    m_Model->GetSNSpline()->RemovePoint(params_undo[1]);

    for(i = m_Widget->GetNextOperationId() - 1; i >= 0 ; i--)
    {
      double params[5];
      m_Model->GetOperationsStackTuple(i, params);
      if (params[0] == params_undo[0] && // same slice
        params[2] == (int) params_undo[2]) // same operation
      {
        m_SN->AddPoint(params_undo[0], params[3]);
        m_Model->GetSNSpline()->AddPoint(params_undo[1], params[3]);
        break;
      }
    }

    // first/last slice
    if ((params_undo[0] == 0) || (params_undo[0] == (m_Model->GetTotalNumberOfScans() - 1)))
    {
      if (!(m_SN->GetValue(params_undo[0]) == 1.0))
      {
        m_SN->AddPoint(params_undo[0], 1.0);
        m_Model->GetSNSpline()->AddPoint(params_undo[0], 1.0);
      }
    }
    m_SN->Render();
    break;

  case 4 : // scale south
    m_SS->RemovePoint(params_undo[0]);
    m_Model->GetSSSpline()->RemovePoint(params_undo[1]);

    for(i = m_Widget->GetNextOperationId() - 1; i >= 0 ; i--)
    {
      double params[5];
      m_Model->GetOperationsStackTuple(i, params);
      if (params[0] == params_undo[0] && // same slice
        params[2] == (int) params_undo[2]) // same operation
      {
        m_SS->AddPoint(params_undo[0], params[3]);
        m_Model->GetSSSpline()->AddPoint(params_undo[1], params[3]);
        break;
      }
    }

    // first/last slice
    if ((params_undo[0] == 0) || (params_undo[0] == (m_Model->GetTotalNumberOfScans() - 1)))
    {
      if (!(m_SS->GetValue(params_undo[0]) == 1.0))
      {
        m_SS->AddPoint(params_undo[0], 1.0);
        m_Model->GetSSSpline()->AddPoint(params_undo[0], 1.0);
      }
    }
    m_SS->Render();
    break;

  case 5 : // scale east
    m_SE->RemovePoint(params_undo[0]);
    m_Model->GetSESpline()->RemovePoint(params_undo[1]);

    for(i = m_Widget->GetNextOperationId() - 1; i >= 0 ; i--)
    {
      double params[5];
      m_Model->GetOperationsStackTuple(i, params);
      if (params[0] == params_undo[0] && // same slice
        params[2] == (int) params_undo[2]) // same operation
      {
        m_SE->AddPoint(params_undo[0], params[3]);
        m_Model->GetSESpline()->AddPoint(params_undo[1], params[3]);
        break;
      }
    }

    // first/last slice
    if ((params_undo[0] == 0) || (params_undo[0] == (m_Model->GetTotalNumberOfScans() - 1)))
    {
      if (!(m_SE->GetValue(params_undo[0]) == 1.0))
      {
        m_SE->AddPoint(params_undo[0], 1.0);
        m_Model->GetSESpline()->AddPoint(params_undo[0], 1.0);
      }
    }
    m_SE->Render();
    break;

  case 6 : // scale west
    m_SW->RemovePoint(params_undo[0]);
    m_Model->GetSWSpline()->RemovePoint(params_undo[1]);

    for(i = m_Widget->GetNextOperationId() - 1; i >= 0 ; i--)
    {
      double params[5];
      m_Model->GetOperationsStackTuple(i, params);
      if (params[0] == params_undo[0] && // same slice
        params[2] == (int) params_undo[2]) // same operation
      {
        m_SW->AddPoint(params_undo[0], params[3]);
        m_Model->GetSWSpline()->AddPoint(params_undo[1], params[3]);
        break;
      }
    }

    // first/last slice
    if ((params_undo[0] == 0) || (params_undo[0] == (m_Model->GetTotalNumberOfScans() - 1)))
    {
      if (!(m_SW->GetValue(params_undo[0]) == 1.0))
      {
        m_SW->AddPoint(params_undo[0], 1.0);
        m_Model->GetSWSpline()->AddPoint(params_undo[0], 1.0);
      }
    }
    m_SW->Render();
    break;

  }

  wxColour Green = wxColour(0, 255, 0);

  // the new current operation mode
  switch ((int) params_undo_again[2])
  {
  case 0: // place
    // other buttons off
    m_TranslateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_TranslateOpButton->SetLabel("T");
    m_RotateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_RotateOpButton->SetLabel("R");
    m_ScaleOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_ScaleOpButton->SetLabel("S");

    // this button on
    m_PlaceOpButton->SetBackgroundColour(Green);
    m_PlaceOpButton->SetLabel(">>P<<");

    //
    ResetOperation();

    // prepare display information
    m_Model->GetVisualPipe2D()->SetTextXPosition(0.0, 0.9) ;
    m_Model->GetVisualPipe2D()->SetTextYPosition(0.0, 0.8) ;

    m_Widget->PlaceModeOn();
    break;

  case 1: // translate
    // other buttons off
    m_PlaceOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_PlaceOpButton->SetLabel("P");
    m_RotateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_RotateOpButton->SetLabel("R");
    m_ScaleOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_ScaleOpButton->SetLabel("S");

    // this button on
    m_TranslateOpButton->SetBackgroundColour(Green);
    m_TranslateOpButton->SetLabel(">>T<<");

    //
    ResetOperation();

    // prepare display information
    m_Model->GetVisualPipe2D()->SetTextXPosition(0.0, 0.9) ;
    m_Model->GetVisualPipe2D()->SetTextYPosition(0.0, 0.8) ;

    m_Widget->TranslationModeOn();
    break;

  case 2: // rotate
    // other buttons off
    m_PlaceOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_PlaceOpButton->SetLabel("P");
    m_TranslateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_TranslateOpButton->SetLabel("T");
    m_ScaleOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_ScaleOpButton->SetLabel("S");

    // this button on
    m_RotateOpButton->SetBackgroundColour(Green);
    m_RotateOpButton->SetLabel(">>R<<");

    //
    ResetOperation();

    // prepare display information
    m_Model->GetVisualPipe2D()->SetTextYPosition(0.0, 0.8) ;

    m_Widget->UpdateRotationHandle();
    m_Widget->RotationHandleOn();
    m_Widget->SetRotationHandleVisibility();
    m_Widget->RotationModeOn();
    break;
  }

  if (params_undo_again[2] == 3 || params_undo_again[2] == 4 ||
    params_undo_again[2] == 5 || params_undo_again[2] == 6)
    // ok, the new current operation mode is scaling
  {
    // other buttons off
    m_PlaceOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_PlaceOpButton->SetLabel("P");
    m_TranslateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_TranslateOpButton->SetLabel("T");
    m_RotateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_RotateOpButton->SetLabel("R");

    // this button on
    m_ScaleOpButton->SetBackgroundColour(Green);
    m_ScaleOpButton->SetLabel(">>S<<");

    m_Model->GetVisualPipe2D()->SetTextYPosition(0.0, 0.9) ;

    //
    ResetOperation();

    m_Widget->UpdateScalingHandles();
    m_Widget->ScalingHandlesOn();
    m_Widget->ScalingModeOn();
  }


  // update model view
  m_Model->Update() ;


  // update widget
  m_Widget->UpdateWidgetTransform();
 
  if (m_Widget->GetRotationMode())
  {
    // update rotation handle
    m_Widget->UpdateRotationHandle();

    // visibility
    m_Widget->SetRotationHandleVisibility();
  }

  // update - scaling mode
  if (m_Widget->GetScalingMode())
  {
    // update scaling handles
    m_Widget->UpdateScalingHandles();
  }

  //
  m_Model->Render();
}





//----------------------------------------------------------------------------
void albaOpMML3::ResetOperation() 
//----------------------------------------------------------------------------
{
  // reset - handles off
  m_Widget->RotationHandleOff();
  m_Widget->ScalingHandlesOff();

  // reset - modes off
  m_Widget->PlaceModeOff();
  m_Widget->ScalingModeOff();
  m_Widget->RotationModeOff();
  m_Widget->TranslationModeOff();
}




//----------------------------------------------------------------------------
// Display mode button
void albaOpMML3::OnDisplayMode() 
//----------------------------------------------------------------------------
{
  m_Flag3D = (m_Flag3D + 1) % 2 ;

  if (m_Flag3D == 0){
    m_Model->SetDisplay2D() ;
    m_Widget->On() ;
    m_DisplayModeButton->SetLabel("Display 3D");
    Set2DButtonsEnable(true) ;
  }
  else{
    m_Model->SetDisplay3D() ;
    m_Widget->Off() ;
    m_DisplayModeButton->SetLabel("Display 2D");
    Set2DButtonsEnable(false) ;
  }

  // switch off the preview
  m_PreviewFlag = 0 ;
  m_PreviewButton->SetLabel("Preview On") ;

  m_Model->Render();
}




//----------------------------------------------------------------------------
// Preview button
void albaOpMML3::OnPreview() 
//----------------------------------------------------------------------------
{  
  m_PreviewFlag = (m_PreviewFlag + 1) % 2 ;
  
  if (m_PreviewFlag == 0){
    // switch off preview and return to previous 2d or 3d view
    if (m_Flag3D == 0){
      m_Model->SetDisplay2D() ;
      m_Widget->On() ;
      Set2DButtonsEnable(true) ;
    }
    else{
      m_Model->SetDisplay3D() ;
      m_Widget->Off() ;
      Set2DButtonsEnable(false) ;
    }

    m_PreviewButton->SetLabel("Preview On");
  }
  else{
    // switch on preview
    m_Model->SetDisplayToPreview() ;
    m_Widget->Off() ;
    m_PreviewButton->SetLabel("Preview Off");
    Set2DButtonsEnable(false) ;
  }
  
  m_Model->Render();
}




//----------------------------------------------------------------------------
void albaOpMML3::OnLandmark1AtlasPatientSelection() 
//----------------------------------------------------------------------------
{
  //
  if(m_SurfaceVME == NULL)
  {
    wxMessageBox("No muscle selected","alert",wxICON_WARNING);
    return;
  }

  albaString title = "Select Landmark 1 (Atlas)";
  albaEvent e(this,VME_CHOOSE);
  e.SetString(&title);
  e.SetPointer(&albaOpMML3::AcceptVMELandmark) ; // accept only landmark vme's
  albaEventMacro(e);
  albaVME *vme = e.GetVme();
  if(!vme) return;

  albaVMELandmark *lm = albaVMELandmark::SafeDownCast(vme);
  if(lm == NULL)
  {
    wxMessageBox("wrong type of vme, a Landmark VME is required","alert",wxICON_WARNING);
    return;
  }

  // get root node
  albaVME *root = m_Input->GetRoot();

  // get landmarks parent node
  albaVME* parentvme = vme->GetParent();

  // search vme tree upwards
  wxString   parentvmename;
  bool inatlasmsfsection = 0;

  while (parentvme != root)
  {
    parentvme = parentvme->GetParent();
    parentvmename = parentvme->GetName();

    if (parentvmename.compare(m_AtlasMSFSectionName) == 0)
    {
      inatlasmsfsection = 1;
      break;
    }
  }

  if (!inatlasmsfsection)
  {
    wxMessageBox("Landmark chosen is not from the " + m_AtlasMSFSectionName + " section","alert",wxICON_WARNING);
    return;
  }

  // patient - landmark 1
  // get parent node
  albaVME *L1PatientParentVME =root->FindInTreeByName(m_PatientMSFSectionName);
  if(L1PatientParentVME == NULL)
  {
    wxMessageBox("No " + m_PatientMSFSectionName + " section", "alert", wxICON_WARNING);
    return;
  }

  // if identical to L2 or L3
  wxString   Name = vme->GetName();
  if (Name.compare(m_L2Name) == 0 || Name.compare(m_L3Name) == 0)
  {
    wxMessageBox("landmarks must be distinct", "alert", wxICON_WARNING);
    return;
  }

  // get landmark node
  albaVME *L1PatientVME = L1PatientParentVME->FindInTreeByName(vme->GetName());
  if(L1PatientVME == NULL)
  {
    wxMessageBox("Landmark missing from " + m_PatientMSFSectionName + " section","alert",wxICON_WARNING);
    return;
  }

  // get landmark
  albaVMELandmark *L1PatientVMELandmark = albaVMELandmark::SafeDownCast(L1PatientVME);
  if(L1PatientVMELandmark == NULL)
  {
    wxMessageBox("wrong type of vme, a landmark vme is required","alert",wxICON_WARNING);
    return;
  }

  // coordinates
  GetAbsPosOfLandmark(lm, m_L1Point); // nb we need abs pos of landmark
  L1PatientVMELandmark->GetPoint(m_P1Point);

  // set name
  m_L1Name = vme->GetName();
  m_P1Name = L1PatientVME->GetName();

  // set flag
  m_L1Defined = (m_L1Name.compare("none") != 0) ;


  // if all landmarks chosen
  if (m_L1Name.compare("none") != 0 && m_L2Name.compare("none") != 0 && m_L3Name.compare("none") != 0)
  {
    // activate ok button
    m_InputsOk->Enable(true);
  }
  else{
    // de-activate ok button
    m_InputsOk->Enable(false);
  }

  // update window
  m_InputsDlg->TransferDataToWindow();
}






//----------------------------------------------------------------------------
void albaOpMML3::OnLandmark2AtlasPatientSelection() 
//----------------------------------------------------------------------------
{
  //
  if(m_SurfaceVME == NULL)
  {
    wxMessageBox("No muscle selected","alert",wxICON_WARNING);
    return;
  }

  albaString title = "Select Landmark 2 (Atlas)";
  albaEvent e(this,VME_CHOOSE);
  e.SetString(&title);
  e.SetPointer(&albaOpMML3::AcceptVMELandmark) ; // accept only landmark vme's
  albaEventMacro(e);
  albaVME *vme = e.GetVme();
  if(!vme) return;

  albaVMELandmark *lm = albaVMELandmark::SafeDownCast(vme);
  if(lm == NULL)
  {
    wxMessageBox("wrong type of vme, a Landmark VME is required","alert",wxICON_WARNING);
    return;
  }

  // get root node
  albaVME *root = m_Input->GetRoot();

  // get landmarks parent node
  albaVME* parentvme = vme->GetParent();

  // search vme tree upwards
  wxString   parentvmename;
  bool inatlasmsfsection = 0;

  while (parentvme != root)
  {
    parentvme = parentvme->GetParent();
    parentvmename = parentvme->GetName();

    if (parentvmename.compare(m_AtlasMSFSectionName) == 0)
    {
      inatlasmsfsection = 1;
      break;
    }
  }

  if (!inatlasmsfsection)
  {
    wxMessageBox("Landmark chosen is not from the " + m_AtlasMSFSectionName + " section","alert",wxICON_WARNING);
    return;
  }

  // patient - landmark 2
  // get parent node
  albaVME *L2PatientParentVME = root->FindInTreeByName(m_PatientMSFSectionName);
  if(L2PatientParentVME == NULL)
  {
    wxMessageBox("No " + m_PatientMSFSectionName + " section", "alert", wxICON_WARNING);
    return;
  }

  // if identical to L1 or L3
  wxString   Name = vme->GetName();
  if (Name.compare(m_L1Name) == 0 || Name.compare(m_L3Name) == 0)
  {
    wxMessageBox("landmarks must be distinct", "alert", wxICON_WARNING);
    return;
  }

  // get landmark node
  albaVME *L2PatientVME = L2PatientParentVME->FindInTreeByName(vme->GetName());
  if(L2PatientVME == NULL)
  {
    wxMessageBox("Landmark missing from " + m_PatientMSFSectionName + " section","alert",wxICON_WARNING);
    return;
  }

  // get landmark
  albaVMELandmark *L2PatientVMELandmark = albaVMELandmark::SafeDownCast(L2PatientVME);
  if(L2PatientVMELandmark == NULL)
  {
    wxMessageBox("wrong type of vme, a landmark vme is required","alert",wxICON_WARNING);
    return;
  }

  // coordinates
  GetAbsPosOfLandmark(lm, m_L2Point); // nb we need abs pos of landmark
  L2PatientVMELandmark->GetPoint(m_P2Point);

  // set name
  m_L2Name = vme->GetName();
  m_P2Name = L2PatientVME->GetName();

  // set flag
  m_L2Defined = (m_L2Name.compare("none") != 0) ;


  // if all landmarks chosen
  if (m_L1Name.compare("none") != 0 && m_L2Name.compare("none") != 0 && m_L3Name.compare("none") != 0){
    // activate ok button
    m_InputsOk->Enable(true);
  }
  else{
    // de-activate ok button
    m_InputsOk->Enable(false);
  }

  // update window
  m_InputsDlg->TransferDataToWindow();
}






//----------------------------------------------------------------------------
void albaOpMML3::OnLandmark3AtlasPatientSelection() 
//----------------------------------------------------------------------------
{
  //
  if(m_SurfaceVME == NULL)
  {
    wxMessageBox("No muscle selected","alert",wxICON_WARNING);
    return;
  }

  albaString title = "Select Landmark 3 (Atlas)";
  albaEvent e(this,VME_CHOOSE);
  e.SetString(&title);
  e.SetPointer(&albaOpMML3::AcceptVMELandmark) ; // accept only landmark vme's
  albaEventMacro(e);
  albaVME *vme = e.GetVme();
  if(!vme) return;

  albaVMELandmark *lm = albaVMELandmark::SafeDownCast(vme);
  if(lm == NULL)
  {
    wxMessageBox("wrong type of vme, a Landmark VME is required","alert",wxICON_WARNING);
    return;
  }

  // get root node
  albaVME *root = m_Input->GetRoot();

  // get landmarks parent node
  albaVME* parentvme = vme->GetParent();

  // search vme tree upwards
  wxString   parentvmename;
  bool inatlasmsfsection = 0;

  while (parentvme != root)
  {
    parentvme = parentvme->GetParent();
    parentvmename = parentvme->GetName();

    if (parentvmename.compare(m_AtlasMSFSectionName) == 0)
    {
      inatlasmsfsection = 1;
      break;
    }
  }

  if (!inatlasmsfsection)
  {
    wxMessageBox("Landmark chosen is not from the " + m_AtlasMSFSectionName + " section","alert",wxICON_WARNING);
    return;
  }

  // patient - landmark 3
  // get parent node
  albaVME *L3PatientParentVME = root->FindInTreeByName(m_PatientMSFSectionName);
  if(L3PatientParentVME == NULL)
  {
    wxMessageBox("No " + m_PatientMSFSectionName + " section", "alert", wxICON_WARNING);
    return;
  }

  // if identical to L1 or L2
  wxString   Name = vme->GetName();
  if (Name.compare(m_L1Name) == 0 || Name.compare(m_L2Name) == 0)
  {
    wxMessageBox("landmarks must be distinct", "alert", wxICON_WARNING);
    return;
  }

  // get landmark node
	albaVME *L3PatientVME = L3PatientParentVME->FindInTreeByName(vme->GetName());
  if(L3PatientVME == NULL)
  {
    wxMessageBox("Landmark missing from " + m_PatientMSFSectionName + " section","alert",wxICON_WARNING);
    return;
  }

  // get landmark
  albaVMELandmark *L3PatientVMELandmark = albaVMELandmark::SafeDownCast(L3PatientVME);
  if(L3PatientVMELandmark == NULL)
  {
    wxMessageBox("wrong type of vme, a landmark vme is required","alert",wxICON_WARNING);
    return;
  }

  // coordinates
  GetAbsPosOfLandmark(lm, m_L3Point); // nb we need abs pos of landmark
  L3PatientVMELandmark->GetPoint(m_P3Point);

  // set name
  m_L3Name = vme->GetName();
  m_P3Name = L3PatientVME->GetName();

  // set flag
  m_L3Defined = (m_L3Name.compare("none") != 0) ;


  // if all landmarks chosen
  if (m_L1Name.compare("none") != 0 && m_L2Name.compare("none") != 0 && m_L3Name.compare("none") != 0){
    // activate ok button
    m_InputsOk->Enable(true);
  }
  else{
    // de-activate ok button
    m_InputsOk->Enable(false);
  }

  // update window
  m_InputsDlg->TransferDataToWindow();
}






//----------------------------------------------------------------------------
void albaOpMML3::OnLandmark4AtlasPatientSelection() 
//----------------------------------------------------------------------------
{
  //
  if(m_SurfaceVME == NULL)
  {
    wxMessageBox("No muscle selected","alert",wxICON_WARNING);
    return;
  }

  albaString title = "Select Landmark 3 (Atlas)";
  albaEvent e(this,VME_CHOOSE);
  e.SetString(&title);
  e.SetPointer(&albaOpMML3::AcceptVMELandmark) ; // accept only landmark vme's
  albaEventMacro(e);
  albaVME *vme = e.GetVme();
  if(!vme) return;

  albaVMELandmark *lm = albaVMELandmark::SafeDownCast(vme);
  if(lm == NULL)
  {
    wxMessageBox("wrong type of vme, a Landmark VME is required","alert",wxICON_WARNING);
    return;
  }

  // get root node
  albaVME *root = m_Input->GetRoot();

  // get landmarks parent node
  albaVME* parentvme = vme->GetParent();

  // search vme tree upwards
  wxString   parentvmename;
  bool inatlasmsfsection = 0;

  while (parentvme != root)
  {
    parentvme = parentvme->GetParent();
    parentvmename = parentvme->GetName();

    if (parentvmename.compare(m_AtlasMSFSectionName) == 0)
    {
      inatlasmsfsection = 1;
      break;
    }
  }

  if (!inatlasmsfsection)
  {
    wxMessageBox("Landmark chosen is not from the " + m_AtlasMSFSectionName + " section","alert",wxICON_WARNING);
    return;
  }

  // patient - landmark 3
  // get parent node
  albaVME *L3PatientParentVME = root->FindInTreeByName(m_PatientMSFSectionName);
  if(L3PatientParentVME == NULL)
  {
    wxMessageBox("No " + m_PatientMSFSectionName + " section", "alert", wxICON_WARNING);
    return;
  }

  // if identical to L1 or L2
  wxString   Name = vme->GetName();
  if (Name.compare(m_L1Name) == 0 || Name.compare(m_L2Name) == 0)
  {
    wxMessageBox("landmarks must be distinct", "alert", wxICON_WARNING);
    return;
  }

  // get landmark node
  albaVME *L3PatientVME = L3PatientParentVME->FindInTreeByName(vme->GetName());
  if(L3PatientVME == NULL)
  {
    wxMessageBox("Landmark missing from " + m_PatientMSFSectionName + " section","alert",wxICON_WARNING);
    return;
  }

  // get landmark
  albaVMELandmark *L3PatientVMELandmark = albaVMELandmark::SafeDownCast(L3PatientVME);
  if(L3PatientVMELandmark == NULL)
  {
    wxMessageBox("wrong type of vme, a landmark vme is required","alert",wxICON_WARNING);
    return;
  }

  // coordinates
  GetAbsPosOfLandmark(lm, m_L4Point); // nb we need abs pos of landmark
  L3PatientVMELandmark->GetPoint(m_P3Point);

  // set name
  m_L3Name = vme->GetName();
  m_P3Name = L3PatientVME->GetName();

  // set flag
  m_L4Defined = (m_L4Name.compare("none") != 0) ;


  // if all landmarks chosen
  if (m_L1Name.compare("none") != 0 && m_L2Name.compare("none") != 0 && m_L3Name.compare("none") != 0){
    // activate ok button
    m_InputsOk->Enable(true);
  }
  else{
    // de-activate ok button
    m_InputsOk->Enable(false);
  }

  // update window
  m_InputsDlg->TransferDataToWindow();
}





//----------------------------------------------------------------------------
// Select landmark axis1
void albaOpMML3::OnLandmarkAxis1AtlasSelection() 
//----------------------------------------------------------------------------
{
  //
  if(m_SurfaceVME == NULL)
  {
    wxMessageBox("No muscle selected","alert",wxICON_WARNING);
    return;
  }

  albaString title = "Select Landmark Axis1 (Atlas)";
  albaEvent e(this,VME_CHOOSE);
  e.SetString(&title);
  e.SetPointer(&albaOpMML3::AcceptVMELandmark) ; // accept only landmark vme's
  albaEventMacro(e);
  albaVME *vme = e.GetVme();
  if(!vme) return;

  albaVMELandmark *lm = albaVMELandmark::SafeDownCast(vme);
  if(lm == NULL)
  {
    wxMessageBox("wrong type of vme, a Landmark VME is required","alert",wxICON_WARNING);
    return;
  }

  // get root node
  albaVME *root = m_Input->GetRoot();

  // get landmarks parent node
  albaVME* parentvme = vme->GetParent();

  // search vme tree upwards
  wxString   parentvmename;
  bool inatlasmsfsection = 0;

  while (parentvme != root)
  {
    parentvme = parentvme->GetParent();
    parentvmename = parentvme->GetName();

    if (parentvmename.compare(m_AtlasMSFSectionName) == 0)
    {
      inatlasmsfsection = 1;
      break;
    }
  }

  if (!inatlasmsfsection)
  {
    wxMessageBox("Landmark chosen is not from the " + m_AtlasMSFSectionName + " section","alert",wxICON_WARNING);
    return;
  }

  // if identical to another landmark
  wxString   Name = vme->GetName();
  if ((Name.compare(m_Axis2Name) == 0) || (Name.compare(m_Axis3Name) == 0))
  {
    wxMessageBox("landmarks must be distinct", "alert", wxICON_WARNING);
    return;
  }


  // coordinates
  GetAbsPosOfLandmark(lm, m_Axis1Point); // nb we need abs pos of landmark

  // set name
  m_Axis1Name = vme->GetName();

  // set flag
  m_Axis1Defined = (m_Axis1Name.compare("none") != 0) ;


  // Now selecting axis by landmarks, so disable the default direction radio box
  m_Radio_slicexyz->Enable(false) ;

  // if all landmarks chosen
  if (m_Axis1Defined && m_Axis2Defined && (m_Axis3Defined || (m_MuscleType == 1))){
    // note that the landmarks have been selected from the atlas
    m_AxisLandmarksFlag = 2 ;

    // activate ok button
    m_InputsOk->Enable(true);
  }
  else{
    // de-activate ok button
    m_InputsOk->Enable(false);
  }

  // update window
  m_InputsDlg->TransferDataToWindow();
}




//----------------------------------------------------------------------------
// Select landmark axis2
void albaOpMML3::OnLandmarkAxis2AtlasSelection() 
//----------------------------------------------------------------------------
{
  //
  if(m_SurfaceVME == NULL)
  {
    wxMessageBox("No muscle selected","alert",wxICON_WARNING);
    return;
  }

  albaString title = "Select Landmark Axis2 (Atlas)";
  albaEvent e(this,VME_CHOOSE);
  e.SetString(&title);
  e.SetPointer(&albaOpMML3::AcceptVMELandmark) ; // accept only landmark vme's
  albaEventMacro(e);
  albaVME *vme = e.GetVme();
  if(!vme) return;

  albaVMELandmark *lm = albaVMELandmark::SafeDownCast(vme);
  if(lm == NULL)
  {
    wxMessageBox("wrong type of vme, a Landmark VME is required","alert",wxICON_WARNING);
    return;
  }

  // get root node
  albaVME *root = m_Input->GetRoot();

  // get landmarks parent node
  albaVME* parentvme = vme->GetParent();

  // search vme tree upwards
  wxString   parentvmename;
  bool inatlasmsfsection = 0;

  while (parentvme != root)
  {
    parentvme = parentvme->GetParent();
    parentvmename = parentvme->GetName();

    if (parentvmename.compare(m_AtlasMSFSectionName) == 0)
    {
      inatlasmsfsection = 1;
      break;
    }
  }

  if (!inatlasmsfsection)
  {
    wxMessageBox("Landmark chosen is not from the " + m_AtlasMSFSectionName + " section","alert",wxICON_WARNING);
    return;
  }

  // if identical to another landmark
  wxString   Name = vme->GetName(); 
  if ((Name.compare(m_Axis1Name) == 0) || (Name.compare(m_Axis3Name) == 0))
  {
    wxMessageBox("landmarks must be distinct", "alert", wxICON_WARNING);
    return;
  }


  // coordinates
  GetAbsPosOfLandmark(lm, m_Axis2Point); // nb we need abs pos of landmark

  // set name
  m_Axis2Name = vme->GetName();

  // set flag
  m_Axis2Defined = (m_Axis2Name.compare("none") != 0) ;


  // Now selecting axis by landmarks, so disable the default direction radio box
  m_Radio_slicexyz->Enable(false) ;

  // if all landmarks chosen
  if (m_Axis1Defined && m_Axis2Defined && (m_Axis3Defined || (m_MuscleType == 1))){
    // note that the landmarks have been selected from the atlas
    m_AxisLandmarksFlag = 2 ;

    // activate ok button
    m_InputsOk->Enable(true);
  }
  else{
    // de-activate ok button
    m_InputsOk->Enable(false);
  }

  // update window
  m_InputsDlg->TransferDataToWindow();
}




//----------------------------------------------------------------------------
// Select landmark axis3
void albaOpMML3::OnLandmarkAxis3AtlasSelection() 
//----------------------------------------------------------------------------
{
  //
  if(m_SurfaceVME == NULL)
  {
    wxMessageBox("No muscle selected","alert",wxICON_WARNING);
    return;
  }

  albaString title = "Select Landmark Axis3 (Atlas)";
  albaEvent e(this,VME_CHOOSE);
  e.SetString(&title);
  e.SetPointer(&albaOpMML3::AcceptVMELandmark) ; // accept only landmark vme's
  albaEventMacro(e);
  albaVME *vme = e.GetVme();
  if(!vme) return;

  albaVMELandmark *lm = albaVMELandmark::SafeDownCast(vme);
  if(lm == NULL)
  {
    wxMessageBox("wrong type of vme, a Landmark VME is required","alert",wxICON_WARNING);
    return;
  }

  // get root node
  albaVME *root = m_Input->GetRoot();

  // get landmarks parent node
  albaVME* parentvme = vme->GetParent();

  // search vme tree upwards
  wxString   parentvmename;
  bool inatlasmsfsection = 0;

  while (parentvme != root)
  {
    parentvme = parentvme->GetParent();
    parentvmename = parentvme->GetName();

    if (parentvmename.compare(m_AtlasMSFSectionName) == 0)
    {
      inatlasmsfsection = 1;
      break;
    }
  }

  if (!inatlasmsfsection)
  {
    wxMessageBox("Landmark chosen is not from the " + m_AtlasMSFSectionName + " section","alert",wxICON_WARNING);
    return;
  }

  // if identical to another landmark
  wxString   Name = vme->GetName(); 
  if ((Name.compare(m_Axis1Name) == 0) || (Name.compare(m_Axis2Name) == 0))
  {
    wxMessageBox("landmarks must be distinct", "alert", wxICON_WARNING);
    return;
  }


  // coordinates
  GetAbsPosOfLandmark(lm, m_Axis3Point); // nb we need abs pos of landmark

  // set name
  m_Axis3Name = vme->GetName();

  // set flag
  m_Axis3Defined = (m_Axis3Name.compare("none") != 0) ;


  // Now selecting axis by landmarks, so disable the default direction radio box
  m_Radio_slicexyz->Enable(false) ;

  // if all landmarks chosen
  if (m_Axis1Defined && m_Axis2Defined && (m_Axis3Defined || (m_MuscleType == 1))){
    // note that the landmarks have been selected from the atlas
    m_AxisLandmarksFlag = 2 ;

    // activate ok button
    m_InputsOk->Enable(true);
  }
  else{
    // de-activate ok button
    m_InputsOk->Enable(false);
  }

  // update window
  m_InputsDlg->TransferDataToWindow();
}





//----------------------------------------------------------------------------
// Create default set of registration landmarks.
// Landmarks are based on the bounds of the input volume.
void albaOpMML3::CreateDefaultRegistrationLandmarks() 
//----------------------------------------------------------------------------
{
  assert(m_Input);

  double inputBounds[6]; 
  m_Input->GetOutput()->GetBounds(inputBounds);
  double xmed = (inputBounds[0] + inputBounds[1]) / 2.0;
  double ymed = (inputBounds[2] + inputBounds[3]) / 2.0;

  double lm1[3] = {xmed, ymed, inputBounds[5]}; //high
  double lm2[3] = {xmed, ymed, inputBounds[4]}; //low
  double lm3[3] = {inputBounds[1], inputBounds[3], inputBounds[4]}; 

  // copy default landmarks to patient and atlas
  for (int i = 0 ;  i < 3 ;  i++){
    m_L1Point[i] = lm1[i] ;
    m_L2Point[i] = lm2[i] ;
    m_L3Point[i] = lm3[i] ;
    m_L4Point[i] = 0.0 ;

    m_P1Point[i] = lm1[i] ;
    m_P2Point[i] = lm2[i] ;
    m_P3Point[i] = lm3[i] ;
    m_P4Point[i] = 0.0 ;
  }

  // set flags indicating which landmarks are defined
  m_L1Defined = true ;
  m_L2Defined = true ;
  m_L3Defined = true ;
  m_L4Defined = false ;
}





//----------------------------------------------------------------------------
// Create default set of axis landmarks.
// The axis is x, y or z in patient coords, depending on selection made in inputs dialog.
void albaOpMML3::CreateDefaultAxisLandmarks() 
//----------------------------------------------------------------------------
{
  assert(m_Input);

  double bnds[6]; 
  m_Input->GetOutput()->GetBounds(bnds);
  double xmed = (bnds[0] + bnds[1]) / 2.0 ;
  double ymed = (bnds[2] + bnds[3]) / 2.0 ;
  double zmed = (bnds[4] + bnds[5]) / 2.0 ;

  if (m_MuscleType == 1){
    if (m_Slicexyz == 0){
      // x axis
      m_Axis1Point[0] = bnds[0] ;  m_Axis1Point[1] = ymed ;  m_Axis1Point[2] = zmed ;
      m_Axis2Point[0] = bnds[1] ;  m_Axis2Point[1] = ymed ;  m_Axis2Point[2] = zmed ;
    }
    else if (m_Slicexyz == 1){
      // y axis
      m_Axis1Point[0] = xmed ;  m_Axis1Point[1] = bnds[2] ;  m_Axis1Point[2] = zmed ;
      m_Axis2Point[0] = xmed ;  m_Axis2Point[1] = bnds[3] ;  m_Axis2Point[2] = zmed ;
    }
    else if (m_Slicexyz == 2){
      // z axis
      m_Axis1Point[0] = xmed ;  m_Axis1Point[1] = ymed ;  m_Axis1Point[2] = bnds[4] ;
      m_Axis2Point[0] = xmed ;  m_Axis2Point[1] = ymed ;  m_Axis2Point[2] = bnds[5] ;
    }
    else{
      // unknown slice direction
      assert(false) ;
    }

    // set flags indicating which landmarks are defined
    m_Axis1Defined = true ;
    m_Axis2Defined = true ;
    m_Axis3Defined = false ;
  }
  else if (m_MuscleType == 2){
    if (m_Slicexyz == 0){
      m_Axis1Point[0] = bnds[0] ;  m_Axis1Point[1] = ymed ;  m_Axis1Point[2] = zmed ;
      m_Axis2Point[0] = xmed ;     m_Axis2Point[1] = ymed ;  m_Axis2Point[2] = zmed ;
      m_Axis3Point[0] = bnds[1] ;  m_Axis3Point[1] = ymed ;  m_Axis3Point[2] = zmed ;
    }
    else if (m_Slicexyz == 1){
      m_Axis1Point[0] = xmed ;  m_Axis1Point[1] = bnds[2] ;  m_Axis1Point[2] = zmed ;
      m_Axis2Point[0] = xmed ;  m_Axis2Point[1] = ymed ;     m_Axis2Point[2] = zmed ;
      m_Axis3Point[0] = xmed ;  m_Axis3Point[1] = bnds[3] ;  m_Axis3Point[2] = zmed ;
    }
    else if (m_Slicexyz == 2){
      m_Axis1Point[0] = xmed ;  m_Axis1Point[1] = ymed ;  m_Axis1Point[2] = bnds[4] ;
      m_Axis2Point[0] = xmed ;  m_Axis2Point[1] = ymed ;  m_Axis2Point[2] = zmed ;
      m_Axis3Point[0] = xmed ;  m_Axis3Point[1] = ymed ;  m_Axis3Point[2] = bnds[5];
    }
    else{
      // unknown slice direction
      assert(false) ;
    }

    // set flags indicating which landmarks are defined
    m_Axis1Defined = true ;
    m_Axis2Defined = true ;
    m_Axis3Defined = true ;
  }
  else{
    // unknown muscle type
    assert(false) ;
  }

  // set flag indicating that landmarks are in patient coords
  m_AxisLandmarksFlag = 1 ;
}



//------------------------------------------------------------------------------
// Apply scaling factor to axis landmarks.
// Do this before uploading the landmarks to the model view.
void albaOpMML3::ApplyAxisRangeFactor()
//------------------------------------------------------------------------------
{
  int i ;
  double s = (m_AxisRangeFactor - 1.0) / 2.0 ;

  switch(m_MuscleType){
    case 1:
      // stretch landmarks 1 and 2 along the vector direction between them
      for (i = 0 ;  i < 3 ;  i++){
        double dv = s*(m_Axis2Point[i] - m_Axis1Point[i]) ;
        m_Axis1Point[i] -= dv ;
        m_Axis2Point[i] += dv ;
      }
      break ;
    case 2:
      // stretch landmarks 1 and 3 away from the central landmark 2
      for (i = 0 ;  i < 3 ;  i++){
        double dv12 = s*(m_Axis1Point[i] - m_Axis2Point[i]) ;
        double dv32 = s*(m_Axis3Point[i] - m_Axis2Point[i]) ;
        m_Axis1Point[i] += dv12 ;
        m_Axis2Point[i] += dv32 ;
      }
      break ;
    default:
      // unknown muscle type
      assert(false) ;
  }
}



//------------------------------------------------------------------------------
// Calculate fractional slice positions along axis,
// where the values 0.0 and 1.0 correspond to the min and max of the slicing range
// NB The first and last slices might not be exactly at 0.0 and 1.0.
// NB Run ApplyAxisRangeFactor() before this - not after !
void albaOpMML3::CalculateSlicePositionsAlongAxis(double *alpha) const
//------------------------------------------------------------------------------
{
  // i indexes the sections, j indexes the slices within each section, k indexes the slice id
  int i, j, k ;

  if (m_NonUniformSliceSpacing == 1){
    // non-uniform slicing
    // calculate fraction of distance along axis for each slice
    for (i = 0, k = 0 ;  i < m_NumberOfNonUniformSections ;  i++){
      double sectionStart = (double)i / (double)m_NumberOfNonUniformSections ;    // fractional pos of section start
      double sectionEnd = (double)(i+1) / (double)m_NumberOfNonUniformSections ;  // fraction pos of section end
      double s = (sectionEnd - sectionStart) / (double)m_SlicesInSection[i] ;   // slice spacing for this section

      for (j = 0 ;  j < m_SlicesInSection[i] ;  j++)
        alpha[k++] = sectionStart + ((double)j + 0.5)*s ;
    }

    // error check that total number of slices is consistent
    assert(k == m_NumberOfScans) ;
  }
  else{
    // simple uniform slice spacing
    for (k = 0 ;  k < m_NumberOfScans ;  k++)
      alpha[k] = (double)k / (double)(m_NumberOfScans-1) ;
  }
}







//------------------------------------------------------------------------------
// Transfer setup parameters from input dialog and landmarks to model view
bool albaOpMML3::SetUpModelView()
//------------------------------------------------------------------------------
{
  // Check that at least 3 registration landmarks are defined
  assert(m_L1Defined && m_L2Defined && m_L3Defined) ;

  // Check that axis landmarks are defined
  assert(m_Axis1Defined && m_Axis2Defined) ;
  if (m_MuscleType == 2)
    assert(m_Axis3Defined) ;


  // set muscle type (1 - one slicing axis, 2 - two slicing axes)
  m_Model->SetTypeOfMuscles(m_MuscleType);

  // set patient landmarks
  m_Model->SetLandmark1OfPatient(m_P1Point);
  m_Model->SetLandmark2OfPatient(m_P2Point);
  m_Model->SetLandmark3OfPatient(m_P3Point);
  m_Model->SetLandmark4OfPatient(m_P4Point);

  // set axis landmarks
  m_Model->SetLandmark1OfAxis(m_Axis1Point_PatientCoords) ;
  m_Model->SetLandmark2OfAxis(m_Axis2Point_PatientCoords) ;
  if (m_MuscleType == 2)
    m_Model->SetLandmark3OfAxis(m_Axis3Point_PatientCoords) ;

  // landmark 4 flag
  m_Model->Set4LandmarksFlag(m_L4Defined);

  // set scans grain
  m_Model->SetGrainOfScans(m_ScansGrain);

  // set distribution of slices along axis
  double *alpha = new double[m_NumberOfScans] ;
  CalculateSlicePositionsAlongAxis(alpha) ;
  m_Model->SetFractionalPosOfScans(alpha) ;
  delete [] alpha ;

  // initialize the model view
  m_Model->Initialize() ;

  // initialize lut slider 
  double r[2] ;
  m_Volume->GetScalarRange(r) ;
  m_Lut->SetRange(r[0], r[1]);
  m_Lut->SetSubRange(r[0], r[1]);

  // select 2d or 3d view
  if (m_PreviewFlag == 1)
    m_Model->SetDisplayToPreview() ;
  else if (m_Flag3D == 0)
    m_Model->SetDisplay2D() ;
  else
    m_Model->SetDisplay3D() ;

  return true ;
}




//------------------------------------------------------------------------------
// Set up the parameter views and initialize spline functions
// The Model View must be initialized first.
// However, this changes the spline values, so you must update the model view afterwards.
// NB The splines for the contour centers have no display, and are initialized in the model view.
bool albaOpMML3::SetUpParameterViews() 
//------------------------------------------------------------------------------
{
  // size of synthetic scans
  double sx, sy;
  m_Model->GetSizeOfScans(&sx, &sy);

  // range of id and zeta
  int n = m_Model->GetTotalNumberOfScans() ;
  double zeta0 = m_Model->GetZetaOfSlice(0) ;
  double zeta1 = m_Model->GetZetaOfSlice(n-1) ;


  // ph spline
  m_PH->SetRangeX(n);
  m_PH->SetRangeY(-1.0 * (sx + sy)/ 3.0, 0.0, (sx + sy) / 3.0); // average x 2
  // pv spline																											   
  m_PV->SetRangeX(n);
  m_PV->SetRangeY(-1.0 * (sx + sy)/ 3.0, 0.0, (sx + sy) / 3.0); // average x 2
  // th spline
  m_TH->SetRangeX(n);
  m_TH->SetRangeY(-1.0 * (sx + sy)/ 3.0, 0.0, (sx + sy) / 3.0); // average x 2
  // tv spline
  m_TV->SetRangeX(n);
  m_TV->SetRangeY(-1.0 * (sx + sy)/ 3.0, 0.0, (sx + sy) / 3.0); // average x 2
  // ra spline
  m_RA->SetRangeX(n);
  m_RA->SetRangeY(-70.0, 0.0, 70.0);
  // sn spline
  m_SN->SetRangeX(n);
  m_SN->SetRangeY(0.1, 1.0, 3.0);
  // ss spline
  m_SS->SetRangeX(n);
  m_SS->SetRangeY(0.1, 1.0, 3.0);
  // se spline
  m_SE->SetRangeX(n);
  m_SE->SetRangeY(0.1, 1.0, 3.0);
  // sw spline
  m_SW->SetRangeX(n);
  m_SW->SetRangeY(0.1, 1.0, 3.0);



  // display values
  m_PH->AddPoint(0, 0.0); 
  m_PH->AddPoint(n-1, 0.0);
  // actual values
  m_Model->GetPHSpline()->AddPoint(zeta0, 0.0); // first slice, PH = 0
  m_Model->GetPHSpline()->AddPoint(zeta1, 0.0); // last slice, PH = 0

  // display values
  m_PV->AddPoint(0, 0.0); // first slice, PV = 0
  m_PV->AddPoint(n-1, 0.0); // last slice, PV = 0
  // actual values
  m_Model->GetPVSpline()->AddPoint(zeta0, 0.0); // first slice - PV = 0
  m_Model->GetPVSpline()->AddPoint(zeta1, 0.0); // last slice, PV = 0

  // display values
  m_TH->AddPoint(0, 0.0); // first slice, TH = 0
  m_TH->AddPoint(n-1, 0.0); // last slice, TH = 0
  // actual values
  m_Model->GetTHSpline()->AddPoint(zeta0, 0.0); // first slice - H = 0
  m_Model->GetTHSpline()->AddPoint(zeta1, 0.0); // last slice - H = 0

  // display values
  m_TV->AddPoint(0, 0.0); // first slice, V = 0
  m_TV->AddPoint(n-1, 0.0); // last slice, TV = 0
  // actual values
  m_Model->GetTVSpline()->AddPoint(zeta0, 0.0); // first slice - V = 0
  m_Model->GetTVSpline()->AddPoint(zeta1, 0.0); // last slice, TV = 0

  // display values
  m_RA->AddPoint(0, 0.0); // first slice, TA = 0
  m_RA->AddPoint(n-1, 0.0); // last slice, RA = 0
  // actual values
  m_Model->GetRASpline()->AddPoint(zeta0, 0.0); // first slice, TA = 0
  m_Model->GetRASpline()->AddPoint(zeta1, 0.0); // last slice, RA = 0

  // display values
  m_SN->AddPoint(0, 1.0); // first slice, SN = 1.0
  m_SN->AddPoint(n-1, 1.0); // last slice, SN = 1.0
  // actual values
  m_Model->GetSNSpline()->AddPoint(zeta0, 1.0); // first slice, SN = 1.0
  m_Model->GetSNSpline()->AddPoint(zeta1, 1.0); // last slice, SN = 1.0

  // display values
  m_SS->AddPoint(0, 1.0); // first slice, SS = 1.0
  m_SS->AddPoint(n-1, 1.0); // last slice, SS = 1.0
  // actual values
  m_Model->GetSSSpline()->AddPoint(zeta0, 1.0); // first slice, SS = 1.0
  m_Model->GetSSSpline()->AddPoint(zeta1, 1.0); // last slice, SS = 1.0

  // display values
  m_SE->AddPoint(0, 1.0); // first slice, SE = 1.0
  m_SE->AddPoint(n-1, 1.0); // last slice, SE = 1.0
  // actual values
  m_Model->GetSESpline()->AddPoint(zeta0, 1.0); // first slice, SE = 1.0
  m_Model->GetSESpline()->AddPoint(zeta1, 1.0); // last slice, SE = 1.0

  // display values
  m_SW->AddPoint(0, 1.0); // first slice, SW = 1.0
  m_SW->AddPoint(n-1, 1.0); // last slice, SW = 1.0
  // actual values
  m_Model->GetSWSpline()->AddPoint(zeta0, 1.0); // first slice, SW = 1.0
  m_Model->GetSWSpline()->AddPoint(zeta1, 1.0); // last slice, SW = 1.0


  // testing
  double scale = m_Model->GetSNSpline()->Evaluate((zeta0+zeta1)/2.0) ;

  // apply operations
  if (m_RegistrationStatus == 1)
  {

    int NumberOfOperations;
    NumberOfOperations = m_CurrentSliceIdStackTag->GetNumberOfComponents();

    int MaxScanId;
    MaxScanId = n-1;

    int NextOperationId;
    NextOperationId = m_Widget->GetNextOperationId();

    // get operations
    double params[5];
    for(int i = 0; i < NumberOfOperations; i++)
    {
      // exceeded max ?
      assert(NextOperationId < 2000);

      // values
      params[0] = m_CurrentSliceIdStackTag->GetComponentAsDouble(i);
      params[1] = m_ZValueStackTag->GetComponentAsDouble(i);
      params[2] = m_OperationTypeStackTag->GetComponentAsDouble(i);
      params[3] = m_Parameter1StackTag->GetComponentAsDouble(i);
      params[4] = m_Parameter2StackTag->GetComponentAsDouble(i);

      // built operations stack
      m_Model->SetOperationsStackTuple(NextOperationId, params);

      // add points in splines
      switch ((int) params[2])
      {
      case 0: // p
        /*if (params[0] == 0 || params[0] == MaxScanId) // first/last slice
        {
        PH->RemovePoint(params[0]);
        PV->RemovePoint(params[0]);
        Model->GetPHSpline()->RemovePoint(params[1]);
        Model->GetPVSpline()->RemovePoint(params[1]);
        }*/

        // display values
        m_PH->AddPoint(params[0], params[3]);
        m_PV->AddPoint(params[0], params[4]);
        // actual values
        m_Model->GetPHSpline()->AddPoint(params[1], params[3]);
        m_Model->GetPVSpline()->AddPoint(params[1], params[4]);

        break;

      case 1: // t
        // display values
        m_TH->AddPoint(params[0], params[3]);
        m_TV->AddPoint(params[0], params[4]);
        // actual values
        m_Model->GetTHSpline()->AddPoint(params[1], params[3]);
        m_Model->GetTVSpline()->AddPoint(params[1], params[4]);
        break;

      case 2: // r
        // display values
        m_RA->AddPoint(params[0], params[3]);
        // actual values
        m_Model->GetRASpline()->AddPoint(params[1], params[3]);
        break;

      case 3: // ns
        // display values
        m_SN->AddPoint(params[0], params[3]);
        // actual values
        m_Model->GetSNSpline()->AddPoint(params[1], params[3]);
        break;

      case 4: // ss
        // display values
        m_SS->AddPoint(params[0], params[3]);
        // actual values
        m_Model->GetSSSpline()->AddPoint(params[1], params[3]);
        break;

      case 5: // es
        // display values
        m_SE->AddPoint(params[0], params[3]);
        // actual values
        m_Model->GetSESpline()->AddPoint(params[1], params[3]);
        break;

      case 6: // ws
        // display values
        m_SW->AddPoint(params[0], params[3]);
        // actual values
        m_Model->GetSWSpline()->AddPoint(params[1], params[3]);
        break;
      }

      // is the operation a scaling?
      if (params[2] == 3 || params[2] == 4 || params[2] == 5 || params[2] == 6)
      {
        m_Model->SetScalingOccured(true) ;
        m_Model->SetScalingOccuredOperationId(NextOperationId) ;

        // grey out twist, h/v translation views

        // redraw curves
        m_PH->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
        m_PH->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
        m_PV->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
        m_PV->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
        m_TH->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
        m_TH->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
        m_TV->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
        m_TV->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
        m_RA->GetSplineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5); // grey
        m_RA->GetPointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
      }

      // new next op
      m_Widget->SetNextOperationId(NextOperationId + 1);
      NextOperationId++;
    }
  }

  return 1;
}




//------------------------------------------------------------------------------
// Set up the contour widget
// The model view should probably be set up first
bool albaOpMML3::SetUpContourWidget()
//------------------------------------------------------------------------------
{
  m_Widget->SetModel(m_Model);
  m_Widget->SetInteractor(m_Model->GetRenderWindowInteractor());

  m_Widget->SetPH(m_PH);
  m_Widget->SetPV(m_PV);
  m_Widget->SetTH(m_TH);
  m_Widget->SetTV(m_TV);
  m_Widget->SetRA(m_RA);
  m_Widget->SetSN(m_SN);
  m_Widget->SetSS(m_SS);
  m_Widget->SetSE(m_SE);
  m_Widget->SetSW(m_SW);

  m_Widget->SetResolution(20);
  m_Widget->GetPlaneProperty()->SetColor(1.0, 1.0, 1.0);
  m_Widget->GetPlaneProperty()->SetOpacity(0.001);
  m_Widget->GetSelectedPlaneProperty()->SetColor(1.0, 1.0, 1.0);
  m_Widget->GetSelectedPlaneProperty()->SetOpacity(0.001);

  if ((m_Flag3D == 1) || (m_PreviewFlag == 1))
    m_Widget->Off();
  else
    m_Widget->On();

  // update widget
  m_Widget->UpdateWidgetTransform();

  return true ;
}








//----------------------------------------------------------------------------
// Update model view and widget
void albaOpMML3::Update()
//----------------------------------------------------------------------------
{
  // update model
  m_Model->Update();

  // update widget
  m_Widget->UpdateWidgetTransform();
}



//----------------------------------------------------------------------------
// Create new render window for parameter view, including
// visual pipe for labelling.
// All the allocated objects will be deleted with the output albaRWI.
albaRWI* albaOpMML3::CreateParameterViewalbaRWI(vtkTextSource *ts, wxString lab, float r, float g, float b)
//----------------------------------------------------------------------------
{ 
  albaRWI *rwi = new albaRWI(m_OpDlg);
  rwi->SetListener(this);
  rwi->m_RwiBase->SetInteractorStyle(NULL);

  // width/height
  rwi->SetSize(0, 0, 250, 65);
  ((vtkRenderWindow*)(rwi->m_RenderWindow))->SetSize(250, 65);
  rwi->m_RwiBase->Show();

  //vtkDEL(rwi->m_C);
  //rwi->m_C = m_RenderWindow_camera;

  ts->SetForegroundColor(r,g,b);
  ts->BackingOff(); // no background drawn with text
  ts->SetText(lab.ToAscii());

  vtkPolyDataMapper2D *mapper = vtkPolyDataMapper2D::New();
  mapper->SetInputConnection(ts->GetOutputPort());

  vtkActor2D *labactor = vtkActor2D::New();
  labactor->SetMapper(mapper);

  rwi->m_RenFront->AddActor(labactor);

  // delete local copies of vtk objects
  mapper->Delete() ;
  labactor->Delete() ;

  return rwi;
}






//----------------------------------------------------------------------------
// Set up a specific set of landmarks
void albaOpMML3::SetUpLandmarks1(wxString AtlasSectionVMEName, wxString PatientSectionVMEName)
//----------------------------------------------------------------------------
{
  int i;

  // landmarks (names)
  wxString Landmark1VMEName;
  wxString Landmark2VMEName;
  wxString Landmark3VMEName;
  wxString Landmark4VMEName;

  // landmarks (coordinates)
  double a1[3];
  double a2[3];
  double a3[3];
  double a4[3];
  double p1[3];
  double p2[3];
  double p3[3];
  double p4[3];

  // weights used to relocate landmarks 1, 2 along
  // their carrier line in both, atlas and patient
  // NewLandmark1 = a * Landmark1 + (1 - a)*Landmark2;
  // NewLandmark2 = b * Landmark1 + (1 - b)*Landmark2;
  //
  // a = 1 ==> NewLandmark1 = Landmark1
  // a > 1 ==> the axis is prolonged (NewLandmark1 is moved away from Landmark2)
  // 0 < a < 1 the axis is shortened (NewLandmark1 is moved towards Landmark2)

  // b = 0 ==> NewLandmark2 = Landmark2
  // b < 0 ==> the axis is prolonged (NewLandmark2 is moved away from Landmark1)
  // 0 < b < 1 the axis is shortened (NewLandmark2 is moved towards Landmark1)

  // exact values in comments next to actual values used (these are slightly adjusted)
  double a, b;

  // starting from no landmarks
  m_L1Name      = "none";
  m_L2Name      = "none";
  m_L3Name      = "none";
  m_L4Name      = "none";
  m_P1Name      = "none";
  m_P2Name      = "none";
  m_P3Name      = "none";
  m_P4Name      = "none";

  // select strings
  if (m_SurfaceName.compare("obturator externus") == 0){
    // landmarks
    Landmark1VMEName = "Superior edge of lateral surface of the Right ischial ramus at juncture with inferior pubic ramus ";
    Landmark2VMEName = "bottom of inferior cavity in trochanteric fossa";
    Landmark3VMEName = "midpoint of ridge between lesser trochanter and femoral head";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.25; // 1.20
    b = 0.10; // 0.14
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("gamellus inferior") == 0){
    // landmarks
    Landmark1VMEName = "Most superior point on posterior surface of Right ischial tuberosity at height of lesser sciatic notch ";
    Landmark2VMEName = "bottom of superior cavity in trochanteric fossa";
    Landmark3VMEName = "Lateral surface of the Right ilium at midpoint between most lateral point on superior tubercle of iliac crest (pt. 39) and origin of Gluteus Medius posterior fibers (pt. 41).   ";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.15; // 1.07
    b = 0.20; // 0.28
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("gamellus superior") == 0){
    // landmarks
    Landmark1VMEName = "Most superior point on posterior surface of Right ischial tuberosity at height of lesser sciatic notch ";
    Landmark2VMEName = "bottom of superior cavity in trochanteric fossa";
    Landmark3VMEName = "Lateral surface of the Right ilium at midpoint between most lateral point on superior tubercle of iliac crest (pt. 39) and origin of Gluteus Medius posterior fibers (pt. 41).   ";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.65; // 1.60
    b = 0.25; // 0.32
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("piriformis") == 0){
    // landmarks
    Landmark1VMEName = "Anterior surface of the Right sacrum between second and third foramen midway between line of foramen and lateral border (first sacral foramen is most superior)";
    Landmark2VMEName = "most anterior superior point on greater trochanter";
    Landmark3VMEName = "Superior edge of lateral surface of the Right ischial ramus at juncture with inferior pubic ramus ";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.20; // 1.14
    b = 0.20; // 0.27
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("gluteus minimus") == 0){
    // landmarks
    Landmark1VMEName = "Top of ridge on lateral surface of the Right ilium between superior tubercle of iliac crest (pt. 39) and acetabulum (pt. 5) along a line from anterior superior iliac spine ASIS (pt. 1) to posterior superior iliac spine PSIS (pt. 3) ";
    Landmark2VMEName = "center of depression on anterior surface of greater trochanter";
    Landmark3VMEName = "Bottom of posterior depression on lateral surface of the Right ilium along a line from anterior superior iliac spine ASIS (pt. 1) to posterior superior iliac spine PSIS (pt. 3) ";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.30; // 1.25
    b = -0.05;// 0.00
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("tensor fasciae latae") == 0){
    // landmarks
    Landmark1VMEName = "Midpoint along Right lateral superior ridge of iliac crest between the anterior superior iliac spine ASIS and most lateral point on superior tubercle of the iliac crest ";
    Landmark2VMEName = "impression for iliotibial tract on lateral anterior surface of superior end of tibia at height of fibular facet just below epiphysial line";
    Landmark3VMEName = "Superior edge of lateral surface of the Right ischial ramus at juncture with inferior pubic ramus ";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.00; // 0.95
    b = 0.60; // 0.69
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("adductor longus") == 0){
    // landmarks
    Landmark1VMEName = "Anterior corner of inferior surface of the Right pubic tubercle ";
    Landmark2VMEName = "on linea aspera at midpoint between juncture of pectineal/gluteal lines and juncture of medial/lateral supracondylar lines";
    Landmark3VMEName = "anterior medial corner of femoral shaft cross-section directly superior to medial border of patellar groove joins at height of juncture of medial supracondylar line and adductor tubercle ridge ";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.00; // 0.92
    b = -0.20;// -0.16
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("adductor brevis") == 0){
    // landmarks
    Landmark1VMEName = "Midpoint of lateral surface of the Right inferior pubic ramus ";
    Landmark2VMEName = "on linea aspera at juncture of pectineal/gluteal lines";
    Landmark3VMEName = "most medial corner of femoral shaft cross-section at height of midpoint between juncture of pectineal/gluteal lines and juncture of medial/lateral supracondylar lines";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.15; // 1.09
    b = -0.20;// -0.14
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("rectus femoris") == 0){
    // landmarks
    Landmark1VMEName = "Right Anterior Inferior Iliac Spine ";
    Landmark2VMEName = "Midside point on superior patella surface";
    Landmark3VMEName = "Superior edge of lateral surface of the Right ischial ramus at juncture with inferior pubic ramus ";
    Landmark4VMEName = "most anterior point on anterior surface of femoral shaft at height of juncture of pectineal/gluteal lines";
    a = 1.05; // 0.97
    b = 0.15; // 0.19
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("semitendinous") == 0){
    // landmarks
    Landmark1VMEName = "Posterior most point on medial border of posterior surface of the Right ischial tuberosity";
    Landmark2VMEName = "medial corner near back of shaft inferior to base of tibial tubercle - near pes anserine bursa";
    Landmark3VMEName = "most medial point on medial epicondyle";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.05; // 0.98
    b = 0.25; // 0.32
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("gluteus maximus") == 0){
    // landmarks
    Landmark1VMEName = "Lateral corner of Right sacral-coccygeal facet  ";
    Landmark2VMEName = "on gluteal line at height of inferior base of lesser trochanter";
    Landmark3VMEName = "Right Posterior Superior Iliac Spine PSIS (repeat of pt. 3) ";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.25; // 1.18
    b = -0.20;// -0.15
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("gluteus medius") == 0){
    // landmarks
    Landmark1VMEName = "Lateral surface of the Right ilium at midpoint between most lateral point on superior tubercle of iliac crest (pt. 39) and origin of Gluteus Medius posterior fibers (pt. 41).   ";
    Landmark2VMEName = "lateral posterior prominence of greater trochanter";
    Landmark3VMEName = "Bottom of posterior depression on lateral surface of the Right ilium along a line from most lateral point on superior tubercle of iliac crest to posterior superior iliac spine PSIS  ";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.30; // 1.25
    b = -0.25;// -0.18
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("pectineus") == 0){
    // landmarks
    Landmark1VMEName = "Base of the Right superior pubic ramus on superior surface lateral to pectineal line ";
    Landmark2VMEName = "inferior base of lesser trochanter on pectineal line";
    Landmark3VMEName = "Superior edge of lateral surface of the Right ischial ramus at juncture with inferior pubic ramus ";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.10; // 1.05
    b = -0.15;// -0.09
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("adductor magnus") == 0){
    // landmarks
    Landmark1VMEName = "Most lateral aspect at posterior corner of lateral surface of Right ischial tuberosity ";
    Landmark2VMEName = "superior corner of adductor tubercle above medial epicondyle";
    Landmark3VMEName = "on linea aspera at midpoint between juncture of pectineal/gluteal lines and juncture of medial/lateral supracondylar lines";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.10; // 1.05
    b = 0.10; // 0.15
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("quadratus femoris") == 0){
    // landmarks
    Landmark1VMEName = "On lateral surface of Right ischial tuberosity at midpoint between obturator foramen and lateral posterior corner ";
    Landmark2VMEName = "quadrate tubercle on intertrochanteric crest";
    Landmark3VMEName = "Lateral surface of the Right ilium at midpoint between most lateral point on superior tubercle of iliac crest (pt. 39) and origin of Gluteus Medius posterior fibers (pt. 41).   ";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.05; // 1.00
    b = -0.20;// -0.14
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("sartorius") == 0){
    // landmarks
    Landmark1VMEName = "Right Anterior Superior Iliac Spine (repeat of pt. 1) ";
    Landmark2VMEName = "most medial point on medial epicondyle";
    Landmark3VMEName = "Superior edge of lateral surface of the Right ischial ramus at juncture with inferior pubic ramus ";
    Landmark4VMEName = "most anterior point on anterior surface of femoral shaft at height of juncture of pectineal/gluteal lines";
    a = 1.05; // 1.00
    b = -0.15;// -0.10
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("vastus medialis") == 0){
    // landmarks
    Landmark1VMEName = "most medial corner of femoral shaft cross-section at height of midpoint between juncture of pectineal/gluteal lines and juncture of medial/lateral supracondylar lines";
    Landmark2VMEName = "anterior medial corner of femoral shaft cross-section directly superior to medial border of patellar groove joins at height of juncture of medial supracondylar line and adductor tubercle ridge ";
    Landmark3VMEName = "Most medial point on superior patella surface";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.85; // 1.80
    b = -0.30;// -0.25
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("semimembranous") == 0){
    // landmarks
    Landmark1VMEName = "Bottom of superior depression on posterior surface of Right ischial tuberosity ";
    Landmark2VMEName = "superior lip of depression on posterior surface of medial tibial condyle";
    Landmark3VMEName = "superior medial corner of medial condyle";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 0.80; // 0.74
    b = 0.00; // 0.06
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("obturator internal") == 0){
    // DAVID P. 19-01-2005: This is not the permanent solution.
    // landmarks
    // old solution
    //Landmark1VMEName = "Most superior point on posterior surface of Right ischial tuberosity at height of lesser sciatic notch ";
    //Landmark2VMEName = "bottom of superior cavity in trochanteric fossa";
    //Landmark3VMEName = "Lateral surface of the Right ilium at midpoint between most lateral point on superior tubercle of iliac crest (pt. 39) and origin of Gluteus Medius posterior fibers (pt. 41).   ";

    // re-shuffled solution (according to david's email)
    Landmark2VMEName = "Most superior point on posterior surface of Right ischial tuberosity at height of lesser sciatic notch ";
    Landmark3VMEName = "bottom of superior cavity in trochanteric fossa";
    Landmark1VMEName = "Lateral surface of the Right ilium at midpoint between most lateral point on superior tubercle of iliac crest (pt. 39) and origin of Gluteus Medius posterior fibers (pt. 41).   ";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.75; // 1.70
    b = 0.50; // 0.58
    m_MuscleType = 2;
  }
  else if (m_SurfaceName.compare("long head of the biceps") == 0){
    // landmarks
    Landmark1VMEName = "Posterior most point on medial border of posterior surface of the Right ischial tuberosity";
    Landmark2VMEName = "superior most point on fibula (apex, styloid process)";
    Landmark3VMEName = "superior medial corner of medial condyle";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.00; // 0.94
    b = 0.15; // 0.20
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("short head of the biceps") == 0){
    // landmarks
    Landmark1VMEName = "on linea aspera at midpoint between juncture of pectineal/gluteal lines and juncture of medial/lateral supracondylar lines";
    Landmark2VMEName = "superior most point on fibula (apex, styloid process)";
    Landmark3VMEName = "superior medial corner of medial condyle";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.10; // 1.03
    b = 0.00; // 0.07
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("gracilis") == 0){
    // landmarks
    Landmark1VMEName = "Inferior margin of lateral surface of right inferior pubic ramus ";
    Landmark2VMEName = "medial corner near back of shaft inferior to base of tibial tubercle - near pes anserine bursa";
    Landmark3VMEName = "on linea aspera at juncture of pectineal/gluteal lines";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.05; // 0.97
    b = 0.25; // 0.30
    m_MuscleType = 1;
  }
  else if (m_SurfaceName.compare("vastus lateralis and intermedius") == 0){
    // landmarks
    Landmark1VMEName = "most anterior point on anterior surface of femoral shaft at height of juncture of pectineal/gluteal lines";
    Landmark2VMEName = "Midside point on superior patella surface";
    Landmark3VMEName = "Superior edge of lateral surface of the Right ischial ramus at juncture with inferior pubic ramus ";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.50; // 1.44
    b = -0.05;// 0.00
    m_MuscleType = 1;
  }
  else{
    wxMessageBox("Muscle not in built in dictionary!","alert",wxICON_WARNING);
    return;
  }

  // get root node
  albaVME *RootVME = m_Input->GetRoot();

  // get atlas section node
  albaVME *AtlasSectionVME = RootVME->FindInTreeByName(AtlasSectionVMEName);
  if(AtlasSectionVME == NULL)
  {
    wxMessageBox("No section" + AtlasSectionVMEName,"alert",wxICON_WARNING);
    return;
  }

  // get patient section node
  albaVME *PatientSectionVME = RootVME->FindInTreeByName(PatientSectionVMEName);
  if(PatientSectionVME == NULL)
  {
    wxMessageBox("No section" + PatientSectionVMEName,"alert",wxICON_WARNING);
    return;
  }

  // atlas - landmark 1
  // get landmark node
  albaVME *Landmark1AtlasVME = AtlasSectionVME->FindInTreeByName(Landmark1VMEName);
  if(Landmark1AtlasVME == NULL)
  {
    wxMessageBox("Expected " + Landmark1VMEName + " in section " + AtlasSectionVMEName + " is missing!","alert",wxICON_WARNING);
  }
  else
  {
    // get landmark
    albaVMELandmark *Landmark1AtlasVMELandmark = albaVMELandmark::SafeDownCast(Landmark1AtlasVME);

    // get coordinates
    GetAbsPosOfLandmark(Landmark1AtlasVMELandmark, a1) ;

    // set name
    m_L1Name = Landmark1AtlasVMELandmark->GetName();
    m_L1Defined = true ;
  }

  // atlas - landmark 2
  // get landmark node
  albaVME *Landmark2AtlasVME = AtlasSectionVME->FindInTreeByName(Landmark2VMEName);
  if(Landmark2AtlasVME == NULL)
  {
    wxMessageBox("Expected " + Landmark2VMEName + " in section " + AtlasSectionVMEName + " is missing!","alert",wxICON_WARNING);
  }
  else
  {
    // get landmark
    albaVMELandmark *Landmark2AtlasVMELandmark = albaVMELandmark::SafeDownCast(Landmark2AtlasVME);

    // get coordinates
    GetAbsPosOfLandmark(Landmark2AtlasVMELandmark, a2) ;

    // set name
    m_L2Name = Landmark2AtlasVMELandmark->GetName();
    m_L2Defined = true ;

  }

  // atlas - landmark 3
  // get landmark node
  albaVME *Landmark3AtlasVME = AtlasSectionVME->FindInTreeByName(Landmark3VMEName);
  if(Landmark3AtlasVME == NULL)
  {
    wxMessageBox("Expected " + Landmark3VMEName + " in section " + AtlasSectionVMEName + " is missing!","alert",wxICON_WARNING);
  }
  else
  {
    // get landmark
    albaVMELandmark *Landmark3AtlasVMELandmark = albaVMELandmark::SafeDownCast(Landmark3AtlasVME);

    // get coordinates
    GetAbsPosOfLandmark(Landmark3AtlasVMELandmark, a3) ;

    // set name
    m_L3Name = Landmark3AtlasVMELandmark->GetName();
    m_L3Defined = true ;
  }

  // atlas - landmark 4
  // get landmark node
  albaVME *Landmark4AtlasVME = AtlasSectionVME->FindInTreeByName(Landmark4VMEName);
  if(Landmark4AtlasVME == NULL)
  {
    wxMessageBox("Expected " + Landmark4VMEName + " in section " + AtlasSectionVMEName + " is missing!","alert",wxICON_WARNING);
  }
  else
  {
    // get landmark
    albaVMELandmark *Landmark4AtlasVMELandmark = albaVMELandmark::SafeDownCast(Landmark4AtlasVME);

    // get coordinates
    Landmark4AtlasVMELandmark->GetPoint(a4);

    // set name
    m_L4Name = Landmark4AtlasVMELandmark->GetName();
    m_L4Defined = true ;
  }

  // patient - landmark 1
  // get landmark node
  albaVME *Landmark1PatientVME = PatientSectionVME->FindInTreeByName(Landmark1VMEName);
  if(Landmark1PatientVME == NULL)
  {
    wxMessageBox("Expected " + Landmark1VMEName + " in section " + PatientSectionVMEName + " is missing!","alert",wxICON_WARNING);
  }
  else
  {
    // get landmark
    albaVMELandmark *Landmark1PatientVMELandmark = albaVMELandmark::SafeDownCast(Landmark1PatientVME);

    // get coordinates
    Landmark1PatientVMELandmark->GetPoint(p1);

    // set name
    m_P1Name = Landmark1PatientVMELandmark->GetName();
  }

  // patient - landmark 2
  // get landmark node
  albaVME *Landmark2PatientVME = PatientSectionVME->FindInTreeByName(Landmark2VMEName);
  if(Landmark2PatientVME == NULL)
  {
    wxMessageBox("Expected " + Landmark2VMEName + " in section " + PatientSectionVMEName + " is missing!","alert",wxICON_WARNING);
  }
  else
  {
    // get landmark
    albaVMELandmark *Landmark2PatientVMELandmark = albaVMELandmark::SafeDownCast(Landmark2PatientVME);

    // get coordinates
    Landmark2PatientVMELandmark->GetPoint(p2);

    // set name
    m_P2Name = Landmark2PatientVMELandmark->GetName();
  }

  // patient - landmark 3
  // get landmark node
  albaVME *Landmark3PatientVME = PatientSectionVME->FindInTreeByName(Landmark3VMEName);
  if(Landmark3PatientVME == NULL)
  {
    wxMessageBox("Expected " + Landmark3VMEName + " in section " + PatientSectionVMEName + " is missing!","alert",wxICON_WARNING);
  }
  else
  {
    // get landmark
    albaVMELandmark *Landmark3PatientVMELandmark = albaVMELandmark::SafeDownCast(Landmark3PatientVME);

    // get coordinates
    Landmark3PatientVMELandmark->GetPoint(p3);

    // set name
    m_P3Name = Landmark3PatientVMELandmark->GetName();
  }

  // patient - landmark 4
  // get landmark node
  albaVME *Landmark4PatientVME = PatientSectionVME->FindInTreeByName(Landmark4VMEName);
  if(Landmark4PatientVME == NULL)
  {
    wxMessageBox("Expected " + Landmark4VMEName + " in section " + PatientSectionVMEName + " is missing!","alert",wxICON_WARNING);
  }
  else
  {
    // get landmark
    albaVMELandmark *Landmark4PatientVMELandmark = albaVMELandmark::SafeDownCast(Landmark4PatientVME);

    // get coordinates
    Landmark4PatientVMELandmark->GetPoint(p4);

    // set name
    m_P4Name = Landmark4PatientVMELandmark->GetName();
  }

  // use weights to relocate landmarks 1, 2
  for(i = 0; i < 3; i++)
  {
    m_L1Point[i] = a * a1[i] + (1 - a) * a2[i];
    m_L2Point[i] = b * a1[i] + (1 - b) * a2[i];

    m_P1Point[i] = a * p1[i] + (1 - a) * p2[i];
    m_P2Point[i] = b * p1[i] + (1 - b) * p2[i];
  }

  // landmarks 3 / 4
  for(i = 0; i < 3; i++)
  {
    m_L3Point[i] = a3[i];
    m_L4Point[i] = a4[i];

    m_P3Point[i] = p3[i];
    m_P4Point[i] = p4[i];
  }
}




//----------------------------------------------------------------------------
// Set up a specific set of landmarks
void albaOpMML3::SetUpLandmarks2(wxString AtlasSectionVMEName, wxString PatientSectionVMEName)
//----------------------------------------------------------------------------
{
  // landmarks (names)
  wxString Landmark1VMEName = "L1" ;
  wxString Landmark2VMEName = "L2" ;
  wxString Landmark3VMEName = "L3" ;

  // names of atlas landmarks
  m_L1Name = "none";
  m_L2Name = "none";
  m_L3Name = "none";
  m_L4Name = "none";

  // get root node
  albaVME *RootVME = m_Input->GetRoot();

  // get atlas section node
  albaVME *AtlasSectionVME = RootVME->FindInTreeByName(AtlasSectionVMEName);
  if(AtlasSectionVME == NULL)
  {
    wxMessageBox("Can't find atlas section" + AtlasSectionVMEName,"alert",wxICON_WARNING);
    return;
  }

  // get patient section node
  albaVME *PatientSectionVME = RootVME->FindInTreeByName(PatientSectionVMEName);
  if(PatientSectionVME == NULL)
  {
    wxMessageBox("Can't find patient section" + PatientSectionVMEName,"alert",wxICON_WARNING);
    return;
  }


  // atlas - landmark 1
  // get landmark node
  albaVME *Landmark1AtlasVME = AtlasSectionVME->FindInTreeByName(Landmark1VMEName);
  if(Landmark1AtlasVME == NULL){
    wxMessageBox("Expected " + Landmark1VMEName + " in section " + AtlasSectionVMEName + " is missing!","alert",wxICON_WARNING);
  }
  else{
    // get landmark
    albaVMELandmark *Landmark1AtlasVMELandmark = albaVMELandmark::SafeDownCast(Landmark1AtlasVME);
    GetAbsPosOfLandmark(Landmark1AtlasVMELandmark, m_L1Point) ;
    m_L1Name = Landmark1AtlasVMELandmark->GetName();
    m_L1Defined = true ;
  }

  // atlas - landmark 2
  // get landmark node
  albaVME *Landmark2AtlasVME = AtlasSectionVME->FindInTreeByName(Landmark2VMEName);
  if(Landmark2AtlasVME == NULL){
    wxMessageBox("Expected " + Landmark2VMEName + " in section " + AtlasSectionVMEName + " is missing!","alert",wxICON_WARNING);
  }
  else{
    // get landmark
    albaVMELandmark *Landmark2AtlasVMELandmark = albaVMELandmark::SafeDownCast(Landmark2AtlasVME);
    GetAbsPosOfLandmark(Landmark2AtlasVMELandmark, m_L2Point) ;
    m_L2Name = Landmark2AtlasVMELandmark->GetName();
    m_L2Defined = true ;
  }

  // atlas - landmark 3
  // get landmark node
	albaVME *Landmark3AtlasVME = AtlasSectionVME->FindInTreeByName(Landmark3VMEName);
  if(Landmark3AtlasVME == NULL){
    wxMessageBox("Expected " + Landmark3VMEName + " in section " + AtlasSectionVMEName + " is missing!","alert",wxICON_WARNING);
  }
  else{
    // get landmark
    albaVMELandmark *Landmark3AtlasVMELandmark = albaVMELandmark::SafeDownCast(Landmark3AtlasVME);
    GetAbsPosOfLandmark(Landmark3AtlasVMELandmark, m_L3Point) ;
    m_L3Name = Landmark3AtlasVMELandmark->GetName();
    m_L3Defined = true ;
  }


  // patient - landmark 1
  // get landmark node
  albaVME *Landmark1PatientVME = PatientSectionVME->FindInTreeByName(Landmark1VMEName);
  if(Landmark1PatientVME == NULL){
    wxMessageBox("Expected " + Landmark1VMEName + " in section " + PatientSectionVMEName + " is missing!","alert",wxICON_WARNING);
  }
  else{
    // get landmark
    albaVMELandmark *Landmark1PatientVMELandmark = albaVMELandmark::SafeDownCast(Landmark1PatientVME);
    Landmark1PatientVMELandmark->GetPoint(m_P1Point);
    m_P1Name = Landmark1PatientVMELandmark->GetName();
  }

  // patient - landmark 2
  // get landmark node
  albaVME *Landmark2PatientVME = PatientSectionVME->FindInTreeByName(Landmark2VMEName);
  if(Landmark2PatientVME == NULL){
    wxMessageBox("Expected " + Landmark2VMEName + " in section " + PatientSectionVMEName + " is missing!","alert",wxICON_WARNING);
  }
  else{
    // get landmark
    albaVMELandmark *Landmark2PatientVMELandmark = albaVMELandmark::SafeDownCast(Landmark2PatientVME);
    Landmark2PatientVMELandmark->GetPoint(m_P2Point);
    m_P2Name = Landmark2PatientVMELandmark->GetName();
  }

  // patient - landmark 3
  // get landmark node
  albaVME *Landmark3PatientVME = PatientSectionVME->FindInTreeByName(Landmark3VMEName);
  if(Landmark3PatientVME == NULL){
    wxMessageBox("Expected " + Landmark3VMEName + " in section " + PatientSectionVMEName + " is missing!","alert",wxICON_WARNING);
  }
  else{
    // get landmark
    albaVMELandmark *Landmark3PatientVMELandmark = albaVMELandmark::SafeDownCast(Landmark3PatientVME);
    Landmark3PatientVMELandmark->GetPoint(m_P3Point);
    m_P3Name = Landmark3PatientVMELandmark->GetName();
  }
}








//----------------------------------------------------------------------------
void albaOpMML3::OnPOperationButton()
//----------------------------------------------------------------------------
{
  // render parameter views
  m_PH->Render();
  m_PV->Render();
  m_TH->Render();
  m_TV->Render();
  m_RA->Render();
  m_SN->Render();
  m_SS->Render();
  m_SE->Render();
  m_SW->Render();

  // render model view
  m_Model->Render();

  wxColour Colour = m_PlaceOpButton->GetBackgroundColour();
  wxColour Green = wxColour(0, 255, 0);

  // p operation active already
  if (Colour == Green)
    return;

  // not allowed if scaling has happened
  if (m_Model->GetScalingOccured())
  {
    wxMessageBox("Operation Unavailable (Scaling Occured)","alert",wxICON_WARNING);
    return;
  }

  // other buttons off
  m_TranslateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
  m_TranslateOpButton->SetLabel("T");
  m_RotateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
  m_RotateOpButton->SetLabel("R");
  m_ScaleOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
  m_ScaleOpButton->SetLabel("S");

  // this button on
  m_PlaceOpButton->SetBackgroundColour(Green);
  m_PlaceOpButton->SetLabel(">>P<<");

  //
  ResetOperation();

  // prepare display information
  m_Model->GetVisualPipe2D()->SetTextXPosition(0.0, 0.9) ;
  m_Model->GetVisualPipe2D()->SetTextYPosition(0.0, 0.8) ;

  m_Widget->UpdateWidgetTransform() ;
  m_Widget->PlaceModeOn();

  //
  m_Model->Render();
}




//----------------------------------------------------------------------------
void albaOpMML3::OnTOperationButton()
//----------------------------------------------------------------------------
{
  // render parameter views
  m_PH->Render();
  m_PV->Render();
  m_TH->Render();
  m_TV->Render();
  m_RA->Render();
  m_SN->Render();
  m_SS->Render();
  m_SE->Render();
  m_SW->Render();

  // render model view
  m_Model->Render();

  wxColour Colour = m_TranslateOpButton->GetBackgroundColour();
  wxColour Green = wxColour(0, 255, 0);

  // t operation active already
  if (Colour == Green)
    return;

  // not allowed if scaling has happened
  if (m_Model->GetScalingOccured())
  {
    wxMessageBox("Operation Unavailable (Scaling Occured)","alert",wxICON_WARNING);
    return;
  }

  // other buttons off
  m_PlaceOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
  m_PlaceOpButton->SetLabel("P");
  m_RotateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
  m_RotateOpButton->SetLabel("R");
  m_ScaleOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
  m_ScaleOpButton->SetLabel("S");

  // this button on
  m_TranslateOpButton->SetBackgroundColour(Green);
  m_TranslateOpButton->SetLabel(">>T<<");

  //
  ResetOperation();

  // prepare display information
  m_Model->GetVisualPipe2D()->SetTextXPosition(0.0, 0.9) ;
  m_Model->GetVisualPipe2D()->SetTextYPosition(0.0, 0.8) ;

  m_Widget->UpdateWidgetTransform() ;
  m_Widget->TranslationModeOn();

  //
  m_Model->Render();
}




//----------------------------------------------------------------------------
void albaOpMML3::OnROperationButton()
//----------------------------------------------------------------------------
{
  // render parameter views
  m_PH->Render();
  m_PV->Render();
  m_TH->Render();
  m_TV->Render();
  m_RA->Render();
  m_SN->Render();
  m_SS->Render();
  m_SE->Render();
  m_SW->Render();

  // render model view
  m_Model->Render();

  wxColour Colour = m_RotateOpButton->GetBackgroundColour();
  wxColour Green = wxColour(0, 255, 0);

  // r operation active already
  if (Colour == Green)
    return;

  // not allowed if scaling has happened
  if (m_Model->GetScalingOccured())
  {
    wxMessageBox("Operation Unavailable (Scaling Occured)","alert",wxICON_WARNING);
    return;
  }

  // other buttons off
  m_PlaceOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
  m_PlaceOpButton->SetLabel("P");
  m_TranslateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
  m_TranslateOpButton->SetLabel("T");
  m_ScaleOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
  m_ScaleOpButton->SetLabel("S");

  // this button on
  m_RotateOpButton->SetBackgroundColour(Green);
  m_RotateOpButton->SetLabel(">>R<<");

  //
  ResetOperation();

  // prepare display information
  m_Model->GetVisualPipe2D()->SetTextYPosition(0.0, 0.9) ;

  m_Widget->UpdateRotationHandle();
  m_Widget->RotationHandleOn();
  m_Widget->SetRotationHandleVisibility();
  m_Widget->RotationModeOn();

  //
  m_Model->Render();
}




//----------------------------------------------------------------------------
void albaOpMML3::OnSOperationButton()
//----------------------------------------------------------------------------
{
  // render parameter views
  m_PH->Render();
  m_PV->Render();
  m_TH->Render();
  m_TV->Render();
  m_RA->Render();
  m_SN->Render();
  m_SS->Render();
  m_SE->Render();
  m_SW->Render();

  // render model view
  m_Model->Render();

  wxColour Colour = m_ScaleOpButton->GetBackgroundColour();
  wxColour Green = wxColour(0, 255, 0);

  // s operation active already
  if (Colour == Green)
    return;

  // other buttons off
  m_PlaceOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
  m_PlaceOpButton->SetLabel("P");
  m_TranslateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
  m_TranslateOpButton->SetLabel("T");
  m_RotateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
  m_RotateOpButton->SetLabel("R");

  // this button on
  m_ScaleOpButton->SetBackgroundColour(Green);
  m_ScaleOpButton->SetLabel(">>S<<");

  m_Model->GetVisualPipe2D()->SetTextYPosition(0.0, 0.9) ;

  ResetOperation();

  m_Widget->UpdateScalingHandles();
  m_Widget->ScalingHandlesOn();
  m_Widget->ScalingModeOn();

  // on successful scaling, ScalingOccured flag in Model
  // is set to true, albaOpMML3ContourWidget::OnLeftButtonUp


  m_Model->Render();
}





//----------------------------------------------------------------------------
// Apply global registration using landmarks
// Creates patient space versions of muscle polydata and landmarks
void albaOpMML3::ApplyGlobalRegistration()
//----------------------------------------------------------------------------
{
  albaOpMML3GlobalRegistration *glob = new albaOpMML3GlobalRegistration() ;

  // If landmarks exist, use them set up the global registration tool.
  // If there are not enough landmarks, the transformation will default to the identity.
  if (m_L1Defined && m_L2Defined && m_L3Defined){
    glob->SetLandmark1OfAtlas(m_L1Point) ;
    glob->SetLandmark2OfAtlas(m_L2Point) ;
    glob->SetLandmark3OfAtlas(m_L3Point) ;
    glob->SetLandmark1OfPatient(m_P1Point) ;
    glob->SetLandmark2OfPatient(m_P2Point) ;
    glob->SetLandmark3OfPatient(m_P3Point) ;

    if (m_L4Defined){
      glob->SetLandmark4OfAtlas(m_L4Point) ;
      glob->SetLandmark4OfPatient(m_P4Point) ;
    }

    glob->Update() ;
  }


  // transform muscle polydata to patient space
  vtkPolyData *muscleGR = glob->TransformPolydata(m_Muscle) ;
  m_MuscleGlobalReg->DeepCopy(muscleGR) ;


  // copy or transform axis landmarks to patient coords
  switch(m_AxisLandmarksFlag){
    case 1:
      // axis landmarks are already in patient coords - just copy them
      for (int i = 0 ;  i < 3 ;  i++){
        m_Axis1Point_PatientCoords[i] = m_Axis1Point[i] ;
        m_Axis2Point_PatientCoords[i] = m_Axis2Point[i] ;
      }
      break ;

    case 2:
      // axis landmarks are in atlas coords, so they must be transformed
      glob->TransformPoint(m_Axis1Point, m_Axis1Point_PatientCoords) ;
      glob->TransformPoint(m_Axis2Point, m_Axis2Point_PatientCoords) ;

      if (m_Axis3Defined)
        glob->TransformPoint(m_Axis3Point, m_Axis3Point_PatientCoords) ;
 
      break ;

    default:
      // unknown axis landmarks flag
      assert(false) ;
  }


  //----------------------------------------------------------------------------
  // Copy the result to the MML output muscle
  //----------------------------------------------------------------------------
  m_MuscleOutput->DeepCopy(m_MuscleGlobalReg) ;


  //----------------------------------------------------------------------------
  // clean up
  //----------------------------------------------------------------------------
  delete glob ;
}





