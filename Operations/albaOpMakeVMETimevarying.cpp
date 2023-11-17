/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMakeVMETimevarying
 Authors: Alberto Losi
 
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

#include "albaOpMakeVMETimevarying.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaVME.h"
#include "albaVMEItemVTK.h"
#include "albaDataVector.h"
#include "albaMatrixVector.h"
#include "albaMatrix.h"
#include "vtkDataSet.h"
#include "albaVMEImage.h"
#include "albaVMEMesh.h"
#include "albaVMEPointSet.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMEPolyLine.h"
#include "albaVMEVector.h"
#include "albaVMESurface.h"
#include "albaVMEVolume.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEVolumeRGB.h"
#include "albaVMEFactory.h"
#include "albaVMESurfaceParametric.h"
#include "albaMatrixPipe.h"
#include "albaAbsMatrixPipe.h"
#include "albaDataPipe.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpMakeVMETimevarying);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum MAKE_VME_TIMEVARYING_ID
{
  //GUI

  //VME LIST PANEL
  ID_ADDED_VMEs_LISTBOX = MINID,
  ID_ADD_VME_BUTTON,

  //ADD VME PANEL
  ID_TIMESTAMP_TEXTBOX,
  ID_TIMESTAMP_FROM_VME_LABEL,
  ID_TIMESTAMP_FROM_VME_LABEL_DATA,
  ID_TIMESTAMP_AUTOINCREMENT_TEXTBOX,
  ID_TIMESTAMP_MODALITY_RADIO,
  ID_ADD_VME_OK_BUTTON,
  ID_ADD_VME_CANCEL_BUTTON,
 
  //MAKE VME PANEL
  ID_DELETE_VME_BUTTON,
  ID_VMETIMEVARYING_NAME_TEXTBOX,
  ID_RESET_BUTTON,
  ID_OK_BUTTON,
  ID_CANCEL_BUTTON,

  /*//Constant for AcceptVME return values
  ID_ACCEPT_VME_ACCEPTED,
  ID_ACCEPT_WRONG_VME_TYPE,
  ID_ACCEPT_WRONG_VME_OUTPUT_TYPE,
  ID_ACCEPT_WRONG_VME_OUTPUT_VTKDATA_TYPE,*/
};

//----------------------------------------------------------------------------
albaOpMakeVMETimevarying::albaOpMakeVMETimevarying(const wxString &label /* = "Make Timevariyng VME" */,bool showShadingPlane /* = false */) : albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo	= true;  

  m_VMETimevarying = NULL;
  m_CurrentVME = NULL;
  m_CurrentVMEName = _("");
  m_VMETimevaryingName = _("New Timevariyng VME");
  m_VMEsType = _("albaVME");
  m_VMEsOutputType = _("albaVMEOutput");
  m_VMEsVTKDataType = VTK_DATA_SET;

  m_AddedVMEsListBox = NULL;

  //m_NumOfAddedVMEs = 0;
  m_CurrentVMETimeStamp = 0.0;
  m_VMETimestampIncrement = 0.01;
  m_CurrentVMETimeStampFromVME = 0.0;
  m_CurrentVMETimestampModality = 0;
}
//----------------------------------------------------------------------------
albaOpMakeVMETimevarying::~albaOpMakeVMETimevarying()
//----------------------------------------------------------------------------
{
  for(int i = 0; i < m_AddedVMEs.size(); i++)
  {
    free(m_AddedVMEs.at(i)); // Remove leaks (with free: space allocated with malloc)
  }
  m_AddedVMEs.clear();
  albaDEL(m_VMETimevarying);
}
//----------------------------------------------------------------------------
bool albaOpMakeVMETimevarying::InternalAccept(albaVME* vme) 
//----------------------------------------------------------------------------
{
	albaEvent e(this,VIEW_SELECTED);
	albaEventMacro(e);
  return true;
}
//----------------------------------------------------------------------------
albaOp *albaOpMakeVMETimevarying::Copy()
//----------------------------------------------------------------------------
{
	return new albaOpMakeVMETimevarying(m_Label);
}

