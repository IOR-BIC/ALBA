/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterPLY
 Authors: Gianluigi Crimi
 
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

#include "albaOpExporterPLY.h"

#include "albaDecl.h"
#include "albaGUI.h"

#include "albaVME.h"
#include "albaVMEOutputSurface.h"
#include "albaTransformBase.h"

#include "vtkALBASmartPointer.h"
#include "vtkPLYWriter.h"
#include "vtkTriangleFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpExporterPLY);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpExporterPLY::albaOpExporterPLY(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_File    = "";

	m_Binary        = 1;
	m_ABSMatrixFlag = 1;
}
//----------------------------------------------------------------------------
albaOpExporterPLY::~albaOpExporterPLY()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool albaOpExporterPLY::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->GetOutput()->IsALBAType(albaVMEOutputSurface));
}
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum PLY_EXPORTER_ID
{
  ID_PLY_BINARY_FILE = MINID,
	ID_ABS_MATRIX_TO_PLY,
  ID_CHOOSE_FILENAME,
};
//----------------------------------------------------------------------------
void albaOpExporterPLY::OpRun()   
//----------------------------------------------------------------------------
{
  albaString wildc = "Polygon File Format (*.ply)|*.ply";

  m_Gui = new albaGUI(this);
	m_Gui->FileSave(ID_CHOOSE_FILENAME,"ply file", &m_File, wildc,"Save As...");
  m_Gui->Label("file type",true);
	m_Gui->Bool(ID_PLY_BINARY_FILE,"binary",&m_Binary,0);
	m_Gui->Label("absolute matrix",true);
	m_Gui->Bool(ID_ABS_MATRIX_TO_PLY,"apply",&m_ABSMatrixFlag,0);
	m_Gui->OkCancel();
  m_Gui->Enable(wxOK,m_File != "");
	
	m_Gui->Divider();

	ShowGui();
}
//----------------------------------------------------------------------------
void albaOpExporterPLY::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
    switch(e->GetId())
    {
      case wxOK:
        ExportSurface();
        OpStop(OP_RUN_OK);
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
void albaOpExporterPLY::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	albaEventMacro(albaEvent(this,result));        
}
//----------------------------------------------------------------------------
void albaOpExporterPLY::ExportSurface()
//----------------------------------------------------------------------------
{
  albaVMEOutputSurface *out_surface = albaVMEOutputSurface::SafeDownCast(m_Input->GetOutput());
  out_surface->Update();

  vtkALBASmartPointer<vtkTriangleFilter>triangles;
	vtkALBASmartPointer<vtkTransformPolyDataFilter> v_tpdf;
	triangles->SetInputData(out_surface->GetSurfaceData());
	triangles->Update();

	v_tpdf->SetInputConnection(triangles->GetOutputPort());
	v_tpdf->SetTransform(out_surface->GetAbsTransform()->GetVTKTransform());
	v_tpdf->Update();

	vtkALBASmartPointer<vtkPLYWriter> writer;
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
}
//----------------------------------------------------------------------------
albaOp* albaOpExporterPLY::Copy()   
//----------------------------------------------------------------------------
{
  albaOpExporterPLY *cp = new albaOpExporterPLY(m_Label);
  cp->m_File = m_File;
  return cp;
}

