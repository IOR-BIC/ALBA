/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpExporterVTK.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:03:51 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
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

#include "mafOpExporterVTK.h"

#include "mafDecl.h"
#include "mafGUI.h"
#include "vtkMAFSmartPointer.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMERoot.h"
#include "mafTransformBase.h"

#include "vtkDataSetWriter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkAbstractTransform.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpExporterVTK);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpExporterVTK::mafOpExporterVTK(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_File    = "";
  m_Input   = NULL;

	m_Binary        = 1;
	m_ABSMatrixFlag = 0;

	m_FileDir = "";//mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mafOpExporterVTK::~mafOpExporterVTK()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mafOpExporterVTK::Accept(mafNode *node)
//----------------------------------------------------------------------------
{ 
  return (node->IsMAFType(mafVME) && !node->IsMAFType(mafVMERoot));
}
//----------------------------------------------------------------------------
mafOp* mafOpExporterVTK::Copy()   
//----------------------------------------------------------------------------
{
  mafOpExporterVTK *cp = new mafOpExporterVTK(m_Label);
  cp->m_File = m_File;
  return cp;
}
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum VTK_EXPORTER_ID
{
  ID_VTK_BINARY_FILE = MINID,
	ID_ABS_MATRIX,
  ID_CHOOSE_FILENAME,
};
//----------------------------------------------------------------------------
void mafOpExporterVTK::OpRun()   
//----------------------------------------------------------------------------
{
  mafString wildc = "vtk Data (*.vtk)|*.vtk";

  m_Gui = new mafGUI(this);
  m_Gui->FileSave(ID_CHOOSE_FILENAME, _("vtk file"), &m_File, wildc);
	m_Gui->Label("file type",true);
	m_Gui->Bool(ID_VTK_BINARY_FILE,"binary",&m_Binary,0);
	m_Gui->Label("absolute matrix",true);
	m_Gui->Bool(ID_ABS_MATRIX,"apply",&m_ABSMatrixFlag,0);
	if (m_Input->IsA("mafVMESurface") || m_Input->IsA("mafVMEPointSet") || m_Input->IsA("mafVMEGroup"))
		m_Gui->Enable(ID_ABS_MATRIX,true);
	else
		m_Gui->Enable(ID_ABS_MATRIX,false);
	m_Gui->OkCancel();
  m_Gui->Enable(wxOK, !m_File.IsEmpty());
	
	m_Gui->Divider();

	ShowGui();
}
//----------------------------------------------------------------------------
void mafOpExporterVTK::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
    switch(e->GetId())
    {
      case wxOK:
        ExportVTK();
        OpStop(OP_RUN_OK);
      break;
      case ID_CHOOSE_FILENAME:
        m_Gui->Enable(wxOK, !m_File.IsEmpty());
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);
      break;
      case VME_ADD:
      {
        //trap the VME_ADD of the mafOpCollapse and mafOpExplode to update the
        //m_Input, then forward the message to mafDMLlogicMDI
        this->m_Input = e->GetVme();
        mafEventMacro(mafEvent(this,VME_ADD,this->m_Input));
      }
      break;
      default:
        mafEventMacro(*e);
      break;
    }
	}
}

//----------------------------------------------------------------------------
void mafOpExporterVTK::ExportVTK()
//----------------------------------------------------------------------------
{					
	((mafVME *)m_Input)->GetOutput()->Update();
	if(this->m_Input->IsA("mafVMELandmarkCloud"))
	{
    if(((mafVMELandmarkCloud *)m_Input)->GetNumberOfLandmarks() > 0)
		{
      bool oldstate = ((mafVMELandmarkCloud *)m_Input)->IsOpen();

	    if (oldstate)
      {
        ((mafVMELandmarkCloud *)m_Input)->Close();
      }
      SaveVTKData();
		  
      if (oldstate)
      {
        ((mafVMELandmarkCloud *)m_Input)->Open();
      }
    }
		else
		{
			mafMessage( _("Data not present!"), _("Warning"), wxOK|wxICON_WARNING);
		}
	}
	else
	{
    SaveVTKData();
	}
}
//----------------------------------------------------------------------------
void mafOpExporterVTK::SaveVTKData()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkDataSetWriter> writer;

  if (m_ABSMatrixFlag)
  {
    vtkMAFSmartPointer<vtkTransformPolyDataFilter> v_tpdf;
    v_tpdf->SetInput((vtkPolyData *)((mafVME *)m_Input)->GetOutput()->GetVTKData());
    v_tpdf->SetTransform(((mafVME *)m_Input)->GetOutput()->GetTransform()->GetVTKTransform());
    v_tpdf->Update();
    writer->SetInput((vtkDataSet *)v_tpdf->GetOutput());
  }
  else
    writer->SetInput(((mafVME *)m_Input)->GetOutput()->GetVTKData());

  if (this->m_Binary)
    writer->SetFileTypeToBinary();
  else
    writer->SetFileTypeToASCII();
  writer->SetFileName(m_File.GetCStr());
  writer->Write();
}
