/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpExporterWrappedMeter.cpp,v $
  Language:  C++
  Date:      $Date: 2007-08-27 12:59:10 $
  Version:   $Revision: 1.1 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "medOpExporterWrappedMeter.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mmgGui.h"

#include "medVMEWrappedMeter.h"
#include "medVMEOutputWrappedMeter.h"

#include <fstream>


//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpExporterWrappedMeter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpExporterWrappedMeter::medOpExporterWrappedMeter(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_EXPORTER;
  m_Canundo = true;
  
  m_File    = "";
}
//----------------------------------------------------------------------------
medOpExporterWrappedMeter::~medOpExporterWrappedMeter( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* medOpExporterWrappedMeter::Copy()   
//----------------------------------------------------------------------------
{
	medOpExporterWrappedMeter *cp = new medOpExporterWrappedMeter(m_Label);
	cp->m_File = m_File;
	return cp;
}
//----------------------------------------------------------------------------
bool medOpExporterWrappedMeter::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(medVMEWrappedMeter));
}
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum WRAPPED_METER_EXPORTER_ID
{
  ID_CHOOSE_FILENAME = MINID,
};
//----------------------------------------------------------------------------
void medOpExporterWrappedMeter::OpRun()   
//----------------------------------------------------------------------------
{
  mafString wildc = "Wrapped Meter Coordinates(*.txt)|*.txt";

  m_Gui = new mmgGui(this);
	//m_Gui->FileSave(ID_CHOOSE_FILENAME,"stl file", &m_File, wildc,"Save As...");
	m_Gui->OkCancel();
  
	m_Gui->Divider();

	ShowGui();
}
//----------------------------------------------------------------------------
void medOpExporterWrappedMeter::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
    switch(e->GetId())
    {
      case wxOK:
				{
          mafString initialFileName;
          initialFileName = mafGetApplicationDirectory().c_str();
          initialFileName.Append("\\WrapperMeter.txt");

          mafString wildc = "configuration file (*.txt)|*.txt";
          m_File = mafGetSaveFile(initialFileName.GetCStr(), wildc).c_str();

          if (m_File == "") return;

				  ExportWrappedMeterCoordinates();
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
void medOpExporterWrappedMeter::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void medOpExporterWrappedMeter::ExportWrappedMeterCoordinates()
//----------------------------------------------------------------------------
{
  medVMEWrappedMeter *vmeWrappedMeter =  medVMEWrappedMeter::SafeDownCast(m_Input);
  vmeWrappedMeter->Update();
  medVMEOutputWrappedMeter *out_wm = medVMEOutputWrappedMeter::SafeDownCast(vmeWrappedMeter->GetOutput());
  out_wm->Update();

  std::ofstream outputFile(m_File, std::ios::out);

  if (outputFile == NULL) {
    wxMessageBox("Error opening configuration file");
    return ;
  }

  for(int i=0; i<vmeWrappedMeter->GetNumberMiddlePoints();i++)
  {
    outputFile << vmeWrappedMeter->GetMiddlePointCoordinate(i)[0] << '\t'
      << vmeWrappedMeter->GetMiddlePointCoordinate(i)[1] << '\t'
      << vmeWrappedMeter->GetMiddlePointCoordinate(i)[2] << std::endl;
  }

  outputFile.close();

}