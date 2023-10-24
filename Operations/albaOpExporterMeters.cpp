/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterMeters
 Authors: Daniele Giunchi
 
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
#include "albaOpExporterMeters.h"
#include "wx/busyinfo.h"

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"

#include "albaVMERoot.h"
#include "albaVMEMeter.h"
#include "albaVMEOutputMeter.h"
#include "albaVMEWrappedMeter.h"
#include "albaVMEOutputWrappedMeter.h"
#include "albaVMEIterator.h"

#include "vtkDataSet.h"

#include <iomanip>

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpExporterMeters);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpExporterMeters::albaOpExporterMeters(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_EXPORTER;
  m_Canundo = true;
  
  m_File    = "";
  m_CurrentVme = NULL;
  m_CurrentTime = -1;
  m_SubTreeExportMeter = FALSE;

  m_ExportRadio = VME_ALL_METERS;
  m_Meters.clear();
}
//----------------------------------------------------------------------------
albaOpExporterMeters::~albaOpExporterMeters( ) 
//----------------------------------------------------------------------------
{
  m_MetersCoordinatesList.clear();
}
//----------------------------------------------------------------------------
albaOp* albaOpExporterMeters::Copy()   
//----------------------------------------------------------------------------
{
	albaOpExporterMeters *cp = new albaOpExporterMeters(m_Label);
	cp->m_File = m_File;
	return cp;
}
//----------------------------------------------------------------------------
bool albaOpExporterMeters::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{ 
  bool inputVMEAccepted = false;

  if (node->IsA("albaVMEWrappedMeter"))
  {
    inputVMEAccepted = true;
  }
  else
  {
    inputVMEAccepted = false;
  }

  return inputVMEAccepted;
}
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum WRAPPED_METER_EXPORTER_ID
{
  ID_CHOOSE_FILENAME = MINID,
  ID_SUBTREE_EXPORT_METERS,
  ID_CHOOSE_EXPORT_VME,
};
//----------------------------------------------------------------------------
void albaOpExporterMeters::OpRun()   
//----------------------------------------------------------------------------
{
  CreateGui();
	ShowGui();
}
//----------------------------------------------------------------------------
void albaOpExporterMeters::CreateGui()   
//----------------------------------------------------------------------------
{
  albaString wildc = "Wrapped Meter Coordinates(*.txt)|*.txt";
  wxString choises[4]={"Selected Meter","Classic Meters","Wrapped Meters", "All Meters"};

  m_Gui = new albaGUI(this);
  m_Gui->Label(_("Export:"),true);
  m_Gui->Bool(ID_SUBTREE_EXPORT_METERS, "only subtree", &m_SubTreeExportMeter,1);
  m_Gui->Radio(ID_CHOOSE_EXPORT_VME,"",&m_ExportRadio,4,choises);
  //m_Gui->FileSave(ID_CHOOSE_FILENAME,"stl file", &m_File, wildc,"Save As...");
  m_Gui->OkCancel();

  m_Gui->Divider();

}
//----------------------------------------------------------------------------
void albaOpExporterMeters::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
    switch(e->GetId())
    {
      case ID_CHOOSE_EXPORT_VME:
        {
          //control if can be checked the selected
          ControlExportChoose();  
        }
      break;
      case wxOK:
				{
				  OpStop(OP_RUN_OK);
				}
      break;
      case ID_CHOOSE_FILENAME:
        m_Gui->Enable(wxOK,m_File != "");
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);
      break;
      default:
        e->Log();
      break;
    }
	}
}
//----------------------------------------------------------------------------
void albaOpExporterMeters::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	albaEventMacro(albaEvent(this,result));        
}
//----------------------------------------------------------------------------
void albaOpExporterMeters::OpDo()   
//----------------------------------------------------------------------------
{
  albaString initialFileName;
  initialFileName = albaGetLastUserFolder();

  initialFileName.Append("\\Meter.txt");

  albaString wildc = "configuration file (*.txt)|*.txt";
  m_File = albaGetSaveFile(initialFileName.GetCStr(), wildc);

  if (m_File == "") return;

  Export();
  
  m_OutputFile.close();
}
//----------------------------------------------------------------------------
void albaOpExporterMeters::Export()
//----------------------------------------------------------------------------
{

  m_OutputFile.open(m_File, std::ios::out);

  if (m_OutputFile.fail()) {
    wxMessageBox("Error opening configuration file");
    return ;
  }

  //set Export function dependently to a radio button
  switch(m_ExportRadio)
  {
  case VME_SELECTED_METER:
    {
      ExportSelectedMeter();
    }
    break;
  case VME_CLASSIC_METERS:  
  case VME_WRAPPED_METERS:
  case VME_ALL_METERS:
    {
      ExportTypeOfMeters();
    }
    break;
  }
}
//----------------------------------------------------------------------------
void albaOpExporterMeters::ExportTypeOfMeters()
//----------------------------------------------------------------------------
{
  //must be a cicle in all vme of a msf
  albaVMEIterator *iter = NULL;
  if(m_SubTreeExportMeter == TRUE)
  {
    iter = m_Input->NewIterator();
  }
  else
  {
    iter = m_Input->GetRoot()->NewIterator();
  }
  
  for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    m_CurrentVme = node;
    if((m_ExportRadio == VME_CLASSIC_METERS || m_ExportRadio == VME_ALL_METERS) && node->IsA("albaVMEMeter"))
    {
      m_Meters.push_back(node);
    }
    else if((m_ExportRadio == VME_WRAPPED_METERS || m_ExportRadio == VME_ALL_METERS) && node->IsA("albaVMEWrappedMeter"))
    {
      m_Meters.push_back(node);
    }
  }
  iter->Delete();

  if(m_Meters.size() != 0)
  {
    ExportMeter();
  }
  
  m_Meters.clear();
}
//----------------------------------------------------------------------------
void albaOpExporterMeters::ExportSelectedMeter()
//----------------------------------------------------------------------------
{
  m_Meters.push_back(m_Input);
  ExportMeter();
  m_Meters.clear();
}
//----------------------------------------------------------------------------
void albaOpExporterMeters::ExportMeter()
//----------------------------------------------------------------------------
{
  wxBusyInfo *wait;
  if(!m_TestMode)
  {
    wait = new wxBusyInfo("Please wait, Exporting...");
  }

  albaVMERoot *root = ((albaVMERoot*)m_Meters[0]->GetRoot());
  root->GetTimeStamps(m_Times);

  for(int j=0; j< m_Times.size(); j++)
  {
    m_CurrentTime = m_Times[j];
    albaEventMacro(albaEvent(this, TIME_SET, m_CurrentTime, 0));

    for(int i=0;i< m_Meters.size();i++)
    {
      m_CurrentVme = m_Meters[i];

      if(albaVMEMeter::SafeDownCast(m_CurrentVme))
      {
        ExportClassicMeterCoordinates(i,j);
      }
      else if(albaVMEWrappedMeter::SafeDownCast(m_CurrentVme))
      {
        ExportWrappedMeterCoordinates(i,j);
      }
    }
  }
  
  //write matrixes over file.
  WriteOnFile();

  if(!m_TestMode)
  {
    delete wait;
  }
}
//----------------------------------------------------------------------------
void albaOpExporterMeters::ExportClassicMeterCoordinates(int index, int indexTime)
//----------------------------------------------------------------------------
{
  //classic meter
  albaVMEMeter *vmeMeter =  albaVMEMeter::SafeDownCast(m_CurrentVme);
  vmeMeter->GetOutput()->GetVTKData()->Modified();
  vmeMeter->GetOutput()->GetVTKData()->Update();
  vmeMeter->Modified();
  vmeMeter->Update();
  vmeMeter->Update();

  if(indexTime == 0) // create coordinate Matrix
  {
    vnl_matrix<double> M;
    M.set_size(6, m_Times.size());
    m_MetersCoordinatesList.push_back(M);
  }

  //origin
  double *value = vmeMeter->GetStartPointCoordinate();
  m_MetersCoordinatesList[index].put(0,indexTime,value[0]);
  m_MetersCoordinatesList[index].put(1,indexTime,value[1]);
  m_MetersCoordinatesList[index].put(2,indexTime,value[2]);

  //end point
  value = vmeMeter->GetEndPointCoordinate();
  m_MetersCoordinatesList[index].put(3,indexTime,value[0]);
  m_MetersCoordinatesList[index].put(4,indexTime,value[1]);
  m_MetersCoordinatesList[index].put(5,indexTime,value[2]);

}
//----------------------------------------------------------------------------
void albaOpExporterMeters::ExportWrappedMeterCoordinates(int index, int indexTime)
//----------------------------------------------------------------------------
{
  int wrappedModality;

  //wrapped meter
  albaVMEWrappedMeter *vmeWrappedMeter =  albaVMEWrappedMeter::SafeDownCast(m_CurrentVme);
  vmeWrappedMeter->GetOutput()->GetVTKData()->Modified();
  vmeWrappedMeter->GetOutput()->GetVTKData()->Update();
  vmeWrappedMeter->Modified();
  vmeWrappedMeter->Update();
  
  wrappedModality = vmeWrappedMeter->GetWrappedMode();

  if(indexTime == 0)
  {
    vnl_matrix<double> M;
    if(wrappedModality == albaVMEWrappedMeter::AUTOMATED_WRAP)
    {
      M.set_size(12, m_Times.size());
    }
    else
    {
      int numberOfRows = 6 + 3*albaVMEWrappedMeter::SafeDownCast(m_CurrentVme)->GetNumberMiddlePoints();
      M.set_size(numberOfRows, m_Times.size());
    }
    m_MetersCoordinatesList.push_back(M);
  }

  
  if(wrappedModality == albaVMEWrappedMeter::AUTOMATED_WRAP)
  {
    //origin
    double *value = vmeWrappedMeter->GetStartPointCoordinate();
    m_MetersCoordinatesList[index].put(0,indexTime,value[0]);
    m_MetersCoordinatesList[index].put(1,indexTime,value[1]);
    m_MetersCoordinatesList[index].put(2,indexTime,value[2]);
    //insertion1
    value = vmeWrappedMeter->GetWrappedGeometryTangent1();
    m_MetersCoordinatesList[index].put(3,indexTime,value[0]);
    m_MetersCoordinatesList[index].put(4,indexTime,value[1]);
    m_MetersCoordinatesList[index].put(5,indexTime,value[2]);
    //insertion 2
    value = vmeWrappedMeter->GetWrappedGeometryTangent2();
    m_MetersCoordinatesList[index].put(6,indexTime,value[0]);
    m_MetersCoordinatesList[index].put(7,indexTime,value[1]);
    m_MetersCoordinatesList[index].put(8,indexTime,value[2]);
    //end point
    value = vmeWrappedMeter->GetEndPointCoordinate();
    m_MetersCoordinatesList[index].put(9,indexTime,value[0]);
    m_MetersCoordinatesList[index].put(10,indexTime,value[1]);
    m_MetersCoordinatesList[index].put(11,indexTime,value[2]);
  }
  else // wrappedModality == albaVMEWrappedMeter::MANUAL_WRAP
  {
    //origin
    double *value = vmeWrappedMeter->GetStartPointCoordinate();
    m_MetersCoordinatesList[index].put(0,indexTime,value[0]);
    m_MetersCoordinatesList[index].put(1,indexTime,value[1]);
    m_MetersCoordinatesList[index].put(2,indexTime,value[2]);

    //middle points
    int numberOfMiddlePoints = vmeWrappedMeter->GetNumberMiddlePoints();
    for(int k=0; k<numberOfMiddlePoints ;k++)
    {
      value = vmeWrappedMeter->GetMiddlePointCoordinate(k);
      m_MetersCoordinatesList[index].put(3*(k+1),indexTime,value[0]);
      m_MetersCoordinatesList[index].put(3*(k+1)+1,indexTime,value[1]);
      m_MetersCoordinatesList[index].put(3*(k+1)+2,indexTime,value[2]);
    }

    //end point
    value = vmeWrappedMeter->GetEndPointCoordinate();
    m_MetersCoordinatesList[index].put(3*(numberOfMiddlePoints+1),indexTime,value[0]);
    m_MetersCoordinatesList[index].put(3*(numberOfMiddlePoints+1)+1,indexTime,value[1]);
    m_MetersCoordinatesList[index].put(3*(numberOfMiddlePoints+1)+2,indexTime,value[2]);

  }
}
//----------------------------------------------------------------------------
void albaOpExporterMeters::WriteOnFile()
//----------------------------------------------------------------------------
{
  // header
  for(int i=0; i< m_Times.size(); i++)
  {
    m_OutputFile << std::fixed << std::setprecision(3) << std::setw(8) <<m_Times[i] << "\t";
  }
  m_OutputFile << std::endl;
  // end header

  //name + coordinates
  for(int i=0; i< m_Meters.size(); i++)
  {
    m_OutputFile << m_Meters[i]->GetName() << std::endl; // vme name
    WriteCoordinatesOnFile(i);
  }
}
//----------------------------------------------------------------------------
void albaOpExporterMeters::WriteCoordinatesOnFile(int index)
//----------------------------------------------------------------------------
{
  unsigned int rows = m_MetersCoordinatesList[index].rows();
  unsigned int columns = m_MetersCoordinatesList[index].columns();
  for(int i=0; i< rows; i++)
  {
    if(i%3 == 0 || i == 0)
    {
      if (i==0)
      {
        m_OutputFile << "Origin" << std::endl;
      }
      else if (i == rows-3)
      {
        m_OutputFile << "Insertion" << std::endl;
      }
      else
      {
        m_OutputFile << "P" << (i)/3 << std::endl;
      }
      
    }

    for(int j=0; j< columns; j++)
    {
      double value =  m_MetersCoordinatesList[index].get(i,j);
      m_OutputFile << std::fixed << std::setprecision(3) << std::setw(8) << value << "\t";
    }
    m_OutputFile << std::endl;
  }
}
//----------------------------------------------------------------------------
bool albaOpExporterMeters::ControlExportChoose()
//----------------------------------------------------------------------------
{
  bool control = false;
  switch(m_ExportRadio)
  {
  case VME_SELECTED_METER:
    {
      control = (albaVMEMeter::SafeDownCast(m_Input) || albaVMEWrappedMeter::SafeDownCast(m_Input));
      if(control == false)
      {
        wxMessageBox("Current Vme is not a meter");
        m_ExportRadio = VME_ALL_METERS;
      }
    }
    break;
  case VME_CLASSIC_METERS:
      control = true;
    break;
  case VME_WRAPPED_METERS:
      control = true;
    break;
  case VME_ALL_METERS:
      control = true;
    break;
  }

  if(NULL != m_Gui)
  {
    m_Gui->Update();
  }

  return control;
}
