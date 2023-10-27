/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUILandmark
 Authors: Stefano Perticoni - porting Daniele Giunchi
 
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


#include "albaGUILandmark.h"

#include "albaGUI.h"
#include "albaGUIButton.h"
#include "albaInteractorGenericMouse.h"
#include "albaInteractorCompositorMouse.h"
#include "albaInteractorPicker.h"            
#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaVME.h"
#include "albaRefSys.h"

#include "albaVMELandmark.h"
#include "albaVMELandmarkCloud.h"
#include "albaAbsMatrixPipe.h"
#include "albaTransformFrame.h"

#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkPoints.h"
#include "vtkPointSource.h"
#include "vtkPointLocator.h"
#include "vtkIdType.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
#include "vtkMath.h"

//----------------------------------------------------------------------------
albaGUILandmark::albaGUILandmark(albaVME *inputVME, albaObserver *listener, bool testMode /* = false */)
//----------------------------------------------------------------------------
{
  m_Listener = listener;
  
  m_LMCloud = NULL;
  m_LMCloudName = "lm_cloud";

  m_InputVME = inputVME;
  m_RefSysVMEName = ""; 
  m_Landmark = NULL;
  
  m_LandmarkName = "lm_";
  m_Gui = NULL;


  m_IsaCompositor = NULL;    
  m_IsaTranslate = NULL;
  m_IsaTranslateSnap = NULL;
  
  m_RefSysVME = m_InputVME;
  m_OldInteractor = NULL;
  m_PickerInteractor = NULL;

  m_OldInputVMEBehavior = NULL;

  m_PickerInteractor = albaInteractorPicker::New();
  //m_PickerInteractor->SetMListener(this);
  m_PickerInteractor->SetListener(this);

  m_Position[0] = m_Position[1] = m_Position[2] = 0;
  m_BoundsFraction = 60;

  SpawnLMOff();

  m_TestMode = testMode;
 
  if (!m_TestMode)
  {
    CreateGui();
  }


  if (m_InputVME == NULL)
  {
    m_CurrentTime = -1;
    SetGUIStatusToDisabled();
  }
  else
  { 
    m_RefSysVMEName = m_InputVME->GetName(); 
    m_CurrentTime = m_InputVME->GetTimeStamp();
    //PPP  Attach interactor to vme and register ald behavior
    AttachInteractor(m_InputVME, m_PickerInteractor, m_OldInputVMEBehavior);        
    SetGUIStatusToPick();
  }  
  if (!m_TestMode)
  {
    m_Gui->Update(); 
  }
}
//----------------------------------------------------------------------------
albaGUILandmark::~albaGUILandmark() 
//----------------------------------------------------------------------------
{  
  //PPP reattach old interactor to  vme

  if (m_InputVME) AttachInteractor(m_InputVME, m_OldInputVMEBehavior);

  albaDEL(m_IsaCompositor); 

  albaDEL(m_PickerInteractor);

  if (m_LMCloud)
  {
    for (int i = 0; i < m_LMCloud->GetNumberOfLandmarks(); i++)
    {
      albaVME *lm = m_LMCloud->GetChild(i);
      GetLogicManager()->VmeShow(lm, false);
      GetLogicManager()->VmeRemove(lm);
      albaDEL(lm);

    }

    GetLogicManager()->VmeShow(m_LMCloud, false);
    GetLogicManager()->VmeRemove(m_LMCloud);
    albaDEL(m_LMCloud);
  }
} 

//----------------------------------------------------------------------------
void albaGUILandmark::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUI(this); 

  m_Gui->Double(ID_TRANSLATE_X, "Translate X", &m_Position[0]);
  m_Gui->Double(ID_TRANSLATE_Y, "Translate Y", &m_Position[1]);
  m_Gui->Double(ID_TRANSLATE_Z, "Translate Z", &m_Position[2]);
  m_Gui->Divider();
 	m_Gui->Button(ID_REF_SYS,"Choose refsys");
  m_Gui->Label(&m_RefSysVMEName);

}

