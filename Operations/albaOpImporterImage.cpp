/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterImage
 Authors: Paolo Quadrani
 
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

#include "albaOpImporterImage.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"

#include "albaTagArray.h"
#include "albaVMEVolumeRGB.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEGroup.h"
#include "albaVMEImage.h"
#include "albaVMEItem.h"

#include "vtkALBASmartPointer.h"
#include "vtkImageData.h"
#include "vtkBMPReader.h"
#include "vtkJPEGReader.h"
#include "vtkPNGReader.h"
#include "vtkALBATIFFReader.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageLuminance.h"

#include <algorithm>
#include "albaProgressBarHelper.h"
#include "wx/filename.h"
#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkDataSetAttributes.h"
#include "vtkUnsignedCharArray.h"
#include "albaOpImporterDicomSliceAccHelper.h"
#include "albaGUIDialogWarnAndSkipOthers.h"
#include "albaGUIBusyInfo.h"
#include "vtkImageFlip.h"



//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterImage);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpImporterImage::albaOpImporterImage(const wxString& label) : albaOpImporterFile(label)
{
	SetWildc("Images (*.bmp;*.jpg;*.jpeg;*.png;*.tif;*.tiff)| *.bmp;*.jpg;*.jpeg;*.png;*.tif;*.tiff|Bitmap (*.bmp)|*.bmp|JPEG (*.jpg;*.jpeg)|*.jpg;*.jpeg|PNG (*.png)|*.png|TIFF (*.tif;*.tiff)|*.tif;*.tiff");
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_Files.clear();
	m_BuildVolumeFlag = false;

	m_Spacing[0] = m_Spacing[1] = m_Spacing[2] = 1.0;

	m_SkipWrongType = m_SkipWrongSize = false;

	m_ZFlip = m_XFlip = m_YFlip = false;

  m_ImportedGroup = NULL;
  m_ImportedVolume = NULL;
}
//----------------------------------------------------------------------------
albaOpImporterImage::~albaOpImporterImage()
//----------------------------------------------------------------------------
{
  albaDEL(m_ImportedGroup);
  albaDEL(m_ImportedVolume);
}
//----------------------------------------------------------------------------
// constant ID
//----------------------------------------------------------------------------
enum IMAGE_IMPORTER_ID
{
  ID_BUILD_VOLUME = MINID,
  ID_SPACING,
	ID_XFLIP,
	ID_YFLIP,
	ID_ZFLIP,
};
//----------------------------------------------------------------------------
void albaOpImporterImage::OpRun()   
//----------------------------------------------------------------------------
{
	
  if (!m_TestMode)
  {
    m_Files.clear();
	  m_Gui = new albaGUI(this);

		std::vector<wxString> files;
			
		albaGetOpenMultiFiles((const char *)albaGetLastUserFolder().ToAscii(), m_Wildc, files);

		for (int i = 0; i < files.size(); i++)
			m_Files.push_back(files[i].ToAscii());
  }

  int numFiles = m_Files.size();

  if(numFiles == 0)
  {
    OpStop(OP_RUN_CANCEL);
  }
  else
  {
    if (!m_TestMode)
    {
			const wxString outputTypes[] = { _("Images"), _("Volume") };

			m_Gui->Label("");

			if (numFiles > 1)
				m_Gui->Radio(ID_BUILD_VOLUME, _("Output Type"), &m_BuildVolumeFlag, 2, outputTypes);
			else
				m_BuildVolumeFlag = false;

			m_Gui->Vector(ID_SPACING, "Spacing:", m_Spacing, 0);
			m_Gui->Label("");
			m_Gui->Bool(ID_XFLIP, "Flip around X axis", &m_XFlip, 1);
			m_Gui->Bool(ID_YFLIP, "Flip around Y axis", &m_YFlip, 1);
			m_Gui->Bool(ID_ZFLIP, "Flip around Z axis", &m_ZFlip, 1);
			
			m_Gui->Label("");
      m_Gui->OkCancel();

      m_Gui->Update();

      ShowGui();
    }
  }
}
//----------------------------------------------------------------------------
albaOp* albaOpImporterImage::Copy()   
//----------------------------------------------------------------------------
{
  return (new albaOpImporterImage(m_Label));
}
//----------------------------------------------------------------------------
void albaOpImporterImage::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
void albaOpImporterImage::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case wxOK:
        Import();
        OpStop(OP_RUN_OK);
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);
      break;
    }
  }
}

