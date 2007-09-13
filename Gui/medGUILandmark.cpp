/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUILandmark.cpp,v $
Language:  C++
Date:      $Date: 2007-09-13 13:06:06 $
Version:   $Revision: 1.2 $
Authors:   Stefano Perticoni - porting Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/


#include "mafDefines.h" 
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "medGUILandmark.h"

#include "mmgGui.h"
#include "mmgButton.h"
#include "mmiGenericMouse.h"
#include "mmiCompositorMouse.h"
#include "mmiPicker.h"            
#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafVME.h"

#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"
#include "mafAbsMatrixPipe.h"
#include "mafTransformFrame.h"

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
medGUILandmark::medGUILandmark(mafNode *inputVME, mafObserver *listener)
//----------------------------------------------------------------------------
{
  m_Listener = listener;
  
  m_LMCloud = NULL;
  m_LMCloudName = "lm_cloud";

  m_InputVME = mafVME::SafeDownCast(inputVME);
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

  m_PickerInteractor = mmiPicker::New();
  //m_PickerInteractor->SetMListener(this);
  m_PickerInteractor->SetListener(this);

  m_Position[0] = m_Position[1] = m_Position[2] = 0;

  SpawnLMOff();
 
  CreateGui();


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
  
  m_Gui->Update(); 
}
//----------------------------------------------------------------------------
medGUILandmark::~medGUILandmark() 
//----------------------------------------------------------------------------
{  
  //PPP reattach old interactor to  vme

  if (m_InputVME) AttachInteractor(m_InputVME, m_OldInputVMEBehavior);

  mafDEL(m_IsaCompositor); 

  mafDEL(m_PickerInteractor);

  if (m_LMCloud)
  {
    for (int i = 0; i < m_LMCloud->GetNumberOfLandmarks(); i++)
    {
      mafNode *lm = m_LMCloud->GetChild(i);
      mafEventMacro(mafEvent(this, VME_REMOVE, lm));
      mafDEL(lm);
      //vtkDEL(lm);
    }

    mafEventMacro(mafEvent(this, VME_REMOVE, m_LMCloud));
    mafDEL(m_LMCloud);
    //vtkDEL(m_LMCloud);
  }


  // delete child landmarks 

  // m_Gui already destroyed?
} 

//----------------------------------------------------------------------------
void medGUILandmark::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mmgGui(this); 
  /*
  m_Gui->Label("mouse interaction", true);
  m_Gui->Label("left mouse: pick landmark");
  m_Gui->Label("middle mouse: translate");
  m_Gui->Label("ctrl: toggle snap on surface during translate");
  */ 

  m_Gui->Double(ID_TRANSLATE_X, "Translate X", &m_Position[0]);
  m_Gui->Double(ID_TRANSLATE_Y, "Translate Y", &m_Position[1]);
  m_Gui->Double(ID_TRANSLATE_Z, "Translate Z", &m_Position[2]);
  m_Gui->Divider();
 	m_Gui->Button(ID_REF_SYS,"choose refsys");
  m_Gui->Label(mafString("refsys name: "),&m_RefSysVMEName);

}

//----------------------------------------------------------------------------
void medGUILandmark::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if(mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
        vtkMAFSmartPointer<vtkTransform> tr;
        tr->PostMultiply();
        tr->SetMatrix(m_Landmark->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
        tr->Concatenate(e->GetMatrix()->GetVTKMatrix());
        tr->Update();

        mafMatrix mat;
        mat.DeepCopy(tr->GetMatrix());
        mat.SetTimeStamp(m_CurrentTime);

        //m_Landmark->SetAbsPose(mat);
        m_Landmark->SetAbsMatrix(mat);
        SetGuiAbsPosition(mat.GetVTKMatrix(), -1);

        //UpdateIsa();

        mafEventMacro(mafEvent(this, CAMERA_UPDATE));        

        // forward transform events to listener operation (notify)
        e->SetSender(this);
        mafEventMacro(*e); 
      }
      break;

    case ID_REF_SYS:
      {
        mafString title = _("Choose VME ref sys");
        mafEvent e(this,VME_CHOOSE,&title,(long)&medGUILandmark::VmeAccept);
        mafEventMacro(e); 
        SetRefSysVME(mafVME::SafeDownCast(e.GetVme())); 			
      }
      break;


    default:
      {
        mafEventMacro(*e);
      }
      break;
    }
  }
  
}
 