//----------------------------------------------------------------------------
void albaGUILandmark::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if(albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {     
    case VME_PICKED:
      {
        OnVmePicked(*e);
      }
      break;  
    case ID_TRANSLATE_X:
    case ID_TRANSLATE_Y:
    case ID_TRANSLATE_Z: 
      {
        OnTranslate(*e);
      }
      break;

    case ID_TRANSFORM:
      { 
        vtkALBASmartPointer<vtkTransform> tr;
        tr->PostMultiply();
        tr->SetMatrix(m_Landmark->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
        tr->Concatenate(e->GetMatrix()->GetVTKMatrix());
        tr->Update();

        albaMatrix mat;
        mat.DeepCopy(tr->GetMatrix());
        mat.SetTimeStamp(m_CurrentTime);

        m_Landmark->SetAbsMatrix(mat);
        SetGuiAbsPosition(mat.GetVTKMatrix(), -1);

        GetLogicManager()->CameraUpdate();

        // forward transform events to listener operation (notify)
        e->SetSender(this);
        albaEventMacro(*e); 
      }
      break;

    case ID_REF_SYS:
      {
        albaString title = _("Choose VME ref sys");
        albaEvent e(this,VME_CHOOSE,&title);
				e.SetPointer(&albaGUILandmark::VmeAccept);
        albaEventMacro(e); 
        SetRefSysVME(e.GetVme());
      }
      break;


    default:
      {
        albaEventMacro(*e);
      }
      break;
    }
  }
  
}
 
//----------------------------------------------------------------------------
void albaGUILandmark::CreateTranslateISACompositor()
//----------------------------------------------------------------------------
{
  assert(m_Landmark);

  // Create the isa compositor:
  m_IsaCompositor = albaInteractorCompositorMouse::New();

  // default aux ref sys is the vme ref sys
  m_RefSysVME = m_InputVME;

  //----------------------------------------------------------------------------
	// create the translate behavior  
	//----------------------------------------------------------------------------
   
  m_IsaTranslate = m_IsaCompositor->CreateBehavior(MOUSE_MIDDLE); 
  m_IsaTranslate->SetListener(this);
  m_IsaTranslate->SetVME(m_Landmark);
  m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
  // set the pivot point
  m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetAbsMatrixPipe()->GetMatrixPointer());
  m_IsaTranslate->GetPivotRefSys()->SetTypeToCustom(m_Landmark->GetOutput()->GetAbsMatrix());

  m_IsaTranslate->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
  m_IsaTranslate->SurfaceSnapOff(); 
  m_IsaTranslate->EnableTranslation(true);
  
	//----------------------------------------------------------------------------
	// create the translate behavior with snap
	//----------------------------------------------------------------------------
  
  m_IsaTranslateSnap = m_IsaCompositor->CreateBehavior(MOUSE_MIDDLE_CONTROL); 
  m_IsaTranslateSnap->SetListener(this);
  m_IsaTranslateSnap->SetVME(m_Landmark);
  m_IsaTranslateSnap->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
  // set the pivot point
  m_IsaTranslateSnap->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetAbsMatrixPipe()->GetMatrixPointer());
  m_IsaTranslateSnap->GetPivotRefSys()->SetTypeToCustom(m_Landmark->GetOutput()->GetAbsMatrix());

  m_IsaTranslateSnap->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
  m_IsaTranslateSnap->SurfaceSnapOn(); 
  m_IsaTranslateSnap->EnableTranslation(true);  
}
 
//----------------------------------------------------------------------------
int albaGUILandmark::AttachInteractor(albaVME *vme, albaInteractor *newInteractor, albaInteractor *storeOldInteractor)
//----------------------------------------------------------------------------
{ 
  if (!vme) return -1;
  storeOldInteractor = vme->GetBehavior();
	vme->SetBehavior(newInteractor);

  return 0;
}

//----------------------------------------------------------------------------
int albaGUILandmark::AttachInteractor(albaVME *vme, albaInteractor *newInteractor)
//----------------------------------------------------------------------------
{ 
  if (!vme) return -1;
	vme->SetBehavior(newInteractor);

  return 0;
}
 
//----------------------------------------------------------------------------
void albaGUILandmark::RefSysVmeChanged()
//----------------------------------------------------------------------------
{
  m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
  m_IsaTranslate->GetPivotRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix()); 

  m_IsaTranslateSnap->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
  m_IsaTranslateSnap->GetPivotRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix()); 
}