//----------------------------------------------------------------------------
void albaOpImporterImage::SetSpacing(double* spacing)
{
	m_Spacing[0] = spacing[0];
	m_Spacing[1] = spacing[1];
	m_Spacing[2] = spacing[2];
  }

//----------------------------------------------------------------------------
void albaOpImporterImage::Import()
//----------------------------------------------------------------------------
{
  wxString path, name, ext;
	std::vector<vtkImageData*> images;

	albaGUIBusyInfo busy("Importing Images.\nPlease wait...", m_TestMode);
	
	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar();
	vtkImageReader2* reader = NULL;

	int numFiles = m_Files.size();
	if (!m_BuildVolumeFlag)
	{
		albaNEW(m_ImportedGroup);
		m_ImportedGroup->SetName("Imported Images");
		m_ImportedGroup->ReparentTo(m_Input);
	}

	for (int i = 0; i < numFiles; i++)
	{
		int index;
		index = m_ZFlip ? numFiles - (i + 1) : i;

		progressHelper.UpdateProgressBar((i * 100) / numFiles);

		wxFileName::SplitPath(m_Files[index].c_str(), &path, &name, &ext);
		ext.MakeUpper();

		albaSmartPointer <albaVMEImage> importedImage;


		if (ext == "BMP")
			reader = vtkBMPReader::New();
		else if (ext == "JPG" || ext == "JPEG")
			reader = vtkJPEGReader::New();
		else if (ext == "PNG")
			reader = vtkPNGReader::New();
		else if (ext == "TIF" || ext == "TIFF")
			reader = vtkALBATIFFReader::New();
		else if (m_SkipWrongType == FALSE)
		{
			albaString msg;
			msg.Printf("unable to import %s, unrecognized type", m_Files[index].c_str());
			if (m_TestMode)
			{
				albaLogMessage(msg.GetCStr());
			}
		else
			{
				albaGUIDialogWarnAndSkipOthers* dialog = new albaGUIDialogWarnAndSkipOthers("Wrong image size", msg.GetCStr(), &m_SkipWrongType);
				dialog->ShowModal();
			}
		}

		if (reader)
		{
			reader->SetFileName(m_Files[index].c_str());
			reader->SetDataSpacing(m_Spacing);
			reader->Update();

			vtkALBASmartPointer<vtkImageLuminance> lumFilter;
			if (reader->GetOutput()->GetNumberOfScalarComponents() == 4)
			{
				vtkALBASmartPointer<vtkImageExtractComponents> extract;
				extract->SetInputConnection(reader->GetOutputPort());
				extract->SetComponents(0, 1, 2);
				extract->Update();
				lumFilter->SetInputConnection(extract->GetOutputPort());
			}
			else
		{
				lumFilter->SetInputConnection(reader->GetOutputPort());
		} 
			lumFilter->Update();
			vtkImageData* finalImage = lumFilter->GetOutput();
			
			vtkALBASmartPointer<vtkImageFlip> xFlipFilter;
			if (m_XFlip)
		{
				xFlipFilter->SetInputData(finalImage);
				xFlipFilter->SetFilteredAxis(0); // X
				xFlipFilter->Update();
				finalImage = xFlipFilter->GetOutput();
		}

			vtkALBASmartPointer<vtkImageFlip> yFlipFilter;
			if (!m_YFlip) //invert Y flip logic for image coordinate system
		{
				yFlipFilter->SetInputData(finalImage);
				yFlipFilter->SetFilteredAxis(1); // Y
				yFlipFilter->Update();
				finalImage = yFlipFilter->GetOutput();
		}


			if (m_BuildVolumeFlag)
		{
				AddImageToList(images, finalImage, name.ToAscii());
		}
		else
			{
				importedImage->SetData(finalImage, m_Input->GetMTime());
				importedImage->SetName(name);
				SetNaturalTag(importedImage);
				importedImage->ReparentTo(m_ImportedGroup);
			}
			vtkDEL(reader);
		}
	}

	if (m_BuildVolumeFlag)
	{
		int sliceNum = images.size();


		if (sliceNum < 2)
		{
			wxMessageBox("Unable to create a Volume, too few images");
			for (int i = 0; i < sliceNum + 1; i++)
				vtkDEL(images[i]);
			return;
		}

		albaOpImporterDicomSliceAccHelper accumulate;
		accumulate.SetNumOfSlices(sliceNum);

		double origin[3] = { 0,0,0 };

		//Loop for each slice
		for (int i = 0; i < sliceNum; i++)
		{
			origin[2] = i * m_Spacing[2];
			accumulate.SetSlice(i, images[i], origin);
			vtkDEL(images[i]);
	}

		vtkDataSet* acc_out;
		acc_out = accumulate.GetNewOutput();

		albaNEW(m_ImportedVolume);
		m_ImportedVolume->SetName("Imported Volume");
		m_ImportedVolume->SetDataByDetaching(acc_out, m_Input->GetMTime());
		SetNaturalTag(m_ImportedVolume);

		m_Output = m_ImportedVolume;
	}
  else
	{
		m_Output = m_ImportedGroup;
	}
}
//----------------------------------------------------------------------------
void albaOpImporterImage::SetNaturalTag(albaVME* vme)
{
	albaTagItem tag_Nature;
	tag_Nature.SetName("VME_NATURE");
	tag_Nature.SetValue("NATURAL");

	vme->GetTagArray()->SetTag(tag_Nature);
}

