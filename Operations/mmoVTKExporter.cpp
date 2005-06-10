/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoVTKExporter.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-10 08:53:28 $
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

#include "mmoVTKExporter.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mmgGui.h"
//#include "mmoExplodeCollapse.h"
#include "vtkMAFSmartPointer.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMERoot.h"

#include "vtkDataSetWriter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkAbstractTransform.h"

//----------------------------------------------------------------------------
mmoVTKExporter::mmoVTKExporter(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_File    = "";
  m_Input   = NULL;

	m_Binary        = 1;
	m_ABSMatrixFlag = 0;

	m_FileDir = mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mmoVTKExporter::~mmoVTKExporter( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mmoVTKExporter::Accept(mafNode *node)
//----------------------------------------------------------------------------
{ 
  return (node->IsMAFType(mafVME) && !node->IsMAFType(mafVMERoot));
}
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum VTK_EXPORTER_ID
{
  ID_VTK_BINARY_FILE = MINID,
	ID_ABS_MATRIX,
};
//----------------------------------------------------------------------------
void mmoVTKExporter::OpRun()   
//----------------------------------------------------------------------------
{
	m_Gui = new mmgGui(this);
	m_Gui->Label("file type",true);
	m_Gui->Bool(ID_VTK_BINARY_FILE,"binary",&m_Binary,0);
	m_Gui->Label("absolute matrix",true);
	m_Gui->Bool(ID_ABS_MATRIX,"apply",&m_ABSMatrixFlag,0);
	if (m_Input->IsA("mafVMESurface") || m_Input->IsA("mafVMEPointSet") || m_Input->IsA("mafVMEGroup"))
		m_Gui->Enable(ID_ABS_MATRIX,true);
	else
		m_Gui->Enable(ID_ABS_MATRIX,false);
	m_Gui->Label("");
	m_Gui->OkCancel();
	
	ShowGui();
}
//----------------------------------------------------------------------------
void mmoVTKExporter::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
    switch(e->GetId())
    {
    case wxOK:
      OpStop(OP_RUN_OK);
      break;
    case wxCANCEL:
      OpStop(OP_RUN_CANCEL);
      break;
    case VME_ADD:
      {
        //trap the VME_ADD of the mmoCollapse and mmoExplode to update the
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
void mmoVTKExporter::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();

	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void mmoVTKExporter::OpDo()   
//----------------------------------------------------------------------------
{					
	assert(m_Input);

	m_File = "";

	mafString wildc = "vtk Data (*.vtk)|*.vtk";
	m_File = mafGetSaveFile(m_FileDir.GetCStr(),wildc.GetCStr()).c_str(); 

	if(m_File == "") 
		return;

	((mafVME *)m_Input)->GetOutput()->Update();

	if(this->m_Input->IsA("mafVMELandmarkCloud"))
	{
    if(((mafVMELandmarkCloud *)m_Input)->GetNumberOfLandmarks() > 0)
		{
      bool oldstate=((mafVMELandmarkCloud *)m_Input)->IsOpen();

	    if (oldstate)
      {
        /*mafOp *close_op = new mmoExplodeCollapse("close cloud");
        close_op->SetInput(((mafVMELandmarkCloud *)m_Input));
        close_op->SetListener(m_Listener);
        close_op->OpDo();*/
      }
      SaveVTKData();
		  
      if (oldstate)
      {
        /*mafOp *open_op = new mmoExplodeCollapse("open cloud");
        open_op->SetInput(((mafVMELandmarkCloud *)m_Input));
        open_op->SetListener(m_Listener);
        open_op->OpDo();*/
      }
    }
		else
		{
			wxString msg("Data not present!");
			wxMessageBox( msg,"Warning", wxOK|wxICON_WARNING , NULL);
		}
	}
	else
	{
    SaveVTKData();
	}
}
//----------------------------------------------------------------------------
void mmoVTKExporter::SaveVTKData()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkDataSetWriter> writer;

  if (m_ABSMatrixFlag)
  {
    vtkMAFSmartPointer<vtkTransform> v_abst;

    vtkMAFSmartPointer<vtkTransformPolyDataFilter> v_tpdf;
    v_tpdf->SetInput((vtkPolyData *)((mafVME *)m_Input)->GetOutput()->GetVTKData());
    v_tpdf->SetTransform((vtkAbstractTransform *)((mafVME *)m_Input)->GetAbsMatrixPipe());
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
//----------------------------------------------------------------------------
void mmoVTKExporter::OpUndo()   
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mmoVTKExporter::Copy()   
//----------------------------------------------------------------------------
{
  mmoVTKExporter *cp = new mmoVTKExporter(m_Label);
  cp->m_File = m_File;
  return cp;
}