void albaGUILandmark::OnVmePicked(albaEvent& e)
{ 
  // new landmark abs position from event 
  vtkPoints *pts = NULL;
  pts = (vtkPoints *)e.GetVtkObj();
  double absPosition[3] = {0, 0, 0};
	pts->GetPoint(0, absPosition); 

  if (m_LMCloud == NULL)
  {
    //m_LMCloud = albaVMELandmarkCloud::New();//we have a reference on the vme (we can call vtkDEL in the UNDO)
    albaNEW(m_LMCloud);
		m_LMCloud->SetName(m_LMCloudName);
    double b[6];
    m_InputVME->GetOutput()->GetVTKData()->GetBounds(b);

    double diffX = fabs(b[1] - b[0]);
    double diffY = fabs(b[3] - b[2]);
    double diffZ = fabs(b[5] - b[4]);

    double maxBound = diffX > diffY ? (diffX > diffZ ? diffX : diffZ ) : (diffY > diffZ ? diffY : diffZ );
    m_LMCloud->SetRadius(maxBound/m_BoundsFraction);
		m_LMCloud->ReparentTo(m_InputVME);
 
     /** 
    Force vme data creation since this is required by albaPipePointSet
    */
//    albaEventMacro(albaEvent(this,VME_CREATE_CLIENT_DATA,m_LMCloud));
		GetLogicManager()->VmeAdd(m_LMCloud);
		GetLogicManager()->VmeShow(m_LMCloud, true);
    
  }

  if (m_Landmark == NULL)
  {  
    // create new landmark   
    int lmNumber = m_LMCloud->GetNumberOfLandmarks();
    wxString  name(m_LandmarkName);
    name << lmNumber; 

    //m_Landmark = albaVMELandmark::New();//we have a reference on the vme (we can call vtkDEL in the UNDO)
    albaNEW(m_Landmark);
    m_Landmark->SetName(name.char_str());
    m_Landmark->ReparentTo(m_LMCloud);

    m_Landmark->Update(); 
    m_Landmark->SetAbsPose(absPosition[0],absPosition[1],absPosition[2],0,0,0);
    
    //albaEventMacro(albaEvent(this,VME_CREATE_CLIENT_DATA,m_Landmark));
		GetLogicManager()->VmeAdd(m_Landmark);
    GetLogicManager()->VmeShow(m_Landmark, true);
		GetLogicManager()->CameraUpdate();

    CreateTranslateISACompositor(); 

    //AttachInteractor(m_InputVME, m_OldInputVMEBehavior); 
    AttachInteractor(m_Landmark, m_IsaCompositor);

    SetGUIStatusToEnabled(); 
    SetGuiAbsPosition(m_Landmark->GetAbsMatrixPipe()->GetMatrixPointer()->GetVTKMatrix());     
  }
  else
  {
    m_Landmark->SetAbsPose(absPosition[0],absPosition[1],absPosition[2],0,0,0);
    GetLogicManager()->VmeShow(m_Landmark, true);
  }
   
  // notify listener
  e.SetSender(this);
  albaEventMacro(e);
	GetLogicManager()->CameraUpdate();
}

 
//----------------------------------------------------------------------------
void albaGUILandmark::OnTranslate(albaEvent &e)
//----------------------------------------------------------------------------
{
  // build the matrix to be applied to vme:
  TextEntriesChanged();   
} 

void albaGUILandmark::SetGUIStatusToDisabled()
{
  m_GUIStatus = DISABLED;
  this->UpdateGuiInternal();
} 
   
 void albaGUILandmark::SetGUIStatusToPick() 
{
  if (m_Landmark)
  { 
    AttachInteractor(m_Landmark, NULL);
    m_Landmark->SetLandmarkVisibility(false);
    m_Landmark = NULL;
  }
  
  if (GetSpawnLM() == 0)
  { 
    m_GUIStatus = PICK; 
    if (m_Gui)
    {
      this->UpdateGuiInternal(); 
    }
  }
  else
  {
    SpawnLandmark();
    UpdateInteractor();
    SetGUIStatusToEnabled();
  }
}   
 
void albaGUILandmark::SetGUIStatusToEnabled() 
{
  m_GUIStatus = ENABLED;
  if (m_Gui)
  {
    this->UpdateGuiInternal();
  }
}

