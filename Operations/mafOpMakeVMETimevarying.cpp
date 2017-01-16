/*=========================================================================

 Program: MAF2
 Module: mafOpMakeVMETimevarying
 Authors: Alberto Losi
 
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

#include "mafOpMakeVMETimevarying.h"
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafVME.h"
#include "mafVMEItemVTK.h"
#include "mafDataVector.h"
#include "mafMatrixVector.h"
#include "mafMatrix.h"
#include "vtkDataSet.h"
#include "mafVMEImage.h"
#include "mafVMEMesh.h"
#include "mafVMEPointSet.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEPolyLine.h"
#include "mafVMEVector.h"
#include "mafVMESurface.h"
#include "mafVMEVolume.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEVolumeRGB.h"
#include "mafVMEFactory.h"
#include "mafVMESurfaceParametric.h"
#include "mafMatrixPipe.h"
#include "mafAbsMatrixPipe.h"
#include "mafDataPipe.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpMakeVMETimevarying);
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
mafOpMakeVMETimevarying::mafOpMakeVMETimevarying(const wxString &label /* = "Make Timevariyng VME" */,bool showShadingPlane /* = false */) : mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo	= true;  

  m_VMETimevarying = NULL;
  m_CurrentVME = NULL;
  m_CurrentVMEName = _("");
  m_VMETimevaryingName = _("New Timevariyng VME");
  m_VMEsType = _("mafVME");
  m_VMEsOutputType = _("mafVMEOutput");
  m_VMEsVTKDataType = VTK_DATA_SET;

  m_AddedVMEsListBox = NULL;

  //m_NumOfAddedVMEs = 0;
  m_CurrentVMETimeStamp = 0.0;
  m_VMETimestampIncrement = 0.01;
  m_CurrentVMETimeStampFromVME = 0.0;
  m_CurrentVMETimestampModality = 0;
}
//----------------------------------------------------------------------------
mafOpMakeVMETimevarying::~mafOpMakeVMETimevarying()
//----------------------------------------------------------------------------
{
  for(int i = 0; i < m_AddedVMEs.size(); i++)
  {
    free(m_AddedVMEs.at(i)); // Remove leaks (with free: space allocated with malloc)
  }
  m_AddedVMEs.clear();
  mafDEL(m_VMETimevarying);
}
//----------------------------------------------------------------------------
bool mafOpMakeVMETimevarying::Accept(mafVME* vme) 
//----------------------------------------------------------------------------
{
	mafEvent e(this,VIEW_SELECTED);
	mafEventMacro(e);
  return true;
}
//----------------------------------------------------------------------------
mafOp *mafOpMakeVMETimevarying::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpMakeVMETimevarying(m_Label);
}

