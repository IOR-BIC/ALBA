/*=========================================================================

 Program: MAF2
 Module: mafOpMML
 Authors: Mel Krokos
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// CreateRegistrationDlg

#define TestingVersion

#include "mafDefines.h"

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
#include "vtkTransformFilter.h"

#include "mafOp.h"
#include "mafOpMML.h"
#include "mafGUIDialog.h"
#include "mafGUIButton.h"
#include "mafRWI.h"
#include "mafRWIBase.h"
#include "mafGUIValidator.h"
#include "mafGUILutSlider.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurface.h"
#include "mafVMEGroup.h"
#include "mafGUICheckListBox.h"
#include "mafOpMMLParameterView.h"
#include "mafOpMMLModelView.h"
#include "mafOpMMLContourWidget.h"
#include "mafVMELandmark.h"
#include "mafTagArray.h"
#include "mafTagItem.h"


//----------------------------------------------------------------------------
enum 
{
  ID = MINID,

  // set up dlg events
  ID_CHOOSE_SURFACE,
  /*
  ID_CHOOSE_L1,
  ID_CHOOSE_L2,
  ID_CHOOSE_L3,
  ID_CHOOSE_L4,
  */
  ID_CHOOSE_FAKE,
  ID_CHOOSE_OK,
  ID_CHOOSE_CANCEL,

  // registration dlg events
  ID_SHOW_AXES, 
  ID_SHOW_CONTOUR,
  ID_RESET_VIEW,
  ID_OK,
  ID_CANCEL,
  ID_P_OPERATION,
  ID_T_OPERATION,
  ID_R_OPERATION,
  ID_S_OPERATION,
  ID_UNDO,
  ID_SLICE,

  //
  ID_STATE,
  ID_CHOOSE_SCANS_DISTANCE,
  //ID_FAKE,
};

//----------------------------------------------------------------------------
mafOpMML::mafOpMML(const wxString &label) : mafOp(label)
//----------------------------------------------------------------------------
{
  //
  m_OpType = OPTYPE_OP;
  m_Canundo = false;

  m_Vol         = NULL;
  m_Surface     = NULL;

  m_OpDlg      = NULL; // registration dialog
  m_Radio       = NULL; // radio buttons

  //
  m_AxesOnOffButton = NULL;
  m_ContourOnOffButton = NULL;
  m_ResetViewButton = NULL;
  m_UndoButton = NULL;
  m_OkButton = NULL;
  m_CancelButton = NULL;

  // p/t/r/s operation buttons
  m_PlaceOpButton = NULL;
  m_TranslateOpButton = NULL;
  m_RotateOpButton = NULL;
  m_ScaleOpButton = NULL;

  m_Slice = 1; // start up slice
  m_State = 0; // start up operation (place)
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

  // no 3d view
  m_Flag3D = 0;

  // no 4th landmark
  m_LandmarksFlag = 0;

  // unregistered
  m_RegistrationStatus = 0;

  //
  m_RegisteredTransform = vtkMatrix4x4::New();

  //
  //m_ScansDistance = 30.0;
  m_ScansGrain = 2;
  m_ScansNumber = 12;
  //m_muscle_type = 1;
  m_RegistrationXYScalingFactor = 1.0;
  m_ScansSize[0] = 90;
  m_ScansSize[1] = 90;
  m_ScansResolution[0] = 2 * m_ScansSize[0];
  m_ScansResolution[1] = 2 * m_ScansSize[1];

  // Model maf RWIs
  m_ModelmafRWI = NULL;

  // parameter maf RWIs
  m_PHmafRWI = NULL;
  m_PVmafRWI = NULL;
  m_THmafRWI = NULL;
  m_TVmafRWI = NULL;
  m_RAmafRWI = NULL;
  m_SNmafRWI = NULL;
  m_SSmafRWI = NULL;
  m_SEmafRWI = NULL;
  m_SWmafRWI = NULL;

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
  m_ChooseOk = NULL;

  // Stefano
  m_MuscleType = 1;
}

//----------------------------------------------------------------------------
mafOpMML::~mafOpMML( ) 
{
  vtkDEL(m_RegisteredTransform);
}

//----------------------------------------------------------------------------
bool mafOpMML::Accept(mafNode* vme)
{ 
  return vme != NULL && vme->IsMAFType(mafVMEVolumeGray);
}

//----------------------------------------------------------------------------
mafOp* mafOpMML::Copy() {return new mafOpMML(m_Label);}

//----------------------------------------------------------------------------
void mafOpMML::OpDo()  {}

//----------------------------------------------------------------------------
void mafOpMML::OpUndo() {}

//----------------------------------------------------------------------------
void mafOpMML::OpStop(int result) {	mafEventMacro(mafEvent(this,result));}

//----------------------------------------------------------------------------
void mafOpMML::OpRun()   
//----------------------------------------------------------------------------
{
  //inputs dialog
  if(!CreateInputsDlg())    
  {
    // terminate operation terminates and start all over
    OpStop(OP_RUN_CANCEL);
    return;
  }

  // registration dialog
  CreateRegistrationDlg(); 

  //
  int res = (m_OpDlg->GetReturnCode() == wxID_OK) ? OP_RUN_OK : OP_RUN_CANCEL;

  //wxDELETE(m_RwiBase);
  //for(int i=0; i<9; i++) wxDELETE(m_RenderWindow[i]);
  //vtkDEL(m_rw_camera);
  wxDELETE(m_OpDlg);

  OpStop(res);
}


//----------------------------------------------------------------------------
void mafOpMML::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event)){
    switch(e->GetId())
    {
    case ID_CHOOSE_SURFACE: // set up dlg muscle selection
      //
      OnMuscleSelection();
      break;
      /*

      case ID_CHOOSE_L1: // set up dlg L1
      //
      OnLandmark1AtlasPatientSelection();
      break;

      case ID_CHOOSE_L2: // set up dlg L2
      //
      OnLandmark2AtlasPatientSelection();
      break;

      case ID_CHOOSE_L3: // set up dlg L3
      //
      OnLandmark3AtlasPatientSelection();
      break;

      case ID_CHOOSE_L4: // set up dlg L4
      //
      OnLandmark4AtlasPatientSelection();
      break;
      */

    case ID_CHOOSE_OK: // set up dlg ok
      m_ChooseDlg->EndModal(wxID_OK);
      break;

    case ID_CHOOSE_CANCEL: // set up dlg cancel
      m_ChooseDlg->EndModal(wxID_CANCEL);
      break;

    case ID_SHOW_AXES: // registration dlg axes on/off
      if (m_Flag3D == 0)
        OnContourLineAxesVisibility();
      break;

    case ID_SHOW_CONTOUR: // registration dlg contour on/off
      if (m_Flag3D == 0)
        OnContourVisibility();
      break;

    case ID_RESET_VIEW: // registration dlg reset view
      if (m_Flag3D == 0)
        OnResetView();
      break;

    case ID_OK: // registration dlg ok
      if (m_Flag3D == 0)
        OnRegistrationOK();
      m_OpDlg->EndModal(wxID_OK);
      break;

    case ID_CANCEL: // registration dlg cancel
      OnRegistrationCANCEL();
      m_OpDlg->EndModal(wxID_CANCEL);
      break;

    case ID_P_OPERATION: // registration dlg p-op
      if (m_Flag3D == 0)
        OnPOperationButton();
      break;

    case ID_T_OPERATION: // registration dlg t-op
      if (m_Flag3D == 0)
        OnTOperationButton();
      break;

    case ID_R_OPERATION: // registration dlg r-op
      if (m_Flag3D == 0)
        OnROperationButton();
      break;

    case ID_S_OPERATION: // registration dlg s-op
      if (m_Flag3D == 0)
        OnSOperationButton();
      break;

    case ID_UNDO: // registration dlg undo
      if (m_Flag3D == 0)
        OnUndo();
      break;

    case ID_RANGE_MODIFIED: // registration dlg lut
      //
      OnLut();
      break;

    case ID_SLICE: // registration dlg slice slider
      //
      OnSlider();
      break;

    case ID_STATE:
      //
      OnOperation();
      break;

    case ID_CHOOSE_SCANS_DISTANCE:
      //wxLogMessage("ttt");
      //wxLogMessage("arg1 changed to %d", m_arg1)
      break;

    default:
      mafEventMacro(*e); 
      break;
    }
  }
}



