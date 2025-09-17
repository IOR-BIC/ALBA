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
#include "vtkTIFFReader.h"
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



//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterImage);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpImporterImage::albaOpImporterImage(const wxString& label) :
	albaOp(label)
	//----------------------------------------------------------------------------
{
	m_OpType = OPTYPE_IMPORTER;
	m_Canundo = true;
	m_Files.clear();
	m_BuildVolumeFlag = 0;

	m_Spacing[0] = m_Spacing[1] = m_Spacing[2] = 1.0;

	m_SkipWrongType = m_SkipWrongSize = false;

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
};
//----------------------------------------------------------------------------
void albaOpImporterImage::OpRun()   
//----------------------------------------------------------------------------
{
	albaString wildc = "Bitmap (*.bmp)|*.bmp|JPEG (*.jpg)|*.jpg|PNG (*.png)|*.png|TIFF (*.tif)|*.tif";
	
  if (!m_TestMode)
  {
    m_Files.clear();
	  m_Gui = new albaGUI(this);

		std::vector<wxString> files;
			
		albaGetOpenMultiFiles((const char *)albaGetLastUserFolder().ToAscii(), wildc.GetCStr(), files);

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
			const wxString outputTypes[] = { _("Images"), _("Volume")};

			m_Gui->Radio(ID_BUILD_VOLUME, _("Output Type"), &m_BuildVolumeFlag, 2, outputTypes);

	    m_Gui->Vector(ID_SPACING,"Spacing:",m_Spacing,0);
      m_Gui->Label("");
      m_Gui->OkCancel();

      m_Gui->Update();

			m_Gui->Divider();

      ShowGui();
    }

    if(numFiles == 1)
    {
      Import();
      if (!m_TestMode)
      {
        OpStop(OP_RUN_OK);
      }
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
      case ID_BUILD_VOLUME:
        if (!m_TestMode)
        {
        }
      break;
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
  if (!m_TestMode)
  {
    wxBusyCursor wait;
  }
  if(m_BuildVolumeFlag)
    BuildVolume();        //Build volume
  else
    BuildImageSequence(); // Build image sequence

  albaTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("NATURAL");

  m_Output->GetTagArray()->SetTag(tag_Nature);
}
//----------------------------------------------------------------------------
void albaOpImporterImage::BuildImageSequence()
//----------------------------------------------------------------------------
{
	wxString path, name, ext;

	albaNEW(m_ImportedGroup);
  
	albaTimeStamp start_time = m_Input->GetRoot()->GetTimeStamp();
	
	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar();

  int numFiles = m_Files.size();

  m_ImportedGroup->SetName("Imported Images");
  m_ImportedGroup->ReparentTo(m_Input);

  for(int i=0; i<numFiles; i++)
	{
    progressHelper.UpdateProgressBar((i*100)/numFiles);

    wxFileName::SplitPath(m_Files[i].c_str(),&path,&name,&ext);
		ext.MakeUpper();

    albaSmartPointer <albaVMEImage> importedImage;
		vtkImageReader2* reader=NULL; 

		if(ext == "BMP")
			reader = vtkBMPReader::New();
		else if (ext == "JPG" || ext == "JPEG" )
			reader = vtkJPEGReader::New();
		else if (ext == "PNG")
			reader = vtkPNGReader::New();
		else if (ext == "TIF" || ext == "TIFF" )
			reader = vtkTIFFReader::New();
		else if (m_SkipWrongType == FALSE)
		{
			albaString msg;
			msg.Printf("unable to import %s, unrecognized type", m_Files[i].c_str());
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
			reader->SetFileName(m_Files[i].c_str());
			reader->SetDataSpacing(m_Spacing);
			reader->Update();
			vtkALBASmartPointer<vtkImageLuminance> lumFilter;
			lumFilter->SetInput(reader->GetOutput());
			lumFilter->Update();
			importedImage->SetData(lumFilter->GetOutput(), start_time);
			importedImage->SetName(name);
			importedImage->ReparentTo(m_ImportedGroup);
			vtkDEL(reader);
		}
	}

  m_ImportedGroup->SetName("Imported Images");
  m_ImportedGroup->ReparentTo(m_Input);
  m_Output = m_ImportedGroup;
}
//----------------------------------------------------------------------------
void albaOpImporterImage::BuildVolume()
//----------------------------------------------------------------------------
{
	wxString path, name, ext;

	std::vector<vtkImageData*> images;

	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar();
	vtkImageReader2* reader = NULL;

	int numFiles = m_Files.size();

	for (int i = 0; i < numFiles; i++)
	{
		progressHelper.UpdateProgressBar((i * 100) / numFiles);

		wxFileName::SplitPath(m_Files[i].c_str(), &path, &name, &ext);
		ext.MakeUpper();

		vtkImageData* img;

		if (ext == "BMP")
			reader = vtkBMPReader::New();
		else if (ext == "JPG" || ext == "JPEG")
			reader = vtkJPEGReader::New();
		else if (ext == "PNG")
			reader = vtkPNGReader::New();
		else if (ext == "TIF" || ext == "TIFF")
			reader = vtkTIFFReader::New();
		else if (m_SkipWrongType == FALSE)
		{
			albaString msg;
			msg.Printf("unable to import %s, unrecognized type", m_Files[i].c_str());
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
			reader->SetFileName(m_Files[i].c_str());
			reader->SetDataSpacing(m_Spacing);
			reader->SetNumberOfScalarComponents(1);
			reader->Update();
			vtkALBASmartPointer<vtkImageLuminance> lumFilter;
			lumFilter->SetInput(reader->GetOutput());
			lumFilter->Update();
			AddImageToList(images, lumFilter->GetOutput(), name.ToAscii());
			vtkDEL(reader);
		}

	}

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

	//Loop foreach slice
	for (int i = 0; i < sliceNum; i++)
	{
		origin[2] = i * m_Spacing[2];
		accumulate.SetSlice(i, images[i], origin);
		vtkDEL(images[i]);
	}

	vtkDataSet* acc_out;
	acc_out = accumulate.GetNewOutput();
	acc_out->Update();

	albaNEW(m_ImportedVolume);
	m_ImportedVolume->SetName("Imported Volume");
	m_ImportedVolume->SetDataByDetaching(acc_out, m_Input->GetMTime());

  m_Output = m_ImportedVolume;
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
char ** albaOpImporterImage::GetIcon()
{
#include "pic/MENU_IMPORT_IMAGE.xpm"
	return MENU_IMPORT_IMAGE_xpm;
}
