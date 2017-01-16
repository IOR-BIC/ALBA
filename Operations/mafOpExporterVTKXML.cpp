/*=========================================================================

 Program: MAF2
 Module: mafOpExporterVTKXML
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

#include "mafOpExporterVTKXML.h"

#include "mafDecl.h"
#include "mafGUI.h"
#include "vtkMAFSmartPointer.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMERoot.h"
#include "mafTransformBase.h"

#include "vtkXMLDataSetWriter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkAbstractTransform.h"
#include "vtkImageCast.h"
#include "vtkImageData.h"
#include "vtkStructuredPoints.h"
#include "vtkPolyData.h"
#include "mafVMEGroup.h"
#include "vtkTransformFilter.h"
#include "vtkUnstructuredGrid.h"
//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpExporterVTKXML);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpExporterVTKXML::mafOpExporterVTKXML(const wxString &label) :
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
mafOpExporterVTKXML::~mafOpExporterVTKXML()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mafOpExporterVTKXML::Accept(mafVME*node)
//----------------------------------------------------------------------------
{ 
  return (node->IsMAFType(mafVME) && !node->IsMAFType(mafVMERoot) && !node->IsMAFType(mafVMEGroup));
}
//----------------------------------------------------------------------------
mafOp* mafOpExporterVTKXML::Copy()   
//----------------------------------------------------------------------------
{
  mafOpExporterVTKXML *cp = new mafOpExporterVTKXML(m_Label);
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
void mafOpExporterVTKXML::OpRun()   
//----------------------------------------------------------------------------
{
  vtkDataSet *inputData = m_Input->GetOutput()->GetVTKData();
  assert(inputData);

  bool isStructuredPoints = inputData->IsA("vtkStructuredPoints") != 0;

  mafString wildc;
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

  m_Gui = new mafGUI(this);
  m_Gui->FileSave(ID_CHOOSE_FILENAME, _("vtk xml file"), &m_File, wildc);
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
void mafOpExporterVTKXML::OnEvent(mafEventBase *maf_event)
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
        GetLogicManager()->VmeAdd(m_Input);
      }
      break;
      default:
        mafEventMacro(*e);
      break;
    }
	}
}

//----------------------------------------------------------------------------
void mafOpExporterVTKXML::ExportVTK()
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
void mafOpExporterVTKXML::SaveVTKData()
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

  vtkDataSet *inputData = m_Input->GetOutput()->GetVTKData();
  assert(inputData);

  vtkDataSet *writerInput = inputData;


  vtkMAFSmartPointer<vtkXMLDataSetWriter> writer;

	if (m_ABSMatrixFlag)
	{
		vtkMAFSmartPointer <vtkTransform> tra;
		tra->SetMatrix(m_Input->GetOutput()->GetAbsMatrix()->GetVTKMatrix());

		if (m_Input->IsA("mafVMEMesh"))
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
    writer->SetDataModeToBinary();
  else
    writer->SetDataModeToAscii();

  mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,writer));

  writer->SetFileName(m_File.GetCStr());
  writer->Write();
  
  if (busyCursor)
  {
    delete busyCursor;
  }
}
