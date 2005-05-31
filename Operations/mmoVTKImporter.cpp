/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoVTKImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-31 23:54:49 $
  Version:   $Revision: 1.4 $
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
#include "mafVMEGeneric.h"
#include "mafVMEImage.h"
#include "mafVMEPolyline.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"

#include "mafTagArray.h"
#include "vtkMAFSmartPointer.h"

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
  
  vtkMAFSmartPointer<vtkDataSetReader> reader;
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
      mafSmartPointer<mafVMEPolyline>   vme_poly;
      mafSmartPointer<mafVMESurface>    vme_surf;
      mafSmartPointer<mafVMEVolumeGray> vme_gray;
      mafSmartPointer<mafVMEGeneric>    vme_generic;
      if (vme_poly->SetDataByDetaching(data,0) == MAF_OK)
      {
        m_Vme = vme_poly;
      }
      else if (vme_surf->SetDataByDetaching(data,0) == MAF_OK)
      {
        m_Vme = vme_surf;
      }
      else if (vme_gray->SetDataByDetaching(data,0) == MAF_OK)
      {
        m_Vme = vme_gray;
      }
      else
      {
        vme_generic->SetDataByDetaching(data,0);
        m_Vme = vme_generic;
      }

      mafTagItem tag_Nature;
      tag_Nature.SetName("VME_NATURE");
      tag_Nature.SetValue("NATURAL");
      m_Vme->GetTagArray()->SetTag(tag_Nature);
      m_Vme->SetName(name.c_str());

      mafEventMacro(mafEvent(this,VME_ADD,m_Vme));
      success = true;
    }
  }
  vtkDEL(preader);
  if(!success)
    wxMessageBox("Error reading VTK file.", "I/O Error", wxICON_ERROR );
}
//----------------------------------------------------------------------------
void mmoVTKImporter::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_Vme);
  mafEventMacro(mafEvent(this,VME_REMOVE,m_Vme));
  m_Vme = NULL;
}
