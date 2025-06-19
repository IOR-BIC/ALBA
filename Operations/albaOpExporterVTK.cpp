/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterVTK
 Authors: Paolo Quadrani
 
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
#include <albaGUIBusyInfo.h>

#include "albaOpExporterVTK.h"

#include "albaDecl.h"
#include "albaGUI.h"
#include "vtkALBASmartPointer.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMERoot.h"
#include "albaTransformBase.h"

#include "vtkDataSetWriter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkAbstractTransform.h"
#include "vtkImageCast.h"
#include "vtkImageData.h"
#include "vtkImageData.h"
#include "vtkPolyData.h"
#include "albaVMEGroup.h"
#include "albaProgressBarHelper.h"
#include "vtkUnstructuredGrid.h"
#include "vtkTransformFilter.h"
//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpExporterVTK);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpExporterVTK::albaOpExporterVTK(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_File    = "";
  m_Input   = NULL;

	m_Binary        = 1;
	m_ABSMatrixFlag = 0;

	m_ForceUnsignedShortScalarOutputForStructuredPoints = false;
}
//----------------------------------------------------------------------------
albaOpExporterVTK::~albaOpExporterVTK()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool albaOpExporterVTK::InternalAccept(albaVME *node)
//----------------------------------------------------------------------------
{ 
  return (node->IsALBAType(albaVME) && !node->IsALBAType(albaVMERoot) && !node->IsALBAType(albaVMEGroup));
}
//----------------------------------------------------------------------------
albaOp* albaOpExporterVTK::Copy()   
//----------------------------------------------------------------------------
{
  albaOpExporterVTK *cp = new albaOpExporterVTK(m_Label);
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
void albaOpExporterVTK::OpRun()
//----------------------------------------------------------------------------
{
	vtkDataSet *inputData = m_Input->GetOutput()->GetVTKData();
	assert(inputData);

	albaString wildc = "vtk Data (*.vtk)|*.vtk";

	m_FileDir = albaGetLastUserFolder();
	m_File = m_FileDir + "\\" + m_Input->GetName() + ".vtk";

	m_Gui = new albaGUI(this);
	m_Gui->FileSave(ID_CHOOSE_FILENAME, _("VTK file"), &m_File, wildc);
/*	m_Gui->Label("File type", true);*/
	m_Gui->Bool(ID_VTK_BINARY_FILE, "Binary File type", &m_Binary, 1);
/*	m_Gui->Label("Absolute matrix", true);*/
	m_Gui->Bool(ID_ABS_MATRIX, "Apply Absolute matrix", &m_ABSMatrixFlag, 1);

	if (m_Input->IsA("albaVMESurface") || m_Input->IsA("albaVMEPointSet") || m_Input->IsA("albaVMEGroup") || m_Input->IsA("albaVMEMesh") || m_Input->IsA("albaVMEPolyline"))
		m_Gui->Enable(ID_ABS_MATRIX, true);
	else
		m_Gui->Enable(ID_ABS_MATRIX, false);

	if (inputData->IsA("vtkImageData"))
	{
		m_Gui->Divider(2);
		m_Gui->Bool(ID_FORCE_UNSIGNED_SHORT_SCALARS_OUTPUT_FOR_STRUCTURED_POINTS, "Force unsigned short scalar output", &m_ForceUnsignedShortScalarOutputForStructuredPoints, 1);
	}

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	m_Gui->Enable(wxOK, !m_File.IsEmpty());

	ShowGui();
}
//----------------------------------------------------------------------------
void albaOpExporterVTK::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
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
        albaEventMacro(*e);
      break;
    }
	}
}

//----------------------------------------------------------------------------
void albaOpExporterVTK::ExportVTK()
//----------------------------------------------------------------------------
{					
	m_Input->GetOutput()->Update();
	if(this->m_Input->IsA("albaVMELandmarkCloud"))
	{
    if(((albaVMELandmarkCloud *)m_Input)->GetNumberOfLandmarks() > 0)
		{
      SaveVTKData();
    }
		else
		{
			albaMessage( _("Data not present!"), _("Warning"), wxOK|wxICON_WARNING);
		}
	}
	else
	{
    SaveVTKData();
	}
}
//----------------------------------------------------------------------------
void albaOpExporterVTK::SaveVTKData()
//----------------------------------------------------------------------------
{
	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar();
 
  vtkDataSet *inputData = m_Input->GetOutput()->GetVTKData();
  assert(inputData);

  vtkDataSet *writerInput = NULL;

  vtkALBASmartPointer<vtkImageCast> imageCast;
  
  if (m_ForceUnsignedShortScalarOutputForStructuredPoints)
  {    
    imageCast->SetInput(vtkImageData::SafeDownCast(inputData));
    imageCast->SetOutputScalarTypeToUnsignedShort();
    imageCast->Update();
    writerInput = imageCast->GetOutput();
  }
  else
  {
    writerInput = inputData;
  }

  vtkALBASmartPointer<vtkDataSetWriter> writer;

  if (m_ABSMatrixFlag)
  {
		vtkALBASmartPointer <vtkTransform> tra;
		tra->SetMatrix(m_Input->GetOutput()->GetAbsMatrix()->GetVTKMatrix());

		if(m_Input->IsA("albaVMEMesh"))
		{
			vtkALBASmartPointer<vtkTransformFilter> v_tpdf;
			v_tpdf->SetInput((vtkUnstructuredGrid *)m_Input->GetOutput()->GetVTKData());
			v_tpdf->SetTransform(tra);
			v_tpdf->Update();
			writer->SetInput(v_tpdf->GetOutput());
		}
		else
		{
			vtkALBASmartPointer<vtkTransformPolyDataFilter> v_tpdf;
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
  // albaEventMacro(albaEvent(this,BIND_TO_PROGRESSBAR, writer));
  long dummyProgressValue = 50;
  
  progressHelper.UpdateProgressBar(dummyProgressValue);

  writer->SetFileName(m_File.GetCStr());
  writer->Write();
}

//----------------------------------------------------------------------------
char ** albaOpExporterVTK::GetIcon()
{
#include "pic/MENU_IMPORT_VTK.xpm"
	return MENU_IMPORT_VTK_xpm;
}