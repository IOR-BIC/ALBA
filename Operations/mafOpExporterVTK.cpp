/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpExporterVTK.cpp,v $
  Language:  C++
  Date:      $Date: 2009-05-08 11:39:05 $
  Version:   $Revision: 1.2.2.2 $
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
#include <wx/busyinfo.h>

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
#include "vtkImageCast.h"
#include "vtkImageData.h"
#include "vtkStructuredPoints.h"

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
  m_ForceUnsignedShortScalarOutputForStructuredPoints = FALSE;
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
  ID_FORCE_UNSIGNED_SHORT_SCALARS_OUTPUT_FOR_STRUCTURED_POINTS,
};
//----------------------------------------------------------------------------
void mafOpExporterVTK::OpRun()   
//----------------------------------------------------------------------------
{
  vtkDataSet *inputData = ((mafVME *)m_Input)->GetOutput()->GetVTKData();
  assert(inputData);

  bool isStructuredPoints = inputData->IsA("vtkStructuredPoints");

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

  m_Gui->Divider(2);
  m_Gui->Label("Force UNSIGNED SHORT scalar output");
  m_Gui->Bool(ID_FORCE_UNSIGNED_SHORT_SCALARS_OUTPUT_FOR_STRUCTURED_POINTS, "", &m_ForceUnsignedShortScalarOutputForStructuredPoints);
  m_Gui->Enable(ID_FORCE_UNSIGNED_SHORT_SCALARS_OUTPUT_FOR_STRUCTURED_POINTS, isStructuredPoints ? true : false);
  m_Gui->Divider(2);

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
  wxBusyCursor *busyCursor = NULL;

  try
  {
    busyCursor = new wxBusyCursor();
  }
  catch (...)
  {
    std::ostringstream stringStream;
    stringStream << "cannot render busy cursor..."  << std::endl;
    mafLogMessage(stringStream.str().c_str());
  }

  vtkDataSet *inputData = ((mafVME *)m_Input)->GetOutput()->GetVTKData();
  assert(inputData);

  vtkDataSet *writerInput = NULL;

  vtkMAFSmartPointer<vtkImageCast> imageCast;
  
  if (m_ForceUnsignedShortScalarOutputForStructuredPoints)
  {    
    imageCast->SetInput(vtkStructuredPoints::SafeDownCast(inputData));
    imageCast->SetOutputScalarTypeToUnsignedShort();
    imageCast->Update();
    writerInput = imageCast->GetOutput();
  }
  else
  {
    writerInput = inputData;
  }

  vtkMAFSmartPointer<vtkDataSetWriter> writer;

  if (m_ABSMatrixFlag)
  {
    vtkMAFSmartPointer<vtkTransformPolyDataFilter> v_tpdf;
    v_tpdf->SetInput((vtkPolyData *)((mafVME *)m_Input)->GetOutput()->GetVTKData());
    v_tpdf->SetTransform(((mafVME *)m_Input)->GetOutput()->GetTransform()->GetVTKTransform());
    v_tpdf->Update();
    writer->SetInput(writerInput);
  }
  else
  {
    writer->SetInput(writerInput);
  }

  if (this->m_Binary)
    writer->SetFileTypeToBinary();
  else
    writer->SetFileTypeToASCII();
  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));

  // workaround code:  this is not working so I'm setting a dummy 50/100 progress value 
  // mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR, writer));
  long dummyProgressValue = 50;
  
  mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,dummyProgressValue));

  writer->SetFileName(m_File.GetCStr());
  writer->Write();
  
  mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));

  if (busyCursor)
  {
    delete busyCursor;
  }
}
