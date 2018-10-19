/*=========================================================================

 Program: MAF2
 Module: mafOpImporterVTK
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

#include "mafOpImporterVTK.h"
#include <wx/busyinfo.h>
#include "mafEvent.h"

#include "mafVME.h"
#include "mafVMEGeneric.h"
#include "mafVMEImage.h"
//#include "mafVMEPointSet.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEPolyline.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEVolumeRGB.h"
#include "mafVMEMesh.h"

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
mafCxxTypeMacro(mafOpImporterVTK);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpImporterVTK::mafOpImporterVTK(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
	m_Canundo = true;
	m_File    = "";

  //m_VmePointSet = NULL;
  m_VmeLandmarkCloud = NULL;
  m_VmePolyLine = NULL;
  m_VmeSurface  = NULL;
  m_VmeImage    = NULL;
  m_VmeGrayVol  = NULL;
  m_VmeRGBVol  = NULL;
  m_VmeMesh     = NULL;
  m_VmeGeneric  = NULL;

  m_FileDir = mafGetLastUserFolder().c_str();
}
//----------------------------------------------------------------------------
mafOpImporterVTK::~mafOpImporterVTK()
//----------------------------------------------------------------------------
{
  //mafDEL(m_VmePointSet);
  mafDEL(m_VmeLandmarkCloud);
  mafDEL(m_VmePolyLine);
  mafDEL(m_VmeSurface);
  mafDEL(m_VmeImage);
  mafDEL(m_VmeGrayVol);
  mafDEL(m_VmeRGBVol);
  mafDEL(m_VmeMesh);
  mafDEL(m_VmeGeneric);
}
//----------------------------------------------------------------------------
mafOp* mafOpImporterVTK::Copy()   
//----------------------------------------------------------------------------
{
  mafOpImporterVTK *cp = new mafOpImporterVTK(m_Label);
  cp->m_File			= m_File;
  return cp;
}
//----------------------------------------------------------------------------
void mafOpImporterVTK::OpRun()   
//----------------------------------------------------------------------------
{
	mafString wildc = "vtk Data (*.vtk)|*.vtk";
  mafString f;
  if (m_File.IsEmpty())
  {
    f = mafGetOpenFile(m_FileDir, wildc, _("Choose VTK file")).c_str();
    m_File = f;
  }

  int result = OP_RUN_CANCEL;
  if(!m_File.IsEmpty())
	{
    if (ImportVTK() == MAF_OK)
    {
      result = OP_RUN_OK;
    }
    else
    {
      if(!this->m_TestMode)
        mafMessage(_("Unsupported file format"), _("I/O Error"), wxICON_ERROR );
    }
	}

	OpStop(result);
}
//----------------------------------------------------------------------------
int mafOpImporterVTK::ImportVTK()
//----------------------------------------------------------------------------
{
	bool success = false;
	if (!this->m_TestMode)
		wxBusyInfo wait(_("Loading file: ..."));

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
		return MAF_ERROR;
	}
	mafEventMacro(mafEvent(this, BIND_TO_PROGRESSBAR, preader));
	preader->SetFileName(m_File);
	preader->Update();

	if (preader->GetNumberOfOutputs() > 0)
	{
		wxString path, name, ext;
		wxSplitPath(m_File.c_str(), &path, &name, &ext);

		vtkDataSet *data = vtkDataSet::SafeDownCast(preader->GetOutputs()[0]);
		if (data)
		{
			mafNEW(m_VmeLandmarkCloud);
			//mafNEW(m_VmePointSet);
			mafNEW(m_VmePolyLine);
			mafNEW(m_VmeSurface);
			mafNEW(m_VmeImage);
			mafNEW(m_VmeGrayVol);
			mafNEW(m_VmeRGBVol);
			mafNEW(m_VmeMesh);
			mafNEW(m_VmeGeneric);
			//if (m_VmePointSet->SetDataByDetaching(data,0) == MAF_OK)
			//{
			//  m_Output = m_VmePointSet;
			//}
			if (m_VmeLandmarkCloud->SetDataByDetaching(data, 0) == MAF_OK)
			{
				m_VmeLandmarkCloud->CreateLMStructureFromDataVector();
				m_Output = m_VmeLandmarkCloud;
			}
			else if (m_VmePolyLine->SetDataByDetaching(data, 0) == MAF_OK)
			{
				m_Output = m_VmePolyLine;
			}
			else if (m_VmeSurface->SetDataByDetaching(data, 0) == MAF_OK)
			{
				m_Output = m_VmeSurface;
			}
			else if (m_VmeImage->SetDataByDetaching(data, 0) == MAF_OK)
			{
				m_Output = m_VmeImage;
			}
			else if (m_VmeGrayVol->SetDataByDetaching(data, 0) == MAF_OK)
			{
				m_Output = m_VmeGrayVol;
			}
			else if (m_VmeRGBVol->SetDataByDetaching(data, 0) == MAF_OK)
			{
				m_Output = m_VmeRGBVol;
			}
			else if (m_VmeMesh->SetDataByDetaching(data, 0) == MAF_OK)
			{
				m_Output = m_VmeMesh;
			}
			else
			{
				m_VmeGeneric->SetDataByDetaching(data, 0);
				m_Output = m_VmeGeneric;
			}

			mafTagItem tag_Nature;
			tag_Nature.SetName("VME_NATURE");
			tag_Nature.SetValue("NATURAL");
			m_Output->GetTagArray()->SetTag(tag_Nature);
			m_Output->ReparentTo(m_Input);
			m_Output->SetName(name.c_str());

			success = true;
		}
	}
	vtkDEL(preader);
	if (!success && !this->m_TestMode)
	{
		mafMessage(_("Error reading VTK file."), _("I/O Error"), wxICON_ERROR);
		return MAF_ERROR;
	}
	return MAF_OK;
}

//----------------------------------------------------------------------------
void mafOpImporterVTK::OpStop(int result)
{
				mafEventMacro(mafEvent(this, result));
}

//----------------------------------------------------------------------------
char ** mafOpImporterVTK::GetIcon()
{
#include "pic/MENU_IMPORT_VTK.xpm"
	return MENU_IMPORT_VTK_xpm;
}
