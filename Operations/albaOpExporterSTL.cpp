/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterSTL
 Authors: Paolo Quadrani, Gianluigi Crimi
 
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

#include "albaOpExporterSTL.h"

#include "albaDecl.h"
#include "albaGUI.h"

#include "albaVME.h"
#include "albaVMEOutputSurface.h"
#include "albaTransformBase.h"

#include "vtkALBASmartPointer.h"
#include "vtkSTLWriter.h"
#include "vtkTriangleFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpExporterSTL);

//----------------------------------------------------------------------------
albaOpExporterSTL::albaOpExporterSTL(const wxString &label) : albaOp(label)
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_File    = "";

	m_Binary        = 1;
	m_ABSMatrixFlag = 1;
}
//----------------------------------------------------------------------------
albaOpExporterSTL::~albaOpExporterSTL()
{
}
//----------------------------------------------------------------------------
bool albaOpExporterSTL::InternalAccept(albaVME*node)
{
  return (node && node->GetOutput()->IsALBAType(albaVMEOutputSurface));
}
//----------------------------------------------------------------------------
// constants
enum STL_EXPORTER_ID
{
  ID_STL_BINARY_FILE = MINID,
	ID_ABS_MATRIX_TO_STL,
};
//----------------------------------------------------------------------------
void albaOpExporterSTL::OpRun()   
{
  albaString wildc = "Stereo Litography (*.stl)|*.stl";

  m_Gui = new albaGUI(this);
	m_Gui->Label("");
	m_Gui->Bool(ID_STL_BINARY_FILE, "Binary File type", &m_Binary, 1);
	m_Gui->Bool(ID_ABS_MATRIX_TO_STL, "Apply Absolute matrix", &m_ABSMatrixFlag, 1);
	m_Gui->Label("");
  m_Gui->Divider(1);
	m_Gui->OkCancel();
  m_Gui->Enable(wxOK,m_File != "");
	

	ShowGui();
}
//----------------------------------------------------------------------------
void albaOpExporterSTL::OnEvent(albaEventBase *alba_event)
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
    switch(e->GetId())
    {
      case wxOK:
        if(ExportSurface())
          OpStop(OP_RUN_OK);
        else
          OpStop(OP_RUN_CANCEL);
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
void albaOpExporterSTL::OpStop(int result)
{
	HideGui();
	albaEventMacro(albaEvent(this,result));        
}
//----------------------------------------------------------------------------
int albaOpExporterSTL::ExportSurface()
{

	if (m_File.IsEmpty())
	{
		m_FileDir = albaGetLastUserFolder();
		m_File = m_FileDir + "\\" + m_Input->GetName() + ".stl";

		m_File = albaGetSaveFile(m_FileDir, "Stereo Litography (*.stl)|*.stl", _("Choose STL file"));

		if (m_File.IsEmpty())
			return ALBA_ERROR;
	}

  albaVMEOutputSurface *out_surface = albaVMEOutputSurface::SafeDownCast(m_Input->GetOutput());
  out_surface->Update();

  vtkALBASmartPointer<vtkTriangleFilter>triangles;
	vtkALBASmartPointer<vtkTransformPolyDataFilter> v_tpdf;
	triangles->SetInputData(out_surface->GetSurfaceData());
	triangles->Update();

	v_tpdf->SetInputConnection(triangles->GetOutputPort());
	v_tpdf->SetTransform(out_surface->GetAbsTransform()->GetVTKTransform());
	v_tpdf->Update();

	vtkALBASmartPointer<vtkSTLWriter> writer;
  albaEventMacro(albaEvent(this,BIND_TO_PROGRESSBAR,writer));
	writer->SetFileName(m_File.GetCStr());
	if(this->m_ABSMatrixFlag)
		writer->SetInputConnection(v_tpdf->GetOutputPort());
	else
		writer->SetInputConnection(triangles->GetOutputPort());
	if(this->m_Binary)
		writer->SetFileTypeToBinary();
	else
		writer->SetFileTypeToASCII();
	writer->Update();

	return ALBA_OK;
}
//----------------------------------------------------------------------------
albaOp* albaOpExporterSTL::Copy()   
{
  albaOpExporterSTL *cp = new albaOpExporterSTL(m_Label);
  cp->m_File = m_File;
  return cp;
}

//----------------------------------------------------------------------------
char ** albaOpExporterSTL::GetIcon()
{
#include "pic/MENU_IMPORT_STL.xpm"
	return MENU_IMPORT_STL_xpm;
}