//----------------------------------------------------------------------------
void medGUILandmark::CreateTranslateISACompositor()
//----------------------------------------------------------------------------
{
  assert(m_Landmark);

  // Create the isa compositor:
  m_IsaCompositor = mmiCompositorMouse::New();

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

  m_IsaTranslate->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
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

  m_IsaTranslateSnap->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
  m_IsaTranslateSnap->SurfaceSnapOn(); 
  m_IsaTranslateSnap->EnableTranslation(true);  
}
 
//----------------------------------------------------------------------------
int medGUILandmark::AttachInteractor(mafNode *vme, mafInteractor *newInteractor, mafInteractor *storeOldInteractor)
//----------------------------------------------------------------------------
{ 
  if (!vme) return -1;
  mafVME *vmeLocal = mafVME::SafeDownCast(vme);
  storeOldInteractor = vmeLocal->GetBehavior();  
  vmeLocal->SetBehavior(newInteractor);

  return 0;
}

//----------------------------------------------------------------------------
int medGUILandmark::AttachInteractor(mafNode *vme, mafInteractor *newInteractor)
//----------------------------------------------------------------------------
{ 
  if (!vme) return -1;
  mafVME *vmeLocal = mafVME::SafeDownCast(vme);
  vmeLocal->SetBehavior(newInteractor);

  return 0;
}
 
//----------------------------------------------------------------------------
void medGUILandmark::RefSysVmeChanged()
//----------------------------------------------------------------------------
{
  m_IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
  m_IsaTranslate->GetPivotRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix()); 

  m_IsaTranslateSnap->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix());
  m_IsaTranslateSnap->GetPivotRefSys()->SetMatrix(m_RefSysVME->GetOutput()->GetAbsMatrix()); 
}

void medGUILandmark::OnVmePicked(mafEvent& e)
{ 
  // new landmark abs position from event 
  vtkPoints *pts = NULL;
  pts = (vtkPoints *)e.GetVtkObj();
  double absPosition[3] = {0, 0, 0};
	pts->GetPoint(0, absPosition); 

  if (m_LMCloud == NULL)
  {
    //m_LMCloud = mafVMELandmarkCloud::New();//we have a reference on the vme (we can call vtkDEL in the UNDO)
    mafNEW(m_LMCloud);
    m_LMCloud->Open();
		m_LMCloud->SetName(m_LMCloudName);
    double b[6];
    mafVME::SafeDownCast(m_InputVME)->GetOutput()->GetBounds(b);

    double diffX = fabs(b[1] - b[0]);
    double diffY = fabs(b[3] - b[2]);
    double diffZ = fabs(b[5] - b[4]);

    double maxBound = diffX > diffY ? (diffX > diffZ ? diffX : diffZ ) : (diffY > diffZ ? diffY : diffZ );
    m_LMCloud->SetRadius(maxBound/60);
		m_LMCloud->ReparentTo(m_InputVME);
 
     /** 
    Force vme data creation since this is required by mafPipePointSet
    */
//    mafEventMacro(mafEvent(this,VME_CREATE_CLIENT_DATA,m_LMCloud));
		mafEventMacro(mafEvent(this,VME_ADD,m_LMCloud));
		mafEventMacro(mafEvent(this,VME_SHOW,m_LMCloud,true)); 
    
  }

  if (m_Landmark == NULL)
  {  
    // create new landmark   
    int lmNumber = m_LMCloud->GetNumberOfLandmarks();
    wxString  name(m_LandmarkName);
    name << lmNumber; 

    //m_Landmark = mafVMELandmark::New();//we have a reference on the vme (we can call vtkDEL in the UNDO)
    mafNEW(m_Landmark);
    m_Landmark->SetName(name.c_str());
    m_Landmark->ReparentTo(m_LMCloud);

    m_Landmark->Update(); 
    m_Landmark->SetAbsPose(absPosition[0],absPosition[1],absPosition[2],0,0,0);
    
    //mafEventMacro(mafEvent(this,VME_CREATE_CLIENT_DATA,m_Landmark));
	  mafEventMacro(mafEvent(this,VME_ADD,m_Landmark));
    mafEventMacro(mafEvent(this,VME_SHOW,m_Landmark,true)); 
	  mafEventMacro(mafEvent(this,CAMERA_UPDATE)); 

    CreateTranslateISACompositor(); 

    //AttachInteractor(m_InputVME, m_OldInputVMEBehavior); 
    AttachInteractor(m_Landmark, m_IsaCompositor);

    SetGUIStatusToEnabled(); 
    SetGuiAbsPosition(m_Landmark->GetAbsMatrixPipe()->GetMatrixPointer()->GetVTKMatrix());     
  }
  else
  {
    m_Landmark->SetAbsPose(absPosition[0],absPosition[1],absPosition[2],0,0,0);
    mafEventMacro(mafEvent(this,VME_SHOW,m_Landmark,true)); 
  }
   
  // notify listener
  e.SetSender(this);
  mafEventMacro(e);
  mafEventMacro(mafEvent(this,CAMERA_UPDATE)); 
}

 
//----------------------------------------------------------------------------
void medGUILandmark::OnTranslate(mafEvent &e)
//----------------------------------------------------------------------------
{
  // build the matrix to be applied to vme:
  TextEntriesChanged();   
} 

