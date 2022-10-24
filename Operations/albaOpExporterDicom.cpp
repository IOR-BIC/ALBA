/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterDicom
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

#include "albaOpExporterDicom.h"

#include "albaDecl.h"
#include "albaGUI.h"
#include "vtkALBASmartPointer.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMERoot.h"
#include "albaVMEVolumeGray.h"

#include "vtkDataSetWriter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkAbstractTransform.h"
#include "vtkImageCast.h"
#include "vtkImageData.h"
#include "vtkImageData.h"
#include "vtkPolyData.h"
#include "albaVMEGroup.h"
#include "albaProgressBarHelper.h"
#include "vtkUnstructuredGrid.h"
#include "vtkTransformFilter.h"
#include "vtkStructuredGrid.h"
#include "gdcmSmartPointer.h"
#include "gdcmImage.h"
#include "vtkDataArray.h"
#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkRectilinearGrid.h"
#include "vtkDoubleArray.h"
#include "albaOpImporterDicom.h"
#include "albaDicomTagDefines.h"
#include "albaGUIDicomSettings.h"

#include "gdcmOverlay.h"
#include "gdcmTag.h"
#include "gdcmImage.h"
#include "gdcmAttribute.h"
#include "gdcmImageWriter.h"
#include "gdcmFileDerivation.h"
#include "gdcmUIDGenerator.h"
#include "gdcmDirectoryHelper.h"
#include "albaOp.h"
#include "albaTagArray.h"
#include "albaVMEImage.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpExporterDicom);
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
albaOpExporterDicom::albaOpExporterDicom(const wxString &label) :
albaOp(label)
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_Folder    = "";
  m_Input   = NULL;

	m_ABSMatrixFlag = 0;

}
//----------------------------------------------------------------------------
albaOpExporterDicom::~albaOpExporterDicom()
{
}
//----------------------------------------------------------------------------
bool albaOpExporterDicom::InternalAccept(albaVME *node)
{ 
  return (node->IsALBAType(albaVMEVolumeGray)||node->IsALBAType(albaVMEImage));
}
//----------------------------------------------------------------------------
albaOp* albaOpExporterDicom::Copy()   
{
  albaOpExporterDicom *cp = new albaOpExporterDicom(m_Label);
  cp->m_Folder = m_Folder;
  return cp;
}

