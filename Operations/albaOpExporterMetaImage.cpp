/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterMetaImage
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
#include <wx/busyinfo.h>

#include "albaOpExporterMetaImage.h"

#include "albaDecl.h"
#include "albaGUI.h"
#include "vtkALBASmartPointer.h"

#include "vtkImageData.h"
#include "albaVMEImage.h"
#include "albaVMEVolumeGray.h"
#include "albaProgressBarHelper.h"
#include "vtkTransformFilter.h"
#include "vtkImageCast.h"


#include "itkImageFileWriter.h"-
#include "itkVTKImageToImageFilter.h"
#include "itkMetaDataDictionary.h"
#include "itkMetaDataObject.h"
#include "itkProcessObject.h"
#include "albaTagArray.h"
//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpExporterMetaImage);

const unsigned int Dimension = 3;
typedef float InputPixelTypeFloat;
typedef itk::Image< InputPixelTypeFloat, Dimension > InputImageTypeFloat;
typedef itk::VTKImageToImageFilter< InputImageTypeFloat > ConvertervtkTOitk;
typedef itk::ImageFileWriter<InputImageTypeFloat> WriterType;
typedef itk::MetaDataObject<std::string> DictString;


//----------------------------------------------------------------------------
albaOpExporterMetaImage::albaOpExporterMetaImage(const wxString &label) :albaOp(label)
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_File    = "";
  m_Input   = NULL;

	m_Compression   = 1;
	m_ABSMatrixFlag = 1;
}
//----------------------------------------------------------------------------
albaOpExporterMetaImage::~albaOpExporterMetaImage()
{
}
//----------------------------------------------------------------------------
bool albaOpExporterMetaImage::InternalAccept(albaVME *node)
{ 
  return ((node->IsALBAType(albaVMEVolumeGray) && vtkImageData::SafeDownCast(node->GetOutput()->GetVTKData())) || node->IsALBAType(albaVMEImage));
}
albaOp* albaOpExporterMetaImage::Copy()   
{
  albaOpExporterMetaImage *cp = new albaOpExporterMetaImage(m_Label);
  cp->m_File = m_File;
  return cp;
}
//----------------------------------------------------------------------------
// constants
enum VTK_EXPORTER_ID
{
  ID_VTK_BINARY_FILE = MINID,
	ID_ABS_MATRIX,
  ID_CHOOSE_FILENAME,
};
//----------------------------------------------------------------------------
void albaOpExporterMetaImage::OpRun()
{
	vtkDataSet *inputData = m_Input->GetOutput()->GetVTKData();
	assert(inputData);

	albaString wildc = "Meta Data Image (*.mha)|*.mha";

	m_FileDir = albaGetLastUserFolder().c_str();
	m_File = m_FileDir + "\\" + m_Input->GetName() + ".mha";

	m_Gui = new albaGUI(this);
	m_Gui->FileSave(ID_CHOOSE_FILENAME, _("Mha file"), &m_File, wildc);
	m_Gui->Bool(ID_VTK_BINARY_FILE, "Use Compression", &m_Compression, 1);
	m_Gui->Bool(ID_ABS_MATRIX, "Apply Absolute matrix", &m_ABSMatrixFlag, 1);
	
	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	m_Gui->Enable(wxOK, !m_File.IsEmpty());

	ShowGui();
}
//----------------------------------------------------------------------------
void albaOpExporterMetaImage::OnEvent(albaEventBase *alba_event)
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
    switch(e->GetId())
    {
      case wxOK:
        ExportMetaImage();
        OpStop(OP_RUN_OK);
      break;
      case ID_CHOOSE_FILENAME:
        m_Gui->Enable(wxOK, !m_File.IsEmpty());
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);
      break;
			case ID_ABS_MATRIX:
				break;
      default:
        albaEventMacro(*e);
      break;
    }
	}
}


//----------------------------------------------------------------------------
void albaOpExporterMetaImage::ExportMetaImage()
{
	m_Input->GetOutput()->Update();

  vtkImageData *inputData = vtkImageData::SafeDownCast(m_Input->GetOutput()->GetVTKData());
  assert(inputData);

	//Generate image output
	vtkALBASmartPointer<vtkImageCast> vtkImageToFloat;
	vtkImageToFloat->SetOutputScalarTypeToFloat();
	vtkImageToFloat->SetInput(inputData);
	vtkImageToFloat->Modified();
	vtkImageToFloat->Update();

	ConvertervtkTOitk::Pointer vtkTOitk = ConvertervtkTOitk::New();
	vtkTOitk->SetInput(vtkImageToFloat->GetOutput());
	vtkTOitk->Update();

	InputImageTypeFloat *itkImage = (InputImageTypeFloat *)vtkTOitk->GetOutput();

	//Setting Metadata Tags
	albaTagArray * tagArray = m_Input->GetTagArray();
	std::vector<std::string> tagNames;
	tagArray->GetTagList(tagNames);

	for (int i = 0; i < tagNames.size();i++)
	{
		albaTagItem *tag = tagArray->GetTag(tagNames[i].c_str());
		const std::string tagValue = tag->GetValue();
		itk::EncapsulateMetaData<std::string>(itkImage->GetMetaDataDictionary(), (const char *)tagNames[i].c_str(), tagValue);
	}


	//Absolute Matrix
  if (m_ABSMatrixFlag)
  {

		//orientation 
		InputImageTypeFloat::DirectionType newDirection;
		albaMatrix rot,invRot;
		albaMatrix * absMatrix = m_Input->GetOutput()->GetAbsMatrix();
		rot.CopyRotation(*absMatrix);

		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				newDirection[i][j] = rot[i][j];

		itkImage->SetDirection(newDirection);

		//current origin is the actual image origin we can calculate it by multiplying with current abs matrix 
		double imageOrigin[4], currentOrigin[4], newOrigin[4];
		imageOrigin[0] = itkImage->GetOrigin()[0];
		imageOrigin[1] = itkImage->GetOrigin()[1];
		imageOrigin[2] = itkImage->GetOrigin()[2];
		imageOrigin[3] = 1;
		absMatrix->MultiplyPoint(imageOrigin, currentOrigin);

		//The output must contain an origin witch should be equal to current origin when multiplied to the rotation matrix stored in the image
		//so we multiplity the current origin with the inverse of the rotation matrix
		invRot.DeepCopy(&rot);
		invRot.Invert();
		invRot.MultiplyPoint(currentOrigin, newOrigin);


		itkImage->SetOrigin(newOrigin);
  }
  

	WriterType::Pointer  writer = WriterType::New();
	writer->SetInput(itkImage);
	writer->SetUseCompression(m_Compression);
  writer->SetFileName(m_File.GetCStr());
  writer->Write();
}

//----------------------------------------------------------------------------
char ** albaOpExporterMetaImage::GetIcon()
{
#include "pic/MENU_IMPORT_VTK.xpm"
	return MENU_IMPORT_VTK_xpm;
}