//----------------------------------------------------------------------------
void mafOpMakeVMETimevarying::OpRun()
//----------------------------------------------------------------------------
{
  CreateGui();
}
//----------------------------------------------------------------------------
void mafOpMakeVMETimevarying::OpDo()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpMakeVMETimevarying::OpUndo()
//----------------------------------------------------------------------------
{
  GetLogicManager()->VmeRemove(m_Output);
  mafDEL(m_Output);
}
//----------------------------------------------------------------------------
void mafOpMakeVMETimevarying::CreateGui() 
//----------------------------------------------------------------------------
{
  //Create Graphical User Interface

  m_Gui = new mafGUI(this);

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
  m_LblVMETimeStampFromVMEData = new wxStaticText(m_Gui,ID_TIMESTAMP_FROM_VME_LABEL_DATA,wxString::Format("%f",m_CurrentVMETimeStampFromVME),wxDefaultPosition,wxSize(100,17));
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
void mafOpMakeVMETimevarying::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
        mafString title = _("Choose VME");
        mafEvent e(this,VME_CHOOSE,&title,(long)&mafOpMakeVMETimevarying::DialogAcceptVME);
        mafEventMacro(e);
        
        //mafNEW(m_CurrentVME);
        m_CurrentVME = e.GetVme();
        bool VMEOk = AcceptVME(m_CurrentVME);
        if(m_CurrentVME)
        {
          if( VMEOk)
          {
            m_CurrentVMEName = m_CurrentVME->GetName();
            mafVMEGeneric * tmpCurrentVME;
            tmpCurrentVME = mafVMEGeneric::SafeDownCast(m_CurrentVME);
            mmuTimeVector vmeTimeStamps;
            if(tmpCurrentVME) //Vme is not a parametric surface
            {
              tmpCurrentVME->GetDataVector()->GetTimeStamps(vmeTimeStamps);
              m_CurrentVMETimeStampFromVME = (* vmeTimeStamps.begin()); //Load the VME's timestamp (for non static VME the first)
            }
            else
              m_CurrentVMETimeStampFromVME = 0;
            //m_Gui->Update();
            m_LblVMETimeStampFromVMEData->SetLabel(wxString::Format("%f",m_CurrentVMETimeStampFromVME));
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
        m_VMEsType = _("mafVME");
        m_VMEsOutputType = _("mafVMEOutput");
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
        mafEventMacro(*e);
      }
    }
  }
  else
  {
    mafEventMacro(*e);
  }
}
//----------------------------------------------------------------------------
void mafOpMakeVMETimevarying::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
bool mafOpMakeVMETimevarying::AcceptVME(mafVME * vme)
//----------------------------------------------------------------------------
{
  //A set of static VMEs represent the same object -> (necessary condition)
  //*   VMEs are of the same type
  //*   VMEs have the same output type
  //*   VMEs' outputs have the same vtk data type

  if(NULL == vme)
    return false;

  if(m_VMEsType.Equals(mafString(_("mafVME"))) && m_VMEsVTKDataType == VTK_DATA_SET && m_VMEsOutputType.Equals(mafString(_("mafVMEOutput"))))
  {
    m_VMEsType = mafString(vme->GetTypeName());
    m_VMEsOutputType = mafString(vme->GetOutput()->GetTypeName());
    m_VMEsVTKDataType = vme->GetOutput()->GetVTKData()->GetDataObjectType();
    return true;
  }
  else
  {
    if(m_VMEsVTKDataType == vme->GetOutput()->GetVTKData()->GetDataObjectType() &&  //Check vtk data type
            m_VMEsOutputType.Equals(mafString(vme->GetOutput()->GetTypeName())) &&  //Check output type
            (m_VMEsType.Equals(mafString(vme->GetTypeName()))))                     //Check VME type      
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
bool mafOpMakeVMETimevarying::AcceptTimeStamp(double timestamp)
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
void mafOpMakeVMETimevarying::AddVME(mafVME * vme, double timestamp)
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
void mafOpMakeVMETimevarying::DeleteVME(int index)
//----------------------------------------------------------------------------
{
  assert(index < m_AddedVMEs.size());
  if(m_AddedVMEs.size() == 1)
  {
    m_AddedVMEs.clear();
    m_VMEsType = _("mafVME");
    m_VMEsOutputType = _("mafVMEOutput");
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
void mafOpMakeVMETimevarying::UpdateAddedVMEsListBox()//GUI
//----------------------------------------------------------------------------
{
  m_AddedVMEsListBox->Clear();
  
  std::vector<VmeTimevaryingItem *>::iterator AddedVMEsIter;
  for(AddedVMEsIter = m_AddedVMEs.begin();AddedVMEsIter != m_AddedVMEs.end();AddedVMEsIter++)
  {
    wxString newListBoxItem;
    newListBoxItem = wxString("t = ");
    newListBoxItem.Append(wxString::Format("%f",(* AddedVMEsIter)->m_TimeStamp));
    newListBoxItem.Append(wxString(" - "));
    newListBoxItem.Append(wxString((* AddedVMEsIter)->m_VME->GetName()));
    m_AddedVMEsListBox->Append(newListBoxItem);
  }
}
//----------------------------------------------------------------------------
void mafOpMakeVMETimevarying::Execute()
//----------------------------------------------------------------------------
{
  assert(m_AddedVMEs.size() != 0);

  //Assign the correct type to the resulting timevarying VME
  if(m_VMEsType.Equals(mafString(_("mafVMEImage"))))
  {
    mafVMEImage * tmpVME;
    mafNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }
  else if(m_VMEsType.Equals(mafString(_("mafVMEMesh"))))//OK
  {
    mafVMEMesh * tmpVME;
    mafNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }
  else if(m_VMEsType.Equals(mafString(_("mafVMEPointSet"))))
  {
    mafVMEPointSet * tmpVME;
    mafNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }
  else if(m_VMEsType.Equals(mafString(_("mafVMELandmarkCloud"))))//OK
  {
    mafVMELandmarkCloud * tmpVME;
    mafNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }
  else if(m_VMEsType.Equals(mafString(_("mafVMEVector"))))
  {
    mafVMEVector * tmpVME;
    mafNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }
  else if(m_VMEsType.Equals(mafString(_("mafVMESurface"))))//OK
  {
    mafVMESurface * tmpVME;
    mafNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }
  else if(m_VMEsType.Equals(mafString(_("mafVMESurfaceParametric"))))//OK
  {
    //A timevarying VME created from a set of parametric surface (mafVMESurfaceParametric) is treated as a surface (mafVMESurface)
    mafVMESurface * tmpVME;
    mafNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }
  else if(m_VMEsType.Equals(mafString(_("mafVMEVolumeGray"))))
  {
    mafVMEVolumeGray * tmpVME;
    mafNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }
  else if(m_VMEsType.Equals(mafString(_("mafVMEVolumeRGB"))))
  {
    mafVMEVolumeRGB * tmpVME;
    mafNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }
  else if(m_VMEsType.Equals(mafString(_("mafVMEPolyline"))))//OK
  {
    mafVMEPolyline * tmpVME;
    mafNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }
  else //Default
  {
    mafVMEGeneric * tmpVME;
    mafNEW(tmpVME);
    m_VMETimevarying = tmpVME;
  }

  std::vector<VmeTimevaryingItem *>::iterator AddedVMEsIter;

  mafVMEItemVTK * lastVmeItem = NULL;
  mafMatrix * lastVmeMatrix = NULL;

  //Fill VME's DataVector and MatrixVector
  for(AddedVMEsIter = m_AddedVMEs.begin();AddedVMEsIter != m_AddedVMEs.end();AddedVMEsIter++)
  {
    //Data changes
    mafVMEItemVTK * vmeItem;
    mafNEW(vmeItem);
    vmeItem->SetData((* AddedVMEsIter)->m_VME->GetOutput()->GetVTKData());

    vmeItem->SetTimeStamp((mafTimeStamp)(* AddedVMEsIter)->m_TimeStamp);
    if(lastVmeItem == NULL)
    {
      mafNEW(lastVmeItem);
      m_VMETimevarying->GetDataVector()->AppendItem(vmeItem);
      lastVmeItem->DeepCopy(vmeItem);
    }
    else
    {
      lastVmeItem->SetTimeStamp((mafTimeStamp)(* AddedVMEsIter)->m_TimeStamp);
      //Todo: Add conditions on vme type
      lastVmeItem->GlobalCompareDataFlagOn();//For volumes and meshes must compare scalar values
      vmeItem->GlobalCompareDataFlagOn();
      if(!lastVmeItem->Equals(vmeItem))
      {
         m_VMETimevarying->GetDataVector()->AppendItem(vmeItem);
         lastVmeItem->DeepCopy(vmeItem);
      }
    }
    mafDEL(vmeItem); // remove leaks

    //Matrix changes
    mafMatrix * vmeMatrix;
    mafNEW(vmeMatrix);
    vmeMatrix->DeepCopy((* AddedVMEsIter)->m_VME->GetOutput()->GetMatrix());
    vmeMatrix->SetTimeStamp((mafTimeStamp)(* AddedVMEsIter)->m_TimeStamp);
    if(lastVmeMatrix==NULL)
    {
      mafNEW(lastVmeMatrix);
      m_VMETimevarying->GetMatrixVector()->SetMatrix(vmeMatrix);
      lastVmeMatrix->DeepCopy(vmeMatrix);
    }
    else
    {
      lastVmeMatrix->SetTimeStamp((mafTimeStamp)(* AddedVMEsIter)->m_TimeStamp);
      if(!lastVmeMatrix->Equals(vmeMatrix))
      {
         m_VMETimevarying->GetMatrixVector()->SetMatrix(vmeMatrix);
         lastVmeMatrix->DeepCopy(vmeMatrix);
      }
    }
    mafDEL(vmeMatrix); // remove leaks
  }
  mafDEL(lastVmeItem); // remove leaks
  mafDEL(lastVmeMatrix);

  //Update VME's DataPipe and MatrixPipe
  m_VMETimevarying->SetName(m_VMETimevaryingName);
  m_VMETimevarying->GetAbsMatrixPipe()->Update();
  m_VMETimevarying->GetMatrixPipe()->Update();
  m_VMETimevarying->GetDataPipe()->Update();

  GetLogicManager()->VmeAdd(m_VMETimevarying);

  m_Output = m_VMETimevarying;

}