void albaGUILandmark::UpdateGuiInternal()
{
  assert(m_Gui);  

  switch(m_GUIStatus) 
  {
    case DISABLED:
    {
 
      m_Gui->Enable(ID_TRANSLATE_X, false);
      m_Gui->Enable(ID_TRANSLATE_Y, false); 
      m_Gui->Enable(ID_TRANSLATE_Z, false);
      m_Gui->Enable(ID_REF_SYS, false);
    }
    break;
       
    case PICK:
    {
  
      m_Gui->Enable(ID_TRANSLATE_X, false);
      m_Gui->Enable(ID_TRANSLATE_Y, false); 
      m_Gui->Enable(ID_TRANSLATE_Z, false);
      m_Gui->Enable(ID_REF_SYS, false);  
    }
    break;
    
    case ENABLED:
    {
   
      m_Gui->Enable(ID_TRANSLATE_X, true);
      m_Gui->Enable(ID_TRANSLATE_Y, true); 
      m_Gui->Enable(ID_TRANSLATE_Z, true);
      m_Gui->Enable(ID_REF_SYS, true);  
    }
    break;

    default:
    {
    }
  }
  m_Gui->Update();
}


void albaGUILandmark::SetInputVME(albaVME *vme)
{
   assert(m_InputVME == NULL); 
   m_InputVME = vme;
   m_CurrentTime = m_InputVME->GetTimeStamp();
   SetRefSysVME(m_InputVME);
   AttachInteractor(m_InputVME, m_PickerInteractor, m_OldInputVMEBehavior);        
}

void albaGUILandmark::GetSpawnPointCoordinates(double newPointCoord[3]) 
{   
  vtkIdType pId;
  vtkDataSet* data = m_InputVME->GetOutput()->GetVTKData();

  if (m_InputVME->IsA("albaVMESurface"))
  { 
    assert(m_LMCloud);

    int npoints = data->GetNumberOfPoints();
    assert(data);

    if (m_LMCloud->GetNumberOfLandmarks() == 1)
    {    
      vtkALBASmartPointer<vtkIdList> idList;
      idList->SetNumberOfIds(npoints);
      
      double pos[3];
      //albaVMELandmark::SafeDownCast(m_LMCloud->GetChild(0))->GetPosition(pos);
      albaVMELandmark::SafeDownCast(m_LMCloud->GetChild(0))->GetPoint(pos);

      vtkALBASmartPointer<vtkPointLocator> locator;
      locator->SetDataSet(data);
      locator->FindClosestNPoints(npoints, pos[0], pos[1], pos[2], idList);
      locator->Update();

      pId = idList->GetId(npoints - 1); 
    }
    else
    {
      vtkALBASmartPointer<vtkPointSource> pSource;
      pSource->SetNumberOfPoints(1);
      pSource->SetCenter(data->GetCenter());
      pSource->SetRadius(data->GetLength() / 2);
      pSource->Update();

      vtkALBASmartPointer<vtkPointLocator> locator;
      locator->SetDataSet(data);
      pId = locator->FindClosestPoint(pSource->GetOutput()->GetPoint(0));     
      locator->Update(); 
    }   

    data->GetPoint(pId, newPointCoord);     
  }
  else if (m_InputVME->IsA("albaVMEVolumeGray"))
  {
    double bounds[6];
    data->GetBounds(bounds);
    double randX = vtkMath::Random(bounds[0], bounds[1]);
    double randY = vtkMath::Random(bounds[2], bounds[3]);
    double randZ = vtkMath::Random(bounds[4], bounds[5]);
    
    newPointCoord[0] = randX;
    newPointCoord[1] = randY;
    newPointCoord[2] = randZ;   
  }

  else
  {
    // unsupported vme type
    assert(false);
  }
}