//----------------------------------------------------------------------------
void albaOpMakeVMETimevarying::OpRun()
//----------------------------------------------------------------------------
{
  CreateGui();
}
//----------------------------------------------------------------------------
void albaOpMakeVMETimevarying::OpDo()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpMakeVMETimevarying::OpUndo()
//----------------------------------------------------------------------------
{
  GetLogicManager()->VmeRemove(m_Output);
  albaDEL(m_Output);
}
//----------------------------------------------------------------------------
void albaOpMakeVMETimevarying::CreateGui() 
//----------------------------------------------------------------------------
{
  //Create Graphical User Interface

  m_Gui = new albaGUI(this);

  m_Gui->Divider();

  //------VME List panel------//
  m_Gui->Label(_("Added VMEs"));
  m_AddedVMEsListBox = m_Gui->ListBox(ID_ADDED_VMEs_LISTBOX,_(""),200);
  
  m_Gui->TwoButtons(ID_ADD_VME_BUTTON,ID_DELETE_VME_BUTTON,_("Add"),_("Delete"));
  m_Gui->Enable(ID_DELETE_VME_BUTTON,false);
  //-------------------------//

  m_Gui->Divider();
  m_Gui->Divider();

  //------Add VME panel------//
  m_Gui->Label(&m_CurrentVMEName,true);
  wxString radioChoice[2];
  radioChoice[0] = _("User input");
  radioChoice[1] = _("From VME");
  m_Gui->Radio(ID_TIMESTAMP_MODALITY_RADIO,_("Timestamp Modality"),&m_CurrentVMETimestampModality,2,radioChoice);
  m_Gui->Double(ID_TIMESTAMP_TEXTBOX,_("User input"),&m_CurrentVMETimeStamp);
  wxBoxSizer * hszVMETimeStampFromVME = new wxBoxSizer(wxHORIZONTAL);
  m_LblVMETimeStampFromVME = new wxStaticText(m_Gui,ID_TIMESTAMP_FROM_VME_LABEL,_("From VME"),wxDefaultPosition,wxSize(55,17),wxALIGN_RIGHT);
  m_LblVMETimeStampFromVMEData = new wxStaticText(m_Gui,ID_TIMESTAMP_FROM_VME_LABEL_DATA,albaString::Format("%f",m_CurrentVMETimeStampFromVME),wxDefaultPosition,wxSize(100,17));
  m_LblVMETimeStampFromVME->Enable(false);
  m_LblVMETimeStampFromVMEData->Enable(false);
	hszVMETimeStampFromVME->Add(m_LblVMETimeStampFromVME,0,wxRIGHT, 5);
	hszVMETimeStampFromVME->Add(m_LblVMETimeStampFromVMEData,0,wxEXPAND || wxLEFT);
  m_Gui->Add(hszVMETimeStampFromVME);
  m_Gui->Double(ID_TIMESTAMP_AUTOINCREMENT_TEXTBOX,_("Increment"),&m_VMETimestampIncrement);
  m_Gui->Divider();
  m_Gui->TwoButtons(ID_ADD_VME_OK_BUTTON,ID_ADD_VME_CANCEL_BUTTON,_("Ok"),_("Cancel"));
  m_Gui->Enable(ID_TIMESTAMP_TEXTBOX,false);
  m_Gui->Enable(ID_TIMESTAMP_AUTOINCREMENT_TEXTBOX,false);
  m_Gui->Enable(ID_TIMESTAMP_MODALITY_RADIO,false);
  m_Gui->Enable(ID_ADD_VME_OK_BUTTON,false);
  m_Gui->Enable(ID_ADD_VME_CANCEL_BUTTON,false);
  //-------------------------//

  m_Gui->Divider();
  m_Gui->Divider();

  //------Make VME panel------//
  m_Gui->Label(_("Name"));
  m_Gui->String(ID_VMETIMEVARYING_NAME_TEXTBOX,_(""),&m_VMETimevaryingName);
  m_Gui->Divider();
  m_Gui->Button(ID_RESET_BUTTON,_("Reset"));
  m_Gui->Enable(ID_RESET_BUTTON,false);
  m_Gui->TwoButtons(ID_OK_BUTTON,ID_CANCEL_BUTTON,_("Ok"),_("Cancel"));
  m_Gui->Enable(ID_OK_BUTTON,false);
  //--------------------------//

	ShowGui();
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpMakeVMETimevarying::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      //------------------------------------------//
      case ID_ADDED_VMEs_LISTBOX:
      {
        //Update GUI
        m_Gui->Enable(ID_DELETE_VME_BUTTON,true);
        break;
      }
      //------------------------------------------//
/*
      case ID_TIMESTAMP_TEXTBOX:
      {
        m_Gui->Enable(ID_RESET_BUTTON,true);
        break;
      }
*/
      //------------------------------------------//
      case ID_ADD_VME_BUTTON:
      {
        albaString title = _("Choose VME");
        albaEvent e(this,VME_CHOOSE,&title);
				e.SetPointer(&albaOpMakeVMETimevarying::DialogAcceptVME);
        albaEventMacro(e);
        
        //albaNEW(m_CurrentVME);
        m_CurrentVME = e.GetVme();
        bool VMEOk = AcceptVME(m_CurrentVME);
        if(m_CurrentVME)
        {
          if( VMEOk)
          {
            m_CurrentVMEName = m_CurrentVME->GetName();
            albaVMEGeneric * tmpCurrentVME;
            tmpCurrentVME = albaVMEGeneric::SafeDownCast(m_CurrentVME);
            mmuTimeVector vmeTimeStamps;
            if(tmpCurrentVME) //Vme is not a parametric surface
            {
              tmpCurrentVME->GetDataVector()->GetTimeStamps(vmeTimeStamps);
              m_CurrentVMETimeStampFromVME = (* vmeTimeStamps.begin()); //Load the VME's timestamp (for non static VME the first)
            }
            else
              m_CurrentVMETimeStampFromVME = 0;
            //m_Gui->Update();
            m_LblVMETimeStampFromVMEData->SetLabel(albaString::Format("%f",m_CurrentVMETimeStampFromVME));
            //TransferDataToWindow(); //Manually forces GUI update for custom created control
            //Update GUI
            if(0 == m_CurrentVMETimestampModality)
            {
              m_Gui->Enable(ID_TIMESTAMP_TEXTBOX,true);
              m_LblVMETimeStampFromVME->Enable(false);
              m_LblVMETimeStampFromVMEData->Enable(false);
            }
            else if (1 == m_CurrentVMETimestampModality)
            {
              m_Gui->Enable(ID_TIMESTAMP_TEXTBOX,false);
              m_LblVMETimeStampFromVME->Enable(true);
              m_LblVMETimeStampFromVMEData->Enable(true);
            }
            m_Gui->Enable(ID_ADD_VME_BUTTON,false);
            m_Gui->Enable(ID_DELETE_VME_BUTTON,false);
            m_Gui->Enable(ID_TIMESTAMP_AUTOINCREMENT_TEXTBOX,true);
            m_Gui->Enable(ID_TIMESTAMP_MODALITY_RADIO,true);
            m_Gui->Enable(ID_ADD_VME_OK_BUTTON,true);
            m_Gui->Enable(ID_ADD_VME_CANCEL_BUTTON,true);
            m_Gui->Enable(ID_RESET_BUTTON,false);
            m_Gui->Enable(ID_OK_BUTTON,false);
            m_Gui->Enable(ID_CANCEL_BUTTON,false);
            m_Gui->Update();
          }
          else
          {
            wxMessageBox(_("Cannot add a VME that represent a different object."),_("Error: Wrong VME's Type"),wxOK | wxICON_ERROR);
          }
        }
        break;
      }
      //------------------------------------------//
      case ID_TIMESTAMP_MODALITY_RADIO:
      {
        //Update GUI
        if(0 == m_CurrentVMETimestampModality)
        {
          m_Gui->Enable(ID_TIMESTAMP_TEXTBOX,true);
          m_LblVMETimeStampFromVME->Enable(false);
          m_LblVMETimeStampFromVMEData->Enable(false);
        }
        else if (1 == m_CurrentVMETimestampModality)
        {
          m_Gui->Enable(ID_TIMESTAMP_TEXTBOX,false);
          m_LblVMETimeStampFromVME->Enable(true);
          m_LblVMETimeStampFromVMEData->Enable(true);
        }
        break;
      }
      //------------------------------------------//
      case ID_ADD_VME_OK_BUTTON:
      {
        double currentTimestamp=0;
        bool TimeStampOk;
        if(0 == m_CurrentVMETimestampModality)
        {
          currentTimestamp = m_CurrentVMETimeStamp;
        }
        else if (1 == m_CurrentVMETimestampModality)
        {
         currentTimestamp=  m_CurrentVMETimeStampFromVME;
        }
        TimeStampOk = AcceptTimeStamp(currentTimestamp);
        if(TimeStampOk)
        {
          AddVME(m_CurrentVME,currentTimestamp);
          //Update GUI
          m_CurrentVMEName = _("");
          UpdateAddedVMEsListBox();
          m_Gui->Enable(ID_OK_BUTTON,true);
          m_Gui->Enable(ID_RESET_BUTTON,true);
          m_Gui->Enable(ID_CANCEL_BUTTON,true);
          m_Gui->Enable(ID_ADD_VME_BUTTON,true);
          m_Gui->Enable(ID_DELETE_VME_BUTTON,true);
          m_Gui->Enable(ID_RESET_BUTTON,true);
          m_CurrentVMETimeStamp = m_CurrentVMETimeStamp + m_VMETimestampIncrement; //Autoincrease current timestamp
          m_Gui->Enable(ID_TIMESTAMP_TEXTBOX,false);
          m_LblVMETimeStampFromVME->Enable(false);
          m_LblVMETimeStampFromVMEData->Enable(false);
          m_Gui->Enable(ID_TIMESTAMP_AUTOINCREMENT_TEXTBOX,false);
          m_Gui->Enable(ID_TIMESTAMP_MODALITY_RADIO,false);
          m_Gui->Enable(ID_ADD_VME_OK_BUTTON,false);
          m_Gui->Enable(ID_ADD_VME_CANCEL_BUTTON,false);
          m_Gui->Update();
          m_CurrentVME = NULL;
        }
        else
        {
          wxMessageBox(_("Cannot add VMEs with the same TimeStamp."),_("Error: Already existing TimeStamp"),wxOK | wxICON_ERROR);
        }
        break;
      }
      //------------------------------------------//
      case ID_ADD_VME_CANCEL_BUTTON:
      {
        m_CurrentVMEName = _("");
        //Update GUI
        m_Gui->Enable(ID_TIMESTAMP_TEXTBOX,false);
        m_LblVMETimeStampFromVME->Enable(false);
        m_LblVMETimeStampFromVMEData->Enable(false);
        m_Gui->Enable(ID_TIMESTAMP_AUTOINCREMENT_TEXTBOX,false);
        m_Gui->Enable(ID_TIMESTAMP_MODALITY_RADIO,false);
        m_Gui->Enable(ID_ADD_VME_OK_BUTTON,false);
        m_Gui->Enable(ID_ADD_VME_CANCEL_BUTTON,false);
        m_Gui->Enable(ID_ADD_VME_BUTTON,true);
        if (0 != m_AddedVMEs.size())
        {
          m_Gui->Enable(ID_DELETE_VME_BUTTON,true);
          m_Gui->Enable(ID_OK_BUTTON,true);
        }
        m_Gui->Enable(ID_RESET_BUTTON,true);
        m_Gui->Enable(ID_CANCEL_BUTTON,true);
        m_Gui->Update();
        m_CurrentVME = NULL;
        break;
      }
      //------------------------------------------//
      case ID_DELETE_VME_BUTTON:
      {
        DeleteVME(m_AddedVMEsListBox->GetSelection());
        UpdateAddedVMEsListBox();
        if(m_AddedVMEs.size() == 0)
        {
          //Update GUI
          m_Gui->Enable(ID_OK_BUTTON,false);
          m_Gui->Enable(ID_DELETE_VME_BUTTON,false);
        }
        //Update GUI
        m_Gui->Enable(ID_DELETE_VME_BUTTON,false); //When a item is deleted the ListBox is unselected
        break;
      }
      //------------------------------------------//
      case ID_VMETIMEVARYING_NAME_TEXTBOX:
      {
        m_Gui->Enable(ID_RESET_BUTTON,true);
        break;
      }
      //------------------------------------------//
      case ID_RESET_BUTTON:
      {
        m_AddedVMEs.clear();
        m_CurrentVMETimeStamp = 0;
        m_VMEsType = _("albaVME");
        m_VMEsOutputType = _("albaVMEOutput");
        m_VMEsVTKDataType = VTK_DATA_SET;
        m_VMETimevaryingName = _("New Timevariyng VME");
        //Update GUI
        m_Gui->Enable(ID_OK_BUTTON,false);
        m_Gui->Enable(ID_DELETE_VME_BUTTON,false);
        m_Gui->Enable(ID_RESET_BUTTON,false);
        m_AddedVMEsListBox->Clear();
        m_Gui->Update();
        break;
      }
      //------------------------------------------//
      case ID_OK_BUTTON:
      {
        Execute();
        OpStop(OP_RUN_OK);
        break;
      }
      //------------------------------------------//
      case ID_CANCEL_BUTTON:
      {
        OpStop(OP_RUN_CANCEL);
        break;
      }
      //------------------------------------------//
      default:
      {
        albaEventMacro(*e);
      }
    }
  }
  else
  {
    albaEventMacro(*e);
  }
}
//----------------------------------------------------------------------------
void albaOpMakeVMETimevarying::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
	albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
