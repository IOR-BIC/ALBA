/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterVTKXML
 Authors: Matteo Giacomoni
 
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

#include "albaOpImporterVTKXML.h"
#include <wx/busyinfo.h>

#include "albaVMEPolylineGraph.h"

#include "albaVMEGeneric.h"
#include "albaVMEImage.h"
#include "albaVMEPointSet.h"
#include "albaVMESurface.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEVolumeRGB.h"
#include "albaVMEMesh.h"
#include "albaTagArray.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkUnstructuredGrid.h"
#include "vtkStructuredGrid.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkXMLImageDataReader.h"
#include "vtkXMLRectilinearGridReader.h"
#include "vtkXMLUnstructuredGridReader.h"
#include "vtkCallbackCommand.h"
#include "vtkXMLStructuredGridReader.h"
#include "vtkALBASmartPointer.h"
#include "wx/filename.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterVTKXML);
//----------------------------------------------------------------------------

static int m_ErrorCount = 0;

//----------------------------------------------------------------------------
albaOpImporterVTKXML::albaOpImporterVTKXML(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_Input   = NULL;

  m_VmePointSet = NULL;
  m_VmePolyLine = NULL;
  m_VmeSurface  = NULL;
  m_VmeGrayVol  = NULL;
  m_VmeRGBVol  = NULL;
  m_VmeMesh     = NULL;
  m_VmeGeneric  = NULL;

  m_EventRouter = NULL;
  vtkNEW(m_EventRouter);
  m_EventRouter->SetCallback(ErrorProcessEvents);
  m_EventRouter->SetClientData(this);

  m_File    = "";
  m_FileDir = "";

  ResetErrorCount();
}
//----------------------------------------------------------------------------
albaOpImporterVTKXML::~albaOpImporterVTKXML()
//----------------------------------------------------------------------------
{
  albaDEL(m_VmePointSet);
  albaDEL(m_VmePolyLine);
  albaDEL(m_VmeSurface);
  albaDEL(m_VmeGrayVol);
  albaDEL(m_VmeRGBVol);
  albaDEL(m_VmeMesh);
  albaDEL(m_VmeGeneric);

  vtkDEL(m_EventRouter);
}
//----------------------------------------------------------------------------
bool albaOpImporterVTKXML::InternalAccept(albaVME* node)   
//----------------------------------------------------------------------------
{
  return true;
}
//----------------------------------------------------------------------------
albaOp* albaOpImporterVTKXML::Copy()   
//----------------------------------------------------------------------------
{
  albaOpImporterVTKXML *cp = new albaOpImporterVTKXML(m_Label);
  cp->m_File = m_File;
  return cp;
}
//----------------------------------------------------------------------------
void albaOpImporterVTKXML::OpRun()   
//----------------------------------------------------------------------------
{
  albaString f;
  if (m_File.IsEmpty())
  {
    f = albaGetOpenFile(m_FileDir, GetWildCard(), _("Choose VTK XML file"));
    m_File = f;
  }

  int result = OP_RUN_CANCEL;
  if(!m_File.IsEmpty())
  {
    if (ImportVTKXML() == ALBA_OK)
    {
      wxString path, name, ext;
      wxFileName::SplitPath(m_File.GetCStr(),&path,&name,&ext);
      albaTagItem tag_Nature;
      tag_Nature.SetName("VME_NATURE");
      tag_Nature.SetValue("NATURAL");
      m_Output->GetTagArray()->SetTag(tag_Nature);
      m_Output->ReparentTo(m_Input);
      m_Output->SetName(name.ToAscii());

      result = OP_RUN_OK;
    }
    else
    {
      if(!this->m_TestMode)
        albaMessage(_("Unsupported file format"), _("I/O Error"), wxICON_ERROR );
    }
  }
  albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
void albaOpImporterVTKXML::SetFileName(albaString filename)
//----------------------------------------------------------------------------
{
  m_File = filename;
}
//----------------------------------------------------------------------------
albaString albaOpImporterVTKXML::GetWildCard()
//----------------------------------------------------------------------------
{
	albaString wildc = "vtk xml ImageData (*.vti)|*.vti ";
	wildc += "|vtk xml PolyData (*.vtp)|*.vtp";
	wildc += "|vtk xml StructuredGrid (*.vts)|*.vts";
	wildc += "|vtk xml RectilinearGrid (*.vtr)|*.vtr";
	wildc += "|vtk xml UnstructuredGrid (*.vtu)|*.vtu";

	return wildc;
}
//----------------------------------------------------------------------------
void albaOpImporterVTKXML::ErrorProcessEvents(vtkObject* sender, unsigned long channel, void* clientdata, void* calldata)
//----------------------------------------------------------------------------
{
  m_ErrorCount++;
}
//----------------------------------------------------------------------------
void albaOpImporterVTKXML::ResetErrorCount()
//----------------------------------------------------------------------------
{
  m_ErrorCount = 0;
}
//----------------------------------------------------------------------------
int albaOpImporterVTKXML::ImportVTKXML()
//----------------------------------------------------------------------------
{
  vtkDataSet *data = NULL;

  albaNEW(m_VmePointSet);
  albaNEW(m_VmePolyLine);
  albaNEW(m_VmeSurface);
  albaNEW(m_VmeGrayVol);
  albaNEW(m_VmeRGBVol);
  albaNEW(m_VmeMesh);
  albaNEW(m_VmeGeneric);

	wxString path, name, ext;
	wxFileName::SplitPath(m_File.GetCStr(), &path, &name, &ext);

	if (ext == "vti")
	{
		vtkALBASmartPointer<vtkXMLImageDataReader> imageReader;
		imageReader->AddObserver(vtkCommand::ErrorEvent, m_EventRouter);
		imageReader->SetFileName(m_File.GetCStr());
		imageReader->Update();

		if (m_ErrorCount == 0)
		{
			data = imageReader->GetOutput();

			if (m_VmeGrayVol->SetDataByDetaching(data, 0) == ALBA_OK)
			{
				m_Output = m_VmeGrayVol;
			}
			else if (m_VmeRGBVol->SetDataByDetaching(data, 0) == ALBA_OK)
			{
				m_Output = m_VmeRGBVol;
			}
			else if (m_VmeSurface->SetDataByDetaching(data, 0) == ALBA_OK)
			{
				m_Output = m_VmeSurface;
			}
			else
			{
				m_VmeGeneric->SetDataByDetaching(data, 0);
				m_Output = m_VmeGeneric;
			}

			return ALBA_OK;
		}

		ResetErrorCount();
	}
	else if (ext == "vtp")
	{
		vtkALBASmartPointer<vtkXMLPolyDataReader> polydataReader;
		polydataReader->AddObserver(vtkCommand::ErrorEvent, m_EventRouter);
		polydataReader->SetFileName(m_File.GetCStr());
		polydataReader->Update();

		if (m_ErrorCount == 0)
		{
			data = polydataReader->GetOutput();

			if (m_VmePolyLine->SetDataByDetaching(data, 0) == ALBA_OK)
			{
				m_Output = m_VmePolyLine;
			}
			else if (m_VmeSurface->SetDataByDetaching(data, 0) == ALBA_OK)
			{
				m_Output = m_VmeSurface;
			}
			else if (m_VmePointSet->SetDataByDetaching(data, 0) == ALBA_OK)
			{
				m_Output = m_VmePointSet;
			}
			else
			{
				m_VmeGeneric->SetDataByDetaching(data, 0);
				m_Output = m_VmeGeneric;
			}

			return ALBA_OK;
		}

		ResetErrorCount();
	}
	else if (ext == "vtr")
	{
		vtkALBASmartPointer<vtkXMLRectilinearGridReader> rgReader;
		rgReader->AddObserver(vtkCommand::ErrorEvent, m_EventRouter);
		rgReader->SetFileName(m_File.GetCStr());
		rgReader->Update();

		if (m_ErrorCount == 0)
		{
			data = rgReader->GetOutput();

			if (m_VmeGrayVol->SetDataByDetaching(data, 0) == ALBA_OK)
			{
				m_Output = m_VmeGrayVol;
			}
			else if (m_VmeRGBVol->SetDataByDetaching(data, 0) == ALBA_OK)
			{
				m_Output = m_VmeRGBVol;
			}
			else
			{
				m_VmeGeneric->SetDataByDetaching(data, 0);
				m_Output = m_VmeGeneric;
			}

			return ALBA_OK;
		}

		ResetErrorCount();
	}
	else if (ext == "vtu")
	{
		vtkALBASmartPointer<vtkXMLUnstructuredGridReader> ugReader;
		ugReader->AddObserver(vtkCommand::ErrorEvent, m_EventRouter);
		ugReader->SetFileName(m_File.GetCStr());
		ugReader->Update();

		if (m_ErrorCount == 0)
		{
			data = ugReader->GetOutput();

			if (m_VmeMesh->SetDataByDetaching(data, 0) == ALBA_OK)
			{
				m_Output = m_VmeMesh;
			}
			else
			{
				m_VmeGeneric->SetDataByDetaching(data, 0);
				m_Output = m_VmeGeneric;
			}

			return ALBA_OK;
		}

		ResetErrorCount();
	}
	else if (ext == "vts")
	{
		vtkALBASmartPointer<vtkXMLStructuredGridReader> sgReader;
		sgReader->AddObserver(vtkCommand::ErrorEvent, m_EventRouter);
		sgReader->SetFileName(m_File.GetCStr());
		sgReader->Update();

		if (m_ErrorCount == 0)
		{
			data = sgReader->GetOutput();

			m_VmeGeneric->SetDataByDetaching(data, 0);
			m_Output = m_VmeGeneric;

			return ALBA_OK;
		}

		ResetErrorCount();
	}

  return ALBA_ERROR;
}
