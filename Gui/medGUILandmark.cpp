/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUILandmark.cpp,v $
Language:  C++
Date:      $Date: 2007-09-13 09:06:56 $
Version:   $Revision: 1.1 $
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
  
  LMCloud = NULL;
  LMCloudName = "lm_cloud";

  InputVME = mafVME::SafeDownCast(inputVME);
  RefSysVMEName = ""; 
  Landmark = NULL;
  
  LandmarkName = "lm_";
  m_gui = NULL;


  IsaCompositor = NULL;    
  IsaTranslate = NULL;
  IsaTranslateSnap = NULL;
  
  RefSysVME = InputVME;
  OldInteractor = NULL;
  PickerInteractor = NULL;

  OldInputVMEBehavior = NULL;

  PickerInteractor = mmiPicker::New();
  //PickerInteractor->SetMListener(this);
  PickerInteractor->SetListener(this);

  Position[0] = Position[1] = Position[2] = 0;

  SpawnLMOff();
 
  CreateGui();


  if (InputVME == NULL)
  {
    CurrentTime = -1;
    SetGUIStatusToDisabled();
  }
  else
  { 
    RefSysVMEName = InputVME->GetName(); 
    CurrentTime = InputVME->GetTimeStamp();
    //PPP  Attach interactor to vme and register ald behavior
    AttachInteractor(InputVME, PickerInteractor, OldInputVMEBehavior);        
    SetGUIStatusToPick();
  }  
  
  m_gui->Update(); 
}
//----------------------------------------------------------------------------
medGUILandmark::~medGUILandmark() 
//----------------------------------------------------------------------------
{  
  //PPP reattach old interactor to  vme

  if (InputVME) AttachInteractor(InputVME, OldInputVMEBehavior);

  mafDEL(IsaCompositor); 

  mafDEL(PickerInteractor);

  if (LMCloud)
  {
    for (int i = 0; i < LMCloud->GetNumberOfLandmarks(); i++)
    {
      mafNode *lm = LMCloud->GetChild(i);
      mafEventMacro(mafEvent(this, VME_REMOVE, lm));
      mafDEL(lm);
      //vtkDEL(lm);
    }

    mafEventMacro(mafEvent(this, VME_REMOVE, LMCloud));
    mafDEL(LMCloud);
    //vtkDEL(LMCloud);
  }


  // delete child landmarks 

  // m_gui already destroyed?
} 

