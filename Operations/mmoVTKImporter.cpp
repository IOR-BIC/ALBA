/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoVTKImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-27 12:35:09 $
  Version:   $Revision: 1.1 $
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

#include "mmoVTKImporter.h"
#include <wx/busyinfo.h>
#include "mafEvent.h"
#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafTagArray.h"

#include "vtkDataSet.h"
#include "vtkDataSetReader.h"
#include "vtkPolyDataReader.h"
#include "vtkStructuredPointsReader.h"
#include "vtkStructuredGridReader.h"
#include "vtkRectilinearGridReader.h"
#include "vtkUnstructuredGridReader.h"

//----------------------------------------------------------------------------
mmoVTKImporter::mmoVTKImporter(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_Vme     = NULL;
	m_OpType  = OPTYPE_IMPORTER;
	m_Canundo = true;
	m_File    = "";

  m_FileDir = mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mmoVTKImporter::~mmoVTKImporter( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mmoVTKImporter::Copy()   
//----------------------------------------------------------------------------
{
  //non devo incrementare l'id counter --- vfc le operazioni sono gia inserite nei menu;
  mmoVTKImporter *cp = new mmoVTKImporter(m_Label);
  cp->m_Canundo		= m_Canundo;
  cp->m_File			= m_File;
  return cp;
}
//----------------------------------------------------------------------------
void mmoVTKImporter::OpRun()   
//----------------------------------------------------------------------------
{
	m_File = "";

	wxString wildc = "vtk Data (*.vtk)|*.vtk";
  wxString f = mafGetOpenFile(m_FileDir, wildc, "Choose VTK file").c_str();

  int result = OP_RUN_CANCEL;
  if(f != "") 
	{
	  m_File = f;
	  result = OP_RUN_OK;
	}
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mmoVTKImporter::OpDo()
//----------------------------------------------------------------------------
{
  bool success = false;
  
	wxBusyInfo wait("Loading file: ...");
  assert(!m_Vme);
  
  vtkDataSetReader *reader = vtkDataSetReader::New();
  reader->SetFileName(m_File);

  vtkDataReader *preader;
  // workaround to avoid double reading
  switch (reader->ReadOutputType())
    {
    case VTK_POLY_DATA:  
      preader = vtkPolyDataReader::New();
      break;
    case VTK_STRUCTURED_POINTS:
      preader = vtkStructuredPointsReader::New();
      break;
    case VTK_STRUCTURED_GRID:
      preader = vtkStructuredGridReader::New();
      break;
    case VTK_RECTILINEAR_GRID:
      preader = vtkRectilinearGridReader::New();
      break;
    case VTK_UNSTRUCTURED_GRID:
      preader = vtkUnstructuredGridReader::New();
      break;
    default:
      wxMessageBox("Unsupported file format", "I/O Error", wxICON_ERROR );
      return;
  }
  mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,preader));
  preader->SetFileName(m_File);
  preader->Update();
  
  if (preader->GetNumberOfOutputs()>0)
  {
    wxString path, name, ext;
    wxSplitPath(m_File.c_str(),&path,&name,&ext);

    vtkDataSet *data=vtkDataSet::SafeDownCast(preader->GetOutputs()[0]);

    if (data)
    {
      mafNEW(m_Vme);
      m_Vme->SetName(name.c_str());
      m_Vme->SetDataByDetaching(data,0);

      mafTagItem tag_Nature;
      tag_Nature.SetName("VME_NATURE");
      tag_Nature.SetValue("NATURAL");

      m_Vme->GetTagArray()->SetTag(tag_Nature);

      mafEventMacro(mafEvent(this,VME_ADD,m_Vme));
      success = true;
    }
  }

  //vtkDEL(dsi);
  vtkDEL(reader);
  vtkDEL(preader);
  if(!success)	wxMessageBox("Error reading VTK file.", "I/O Error", wxICON_ERROR );
}
//----------------------------------------------------------------------------
void mmoVTKImporter::OpUndo()   
/**  */
//----------------------------------------------------------------------------
{
  assert(m_Vme);
  mafEventMacro(mafEvent(this,VME_REMOVE,m_Vme));
  mafDEL(m_Vme);
}