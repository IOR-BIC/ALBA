/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterVTKXML
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
#include <wx/busyinfo.h>

#include "albaOpExporterVTKXML.h"

#include "albaDecl.h"
#include "albaGUI.h"
#include "vtkALBASmartPointer.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMERoot.h"
#include "albaTransformBase.h"

#include "vtkXMLDataSetWriter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkAbstractTransform.h"
#include "vtkImageCast.h"
#include "vtkImageData.h"
#include "vtkImageData.h"
#include "vtkPolyData.h"
#include "albaVMEGroup.h"
#include "vtkTransformFilter.h"
#include "vtkUnstructuredGrid.h"
//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpExporterVTKXML);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpExporterVTKXML::albaOpExporterVTKXML(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_File    = "";
  m_Input   = NULL;

	m_Binary        = 1;
	m_ABSMatrixFlag = 0;

	m_FileDir = albaGetLastUserFolder().c_str();
  m_ForceUnsignedShortScalarOutputForStructuredPoints = FALSE;
}
//----------------------------------------------------------------------------
albaOpExporterVTKXML::~albaOpExporterVTKXML()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool albaOpExporterVTKXML::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{ 
  return (node->IsALBAType(albaVME) && !node->IsALBAType(albaVMERoot) && !node->IsALBAType(albaVMEGroup));
}
//----------------------------------------------------------------------------
albaOp* albaOpExporterVTKXML::Copy()   
//----------------------------------------------------------------------------
{
  albaOpExporterVTKXML *cp = new albaOpExporterVTKXML(m_Label);
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
void albaOpExporterVTKXML::OpRun()   
//----------------------------------------------------------------------------
{
  vtkDataSet *inputData = m_Input->GetOutput()->GetVTKData();
  assert(inputData);

  bool isStructuredPoints = inputData->IsA("vtkImageData") != 0;

  albaString wildc;
  if(inputData->IsA("vtkRectilinearGrid"))
  {
    wildc = "vtk xml Data (*.vtr)|*.vtr";
  }
  if(inputData->IsA("vtkPolyData"))
  {
    wildc = "vtk xml Data (*.vtp)|*.vtp";
  }
  if(inputData->IsA("vtkImageData"))
  {
    wildc = "vtk xml Data (*.vti)|*.vti";
  }
  if(inputData->IsA("vtkUnstructuredGrid"))
  {
    wildc = "vtk xml Data (*.vtu)|*.vtu";
  }
  if(inputData->IsA("vtkStructuredGrid"))
  {
    wildc = "vtk xml Data (*.vts)|*.vts";
  }

  m_Gui = new albaGUI(this);
  m_Gui->FileSave(ID_CHOOSE_FILENAME, _("vtk xml file"), &m_File, wildc);
	m_Gui->Label("file type",true);
	m_Gui->Bool(ID_VTK_BINARY_FILE,"binary",&m_Binary,0);
	m_Gui->Label("absolute matrix",true);
	m_Gui->Bool(ID_ABS_MATRIX,"apply",&m_ABSMatrixFlag,0);
	if (m_Input->IsA("albaVMESurface") || m_Input->IsA("albaVMEPointSet") || m_Input->IsA("albaVMEGroup"))
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
void albaOpExporterVTKXML::OnEvent(albaEventBase *alba_event)
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
      default:
        albaEventMacro(*e);
      break;
    }
	}
}

//----------------------------------------------------------------------------
void albaOpExporterVTKXML::ExportVTK()
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
void albaOpExporterVTKXML::SaveVTKData()
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
    albaLogMessage(stringStream.str().c_str());
  }

  vtkDataSet *inputData = m_Input->GetOutput()->GetVTKData();
  assert(inputData);

  vtkDataSet *writerInput = inputData;


  vtkALBASmartPointer<vtkXMLDataSetWriter> writer;

	if (m_ABSMatrixFlag)
	{
		vtkALBASmartPointer <vtkTransform> tra;
		tra->SetMatrix(m_Input->GetOutput()->GetAbsMatrix()->GetVTKMatrix());

		if (m_Input->IsA("albaVMEMesh"))
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
    writer->SetDataModeToBinary();
  else
    writer->SetDataModeToAscii();

  albaEventMacro(albaEvent(this,BIND_TO_PROGRESSBAR,writer));

  writer->SetFileName(m_File.GetCStr());
  writer->Write();
  
  if (busyCursor)
  {
    delete busyCursor;
  }
}
