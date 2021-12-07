/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpDecomposeTimeVarVME
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

#include "albaOpDecomposeTimeVarVME.h"

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaGUIRollOut.h"
#include "albaGUIListBox.h"

#include "albaVMEIterator.h"
#include "albaTagArray.h"

#include "albaSmartPointer.h"
#include "albaMatrixVector.h"
#include "albaDataVector.h"
#include "albaStorageElement.h"
#include "albaMatrix.h"
#include "albaVMEFactory.h"
#include "albaVMEItem.h"
#include "albaVME.h"
#include "albaVMEGroup.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMESurface.h"

//----------------------------------------------------------------------------
albaOpDecomposeTimeVarVME::albaOpDecomposeTimeVarVME(const wxString& label) :
albaOp(label)
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
albaOpDecomposeTimeVarVME::~albaOpDecomposeTimeVarVME()
//----------------------------------------------------------------------------
{
  if (m_Group != NULL) 
  {
   

    for (int i = 0; i < m_VectorVME.size(); i++)
    {
      albaDEL(m_VectorVME[i]);
    }

		for (int i = 0; i < m_VectorCloud.size(); i++)
		{
			albaDEL(m_VectorCloud[i]);
		}
    m_Group->ReparentTo(NULL);
    albaDEL(m_Group);
  }
}

//----------------------------------------------------------------------------
albaOp* albaOpDecomposeTimeVarVME::Copy()
//----------------------------------------------------------------------------
{
  return (new albaOpDecomposeTimeVarVME(m_Label));
}

//----------------------------------------------------------------------------
bool albaOpDecomposeTimeVarVME::InternalAccept(albaVME* node)
//----------------------------------------------------------------------------
{
  //Until VMEAnalog is non time varying
  if (node->IsA("albaVMEScalarMatrix"))
  {
    return false;
  }

  return (node && node->IsAnimated());
}

//----------------------------------------------------------------------------
void albaOpDecomposeTimeVarVME::OpRun()   
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUI(this);
  m_Gui->SetListener(this);

  char string[100];
  m_NumberFrames = albaVMEGenericAbstract::SafeDownCast(m_Input)->GetNumberOfLocalTimeStamps();
  sprintf(string, "Node has %d timestamps", m_NumberFrames);
  m_Gui->Label(string);

  ModeGui();
  m_Gui->Divider();
  ShowGui();
}

