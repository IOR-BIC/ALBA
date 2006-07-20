/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoSTLExporter.cpp,v $
  Language:  C++
  Date:      $Date: 2006-07-20 17:36:33 $
  Version:   $Revision: 1.3 $
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

#include "mmoSTLExporter.h"

#include "mafDecl.h"
#include "mmgGui.h"

#include "mafVMESurface.h"
#include "mafVMEOutputSurface.h"
#include "mafTransformBase.h"

#include "vtkMAFSmartPointer.h"
#include "vtkSTLWriter.h"
#include "vtkTriangleFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"

//----------------------------------------------------------------------------
mmoSTLExporter::mmoSTLExporter(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_File    = "";

	m_Binary        = 1;
	m_ABSMatrixFlag = 1;

	m_FileDir = mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mmoSTLExporter::~mmoSTLExporter( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mmoSTLExporter::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVMESurface));
}
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum STL_EXPORTER_ID
{
  ID_STL_BINARY_FILE = MINID,
	ID_ABS_MATRIX_TO_STL,
  ID_CHOOSE_FILENAME,
};
//----------------------------------------------------------------------------
void mmoSTLExporter::OpRun()   
//----------------------------------------------------------------------------
{
  mafString wildc = "Stereo Litography (*.stl)|*.stl";

  m_Gui = new mmgGui(this);
	//m_Gui->FileSave(ID_CHOOSE_FILENAME,"stl file", &m_File, wildc,"Save As...");
  m_Gui->Label("file type",true);
	m_Gui->Bool(ID_STL_BINARY_FILE,"binary",&m_Binary,0);
	m_Gui->Label("absolute matrix",true);
	m_Gui->Bool(ID_ABS_MATRIX_TO_STL,"apply",&m_ABSMatrixFlag,0);
	m_Gui->OkCancel();
  //m_Gui->Enable(wxOK,m_File != "");
	
	ShowGui();
}
//----------------------------------------------------------------------------
void mmoSTLExporter::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
    switch(e->GetId())
    {
      case wxOK:
				{
					mafString FileDir = mafGetApplicationDirectory().c_str();
					FileDir<<"\\";
					FileDir<<this->m_Input->GetName();
					FileDir<<".stl";
					mafString wildc = "STL (*.stl)|*.stl";
					m_File = mafGetSaveFile(FileDir.GetCStr(), wildc.GetCStr()).c_str();
					if(m_File!="")
					{
						ExportSurface();
						OpStop(OP_RUN_OK);
					}
					else
						OpStop(OP_RUN_CANCEL);
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
void mmoSTLExporter::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void mmoSTLExporter::ExportSurface()
//----------------------------------------------------------------------------
{
	((mafVMESurface *)m_Input)->GetSurfaceOutput()->Update();
	
	vtkMAFSmartPointer<vtkTriangleFilter>triangles;
	triangles->SetInput(((mafVMESurface *)m_Input)->GetSurfaceOutput()->GetSurfaceData());
	triangles->Update();

	vtkMAFSmartPointer<vtkTransformPolyDataFilter> v_tpdf;
	v_tpdf->SetInput(triangles->GetOutput());
  v_tpdf->SetTransform(((mafVMESurface *)m_Input)->GetOutput()->GetAbsTransform()->GetVTKTransform());
	v_tpdf->Update();

	vtkMAFSmartPointer<vtkSTLWriter> writer;
  mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,writer));
	writer->SetFileName(m_File.GetCStr());
	if(this->m_ABSMatrixFlag)
		writer->SetInput(v_tpdf->GetOutput());
	else
		writer->SetInput(triangles->GetOutput());
	if(this->m_Binary)
		writer->SetFileTypeToBinary();
	else
		writer->SetFileTypeToASCII();
	writer->Update();
}
//----------------------------------------------------------------------------
mafOp* mmoSTLExporter::Copy()   
//----------------------------------------------------------------------------
{
  mmoSTLExporter *cp = new mmoSTLExporter(m_Label);
  cp->m_File = m_File;
  return cp;
}