//----------------------------------------------------------------------------
void albaOpExporterDicom::OpRun()   
{
  vtkDataSet *inputData = m_Input->GetOutput()->GetVTKData();
  assert(inputData);

  bool isStructuredPoints = inputData->IsA("vtkImageData");

  albaString wildc = "vtk Data (*.vtk)|*.vtk";

	wxString lastDicomDir = GetSetting()->GetLastDicomDir();

	if (lastDicomDir == "UNEDFINED_m_LastDicomDir")
		lastDicomDir = albaGetLastUserFolder().c_str();

	wxDirDialog dialog(NULL, "Choose directory where to save files:", lastDicomDir, wxDD_DEFAULT_STYLE);
	dialog.SetReturnCode(wxID_OK);
	int ret_code = dialog.ShowModal();
	if (ret_code == wxID_OK)
	{
		m_Folder = dialog.GetPath();

		GetSetting()->SetLastDicomDir(dialog.GetPath());
	
		m_PatientsName = m_Input->GetTagArray()->GetTag("PatientsName") ? m_Input->GetTagArray()->GetTag("PatientsName")->GetValue() : "";
		m_PatientsSex = m_Input->GetTagArray()->GetTag("PatientsSex") ? m_Input->GetTagArray()->GetTag("PatientsSex")->GetValue() : "";
		m_PatientsBirthDate = m_Input->GetTagArray()->GetTag("PatientsBirthDate") ? m_Input->GetTagArray()->GetTag("PatientsBirthDate")->GetValue() : "";
		m_PatientsWeight = m_Input->GetTagArray()->GetTag("PatientsWeight") ? m_Input->GetTagArray()->GetTag("PatientsWeight")->GetValue() : "";
		m_PatientsAge = m_Input->GetTagArray()->GetTag("PatientsAge") ? m_Input->GetTagArray()->GetTag("PatientsAge")->GetValue() : "";
		m_PatientID = m_Input->GetTagArray()->GetTag("PatientID") ? m_Input->GetTagArray()->GetTag("PatientID")->GetValue() : "";
		m_InstitutionName = m_Input->GetTagArray()->GetTag("InstitutionName") ? m_Input->GetTagArray()->GetTag("InstitutionName")->GetValue() : "";
		m_AcquisitionDate = m_Input->GetTagArray()->GetTag("AcquisitionDate") ? m_Input->GetTagArray()->GetTag("AcquisitionDate")->GetValue() : "";
		m_ProtocolName = m_Input->GetTagArray()->GetTag("ProtocolName") ? m_Input->GetTagArray()->GetTag("ProtocolName")->GetValue() : "";
		m_ManufacturersModelName = m_Input->GetTagArray()->GetTag("ManufacturersModelName") ? m_Input->GetTagArray()->GetTag("ManufacturersModelName")->GetValue() : "";

		m_Gui = new albaGUI(this);

		m_Gui->Label("");
		m_Gui->Label("Patient:", true);
		m_Gui->String(-1, "Name:", &m_PatientsName, "", false);
		m_Gui->String(-1, "Sex:", &m_PatientsSex, "", false);
		m_Gui->String(-1, "Birth date:", &m_PatientsBirthDate, "", false);
		m_Gui->String(-1, "Weight:", &m_PatientsWeight, "", false);
		m_Gui->String(-1, "Age:", &m_PatientsAge, "", false);
		m_Gui->String(-1, "ID:", &m_PatientID, "", false);
		m_Gui->Label("");
		m_Gui->Label("Others:", true);
		m_Gui->String(-1, "Institution:", &m_InstitutionName, "", false);
		m_Gui->String(-1, "Acq. Date:", &m_AcquisitionDate, "", false);
		m_Gui->String(-1, "Protocol:", &m_ProtocolName, "", false);
		m_Gui->String(-1, "Model:", &m_ManufacturersModelName, "", false);
		
		m_Gui->Label("");

		m_Gui->OkCancel();
		m_Gui->Enable(wxOK, !m_Folder.IsEmpty());

		m_Gui->Divider();

		ShowGui();
	}
	else
	{
		OpStop(OP_RUN_CANCEL);
	}
	
}
//----------------------------------------------------------------------------
void albaOpExporterDicom::OnEvent(albaEventBase *alba_event)
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
    switch(e->GetId())
    {
      case wxOK:
        ExportDicom();
        OpStop(OP_RUN_OK);
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);
      break;
      default:
        albaEventMacro(*e);
      break;
    }
	}
}

