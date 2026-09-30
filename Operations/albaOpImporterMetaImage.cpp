/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterMetaImage
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
//----------------------------------------------------------------------------

#include "albaOpImporterMetaImage.h"
#include "albaEvent.h"

#include "albaVME.h"
#include "albaVMEGeneric.h"
#include "albaVMEImage.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMEPolyline.h"
#include "albaVMESurface.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEVolumeRGB.h"
#include "albaVMEMesh.h"

#include "albaTagArray.h"
#include "vtkALBASmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkImageData.h"
#include "albaGUIBusyInfo.h"
#include "wx/filename.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterMetaImage);

//----------------------------------------------------------------------------
albaOpImporterMetaImage::albaOpImporterMetaImage(const wxString &label) : albaOpImporterFile(label)
{
	SetWildc("Meta Data Image(*.mha) | *.mha");
	m_OpType = OPTYPE_IMPORTER;
	m_Canundo = true;
	m_FileName = "";

	m_VmeImage = NULL;
	m_VmeGrayVol = NULL;

	m_FileDir = albaGetLastUserFolder();
}

//----------------------------------------------------------------------------
albaOpImporterMetaImage::~albaOpImporterMetaImage()
{
	albaDEL(m_VmeImage);
	albaDEL(m_VmeGrayVol);
}

//----------------------------------------------------------------------------
albaOp *albaOpImporterMetaImage::Copy()
{
	albaOpImporterMetaImage *copy = new albaOpImporterMetaImage(m_Label);
	copy->m_FileName = m_FileName;
	return copy;
}

//----------------------------------------------------------------------------
void albaOpImporterMetaImage::OpRun()
{
	albaString fileName;

	if (m_FileName.IsEmpty())
	{
		fileName = albaGetOpenFile(m_FileDir, m_Wildc, _("Choose MetaImage file"));
		m_FileName = fileName;
	}

	int result = OP_RUN_CANCEL;

	if (!m_FileName.IsEmpty())
	{
		if (ImportFile() == ALBA_OK)
		{
			result = OP_RUN_OK;
		}
    else
		{
			albaMessage(_("Unsupported file format"), _("I/O Error"), wxICON_ERROR);
		}
	}

	OpStop(result);
}

//----------------------------------------------------------------------------
int albaOpImporterMetaImage::ImportFile()
{
	bool success = false;
	albaGUIBusyInfo wait(_("Loading file..."), m_TestMode);

	vtkALBASmartPointer<vtkMetaImageReader> reader;
	reader->SetFileName(m_FileName.GetCStr());
	reader->Update();

	vtkImageData *data = reader->GetOutput();

	if (data != NULL)
	{
		wxString path;
		wxString name;
		wxString extension;
		wxFileName::SplitPath(m_FileName.GetCStr(), &path, &name, &extension);

		albaNEW(m_VmeImage);
		albaNEW(m_VmeGrayVol);

		vtkALBASmartPointer<vtkImageData> image;
		image->DeepCopy(data);

		if (m_VmeImage->SetDataByDetaching(image, 0) == ALBA_OK)
			m_Output = m_VmeImage;
		else if (m_VmeGrayVol->SetDataByDetaching(image, 0) == ALBA_OK)
			m_Output = m_VmeGrayVol;

		double orientation[3][3];
		reader->GetOrientationMatrix(orientation);
		albaMatrix absMatrix;
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				absMatrix.SetElement(i, j, orientation[i][j]);
			}
		}



		if (m_Output != NULL)
		{
			albaTagItem natureTag;
			natureTag.SetName("VME_NATURE");
			natureTag.SetValue("NATURAL");

			m_Output->GetTagArray()->SetTag(natureTag);
			m_Output->SetName(name.ToAscii());
			m_Output->SetAbsMatrix(absMatrix);

			m_Output->ReparentTo(m_Input);
			success = true;
		}
	}

	if (!success)
	{
		if (!m_TestMode)
		{
			albaMessage(_("Error reading mha/mhd file."), _("I/O Error"), wxICON_ERROR);
		}

		return ALBA_ERROR;
	}

	return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaOpImporterMetaImage::OpStop(int result)
{
	albaEventMacro(albaEvent(this, result));
}

//----------------------------------------------------------------------------
char **albaOpImporterMetaImage::GetIcon()
{
#include "pic/MENU_IMPORT_VTK.xpm"
	return MENU_IMPORT_VTK_xpm;
}