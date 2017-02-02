/*=========================================================================

 Program: MAF2
 Module: mafOpExporterVTK
 Authors: Paolo Quadrani
 
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
#include "vtkPolyData.h"
#include "mafVMEGroup.h"
#include "mafProgressBarHelper.h"
#include "vtkUnstructuredGrid.h"
#include "vtkTransformFilter.h"
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

	m_FileDir = mafGetLastUserFolder().c_str();
  m_ForceUnsignedShortScalarOutputForStructuredPoints = FALSE;
}
//----------------------------------------------------------------------------
mafOpExporterVTK::~mafOpExporterVTK()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mafOpExporterVTK::Accept(mafVME *node)
//----------------------------------------------------------------------------
{ 
  return (node->IsMAFType(mafVME) && !node->IsMAFType(mafVMERoot) && !node->IsMAFType(mafVMEGroup));
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
  vtkDataSet *inputData = m_Input->GetOutput()->GetVTKData();
  assert(inputData);

  bool isStructuredPoints = inputData->IsA("vtkStructuredPoints");

  mafString wildc = "vtk Data (*.vtk)|*.vtk";

  m_Gui = new mafGUI(this);
  m_Gui->FileSave(ID_CHOOSE_FILENAME, _("vtk file"), &m_File, wildc);
	m_Gui->Label("file type",true);
	m_Gui->Bool(ID_VTK_BINARY_FILE,"binary",&m_Binary,0);
	m_Gui->Label("absolute matrix",true);
	m_Gui->Bool(ID_ABS_MATRIX,"apply",&m_ABSMatrixFlag,0);
	if (m_Input->IsA("mafVMESurface") || m_Input->IsA("mafVMEPointSet") || m_Input->IsA("mafVMEGroup")||m_Input->IsA("mafVMEMesh")||m_Input->IsA("mafVMEPolyline"))
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
			case ID_ABS_MATRIX:
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
	m_Input->GetOutput()->Update();
	if(this->m_Input->IsA("mafVMELandmarkCloud"))
	{
    if(((mafVMELandmarkCloud *)m_Input)->GetNumberOfLandmarks() > 0)
		{
      SaveVTKData();
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
	mafProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar();
 
  vtkDataSet *inputData = m_Input->GetOutput()->GetVTKData();
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
		vtkMAFSmartPointer <vtkTransform> tra;
		tra->SetMatrix(m_Input->GetOutput()->GetAbsMatrix()->GetVTKMatrix());

		if(m_Input->IsA("mafVMEMesh"))
		{
			vtkMAFSmartPointer<vtkTransformFilter> v_tpdf;
			v_tpdf->SetInput((vtkUnstructuredGrid *)m_Input->GetOutput()->GetVTKData());
			v_tpdf->SetTransform(tra);
			v_tpdf->Update();
			writer->SetInput(v_tpdf->GetOutput());
		}
		else
		{
			vtkMAFSmartPointer<vtkTransformPolyDataFilter> v_tpdf;
			v_tpdf->SetInput((vtkPolyData *)m_Input->GetOutput()->GetVTKData());
			v_tpdf->SetTransform(tra);
			v_tpdf->Update();
			writer->SetInput(v_tpdf->GetOutput());
		}
  }
  else
  {
    writer->SetInput(writerInput);
  }

  if (this->m_Binary)
    writer->SetFileTypeToBinary();
  else
    writer->SetFileTypeToASCII();

  // workaround code:  this is not working so I'm setting a dummy 50/100 progress value 
  // mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR, writer));
  long dummyProgressValue = 50;
  
  progressHelper.UpdateProgressBar(dummyProgressValue);

  writer->SetFileName(m_File.GetCStr());
  writer->Write();
}