bool albaOpMakeVMETimevarying::AcceptVME(albaVME * vme)
//----------------------------------------------------------------------------
{
  //A set of static VMEs represent the same object -> (necessary condition)
  //*   VMEs are of the same type
  //*   VMEs have the same output type
  //*   VMEs' outputs have the same vtk data type

  if(NULL == vme)
    return false;

  if(m_VMEsType.Equals(albaString(_("albaVME"))) && m_VMEsVTKDataType == VTK_DATA_SET && m_VMEsOutputType.Equals(albaString(_("albaVMEOutput"))))
  {
    m_VMEsType = albaString(vme->GetTypeName());
    m_VMEsOutputType = albaString(vme->GetOutput()->GetTypeName());
    m_VMEsVTKDataType = vme->GetOutput()->GetVTKData()->GetDataObjectType();
    return true;
  }
  else
  {
    if(m_VMEsVTKDataType == vme->GetOutput()->GetVTKData()->GetDataObjectType() &&  //Check vtk data type
            m_VMEsOutputType.Equals(albaString(vme->GetOutput()->GetTypeName())) &&  //Check output type
            (m_VMEsType.Equals(albaString(vme->GetTypeName()))))                     //Check VME type      
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  return false;
}
//----------------------------------------------------------------------------
bool albaOpMakeVMETimevarying::AcceptTimeStamp(double timestamp)
//----------------------------------------------------------------------------
{
  std::vector<VmeTimevaryingItem *>::iterator AddedVMEsIter;
  for(AddedVMEsIter = m_AddedVMEs.begin();AddedVMEsIter != m_AddedVMEs.end();AddedVMEsIter++)
  {
     if(timestamp == (* AddedVMEsIter)->m_TimeStamp)
       return false;
  }
  return true;
}
//----------------------------------------------------------------------------
void albaOpMakeVMETimevarying::AddVME(albaVME * vme, double timestamp)
//----------------------------------------------------------------------------
{
  assert(vme);
  std::vector<VmeTimevaryingItem *>::iterator AddedVMEsIter;
  VmeTimevaryingItem * newVmeTimevaryingItem;
  newVmeTimevaryingItem = (VmeTimevaryingItem *)malloc(sizeof VmeTimevaryingItem);
  newVmeTimevaryingItem->m_VME = vme;
  newVmeTimevaryingItem->m_TimeStamp = timestamp;
  for(AddedVMEsIter = m_AddedVMEs.begin();AddedVMEsIter != m_AddedVMEs.end();AddedVMEsIter++)
  {
    if((* AddedVMEsIter)->m_TimeStamp > timestamp)
    {
      m_AddedVMEs.insert(AddedVMEsIter,newVmeTimevaryingItem);
      return;
    }
  }
  m_AddedVMEs.push_back(newVmeTimevaryingItem);
}
//----------------------------------------------------------------------------
void albaOpMakeVMETimevarying::DeleteVME(int index)
//----------------------------------------------------------------------------
{
  assert(index < m_AddedVMEs.size());
  if(m_AddedVMEs.size() == 1)
  {
    m_AddedVMEs.clear();
    m_VMEsType = _("albaVME");
    m_VMEsOutputType = _("albaVMEOutput");
    m_VMEsVTKDataType = VTK_DATA_SET;
    m_CurrentVMETimeStamp = 0.0;
  }
  else
  {
    std::vector<VmeTimevaryingItem *>::iterator AddedVMEsIter = m_AddedVMEs.begin() + index;
    m_AddedVMEs.erase(AddedVMEsIter);
  }
}
//----------------------------------------------------------------------------
void albaOpMakeVMETimevarying::UpdateAddedVMEsListBox()//GUI
//----------------------------------------------------------------------------
{
  m_AddedVMEsListBox->Clear();
  
  std::vector<VmeTimevaryingItem *>::iterator AddedVMEsIter;
  for(AddedVMEsIter = m_AddedVMEs.begin();AddedVMEsIter != m_AddedVMEs.end();AddedVMEsIter++)
  {
    wxString newListBoxItem;
    newListBoxItem = wxString("t = ");
    newListBoxItem.Append(albaString::Format("%f",(* AddedVMEsIter)->m_TimeStamp));
    newListBoxItem.Append(wxString(" - "));
    newListBoxItem.Append(wxString((* AddedVMEsIter)->m_VME->GetName()));
    m_AddedVMEsListBox->Append(newListBoxItem);
  }
}
//----------------------------------------------------------------------------
void albaOpMakeVMETimevarying::Execute()
//----------------------------------------------------------------------------
{
  assert(m_AddedVMEs.size() != 0);

  //Assign the correct type to the resulting timevarying VME
  if(m_VMEsType.Equals(albaString(_("albaVMEImage"))))
  {
    albaVMEImage * tmpVME;
    albaNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }
  else if(m_VMEsType.Equals(albaString(_("albaVMEMesh"))))//OK
  {
    albaVMEMesh * tmpVME;
    albaNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }
  else if(m_VMEsType.Equals(albaString(_("albaVMEPointSet"))))
  {
    albaVMEPointSet * tmpVME;
    albaNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }
  else if(m_VMEsType.Equals(albaString(_("albaVMELandmarkCloud"))))//OK
  {
    albaVMELandmarkCloud * tmpVME;
    albaNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }
  else if(m_VMEsType.Equals(albaString(_("albaVMEVector"))))
  {
    albaVMEVector * tmpVME;
    albaNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }
  else if(m_VMEsType.Equals(albaString(_("albaVMESurface"))))//OK
  {
    albaVMESurface * tmpVME;
    albaNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }
  else if(m_VMEsType.Equals(albaString(_("albaVMESurfaceParametric"))))//OK
  {
    //A timevarying VME created from a set of parametric surface (albaVMESurfaceParametric) is treated as a surface (albaVMESurface)
    albaVMESurface * tmpVME;
    albaNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }
  else if(m_VMEsType.Equals(albaString(_("albaVMEVolumeGray"))))
  {
    albaVMEVolumeGray * tmpVME;
    albaNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }
  else if(m_VMEsType.Equals(albaString(_("albaVMEVolumeRGB"))))
  {
    albaVMEVolumeRGB * tmpVME;
    albaNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }
  else if(m_VMEsType.Equals(albaString(_("albaVMEPolyline"))))//OK
  {
    albaVMEPolyline * tmpVME;
    albaNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }
  else //Default
  {
    albaVMEGeneric * tmpVME;
    albaNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }

  std::vector<VmeTimevaryingItem *>::iterator AddedVMEsIter;

  albaVMEItemVTK * lastVmeItem = NULL;
  albaMatrix * lastVmeMatrix = NULL;

  //Fill VME's DataVector and MatrixVector
  for(AddedVMEsIter = m_AddedVMEs.begin();AddedVMEsIter != m_AddedVMEs.end();AddedVMEsIter++)
  {
    //Data changes
    albaVMEItemVTK * vmeItem;
    albaNEW(vmeItem);
    vmeItem->SetData((* AddedVMEsIter)->m_VME->GetOutput()->GetVTKData());

    vmeItem->SetTimeStamp((albaTimeStamp)(* AddedVMEsIter)->m_TimeStamp);
    if(lastVmeItem == NULL)
    {
      albaNEW(lastVmeItem);
      m_VMETimevarying->GetDataVector()->AppendItem(vmeItem);
      lastVmeItem->DeepCopy(vmeItem);
    }
    else
    {
      lastVmeItem->SetTimeStamp((albaTimeStamp)(* AddedVMEsIter)->m_TimeStamp);
      //Todo: Add conditions on vme type
      lastVmeItem->GlobalCompareDataFlagOn();//For volumes and meshes must compare scalar values
      vmeItem->GlobalCompareDataFlagOn();
      if(!lastVmeItem->Equals(vmeItem))
      {
         m_VMETimevarying->GetDataVector()->AppendItem(vmeItem);
         lastVmeItem->DeepCopy(vmeItem);
      }
    }
    albaDEL(vmeItem); // remove leaks

    //Matrix changes
    albaMatrix * vmeMatrix;
    albaNEW(vmeMatrix);
    vmeMatrix->DeepCopy((* AddedVMEsIter)->m_VME->GetOutput()->GetMatrix());
    vmeMatrix->SetTimeStamp((albaTimeStamp)(* AddedVMEsIter)->m_TimeStamp);
    if(lastVmeMatrix==NULL)
    {
      albaNEW(lastVmeMatrix);
      m_VMETimevarying->GetMatrixVector()->SetMatrix(vmeMatrix);
      lastVmeMatrix->DeepCopy(vmeMatrix);
    }
    else
    {
      lastVmeMatrix->SetTimeStamp((albaTimeStamp)(* AddedVMEsIter)->m_TimeStamp);
      if(!lastVmeMatrix->Equals(vmeMatrix))
      {
         m_VMETimevarying->GetMatrixVector()->SetMatrix(vmeMatrix);
         lastVmeMatrix->DeepCopy(vmeMatrix);
      }
    }
    albaDEL(vmeMatrix); // remove leaks
  }
  albaDEL(lastVmeItem); // remove leaks
  albaDEL(lastVmeMatrix);

  //Update VME's DataPipe and MatrixPipe
  m_VMETimevarying->SetName(m_VMETimevaryingName);
  m_VMETimevarying->GetAbsMatrixPipe()->Update();
  m_VMETimevarying->GetMatrixPipe()->Update();
  m_VMETimevarying->GetDataPipe()->Update();

  GetLogicManager()->VmeAdd(m_VMETimevarying);

  m_Output = m_VMETimevarying;

}