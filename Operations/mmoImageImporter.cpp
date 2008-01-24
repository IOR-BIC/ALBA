/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoImageImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2008-01-24 16:25:29 $
  Version:   $Revision: 1.13 $
  Authors:   Paolo Quadrani     
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmoImageImporter.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mmgGui.h"

#include "mafTagArray.h"
#include "mafVMEVolumeRGB.h"
#include "mafVMEImage.h"
#include "mafVMEItem.h"

#include "vtkMAFSmartPointer.h"
#include "vtkImageData.h"
#include "vtkBMPReader.h"
#include "vtkJPEGReader.h"
#include "vtkPNGReader.h"
#include "vtkTIFFReader.h"

#include <algorithm>

//----------------------------------------------------------------------------
// Global Function (locale to this file) to sort the filenames
//----------------------------------------------------------------------------
bool CompareNumber(std::string first, std::string second)
{
	wxString first_path, first_name, first_ext;
	wxString second_path, second_name, second_ext;
	long first_num, second_num;

	wxSplitPath(first.c_str(),&first_path,&first_name,&first_ext);
	wxSplitPath(second.c_str(),&second_path,&second_name,&second_ext);

	first_name.ToLong(&first_num);
	second_name.ToLong(&second_num);

  return (first_num - second_num) < 0;   // compare number
}

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoImageImporter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoImageImporter::mmoImageImporter(const wxString &label) :
mafOp(label)
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

  m_FileDirectory = "";//mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mmoImageImporter::~mmoImageImporter()