void albaGUILandmark::SpawnLandmark()
{    
  // create new landmark   
  int lmNumber = m_LMCloud->GetNumberOfLandmarks();
  wxString  name(m_LandmarkName);
  name << lmNumber; 

  double position[3];
  GetSpawnPointCoordinates(position);

  //m_Landmark = albaVMELandmark::New();//we have a reference on the vme (we can call vtkDEL in the UNDO)
  albaNEW(m_Landmark);
  m_Landmark->SetName(name.char_str());
  m_Landmark->ReparentTo(m_LMCloud);
  m_Landmark->Update(); 

  //m_Landmark->SetPose(position[0],position[1],position[2],-1);
  double rot[3] = {0.0,0.0,0.0};
  m_Landmark->SetPose(position[0],position[1],position[2],rot[0], rot[1], rot[2],-1);
  
  //albaEventMacro(albaEvent(this,VME_CREATE_CLIENT_DATA,m_Landmark));
	GetLogicManager()->VmeAdd(m_Landmark);
  GetLogicManager()->VmeShow(m_Landmark, true);
	GetLogicManager()->CameraUpdate();
  
  AttachInteractor(m_Landmark, m_IsaCompositor);

  SetGUIStatusToEnabled(); 
  SetGuiAbsPosition(m_Landmark->GetAbsMatrixPipe()->GetMatrixPointer()->GetVTKMatrix());     
}

void albaGUILandmark::UpdateInteractor()
{ 
  m_IsaTranslateSnap->SetVME(m_Landmark);
  m_IsaTranslateSnap->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
  // set the pivot point
  m_IsaTranslateSnap->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetAbsMatrixPipe()->GetMatrixPointer());
  m_IsaTranslateSnap->GetPivotRefSys()->SetTypeToCustom(m_Landmark->GetOutput()->GetAbsMatrix()); 
}


//----------------------------------------------------------------------------
void albaGUILandmark::OnRefSysVmeChanged()
//----------------------------------------------------------------------------
{
  m_RefSysVMEName = m_RefSysVME->GetName();
  if (m_Landmark) this->SetGuiAbsPosition(m_Landmark->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
}

//----------------------------------------------------------------------------
void albaGUILandmark::TextEntriesChanged()
//----------------------------------------------------------------------------
{
  // build the matrix to be applied to vme:

  /*
  - scale
  - rotate around Y, than X, then Z
  - translate 
  */

  albaSmartPointer<albaTransform> tran;
  tran->SetPosition(m_Position[0], m_Position[1], m_Position[2]);

  // premultiply to ref sys abs matrix
  //tran->Concatenate(m_RefSysVME->GetOutput()->GetAbsPose(), POST_MULTIPLY);
  tran->Concatenate(*m_RefSysVME->GetOutput()->GetAbsMatrix(), POST_MULTIPLY);
//  m_Landmark-SetAbsPose(tran->GetMatrix(), m_CurrentTime);
  double pos[3],rot[3];
  albaTransform::GetPosition(tran->GetMatrix(), pos);
  albaTransform::GetOrientation(tran->GetMatrix(), rot);
  m_Landmark->SetAbsPose(pos,rot, m_CurrentTime);
  
  this->SetGuiAbsPosition(m_Landmark->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
 
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaGUILandmark::SetGuiAbsPosition(vtkMatrix4x4* absPose, albaTimeStamp timeStamp)
//----------------------------------------------------------------------------
{
  // express absPose in m_RefSysVME refsys
 /* albaSmartPointer<albaTransform>  mflTr;
  mflTr->SetInput(absPose);
  //mflTr->SetTargetFrame(m_RefSysVME->GetAbsPose());
  mflTr->Update();*/

  // Express VME abs matrix in m_RefSysVME refsys via albaTransform
  albaSmartPointer<albaTransformFrame> mflTr;
  //mflTr->SetInput(albaMatrix(absPose));
  albaMatrix mat;
  mat.SetVTKMatrix(absPose);
  mflTr->SetInput(&mat);
  mflTr->SetTargetFrame(m_RefSysVME->GetOutput()->GetAbsMatrix());
  mflTr->Update();
  
  // update gui with new position 
  albaTransform::GetPosition(mflTr->GetMatrix(), m_Position);
  
  m_Gui->Update();
}

//----------------------------------------------------------------------------
void albaGUILandmark::SetRefSysVME(albaVME* refSysVME)
//----------------------------------------------------------------------------
{
  // ref sys vme must be not null
  if(refSysVME == NULL)
	return;

  m_RefSysVME = refSysVME;
  OnRefSysVmeChanged();
}