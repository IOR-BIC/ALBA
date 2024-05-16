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
#include "albaVMEImage.h"
#include "albaVMEItem.h"

#include "vtkALBASmartPointer.h"
#include "vtkImageData.h"
#include "vtkBMPReader.h"
#include "vtkJPEGReader.h"
#include "vtkPNGReader.h"
#include "vtkTIFFReader.h"
#include "vtkImageExtractComponents.h"

#include <algorithm>
#include "albaProgressBarHelper.h"
#include "wx/filename.h"
#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkDataSetAttributes.h"
#include "vtkUnsignedCharArray.h"

//----------------------------------------------------------------------------
// Global Function (locale to this file) to sort the filenames
//----------------------------------------------------------------------------
bool CompareNumber(std::string first, std::string second)
{
	wxString first_path, first_name, first_ext;
	wxString second_path, second_name, second_ext;
	long first_num, second_num;

	wxFileName::SplitPath(first.c_str(),&first_path,&first_name,&first_ext);
	wxFileName::SplitPath(second.c_str(),&second_path,&second_name,&second_ext);

	first_name.ToLong(&first_num);
	second_name.ToLong(&second_num);

  return (first_num - second_num) < 0;   // compare number

}

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterImage);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpImporterImage::albaOpImporterImage(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_Files.clear();
  m_NumFiles = 0;
  m_BuildVolumeFlag = 0;

  m_FilePrefix    = "";
  m_FilePattern   = "%s%04d";
  m_FileExtension = "";
  m_FileOffset    = 0;
  m_FileSpacing = 1;
  m_ImageZSpacing = 1.0;

  m_ImportedImage = NULL;
  m_ImportedImageAsVolume = NULL;

  m_FileDirectory = albaGetLastUserFolder();
}
//----------------------------------------------------------------------------
albaOpImporterImage::~albaOpImporterImage()
//----------------------------------------------------------------------------
{
  albaDEL(m_ImportedImage);
  albaDEL(m_ImportedImageAsVolume);
}
//----------------------------------------------------------------------------
// constant ID
//----------------------------------------------------------------------------
enum IMAGE_IMPORTER_ID
{
  ID_BUILD_VOLUME = MINID,
  ID_STRING_PREFIX,
  ID_STRING_PATTERN,
  ID_STRING_EXT,
  ID_OFFSET,
  ID_SPACING,
  ID_DATA_SPACING,
};
//----------------------------------------------------------------------------
void albaOpImporterImage::OpRun()   
//----------------------------------------------------------------------------
{
	albaString wildc = "Images (*.bmp;*.jpg;*.png;*.tif)| *.bmp;*.jpg;*.png;*.tif";
	
  if (!m_TestMode)
  {
    m_Files.clear();
	  m_Gui = new albaGUI(this);

		std::vector<wxString> files;
			
		albaGetOpenMultiFiles((const char *)m_FileDirectory.ToAscii(), wildc.GetCStr(), files);

		for (int i = 0; i < files.size(); i++)
			m_Files.push_back(files[i].ToAscii());
  }

  m_NumFiles = m_Files.size();

  if(m_NumFiles == 0)
  {
    OpStop(OP_RUN_CANCEL);
  }
  else
  {
    wxFileName::SplitPath(m_Files[0].c_str(),&m_FileDirectory,&m_FilePrefix,&m_FileExtension);
    
    if (!m_TestMode)
    {
      m_Gui->Bool(ID_BUILD_VOLUME,"Volume",&m_BuildVolumeFlag,0,"Check to build volume, otherwise a sequence of image is generated!");
	    m_Gui->String(ID_STRING_PREFIX,"file pref.", &m_FilePrefix);
	    m_Gui->String(ID_STRING_PATTERN,"file patt.", &m_FilePattern);
	    m_Gui->String(ID_STRING_EXT,"file ext.", &m_FileExtension);
	    m_Gui->Integer(ID_OFFSET,"file offset:",&m_FileOffset,0, MAXINT,"set the first slice number in the files name");
	    m_Gui->Integer(ID_SPACING,"file spc.:",&m_FileSpacing,1, MAXINT, "set the spacing between the slices in the files name");
	    m_Gui->Double(ID_DATA_SPACING,"data spc.:",&m_ImageZSpacing,1);
      m_Gui->OkCancel();

      m_Gui->Enable(ID_STRING_PREFIX,false);
      m_Gui->Enable(ID_STRING_PATTERN,false);
      m_Gui->Enable(ID_STRING_EXT,false);
      m_Gui->Enable(ID_OFFSET,false);
      m_Gui->Enable(ID_SPACING,false);
      m_Gui->Enable(ID_DATA_SPACING,false);
      m_Gui->Update();

			m_Gui->Divider();

      ShowGui();
    }

    if(m_NumFiles == 1)
    {
      ImportImage();
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
          m_Gui->Enable(ID_STRING_PREFIX,m_BuildVolumeFlag != 0);
          m_Gui->Enable(ID_STRING_PATTERN,m_BuildVolumeFlag != 0);
          m_Gui->Enable(ID_STRING_EXT,m_BuildVolumeFlag != 0);
          m_Gui->Enable(ID_OFFSET,m_BuildVolumeFlag != 0);
          m_Gui->Enable(ID_SPACING,m_BuildVolumeFlag != 0);
          m_Gui->Enable(ID_DATA_SPACING,m_BuildVolumeFlag != 0);
          m_Gui->Update();
        }
      break;
      case wxOK:
        ImportImage();
        OpStop(OP_RUN_OK);
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);
      break;
    }
  }
}
//----------------------------------------------------------------------------
void albaOpImporterImage::ImportImage()
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
	long time;
  wxString path, name, ext;

	albaNEW(m_ImportedImage);
  
  wxFileName::SplitPath(m_Files[0].c_str(),&path,&name,&ext);
  if(name.IsNumber())
    std::sort(m_Files.begin(),m_Files.end(),CompareNumber);
  else
    std::sort(m_Files.begin(),m_Files.end());

	albaTimeStamp start_time = m_Input->GetRoot()->GetTimeStamp();
	
	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar();

  for(int i=0; i<m_NumFiles; i++)
	{
    progressHelper.UpdateProgressBar((i*100)/m_NumFiles);

    wxFileName::SplitPath(m_Files[i].c_str(),&path,&name,&ext);
		ext.MakeUpper();
		if(name.IsNumber())
			name.ToLong(&time);
		else
			time = i;

		if(ext == "BMP")
		{
			vtkALBASmartPointer<vtkBMPReader> r;
			r->SetFileName(m_Files[i].c_str());
			r->Update();
      m_ImportedImage->SetData(r->GetOutput(),time);
		} 
		else if (ext == "JPG" || ext == "JPEG" )
		{
			vtkALBASmartPointer<vtkJPEGReader> r;
			r->SetFileName(m_Files[i].c_str());
			r->Update();
      m_ImportedImage->SetData(r->GetOutput(),time);
		}
		else if (ext == "PNG")
		{
			vtkALBASmartPointer<vtkPNGReader> r;
			r->SetFileName(m_Files[i].c_str());
			r->Update();
      m_ImportedImage->SetData(r->GetOutput(),time);
		}
		else if (ext == "TIF" || ext == "TIFF" )
		{
			vtkALBASmartPointer<vtkTIFFReader> r;
			r->SetFileName(m_Files[i].c_str());
			r->Update();
      m_ImportedImage->SetData(r->GetOutput(),time);
		}
		else
			wxMessageBox("unable to import %s, unrecognized type",m_Files[i].c_str());
	}

  if(m_NumFiles > 1)
    m_ImportedImage->SetName("Imported Images");
  else
    m_ImportedImage->SetName(name);

  m_ImportedImage->SetTimeStamp(start_time);
  m_ImportedImage->ReparentTo(m_Input);
  m_Output = m_ImportedImage;
}
//----------------------------------------------------------------------------
void albaOpImporterImage::BuildVolume()
//----------------------------------------------------------------------------
{
  wxString prefix  = m_FileDirectory + "\\" + m_FilePrefix;
  prefix.Replace("/", "\\");
  wxString pattern = m_FilePattern  + "."  + m_FileExtension;
  int extent[6];

  albaNEW(m_ImportedImageAsVolume);
  m_ImportedImageAsVolume->SetName("Imported Volume");

  if(m_FileExtension.Upper() == "BMP")
	{
    vtkBMPReader *r = vtkBMPReader::New();
    r->SetFileName(m_Files[0].c_str());
    r->UpdateInformation();
    r->GetDataExtent(extent);
    r->Delete();
    r = vtkBMPReader::New();
    albaEventMacro(albaEvent(this,BIND_TO_PROGRESSBAR,r));
    r->SetFileDimensionality(2);
    r->SetFilePrefix(prefix);
    r->SetFilePattern(pattern);
    r->SetFileNameSliceSpacing(m_FileSpacing);
    r->SetFileNameSliceOffset(m_FileOffset);
    r->SetDataExtent(extent[0], extent[1], extent[2], extent[3], extent[4], m_NumFiles - 1);
    r->SetDataVOI(extent[0], extent[1], extent[2], extent[3], extent[4], m_NumFiles - 1);
    r->SetDataOrigin(0, 0, m_FileOffset);
    r->SetDataSpacing(1.0,1.0,m_ImageZSpacing);
    r->Update();

		vtkALBASmartPointer<vtkImageExtractComponents> extractComponents;
		extractComponents->SetInput(r->GetOutput());
		extractComponents->SetComponents(0, 1, 2); // RGB channels
		extractComponents->Update();
    
    m_ImportedImageAsVolume->SetData(extractComponents->GetOutput(),m_Input->GetTimeStamp());
    
    r->Delete();
	} 
	else if (m_FileExtension.Upper() == "JPG" || m_FileExtension.Upper() == "JPEG")
	{
		vtkJPEGReader *r = vtkJPEGReader::New();
    r->SetFileName(m_Files[0].c_str());
    r->UpdateInformation();
    r->GetDataExtent(extent);
    r->Delete();
		r = vtkJPEGReader::New();
    albaEventMacro(albaEvent(this,BIND_TO_PROGRESSBAR,r));
    r->SetFileDimensionality(2);
    r->SetFilePrefix(prefix);
    r->SetFilePattern(pattern);
    r->SetFileNameSliceSpacing(m_FileSpacing);
    r->SetFileNameSliceOffset(m_FileOffset);
    r->SetDataExtent(extent[0], extent[1], extent[2], extent[3], extent[4], m_NumFiles - 1);
    r->SetDataOrigin(0, 0, m_FileOffset);
    r->SetDataSpacing(1.0,1.0,m_ImageZSpacing);
    r->Update();
    
    m_ImportedImageAsVolume->SetData(r->GetOutput(),m_Input->GetTimeStamp());
    
    r->Delete();
	}
	else if (m_FileExtension.Upper() == "PNG")
	{
		vtkPNGReader *r = vtkPNGReader::New();
    r->SetFileName(m_Files[0].c_str());
    r->UpdateInformation();
    r->GetDataExtent(extent);
    r->Delete();

		r = vtkPNGReader::New();
    albaEventMacro(albaEvent(this,BIND_TO_PROGRESSBAR,r));
    r->SetFileDimensionality(2);
    r->SetFilePrefix(prefix);
    r->SetFilePattern(pattern);
    r->SetFileNameSliceSpacing(m_FileSpacing);
    r->SetFileNameSliceOffset(m_FileOffset);
    r->SetDataExtent(extent[0], extent[1], extent[2], extent[3], extent[4], m_NumFiles - 1);
    r->SetDataOrigin(0, 0, m_FileOffset);
    r->SetDataSpacing(1.0,1.0,m_ImageZSpacing);
    r->Update();

		vtkImageData *impOut=r->GetOutput();

		vtkImageData *outNormal;
		vtkNEW(outNormal);
		outNormal->SetDimensions(impOut->GetDimensions());
		outNormal->SetSpacing(impOut->GetSpacing());
		outNormal->SetOrigin(impOut->GetOrigin());
		outNormal->SetScalarTypeToUnsignedChar();
		vtkUnsignedCharArray* scalars;
		vtkNEW(scalars);

		vtkDataArray* impScalars=impOut->GetPointData()->GetScalars();

		int numberOfTuples = impScalars->GetSize()/ impScalars->GetNumberOfComponents();
		scalars->SetNumberOfValues(numberOfTuples);
		for (int i = 0; i < numberOfTuples; i++)
			scalars->SetTuple1(i, impScalars->GetTuple(i)[0]);
		
		outNormal->GetPointData()->SetScalars(scalars);

		m_ImportedImageAsVolume->SetData(outNormal, m_Input->GetTimeStamp());
		    
    r->Delete();
	}
	else if (m_FileExtension.Upper() == "TIF" || m_FileExtension.Upper() == "TIFF" )
	{
		vtkTIFFReader *r = vtkTIFFReader::New();
    r->SetFileName(m_Files[0].c_str());
    r->UpdateInformation();
    r->GetDataExtent(extent);
    r->Delete();
		r = vtkTIFFReader::New();
    albaEventMacro(albaEvent(this,BIND_TO_PROGRESSBAR,r));
    r->SetFileDimensionality(2);
    r->SetFilePrefix(prefix);
    r->SetFilePattern(pattern);
    r->SetFileNameSliceSpacing(m_FileSpacing);
    r->SetFileNameSliceOffset(m_FileOffset);
    r->SetDataExtent(extent[0], extent[1], extent[2], extent[3], extent[4], m_NumFiles - 1);
    r->SetDataOrigin(0, 0, m_FileOffset);
    r->SetDataSpacing(1.0,1.0,m_ImageZSpacing);
    r->Update();
    
    m_ImportedImageAsVolume->SetData(r->GetOutput(),m_Input->GetTimeStamp());

    r->Delete();
	}
	else
  {
		albaLogMessage("unable to import %s, unrecognized type", m_Files[0].c_str());
    albaDEL(m_ImportedImageAsVolume);
  }
  
  m_Output = m_ImportedImageAsVolume;
}
//----------------------------------------------------------------------------
void albaOpImporterImage::SetFileName(const char *file_name)
//----------------------------------------------------------------------------
{
 m_Files.push_back(file_name);
 m_NumFiles = m_Files.size();
}

//----------------------------------------------------------------------------
char ** albaOpImporterImage::GetIcon()
{
#include "pic/MENU_IMPORT_IMAGE.xpm"
	return MENU_IMPORT_IMAGE_xpm;
}
