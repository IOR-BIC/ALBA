/*=========================================================================

  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgTrackerSettings.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-30 14:34:57 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone

==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
// wxWindows related includes must stay first of all!
#ifdef __GNUG__
    #pragma implementation "mmgTrackerSettings.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "mmgTrackerSettings.h"
#include "mmgAvatarSettings.h"
#include "mmgGui.h"

#include "mmdTracker.h"
#include "mafAvatar.h"
#include "mafAvatar3D.h"
#include "mmi6DOF.h"

#include "mafInteractionFactory.h"

#include "mflCameraTransform.h"
#include "mflBounds.h"

mafCxxTypeMacro(mmgTrackerSettings);

//----------------------------------------------------------------------------
mmgTrackerSettings::mmgTrackerSettings(mmdTracker *tracker):
mmgDeviceSettings(tracker)
//----------------------------------------------------------------------------
{
  AvatarFlag = 0;
  TBPosition[0] = TBPosition[1] = TBPosition[2] = 0;
  
}
//----------------------------------------------------------------------------
mmgTrackerSettings::~mmgTrackerSettings()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mmdTracker *mmgTrackerSettings::GetTracker()
//----------------------------------------------------------------------------
{
  return (mmdTracker *)Device;
}
//----------------------------------------------------------------------------
int mmgTrackerSettings::AvatarChooser(wxString &avatar_name,wxString &avatar_type)
//----------------------------------------------------------------------------
{
  mafInteractionFactory *IFactory=mafInteractionFactory::GetInstance();
  
  assert(IFactory);

  if (IFactory->GetNumberOfAvatars()==0)
  {
    wxMessageBox("No avatars available!","Avatar Chooser Error",wxOK|wxICON_ERROR|wxCENTRE);
    return -1;
  }

  wxString *avatars = new wxString[IFactory->GetNumberOfAvatars()];

  for (int id=0;id<IFactory->GetNumberOfAvatars();id++)
  {
    avatars[id]=IFactory->GetAvatarName(id);
  }

  int index = -1;

  wxSingleChoiceDialog chooser(Gui,"select an avatar","Avatar Chooser",IFactory->GetNumberOfAvatars(),avatars);

  if (chooser.ShowModal()==wxID_OK )
  {
    index = chooser.GetSelection();
    if (index>=0)
    {
      avatar_type = IFactory->GetAvatarType(index);
      avatar_name = IFactory->GetAvatarName(index);
    }
  }
  
  delete [] avatars;
  return index;
}
//----------------------------------------------------------------------------
void mmgTrackerSettings::CreateGui()
//----------------------------------------------------------------------------
{
  

  mmgDeviceSettings::CreateGui();
  Gui->Divider(1);
  
  Gui->Bool(ID_AVATAR_CHECK,"avatar",&AvatarFlag,true,"enable the default avatar for the tracker");
  Gui->Button(ID_AVATAR_SELECT,"set avatar");
  Gui->Enable(ID_AVATAR_SELECT,false);

  Gui->Divider(1);
  Gui->VectorN(ID_TB_X_EXTENT,"X extent",	GetTracker()->GetTrackedBounds()->Bounds,2, MINFLOAT, MAXFLOAT, -1);
	Gui->VectorN(ID_TB_Y_EXTENT,"Y extent",	&(GetTracker()->GetTrackedBounds()->Bounds[2]),2, MINFLOAT, MAXFLOAT, -1);
  Gui->VectorN(ID_TB_Z_EXTENT,"Z extent",	&(GetTracker()->GetTrackedBounds()->Bounds[4]),2, MINFLOAT, MAXFLOAT, -1);
  Gui->Divider();
  Gui->Vector(ID_TB_POSITION,"position",	TBPosition, MINFLOAT, MAXFLOAT, -1);
	Gui->Vector(ID_TB_ORIENTATION,"orientation",	GetTracker()->GetTrackedBoxOrientation(), MINFLOAT, MAXFLOAT, -1);
  Gui->Divider();
  
  //Update();
}

//----------------------------------------------------------------------------
void mmgTrackerSettings::Update()
//----------------------------------------------------------------------------
{
  
  if (AvatarFlag=GetTracker()->GetAvatar()!=NULL) 
  {
    mafAvatar *avatar=GetTracker()->GetAvatar();
    avatar->GetSettings()->Update();
   
  }
  if (Gui)
    Gui->Enable(ID_AVATAR_SELECT,AvatarFlag!=0);
  
  mmgDeviceSettings::Update();
}

//----------------------------------------------------------------------------
void mmgTrackerSettings::OnEvent(mafEvent& e)
//----------------------------------------------------------------------------
{
  switch(e.GetId()) 
  {
  // Recompute the transform from the Tracker to the Canonical coordinate systems
  case ID_TB_POSITION:
    {
      // Translate bounds to the new center
      GetTracker()->GetTrackedBounds()->SetCenter(TBPosition);
      Gui->Update();
      // force transform update
    GetTracker()->ComputeTrackerToCanonicalTansform();
    }
  case ID_TB_ORIENTATION:
    GetTracker()->GetTrackedBounds()->Modified();
    // force transform update
    GetTracker()->ComputeTrackerToCanonicalTansform();
    break;
  case ID_AVATAR_CHECK:
    Gui->Enable(ID_AVATAR_SELECT,AvatarFlag!=0);
    if (!AvatarFlag)
    {
      GetTracker()->SetDefaultAvatar(NULL);
    }
    Gui->Update();
    break;
  case ID_AVATAR_SELECT:
    {
      wxString avatar_name,avatar_type;
      int sel=AvatarChooser(avatar_name,avatar_type);
    
      if (sel>=0)
      {
        // Set the default avatar for the tracker
        //no SafeDownCast in mafAvatar3D: how can I create an mafAvatar3D from mafAvatar
        mafAvatar *avatar = mafInteractionFactory::CreateAvatarInstance(avatar_type);
        assert(avatar);
        avatar->SetName(avatar_name);
        GetTracker()->SetDefaultAvatar(avatar);
        avatar->Delete();
        Gui->Update();
      }
    }
    break;
  // this should be moved to avatar settings!!!
  case ID_TB_X_EXTENT:
  case ID_TB_Y_EXTENT:
  case ID_TB_Z_EXTENT:
    GetTracker()->GetTrackedBounds()->Modified();
    GetTracker()->GetTrackedBounds()->GetCenter(TBPosition);
    Gui->Update();
    // force transform update
    GetTracker()->ComputeTrackerToCanonicalTansform();
    break;
  default:
    mmgDeviceSettings::OnEvent(e);
  }
}