//----------------------------------------------------------------------------
void albaOpExporterDicom::ExportDicom()
{		
	int dims[3];
	double origin[3],spacing[3];
	double sr[2];
	int scalarSpan, scalarShift;
	vtkDoubleArray *zCoord=NULL;

	albaString firstFilename = GetIthFilename(0);

	if (wxFileExists(firstFilename.GetCStr()))
	{
		int tmp = wxMessageBox("The output directory already contains this Dicom\ndo you want to override it? ", "File Exists" , wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION | wxCENTRE | wxSTAY_ON_TOP);

		//User has pressed no
		if (tmp == wxNO)
			return;
	}


	m_Input->GetOutput()->Update();

	vtkDataSet *volume =m_Input->GetOutput()->GetVTKData();
	vtkImageData *imgData = vtkImageData::SafeDownCast(volume);
	vtkRectilinearGrid *rg = vtkRectilinearGrid::SafeDownCast(volume);
	if (imgData)
	{
		imgData->GetDimensions(dims);
		imgData->GetOrigin(origin);
		imgData->GetSpacing(spacing);
	}
	else
	{
		rg->GetDimensions(dims);
		origin[0] = rg->GetXCoordinates()->GetTuple1(0);
		origin[1] = rg->GetYCoordinates()->GetTuple1(0);
		origin[2] = rg->GetZCoordinates()->GetTuple1(0);
		spacing[0] = rg->GetXCoordinates()->GetTuple1(1) - origin[0];
		spacing[1] = rg->GetYCoordinates()->GetTuple1(1) - origin[1];
		spacing[2] = rg->GetZCoordinates()->GetTuple1(1) - origin[2];
		zCoord = vtkDoubleArray::SafeDownCast(rg->GetZCoordinates());
	}

	int imgDim = dims[0] * dims[1];

	gdcm::UIDGenerator uid; // helper for uid generation
	const char ReferencedSOPClassUID[] = "1.2.840.10008.5.1.4.1.1.7"; // Secondary Capture
	albaString uidstr = uid.Generate();

	//Study ID
	albaString studyID = uid.Generate();
	gdcm::DataElement studyDE(TAG_StudyInstanceUID);
	studyDE.SetByteValue(studyID.GetCStr(), studyID.Length());
	studyDE.SetVR(gdcm::Attribute ATTRIBUTE_StudyInstanceUID::GetVR());

	//Series ID
	albaString seriesID = uid.Generate();
	gdcm::DataElement seriesDE(TAG_SeriesInstanceUID);
	seriesDE.SetByteValue(seriesID.GetCStr(), seriesID.Length());
	seriesDE.SetVR(gdcm::Attribute ATTRIBUTE_SeriesInstanceUID::GetVR());
	
	albaProgressBarHelper helper(this);

	helper.InitProgressBar("Exporting...");
		
	for (int i = 0; i < dims[2]; i++)
	{
		double sliceZHeight;
		//Create dicom image;
		gdcm::SmartPointer<gdcm::Image> im = new gdcm::Image;

		im->SetNumberOfDimensions(2);
		im->SetDimension(0, dims[0]);
		im->SetDimension(1, dims[1]);

		im->GetPixelFormat().SetSamplesPerPixel(1);

		//TODO read and store this tag and add a switch here 
		im->SetPhotometricInterpretation(gdcm::PhotometricInterpretation::MONOCHROME2);

		void *start=volume->GetPointData()->GetScalars()->GetVoidPointer(0);
		volume->GetScalarRange(sr);
		scalarSpan = sr[1] - sr[0];
		int pixelSize;
		void *buffer;
		bool rescaled=false;

		switch (volume->GetPointData()->GetScalars()->GetDataType())
		{
			case VTK_CHAR:
				pixelSize = sizeof(INT8);
				buffer = (INT8 *)start + imgDim*i;
				im->GetPixelFormat().SetScalarType(gdcm::PixelFormat::ScalarType::INT8);
				break;
			case VTK_UNSIGNED_CHAR:
				pixelSize = sizeof(UINT8);
				buffer = (UINT8 *)start + imgDim*i;
				im->GetPixelFormat().SetScalarType(gdcm::PixelFormat::ScalarType::UINT8);
				break;
			case VTK_SHORT:
				pixelSize = sizeof(INT16);
				buffer = (INT16 *)start + imgDim*i;
				im->GetPixelFormat().SetScalarType(gdcm::PixelFormat::ScalarType::INT16);
				break;
			case VTK_UNSIGNED_SHORT:
				pixelSize = sizeof(UINT16);
				buffer = (UINT16 *)start + imgDim*i;
				im->GetPixelFormat().SetScalarType(gdcm::PixelFormat::ScalarType::UINT16);
				break;
			case VTK_INT:
				if (sr[0] >= -32768 && sr[1] <= 32767)
				{
					buffer = new INT16[imgDim];
					pixelSize = sizeof(INT16);
					im->GetPixelFormat().SetScalarType(gdcm::PixelFormat::ScalarType::INT16);
					rescaled = true;
					ScalarScale((int *)start + imgDim*i, (INT16 *)buffer, imgDim);
				}
				else if (scalarSpan <= 65535)
				{
					buffer = new UINT16[imgDim];
					pixelSize = sizeof(UINT16);
					im->GetPixelFormat().SetScalarType(gdcm::PixelFormat::ScalarType::UINT16);
					scalarShift = -sr[0];
					rescaled = true;
					im->SetIntercept(-scalarShift);
					ScalarScale((int *)start + imgDim*i, (unsigned short *)buffer, imgDim, scalarShift);
				}
				else
				{
					pixelSize = sizeof(INT32);
					buffer = (int *)start + imgDim*i;
					im->GetPixelFormat().SetScalarType(gdcm::PixelFormat::ScalarType::INT32);
				}
				break;
			case VTK_UNSIGNED_INT:
				if (sr[1] <= 65535)
				{
					buffer = new INT16[imgDim];
					pixelSize = sizeof(INT16);
					im->GetPixelFormat().SetScalarType(gdcm::PixelFormat::ScalarType::INT16);
					rescaled = true;
					ScalarScale((unsigned int *)start + imgDim*i, (unsigned short *)buffer, imgDim);
				}
				else if (scalarSpan <= 65535)
				{
					buffer = new UINT16[imgDim];
					pixelSize = sizeof(UINT16);
					im->GetPixelFormat().SetScalarType(gdcm::PixelFormat::ScalarType::UINT16);
					scalarShift = -sr[0];
					rescaled = true;
					im->SetIntercept(-scalarShift);
					ScalarScale((unsigned int *)start + imgDim*i, (unsigned short *)buffer, imgDim, scalarShift);
				}
				else
				{
					pixelSize = sizeof(UINT32);
					buffer = (UINT32 *)start + imgDim*i;
					im->GetPixelFormat().SetScalarType(gdcm::PixelFormat::ScalarType::UINT32);
				}
				break;
			case VTK_FLOAT:
				if (sr[0] >= -32768 && sr[1] <= 32767)
				{
					buffer = new INT16[imgDim];
					pixelSize = sizeof(INT16);
					im->GetPixelFormat().SetScalarType(gdcm::PixelFormat::ScalarType::INT16);
					rescaled = true;
					ScalarScale((float *)start + imgDim*i, (INT16 *)buffer, imgDim);
				}
				else if (scalarSpan <= 65535)
				{
					buffer = new UINT16[imgDim];
					pixelSize = sizeof(UINT16);
					im->GetPixelFormat().SetScalarType(gdcm::PixelFormat::ScalarType::UINT16);
					scalarShift = -sr[0];
					rescaled = true;
					im->SetIntercept(-scalarShift);
					ScalarScale((float *)start + imgDim*i, (UINT16 *)buffer, imgDim, scalarShift);
				}
				else if (sr[0] >= -2147483648 && sr[1] <= 2147483647)
				{
					buffer = new INT32[imgDim];
					pixelSize = sizeof(INT32);
					im->GetPixelFormat().SetScalarType(gdcm::PixelFormat::ScalarType::INT32);
					rescaled = true;
					ScalarScale((float *)start + imgDim*i, (INT32 *)buffer, imgDim);
				}
				else if (scalarSpan <= 4294967295)
				{
					buffer = new UINT32[imgDim];
					pixelSize = sizeof(UINT32);
					im->GetPixelFormat().SetScalarType(gdcm::PixelFormat::ScalarType::UINT32);
					scalarShift = -sr[0];
					rescaled = true;
					im->SetIntercept(-scalarShift);
					ScalarScale((float *)start + imgDim*i, (UINT32 *)buffer, imgDim, scalarShift);
				}
				else
				{
					albaErrorMessage("Cannot save a Dicom from this Volume");
					return;
				}
				break;
			case VTK_DOUBLE:
				if (sr[0] >= -32768 && sr[1] <= 32767)
				{
					buffer = new INT16[imgDim];
					pixelSize = sizeof(INT16);
					im->GetPixelFormat().SetScalarType(gdcm::PixelFormat::ScalarType::INT16);
					rescaled = true;
					ScalarScale((double *)start + imgDim*i, (INT16 *)buffer, imgDim);
				}
				else if (scalarSpan <= 65535)
				{
					buffer = new UINT16[imgDim];
					pixelSize = sizeof(UINT16);
					im->GetPixelFormat().SetScalarType(gdcm::PixelFormat::ScalarType::UINT16);
					scalarShift = -sr[0];
					rescaled = true;
					im->SetIntercept(-scalarShift);
					ScalarScale((double *)start + imgDim*i, (UINT16 *)buffer, imgDim, scalarShift);
				}
				else if (sr[0] >= -2147483648 && sr[1] <= 2147483647)
				{
					buffer = new INT32[imgDim];
					pixelSize = sizeof(INT32);
					im->GetPixelFormat().SetScalarType(gdcm::PixelFormat::ScalarType::INT32);
					rescaled = true;
					ScalarScale((double *)start + imgDim*i, (INT32 *)buffer, imgDim);
				}
				else if (scalarSpan <= 4294967295)
				{
					buffer = new UINT32[imgDim];
					pixelSize = sizeof(UINT32);
					im->GetPixelFormat().SetScalarType(gdcm::PixelFormat::ScalarType::UINT32);
					scalarShift = -sr[0];
					rescaled = true;
					im->SetIntercept(-scalarShift);
					ScalarScale((double *)start + imgDim*i, (UINT32 *)buffer, imgDim, scalarShift);
				}
				else
				{
					albaErrorMessage("Cannot save a Dicom from this Volume");
					return;
				}
				break;
			default:
				{
					albaErrorMessage("Cannot save a Dicom from this Volume");
					return;
				}
		}

		long size = imgDim * pixelSize;

		gdcm::DataElement pixeldata(gdcm::Tag(0x7fe0, 0x0010));
		pixeldata.SetByteValue((const char*)buffer, (uint32_t)size);
		im->SetDataElement(pixeldata);
	
		gdcm::SmartPointer<gdcm::File> file = new gdcm::File; // empty file

																													// Step 2: DERIVED object
		gdcm::FileDerivation fd;
		fd.AddReference(ReferencedSOPClassUID, uidstr);


		// Again for the purpose of the exercise we will pretend that the image is a
		// multiplanar reformat (MPR):
		// CID 7202 Source Image Purposes of Reference
		// {"DCM",121322,"Source image for image processing operation"},
		fd.SetPurposeOfReferenceCodeSequenceCodeValue(121322);
		// CID 7203 Image Derivation
		// { "DCM",113072,"Multiplanar reformatting" },
		fd.SetDerivationCodeSequenceCodeValue(113072);
		fd.SetFile(*file);
		// If all Code Value are ok the filter will execute properly
		if (!fd.Derive())
		{
			std::cerr << "Sorry could not derive using input info" << std::endl;
			break;
		}


		//Set origin
		sliceZHeight = zCoord ? zCoord->GetTuple1(i) : origin[2] + spacing[2] * i;
		gdcm::Attribute ATTRIBUTE_ImagePositionPatient imgPosAttr;
		imgPosAttr.SetValue(origin[0], 0);
		imgPosAttr.SetValue(origin[1], 1);
		imgPosAttr.SetValue(sliceZHeight, 2);

		//Set Spacing
		gdcm::Attribute ATTRIBUTE_PixelSpacing pixelSpacingAttr;
		pixelSpacingAttr.SetValue(spacing[0], 0);
		pixelSpacingAttr.SetValue(spacing[1], 1);

		//Set Orientation
		double cosines[6] = { 1.0,0.0,0.0,0.0,1.0,0.0 };
		gdcm::Attribute ATTRIBUTE_ImageOrientationPatient orientPatientAttr;
		for (int c = 0; c < 6; c++)
			orientPatientAttr.SetValue(cosines[c], c);
		
		//Series Num:
		gdcm::Attribute ATTRIBUTE_SeriesNumber seriesNumAttr;
		seriesNumAttr.SetValue(i + 1);
		
		//Instance Num:
		gdcm::Attribute ATTRIBUTE_InstanceNumber instanceNumAttr;
		instanceNumAttr.SetValue(i + 2);

		
		// We pass both :
		// 1. the fake generated image
		// 2. the 'DERIVED' dataset object
		// to the writer.
		gdcm::ImageWriter w;
		w.SetImage(*im);
		w.SetFile(fd.GetFile());
		gdcm::DataSet & dcmDataSet = w.GetFile().GetDataSet();
		dcmDataSet.Replace(imgPosAttr.GetAsDataElement());
		dcmDataSet.Replace(pixelSpacingAttr.GetAsDataElement());
		dcmDataSet.Replace(orientPatientAttr.GetAsDataElement());
		dcmDataSet.Replace(seriesNumAttr.GetAsDataElement());
		dcmDataSet.Replace(instanceNumAttr.GetAsDataElement());
		dcmDataSet.Replace(studyDE);
		dcmDataSet.Replace(seriesDE);

		DEFINE_TAG(PatientsName);
		DEFINE_TAG(PatientsSex);
		DEFINE_TAG(PatientsBirthDate);
		DEFINE_TAG(PatientsWeight);
		DEFINE_TAG(PatientsAge);
		DEFINE_TAG(PatientID);
		DEFINE_TAG(InstitutionName);
		DEFINE_TAG(StudyDescription);
		DEFINE_TAG(SeriesDescription);
		DEFINE_TAG(AcquisitionDate);
		DEFINE_TAG(ProtocolName);
		DEFINE_TAG(ManufacturersModelName);

		DefineAppSpecificTags(dcmDataSet);

		albaString filename=GetIthFilename(i);

		// Set the filename:
		w.SetFileName(filename.GetCStr());
		if (!w.Write())
			break;

		if (rescaled) 
		{
			delete[] buffer;
			rescaled = false;
		}

		helper.UpdateProgressBar((i + 1) * 100 / dims[2] );
	}
	helper.CloseProgressBar();
}

