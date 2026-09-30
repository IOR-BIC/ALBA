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
#include <albaGUIBusyInfo.h>

#include "albaOpExporterMetaImage.h"

#include "albaDecl.h"
#include "albaGUI.h"
#include "vtkALBASmartPointer.h"

#include "vtkImageData.h"
#include "vtkMetaImageWriter.h"
#include "albaVMEImage.h"
#include "albaVMEVolumeGray.h"
#include "albaProgressBarHelper.h"
#include "vtkTransformFilter.h"
#include "vtkImageCast.h"
#include "albaTagArray.h"
#include "albaMatrix.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpExporterMetaImage);

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

	m_FileDir = albaGetLastUserFolder();
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

	//Setting Metadata Tags
	albaTagArray * tagArray = m_Input->GetTagArray();
	std::vector<std::string> tagNames;
	tagArray->GetTagList(tagNames);

	for (size_t i = 0; i < tagNames.size(); i++)
	{
		albaTagItem *tag = tagArray->GetTag(tagNames[i].c_str());
		const std::string tagValue = tag->GetValue();
		// VTK's vtkMetaImageWriter stores metadata in the MHA header
		// Tag metadata handling would need to be done via custom header writing if needed
	}

	vtkALBASmartPointer<vtkMetaImageWriter> writer;


	//Absolute Matrix
	if (m_ABSMatrixFlag)
	{
		writer->SetInputConnection(m_Input->GetOutput()->GetVTKOutputPort());
	}
	else
	{
		vtkImageData *inputData = vtkImageData::SafeDownCast(m_Input->GetOutput()->GetVTKData());
		//TODO apply transform matrix and set input to writer
	}
	writer->SetFileName(m_File.GetCStr());
	writer->SetCompression(m_Compression != 0);
	writer->Write();
}

//----------------------------------------------------------------------------
char ** albaOpExporterMetaImage::GetIcon()
{
#include "pic/MENU_IMPORT_VTK.xpm"
	return MENU_IMPORT_VTK_xpm;
}