//----------------------------------------------------------------------------
void medGUILandmark::CreateGui()
//----------------------------------------------------------------------------
{
  m_gui = new mmgGui(this); 
  /*
  m_gui->Label("mouse interaction", true);
  m_gui->Label("left mouse: pick landmark");
  m_gui->Label("middle mouse: translate");
  m_gui->Label("ctrl: toggle snap on surface during translate");
  */ 

  m_gui->Double(ID_TRANSLATE_X, "Translate X", &Position[0]);
  m_gui->Double(ID_TRANSLATE_Y, "Translate Y", &Position[1]);
  m_gui->Double(ID_TRANSLATE_Z, "Translate Z", &Position[2]);
  m_gui->Divider();
 	m_gui->Button(ID_REF_SYS,"choose refsys");
  m_gui->Label(mafString("refsys name: "),&RefSysVMEName);

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
        tr->SetMatrix(Landmark->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
        tr->Concatenate(e->GetMatrix()->GetVTKMatrix());
        tr->Update();

        mafMatrix mat;
        mat.DeepCopy(tr->GetMatrix());
        mat.SetTimeStamp(CurrentTime);

        //Landmark->SetAbsPose(mat);
        Landmark->SetAbsMatrix(mat);
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
  assert(Landmark);

  // Create the isa compositor:
  IsaCompositor = mmiCompositorMouse::New();

  // default aux ref sys is the vme ref sys
  RefSysVME = InputVME;

  //----------------------------------------------------------------------------
	// create the translate behavior  
	//----------------------------------------------------------------------------
   
  IsaTranslate = IsaCompositor->CreateBehavior(MOUSE_MIDDLE); 
  IsaTranslate->SetListener(this);
  IsaTranslate->SetVME(Landmark);
  IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
  // set the pivot point
  IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(RefSysVME->GetAbsMatrixPipe()->GetMatrixPointer());
  IsaTranslate->GetPivotRefSys()->SetTypeToCustom(Landmark->GetOutput()->GetAbsMatrix());

  IsaTranslate->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
  IsaTranslate->SurfaceSnapOff(); 
  IsaTranslate->EnableTranslation(true);
  
	//----------------------------------------------------------------------------
	// create the translate behavior with snap
	//----------------------------------------------------------------------------
  
  IsaTranslateSnap = IsaCompositor->CreateBehavior(MOUSE_MIDDLE_CONTROL); 
  IsaTranslateSnap->SetListener(this);
  IsaTranslateSnap->SetVME(Landmark);
  IsaTranslateSnap->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
  // set the pivot point
  IsaTranslateSnap->GetTranslationConstraint()->GetRefSys()->SetMatrix(RefSysVME->GetAbsMatrixPipe()->GetMatrixPointer());
  IsaTranslateSnap->GetPivotRefSys()->SetTypeToCustom(Landmark->GetOutput()->GetAbsMatrix());

  IsaTranslateSnap->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
  IsaTranslateSnap->SurfaceSnapOn(); 
  IsaTranslateSnap->EnableTranslation(true);  
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
  IsaTranslate->GetTranslationConstraint()->GetRefSys()->SetMatrix(RefSysVME->GetOutput()->GetAbsMatrix());
  IsaTranslate->GetPivotRefSys()->SetMatrix(RefSysVME->GetOutput()->GetAbsMatrix()); 

  IsaTranslateSnap->GetTranslationConstraint()->GetRefSys()->SetMatrix(RefSysVME->GetOutput()->GetAbsMatrix());
  IsaTranslateSnap->GetPivotRefSys()->SetMatrix(RefSysVME->GetOutput()->GetAbsMatrix()); 
}

void medGUILandmark::OnVmePicked(mafEvent& e)
{ 
  // new landmark abs position from event 
  vtkPoints *pts = NULL;
  pts = (vtkPoints *)e.GetVtkObj();
  double absPosition[3] = {0, 0, 0};
	pts->GetPoint(0, absPosition); 

  if (LMCloud == NULL)
  {
    //LMCloud = mafVMELandmarkCloud::New();//we have a reference on the vme (we can call vtkDEL in the UNDO)
    mafNEW(LMCloud);
    LMCloud->Open();
		LMCloud->SetName(LMCloudName);
    //LMCloud->SetRadius(InputVME->GetOutput()->GetLength()/60);
		LMCloud->ReparentTo(InputVME);
 
     /** 
    Force vme data creation since this is required by mafPipePointSet
    */
//    mafEventMacro(mafEvent(this,VME_CREATE_CLIENT_DATA,LMCloud));
		mafEventMacro(mafEvent(this,VME_ADD,LMCloud));
		mafEventMacro(mafEvent(this,VME_SHOW,LMCloud,true)); 
    
  }

  if (Landmark == NULL)
  {  
    // create new landmark   
    int lmNumber = LMCloud->GetNumberOfLandmarks();
    wxString  name(LandmarkName);
    name << lmNumber; 

    //Landmark = mafVMELandmark::New();//we have a reference on the vme (we can call vtkDEL in the UNDO)
    mafNEW(Landmark);
    Landmark->SetName(name.c_str());
    Landmark->ReparentTo(LMCloud);

    Landmark->Update(); 
    Landmark->SetAbsPose(absPosition[0],absPosition[1],absPosition[2],0,0,0);
    
    //mafEventMacro(mafEvent(this,VME_CREATE_CLIENT_DATA,Landmark));
	  mafEventMacro(mafEvent(this,VME_ADD,Landmark));
    mafEventMacro(mafEvent(this,VME_SHOW,Landmark,true)); 
	  mafEventMacro(mafEvent(this,CAMERA_UPDATE)); 

    CreateTranslateISACompositor(); 

    //AttachInteractor(InputVME, OldInputVMEBehavior); 
    AttachInteractor(Landmark, IsaCompositor);

    SetGUIStatusToEnabled(); 
    SetGuiAbsPosition(Landmark->GetAbsMatrixPipe()->GetMatrixPointer()->GetVTKMatrix());     
  }
  else
  {
    Landmark->SetAbsPose(absPosition[0],absPosition[1],absPosition[2],0,0,0);
    mafEventMacro(mafEvent(this,VME_SHOW,Landmark,true)); 
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
  GUIStatus = DISABLED;
  this->UpdateGuiInternal();
} 
   
 void medGUILandmark::SetGUIStatusToPick() 
{
  if (Landmark)
  { 
    AttachInteractor(Landmark, NULL);
    Landmark->SetLandmarkVisibility(false);
    Landmark = NULL;
  }
  
  if (GetSpawnLM() == 0)
  { 
    GUIStatus = PICK;    
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
  GUIStatus = ENABLED;
  this->UpdateGuiInternal();
}

void medGUILandmark::UpdateGuiInternal()
{
  assert(m_gui);  

  switch(GUIStatus) 
  {
    case DISABLED:
    {
 
      m_gui->Enable(ID_TRANSLATE_X, false);
      m_gui->Enable(ID_TRANSLATE_Y, false); 
      m_gui->Enable(ID_TRANSLATE_Z, false);
      m_gui->Enable(ID_REF_SYS, false);
    }
    break;
       
    case PICK:
    {
  
      m_gui->Enable(ID_TRANSLATE_X, false);
      m_gui->Enable(ID_TRANSLATE_Y, false); 
      m_gui->Enable(ID_TRANSLATE_Z, false);
      m_gui->Enable(ID_REF_SYS, false);  
    }
    break;
    
    case ENABLED:
    {
   
      m_gui->Enable(ID_TRANSLATE_X, true);
      m_gui->Enable(ID_TRANSLATE_Y, true); 
      m_gui->Enable(ID_TRANSLATE_Z, true);
      m_gui->Enable(ID_REF_SYS, true);  
    }
    break;

    default:
    {
    }
  }
  m_gui->Update();
}


void medGUILandmark::SetInputVME(mafNode *vme)
{
   assert(InputVME == NULL); 
   InputVME = mafVME::SafeDownCast(vme);
   CurrentTime = InputVME->GetTimeStamp();
   SetRefSysVME(InputVME);
   AttachInteractor(InputVME, PickerInteractor, OldInputVMEBehavior);        
}

void medGUILandmark::GetSpawnPointCoordinates(double newPointCoord[3]) 
{   
  vtkIdType pId;
  vtkDataSet* data = InputVME->GetOutput()->GetVTKData();

  if (InputVME->IsA("mafVMESurface"))
  { 
    assert(LMCloud);

    int npoints = data->GetNumberOfPoints();
    assert(data);

    if (LMCloud->GetNumberOfLandmarks() == 1)
    {    
      vtkMAFSmartPointer<vtkIdList> idList;
      idList->SetNumberOfIds(npoints);
      
      double pos[3];
      //mafVMELandmark::SafeDownCast(LMCloud->GetChild(0))->GetPosition(pos);
      mafVMELandmark::SafeDownCast(LMCloud->GetChild(0))->GetPoint(pos);

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
  else if (InputVME->IsA("mafVMEGrayVolume"))
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
  int lmNumber = LMCloud->GetNumberOfLandmarks();
  wxString  name(LandmarkName);
  name << lmNumber; 

  double position[3];
  GetSpawnPointCoordinates(position);

  //Landmark = mafVMELandmark::New();//we have a reference on the vme (we can call vtkDEL in the UNDO)
  mafNEW(Landmark);
  Landmark->SetName(name.c_str());
  Landmark->ReparentTo(LMCloud);
  Landmark->Update(); 

  //Landmark->SetPose(position[0],position[1],position[2],-1);
  double rot[3] = {0.0,0.0,0.0};
  Landmark->SetPose(position[0],position[1],position[2],rot[0], rot[1], rot[2],-1);
  
  //mafEventMacro(mafEvent(this,VME_CREATE_CLIENT_DATA,Landmark));
	mafEventMacro(mafEvent(this,VME_ADD,Landmark));
  mafEventMacro(mafEvent(this,VME_SHOW,Landmark,true)); 
	mafEventMacro(mafEvent(this,CAMERA_UPDATE)); 
  
  AttachInteractor(Landmark, IsaCompositor);

  SetGUIStatusToEnabled(); 
  SetGuiAbsPosition(Landmark->GetAbsMatrixPipe()->GetMatrixPointer()->GetVTKMatrix());     
}

void medGUILandmark::UpdateInteractor()
{ 
  IsaTranslateSnap->SetVME(Landmark);
  IsaTranslateSnap->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
  // set the pivot point
  IsaTranslateSnap->GetTranslationConstraint()->GetRefSys()->SetMatrix(RefSysVME->GetAbsMatrixPipe()->GetMatrixPointer());
  IsaTranslateSnap->GetPivotRefSys()->SetTypeToCustom(Landmark->GetOutput()->GetAbsMatrix()); 
}


//----------------------------------------------------------------------------
void medGUILandmark::OnRefSysVmeChanged()
//----------------------------------------------------------------------------
{
  RefSysVMEName = RefSysVME->GetName();
  if (Landmark) this->SetGuiAbsPosition(Landmark->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
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
  tran->SetPosition(Position[0], Position[1], Position[2]);

  // premultiply to ref sys abs matrix
  //tran->Concatenate(RefSysVME->GetOutput()->GetAbsPose(), POST_MULTIPLY);
  tran->Concatenate(*RefSysVME->GetOutput()->GetAbsMatrix(), POST_MULTIPLY);
//  Landmark-SetAbsPose(tran->GetMatrix(), CurrentTime);
  double pos[3],rot[3];
  mafTransform::GetPosition(tran->GetMatrix(), pos);
  mafTransform::GetOrientation(tran->GetMatrix(), rot);
  Landmark->SetAbsPose(pos,rot, CurrentTime);
  
  this->SetGuiAbsPosition(Landmark->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
 
   mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void medGUILandmark::SetGuiAbsPosition(vtkMatrix4x4* absPose, mafTimeStamp timeStamp)
//----------------------------------------------------------------------------
{
  // express absPose in RefSysVME refsys
 /* mafSmartPointer<mafTransform>  mflTr;
  mflTr->SetInput(absPose);
  //mflTr->SetTargetFrame(RefSysVME->GetAbsPose());
  mflTr->Update();*/

  // Express VME abs matrix in RefSysVME refsys via mafTransform
  mafSmartPointer<mafTransformFrame> mflTr;
  //mflTr->SetInput(mafMatrix(absPose));
  mafMatrix mat;
  mat.SetVTKMatrix(absPose);
  mflTr->SetInput(&mat);
  mflTr->SetTargetFrame(RefSysVME->GetOutput()->GetAbsMatrix());
  mflTr->Update();
  
  // update gui with new position 
  mafTransform::GetPosition(mflTr->GetMatrix(), Position);
  
  m_gui->Update();
}

//----------------------------------------------------------------------------
void medGUILandmark::SetRefSysVME(mafVME* refSysVME)
//----------------------------------------------------------------------------
{
  // ref sys vme must be not null
  if(refSysVME == NULL)
	return;

  RefSysVME = refSysVME;
  OnRefSysVmeChanged();
}