//----------------------------------------------------------------------------
void albaOpExporterDicom::DefineAppSpecificTags(gdcm::DataSet & dcmDataSet)
{
	/** 
	Example*: 
	---
	
	DEFINE_TAG(ImageComments);
	
	---
	*albaString m_ImageComment should be a class member and TAG_ImageComment a defined Dicom Tag
	*/
}

//----------------------------------------------------------------------------
albaString albaOpExporterDicom::GetIthFilename(int i)
{
	albaString filename, name;
	name = m_Input->GetName();
	name.Replace('.', '_');
	name.Replace(',', '_');
	name.Replace(':', '_');
	filename.Printf("%s/%s.%d.dcm", m_Folder.GetCStr(), name.GetCStr(), i);
	return filename;
}

//----------------------------------------------------------------------------
char ** albaOpExporterDicom::GetIcon()
{
#include "pic/MENU_IMPORT_DICOM.xpm"
	return MENU_IMPORT_DICOM_xpm;
}

//----------------------------------------------------------------------------
albaGUIDicomSettings* albaOpExporterDicom::GetSetting()
{
	return (albaGUIDicomSettings*)Superclass::GetSetting();
}

//----------------------------------------------------------------------------
template<typename InDataType, typename OutDataType>
void albaOpExporterDicom::ScalarScale(InDataType from, OutDataType to, int imgDim, int scalarShift/*=0*/)
{
	for (int i = 0; i < imgDim; i++)
		to[i] = from[i] + scalarShift;
}