//----------------------------------------------------------------------------
void albaOpImporterImage::AddImageToList(std::vector<vtkImageData*> &images, vtkImageData* image,const char *name)
{
	if (images.size() != 0)
	{
		int *groupDims = images[0]->GetDimensions();
		int* imageDims = image->GetDimensions();
		vtkImageData* pushImg = NULL;

		if (groupDims[0] == imageDims[0] && groupDims[1] == imageDims[1] && groupDims[2] == imageDims[2])
		{
			vtkNEW(pushImg);
			pushImg->DeepCopy(image);
			images.push_back(pushImg);
		}
		else if (m_SkipWrongSize == FALSE)
		{
			albaString msg;
			msg.Printf("Unable to import %s:\nImage dimensions are not consistent with the other images\n", name);
			if (m_TestMode)
			{
				albaLogMessage(msg.GetCStr());
	}
	else
  {
				albaGUIDialogWarnAndSkipOthers* dialog = new albaGUIDialogWarnAndSkipOthers("Wrong image size", msg.GetCStr(), &m_SkipWrongSize);
				dialog->ShowModal();
			}
		}
	}
	else
	{
		vtkImageData* pushImg = NULL;
		vtkNEW(pushImg);
		pushImg->DeepCopy(image);
		images.push_back(pushImg);
  }
  
}

//----------------------------------------------------------------------------
void albaOpImporterImage::AddFileName(const char *file_name)
{
 m_Files.push_back(file_name);
}

//----------------------------------------------------------------------------
void albaOpImporterImage::SetFileName(albaString filename)
{
	Superclass::SetFileName(filename);
	m_Files.resize(1);
	m_Files[0] = filename;
}

//----------------------------------------------------------------------------
char ** albaOpImporterImage::GetIcon()
{
#include "pic/MENU_IMPORT_IMAGE.xpm"
	return MENU_IMPORT_IMAGE_xpm;
}

//----------------------------------------------------------------------------
int albaOpImporterImage::ImportFile()
{
  // Forward to existing importer method
  Import();

	if(m_Output)
		return ALBA_OK;
	else
		return ALBA_ERROR;
}