void medGUILandmark::SetGUIStatusToDisabled()
{
  m_GUIStatus = DISABLED;
  this->UpdateGuiInternal();
} 
   
 void medGUILandmark::SetGUIStatusToPick() 
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
    this->UpdateGuiInternal(); 
  }
  else
  {
    SpawnLandmark();
    UpdateInteractor();
    SetGUIStatusToEnabled();
  }
}   
 
void medGUILandmark::SetGUIStatusToEnabled() 
{
  m_GUIStatus = ENABLED;
  this->UpdateGuiInternal();
}

void medGUILandmark::UpdateGuiInternal()
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


void medGUILandmark::SetInputVME(mafNode *vme)
{
   assert(m_InputVME == NULL); 
   m_InputVME = mafVME::SafeDownCast(vme);
   m_CurrentTime = m_InputVME->GetTimeStamp();
   SetRefSysVME(m_InputVME);
   AttachInteractor(m_InputVME, m_PickerInteractor, m_OldInputVMEBehavior);        
}

void medGUILandmark::GetSpawnPointCoordinates(double newPointCoord[3]) 
{   
  vtkIdType pId;
  vtkDataSet* data = m_InputVME->GetOutput()->GetVTKData();

  if (m_InputVME->IsA("mafVMESurface"))
  { 
    assert(m_LMCloud);

    int npoints = data->GetNumberOfPoints();
    assert(data);

    if (m_LMCloud->GetNumberOfLandmarks() == 1)
    {    
      vtkMAFSmartPointer<vtkIdList> idList;
      idList->SetNumberOfIds(npoints);
      
      double pos[3];
      //mafVMELandmark::SafeDownCast(m_LMCloud->GetChild(0))->GetPosition(pos);
      mafVMELandmark::SafeDownCast(m_LMCloud->GetChild(0))->GetPoint(pos);

      vtkMAFSmartPointer<vtkPointLocator> locator;
      locator->SetDataSet(data);
      locator->FindClosestNPoints(npoints, pos[0], pos[1], pos[2], idList);
      locator->Update();

      pId = idList->GetId(npoints - 1); 
    }
    else
    {
      vtkMAFSmartPointer<vtkPointSource> pSource;
      pSource->SetNumberOfPoints(1);
      pSource->SetCenter(data->GetCenter());
      pSource->SetRadius(data->GetLength() / 2);
      pSource->Update();

      vtkMAFSmartPointer<vtkPointLocator> locator;
      locator->SetDataSet(data);
      pId = locator->FindClosestPoint(pSource->GetOutput()->GetPoint(0));     
      locator->Update(); 
    }   

    data->GetPoint(pId, newPointCoord);     
  }
  else if (m_InputVME->IsA("mafVMEGrayVolume"))
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

void medGUILandmark::SpawnLandmark()
{    
  // create new landmark   
  int lmNumber = m_LMCloud->GetNumberOfLandmarks();
  wxString  name(m_LandmarkName);
  name << lmNumber; 

  double position[3];
  GetSpawnPointCoordinates(position);

  //m_Landmark = mafVMELandmark::New();//we have a reference on the vme (we can call vtkDEL in the UNDO)
  mafNEW(m_Landmark);
  m_Landmark->SetName(name.c_str());
  m_Landmark->ReparentTo(m_LMCloud);
  m_Landmark->Update(); 

  //m_Landmark->SetPose(position[0],position[1],position[2],-1);
  double rot[3] = {0.0,0.0,0.0};
  m_Landmark->SetPose(position[0],position[1],position[2],rot[0], rot[1], rot[2],-1);
  
  //mafEventMacro(mafEvent(this,VME_CREATE_CLIENT_DATA,m_Landmark));
	mafEventMacro(mafEvent(this,VME_ADD,m_Landmark));
  mafEventMacro(mafEvent(this,VME_SHOW,m_Landmark,true)); 
	mafEventMacro(mafEvent(this,CAMERA_UPDATE)); 
  
  AttachInteractor(m_Landmark, m_IsaCompositor);

  SetGUIStatusToEnabled(); 
  SetGuiAbsPosition(m_Landmark->GetAbsMatrixPipe()->GetMatrixPointer()->GetVTKMatrix());     
}

void medGUILandmark::UpdateInteractor()
{ 
  m_IsaTranslateSnap->SetVME(m_Landmark);
  m_IsaTranslateSnap->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
  // set the pivot point
  m_IsaTranslateSnap->GetTranslationConstraint()->GetRefSys()->SetMatrix(m_RefSysVME->GetAbsMatrixPipe()->GetMatrixPointer());
  m_IsaTranslateSnap->GetPivotRefSys()->SetTypeToCustom(m_Landmark->GetOutput()->GetAbsMatrix()); 
}


//----------------------------------------------------------------------------
void medGUILandmark::OnRefSysVmeChanged()
//----------------------------------------------------------------------------
{
  m_RefSysVMEName = m_RefSysVME->GetName();
  if (m_Landmark) this->SetGuiAbsPosition(m_Landmark->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
}

//----------------------------------------------------------------------------
void medGUILandmark::TextEntriesChanged()
//----------------------------------------------------------------------------
{
  // build the matrix to be applied to vme:

  /*
  - scale
  - rotate around Y, than X, then Z
  - translate 
  */

  mafSmartPointer<mafTransform> tran;
  tran->SetPosition(m_Position[0], m_Position[1], m_Position[2]);

  // premultiply to ref sys abs matrix
  //tran->Concatenate(m_RefSysVME->GetOutput()->GetAbsPose(), POST_MULTIPLY);
  tran->Concatenate(*m_RefSysVME->GetOutput()->GetAbsMatrix(), POST_MULTIPLY);
//  m_Landmark-SetAbsPose(tran->GetMatrix(), m_CurrentTime);
  double pos[3],rot[3];
  mafTransform::GetPosition(tran->GetMatrix(), pos);
  mafTransform::GetOrientation(tran->GetMatrix(), rot);
  m_Landmark->SetAbsPose(pos,rot, m_CurrentTime);
  
  this->SetGuiAbsPosition(m_Landmark->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
 
   mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void medGUILandmark::SetGuiAbsPosition(vtkMatrix4x4* absPose, mafTimeStamp timeStamp)
//----------------------------------------------------------------------------
{
  // express absPose in m_RefSysVME refsys
 /* mafSmartPointer<mafTransform>  mflTr;
  mflTr->SetInput(absPose);
  //mflTr->SetTargetFrame(m_RefSysVME->GetAbsPose());
  mflTr->Update();*/

  // Express VME abs matrix in m_RefSysVME refsys via mafTransform
  mafSmartPointer<mafTransformFrame> mflTr;
  //mflTr->SetInput(mafMatrix(absPose));
  mafMatrix mat;
  mat.SetVTKMatrix(absPose);
  mflTr->SetInput(&mat);
  mflTr->SetTargetFrame(m_RefSysVME->GetOutput()->GetAbsMatrix());
  mflTr->Update();
  
  // update gui with new position 
  mafTransform::GetPosition(mflTr->GetMatrix(), m_Position);
  
  m_Gui->Update();
}

//----------------------------------------------------------------------------
void medGUILandmark::SetRefSysVME(mafVME* refSysVME)
//----------------------------------------------------------------------------
{
  // ref sys vme must be not null
  if(refSysVME == NULL)
	return;

  m_RefSysVME = refSysVME;
  OnRefSysVmeChanged();
}