//----------------------------------------------------------------------------
void mafOpMML::OnRegistrationOK() 
//----------------------------------------------------------------------------
{
  // deform
  ApplyRegistrationOps();

  // inverse m_pfinalm matrix
  vtkMatrix4x4 *inversem_pfinalm= vtkMatrix4x4::New();
  inversem_pfinalm->Identity();
  inversem_pfinalm->Invert(m_Model->GetFinalM(), inversem_pfinalm);

  // transform
  vtkTransform *deformedt = vtkTransform::New();
  deformedt->SetMatrix(inversem_pfinalm);

  // muscle: deformed version
  vtkTransformPolyDataFilter *deformedpd = vtkTransformPolyDataFilter::New();
  deformedpd->SetInput(m_Model->GetMuscleTransform2PolyDataFilter()->GetOutput());
  deformedpd->SetTransform(deformedt);
  deformedpd->Update();

  // vme
  mafSmartPointer<mafVMESurface> vme ;

  // vme item
  vme->SetData(deformedpd->GetOutput(), ((mafVME *)m_Input)->GetTimeStamp()) ;


  // tag 1: status
  mafTagItem status;
  status.SetName("STATUS_TAG");
  vme->GetTagArray()->SetTag(status);

  // tag 2: number of slices
  mafTagItem noofslices;
  noofslices.SetName("NUMBER_OF_SLICES_TAG");
  noofslices.SetValue(m_Model->GetTotalNumberOfSyntheticScans());
  vme->GetTagArray()->SetTag(noofslices);

  // tag 3: xy scaling factors
  mafTagItem xyscalingfactors;
  xyscalingfactors.SetName("XY_SCALING_FACTORS_TAG");
  xyscalingfactors.SetNumberOfComponents(2);
  double x, y;
  m_Model->GetXYScalingFactorsOfMuscle(&x, &y);
  xyscalingfactors.SetComponent(x, 0);
  xyscalingfactors.SetComponent(y, 1);
  vme->GetTagArray()->SetTag(xyscalingfactors);

  // tag 4: operations stack
  mafTagItem SliceId_StackTag;
  mafTagItem ZValue_StackTag;
  mafTagItem OperationType_StackTag;
  mafTagItem Parameter1_StackTag;
  mafTagItem Parameter2_StackTag;

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
    m_Model->m_OperationsStack->GetTuple(i, operation_stack);

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
  mafTagItem typeofmuscle;
  typeofmuscle.SetName("TYPE_OF_MUSCLE_TAG");
  typeofmuscle.SetValue(m_Model->GetTypeOfMuscles());
  vme->GetTagArray()->SetTag(typeofmuscle);

  // root vme
  mafVME *RootVME = mafVME::SafeDownCast(m_Input->GetRoot());
  /*

  // patient section vme
  mafVME *PatientSectionVME = RootVME->FindInTreeByName(m_PatientMSFSectionName);
  assert (!(PatientSectionVME == NULL));

  // is there a "registered muscles" groupVME under patient section
  // of .msf file? if no, create it, if yes get a pointer to it
  mafVMEGroup *RegisteredMusclesVME = (mafVMEGroup *) PatientSectionVME->FindInTreeByName("registered muscles");
  if(RegisteredMusclesVME == NULL)
  {
  RegisteredMusclesVME = mafVMEGroup::New();
  RegisteredMusclesVME->SetName("registered muscles");
  PatientSectionVME->AddChild(RegisteredMusclesVME);
  mafEventMacro(mafEvent(this, VME_ADD, RegisteredMusclesVME));
  }

  // is there a groupVME under the "registered muscles" groupVME with name identical to
  // that of the muscle under registration? If no create it, if yes get a pointer to it
  mafVMEGroup *OurMuscleVME = (mafVMEGroup *) RegisteredMusclesVME->FindInTreeByName(m_surface_name);
  if(OurMuscleVME == NULL)
  {
  OurMuscleVME = mafVMEGroup::New();
  OurMuscleVME->SetName(m_surface_name);
  RegisteredMusclesVME->AddChild(OurMuscleVME);
  mafEventMacro(mafEvent(this, VME_ADD, OurMuscleVME));
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
  mafVME *LastChildVME = OurMuscleVME->GetLastChild();
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
  vme->SetName(m_SurfaceName + " registered");

  // add to tree (save now)
  //OurMuscleVME->AddChild(vme);
  mafEventMacro(mafEvent(this, VME_ADD, vme));
}

//----------------------------------------------------------------------------
void mafOpMML::OnRegistrationCANCEL()
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void mafOpMML::OnSlider() 
//----------------------------------------------------------------------------
{
  //
  m_Model->SetCurrentIdOfSyntheticScans(m_Slice - 1);

  // switch off non-relevant scans
  for (int j = 0; j < m_Model->GetTotalNumberOfSyntheticScans(); j++)
    m_Model->GetActorOfSyntheticScans(j)->VisibilityOff();

  // switch on current scan
  m_Model->GetActorOfSyntheticScans(m_Slice - 1)->VisibilityOn();

  // update line actors
  m_PH->SetLineActorX(m_Slice - 1);
  m_PV->SetLineActorX(m_Slice - 1);
  m_TH->SetLineActorX(m_Slice - 1);
  m_TV->SetLineActorX(m_Slice - 1);
  m_RA->SetLineActorX(m_Slice - 1);
  m_SN->SetLineActorX(m_Slice - 1);
  m_SS->SetLineActorX(m_Slice - 1);
  m_SE->SetLineActorX(m_Slice - 1);
  m_SW->SetLineActorX(m_Slice - 1);

  //
  Update();

  //
  if (m_Widget->GetCenterMode())
  {

  }

  //
  if (m_Widget->GetTranslationMode())
  {

  }

  //
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

//----------------------------------------------------------------------------
void mafOpMML::OnLut() 
//----------------------------------------------------------------------------
{
  double Low,High;

  m_Lut->GetSubRange(&Low, &High);

  m_Model->GetWindowLevelLookupTableOfSyntheticScans()->SetLevel((float) ((High + Low) / 2.0));
  m_Model->GetWindowLevelLookupTableOfSyntheticScans()->SetWindow((float) (High - Low));

  m_Model->Render();
}



//----------------------------------------------------------------------------
void mafOpMML::OnMuscleSelection() 
//----------------------------------------------------------------------------
{
  // raise event to request a VME selection dialog
  mafString title = "Select Muscle (Atlas)";
  mafEvent e(this,VME_CHOOSE);
  e.SetString(&title);
  mafEventMacro(e);
  mafVME *vme = (mafVME*)e.GetVme();
  if(!vme) return;

  // get vme surface
  m_Surface = mafVMESurface::SafeDownCast(vme);

  //
  if(m_Surface == NULL)
  {
    wxMessageBox("Wrong type of vme, a muscle vme is required","alert",wxICON_WARNING);
    return;
  }

  // get vme name
  m_SurfaceName = vme->GetName();

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
    mafTagItem *NumberOfSlicesTag;
    NumberOfSlicesTag = vme->GetTagArray()->GetTag("NUMBER_OF_SLICES_TAG");
    m_ScansNumber = NumberOfSlicesTag->GetComponentAsDouble(0);

    // can not change slice number
    m_ScansNumber2 = m_ScansNumber;
    m_ScansNumberTxt->SetValidator(mafGUIValidator(this,ID_CHOOSE_FAKE,m_ScansNumberTxt,&m_ScansNumber2,3,100)); // min/max values

    // tag 3: xy scaling factors
    if(vme->GetTagArray()->IsTagPresent("XY_SCALING_FACTORS_TAG") == false)
    {
      wxMessageBox("XY scaling factors tag in already registered muscle vme is missing!","alert",wxICON_WARNING);
      return;
    }
    mafTagItem *XYScalingFactorsTag;
    XYScalingFactorsTag = vme->GetTagArray()->GetTag("XY_SCALING_FACTORS_TAG");
    m_RegistrationXYScalingFactor = XYScalingFactorsTag->GetValueAsDouble(0);

    // can not change xy scaling factor
    m_RegistrationXYScalingFactor2 = m_RegistrationXYScalingFactor;
    m_RegistrationXYSxalingFactorTxt->SetValidator(mafGUIValidator(this,ID_CHOOSE_FAKE,m_RegistrationXYSxalingFactorTxt,&m_RegistrationXYScalingFactor2,0.01,2.0)); // min/max values

    // tag 4: operation stack
    //
    // slice id
    if(vme->GetTagArray()->IsTagPresent("SLICE_ID_STACK_TAG") == false)
    {
      wxMessageBox("Slice id tag in already registered muscle vme is missing!","alert",wxICON_WARNING);
      return;
    }
    m_SliceIdStackTag = vme->GetTagArray()->GetTag("SLICE_ID_STACK_TAG");

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
    mafTagItem *TypeOfMuscleTag;
    TypeOfMuscleTag = vme->GetTagArray()->GetTag("TYPE_OF_MUSCLE_TAG");
    m_MuscleType = TypeOfMuscleTag->GetComponentAsDouble(0);

    // set m_Surface to parent vme (original non-registered muscle)
    // to identify landmarks correctly
    wxString ParentVMEName;
    ParentVMEName = m_SurfaceName.BeforeFirst('.');
    m_SurfaceName = ParentVMEName;
  }

  // set up landmarks (parameters are .msf section names)
  // SetUpLandmarks(m_AtlasMSFSectionName, m_PatientMSFSectionName);

  //
  mafVME *RootVME;
  mafVME *AtlasSectionVME;
  mafVME *MuscleInAtlasSectionVME;

  if (m_RegistrationStatus == 1)
  {
    // root vme
    RootVME = mafVME::SafeDownCast(m_Input->GetRoot());

    // atlas section vme
    AtlasSectionVME = (mafVME*)(RootVME->FindInTreeByName(m_AtlasMSFSectionName));
    assert (!(AtlasSectionVME == NULL));

    MuscleInAtlasSectionVME = (mafVME*)(AtlasSectionVME->FindInTreeByName(m_SurfaceName));
    assert (!(MuscleInAtlasSectionVME == NULL));

    //
    m_Surface = mafVMESurface::SafeDownCast(MuscleInAtlasSectionVME);
  }


  // set polydata transform filter to get selected muscle
  vtkTransform *transform = vtkTransform::New();
  vtkTransformPolyDataFilter *musclepd = vtkTransformPolyDataFilter::New();
  transform->SetMatrix(m_Surface->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
  musclepd->SetInput((vtkPolyData*)m_Surface->GetOutput()->GetVTKData());
  musclepd->SetTransform(transform);
  musclepd->Update();

  // set muscle as output from transform
  m_Muscle = musclepd->GetOutput();


  // clean up
  transform->Delete();
  //
  //	// if all landmarks chosen
  //	if (m_l1_name.compare("none") != 0 &&
  //		m_l2_name.compare("none") != 0 &&
  //		m_l3_name.compare("none") != 0
  //	   )
  //	{
  //		// activate ok button
  //    m_choose_ok->Enable(true);
  //	}
  //	else
  //		// de-activate ok button
  //		m_choose_ok->Enable(false);  


  m_ChooseOk->Enable(true);

  // update window
  m_ChooseDlg->TransferDataToWindow();
}

//----------------------------------------------------------------------------
void mafOpMML::OnResetView() 
//----------------------------------------------------------------------------
{
  m_Model->GetRenderer()->ResetCamera();
  m_Model->Render();
}

//----------------------------------------------------------------------------
void mafOpMML::OnUndo() 
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
  m_Model->m_OperationsStack->GetTuple(m_Widget->GetNextOperationId() - 1, params_undo);

  int maxsliceid = m_Model->GetTotalNumberOfSyntheticScans() - 1;

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
  m_Model->m_OperationsStack->GetTuple(m_Widget->GetNextOperationId() - 1, params_undo_again);

  // if go back to non-scaling situation reset non-scaling splines colours
  if (m_Model->m_ScalingOccured)
  {
    if (m_Model->m_ScalingOccuredOperationId == m_Widget->GetNextOperationId())
    {
      m_Model->m_ScalingOccured = FALSE;

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
      m_Model->m_OperationsStack->GetTuple(i, params);
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
    if ((params_undo[0] == 0) || (params_undo[0] == (m_Model->GetTotalNumberOfSyntheticScans() - 1)))
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
      m_Model->m_OperationsStack->GetTuple(i, params);
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
    if ((params_undo[0] == 0) || (params_undo[0] == (m_Model->GetTotalNumberOfSyntheticScans() - 1)))
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
      m_Model->m_OperationsStack->GetTuple(i, params);
      if (params[0] == params_undo[0] && // same slice
        params[2] == (int) params_undo[2]) // same operation
      {
        m_RA->AddPoint(params_undo[0], params[3]);
        m_Model->GetRASpline()->AddPoint(params_undo[1], params[3]);
        break;
      }
    }

    // first/last slice
    if ((params_undo[0] == 0) || (params_undo[0] == (m_Model->GetTotalNumberOfSyntheticScans() - 1)))
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
      m_Model->m_OperationsStack->GetTuple(i, params);
      if (params[0] == params_undo[0] && // same slice
        params[2] == (int) params_undo[2]) // same operation
      {
        m_SN->AddPoint(params_undo[0], params[3]);
        m_Model->GetSNSpline()->AddPoint(params_undo[1], params[3]);
        break;
      }
    }

    // first/last slice
    if ((params_undo[0] == 0) || (params_undo[0] == (m_Model->GetTotalNumberOfSyntheticScans() - 1)))
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
      m_Model->m_OperationsStack->GetTuple(i, params);
      if (params[0] == params_undo[0] && // same slice
        params[2] == (int) params_undo[2]) // same operation
      {
        m_SS->AddPoint(params_undo[0], params[3]);
        m_Model->GetSSSpline()->AddPoint(params_undo[1], params[3]);
        break;
      }
    }

    // first/last slice
    if ((params_undo[0] == 0) || (params_undo[0] == (m_Model->GetTotalNumberOfSyntheticScans() - 1)))
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
      m_Model->m_OperationsStack->GetTuple(i, params);
      if (params[0] == params_undo[0] && // same slice
        params[2] == (int) params_undo[2]) // same operation
      {
        m_SE->AddPoint(params_undo[0], params[3]);
        m_Model->GetSESpline()->AddPoint(params_undo[1], params[3]);
        break;
      }
    }

    // first/last slice
    if ((params_undo[0] == 0) || (params_undo[0] == (m_Model->GetTotalNumberOfSyntheticScans() - 1)))
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
      m_Model->m_OperationsStack->GetTuple(i, params);
      if (params[0] == params_undo[0] && // same slice
        params[2] == (int) params_undo[2]) // same operation
      {
        m_SW->AddPoint(params_undo[0], params[3]);
        m_Model->GetSWSpline()->AddPoint(params_undo[1], params[3]);
        break;
      }
    }

    // first/last slice
    if ((params_undo[0] == 0) || (params_undo[0] == (m_Model->GetTotalNumberOfSyntheticScans() - 1)))
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
    m_TranslateOpButton->SetTitle("T");
    m_RotateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_RotateOpButton->SetTitle("R");
    m_ScaleOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_ScaleOpButton->SetTitle("S");

    // this button on
    m_PlaceOpButton->SetBackgroundColour(Green);
    m_PlaceOpButton->SetTitle(">>P<<");

    //
    ResetOperation();

    // prepare display information
    m_Model->GetScaledTextActor1()->GetPositionCoordinate()->SetValue(0.0, 0.8);
    m_Model->GetScaledTextActor2()->GetPositionCoordinate()->SetValue(0.0, 0.9);

    m_Widget->CenterModeOn();
    break;

  case 1: // translate
    // other buttons off
    m_PlaceOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_PlaceOpButton->SetTitle("P");
    m_RotateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_RotateOpButton->SetTitle("R");
    m_ScaleOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_ScaleOpButton->SetTitle("S");

    // this button on
    m_TranslateOpButton->SetBackgroundColour(Green);
    m_TranslateOpButton->SetTitle(">>T<<");

    //
    ResetOperation();

    // prepare display information
    m_Model->GetScaledTextActor1()->GetPositionCoordinate()->SetValue(0.0, 0.8);
    m_Model->GetScaledTextActor2()->GetPositionCoordinate()->SetValue(0.0, 0.9);

    m_Widget->TranslationModeOn();
    break;

  case 2: // rotate
    // other buttons off
    m_PlaceOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_PlaceOpButton->SetTitle("P");
    m_TranslateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_TranslateOpButton->SetTitle("T");
    m_ScaleOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_ScaleOpButton->SetTitle("S");

    // this button on
    m_RotateOpButton->SetBackgroundColour(Green);
    m_RotateOpButton->SetTitle(">>R<<");

    //
    ResetOperation();

    // prepare display information
    m_Model->GetScaledTextActor1()->GetPositionCoordinate()->SetValue(0.0, 0.9);

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
    m_PlaceOpButton->SetTitle("P");
    m_TranslateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_TranslateOpButton->SetTitle("T");
    m_RotateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
    m_RotateOpButton->SetTitle("R");

    // this button on
    m_ScaleOpButton->SetBackgroundColour(Green);
    m_ScaleOpButton->SetTitle(">>S<<");

    m_Model->GetScaledTextActor1()->GetPositionCoordinate()->SetValue(0.0, 0.9);

    //
    ResetOperation();

    m_Widget->UpdateScalingHandles();
    m_Widget->ScalingHandlesOn();
    m_Widget->ScalingModeOn();
  }

  // update vertical cuts
  m_Model->UpdateSegmentCuttingPlanes();

  // update widget
  m_Widget->UpdateWidgetTransform();

  // update north east segment
  m_Model->UpdateSegmentNorthEastTransform();

  // update north west segment
  m_Model->UpdateSegmentNorthWestTransform();

  // update south east segment
  m_Model->UpdateSegmentSouthEastTransform();

  // update south west segment
  m_Model->UpdateSegmentSouthWestTransform();

  // update contour axes
  m_Model->UpdateContourAxesTransform();

  // set visibility
  m_Model->SetContourAxesVisibility();

  // update global axes
  m_Model->UpdateGlobalAxesTransform();

  // set visibility
  m_Model->SetGlobalAxesVisibility();

  //
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
void mafOpMML::OnOperation() 
//----------------------------------------------------------------------------
{
  // operations
  if (m_State == 0) // place
  {
    if (m_Model->GetScalingOccured())
    {
      m_State = 3;
      m_OpDlg->TransferDataToWindow();
      wxMessageBox("Operation Unavailable (Scaling Occured)","alert",wxICON_WARNING);
      return;
    }

    //
    ResetOperation();

    // prepare display information
    m_Model->GetScaledTextActor1()->GetPositionCoordinate()->SetValue(0.0, 0.8);
    m_Model->GetScaledTextActor2()->GetPositionCoordinate()->SetValue(0.0, 0.9);

    m_Widget->CenterModeOn();
  }
  else
    if (m_State == 1) // translate
    {
      if (m_Model->GetScalingOccured())
      {
        m_State = 3;
        m_OpDlg->TransferDataToWindow();
        wxMessageBox("Operation Unavailable (Scaling Occured)","alert",wxICON_WARNING);
        return;
      }

      //
      ResetOperation();

      // prepare display information
      m_Model->GetScaledTextActor1()->GetPositionCoordinate()->SetValue(0.0, 0.8);
      m_Model->GetScaledTextActor2()->GetPositionCoordinate()->SetValue(0.0, 0.9);

      // prepare widget
      m_Widget->TranslationModeOn();
    }
    else
      if (m_State == 2) // rotate
      {
        if (m_Model->GetScalingOccured())
        {
          m_State = 3;
          m_OpDlg->TransferDataToWindow();
          wxMessageBox("Operation Unavailable (Scaling Occured)","alert",wxICON_WARNING);
          return;
        }

        //
        ResetOperation();

        m_Model->GetScaledTextActor1()->GetPositionCoordinate()->SetValue(0.0, 0.9);

        m_Widget->UpdateRotationHandle();
        m_Widget->RotationHandleOn();
        m_Widget->RotationModeOn();
      }
      else
        if (m_State == 3) // scale
        {
          m_Model->GetScaledTextActor1()->GetPositionCoordinate()->SetValue(0.0, 0.9);

          //
          ResetOperation();

          m_Widget->UpdateScalingHandles();
          m_Widget->ScalingHandlesOn();
          m_Widget->ScalingModeOn();

          // on successful scaling, ScalingOccured flag in Model
          // is set to true, mafOpMMLContourWidget::OnLeftButtonUp
        }
        else
          wxMessageBox("Unknown operation","alert",wxICON_WARNING);

  //
  m_Model->Render();
}

//----------------------------------------------------------------------------
void mafOpMML::ResetOperation() 
//----------------------------------------------------------------------------
{
  // reset - handles off
  m_Widget->RotationHandleOff();
  m_Widget->ScalingHandlesOff();

  // reset - modes off
  m_Widget->CenterModeOff();
  m_Widget->ScalingModeOff();
  m_Widget->RotationModeOff();
  m_Widget->TranslationModeOff();
}

//----------------------------------------------------------------------------
void mafOpMML::OnContourLineAxesVisibility() 
//----------------------------------------------------------------------------
{
  m_ShowAxes = (m_ShowAxes + 1) % 2;

  if (m_ShowAxes)
  {
    m_AxesOnOffButton->SetTitle("Axes Off");
    // action line coordinate system
    //Model->GetPositiveXAxisActor()->VisibilityOn();
    //Model->GetNegativeXAxisActor()->VisibilityOn();
    //Model->GetPositiveYAxisActor()->VisibilityOn();
    //Model->GetNegativeYAxisActor()->VisibilityOn();
    //Model->GetPositiveZAxisActor()->VisibilityOn();
    //Model->GetNegativeZAxisActor()->VisibilityOn();

    // contour coordinate system
    m_Model->GetContourPositiveXAxisActor()->VisibilityOn();
    m_Model->GetContourNegativeXAxisActor()->VisibilityOn();
    m_Model->GetContourPositiveYAxisActor()->VisibilityOn();
    m_Model->GetContourNegativeYAxisActor()->VisibilityOn();
  }
  else
  {
    m_AxesOnOffButton->SetTitle("Axes On");
    // action line coordinate system
    //Model->GetPositiveXAxisActor()->VisibilityOff();
    //Model->GetNegativeXAxisActor()->VisibilityOff();
    //Model->GetPositiveYAxisActor()->VisibilityOff();
    //Model->GetNegativeYAxisActor()->VisibilityOff();
    //Model->GetPositiveZAxisActor()->VisibilityOff();
    //Model->GetNegativeZAxisActor()->VisibilityOff();

    m_Model->GetContourPositiveXAxisActor()->VisibilityOff();
    m_Model->GetContourNegativeXAxisActor()->VisibilityOff();
    m_Model->GetContourPositiveYAxisActor()->VisibilityOff();
    m_Model->GetContourNegativeYAxisActor()->VisibilityOff();
  }

  m_Model->Render();
}

//----------------------------------------------------------------------------
void mafOpMML::OnContourVisibility() 
//----------------------------------------------------------------------------
{
  m_ContourVisibility = (m_ContourVisibility + 1) % 2;

  if (m_ContourVisibility) // visible
  {
    m_ContourOnOffButton->SetTitle("Contour Off");

    m_Model->GetNEContourActor()->VisibilityOn();
    m_Model->GetNWContourActor()->VisibilityOn();
    m_Model->GetSEContourActor()->VisibilityOn();
    m_Model->GetSWContourActor()->VisibilityOn();

    if (m_Widget->GetRotationMode())
    {
      // handle
      m_Widget->RotationHandleOn();
    }
    else
      if (m_Widget->GetScalingMode())
      {
        // handles
        m_Widget->ScalingHandlesOn();
      }
  }
  else // non-visible
  {
    m_ContourOnOffButton->SetTitle("Contour On");

    m_Model->GetNEContourActor()->VisibilityOff();
    m_Model->GetNWContourActor()->VisibilityOff();
    m_Model->GetSEContourActor()->VisibilityOff();
    m_Model->GetSWContourActor()->VisibilityOff();

    if (m_Widget->GetRotationMode())
    {
      // handle
      m_Widget->RotationHandleOff();
    }
    else
      if (m_Widget->GetScalingMode())
      {
        // handles
        m_Widget->ScalingHandlesOff();
      }
  }

  m_Model->Render();
}

//----------------------------------------------------------------------------
void mafOpMML::OnLandmark1AtlasPatientSelection() 
//----------------------------------------------------------------------------
{
  //
  if(m_Surface == NULL)
  {
    wxMessageBox("No muscle selected","alert",wxICON_WARNING);
    return;
  }

  mafString title = "Select Landmark 1 (Atlas)";
  mafEvent e(this,VME_CHOOSE);
  e.SetString(&title);
  mafEventMacro(e);
  mafVME *vme = (mafVME*)e.GetVme();
  if(!vme) return;

  mafVMELandmark *lm = mafVMELandmark::SafeDownCast(vme);
  if(lm == NULL)
  {
    wxMessageBox("wrong type of vme, a Landmark VME is required","alert",wxICON_WARNING);
    return;
  }

  // get root node
  mafVME *root = mafVME::SafeDownCast(m_Input->GetRoot());

  // get landmarks parent node
  mafVME* parentvme = vme->GetParent();

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
  mafVME *L1PatientParentVME = (mafVME*)(root->FindInTreeByName(m_PatientMSFSectionName));
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
  mafVME *L1PatientVME = (mafVME*)(L1PatientParentVME->FindInTreeByName(vme->GetName()));
  if(L1PatientVME == NULL)
  {
    wxMessageBox("Landmark missing from " + m_PatientMSFSectionName + " section","alert",wxICON_WARNING);
    return;
  }
  // get landmark
  mafVMELandmark *L1PatientVMELandmark = mafVMELandmark::SafeDownCast(L1PatientVME);
  if(L1PatientVMELandmark == NULL)
  {
    wxMessageBox("wrong type of vme, a landmark vme is required","alert",wxICON_WARNING);
    return;
  }

  // coordinates
  lm->GetPoint(m_L1Point);
  L1PatientVMELandmark->GetPoint(m_P1);

  // set name
  m_L1Name = vme->GetName();
  m_P1Name = L1PatientVME->GetName();

  // if all landmarks chosen
  if (m_L1Name.compare("none") != 0 &&
    m_L2Name.compare("none") != 0 &&
    m_L3Name.compare("none") != 0
    )
  {
    // activate ok button
    m_ChooseOk->Enable(true);
  }
  else
    // de-activate ok button
    m_ChooseOk->Enable(false);

  // update window
  m_ChooseDlg->TransferDataToWindow();
}

//----------------------------------------------------------------------------
void mafOpMML::OnLandmark2AtlasPatientSelection() 
//----------------------------------------------------------------------------
{
  //
  if(m_Surface == NULL)
  {
    wxMessageBox("No muscle selected","alert",wxICON_WARNING);
    return;
  }

  mafString title = "Select Landmark 2 (Atlas)";
  mafEvent e(this,VME_CHOOSE);
  e.SetString(&title);
  mafEventMacro(e);
  mafVME *vme = (mafVME*)e.GetVme();
  if(!vme) return;

  mafVMELandmark *lm = mafVMELandmark::SafeDownCast(vme);
  if(lm == NULL)
  {
    wxMessageBox("wrong type of vme, a Landmark VME is required","alert",wxICON_WARNING);
    return;
  }

  // get root node
  mafVME *root = mafVME::SafeDownCast(m_Input->GetRoot());

  // get landmarks parent node
  mafVME* parentvme = vme->GetParent();

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
  mafVME *L2PatientParentVME = (mafVME*)(root->FindInTreeByName(m_PatientMSFSectionName));
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
  mafVME *L2PatientVME = (mafVME*)(L2PatientParentVME->FindInTreeByName(vme->GetName()));
  if(L2PatientVME == NULL)
  {
    wxMessageBox("Landmark missing from " + m_PatientMSFSectionName + " section","alert",wxICON_WARNING);
    return;
  }
  // get landmark
  mafVMELandmark *L2PatientVMELandmark = mafVMELandmark::SafeDownCast(L2PatientVME);
  if(L2PatientVMELandmark == NULL)
  {
    wxMessageBox("wrong type of vme, a landmark vme is required","alert",wxICON_WARNING);
    return;
  }

  // coordinates
  lm->GetPoint(m_L2Point);
  L2PatientVMELandmark->GetPoint(m_P2);

  // set name
  m_L2Name = vme->GetName();
  m_P2Name = L2PatientVME->GetName();

  // if all landmarks chosen
  if (m_L1Name.compare("none") != 0 &&
    m_L2Name.compare("none") != 0 &&
    m_L3Name.compare("none") != 0
    )
  {
    // activate ok button
    m_ChooseOk->Enable(true);
  }
  else
    // de-activate ok button
    m_ChooseOk->Enable(false);

  // update window
  m_ChooseDlg->TransferDataToWindow();
}

//----------------------------------------------------------------------------
void mafOpMML::OnLandmark3AtlasPatientSelection() 
//----------------------------------------------------------------------------
{
  //
  if(m_Surface == NULL)
  {
    wxMessageBox("No muscle selected","alert",wxICON_WARNING);
    return;
  }

  mafString title = "Select Landmark 3 (Atlas)";
  mafEvent e(this,VME_CHOOSE);
  e.SetString(&title);
  mafEventMacro(e);
  mafVME *vme = (mafVME*)e.GetVme();
  if(!vme) return;

  mafVMELandmark *lm = mafVMELandmark::SafeDownCast(vme);
  if(lm == NULL)
  {
    wxMessageBox("wrong type of vme, a Landmark VME is required","alert",wxICON_WARNING);
    return;
  }

  // get root node
  mafVME *root = mafVME::SafeDownCast(m_Input->GetRoot());

  // get landmarks parent node
  mafVME* parentvme = vme->GetParent();

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
  mafVME *L3PatientParentVME = (mafVME*)(root->FindInTreeByName(m_PatientMSFSectionName));
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
  mafVME *L3PatientVME = (mafVME*)(L3PatientParentVME->FindInTreeByName(vme->GetName()));
  if(L3PatientVME == NULL)
  {
    wxMessageBox("Landmark missing from " + m_PatientMSFSectionName + " section","alert",wxICON_WARNING);
    return;
  }
  // get landmark
  mafVMELandmark *L3PatientVMELandmark = mafVMELandmark::SafeDownCast(L3PatientVME);
  if(L3PatientVMELandmark == NULL)
  {
    wxMessageBox("wrong type of vme, a landmark vme is required","alert",wxICON_WARNING);
    return;
  }

  // coordinates
  lm->GetPoint(m_L3Point);
  L3PatientVMELandmark->GetPoint(m_P3);

  // set name
  m_L3Name = vme->GetName();
  m_P3Name = L3PatientVME->GetName();

  // if all landmarks chosen
  if (m_L1Name.compare("none") != 0 &&
    m_L2Name.compare("none") != 0 &&
    m_L3Name.compare("none") != 0
    )
  {
    // activate ok button
    m_ChooseOk->Enable(true);
  }
  else
    // de-activate ok button
    m_ChooseOk->Enable(false);

  // update window
  m_ChooseDlg->TransferDataToWindow();
}

//----------------------------------------------------------------------------
void mafOpMML::OnLandmark4AtlasPatientSelection() 
//----------------------------------------------------------------------------
{
  //
  if(m_Surface == NULL)
  {
    wxMessageBox("No muscle selected","alert",wxICON_WARNING);
    return;
  }

  mafString title = "Select Landmark 3 (Atlas)";
  mafEvent e(this,VME_CHOOSE);
  e.SetString(&title);
  mafEventMacro(e);
  mafVME *vme = (mafVME*)e.GetVme();
  if(!vme) return;

  mafVMELandmark *lm = mafVMELandmark::SafeDownCast(vme);
  if(lm == NULL)
  {
    wxMessageBox("wrong type of vme, a Landmark VME is required","alert",wxICON_WARNING);
    return;
  }

  // get root node
  mafVME *root = mafVME::SafeDownCast(m_Input->GetRoot());

  // get landmarks parent node
  mafVME* parentvme = vme->GetParent();

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
  mafVME *L3PatientParentVME = (mafVME*)(root->FindInTreeByName(m_PatientMSFSectionName));
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
  mafVME *L3PatientVME = (mafVME*)(L3PatientParentVME->FindInTreeByName(vme->GetName()));
  if(L3PatientVME == NULL)
  {
    wxMessageBox("Landmark missing from " + m_PatientMSFSectionName + " section","alert",wxICON_WARNING);
    return;
  }
  // get landmark
  mafVMELandmark *L3PatientVMELandmark = mafVMELandmark::SafeDownCast(L3PatientVME);
  if(L3PatientVMELandmark == NULL)
  {
    wxMessageBox("wrong type of vme, a landmark vme is required","alert",wxICON_WARNING);
    return;
  }

  // coordinates
  lm->GetPoint(m_L3Point);
  L3PatientVMELandmark->GetPoint(m_P3);

  // set name
  m_L3Name = vme->GetName();
  m_P3Name = L3PatientVME->GetName();

  // if all landmarks chosen
  if (m_L1Name.compare("none") != 0 &&
    m_L2Name.compare("none") != 0 &&
    m_L3Name.compare("none") != 0
    )
  {
    // activate ok button
    m_ChooseOk->Enable(true);
  }
  else
    // de-activate ok button
    m_ChooseOk->Enable(false);

  // update window
  m_ChooseDlg->TransferDataToWindow();
}



//----------------------------------------------------------------------------
bool mafOpMML::CreateInputsDlg() 
//----------------------------------------------------------------------------
{
  // create the dialog
  m_ChooseDlg = new mafGUIDialog("SetUp"); 

  // vertical stacker for the rows of widgets
  wxBoxSizer *vs1 = new wxBoxSizer(wxVERTICAL);


  // muscle
  wxStaticText *lab_1  = new wxStaticText(m_ChooseDlg, -1, "Surface", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl   *text_1 = new wxTextCtrl(m_ChooseDlg ,  -1, "",        wxPoint(0,0), wxSize(150,20), wxNO_BORDER |wxTE_READONLY );
  text_1->SetValidator(mafGUIValidator(this, ID_CHOOSE_SURFACE, text_1, &m_SurfaceName));
  mafGUIButton    *b_1    = new mafGUIButton(m_ChooseDlg , ID_CHOOSE_SURFACE, "select", wxPoint(0,0), wxSize(50,20));
  b_1->SetListener(this);

  wxBoxSizer *hs_1 = new wxBoxSizer(wxHORIZONTAL);
  hs_1->Add(lab_1, 0);
  hs_1->Add(text_1, 1, wxEXPAND);
  hs_1->Add(b_1,0);
  vs1->Add(hs_1, 0, wxEXPAND | wxALL, 2);



  /*
  // landmarks
  wxStaticText *lab_5  = new wxStaticText(m_choose_dlg, -1, "Landmark 1", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl   *text_5 = new wxTextCtrl(m_choose_dlg ,  -1, "",        wxPoint(0,0), wxSize(150,20),wxNO_BORDER |wxTE_READONLY );
  text_5->SetValidator(mafGUIValidator(this,ID_CHOOSE_L1,text_5,&m_l1_name));
  mafGUIButton *b_5       = new mafGUIButton(m_choose_dlg ,ID_CHOOSE_L1,"select",wxPoint(0,0), wxSize(50,20));
  b_5->SetListener(this);
  wxBoxSizer *hs_5 = new wxBoxSizer(wxHORIZONTAL);
  hs_5->Add(lab_5,0);
  hs_5->Add(text_5,1,wxEXPAND);
  hs_5->Add(b_5,0);
  vs1->Add(hs_5,0,wxEXPAND | wxALL, 2);

  wxStaticText *lab_6  = new wxStaticText(m_choose_dlg, -1, "Landmark 2", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl   *text_6 = new wxTextCtrl(m_choose_dlg ,  -1, "",        wxPoint(0,0), wxSize(150,20),wxNO_BORDER |wxTE_READONLY );
  text_6->SetValidator(mafGUIValidator(this,ID_CHOOSE_L2,text_6,&m_l2_name));
  mafGUIButton *b_6       = new mafGUIButton(m_choose_dlg ,ID_CHOOSE_L2,"select",wxPoint(0,0), wxSize(50,20));
  b_6->SetListener(this);
  wxBoxSizer *hs_6 = new wxBoxSizer(wxHORIZONTAL);
  hs_6->Add(lab_6,0);
  hs_6->Add(text_6,1,wxEXPAND);
  hs_6->Add(b_6,0);
  vs1->Add(hs_6,0,wxEXPAND | wxALL, 2);

  wxStaticText *lab_7  = new wxStaticText(m_choose_dlg, -1, "Landmark 3", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl   *text_7 = new wxTextCtrl(m_choose_dlg ,  -1, "",        wxPoint(0,0), wxSize(150,20),wxNO_BORDER |wxTE_READONLY );
  text_7->SetValidator(mafGUIValidator(this,ID_CHOOSE_L3,text_7,&m_l3_name));
  mafGUIButton *b_7       = new mafGUIButton(m_choose_dlg ,ID_CHOOSE_L3,"select",wxPoint(0,0), wxSize(50,20));
  b_7->SetListener(this);
  wxBoxSizer *hs_7 = new wxBoxSizer(wxHORIZONTAL);
  hs_7->Add(lab_7,0);
  hs_7->Add(text_7,1,wxEXPAND);
  hs_7->Add(b_7,0);
  vs1->Add(hs_7,0,wxEXPAND | wxALL, 2);

  #ifdef TestingVersion
  wxStaticText *lab_8  = new wxStaticText(m_choose_dlg, -1, "Landmark 4", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl   *text_8 = new wxTextCtrl(m_choose_dlg ,  -1, "",        wxPoint(0,0), wxSize(150,20),wxNO_BORDER |wxTE_READONLY );
  text_8->SetValidator(mafGUIValidator(this,ID_CHOOSE_L4,text_8,&m_l4_name));
  mafGUIButton *b_8       = new mafGUIButton(m_choose_dlg ,ID_CHOOSE_L4,"select",wxPoint(0,0), wxSize(50,20));
  b_8->SetListener(this);
  wxBoxSizer *hs_8 = new wxBoxSizer(wxHORIZONTAL);
  hs_8->Add(lab_8,0);
  hs_8->Add(text_8,1,wxEXPAND);
  hs_8->Add(b_8,0);
  vs1->Add(hs_8,0,wxEXPAND | wxALL, 2);
  #endif

  // registration xy scaling factor
  wxStaticText *RegistrationXYSxalingFactorLab  = new wxStaticText(m_choose_dlg, -1, "XY scale (0.01 - 2.0)", wxPoint(0,0), wxSize(150,20));
  RegistrationXYSxalingFactorTxt = new wxTextCtrl(m_choose_dlg ,  -1, "",        wxPoint(0,0), wxSize(150,20),wxNO_BORDER );
  RegistrationXYSxalingFactorTxt->SetValidator(mafGUIValidator(this,ID_CHOOSE_FAKE,RegistrationXYSxalingFactorTxt,&m_RegistrationXYScalingFactor,0.01,2.0)); // min/max values
  wxBoxSizer *RegistrationXYSxalingFactorSizer = new wxBoxSizer(wxHORIZONTAL);
  RegistrationXYSxalingFactorSizer->Add(RegistrationXYSxalingFactorLab,0);
  RegistrationXYSxalingFactorSizer->Add(RegistrationXYSxalingFactorTxt,1,wxEXPAND);
  vs1->Add(RegistrationXYSxalingFactorSizer,0,wxEXPAND | wxALL, 2);


  // scans distance
  wxStaticText *ScansDistanceLab  = new wxStaticText(m_choose_dlg, -1, "Slice distance (0.1 - 100.0)", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl   *ScansDistanceTxt = new wxTextCtrl(m_choose_dlg ,  -1, "",        wxPoint(0,0), wxSize(150,20),wxNO_BORDER );
  ScansDistanceTxt->SetValidator(mafGUIValidator(this,ID_FAKE,ScansDistanceTxt,&m_ScansDistance,0.1,100)); // min/max values
  wxBoxSizer *ScansDistanceHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
  ScansDistanceHorizontalSizer->Add(ScansDistanceLab,0);
  ScansDistanceHorizontalSizer->Add(ScansDistanceTxt,1,wxEXPAND);
  vs1->Add(ScansDistanceHorizontalSizer,0,wxEXPAND | wxALL, 2);
  */



  // scans number
  wxStaticText *ScansNumberLab  = new wxStaticText(m_ChooseDlg, -1, "Slice number (3 - 100)", wxPoint(0,0), wxSize(150,20));
  m_ScansNumberTxt = new wxTextCtrl(m_ChooseDlg ,  -1, "",        wxPoint(0,0), wxSize(150,20),wxNO_BORDER );
  m_ScansNumberTxt->SetValidator(mafGUIValidator(this,ID_CHOOSE_FAKE,m_ScansNumberTxt,&m_ScansNumber,3,100)); // min/max values
  wxBoxSizer *ScansNumberHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
  ScansNumberHorizontalSizer->Add(ScansNumberLab,0);
  ScansNumberHorizontalSizer->Add(m_ScansNumberTxt,1,wxEXPAND);
  vs1->Add(ScansNumberHorizontalSizer,0,wxEXPAND | wxALL, 2);


  /*
  // scans x y size (now done automatically)
  wxStaticText *ScansSizeLab  = new wxStaticText(m_choose_dlg, -1, "Slice size (1 - 128)", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl   *ScansSizeTxt1 = new wxTextCtrl(m_choose_dlg ,  -1, "",        wxPoint(0,0), wxSize(75,20),wxNO_BORDER );
  ScansSizeTxt1->SetValidator(mafGUIValidator(this,ID_FAKE,ScansSizeTxt1,&m_ScansSize[0],1,128)); //min/max values
  wxTextCtrl   *ScansSizeTxt2 = new wxTextCtrl(m_choose_dlg ,  -1, "",        wxPoint(0,0), wxSize(75,20),wxNO_BORDER );
  ScansSizeTxt2->SetValidator(mafGUIValidator(this,ID_FAKE,ScansSizeTxt2,&m_ScansSize[1],1,128)); // min/max values
  wxBoxSizer *ScansSizeHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
  ScansSizeHorizontalSizer->Add(ScansSizeLab,0);
  ScansSizeHorizontalSizer->Add(ScansSizeTxt1,1,wxEXPAND | wxRIGHT, 3);
  ScansSizeHorizontalSizer->Add(ScansSizeTxt2,1,wxEXPAND);
  vs1->Add(ScansSizeHorizontalSizer,0,wxEXPAND | wxALL, 2);
  */


  // scans grain
  wxStaticText *ScansGrainLab  = new wxStaticText(m_ChooseDlg, -1, "Slice grain (1 - 5)", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl   *ScansGrainTxt1 = new wxTextCtrl(m_ChooseDlg ,  -1, "",        wxPoint(0,0), wxSize(75,20),wxNO_BORDER );
  ScansGrainTxt1->SetValidator(mafGUIValidator(this,ID_CHOOSE_FAKE,ScansGrainTxt1,&m_ScansGrain,1,5));
  wxBoxSizer *ScansGrainHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
  ScansGrainHorizontalSizer->Add(ScansGrainLab,0);
  ScansGrainHorizontalSizer->Add(ScansGrainTxt1,1,wxEXPAND | wxRIGHT, 3);
  vs1->Add(ScansGrainHorizontalSizer,0,wxEXPAND | wxALL, 2);


#ifdef TestingVersion
  // 3d flag
  wxStaticText *flagLab  = new wxStaticText(m_ChooseDlg, -1, "3D (0/1)", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl   *flagTxt1 = new wxTextCtrl(m_ChooseDlg ,  -1, "",        wxPoint(0,0), wxSize(75,20),wxNO_BORDER );
  flagTxt1->SetValidator(mafGUIValidator(this,ID_CHOOSE_FAKE,flagTxt1,&m_Flag3D,0,1));
  wxBoxSizer *flagHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
  flagHorizontalSizer->Add(flagLab, 0);
  flagHorizontalSizer->Add(flagTxt1,1,wxEXPAND | wxRIGHT, 3);
  vs1->Add(flagHorizontalSizer,0,wxEXPAND | wxALL, 2);

  /*
  // 4 landmarks flag
  wxStaticText *LandmarksflagLab  = new wxStaticText(m_choose_dlg, -1, "4 Landmarks (0/1)", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl   *LandmarksflagTxt1 = new wxTextCtrl(m_choose_dlg ,  -1, "",        wxPoint(0,0), wxSize(75,20),wxNO_BORDER );
  LandmarksflagTxt1->SetValidator(mafGUIValidator(this,ID_CHOOSE_FAKE,LandmarksflagTxt1,&m_Landmarksflag,0,1));
  wxBoxSizer *LandmarksflagHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
  LandmarksflagHorizontalSizer->Add(LandmarksflagLab, 0);
  LandmarksflagHorizontalSizer->Add(LandmarksflagTxt1,1,wxEXPAND | wxRIGHT, 3);
  vs1->Add(LandmarksflagHorizontalSizer,0,wxEXPAND | wxALL, 2);
  */
#endif

  /*
  // scans x y resolution  (set automatically from scans x, y size)
  wxStaticText *ScansResolutionLab  = new wxStaticText(m_choose_dlg, -1, "Slice resolution (8 - 256)", wxPoint(0,0), wxSize(150,20));
  wxTextCtrl   *ScansResolutionTxt1 = new wxTextCtrl(m_choose_dlg ,  -1, "",        wxPoint(0,0), wxSize(75,20),wxNO_BORDER );
  ScansResolutionTxt1->SetValidator(mafGUIValidator(this,ID_FAKE,ScansResolutionTxt1,&m_ScansResolution[0],8,256));
  wxTextCtrl   *ScansResolutionTxt2 = new wxTextCtrl(m_choose_dlg ,  -1, "",        wxPoint(0,0), wxSize(75,20),wxNO_BORDER );
  ScansResolutionTxt2->SetValidator(mafGUIValidator(this,ID_FAKE,ScansResolutionTxt2,&m_ScansResolution[1],8,256));
  wxBoxSizer *ScansResolutionHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
  ScansResolutionHorizontalSizer->Add(ScansResolutionLab,0);
  ScansResolutionHorizontalSizer->Add(ScansResolutionTxt1,1,wxEXPAND | wxRIGHT, 3);
  ScansResolutionHorizontalSizer->Add(ScansResolutionTxt2,1,wxEXPAND);
  vs1->Add(ScansResolutionHorizontalSizer,0,wxEXPAND | wxALL, 2);
  */

  // ok/cancel button
  m_ChooseOk = new mafGUIButton(m_ChooseDlg, ID_CHOOSE_OK, "OK", wxPoint(0,0), wxSize(50,20));
  m_ChooseOk->SetListener(this);
  m_ChooseOk->Enable(false);
  mafGUIButton *b_cancel = new mafGUIButton(m_ChooseDlg, ID_CHOOSE_CANCEL, "CANCEL", wxPoint(0,0), wxSize(50,20));
  wxBoxSizer *hs_b = new wxBoxSizer(wxHORIZONTAL);
  b_cancel->SetListener(this);
  hs_b->Add(m_ChooseOk,0);
  hs_b->Add(b_cancel,0);
  vs1->Add(hs_b,0,wxALIGN_CENTER | wxALL, 2);



  // put the vertical sizer into the dialog and display
  vs1->Fit(m_ChooseDlg);	  // fit the window to the min size of the sizer
  m_ChooseDlg->Add(vs1) ;  // plug the sizer into the dialog
  m_ChooseDlg->ShowModal();


  // dialog return code
  int returncode =  m_ChooseDlg->GetReturnCode();

  // destroy object
  wxDELETE(m_ChooseDlg);

  //
  if (returncode == wxID_OK) // ok button pressed
  {
    if (m_Surface)
    {
      return 1;
    }
    else
    {
      wxMessageBox("Not all parameters have been set up","alert",wxICON_WARNING);
      return 0;
    }
  }
  else // cancel button pressed
  {
    return 0;
  }
}



//----------------------------------------------------------------------------
void mafOpMML::CreateRegistrationDlg()
//----------------------------------------------------------------------------
{ 

  // create dialog
  wxString Title;
  Title = "registration of " + m_SurfaceName;
  m_OpDlg = new mafGUIDialog(Title); 


  // left/right vertical box sizers
  wxBoxSizer *LeftVerticalBoxSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *RightVerticalBoxSizer = new wxBoxSizer(wxVERTICAL);

  // registration window horizontal sizer
  wxBoxSizer *WindowHorizontalBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  WindowHorizontalBoxSizer->Add(LeftVerticalBoxSizer,1,wxEXPAND);
  WindowHorizontalBoxSizer->Add(RightVerticalBoxSizer,0,wxALL,0);


  // top horizontal box sizer
  wxBoxSizer *TopHorizontalBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  LeftVerticalBoxSizer->Add(TopHorizontalBoxSizer);

  // axes on/off button
  m_AxesOnOffButton = new mafGUIButton(m_OpDlg, ID_SHOW_AXES, "Axes Off", wxPoint(0,0), wxSize(75,20));
  m_AxesOnOffButton->SetListener(this);
  TopHorizontalBoxSizer->Add(m_AxesOnOffButton,0,wxALL, 5);

  // contour on/off buton
  m_ContourOnOffButton = new mafGUIButton(m_OpDlg, ID_SHOW_CONTOUR, "Contour Off", wxPoint(0,0), wxSize(75,20));
  m_ContourOnOffButton->SetListener(this);
  TopHorizontalBoxSizer->Add(m_ContourOnOffButton,0,wxALL, 5);

  // reset view button
  m_ResetViewButton = new mafGUIButton(m_OpDlg, ID_RESET_VIEW, "Reset View", wxPoint(0,0), wxSize(75,20));
  m_ResetViewButton->SetListener(this);
  TopHorizontalBoxSizer->Add(m_ResetViewButton,0,wxALL, 5);

  // ok button
  m_OkButton = new mafGUIButton(m_OpDlg, ID_OK, "OK", wxPoint(0,0), wxSize(75,20));
  m_OkButton->SetListener(this);
  TopHorizontalBoxSizer->Add(m_OkButton,0,wxALL, 5);

  // cancel button
  m_CancelButton = new mafGUIButton(m_OpDlg, ID_CANCEL, "CANCEL", wxPoint(0,0), wxSize(75,20));
  m_CancelButton->SetListener(this);
  TopHorizontalBoxSizer->Add(m_CancelButton,0, wxALL, 5);

  // button background colour
  m_ButtonBackgroundColour = m_CancelButton->GetBackgroundColour();

  // maf check boxes
  //wxBoxSizer *CheckBoxHorizontalBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  //wxCheckBox *c0 = new wxCheckBox(m_op_dlg,ID_SHOW_AXES,"show axes");
  //c0->SetValidator( mafGUIValidator(this,ID_SHOW_AXES,c0,&m_show_axes) );
  //CheckBoxHorizontalBoxSizer->Add(c0,0,wxALL, 5);
  //wxCheckBox *c1 = new wxCheckBox(m_op_dlg,ID_SHOW_CONTOUR,"show contour");
  //c1->SetValidator( mafGUIValidator(this,ID_SHOW_CONTOUR,c1,&m_ContourVisibility));
  //CheckBoxHorizontalBoxSizer->Add(c1,0,wxALL, 5);
  //LeftVerticalBoxSizer->Add(CheckBoxHorizontalBoxSizer);


  // box sizer
  wxBoxSizer *OperationHorizontalBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  LeftVerticalBoxSizer->Add(OperationHorizontalBoxSizer);

  // create p/t/r/s operation buttons
  m_PlaceOpButton = new mafGUIButton(m_OpDlg, ID_P_OPERATION, "P", wxPoint(0,0), wxSize(75,20));
  m_PlaceOpButton->SetListener(this);
  OperationHorizontalBoxSizer->Add(m_PlaceOpButton,0,wxALL, 5);

  m_TranslateOpButton = new mafGUIButton(m_OpDlg, ID_T_OPERATION,"T", wxPoint(0,0), wxSize(75,20));
  m_TranslateOpButton->SetListener(this);
  OperationHorizontalBoxSizer->Add(m_TranslateOpButton,0,wxALL, 5);

  m_RotateOpButton = new mafGUIButton(m_OpDlg, ID_R_OPERATION, "R", wxPoint(0,0), wxSize(75,20));
  m_RotateOpButton->SetListener(this);
  OperationHorizontalBoxSizer->Add(m_RotateOpButton,0,wxALL, 5);

  m_ScaleOpButton = new mafGUIButton(m_OpDlg, ID_S_OPERATION, "S", wxPoint(0,0), wxSize(75,20));
  m_ScaleOpButton->SetListener(this);
  OperationHorizontalBoxSizer->Add(m_ScaleOpButton,0,wxALL, 5);

  // undo button
  m_UndoButton = new mafGUIButton(m_OpDlg, ID_UNDO, "Undo", wxPoint(0,0), wxSize(75,20));
  m_UndoButton->SetListener(this);
  OperationHorizontalBoxSizer->Add(m_UndoButton,0,wxALL, 5);

  //wxString choices[4];
  //choices[0] = "PLACE       >>>";
  //choices[1] = "TRANSLATE   >>>";
  //choices[2] = "ROTATE     >>>";
  //choices[3] = "SCALE";
  //m_radio = new wxRadioBox(m_op_dlg, ID_STATE, "", wxPoint(0,0), wxSize(500,40), 4,choices,4,wxRA_SPECIFY_COLS|wxNO_BORDER|wxTAB_TRAVERSAL);
  //m_radio->SetValidator( mafGUIValidator(this,ID_STATE,m_radio,&m_state) );
  //if (m_3dflag == 0)
  //	m_radio->Enable(TRUE);
  //LeftVerticalBoxSizer->Add(m_radio,0,wxLEFT,5);


  // maf model view RWI
  m_ModelmafRWI = new mafRWI(m_OpDlg);
  m_ModelmafRWI->SetListener(this);
  m_ModelmafRWI->m_RwiBase->SetInteractorStyle(NULL);

  // width/height
  m_ModelmafRWI->SetSize(0,0,420,420);
  //((vtkRenderWindow*)(m_ModelmafRWI->m_RenderWindow))->SetSize(420,420);
  //m_ModelmafRWI->m_RwiBase->Show();

  m_ModelmafRWI->Show(true);
  m_ModelmafRWI->CameraUpdate();
  LeftVerticalBoxSizer->Add(m_ModelmafRWI->m_RwiBase, 1, wxEXPAND | wxALL, 5);

  // maf lut
  m_Lut = new mafGUILutSlider(m_OpDlg,-1,wxPoint(0,0),wxSize(420,24));
  m_Lut->SetListener(this);
  LeftVerticalBoxSizer->Add(m_Lut,0,wxEXPAND ,6);

  // get maf gray volume 
  m_Vol = mafVMEVolumeGray::SafeDownCast(m_Input);
  assert(!(m_Vol == NULL));

  /*vtkTransform *t = vtkTransform::New();
  t->SetMatrix(m_Vol->GetAbsMatrix());

  for(int i = 0; i < 4; i++)
  for(int j = 0; j < 4; j++)
  {
  double element = m_Vol->GetMatrix()->GetElement(i, j);
  }*/

  /*vtkTransformFilter *tf = vtkTransformFilter::New();
  tf->SetInput(vtkRectilinearGrid::SafeDownCast(m_Vol->GetCurrentData()));
  tf->SetTransform(t);
  tf->Update();
  m_Volume = tf->GetOutput();*/


  // get vtk rectilinear grid 
  m_Volume = m_Vol->GetOutput()->GetVTKData();
  assert(!(m_Volume == NULL));

  // polydata muscle should be known already from OnMuscleSelection()
  assert(!(m_Muscle == NULL));

  //
  // mml model view
  // create
  m_Model = new mafOpMMLModelView(m_ModelmafRWI->m_RenderWindow, m_ModelmafRWI->m_RenFront, m_Muscle, m_Volume);
  // initialise
  CreateFakeLandmarks();
  SetUpInputs();

  // maf slider widget 
  wxStaticText *lab  = new wxStaticText(m_OpDlg, -1, "Slice", wxPoint(0,0), wxSize(-1, 16));
  wxTextCtrl   *text = new wxTextCtrl  (m_OpDlg, ID_SLICE, "hello",  wxPoint(0,0), wxSize(20, 16),wxNO_BORDER |wxTE_READONLY );
  wxBoxSizer *hs4 = new wxBoxSizer(wxHORIZONTAL);
  hs4->Add(lab,  0, wxLEFT, 85);
  hs4->Add(text, 0, wxLEFT, 10);
  RightVerticalBoxSizer->Add(hs4, 0, wxTOP, 25);

  wxSlider *sli  = new wxSlider(m_OpDlg, ID_SLICE,0, 1, m_Model->GetTotalNumberOfSyntheticScans(), wxPoint(0,0), wxSize(269,-1));
  sli->SetValidator(mafGUIValidator(this,ID_SLICE,(wxSlider*)sli,&m_Slice,text));
  //RightVerticalBoxSizer->Add(sli,0);
  RightVerticalBoxSizer->Add(sli, 0, wxLEFT, -9);

  // maf parameter views RWIs
  m_PHmafRWI = CreateParameterViewmafRWI("P-H", 1,1,1); // white
  m_PVmafRWI = CreateParameterViewmafRWI("P-V", 1,1,1); // white
  m_THmafRWI = CreateParameterViewmafRWI("T-H", 1,1,1); // white
  m_TVmafRWI = CreateParameterViewmafRWI("T-V", 1,1,1); // white
  m_RAmafRWI = CreateParameterViewmafRWI("R-A", 1,1,1); // white
  m_SNmafRWI = CreateParameterViewmafRWI("S-N", 1,0,0); // red
  m_SSmafRWI = CreateParameterViewmafRWI("S-S", 0,1,0); // green
  m_SEmafRWI = CreateParameterViewmafRWI("S-E", 0,0,1); // blue
  m_SWmafRWI = CreateParameterViewmafRWI("S-W", 1,0,1); // magenta

  // add to box sizer
  RightVerticalBoxSizer->Add(m_PHmafRWI->m_RwiBase, 0, wxTOP, 10);
  RightVerticalBoxSizer->Add(m_PVmafRWI->m_RwiBase, 0, wxTOP, 3);
  RightVerticalBoxSizer->Add(m_THmafRWI->m_RwiBase, 0, wxTOP, 3);
  RightVerticalBoxSizer->Add(m_TVmafRWI->m_RwiBase, 0, wxTOP, 3);
  RightVerticalBoxSizer->Add(m_RAmafRWI->m_RwiBase, 0, wxTOP, 3);
  RightVerticalBoxSizer->Add(m_SNmafRWI->m_RwiBase, 0, wxTOP, 3);
  RightVerticalBoxSizer->Add(m_SSmafRWI->m_RwiBase, 0, wxTOP, 3);
  RightVerticalBoxSizer->Add(m_SEmafRWI->m_RwiBase, 0, wxTOP, 3);
  RightVerticalBoxSizer->Add(m_SWmafRWI->m_RwiBase, 0, wxTOP, 3);

  //
  m_OpDlg->Add(WindowHorizontalBoxSizer);    
  WindowHorizontalBoxSizer->Fit(m_OpDlg);

  //
  // mml parameter views
  // create
  m_PH = new mafOpMMLParameterView(m_PHmafRWI->m_RenderWindow, m_PHmafRWI->m_RenFront); 
  m_PV = new mafOpMMLParameterView(m_PVmafRWI->m_RenderWindow, m_PVmafRWI->m_RenFront);
  m_TH = new mafOpMMLParameterView(m_THmafRWI->m_RenderWindow, m_THmafRWI->m_RenFront);
  m_TV = new mafOpMMLParameterView(m_TVmafRWI->m_RenderWindow, m_TVmafRWI->m_RenFront);
  m_RA = new mafOpMMLParameterView(m_RAmafRWI->m_RenderWindow, m_RAmafRWI->m_RenFront);
  m_SN = new mafOpMMLParameterView(m_SNmafRWI->m_RenderWindow, m_SNmafRWI->m_RenFront);
  m_SS = new mafOpMMLParameterView(m_SSmafRWI->m_RenderWindow, m_SSmafRWI->m_RenFront);
  m_SE = new mafOpMMLParameterView(m_SEmafRWI->m_RenderWindow, m_SEmafRWI->m_RenFront);
  m_SW = new mafOpMMLParameterView(m_SWmafRWI->m_RenderWindow, m_SWmafRWI->m_RenFront);


  // mml contour widget
  // create
  m_Widget = mafOpMMLContourWidget::New();

  //
  SetUpWidget();
  SetUpModelView();
  SetUpParameterViews();
  //
  mafEventMacro(mafEvent(this, ID_T_OPERATION));
  //

  m_OpDlg->ShowModal();	
}

//----------------------------------------------------------------------------
bool mafOpMML::SetUpInputs()
//----------------------------------------------------------------------------
{
  // set muscle type (1 - one slicing axis, 2 - two slicing axes)
  m_Model->SetTypeOfMuscles(m_MuscleType);
  /*

  // set atlas landmarks
  Model->SetLandmark1OfAtlas(m_l1_point); //high
  Model->SetLandmark2OfAtlas(m_l2_point); //low
  Model->SetLandmark3OfAtlas(m_l3_point);
  Model->SetLandmark4OfAtlas(m_l4_point);

  // set patient landmarks
  Model->SetLandmark1OfPatient(m_p1); //high
  Model->SetLandmark2OfPatient(m_p2); //low
  Model->SetLandmark3OfPatient(m_p3);
  Model->SetLandmark4OfPatient(m_p4);
  */

  // landmark flag
  m_Model->Set4LandmarksFlag(m_LandmarksFlag);

  // set x, y scaling factors
  m_Model->SetXYScalingFactorsOfMuscle(m_RegistrationXYScalingFactor, m_RegistrationXYScalingFactor);

  //// set number of scans based on scans distance
  //double length; // length between scans insertions
  //length = sqrt(pow(m_p2[2] - m_p1[2], 2.0) +
  //			  pow(m_p2[1] - m_p1[1], 2.0) +
  //			  pow(m_p2[0] - m_p1[0], 2.0));
  //Model->SetTotalNumberOfSyntheticScans((int) (length / m_ScansDistance));

  // set number of scans
  m_Model->SetTotalNumberOfSyntheticScans(m_ScansNumber);

  // set x, y, scans size
  m_Model->SetSizeOfSyntheticScans(m_ScansSize[0], m_ScansSize[1]);

  // set x, y scans resolution
  m_Model->SetResolutionOfSyntheticScans(2.0 * m_ScansSize[0], 2.0 * m_ScansSize[1]/*m_ScansResolution[0], m_ScansResolution[1]*/);

  // set scans grain
  m_Model->SetGrainOfScans(m_ScansGrain);

  // landmarks
  //double la1[3]; // low
  //double la2[3]; // high
  //double la3[3]; // ref
  //Model->GetLandmark1OfAtlas(la1);
  //Model->GetLandmark2OfAtlas(la2);
  //Model->GetLandmark3OfAtlas(la3);

  //double lp1[3]; // low
  //double lp2[3]; // high
  //double lp3[3]; // ref
  //Model->GetLandmark1OfPatient(lp1);
  //Model->GetLandmark2OfPatient(lp2);
  //Model->GetLandmark3OfPatient(lp3);

  //double factorX, factorY;
  //Model->GetXYScalingFactorsOfMuscle(&factorX, &factorY);

  //int n;
  //n = Model->GetTotalNumberOfSyntheticScans();
  //
  //int rx, ry;
  //Model->GetResolutionOfSyntheticScans(&rx, &ry);
  //
  //float sx, sy;
  //Model->GetSizeOfSyntheticScans(&sx, &sy);

  return 1;
}

//----------------------------------------------------------------------------
bool mafOpMML::SetUpParameterViews() 
//----------------------------------------------------------------------------
{
  // size of synthetic scans
  float sx, sy;
  m_Model->GetSizeOfSyntheticScans(&sx, &sy);

  // ph spline
  m_PH->SetRangeX(m_Model->GetTotalNumberOfSyntheticScans());
  m_PH->SetRangeY(-1.0 * (sx + sy)/ 3.0, 0.0, (sx + sy) / 3.0); // average x 2
  // pv spline																											   
  m_PV->SetRangeX(m_Model->GetTotalNumberOfSyntheticScans());
  m_PV->SetRangeY(-1.0 * (sx + sy)/ 3.0, 0.0, (sx + sy) / 3.0); // average x 2
  // th spline
  m_TH->SetRangeX(m_Model->GetTotalNumberOfSyntheticScans());
  m_TH->SetRangeY(-1.0 * (sx + sy)/ 3.0, 0.0, (sx + sy) / 3.0); // average x 2
  // tv spline
  m_TV->SetRangeX(m_Model->GetTotalNumberOfSyntheticScans());
  m_TV->SetRangeY(-1.0 * (sx + sy)/ 3.0, 0.0, (sx + sy) / 3.0); // average x 2
  // ra spline
  m_RA->SetRangeX(m_Model->GetTotalNumberOfSyntheticScans());
  m_RA->SetRangeY(-70.0, 0.0, 70.0);
  // sn spline
  m_SN->SetRangeX(m_Model->GetTotalNumberOfSyntheticScans());
  m_SN->SetRangeY(0.1, 1.0, 3.0);
  // ss spline
  m_SS->SetRangeX(m_Model->GetTotalNumberOfSyntheticScans());
  m_SS->SetRangeY(0.1, 1.0, 3.0);
  // se spline
  m_SE->SetRangeX(m_Model->GetTotalNumberOfSyntheticScans());
  m_SE->SetRangeY(0.1, 1.0, 3.0);
  // sw spline
  m_SW->SetRangeX(m_Model->GetTotalNumberOfSyntheticScans());
  m_SW->SetRangeY(0.1, 1.0, 3.0);

  // display values
  m_PH->AddPoint(0, 0.0); 
  m_PH->AddPoint(m_Model->GetTotalNumberOfSyntheticScans() - 1, 0.0);
  // actual values
  m_Model->GetPHSpline()->AddPoint(m_Model->GetCurrentZOfSyntheticScans(), 0.0); // first slice, PH = 0
  m_Model->GetPHSpline()->AddPoint(m_Model->GetZOfSyntheticScans(m_Model->GetTotalNumberOfSyntheticScans() - 1), 0.0); // last slice, PH = 0
  // display values
  m_PV->AddPoint(0, 0.0); // first slice, PV = 0
  m_PV->AddPoint(m_Model->GetTotalNumberOfSyntheticScans() - 1, 0.0); // last slice, PV = 0
  // actual values
  m_Model->GetPVSpline()->AddPoint(m_Model->GetCurrentZOfSyntheticScans(), 0.0); // first slice - PV = 0
  m_Model->GetPVSpline()->AddPoint(m_Model->GetZOfSyntheticScans(m_Model->GetTotalNumberOfSyntheticScans() - 1), 0.0); // last slice, PV = 0
  // display values
  m_TH->AddPoint(0, 0.0); // first slice, TH = 0
  m_TH->AddPoint(m_Model->GetTotalNumberOfSyntheticScans() - 1, 0.0); // last slice, TH = 0
  // actual values
  m_Model->GetTHSpline()->AddPoint(m_Model->GetCurrentZOfSyntheticScans(), 0.0); // first slice - H = 0
  m_Model->GetTHSpline()->AddPoint(m_Model->GetZOfSyntheticScans(m_Model->GetTotalNumberOfSyntheticScans() - 1), 0.0); // last slice - H = 0
  // display values
  m_TV->AddPoint(0, 0.0); // first slice, V = 0
  m_TV->AddPoint(m_Model->GetTotalNumberOfSyntheticScans() - 1, 0.0); // last slice, TV = 0
  // actual values
  m_Model->GetTVSpline()->AddPoint(m_Model->GetCurrentZOfSyntheticScans(), 0.0); // first slice - V = 0
  m_Model->GetTVSpline()->AddPoint(m_Model->GetZOfSyntheticScans(m_Model->GetTotalNumberOfSyntheticScans() - 1), 0.0); // last slice, TV = 0
  // display values
  m_RA->AddPoint(0, 0.0); // first slice, TA = 0
  m_RA->AddPoint(m_Model->GetTotalNumberOfSyntheticScans() - 1, 0.0); // last slice, RA = 0
  // actual values
  m_Model->GetRASpline()->AddPoint(m_Model->GetCurrentZOfSyntheticScans(), 0.0); // first slice, TA = 0
  m_Model->GetRASpline()->AddPoint(m_Model->GetZOfSyntheticScans(m_Model->GetTotalNumberOfSyntheticScans() - 1), 0.0); // last slice, RA = 0
  // display values
  m_SN->AddPoint(0, 1.0); // first slice, SN = 1.0
  m_SN->AddPoint(m_Model->GetTotalNumberOfSyntheticScans() - 1, 1.0); // last slice, SN = 1.0
  // actual values
  m_Model->GetSNSpline()->AddPoint(m_Model->GetCurrentZOfSyntheticScans(), 1.0); // first slice, SN = 1.0
  m_Model->GetSNSpline()->AddPoint(m_Model->GetZOfSyntheticScans(m_Model->GetTotalNumberOfSyntheticScans() - 1), 1.0); // last slice, SN = 1.0
  // display values
  m_SS->AddPoint(0, 1.0); // first slice, SS = 1.0
  m_SS->AddPoint(m_Model->GetTotalNumberOfSyntheticScans() - 1, 1.0); // last slice, SS = 1.0
  // actual values
  m_Model->GetSSSpline()->AddPoint(m_Model->GetCurrentZOfSyntheticScans(), 1.0); // first slice, SS = 1.0
  m_Model->GetSSSpline()->AddPoint(m_Model->GetZOfSyntheticScans(m_Model->GetTotalNumberOfSyntheticScans() - 1), 1.0); // last slice, SS = 1.0
  // display values
  m_SE->AddPoint(0, 1.0); // first slice, SE = 1.0
  m_SE->AddPoint(m_Model->GetTotalNumberOfSyntheticScans() - 1, 1.0); // last slice, SE = 1.0
  // actual values
  m_Model->GetSESpline()->AddPoint(m_Model->GetCurrentZOfSyntheticScans(), 1.0); // first slice, SE = 1.0
  m_Model->GetSESpline()->AddPoint(m_Model->GetZOfSyntheticScans(m_Model->GetTotalNumberOfSyntheticScans() - 1), 1.0); // last slice, SE = 1.0
  // display values
  m_SW->AddPoint(0, 1.0); // first slice, SW = 1.0
  m_SW->AddPoint(m_Model->GetTotalNumberOfSyntheticScans() - 1, 1.0); // last slice, SW = 1.0
  // actual values
  m_Model->GetSWSpline()->AddPoint(m_Model->GetCurrentZOfSyntheticScans(), 1.0); // first slice, SW = 1.0
  m_Model->GetSWSpline()->AddPoint(m_Model->GetZOfSyntheticScans(m_Model->GetTotalNumberOfSyntheticScans() - 1), 1.0); // last slice, SW = 1.0

  // apply operations
  if (m_RegistrationStatus == 1)
  {

    int NumberOfOperations;
    NumberOfOperations = m_SliceIdStackTag->GetNumberOfComponents();

    int MaxScanId;
    MaxScanId = m_Model->GetTotalNumberOfSyntheticScans() - 1;

    int NextOperationId;
    NextOperationId = m_Widget->GetNextOperationId();

    // get operations
    double params[5];
    for(int i = 0; i < NumberOfOperations; i++)
    {
      // exceeded max ?
      assert(NextOperationId < 2000);

      // values
      params[0] = m_SliceIdStackTag->GetComponentAsDouble(i);
      params[1] = m_ZValueStackTag->GetComponentAsDouble(i);
      params[2] = m_OperationTypeStackTag->GetComponentAsDouble(i);
      params[3] = m_Parameter1StackTag->GetComponentAsDouble(i);
      params[4] = m_Parameter2StackTag->GetComponentAsDouble(i);

      // built operations stack
      m_Model->m_OperationsStack->SetTuple(NextOperationId, params);

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
        m_Model->m_ScalingOccured = TRUE;
        m_Model->m_ScalingOccuredOperationId = NextOperationId;

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

//----------------------------------------------------------------------------
bool mafOpMML::SetUpWidget()
//----------------------------------------------------------------------------
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
  //Widget->CenterModeOn();
  m_Widget->On();

  return 1;
}

//----------------------------------------------------------------------------
bool mafOpMML::SetUpModelView() 
//----------------------------------------------------------------------------
{
  // pre-process muscle
  m_Model->FindUnitVectorsAndLengthsOfLandmarkLines();
  m_Model->MapAtlasToPatient();
  m_Model->MakeActionLineZAxis();

  // pre-process scans
  m_Model->FindSizeAndResolutionOfSyntheticScans();
  m_Model->SetUpSyntheticScans();

  // initalise lut slider (called after void mafOpMMLModelView::SetUpSyntheticScans()
  m_Lut->SetRange(m_Model->GetLowScalar(), m_Model->GetHighScalar());
  m_Lut->SetSubRange(m_Model->GetSyntheticScansLevel() - 0.5 * m_Model->GetSyntheticScansWindow(),
    m_Model->GetSyntheticScansLevel() + 0.5 * m_Model->GetSyntheticScansWindow());
  //m_lut->SetText(0, "");
  //m_lut->SetText(1, ""); 
  //m_lut->SetText(2, "");

  // sub-contours actors (switch on)
  m_Model->GetNEContourActor()->VisibilityOn();
  m_Model->GetNWContourActor()->VisibilityOn();
  m_Model->GetSEContourActor()->VisibilityOn();
  m_Model->GetSWContourActor()->VisibilityOn();

  // contour axes
  m_Model->SetUpContourCoordinateAxes();

  // global axes
  m_Model->SetUpGlobalCoordinateAxes();

  // set up display information
  m_Model->GetTextMapper1()->GetTextProperty()->SetFontSize(10);
  m_Model->GetTextMapper2()->GetTextProperty()->SetFontSize(10);
  m_Model->GetTextMapper1()->GetTextProperty()->SetFontFamilyToCourier();
  m_Model->GetTextMapper2()->GetTextProperty()->SetFontFamilyToCourier();
  m_Model->GetScaledTextActor1()->GetProperty()->SetColor(1.0, 1.0, 1.0);
  m_Model->GetScaledTextActor2()->GetProperty()->SetColor(1.0, 1.0, 1.0);
  m_Model->GetScaledTextActor1()->GetPositionCoordinate()->SetValue(0.0, 0.8);
  m_Model->GetScaledTextActor2()->GetPositionCoordinate()->SetValue(0.0, 0.9);

  // 3d display?
  if (m_Flag3D == 1)
  {
    //
    m_Widget->Off();

    //
    m_Model->Switch3dDisplayOn();
  }

  //
  Update();

  // initialise
  m_Model->GetRenderer()->ResetCamera();

  return 1;
}

//----------------------------------------------------------------------------
mafRWI* mafOpMML::CreateParameterViewmafRWI(wxString lab, float r, float g, float b)
//----------------------------------------------------------------------------
{ 
  mafRWI *rwi = new mafRWI(m_OpDlg);
  rwi->SetListener(this);
  rwi->m_RwiBase->SetInteractorStyle(NULL);

  // width/height
  rwi->SetSize(0, 0, 250, 65);
  ((vtkRenderWindow*)(rwi->m_RenderWindow))->SetSize(250, 65);
  rwi->m_RwiBase->Show();

  //vtkDEL(rwi->m_c);
  //rwi->m_c = m_RenderWindow_camera;

  vtkTextSource *ts = vtkTextSource::New();
  ts->SetForegroundColor(r,g,b);
  ts->BackingOff(); // no background drawn with text
  ts->SetText(lab.c_str());

  vtkPolyDataMapper2D *mapper = vtkPolyDataMapper2D::New();
  mapper->SetInput(ts->GetOutput());

  vtkActor2D *labactor = vtkActor2D::New();
  labactor->SetMapper(mapper);

  rwi->m_RenFront->AddActor(labactor);

  return rwi;
}

//----------------------------------------------------------------------------
void mafOpMML::SetUpLandmarks(wxString AtlasSectionVMEName, wxString PatientSectionVMEName)
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
  if (m_SurfaceName.compare("obturator externus") == 0)
  {
    // landmarks
    Landmark1VMEName = "Superior edge of lateral surface of the Right ischial ramus at juncture with inferior pubic ramus ";
    Landmark2VMEName = "bottom of inferior cavity in trochanteric fossa";
    Landmark3VMEName = "midpoint of ridge between lesser trochanter and femoral head";
    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
    a = 1.25; // 1.20
    b = 0.10; // 0.14
    m_MuscleType = 1;
  }
  else
    if (m_SurfaceName.compare("gamellus inferior") == 0)
    {
      // landmarks
      Landmark1VMEName = "Most superior point on posterior surface of Right ischial tuberosity at height of lesser sciatic notch ";
      Landmark2VMEName = "bottom of superior cavity in trochanteric fossa";
      Landmark3VMEName = "Lateral surface of the Right ilium at midpoint between most lateral point on superior tubercle of iliac crest (pt. 39) and origin of Gluteus Medius posterior fibers (pt. 41).   ";
      Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
      a = 1.15; // 1.07
      b = 0.20; // 0.28
      m_MuscleType = 1;
    }
    else
      if (m_SurfaceName.compare("gamellus superior") == 0)
      {
        // landmarks
        Landmark1VMEName = "Most superior point on posterior surface of Right ischial tuberosity at height of lesser sciatic notch ";
        Landmark2VMEName = "bottom of superior cavity in trochanteric fossa";
        Landmark3VMEName = "Lateral surface of the Right ilium at midpoint between most lateral point on superior tubercle of iliac crest (pt. 39) and origin of Gluteus Medius posterior fibers (pt. 41).   ";
        Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
        a = 1.65; // 1.60
        b = 0.25; // 0.32
        m_MuscleType = 1;
      }
      else
        if (m_SurfaceName.compare("piriformis") == 0)
        {
          // landmarks
          Landmark1VMEName = "Anterior surface of the Right sacrum between second and third foramen midway between line of foramen and lateral border (first sacral foramen is most superior)";
          Landmark2VMEName = "most anterior superior point on greater trochanter";
          Landmark3VMEName = "Superior edge of lateral surface of the Right ischial ramus at juncture with inferior pubic ramus ";
          Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
          a = 1.20; // 1.14
          b = 0.20; // 0.27
          m_MuscleType = 1;
        }
        else
          if (m_SurfaceName.compare("gluteus minimus") == 0)
          {
            // landmarks
            Landmark1VMEName = "Top of ridge on lateral surface of the Right ilium between superior tubercle of iliac crest (pt. 39) and acetabulum (pt. 5) along a line from anterior superior iliac spine ASIS (pt. 1) to posterior superior iliac spine PSIS (pt. 3) ";
            Landmark2VMEName = "center of depression on anterior surface of greater trochanter";
            Landmark3VMEName = "Bottom of posterior depression on lateral surface of the Right ilium along a line from anterior superior iliac spine ASIS (pt. 1) to posterior superior iliac spine PSIS (pt. 3) ";
            Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
            a = 1.30; // 1.25
            b = -0.05;// 0.00
            m_MuscleType = 1;
          }
          else
            if (m_SurfaceName.compare("tensor fasciae latae") == 0)
            {
              // landmarks
              Landmark1VMEName = "Midpoint along Right lateral superior ridge of iliac crest between the anterior superior iliac spine ASIS and most lateral point on superior tubercle of the iliac crest ";
              Landmark2VMEName = "impression for iliotibial tract on lateral anterior surface of superior end of tibia at height of fibular facet just below epiphysial line";
              Landmark3VMEName = "Superior edge of lateral surface of the Right ischial ramus at juncture with inferior pubic ramus ";
              Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
              a = 1.00; // 0.95
              b = 0.60; // 0.69
              m_MuscleType = 1;
            }
            else
              if (m_SurfaceName.compare("adductor longus") == 0)
              {
                // landmarks
                Landmark1VMEName = "Anterior corner of inferior surface of the Right pubic tubercle ";
                Landmark2VMEName = "on linea aspera at midpoint between juncture of pectineal/gluteal lines and juncture of medial/lateral supracondylar lines";
                Landmark3VMEName = "anterior medial corner of femoral shaft cross-section directly superior to medial border of patellar groove joins at height of juncture of medial supracondylar line and adductor tubercle ridge ";
                Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
                a = 1.00; // 0.92
                b = -0.20;// -0.16
                m_MuscleType = 1;
              }
              else
                if (m_SurfaceName.compare("adductor brevis") == 0)
                {
                  // landmarks
                  Landmark1VMEName = "Midpoint of lateral surface of the Right inferior pubic ramus ";
                  Landmark2VMEName = "on linea aspera at juncture of pectineal/gluteal lines";
                  Landmark3VMEName = "most medial corner of femoral shaft cross-section at height of midpoint between juncture of pectineal/gluteal lines and juncture of medial/lateral supracondylar lines";
                  Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
                  a = 1.15; // 1.09
                  b = -0.20;// -0.14
                  m_MuscleType = 1;
                }
                else
                  if (m_SurfaceName.compare("rectus femoris") == 0)
                  {
                    // landmarks
                    Landmark1VMEName = "Right Anterior Inferior Iliac Spine ";
                    Landmark2VMEName = "Midside point on superior patella surface";
                    Landmark3VMEName = "Superior edge of lateral surface of the Right ischial ramus at juncture with inferior pubic ramus ";
                    Landmark4VMEName = "most anterior point on anterior surface of femoral shaft at height of juncture of pectineal/gluteal lines";
                    a = 1.05; // 0.97
                    b = 0.15; // 0.19
                    m_MuscleType = 1;
                  }
                  else
                    if (m_SurfaceName.compare("semitendinous") == 0)
                    {
                      // landmarks
                      Landmark1VMEName = "Posterior most point on medial border of posterior surface of the Right ischial tuberosity";
                      Landmark2VMEName = "medial corner near back of shaft inferior to base of tibial tubercle - near pes anserine bursa";
                      Landmark3VMEName = "most medial point on medial epicondyle";
                      Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
                      a = 1.05; // 0.98
                      b = 0.25; // 0.32
                      m_MuscleType = 1;
                    }
                    else
                      if (m_SurfaceName.compare("gluteus maximus") == 0)
                      {
                        // landmarks
                        Landmark1VMEName = "Lateral corner of Right sacral-coccygeal facet  ";
                        Landmark2VMEName = "on gluteal line at height of inferior base of lesser trochanter";
                        Landmark3VMEName = "Right Posterior Superior Iliac Spine PSIS (repeat of pt. 3) ";
                        Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
                        a = 1.25; // 1.18
                        b = -0.20;// -0.15
                        m_MuscleType = 1;
                      }
                      else
                        if (m_SurfaceName.compare("gluteus medius") == 0)
                        {
                          // landmarks
                          Landmark1VMEName = "Lateral surface of the Right ilium at midpoint between most lateral point on superior tubercle of iliac crest (pt. 39) and origin of Gluteus Medius posterior fibers (pt. 41).   ";
                          Landmark2VMEName = "lateral posterior prominence of greater trochanter";
                          Landmark3VMEName = "Bottom of posterior depression on lateral surface of the Right ilium along a line from most lateral point on superior tubercle of iliac crest to posterior superior iliac spine PSIS  ";
                          Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
                          a = 1.30; // 1.25
                          b = -0.25;// -0.18
                          m_MuscleType = 1;
                        }
                        else
                          if (m_SurfaceName.compare("pectineus") == 0)
                          {
                            // landmarks
                            Landmark1VMEName = "Base of the Right superior pubic ramus on superior surface lateral to pectineal line ";
                            Landmark2VMEName = "inferior base of lesser trochanter on pectineal line";
                            Landmark3VMEName = "Superior edge of lateral surface of the Right ischial ramus at juncture with inferior pubic ramus ";
                            Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
                            a = 1.10; // 1.05
                            b = -0.15;// -0.09
                            m_MuscleType = 1;
                          }
                          else
                            if (m_SurfaceName.compare("adductor magnus") == 0)
                            {
                              // landmarks
                              Landmark1VMEName = "Most lateral aspect at posterior corner of lateral surface of Right ischial tuberosity ";
                              Landmark2VMEName = "superior corner of adductor tubercle above medial epicondyle";
                              Landmark3VMEName = "on linea aspera at midpoint between juncture of pectineal/gluteal lines and juncture of medial/lateral supracondylar lines";
                              Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
                              a = 1.10; // 1.05
                              b = 0.10; // 0.15
                              m_MuscleType = 1;
                            }
                            else
                              if (m_SurfaceName.compare("quadratus femoris") == 0)
                              {
                                // landmarks
                                Landmark1VMEName = "On lateral surface of Right ischial tuberosity at midpoint between obturator foramen and lateral posterior corner ";
                                Landmark2VMEName = "quadrate tubercle on intertrochanteric crest";
                                Landmark3VMEName = "Lateral surface of the Right ilium at midpoint between most lateral point on superior tubercle of iliac crest (pt. 39) and origin of Gluteus Medius posterior fibers (pt. 41).   ";
                                Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
                                a = 1.05; // 1.00
                                b = -0.20;// -0.14
                                m_MuscleType = 1;
                              }
                              else
                                if (m_SurfaceName.compare("sartorius") == 0)
                                {
                                  // landmarks
                                  Landmark1VMEName = "Right Anterior Superior Iliac Spine (repeat of pt. 1) ";
                                  Landmark2VMEName = "most medial point on medial epicondyle";
                                  Landmark3VMEName = "Superior edge of lateral surface of the Right ischial ramus at juncture with inferior pubic ramus ";
                                  Landmark4VMEName = "most anterior point on anterior surface of femoral shaft at height of juncture of pectineal/gluteal lines";
                                  a = 1.05; // 1.00
                                  b = -0.15;// -0.10
                                  m_MuscleType = 1;
                                }
                                else
                                  if (m_SurfaceName.compare("vastus medialis") == 0)
                                  {
                                    // landmarks
                                    Landmark1VMEName = "most medial corner of femoral shaft cross-section at height of midpoint between juncture of pectineal/gluteal lines and juncture of medial/lateral supracondylar lines";
                                    Landmark2VMEName = "anterior medial corner of femoral shaft cross-section directly superior to medial border of patellar groove joins at height of juncture of medial supracondylar line and adductor tubercle ridge ";
                                    Landmark3VMEName = "Most medial point on superior patella surface";
                                    Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
                                    a = 1.85; // 1.80
                                    b = -0.30;// -0.25
                                    m_MuscleType = 1;
                                  }
                                  else
                                    if (m_SurfaceName.compare("semimembranous") == 0)
                                    {
                                      // landmarks
                                      Landmark1VMEName = "Bottom of superior depression on posterior surface of Right ischial tuberosity ";
                                      Landmark2VMEName = "superior lip of depression on posterior surface of medial tibial condyle";
                                      Landmark3VMEName = "superior medial corner of medial condyle";
                                      Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
                                      a = 0.80; // 0.74
                                      b = 0.00; // 0.06
                                      m_MuscleType = 1;
                                    }
                                    else
                                      if (m_SurfaceName.compare("obturator internal") == 0)
                                      {
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
                                      else
                                        if (m_SurfaceName.compare("long head of the biceps") == 0)
                                        {
                                          // landmarks
                                          Landmark1VMEName = "Posterior most point on medial border of posterior surface of the Right ischial tuberosity";
                                          Landmark2VMEName = "superior most point on fibula (apex, styloid process)";
                                          Landmark3VMEName = "superior medial corner of medial condyle";
                                          Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
                                          a = 1.00; // 0.94
                                          b = 0.15; // 0.20
                                          m_MuscleType = 1;
                                        }
                                        else
                                          if (m_SurfaceName.compare("short head of the biceps") == 0)
                                          {
                                            // landmarks
                                            Landmark1VMEName = "on linea aspera at midpoint between juncture of pectineal/gluteal lines and juncture of medial/lateral supracondylar lines";
                                            Landmark2VMEName = "superior most point on fibula (apex, styloid process)";
                                            Landmark3VMEName = "superior medial corner of medial condyle";
                                            Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
                                            a = 1.10; // 1.03
                                            b = 0.00; // 0.07
                                            m_MuscleType = 1;
                                          }
                                          else
                                            if (m_SurfaceName.compare("gracilis") == 0)
                                            {
                                              // landmarks
                                              Landmark1VMEName = "Inferior margin of lateral surface of right inferior pubic ramus ";
                                              Landmark2VMEName = "medial corner near back of shaft inferior to base of tibial tubercle - near pes anserine bursa";
                                              Landmark3VMEName = "on linea aspera at juncture of pectineal/gluteal lines";
                                              Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
                                              a = 1.05; // 0.97
                                              b = 0.25; // 0.30
                                              m_MuscleType = 1;
                                            }
                                            else
                                              if (m_SurfaceName.compare("vastus lateralis and intermedius") == 0)
                                              {
                                                // landmarks
                                                Landmark1VMEName = "most anterior point on anterior surface of femoral shaft at height of juncture of pectineal/gluteal lines";
                                                Landmark2VMEName = "Midside point on superior patella surface";
                                                Landmark3VMEName = "Superior edge of lateral surface of the Right ischial ramus at juncture with inferior pubic ramus ";
                                                Landmark4VMEName = "Right Anterior Inferior Iliac Spine ";
                                                a = 1.50; // 1.44
                                                b = -0.05;// 0.00
                                                m_MuscleType = 1;
                                              }
                                              else
                                              {
                                                wxMessageBox("Muscle not in built in dictionary!","alert",wxICON_WARNING);
                                                return;
                                              }

                                              // get root node
                                              mafVME *RootVME = mafVME::SafeDownCast(m_Input->GetRoot());

                                              // get atlas section node
                                              mafVME *AtlasSectionVME = (mafVME*)(RootVME->FindInTreeByName(AtlasSectionVMEName));
                                              if(AtlasSectionVME == NULL)
                                              {
                                                wxMessageBox("No section" + AtlasSectionVMEName,"alert",wxICON_WARNING);
                                                return;
                                              }

                                              // get patient section node
                                              mafVME *PatientSectionVME = (mafVME*)(RootVME->FindInTreeByName(PatientSectionVMEName));
                                              if(PatientSectionVME == NULL)
                                              {
                                                wxMessageBox("No section" + PatientSectionVMEName,"alert",wxICON_WARNING);
                                                return;
                                              }

                                              // atlas - landmark 1
                                              // get landmark node
                                              mafVME *Landmark1AtlasVME = (mafVME*)(AtlasSectionVME->FindInTreeByName(Landmark1VMEName));
                                              if(Landmark1AtlasVME == NULL)
                                              {
                                                wxMessageBox("Expected " + Landmark1VMEName + " in section " + AtlasSectionVMEName + " is missing!","alert",wxICON_WARNING);
                                              }
                                              else
                                              {
                                                // get landmark
                                                mafVMELandmark *Landmark1AtlasVMELandmark = mafVMELandmark::SafeDownCast(Landmark1AtlasVME);

                                                // get coordinates
                                                Landmark1AtlasVMELandmark->GetPoint(a1);

                                                // set name
                                                m_L1Name = Landmark1AtlasVMELandmark->GetName();
                                              }

                                              // atlas - landmark 2
                                              // get landmark node
                                              mafVME *Landmark2AtlasVME = (mafVME*)(AtlasSectionVME->FindInTreeByName(Landmark2VMEName));
                                              if(Landmark2AtlasVME == NULL)
                                              {
                                                wxMessageBox("Expected " + Landmark2VMEName + " in section " + AtlasSectionVMEName + " is missing!","alert",wxICON_WARNING);
                                              }
                                              else
                                              {
                                                // get landmark
                                                mafVMELandmark *Landmark2AtlasVMELandmark = mafVMELandmark::SafeDownCast(Landmark2AtlasVME);

                                                // get coordinates
                                                Landmark2AtlasVMELandmark->GetPoint(a2);

                                                // set name
                                                m_L2Name = Landmark2AtlasVMELandmark->GetName();
                                              }

                                              // atlas - landmark 3
                                              // get landmark node
                                              mafVME *Landmark3AtlasVME = (mafVME*)(AtlasSectionVME->FindInTreeByName(Landmark3VMEName));
                                              if(Landmark3AtlasVME == NULL)
                                              {
                                                wxMessageBox("Expected " + Landmark3VMEName + " in section " + AtlasSectionVMEName + " is missing!","alert",wxICON_WARNING);
                                              }
                                              else
                                              {
                                                // get landmark
                                                mafVMELandmark *Landmark3AtlasVMELandmark = mafVMELandmark::SafeDownCast(Landmark3AtlasVME);

                                                // get coordinates
                                                Landmark3AtlasVMELandmark->GetPoint(a3);

                                                // set name
                                                m_L3Name = Landmark3AtlasVMELandmark->GetName();
                                              }

                                              // atlas - landmark 4
                                              // get landmark node
                                              mafVME *Landmark4AtlasVME = (mafVME*)(AtlasSectionVME->FindInTreeByName(Landmark4VMEName));
                                              if(Landmark4AtlasVME == NULL)
                                              {
                                                wxMessageBox("Expected " + Landmark4VMEName + " in section " + AtlasSectionVMEName + " is missing!","alert",wxICON_WARNING);
                                              }
                                              else
                                              {
                                                // get landmark
                                                mafVMELandmark *Landmark4AtlasVMELandmark = mafVMELandmark::SafeDownCast(Landmark4AtlasVME);

                                                // get coordinates
                                                Landmark4AtlasVMELandmark->GetPoint(a4);

                                                // set name
                                                m_L4Name = Landmark4AtlasVMELandmark->GetName();
                                              }

                                              // patient - landmark 1
                                              // get landmark node
                                              mafVME *Landmark1PatientVME = (mafVME*)(PatientSectionVME->FindInTreeByName(Landmark1VMEName));
                                              if(Landmark1PatientVME == NULL)
                                              {
                                                wxMessageBox("Expected " + Landmark1VMEName + " in section " + PatientSectionVMEName + " is missing!","alert",wxICON_WARNING);
                                              }
                                              else
                                              {
                                                // get landmark
                                                mafVMELandmark *Landmark1PatientVMELandmark = mafVMELandmark::SafeDownCast(Landmark1PatientVME);

                                                // get coordinates
                                                Landmark1PatientVMELandmark->GetPoint(p1);

                                                // set name
                                                m_P1Name = Landmark1PatientVMELandmark->GetName();
                                              }

                                              // patient - landmark 2
                                              // get landmark node
                                              mafVME *Landmark2PatientVME = (mafVME*)(PatientSectionVME->FindInTreeByName(Landmark2VMEName));
                                              if(Landmark2PatientVME == NULL)
                                              {
                                                wxMessageBox("Expected " + Landmark2VMEName + " in section " + PatientSectionVMEName + " is missing!","alert",wxICON_WARNING);
                                              }
                                              else
                                              {
                                                // get landmark
                                                mafVMELandmark *Landmark2PatientVMELandmark = mafVMELandmark::SafeDownCast(Landmark2PatientVME);

                                                // get coordinates
                                                Landmark2PatientVMELandmark->GetPoint(p2);

                                                // set name
                                                m_P2Name = Landmark2PatientVMELandmark->GetName();
                                              }

                                              // patient - landmark 3
                                              // get landmark node
                                              mafVME *Landmark3PatientVME = (mafVME*)(PatientSectionVME->FindInTreeByName(Landmark3VMEName));
                                              if(Landmark3PatientVME == NULL)
                                              {
                                                wxMessageBox("Expected " + Landmark3VMEName + " in section " + PatientSectionVMEName + " is missing!","alert",wxICON_WARNING);
                                              }
                                              else
                                              {
                                                // get landmark
                                                mafVMELandmark *Landmark3PatientVMELandmark = mafVMELandmark::SafeDownCast(Landmark3PatientVME);

                                                // get coordinates
                                                Landmark3PatientVMELandmark->GetPoint(p3);

                                                // set name
                                                m_P3Name = Landmark3PatientVMELandmark->GetName();
                                              }

                                              // patient - landmark 4
                                              // get landmark node
                                              mafVME *Landmark4PatientVME = (mafVME*)(PatientSectionVME->FindInTreeByName(Landmark4VMEName));
                                              if(Landmark4PatientVME == NULL)
                                              {
                                                wxMessageBox("Expected " + Landmark4VMEName + " in section " + PatientSectionVMEName + " is missing!","alert",wxICON_WARNING);
                                              }
                                              else
                                              {
                                                // get landmark
                                                mafVMELandmark *Landmark4PatientVMELandmark = mafVMELandmark::SafeDownCast(Landmark4PatientVME);

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

                                                m_P1[i] = a * p1[i] + (1 - a) * p2[i];
                                                m_P2[i] = b * p1[i] + (1 - b) * p2[i];
                                              }

                                              // landmarks 3 / 4
                                              for(i = 0; i < 3; i++)
                                              {
                                                m_L3Point[i] = a3[i];
                                                m_L4Point[i] = a4[i];

                                                m_P3[i] = p3[i];
                                                m_P4[i] = p4[i];
                                              }
}

//----------------------------------------------------------------------------
void mafOpMML::Update()
//----------------------------------------------------------------------------
{
  // update sectional cut
  m_Model->UpdateContourCuttingPlane();

  // update vertical cuts
  m_Model->UpdateSegmentCuttingPlanes();

  // update widget
  m_Widget->UpdateWidgetTransform();

  // update north east segment
  m_Model->UpdateSegmentNorthEastTransform();

  // update north west segment
  m_Model->UpdateSegmentNorthWestTransform();

  // update south east segment
  m_Model->UpdateSegmentSouthEastTransform();

  // update south west segment
  m_Model->UpdateSegmentSouthWestTransform();

  // update contour axes
  m_Model->UpdateContourAxesTransform();

  // update global axes
  m_Model->UpdateGlobalAxesTransform();

  // set visibility
  m_Model->SetContourAxesVisibility();

  // set visibility
  m_Model->SetGlobalAxesVisibility();
}

//----------------------------------------------------------------------------
void mafOpMML::OnPOperationButton()
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
  m_TranslateOpButton->SetTitle("T");
  m_RotateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
  m_RotateOpButton->SetTitle("R");
  m_ScaleOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
  m_ScaleOpButton->SetTitle("S");

  // this button on
  m_PlaceOpButton->SetBackgroundColour(Green);
  m_PlaceOpButton->SetTitle(">>P<<");

  //
  ResetOperation();

  // prepare display information
  m_Model->GetScaledTextActor1()->GetPositionCoordinate()->SetValue(0.0, 0.8);
  m_Model->GetScaledTextActor2()->GetPositionCoordinate()->SetValue(0.0, 0.9);

  m_Widget->CenterModeOn();

  //
  m_Model->Render();
}

//----------------------------------------------------------------------------
void mafOpMML::OnTOperationButton()
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
  m_PlaceOpButton->SetTitle("P");
  m_RotateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
  m_RotateOpButton->SetTitle("R");
  m_ScaleOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
  m_ScaleOpButton->SetTitle("S");

  // this button on
  m_TranslateOpButton->SetBackgroundColour(Green);
  m_TranslateOpButton->SetTitle(">>T<<");

  //
  ResetOperation();

  // prepare display information
  m_Model->GetScaledTextActor1()->GetPositionCoordinate()->SetValue(0.0, 0.8);
  m_Model->GetScaledTextActor2()->GetPositionCoordinate()->SetValue(0.0, 0.9);

  m_Widget->TranslationModeOn();

  //
  m_Model->Render();
}

//----------------------------------------------------------------------------
void mafOpMML::OnROperationButton()
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
  m_PlaceOpButton->SetTitle("P");
  m_TranslateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
  m_TranslateOpButton->SetTitle("T");
  m_ScaleOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
  m_ScaleOpButton->SetTitle("S");

  // this button on
  m_RotateOpButton->SetBackgroundColour(Green);
  m_RotateOpButton->SetTitle(">>R<<");

  //
  ResetOperation();

  // prepare display information
  m_Model->GetScaledTextActor1()->GetPositionCoordinate()->SetValue(0.0, 0.9);

  m_Widget->UpdateRotationHandle();
  m_Widget->RotationHandleOn();
  m_Widget->SetRotationHandleVisibility();
  m_Widget->RotationModeOn();

  //
  m_Model->Render();
}

//----------------------------------------------------------------------------
void mafOpMML::OnSOperationButton()
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
  m_PlaceOpButton->SetTitle("P");
  m_TranslateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
  m_TranslateOpButton->SetTitle("T");
  m_RotateOpButton->SetBackgroundColour(m_ButtonBackgroundColour);
  m_RotateOpButton->SetTitle("R");

  // this button on
  m_ScaleOpButton->SetBackgroundColour(Green);
  m_ScaleOpButton->SetTitle(">>S<<");

  m_Model->GetScaledTextActor1()->GetPositionCoordinate()->SetValue(0.0, 0.9);

  //
  ResetOperation();

  m_Widget->UpdateScalingHandles();
  m_Widget->ScalingHandlesOn();
  m_Widget->ScalingModeOn();

  // on successful scaling, ScalingOccured flag in Model
  // is set to true, mafOpMMLContourWidget::OnLeftButtonUp

  //
  m_Model->Render();
}





