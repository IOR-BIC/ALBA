/*=========================================================================

 Program: MAF2
 Module: mafOpDecomposeTimeVarVME
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpDecomposeTimeVarVME.h"

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafGUIRollOut.h"
#include "mafGUIListBox.h"

#include "mafVMEIterator.h"
#include "mafTagArray.h"

#include "mafSmartPointer.h"
#include "mafMatrixVector.h"
#include "mafDataVector.h"
#include "mafStorageElement.h"
#include "mafMatrix.h"
#include "mafVMEFactory.h"
#include "mafVMEItem.h"
#include "mafVME.h"
#include "mafVMEGroup.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMESurface.h"

//----------------------------------------------------------------------------
mafOpDecomposeTimeVarVME::mafOpDecomposeTimeVarVME(const wxString& label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType    = OPTYPE_OP;
  m_Canundo   = true;
  m_InsertMode = MODE_NONE;
  m_IntervalFrom = 0;
  m_IntervalTo = 0;
  m_FramesListBox = NULL;
  m_Frame = 0;
  m_Group = NULL;
  m_Cloud = NULL;
  m_VectorVME.clear();
  m_VectorCloud.clear();
}

//----------------------------------------------------------------------------
mafOpDecomposeTimeVarVME::~mafOpDecomposeTimeVarVME()
//----------------------------------------------------------------------------
{
  if (m_Group != NULL) 
  {
   

    for (int i = 0; i < m_VectorVME.size(); i++)
    {
      mafDEL(m_VectorVME[i]);
    }

		for (int i = 0; i < m_VectorCloud.size(); i++)
		{
			mafDEL(m_VectorCloud[i]);
		}
    m_Group->ReparentTo(NULL);
    mafDEL(m_Group);
  }
}

//----------------------------------------------------------------------------
mafOp* mafOpDecomposeTimeVarVME::Copy()
//----------------------------------------------------------------------------
{
  return (new mafOpDecomposeTimeVarVME(m_Label));
}

//----------------------------------------------------------------------------
bool mafOpDecomposeTimeVarVME::Accept(mafVME* node)
//----------------------------------------------------------------------------
{
  //Until VMEAnalog is non time varying
  if (node->IsA("mafVMEScalarMatrix"))
  {
    return false;
  }

  return (node && node->IsAnimated());
}

//----------------------------------------------------------------------------
void mafOpDecomposeTimeVarVME::OpRun()   
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);

  mafEvent buildHelpGui;
  buildHelpGui.SetSender(this);
  buildHelpGui.SetId(GET_BUILD_HELP_GUI);
  mafEventMacro(buildHelpGui);

  if (buildHelpGui.GetArg() == true)
  {
	  m_Gui->Button(ID_HELP, "Help","");	
  }

  char string[100];
  m_NumberFrames = mafVMEGenericAbstract::SafeDownCast(m_Input)->GetNumberOfLocalTimeStamps();
  sprintf(string, "Node has %d timestamps", m_NumberFrames);
  m_Gui->Label(string);

  ModeGui();
  m_Gui->Divider();
  ShowGui();
}

//-------------------------------------------------------------------------
void mafOpDecomposeTimeVarVME::ModeGui()
//-------------------------------------------------------------------------
{
  if(m_Gui)
  {
    m_GuiFrames = new mafGUI(this);
    m_GuiInterval = new mafGUI(this);
    m_GuiPeriodicity = new mafGUI(this);
    
    m_GuiFrames->Button(ID_INSERT_FRAME, _("Add timestamp"), "", _("Add a timestamp"));
    m_GuiFrames->Button(ID_REMOVE_FRAME, _("Remove timestamp"), "", _("Remove a timestamp"));

    double min = 0;
    m_GuiFrames->Double(ID_FRAME, _("Timestamp"), &m_Frame, min);

    m_FramesListBox =  m_GuiFrames->ListBox(ID_LIST_FRAMES,_("List"),60,_("Chose label to visualize"));
  
    m_GuiInterval->Double(CHANGE_VALUE_INTERVAL, _("From"), &m_IntervalFrom, min );
    m_GuiInterval->Double(CHANGE_VALUE_INTERVAL, _("To"), &m_IntervalTo, min, m_NumberFrames);

    m_GuiPeriodicity->Integer(CHANGE_VALUE_PERIODICITY, _("Period"), &m_Periodicity, min, m_NumberFrames);

    m_RollOutFrames = m_Gui->RollOut(ID_ROLLOUT_FRAMES,_("Timestamps mode"), m_GuiFrames, false);
    m_RollOutInterval = m_Gui->RollOut(ID_ROLLOUT_INTERVAL,_("Interval mode"), m_GuiInterval, false);
    m_RollOutPeriodicity = m_Gui->RollOut(ID_ROLLOUT_PERIODICITY,_("Periodicity mode"), m_GuiPeriodicity, false);

    m_Gui->OkCancel();
    m_Gui->Divider();
    EnableWidget(false);
    m_Gui->Update();
  }
}

//----------------------------------------------------------------------------
void mafOpDecomposeTimeVarVME::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{ 
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
	   case ID_HELP:
	   {
			mafEvent helpEvent;
			helpEvent.SetSender(this);
			mafString operationLabel = this->m_Label;
			helpEvent.SetString(&operationLabel);
			helpEvent.SetId(OPEN_HELP_PAGE);
			mafEventMacro(helpEvent);
	  }
	  break;

      case wxOK:          
      { 
        if (UpdateFrames() == MAF_OK)
        {
          OpStop(OP_RUN_OK);
        }
        else
        {
          OpStop(OP_RUN_CANCEL);
        }
        break;
      }
      case wxCANCEL:
      {
        OpStop(OP_RUN_CANCEL);
        break;
      }
      case ID_INSERT_FRAME:
      {
        char frameStr[50];
        int n;
        n = sprintf(frameStr, "%f", m_Frame);

        //Check if the label already exists
        int pos = m_FramesListBox->FindString(frameStr);
        if (pos == -1)
        { 
          AppendFrame(frameStr);
        }
        else
        {
          wxMessageBox( _("Frame already inserted"), _("Error"), wxOK | wxICON_ERROR  ); 
          return;
        }
      }
      break;
      case ID_REMOVE_FRAME:
      {
        DeleteFrame(m_ItemId);
        break;
      }
      case ID_LIST_FRAMES:
      {
        m_ItemId = e->GetArg();
        break;
      }
      default:
      {
        mafEventMacro(*maf_event); 
      }
      break;
    }
  }

  if (maf_event->GetSender() == m_RollOutFrames) // from this operation gui
  {
    mafEvent *e = mafEvent::SafeDownCast(maf_event);
    if  (e->GetBool())
    {
      m_InsertMode = MODE_FRAMES;           
      m_RollOutInterval->RollOut(false);
      m_RollOutPeriodicity->RollOut(false);
      EnableWidget(true);
    }
    else
    {
      m_InsertMode = MODE_NONE;   
      EnableWidget(false);
    }
  }
  else if (maf_event->GetSender() == m_RollOutInterval) // from this operation gui
  {
    mafEvent *e = mafEvent::SafeDownCast(maf_event);
    if  (e->GetBool())
    {
      m_InsertMode = MODE_INTERVAL;           
      m_RollOutFrames->RollOut(false);
      m_RollOutPeriodicity->RollOut(false);
      EnableWidget(true);
    }
    else
    {
      m_InsertMode = MODE_NONE;
      EnableWidget(false);
    }
  }
  else if (maf_event->GetSender() == m_RollOutPeriodicity) // from this operation gui
  {
    mafEvent *e = mafEvent::SafeDownCast(maf_event);
    if  (e->GetBool())
    {
      m_InsertMode = MODE_PERIODICITY;           
      m_RollOutFrames->RollOut(false);
      m_RollOutInterval->RollOut(false);
      EnableWidget(true);
    }
    else
    {
      m_InsertMode = MODE_NONE;  
      EnableWidget(false);
    }
  }
}

//----------------------------------------------------------------------------
void mafOpDecomposeTimeVarVME::AppendFrame(char *string)
//----------------------------------------------------------------------------
{
  if (m_FramesListBox)
  {
    m_FramesListBox->Append(string);
  }

  m_FrameLabel.push_back(string);
}

//----------------------------------------------------------------------------
void mafOpDecomposeTimeVarVME::DeleteFrame(int frameId)
//----------------------------------------------------------------------------
{
  if (m_FramesListBox)
  {
    m_FramesListBox->Delete(m_ItemId);
  }
  m_FrameLabel.erase(m_FrameLabel.begin() + frameId);
}

//----------------------------------------------------------------------------
void mafOpDecomposeTimeVarVME::SelectMode(int mode)
//----------------------------------------------------------------------------
{
  if (mode < MODE_NONE || mode > MODE_PERIODICITY)
    return;
  m_InsertMode = mode;
}

//----------------------------------------------------------------------------
void mafOpDecomposeTimeVarVME::SetInterval(double from, double to)
//----------------------------------------------------------------------------
{
  if (from >= to)
    return;
  m_IntervalFrom = from;
  m_IntervalTo = to;
}

//----------------------------------------------------------------------------
void mafOpDecomposeTimeVarVME::SetPeriodicity(int period)
//----------------------------------------------------------------------------
{
  m_Periodicity = period;
}

//----------------------------------------------------------------------------
int mafOpDecomposeTimeVarVME::UpdateFrames()
//----------------------------------------------------------------------------
{
  //wxBusyInfo wait("Please wait, working...");
  std::vector<mafTimeStamp> kframes;
  mafVMEGenericAbstract *vme = mafVMEGenericAbstract::SafeDownCast(m_Input);

  vme->GetLocalTimeStamps(kframes);
  mafString name = vme->GetName();
  mafString groupName = "Decomposed from ";
  groupName << name;
  
  mafNEW(m_Group);
  m_Group->SetName(groupName);
  
  mafTimeStamp timeSt;

  switch(m_InsertMode)
  {
    case MODE_FRAMES:
    { 
      int framesNummber = m_FrameLabel.size();
      if (framesNummber > 0)
      {
        mafString timeStr;
        for (int f = 0; f < framesNummber; f++)
        {
          mafString frame = m_FrameLabel.at(f);
          timeSt = atof(frame.GetCStr());
          //check if the inserted frame is inside the timestamps bounds
          if (timeSt < kframes[0] || timeSt > kframes[kframes.size()-1])
          {
            char errorMessage[50];
            sprintf(errorMessage, "Frame %s is outside timestamps bounds!", frame);
            wxMessageBox( errorMessage, _("Warning"), wxOK | wxICON_ERROR  ); 
          }
          CreateStaticVME(timeSt);
        }
      }
      else
      {
        return MAF_ERROR;
      }
    } 
    break;
    case MODE_INTERVAL:
    {
      if (m_IntervalTo < m_IntervalFrom)
      {
        wxMessageBox( _("Not valid interval"), _("Error"), wxOK | wxICON_ERROR  ); 
        return MAF_ERROR;
      }

      int start, end;
      bool startFound = false;
      bool endFound = false;

      for (int n = 0; n < kframes.size(); n++)
      {
        if (m_IntervalFrom == kframes[n])
        {
          start = n;
          startFound = true;
        }
        if (m_IntervalTo == kframes[n])
        {
          end = n;
          endFound = true;
          break;
        }
      }
      if (!startFound)
      {
        if (m_IntervalFrom < kframes[0])
          start = 0;
      }
      if (!endFound)
      {
        if (m_IntervalTo > kframes[kframes.size()-1])
          end = (kframes.size()-1);
      }
      for (int n = start; n <= end; n++)
      {
        CreateStaticVME(kframes[n]);
      }
    }
    break;
    case MODE_PERIODICITY:
    {
      vme->GetLocalTimeStamps(kframes);

      //Check if period is valid
      if(m_Periodicity == 0 || m_Periodicity == kframes.size())
      {
        wxMessageBox( _("Not valid period"), _("Error"), wxOK | wxICON_ERROR  ); 
        return MAF_ERROR;
      }

      int pCounter = 0;
      for (int n = 0; n <= kframes.size(); n++)
      {
        pCounter++;
        if (pCounter == m_Periodicity)
        {
          timeSt = kframes[n];
          //Create new static VME with var "time"
          CreateStaticVME(timeSt);
          pCounter = 0;
        }
      }  
    }
    break;
  }
  m_Output = m_Group;
  return MAF_OK;
}

//----------------------------------------------------------------------------
void mafOpDecomposeTimeVarVME::CreateStaticVME(mafTimeStamp timeSt)
//----------------------------------------------------------------------------
{
  mafMatrix *matrix;
  mafMatrix *matrixCopy;
  mafVMEItem *vmeItem;
  mafVMEItem *vmeItemCopy;
  mafTimeStamp oldTime;

  std::vector<mafTimeStamp> kframes;
  mafVMEGenericAbstract *oldVme = mafVMEGenericAbstract::SafeDownCast(m_Input);

  // restore due attributes
  mafString typeVme;
  typeVme = m_Input->GetTypeName();

  mafSmartPointer<mafVMEFactory> factory;
  mafVME *newVme = factory->CreateVMEInstance(typeVme);
  if (!newVme)
    return;

	newVme->Register(this);

  //If VME is a landmark, a landmark cloud must be created
  if (typeVme.Equals("mafVMELandmark"))
  {
    mafNEW(m_Cloud);
	if (m_TestMode == true)
	{
		m_Cloud->TestModeOn();
	}
    m_VectorCloud.push_back(m_Cloud);
  }
  else
  {
    m_VectorVME.push_back(newVme);
  }
  
  newVme->GetTagArray()->DeepCopy(oldVme->GetTagArray());
  
  mafVMEGenericAbstract *vmeGeneric = mafVMEGenericAbstract::SafeDownCast(newVme);

  mafMatrixVector *mv = oldVme->GetMatrixVector();
  if (mv)
  {
    matrix = mv->GetNearestMatrix(timeSt);
    if (matrix)
    {
      mafNEW(matrixCopy);
      matrixCopy->DeepCopy(matrix);
      vmeGeneric->GetMatrixVector()->SetMatrix(matrixCopy);
      oldTime = matrix->GetTimeStamp();
      mafDEL(matrixCopy);
    }
  }

  mafDataVector *dv = oldVme->GetDataVector();
  if (dv)
  {
    vmeItem = dv->GetNearestItem(timeSt);
    if (vmeItem)
    {
      vmeItemCopy = (mafVMEItem*)factory->CreateInstance(vmeItem->GetTypeName());
      vmeItemCopy->DeepCopy(vmeItem);
      vmeGeneric->GetDataVector()->AppendItem(vmeItemCopy);
      oldTime = vmeItem->GetTimeStamp();
    }
  }

  mafString newName;
  mafString timeStr;
  mafString vme_name;
 
  //Add the timestamp information to the name
  vme_name = oldVme->GetName();   
  timeStr = wxString::Format("_%.3f", oldTime);
  newName = vme_name;
  newName << timeStr;
  newVme->SetName(newName.GetCStr());  

  //If VME is a landmark, the new landmark must be added to the landmark cloud created
  if (m_Cloud != NULL)
  {
    mafString cloudName = "cloud_";
    cloudName << newName;
    m_Cloud->SetName(cloudName.GetCStr());
    newVme->ReparentTo(m_Cloud);
    m_Cloud->ReparentTo(m_Group);
    m_Group->Update();
  }
  else
  {
    newVme->ReparentTo(m_Group);
    m_Group->Update();
  }

}
//----------------------------------------------------------------------------
void mafOpDecomposeTimeVarVME::EnableWidget(bool enable)
//----------------------------------------------------------------------------
{
  m_Gui->Enable(wxOK, enable);
}