//----------------------------------------------------------------------------
{
  mafDEL(m_ImportedImage);
  mafDEL(m_ImportedImageAsVolume);
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
void mmoImageImporter::OpRun()   
//----------------------------------------------------------------------------
{
	mafString wildc = "Images (*.bmp;*.jpg;*.png;*.tif)| *.bmp;*.jpg;*.png;*.tif";
	
  if (!m_TestMode)
  {
    m_Files.clear();
	  m_Gui = new mmgGui(this);
  
    mafGetOpenMultiFiles(m_FileDirectory.c_str(),wildc.GetCStr(),m_Files);
  }

  m_NumFiles = m_Files.size();

  if(m_NumFiles == 0)
  {
    OpStop(OP_RUN_CANCEL);
  }
  else
  {
    wxSplitPath(m_Files[0].c_str(),&m_FileDirectory,&m_FilePrefix,&m_FileExtension);
    
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
mafOp* mmoImageImporter::Copy()   
//----------------------------------------------------------------------------
{
  return (new mmoImageImporter(m_Label));
}
//----------------------------------------------------------------------------
void mmoImageImporter::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mmoImageImporter::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
void mmoImageImporter::ImportImage()
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

  mafTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("NATURAL");

  m_Output->GetTagArray()->SetTag(tag_Nature);
}
//----------------------------------------------------------------------------
void mmoImageImporter::BuildImageSequence()
//----------------------------------------------------------------------------
{
	long time;
  wxString path, name, ext;

	mafNEW(m_ImportedImage);
  
  wxSplitPath(m_Files[0].c_str(),&path,&name,&ext);
  if(name.IsNumber())
    std::sort(m_Files.begin(),m_Files.end(),CompareNumber);
  else
    std::sort(m_Files.begin(),m_Files.end());

	mafTimeStamp start_time = ((mafVME *)m_Input->GetRoot())->GetTimeStamp();
  long progress_value = 0;
  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
  for(int i=0; i<m_NumFiles; i++)
	{
    if (mafFloatEquals(fmod(i,10.0f),0.0f))
    {
      progress_value = (i*100)/m_NumFiles;
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress_value));
    }

    wxSplitPath(m_Files[i].c_str(),&path,&name,&ext);
		ext.MakeUpper();
		if(name.IsNumber())
			name.ToLong(&time);
		else
			time = i;

		if(ext == "BMP")
		{
			vtkMAFSmartPointer<vtkBMPReader> r;
			r->SetFileName(m_Files[i].c_str());
			r->Update();
      m_ImportedImage->SetData(r->GetOutput(),time);
		} 
		else if (ext == "JPG" || ext == "JPEG" )
		{
			vtkMAFSmartPointer<vtkJPEGReader> r;
			r->SetFileName(m_Files[i].c_str());
			r->Update();
      m_ImportedImage->SetData(r->GetOutput(),time);
		}
		else if (ext == "PNG")
		{
			vtkMAFSmartPointer<vtkPNGReader> r;
			r->SetFileName(m_Files[i].c_str());
			r->Update();
      m_ImportedImage->SetData(r->GetOutput(),time);
		}
		else if (ext == "TIF" || ext == "TIFF" )
		{
			vtkMAFSmartPointer<vtkTIFFReader> r;
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

  mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));

  m_ImportedImage->SetTimeStamp(start_time);
  m_Output = m_ImportedImage;
}
//----------------------------------------------------------------------------
void mmoImageImporter::BuildVolume()
//----------------------------------------------------------------------------
{
  wxString prefix  = m_FileDirectory + "\\" + m_FilePrefix;
  prefix.Replace("/", "\\");
  wxString pattern = m_FilePattern  + "."  + m_FileExtension;
  int extent[6];

  mafNEW(m_ImportedImageAsVolume);
  m_ImportedImageAsVolume->SetName("Imported Volume");

  if(m_FileExtension.Upper() == "BMP")
	{
    vtkBMPReader *r = vtkBMPReader::New();
    r->SetFileName(m_Files[0].c_str());
    r->UpdateInformation();
    r->GetDataExtent(extent);
    r->Delete();
    r = vtkBMPReader::New();
    mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,r));
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
    
    m_ImportedImageAsVolume->SetData(r->GetOutput(),((mafVME *)m_Input)->GetTimeStamp());
    
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
    mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,r));
    r->SetFileDimensionality(2);
    r->SetFilePrefix(prefix);
    r->SetFilePattern(pattern);
    r->SetFileNameSliceSpacing(m_FileSpacing);
    r->SetFileNameSliceOffset(m_FileOffset);
    r->SetDataExtent(extent[0], extent[1], extent[2], extent[3], extent[4], m_NumFiles - 1);
    r->SetDataOrigin(0, 0, m_FileOffset);
    r->SetDataSpacing(1.0,1.0,m_ImageZSpacing);
    r->Update();
    
    m_ImportedImageAsVolume->SetData(r->GetOutput(),((mafVME *)m_Input)->GetTimeStamp());
    
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
    mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,r));
    r->SetFileDimensionality(2);
    r->SetFilePrefix(prefix);
    r->SetFilePattern(pattern);
    r->SetFileNameSliceSpacing(m_FileSpacing);
    r->SetFileNameSliceOffset(m_FileOffset);
    r->SetDataExtent(extent[0], extent[1], extent[2], extent[3], extent[4], m_NumFiles - 1);
    r->SetDataOrigin(0, 0, m_FileOffset);
    r->SetDataSpacing(1.0,1.0,m_ImageZSpacing);
    r->Update();

    m_ImportedImageAsVolume->SetData(r->GetOutput(),((mafVME *)m_Input)->GetTimeStamp());
    
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
    mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,r));
    r->SetFileDimensionality(2);
    r->SetFilePrefix(prefix);
    r->SetFilePattern(pattern);
    r->SetFileNameSliceSpacing(m_FileSpacing);
    r->SetFileNameSliceOffset(m_FileOffset);
    r->SetDataExtent(extent[0], extent[1], extent[2], extent[3], extent[4], m_NumFiles - 1);
    r->SetDataOrigin(0, 0, m_FileOffset);
    r->SetDataSpacing(1.0,1.0,m_ImageZSpacing);
    r->Update();
    
    m_ImportedImageAsVolume->SetData(r->GetOutput(),((mafVME *)m_Input)->GetTimeStamp());

    r->Delete();
	}
	else
  {
		mafLogMessage("unable to import %s, unrecognized type", m_Files[0].c_str());
    mafDEL(m_ImportedImageAsVolume);
  }
  
  m_Output = m_ImportedImageAsVolume;
}
//----------------------------------------------------------------------------
void mmoImageImporter::SetFileName(const char *file_name)
//----------------------------------------------------------------------------
{
 m_Files.push_back(file_name);
 m_NumFiles = m_Files.size();
}