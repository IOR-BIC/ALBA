/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoVTKImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2005-06-21 11:35:30 $
  Version:   $Revision: 1.5 $
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
  m_OpType  = OPTYPE_IMPORTER;
	m_Canundo = true;
	m_File    = "";

  m_VmePolyLine = NULL;
  m_VmeSurface  = NULL;
  m_VmeGrayVol  = NULL;
  m_VmeGeneric  = NULL;

  m_FileDir = mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mmoVTKImporter::~mmoVTKImporter( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_VmePolyLine);
  mafDEL(m_VmeSurface);
  mafDEL(m_VmeGrayVol);
  mafDEL(m_VmeGeneric);
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
    ImportVTK();
	  result = OP_RUN_OK;
	}
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mmoVTKImporter::ImportVTK()
//----------------------------------------------------------------------------
{
  bool success = false;
	wxBusyInfo wait("Loading file: ...");
  
  vtkMAFSmartPointer<vtkDataSetReader> reader;
  reader->SetFileName(m_File);

  vtkDataReader *preader = NULL;
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

    vtkDataSet *data = vtkDataSet::SafeDownCast(preader->GetOutputs()[0]);
    if (data)
    {
      mafNEW(m_VmePolyLine);
      mafNEW(m_VmeSurface);
      mafNEW(m_VmeGrayVol);
      mafNEW(m_VmeGeneric);
      if (m_VmePolyLine->SetDataByDetaching(data,0) == MAF_OK)
      {
        m_Output = m_VmePolyLine;
      }
      else if (m_VmeSurface->SetDataByDetaching(data,0) == MAF_OK)
      {
        m_Output = m_VmeSurface;
      }
      else if (m_VmeGrayVol->SetDataByDetaching(data,0) == MAF_OK)
      {
        m_Output = m_VmeGrayVol;
      }
      else
      {
        m_VmeGeneric->SetDataByDetaching(data,0);
        m_Output = m_VmeGeneric;
      }

      mafTagItem tag_Nature;
      tag_Nature.SetName("VME_NATURE");
      tag_Nature.SetValue("NATURAL");
      m_Output->GetTagArray()->SetTag(tag_Nature);
      m_Output->SetName(name.c_str());

      success = true;
    }
  }
  vtkDEL(preader);
  if(!success)
    wxMessageBox("Error reading VTK file.", "I/O Error", wxICON_ERROR );
}