//----------------------------------------------------------------------------
// Apply the registration operations: translation, rotation, deformation
// Called when we select "ok" from the registration dialog.
void mafOpMML::ApplyRegistrationOps()
//----------------------------------------------------------------------------
{
  int i;
  int n;

  double z;
  double pt[3];
  double temppt[3];

  double bounds[6];
  double cx, cy;

  // transform
  vtkTransform* Transform = vtkTransform::New();

  // number of points
  n = m_Model->GetMuscleTransform2PolyDataFilter()->GetOutput()->GetPoints()->GetNumberOfPoints();

  // deform each point
  for(i = 0; i < n; i++)
  {
    // get ith point
    m_Model->GetMuscleTransform2PolyDataFilter()->GetOutput()->GetPoints()->GetPoint(i, pt);

    // get z level and evaluate the splines at z.
    z = pt[2];

    // get ph
    double ph = m_Model->GetPHSpline()->Evaluate(z);

    // get pv
    double pv = m_Model->GetPVSpline()->Evaluate(z);

    // get ra
    double ra = m_Model->GetRASpline()->Evaluate(z);

    // get th
    double th = m_Model->GetTHSpline()->Evaluate(z);

    // get tv
    double tv = m_Model->GetTVSpline()->Evaluate(z);

    // get se
    double se = m_Model->GetSESpline()->Evaluate(z);

    // get sw
    double sw = m_Model->GetSWSpline()->Evaluate(z);

    // get sn
    double sn = m_Model->GetSNSpline()->Evaluate(z);

    // get ss
    double ss = m_Model->GetSSSpline()->Evaluate(z);

    // update the contour actor, for z level used currently
    // doesn't matter to change it as this method is called
    // only when terminating the registration operation (ok)
    m_Model->GetContourPlane()->SetOrigin(0.0, 0.0, z);

    // original bounds
    m_Model->GetContourTubeFilter()->GetOutput()->GetBounds(bounds);

    // original center
    cx = (bounds[0] + bounds[1]) / 2.0; // x
    cy = (bounds[2] + bounds[3]) / 2.0; // y

    // initialise
    Transform->Identity();

    // s operation
    Transform->Translate(cx, cy, 0.0); // contour bbox axes origin
    Transform->Translate(ph, pv, 0.0); // p operation
    Transform->RotateZ(ra); // r operation

    //
    Transform->TransformPoint(pt, temppt);
    if (temppt[0] > 0.0 && temppt[1] > 0.0) // 1st quadrant (+, +)
      Transform->Scale(se, sn, 1.0);
    else
      if (temppt[0] < 0.0 && temppt[1] > 0.0) // 2nd quadrant (-, +)
        Transform->Scale(sw, sn, 1.0);
      else
        if (temppt[0] < 0.0 && temppt[1] < 0.0) // 3rd quadrant (-, -)
          Transform->Scale(sw, ss, 1.0);
        else
          if (temppt[0] > 0.0 && temppt[1] < 0.0) // 4th quadrant (+, -)
            Transform->Scale(se, ss, 1.0);
          else
            if (temppt[0] > 0.0 && temppt[1] == 0.0) // positive x axis
              Transform->Scale(se, 1.0, 1.0);
            else
              if (temppt[0] < 0.0 && temppt[1] == 0.0) // negative x axis
                Transform->Scale(sw, 1.0, 1.0);
              else
                if (temppt[1] > 0.0 && temppt[0] == 0.0) // positive y axis
                  Transform->Scale(1.0, sn, 1.0);
                else
                  if (temppt[1] > 0.0 && temppt[0] == 0.0) // negative y axis
                    Transform->Scale(1.0, ss, 1.0);

    Transform->RotateZ(-1.0 * ra); // inverse r operation
    Transform->Translate(-1.0 * ph, -1.0 * pv, 0.0); // inverse p operation
    Transform->Translate(-1.0 * cx, -1.0 * cy, 0.0); // inverse contour bbox axes origin

    // r operation
    Transform->Translate(cx, cy, 0.0); // contour bbox axes origin
    Transform->Translate(ph, pv, 0.0); // p operation
    Transform->RotateZ(ra); // r operation
    Transform->Translate(-1.0 * ph, -1.0 * pv, 0.0); // inverse p operation
    Transform->Translate(-1.0 * cx, -1.0 * cy, 0.0); // inverse contour bbox axes origin

    // t operation
    Transform->Translate(th, tv, 0.0);

    // p operation
    Transform->Translate(ph, pv, 0.0);

    //
    Transform->TransformPoint(pt, pt);

    // set
    m_Model->GetMuscleTransform2PolyDataFilter()->GetOutput()->GetPoints()->SetPoint(i, pt);
  }

  m_Model->GetMuscleTransform2PolyDataFilter()->Update();

  // clean up
  Transform->Delete();
}

//----------------------------------------------------------------------------
void mafOpMML::ApplyInverseRegistrationOps()
//----------------------------------------------------------------------------
{

}

void mafOpMML::CreateFakeLandmarks() 
{
  double inputBounds[6]; 

  assert(m_Input);
  ((mafVME*)m_Input)->GetOutput()->GetBounds(inputBounds);
  double xmed = (inputBounds[0] + inputBounds[1])/2;
  double ymed = (inputBounds[2] + inputBounds[3])/2;


  double lm1[3] = {xmed ,ymed ,inputBounds[5]}; //high
  double lm2[3] = {xmed ,ymed, inputBounds[4]}; //low
  double lm3[3] = {inputBounds[1], inputBounds[3], inputBounds[4]}; 

  m_Model->SetLandmark1OfPatient(lm1);
  m_Model->SetLandmark2OfPatient(lm2);
  m_Model->SetLandmark3OfPatient(lm3); 

  m_Model->SetLandmark1OfAtlas(lm1);
  m_Model->SetLandmark2OfAtlas(lm2);
  m_Model->SetLandmark3OfAtlas(lm3);  
}
