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
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaOpImporterMetaImage.h"
#include "albaEvent.h"

#include "albaVME.h"
#include "albaVMEGeneric.h"
#include "albaVMEImage.h"
//#include "albaVMEPointSet.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMEPolyline.h"
#include "albaVMESurface.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEVolumeRGB.h"
#include "albaVMEMesh.h"

#include "albaTagArray.h"
#include "vtkALBASmartPointer.h"
#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"
#include "itkMetaDataDictionary.h"
#include "itkMetaDataObject.h"
#include "albaGUIBusyInfo.h"
#include "wx/filename.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterMetaImage);


const unsigned int Dimension = 3;
typedef float InputPixelTypeFloat;
typedef itk::Image< InputPixelTypeFloat, Dimension > InputImageTypeFloat;
typedef itk::ImageToVTKImageFilter< InputImageTypeFloat > ConverteritkTOvtk;
typedef itk::MetaDataObject<std::string> DictString;

//----------------------------------------------------------------------------
albaOpImporterMetaImage::albaOpImporterMetaImage(const wxString &label) :
albaOp(label)
{
  m_OpType  = OPTYPE_IMPORTER;
	m_Canundo = true;
	m_File    = "";

  //m_VmePointSet = NULL;
  m_VmeImage    = NULL;
  m_VmeGrayVol  = NULL;

  m_FileDir = albaGetLastUserFolder();
}
//----------------------------------------------------------------------------
albaOpImporterMetaImage::~albaOpImporterMetaImage()
{
  albaDEL(m_VmeImage);
  albaDEL(m_VmeGrayVol);
	mode_t tmp;
}
//----------------------------------------------------------------------------
albaOp* albaOpImporterMetaImage::Copy()   
{
  albaOpImporterMetaImage *cp = new albaOpImporterMetaImage(m_Label);
  cp->m_File			= m_File;
  return cp;
}
//----------------------------------------------------------------------------
void albaOpImporterMetaImage::OpRun()   
{
	albaString wildc = "Meta Data Image (*.mha)|*.mha";
  albaString f;
  if (m_File.IsEmpty())
  {
    f = albaGetOpenFile(m_FileDir, wildc, _("Choose VTK file"));
    m_File = f;
  }

  int result = OP_RUN_CANCEL;
  if(!m_File.IsEmpty())
	{
    if (ImportMetaImage() == ALBA_OK)
    {
      result = OP_RUN_OK;
    }
    else
    {
      if(!this->m_TestMode)
        albaMessage(_("Unsupported file format"), _("I/O Error"), wxICON_ERROR );
    }
	}

	OpStop(result);
}
//----------------------------------------------------------------------------
int albaOpImporterMetaImage::ImportMetaImage()
{
	bool success = false;
	albaGUIBusyInfo wait(_("Loading file..."),m_TestMode);


	using ReaderType = itk::ImageFileReader<InputImageTypeFloat>;
	ReaderType::Pointer  reader = ReaderType::New();

	reader->SetFileName(m_File.ToAscii());
	reader->Update();
	InputImageTypeFloat *itkImage=reader->GetOutput();
	

	if (itkImage != NULL)
	{
		wxString path, name, ext;
		wxFileName::SplitPath(m_File.ToAscii(), &path, &name, &ext);

		ConverteritkTOvtk::Pointer itkTOvtk = ConverteritkTOvtk::New();
		itkTOvtk->SetInput(itkImage);
		itkTOvtk->Update();


		vtkDataSet *data = itkTOvtk->GetOutput();


		if (data)
		{
			albaNEW(m_VmeImage);
			albaNEW(m_VmeGrayVol);

			vtkALBASmartPointer<vtkImageData> imOut;
			imOut->DeepCopy(data);


			if (m_VmeImage->SetDataByDetaching(imOut, 0) == ALBA_OK)
			{
				m_Output = m_VmeImage;
			}
			else if (m_VmeGrayVol->SetDataByDetaching(imOut, 0) == ALBA_OK)
			{
				m_Output = m_VmeGrayVol;
			}
	

			albaMatrix orientationMatrix;
			const InputImageTypeFloat::DirectionType & direction = itkImage->GetDirection();


			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
				{
					orientationMatrix[i][j] = direction[i][j];
				}

			m_Output->SetAbsMatrix(orientationMatrix);


			//Setting Metadata Tags
			const itk::MetaDataDictionary  metaDataDictionary = itkImage->GetMetaDataDictionary();
			std::vector<std::string> keys = metaDataDictionary.GetKeys();
			itk::MetaDataDictionary::ConstIterator dicIter = metaDataDictionary.Begin();
			itk::MetaDataDictionary::ConstIterator dicEnd = metaDataDictionary.End();

			while (dicIter != dicEnd)
			{
				itk::MetaDataObjectBase::Pointer entry = dicIter->second;

				DictString::ConstPointer entryvalue = dynamic_cast<const DictString *>(entry.GetPointer());

				if (entryvalue)
				{
					std::string tagkey = dicIter->first;
					std::string tagvalue = entryvalue->GetMetaDataObjectValue();
					m_Output->GetTagArray()->SetTag(tagkey.c_str(), tagvalue.c_str());
				}

				++dicIter;
			}

			albaTagItem tag_Nature;
			tag_Nature.SetName("VME_NATURE");
			tag_Nature.SetValue("NATURAL");
			m_Output->GetTagArray()->SetTag(tag_Nature);
			m_Output->ReparentTo(m_Input);
			m_Output->SetName(name.ToAscii());

			success = true;
		}
	}

	if (!success && !this->m_TestMode)
	{
		albaMessage(_("Error reading mha/mhd file."), _("I/O Error"), wxICON_ERROR);
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
char ** albaOpImporterMetaImage::GetIcon()
{
#include "pic/MENU_IMPORT_VTK.xpm"
	return MENU_IMPORT_VTK_xpm;
}