//-------------------------------------------------------------------------
void albaOpDecomposeTimeVarVME::ModeGui()
//-------------------------------------------------------------------------
{
  if(m_Gui)
  {
    m_GuiFrames = new albaGUI(this);
    m_GuiInterval = new albaGUI(this);
    m_GuiPeriodicity = new albaGUI(this);
    
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
void albaOpDecomposeTimeVarVME::OnEvent(albaEventBase *alba_event) 
//----------------------------------------------------------------------------
{ 
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case wxOK:          
      { 
        if (UpdateFrames() == ALBA_OK)
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
        albaEventMacro(*alba_event); 
      }
      break;
    }
  }

  if (alba_event->GetSender() == m_RollOutFrames) // from this operation gui
  {
    albaEvent *e = albaEvent::SafeDownCast(alba_event);
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
  else if (alba_event->GetSender() == m_RollOutInterval) // from this operation gui
  {
    albaEvent *e = albaEvent::SafeDownCast(alba_event);
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
  else if (alba_event->GetSender() == m_RollOutPeriodicity) // from this operation gui
  {
    albaEvent *e = albaEvent::SafeDownCast(alba_event);
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
void albaOpDecomposeTimeVarVME::AppendFrame(char *string)
//----------------------------------------------------------------------------
{
  if (m_FramesListBox)
  {
    m_FramesListBox->Append(string);
  }

  m_FrameLabel.push_back(string);
}

//----------------------------------------------------------------------------
void albaOpDecomposeTimeVarVME::DeleteFrame(int frameId)
//----------------------------------------------------------------------------
{
  if (m_FramesListBox)
  {
    m_FramesListBox->Delete(m_ItemId);
  }
  m_FrameLabel.erase(m_FrameLabel.begin() + frameId);
}

//----------------------------------------------------------------------------
void albaOpDecomposeTimeVarVME::SelectMode(int mode)
//----------------------------------------------------------------------------
{
  if (mode < MODE_NONE || mode > MODE_PERIODICITY)
    return;
  m_InsertMode = mode;
}

//----------------------------------------------------------------------------
void albaOpDecomposeTimeVarVME::SetInterval(double from, double to)
//----------------------------------------------------------------------------
{
  if (from >= to)
    return;
  m_IntervalFrom = from;
  m_IntervalTo = to;
}

//----------------------------------------------------------------------------
void albaOpDecomposeTimeVarVME::SetPeriodicity(int period)
//----------------------------------------------------------------------------
{
  m_Periodicity = period;
}

//----------------------------------------------------------------------------
int albaOpDecomposeTimeVarVME::UpdateFrames()
//----------------------------------------------------------------------------
{
  //wxBusyInfo wait("Please wait, working...");
  std::vector<albaTimeStamp> kframes;
  albaVMEGenericAbstract *vme = albaVMEGenericAbstract::SafeDownCast(m_Input);

  vme->GetLocalTimeStamps(kframes);
  albaString name = vme->GetName();
  albaString groupName = "Decomposed from ";
  groupName << name;
  
  albaNEW(m_Group);
  m_Group->SetName(groupName);
  
  albaTimeStamp timeSt;

  switch(m_InsertMode)
  {
    case MODE_FRAMES:
    { 
      int framesNummber = m_FrameLabel.size();
      if (framesNummber > 0)
      {
        albaString timeStr;
        for (int f = 0; f < framesNummber; f++)
        {
          albaString frame = m_FrameLabel.at(f);
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
        return ALBA_ERROR;
      }
    } 
    break;
    case MODE_INTERVAL:
    {
      if (m_IntervalTo < m_IntervalFrom)
      {
        wxMessageBox( _("Not valid interval"), _("Error"), wxOK | wxICON_ERROR  ); 
        return ALBA_ERROR;
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
        return ALBA_ERROR;
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
  return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaOpDecomposeTimeVarVME::CreateStaticVME(albaTimeStamp timeSt)
//----------------------------------------------------------------------------
{
  albaMatrix *matrix;
  albaMatrix *matrixCopy;
  albaVMEItem *vmeItem;
  albaVMEItem *vmeItemCopy;
  albaTimeStamp oldTime;

  std::vector<albaTimeStamp> kframes;
  albaVMEGenericAbstract *oldVme = albaVMEGenericAbstract::SafeDownCast(m_Input);

  // restore due attributes
  albaString typeVme;
  typeVme = m_Input->GetTypeName();

  albaSmartPointer<albaVMEFactory> factory;
  albaVME *newVme = factory->CreateVMEInstance(typeVme);
  if (!newVme)
    return;

	newVme->Register(this);

  //If VME is a landmark, a landmark cloud must be created
  if (typeVme.Equals("albaVMELandmark"))
  {
    albaNEW(m_Cloud);
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
  
  albaVMEGenericAbstract *vmeGeneric = albaVMEGenericAbstract::SafeDownCast(newVme);

  albaMatrixVector *mv = oldVme->GetMatrixVector();
  if (mv)
  {
    matrix = mv->GetNearestMatrix(timeSt);
    if (matrix)
    {
      albaNEW(matrixCopy);
      matrixCopy->DeepCopy(matrix);
      vmeGeneric->GetMatrixVector()->SetMatrix(matrixCopy);
      oldTime = matrix->GetTimeStamp();
      albaDEL(matrixCopy);
    }
  }

  albaDataVector *dv = oldVme->GetDataVector();
  if (dv)
  {
    vmeItem = dv->GetNearestItem(timeSt);
    if (vmeItem)
    {
      vmeItemCopy = (albaVMEItem*)factory->CreateInstance(vmeItem->GetTypeName());
      vmeItemCopy->DeepCopy(vmeItem);
      vmeGeneric->GetDataVector()->AppendItem(vmeItemCopy);
      oldTime = vmeItem->GetTimeStamp();
    }
  }

  albaString newName;
  albaString timeStr;
  albaString vme_name;
 
  //Add the timestamp information to the name
  vme_name = oldVme->GetName();   
  timeStr = wxString::Format("_%.3f", oldTime);
  newName = vme_name;
  newName << timeStr;
  newVme->SetName(newName.GetCStr());  

  //If VME is a landmark, the new landmark must be added to the landmark cloud created
  if (m_Cloud != NULL)
  {
    albaString cloudName = "cloud_";
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
void albaOpDecomposeTimeVarVME::EnableWidget(bool enable)
//----------------------------------------------------------------------------
{
  m_Gui->Enable(wxOK, enable);
}
