/*=========================================================================

Program: ALBA
Module: albaOpImporterDicom
Authors: Matteo Giacomoni, Roberto Mucci , Stefano Perticoni, Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

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

#include <algorithm>

#include "albaGUIBusyInfo.h"
#include "wx/listctrl.h"
#include "wx/dir.h"

#include "albaOpImporterDicom.h"
#include "albaGUIWizardPageNew.h"
#include "albaGUIValidator.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaInteractorDICOMImporter.h"
#include "albaTagArray.h"
#include "albaVMERoot.h"
#include "albaVME.h"
#include "albaVMEImage.h"
#include "albaVMEVolumeGray.h"
#include "albaGUICheckListBox.h"
#include "albaGUIDicomSettings.h"
#include "albaGUISettingsAdvanced.h"
#include "albaVMEItemVTK.h"
#include "albaDataVector.h"
#include "albaSmartPointer.h"
#include "albaVMEGroup.h"
#include "mmaMaterial.h"
#include "albaGUILutPreset.h"
#include "vtkImageReslice.h"
#include "albaProgressBarHelper.h"
#include "vtkALBASmartPointer.h"
#include "vtkPlaneSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkTexture.h"
#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkProperty.h"
#include "vtkOutlineFilter.h"
#include "vtkImageData.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkRenderer.h"
#include "albaOpImporterDicomSliceAccHelper.h"
#include "vtkImageData.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"
#include "vtkRectilinearGrid.h"
#include "vtkUnsignedShortArray.h"
#include "vtkPointData.h"

#include "gdcmDirectoryHelper.h"
#include "gdcmAttribute.h"
#include "gdcmDataElement.h"
#include "gdcmReader.h"
#include "gdcmImageReader.h"
#include "gdcmRescaler.h"


#include "albaDicomTagDefines.h"
#include "vtkALBAVolumeResample.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterDicom);

//----------------------------------------------------------------------------
// constants :

#define EPSILON 1e-7

//----------------------------------------------------------------------------
albaOpImporterDicom::albaOpImporterDicom(wxString label, bool justOnce) :
albaOp(label)
{
	m_OpType = OPTYPE_IMPORTER;
	m_Canundo = true;
	//Gui stuff
	m_Wizard = NULL;
	m_LoadPage = NULL;
	m_CropPage = NULL;
	m_CropGuiLeft = NULL;
	m_LoadGuiLeft = NULL;
	m_LoadGuiUnderLeft = NULL;
	m_CropGuiCenter = NULL;
	m_LoadGuiCenter = NULL;
	m_LoadGuiUnderCenter = NULL;
	m_SliceScannerCropPage = NULL;
	m_SliceScannerLoadPage = NULL;
	m_StudyListbox = NULL;
	m_SeriesListbox = NULL;
	//vtk stuff
	m_SliceLookupTable = NULL;
	m_SlicePlane = NULL;
	m_SliceMapper = NULL;
	m_SliceTexture = NULL;
	m_SliceActor = NULL;
	m_TextActor=NULL;
	m_TextMapper=NULL;
	m_CurrentSliceID = NULL;
	//other pointers
	m_DicomInteractor = NULL;
	m_TagArray = NULL;
	m_StudyList = NULL;
	m_SelectedSeries = NULL;
	//variables
	m_OutputType = TYPE_VOLUME;
	m_ConstantRotation = true;
	m_JustOnceImport = justOnce;
	m_DescrInName = m_SizeInName = m_PatientNameInName = true;
	m_VMEName = "";
	m_CurrentSlice = VTK_INT_MAX;
	m_SliceSubRange[0] = VTK_FLOAT_MIN;
	m_SliceSubRange[0] = VTK_FLOAT_MAX;
	m_CurrentImageID = 0;
	m_CurrentTime = 0;
	m_ShowOrientationPosition = 0;
	m_SelectedStudy = -1;
	m_ZCropBounds[0] = 0;
	m_ZCropBounds[1] = 0;
	for (int i = 0; i < 6; i++)
		m_SliceBounds[i] = 0;
}
//----------------------------------------------------------------------------
albaGUIDicomSettings* albaOpImporterDicom::GetSetting()
{
	return (albaGUIDicomSettings*) Superclass::GetSetting();
}
//----------------------------------------------------------------------------
albaOpImporterDicom::~albaOpImporterDicom()
{
	vtkDEL(m_SliceActor);
	albaDEL(m_Output);
}



//----------------------------------------------------------------------------
void albaOpImporterDicom::InsertAppSpecificTagsToReadList(std::set<gdcm::Tag> &TagsToRead)
{
	/**
	Example*:
	---
	
	TagsToRead.insert(TAG_ManufacturersModelName);
	
	---
	*This add the ManufacturersModelName to the list of the "to read" tags
	*/
}

//----------------------------------------------------------------------------
void albaOpImporterDicom::ReadAndSetAppSpecificTags(albaTagArray * m_TagArray, gdcm::DataSet & dcmDataSet)
{
	/**
	Example*:
	---

	READ_AND_SET_TAGARRAY(TAG_ManufacturersModelName, "ManufacturersModelName");

	---
	*This code will search the ManufacturersModelName and if exist will add an alba tag in the VME output
	the tag should be inserted InsertAppSpecificTagsToReadList method
	*/
}

//----------------------------------------------------------------------------
albaOp *albaOpImporterDicom::Copy()
{
	albaOpImporterDicom *importer = new albaOpImporterDicom(m_Label, m_JustOnceImport);
	return importer;
}
//----------------------------------------------------------------------------
bool albaOpImporterDicom::InternalAccept(albaVME*node)
{
	if (m_JustOnceImport && node)
	{
		albaVMERoot *root = (albaVMERoot *)node->GetRoot();
		return FindVolumeInTree(root) == NULL;
	}

	return true;
}
//-------------------------------------------------------------------------
albaVME* albaOpImporterDicom::FindVolumeInTree(albaVME *node)
{
	wxString typeName = "albaVMEVolumeGray";

	if (node->GetTypeName() == typeName)
		return node;

	std::vector<albaAutoPointer<albaVME>> children;
	children = *(node->GetChildren());

	for (albaID i = 0; i < children.size(); i++)
	{
		if (albaVME *n = FindVolumeInTree(children[i]))
			return n;
	}

	return NULL;
}
//----------------------------------------------------------------------------
void albaOpImporterDicom::OpRun()
{
	m_Wizard = new albaGUIWizard(_("DICOM Importer"));
	m_Wizard->SetListener(this);


	//TODO RESTORE SET BUTTON STRING OPTION

		
	wxString lastDicomDir;
	if(GetSetting())
		lastDicomDir = GetSetting()->GetLastDicomDir();
		
	if (lastDicomDir == "UNEDFINED_m_LastDicomDir" || lastDicomDir.empty())
		lastDicomDir = albaGetLastUserFolder();		
			
	wxDirDialog dialog(m_Wizard->GetParent(),"", lastDicomDir, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER, m_Wizard->GetPosition());
	dialog.SetReturnCode(wxID_OK);
	int ret_code = dialog.ShowModal();

	if (ret_code == wxID_OK)
	{
		CreateSliceVTKPipeline(); 
		
		CreateLoadPage();


		if (GetSetting() && !GetSetting()->GetSkipCrop())
		{
			CreateCropPage();
			m_LoadPage->SetNextPage(m_CropPage);
		}
		m_Wizard->SetFirstPage(m_LoadPage);

		wxString path = dialog.GetPath();
		GetSetting()->SetLastDicomDir(path);
		GuiUpdate();

		wxWindow* NextButton = m_Wizard->FindWindowById(wxID_FORWARD);

		wxString tmp=NextButton->GetLabel();
		if (!GetSetting()->GetSkipCrop())
			NextButton->SetLabel("&Crop >");
		else
			NextButton->SetLabel("Finish");

		if (OpenDir(path))
		{
			int wizardResult = RunWizard();
			OpStop(wizardResult);
		}
		else
			OpStop(OP_RUN_CANCEL); 
	}
	else
		OpStop(OP_RUN_CANCEL);
}
//----------------------------------------------------------------------------
int albaOpImporterDicom::RunWizard()
{
	if(m_Wizard->Run())
	{
		//Prepare data structures for output VME generation
		CalculateCropExtent();
		ImportDicomTags();
		SetVMEName();

		//if only 1 slice create a VMEImage
		if (m_ZCropBounds[1] + 1 - m_ZCropBounds[0] == 1)
			m_OutputType = TYPE_IMAGE;

		int result;
		switch (m_OutputType)
		{
			case TYPE_VOLUME:
					result = BuildVMEVolumeGrayOutput();
			break;
			case TYPE_IMAGE:
					result = BuildVMEImagesOutput();
			break;
		}
		return result;
	}
	else
		return OP_RUN_CANCEL;
}

//----------------------------------------------------------------------------
void albaOpImporterDicom::OpDo()
{
	if(m_Output != NULL)
	{
		m_Output->ReparentTo(m_Input);
	}
}
//----------------------------------------------------------------------------
void albaOpImporterDicom::OpStop(int result)
{
	if (m_DicomInteractor)
		m_Mouse->RemoveObserver(m_DicomInteractor);

	if (m_LoadPage)
	{
		m_LoadPage->GetRWI()->m_RenFront->RemoveActor(m_SliceActor);
		m_LoadPage->GetRWI()->m_RenFront->RemoveActor(m_TextActor);
	}

	vtkDEL(m_SliceTexture);
	vtkDEL(m_SliceLookupTable);
	vtkDEL(m_SlicePlane);
	vtkDEL(m_SliceMapper);
	vtkDEL(m_SliceActor);
	vtkDEL(m_TextMapper);
	vtkDEL(m_TextActor);

	albaDEL(m_TagArray);
	albaDEL(m_DicomInteractor);
	cppDEL(m_StudyList);

	if (!this->m_TestMode)
	{
		cppDEL(m_LoadGuiLeft);
		cppDEL(m_LoadGuiUnderLeft);
		cppDEL(m_LoadGuiUnderCenter);
		cppDEL(m_CropGuiLeft);
		cppDEL(m_CropGuiCenter);
		cppDEL(m_LoadPage);
		cppDEL(m_CropPage);
		cppDEL(m_Wizard);
	}

	vtkDEL(m_CurrentSliceID);

	albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
int albaOpImporterDicom::BuildVMEImagesOutput()
{
	albaVMEGroup       *imagesGroupOuput;
	int nFrames = m_SelectedSeries->GetCardiacImagesNum();

	int step = GetSetting()->GetBuildStep() + 1;
	int cropInterval = (m_ZCropBounds[1]+1 - m_ZCropBounds[0]);
	
	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Building images: please wait...");

	albaNEW(imagesGroupOuput);

	imagesGroupOuput->SetName(albaString::Format("%s images",m_VMEName.GetCStr()));
	imagesGroupOuput->ReparentTo(m_Input);

	int parsedSlices = 0;

	//Loop foreach slice
	for (int i = m_ZCropBounds[0]; i < m_ZCropBounds[1]+1; i += step)
	{
		albaString name;
		name.Printf("%s s:%d", m_VMEName.GetCStr(), i);
		albaSmartPointer<albaVMEImage> vmeImage;
		vmeImage->SetName(name);
		vmeImage->GetTagArray()->DeepCopy(m_TagArray);
		vmeImage->GetMaterial()->m_ColorLut->DeepCopy(m_SliceTexture->GetLookupTable());

		//loop foreach time
		for (int t = 0; t < nFrames; t++)
		{
			int sliceID = GetSliceIDInSeries(t, i);
			albaDicomSlice * currentSlice = m_SelectedSeries->GetSlice(sliceID);
			albaTimeStamp triggerTime = currentSlice->GetTriggerTime();

			vtkImageData *image = currentSlice->GetNewVTKImageData();
			Crop(image);
			image->SetOrigin(currentSlice->GetUnrotatedOrigin());

			//Set data at specific time
			vmeImage->SetData(image, triggerTime);

			vtkDEL(image);
		}
		
		//Setting orientation matrix
		albaDicomSlice *sliceAtZeroTime = m_SelectedSeries->GetSlice(GetSliceIDInSeries(0, i));
		albaMatrix orientationMatrix;
		orientationMatrix.SetFromDirectionCosines(sliceAtZeroTime->GetDcmImageOrientationPatient());
		vmeImage->SetAbsMatrix(orientationMatrix);

		imagesGroupOuput->AddChild(vmeImage);

		progressHelper.UpdateProgressBar(i * 100 / m_SelectedSeries->GetSlicesNum());
	}

	m_Output = imagesGroupOuput;

	return OP_RUN_OK;
}

//----------------------------------------------------------------------------
int albaOpImporterDicom::BuildVMEVolumeGrayOutput()
{
	albaVMEVolumeGray	*VolumeOut;
	albaDicomSlice* firstSlice = m_SelectedSeries->GetSlice(0);
	
	int nFrames = m_SelectedSeries->GetCardiacImagesNum();
	int step = (m_TestMode || !GetSetting()) ? 1 : GetSetting()->GetBuildStep() + 1;
	int cropInterval = (m_ZCropBounds[1]+1 - m_ZCropBounds[0]);
	int SlicesPerFrame = (cropInterval / step);
	
	if(cropInterval % step != 0)
		SlicesPerFrame+=1;

	int totalNumberOfImages = SlicesPerFrame*nFrames;
	int parsedSlices = 0;

	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Building volume: please wait...");

	albaNEW(VolumeOut);
	
	//Loop foreach time 
	for (int t = 0; t < nFrames; t++)
	{
		int firstSliceAtTimeID = GetSliceIDInSeries(t, 0);
		albaTimeStamp triggerTime = m_SelectedSeries->GetSlice(firstSliceAtTimeID)->GetTriggerTime();

		albaOpImporterDicomSliceAccHelper accumulate;
		accumulate.SetNumOfSlices(SlicesPerFrame);

		int accumSliceN = 0;
		//Loop foreach slice
		for (int i = m_ZCropBounds[0], s_id=0; i < m_ZCropBounds[1] + 1; i += step)
		{
			int sliceID = GetSliceIDInSeries(t, i);
			parsedSlices++;

			albaDicomSlice * slice = m_SelectedSeries->GetSlice(sliceID);
			vtkImageData *image = slice->GetNewVTKImageData();
			Crop(image);

			accumulate.SetSlice(accumSliceN, image, slice->GetUnrotatedOrigin());

			progressHelper.UpdateProgressBar(parsedSlices * 100 / totalNumberOfImages);
			accumSliceN++;

			vtkDEL(image);
		}

		vtkDataSet *acc_out;
		acc_out = accumulate.GetNewOutput(); 
		acc_out->Update();

		if (GetSetting() && GetSetting()->GetAutoResample() && vtkRectilinearGrid::SafeDownCast(acc_out))
		{
			vtkRectilinearGrid *rg = vtkRectilinearGrid::SafeDownCast(acc_out);
			vtkALBASmartPointer<vtkALBAVolumeResample> resample = vtkALBAVolumeResample::New();
			vtkALBASmartPointer<vtkImageData> sp;

			double inputDataOrigin[3];
			inputDataOrigin[0] = rg->GetXCoordinates()->GetComponent(0, 0);
			inputDataOrigin[1] = rg->GetYCoordinates()->GetComponent(0, 0);
			inputDataOrigin[2] = rg->GetZCoordinates()->GetComponent(0, 0);

			double sr[2];
			rg->GetScalarRange(sr);
			double w = sr[1] - sr[0];
			double l = (sr[1] + sr[0]) * 0.5;
			double bounds[6];
			rg->GetBounds(bounds);
			
			double volSpacing[3] = { VTK_DOUBLE_MAX ,VTK_DOUBLE_MAX ,VTK_DOUBLE_MAX };
			
			for (int xi = 1; xi < rg->GetXCoordinates()->GetNumberOfTuples(); xi++)
			{
				double spcx = rg->GetXCoordinates()->GetTuple1(xi) - rg->GetXCoordinates()->GetTuple1(xi - 1);
				if (volSpacing[0] > spcx && spcx != 0.0)
					volSpacing[0] = spcx;
			}

			for (int yi = 1; yi < rg->GetYCoordinates()->GetNumberOfTuples(); yi++)
			{
				double spcy = rg->GetYCoordinates()->GetTuple1(yi) - rg->GetYCoordinates()->GetTuple1(yi - 1);
				if (volSpacing[1] > spcy && spcy != 0.0)
					volSpacing[1] = spcy;
			}

			for (int zi = 1; zi < rg->GetZCoordinates()->GetNumberOfTuples(); zi++)
			{
				double spcz = rg->GetZCoordinates()->GetTuple1(zi) - rg->GetZCoordinates()->GetTuple1(zi - 1);
				if (volSpacing[2] > spcz && spcz != 0.0)
					volSpacing[2] = spcz;
			}

			int output_extent[6];
			output_extent[0] = 0;
			output_extent[1] = (bounds[1] - bounds[0]) / volSpacing[0];
			output_extent[2] = 0;
			output_extent[3] = (bounds[3] - bounds[2]) / volSpacing[1];
			output_extent[4] = 0;
			output_extent[5] = (bounds[5] - bounds[4]) / volSpacing[2];

			sp->SetSpacing(volSpacing);
			sp->SetOrigin(inputDataOrigin);
			sp->SetScalarType(rg->GetPointData()->GetScalars()->GetDataType());
			sp->SetExtent(output_extent);
			sp->SetUpdateExtent(output_extent);

			resample->SetVolumeOrigin(inputDataOrigin);
			resample->SetZeroValue(0);
			resample->SetWindow(w);
			resample->SetLevel(l);
			resample->SetInput(acc_out);
			resample->SetOutput(sp);
			resample->AutoSpacingOff();
			resample->Update();
			resample->GetOutput()->Update();

			VolumeOut->SetDataByDetaching(resample->GetOutput(), triggerTime);
		}
		else
		{
			//Set data at specific time
			VolumeOut->SetDataByDetaching(acc_out, triggerTime);
		}
	}

	//Setting orientation matrix
	albaMatrix orientationMatrix;
	orientationMatrix.SetFromDirectionCosines(firstSlice->GetDcmImageOrientationPatient());
	VolumeOut->SetAbsMatrix(orientationMatrix);
		
	//Copy Dicom's tags inside the VME 
	VolumeOut->GetTagArray()->DeepCopy(m_TagArray);
	VolumeOut->SetName(m_VMEName);  
	m_Output = VolumeOut;

	return OP_RUN_OK;
}
//----------------------------------------------------------------------------
void albaOpImporterDicom::CreateLoadPage()
{
	m_LoadPage = new albaGUIWizardPageNew(m_Wizard,albaWIZARDUSEGUI|albaWIZARDUSERWI);
	m_LoadGuiLeft = new albaGUI(this);
	m_LoadGuiUnderLeft = new albaGUI(this);
	m_LoadGuiUnderCenter = new albaGUI(this);
	m_LoadGuiCenter = new albaGUI(this);

	m_StudyListbox  = m_LoadGuiUnderLeft->ListBox(ID_STUDY_SELECT,_("Study"),80,"",wxLB_HSCROLL,190);

	m_SeriesListbox = m_LoadGuiUnderCenter->ListBox(ID_SERIES_SELECT,_("Series"),80,"", wxLB_HSCROLL,190);

	m_LoadGuiLeft->FitGui();
	m_LoadGuiUnderLeft->FitGui();
	m_LoadGuiUnderCenter->FitGui();
	m_LoadPage->AddGuiLowerLeft(m_LoadGuiLeft);
	m_LoadPage->AddGuiLowerUnderLeft(m_LoadGuiUnderLeft);
	m_LoadPage->AddGuiLowerUnderCenter(m_LoadGuiUnderCenter);
	m_LoadPage->AddGuiLowerCenter(m_LoadGuiCenter);

	m_LoadPage->GetRWI()->m_RwiBase->SetMouse(m_Mouse);
	m_LoadPage->GetRWI()->m_RenFront->AddActor(m_SliceActor);
	m_LoadPage->GetRWI()->m_RenFront->AddActor(m_TextActor);

}
//----------------------------------------------------------------------------
void albaOpImporterDicom::CreateCropPage()
{
	m_CropPage = new albaGUIWizardPageNew(m_Wizard,albaWIZARDUSEGUI|albaWIZARDUSERWI,true);
	m_CropPage->SetListener(this);
	m_CropGuiLeft = new albaGUI(this);
	m_CropGuiCenter = new albaGUI(this);

	m_CropGuiLeft->FitGui();
	m_CropGuiCenter->FitGui();
	m_CropPage->AddGuiLowerLeft(m_CropGuiLeft);
	m_CropPage->AddGuiLowerCenter(m_CropGuiCenter);

	m_CropPage->GetRWI()->m_RwiBase->SetMouse(m_Mouse);
	m_CropPage->GetRWI()->m_RenFront->AddActor(m_SliceActor);
	m_DicomInteractor->SetRWI(m_CropPage->GetRWI());
}
//----------------------------------------------------------------------------
void albaOpImporterDicom::GuiUpdate()
{
	m_LoadGuiLeft->Update();
	m_LoadGuiUnderLeft->Update();
	m_LoadGuiCenter->Update();
	if (m_CropPage)
	{
		m_CropGuiLeft->Update();
		m_CropGuiCenter->Update();
	}
}
//----------------------------------------------------------------------------
bool albaOpImporterDicom::OpenDir(const char *dirPath)
{
	wxBusyCursor *busyCursor = NULL; 
	if (!m_TestMode)
		busyCursor = new wxBusyCursor();

	bool successful = LoadDicomFromDir(dirPath);

	cppDEL(busyCursor);

	if (!successful)
		return false;

	if(!this->m_TestMode)
	{
		FillStudyListBox();
		OnStudySelect();
		CameraReset();
	}
	else 
		SelectSeries(m_StudyList->GetStudy(0)->GetSeries(0));

	return true;
}

//----------------------------------------------------------------------------
vtkImageData * albaOpImporterDicom::GetSliceInCurrentSeries(int id)
{
	return	m_SelectedSeries->GetSlice(id)->GetNewVTKImageData();
}

//----------------------------------------------------------------------------
void albaOpImporterDicom::OnEvent(albaEventBase *alba_event) 
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
			case ID_RANGE_MODIFIED:
				OnRangeModified();
			break;
			case albaGUIWizard::ALBA_WIZARD_CHANGE_PAGE:
				OnWizardChangePage(e);
			break;
			case albaGUIWizard::ALBA_WIZARD_CHANGED_PAGE:
			{
				/* This is a ack, because that "genius" of wx  send the change event
				before page show, so we need to duplicate the code here in order to
				manage the camera update */
				m_Wizard->GetCurrentPage()->Show();
				m_Wizard->GetCurrentPage()->SetFocus();
				m_Wizard->GetCurrentPage()->Update();

				wxWindow* NextButton = m_Wizard->FindWindowById(wxID_FORWARD);
				wxString tmp = NextButton->GetLabel();

				if (GetSetting() && !GetSetting()->GetSkipCrop())
					NextButton->SetLabel("&Crop >");
				else
					NextButton->SetLabel("Finish");
				CameraReset(); 
			}
			break;
			case ID_STUDY_SELECT:
				OnStudySelect();
			break;
			case ID_SERIES_SELECT:
				SelectSeries(m_StudyList->GetStudy(m_SelectedStudy)->GetSeries(m_SeriesListbox->GetSelection()));
			break;
			case ID_SHOW_TEXT:
				m_TextActor->SetVisibility(m_ShowOrientationPosition);
				m_LoadPage->GetRWI()->CameraUpdate();
			break;
			case ID_SCAN_TIME:
			case ID_SCAN_SLICE:
				OnChangeSlice();
			break;
			case ID_UPDATE_NAME:
				SetVMEName();
			break;
			default:
				albaEventMacro(*e);
		}
	}
}
//----------------------------------------------------------------------------
void albaOpImporterDicom::SetVMEName()
{
	albaDicomSlice * sliceData = m_SelectedSeries->GetSlice(0);
	m_VMEName = sliceData->GetModality();
	if (sliceData->GetDescription() != "" && m_DescrInName)
	{
		m_VMEName += " ";
		m_VMEName += sliceData->GetDescription();
	}
	if (sliceData->GetPatientName() != "" && m_PatientNameInName)
	{
		albaString patientName = sliceData->GetPatientName();
		patientName.Replace('^', ' ');
		m_VMEName += " ";
		m_VMEName += patientName;
	}
	if (m_SizeInName)
	{
		albaString size;
		int *dims=sliceData->GetSliceSize();
		if (sliceData->GetNumberOfCardiacImages() > 1)
			size.Printf(" %dx%dx%d f:%d", dims[0], dims[1], m_SelectedSeries->GetSlicesNum(), sliceData->GetNumberOfCardiacImages());
		else
			size.Printf(" %dx%dx%d", dims[0], dims[1], m_SelectedSeries->GetSlicesNum());
		m_VMEName += size;
	}

	if(m_CropGuiCenter)
		m_CropGuiCenter->Update();
}
//----------------------------------------------------------------------------
void albaOpImporterDicom::CameraUpdate()
{
	if(m_Wizard->GetCurrentPage() == m_LoadPage)
	{
		m_LoadPage->UpdateWindowing(m_SliceRange, m_SliceSubRange);
		m_LoadPage->UpdateActor();
		m_LoadPage->GetRWI()->CameraUpdate();
	}
	else
	{
		m_CropPage->UpdateWindowing(m_SliceRange, m_SliceSubRange);
		m_CropPage->UpdateActor();
		m_CropPage->GetRWI()->CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void albaOpImporterDicom::CameraReset()
{
	if (m_LoadPage && m_Wizard->GetCurrentPage() == m_LoadPage)
	{
		m_LoadPage->UpdateWindowing(m_SliceRange, m_SliceSubRange);
		m_LoadPage->GetRWI()->CameraReset();
	}
	else if(m_CropPage)
	{
		m_CropPage->UpdateWindowing(m_SliceRange, m_SliceSubRange);
		m_CropPage->GetRWI()->CameraReset();
	}
}
//----------------------------------------------------------------------------
void albaOpImporterDicom::CreateSliceVTKPipeline()
{
	vtkNEW(m_SliceLookupTable);
	vtkNEW(m_SliceTexture);
	vtkNEW(m_SlicePlane);
	vtkNEW(m_SliceMapper);
	vtkNEW(m_SliceActor);

	m_SliceTexture->InterpolateOn();
	m_SliceMapper->SetInput(m_SlicePlane->GetOutput());
	m_SliceActor->SetMapper(m_SliceMapper);
	m_SliceActor->SetTexture(m_SliceTexture); 
	
	// text stuff
	m_TextMapper = vtkTextMapper::New();
	m_TextMapper->GetTextProperty()->AntiAliasingOn();

	m_TextActor = vtkActor2D::New();
	m_TextActor->GetProperty()->SetColor(0.8,0,0);
	m_TextActor->SetMapper(m_TextMapper);
	m_TextActor->SetPosition(3,3);  
	m_TextActor->VisibilityOff();
	m_TextMapper->Modified();

	// interactor
	if (m_Mouse)
	{
		albaNEW(m_DicomInteractor);
		m_DicomInteractor->SetListener(this);
		m_Mouse->AddObserver(m_DicomInteractor, MCH_INPUT);
	}
}
//----------------------------------------------------------------------------
void albaOpImporterDicom::FillStudyListBox()
{
	albaString studyName;
	if (m_StudyListbox)
	{
		for (int n = 0; n < m_StudyList->GetStudiesNum(); n++)
		{
			studyName.Printf("Study %d", n);
			m_StudyListbox->Append(studyName.GetCStr());
		}
		m_StudyListbox->SetSelection(0);
	}
}
//----------------------------------------------------------------------------
void albaOpImporterDicom::FillSeriesListBox()
{
	if (m_SeriesListbox)
	{
		int counter = 0;
		m_SeriesListbox->Clear();

		albaDicomStudy *study = m_StudyList->GetStudy(m_SelectedStudy);
		albaString seriesName;

		for (int i = 0; i < study->GetSeriesNum(); i++)
		{
			albaDicomSeries *series = study->GetSeries(i);

			const int *dim = series->GetDimensions();
			int framesNum = series->GetCardiacImagesNum();

			if (framesNum > 1)
				seriesName.Printf("Series %dx%dx%d f%d", dim[0], dim[1], framesNum / series->GetSlicesNum(), framesNum);
			else
				seriesName.Printf("Series %dx%dx%d", dim[0], dim[1], series->GetSlicesNum());

			m_SeriesListbox->Append(seriesName.GetCStr());
		}
	}
}
//----------------------------------------------------------------------------
bool albaOpImporterDicom::LoadDicomFromDir(const char *dicomDirABSPath)
{   
	wxDir dir;
	wxArrayString allFiles;

	//Reading file list in folder and sub folders
	dir.GetAllFiles(dicomDirABSPath, &allFiles, wxEmptyString, wxDIR_FILES | wxDIR_DIRS);
	int fileNumber = allFiles.size();

	albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Reading DICOM directory: please wait...");

	m_StudyList = new albaDicomStudyList();
		
	for (int i = 0; i < fileNumber; i++)
	{
		albaDicomSlice *slice= ReadDicomFile(allFiles[i]);
		if (slice)
			m_StudyList->AddSlice(slice,GetSetting());
		progressHelper.UpdateProgressBar( i*100 / fileNumber);
	}
		
	progressHelper.CloseProgressBar();

	if(GetSetting() && GetSetting()->GetAutoVMEType() && GetSetting()->GetOutputType() == TYPE_VOLUME)
		m_StudyList->RemoveSingleImagesFromList();
	
	// start handling files
	if(m_StudyList->GetStudiesNum() == 0)
	{
		if (!this->m_TestMode)
		{
			wxString msg = "No study found!";
			wxMessageBox(msg,"Confirm", wxOK , NULL);
		}
		return false;
	}

	albaLogMessage("Found %d Dicom series", m_StudyList->GetSeriesTotalNum());

	return m_StudyList->GetStudiesNum()>0;
}


//----------------------------------------------------------------------------
albaDicomSlice *albaOpImporterDicom::ReadDicomFile(albaString fileName)
{
	int dcmCardiacNumberOfImages = -1;
	std::string dcmModality, dcmStudyInstanceUID, dcmAcquisitionNumber, dcmSeriesInstanceUID, dcmScanOptions, dcmImageType;
	std::string date, description, patientName, birthdate, photometricInterpretation;
	double dcmTriggerTime = 0;
	double defaulOrienatation[6] = { 1.0,0.0,0.0,0.0,1.0,0.0 };
	double dcmImageOrientationPatient[6];
	double dcmImagePositionPatient[3] = {0.0,0.0,0.0};
	int imageSize[2];
	
	//Load selected data
	gdcm::Reader dcmReader;
	dcmReader.SetFileName(fileName.GetCStr());
		
	if (!dcmReader.CanRead())
	{
		albaLogMessage("File <%s> can not be opened and will be skipped",fileName.GetCStr());
		return NULL;
	}

	std::set<gdcm::Tag> TagsToRead;
	TagsToRead.insert(TAG_StudyInstanceUID);
	TagsToRead.insert(TAG_SeriesInstanceUID);
	TagsToRead.insert(TAG_AcquisitionNumber);
	TagsToRead.insert(TAG_ScanOptions);
	TagsToRead.insert(TAG_ImagePositionPatient);
	TagsToRead.insert(TAG_ImageOrientationPatient);
	TagsToRead.insert(TAG_ImageType);
	TagsToRead.insert(TAG_TriggerTime);
	TagsToRead.insert(TAG_CardiacNumberOfImages);
	TagsToRead.insert(TAG_StudyDate);
	TagsToRead.insert(TAG_SeriesDescription);
	TagsToRead.insert(TAG_PatientsName);
	TagsToRead.insert(TAG_PhotometricInterpretation);
	TagsToRead.insert(TAG_Rows);
	TagsToRead.insert(TAG_Columns);


	if (!dcmReader.ReadSelectedTags(TagsToRead))
	{
		albaLogMessage("Cannot read tags from file:<%s>, this file will be skipped", fileName.GetCStr());
		return NULL;
	}

	//Obtain dataset information from Dicom file (loaded into memory)
	gdcm::DataSet & dcmDataSet = dcmReader.GetFile().GetDataSet();

	dcmScanOptions = READTAG(TAG_ScanOptions);
	dcmModality = READTAG(TAG_Modality);

	//Read Study-Series IDs
	dcmStudyInstanceUID = READTAG(TAG_StudyInstanceUID);
	dcmSeriesInstanceUID = READTAG(TAG_SeriesInstanceUID);
	dcmAcquisitionNumber = READTAG(TAG_AcquisitionNumber);
	dcmImageType = READTAG(TAG_ImageType);

	//Read the image size
	imageSize[1] = GetAttributeValue ATTRIBUTE_Rows(dcmDataSet);
	imageSize[0] = GetAttributeValue ATTRIBUTE_Columns(dcmDataSet);
	if (imageSize[0] * imageSize[1] == 0)
	{
		albaLogMessage("Wrong slice size on %s\nSkip Slice.", fileName.GetCStr());
		return NULL;
	}
	
	//Try to read image position patient form Dicom
	if (dcmDataSet.FindDataElement(TAG_ImagePositionPatient))
	{
		gdcm::Attribute ATTRIBUTE_ImagePositionPatient at;
		at.SetFromDataSet(dcmDataSet);

		dcmImagePositionPatient[0] = at.GetValue(0);
		dcmImagePositionPatient[1] = at.GetValue(1);
		dcmImagePositionPatient[2] = at.GetValue(2);
	}
	else
	{
		if (GetSetting() && GetSetting()->GetDCMImagePositionPatientExceptionHandling() == albaGUIDicomSettings::APPLY_DEFAULT_POSITION)
		{
			albaLogMessage("Cannot read Dicom tag ImagePositionPatient on %s\nUse default position.", fileName.GetCStr());
		}
		else // albaGUIDicomSettings::SKIP_ALL 
		{
			albaLogMessage("Cannot read Dicom tag ImagePositionPatient on %s\nSkip Slice.", fileName.GetCStr());
			return NULL;
		}
	}
		
	//Read Image Orientation Patient
	if (dcmDataSet.FindDataElement(TAG_ImageOrientationPatient))
	{
		
		const gdcm::DataElement& de = dcmDataSet.GetDataElement(TAG_ImageOrientationPatient);
		gdcm::Attribute ATTRIBUTE_ImageOrientationPatient at;
		at.SetFromDataElement(de);

		for (int i = 0; i < 6; i++)
			dcmImageOrientationPatient[i] = at.GetValue(i);
	}
	else
	{
		for (int i = 0; i < 6; i++)
			dcmImageOrientationPatient[i] = defaulOrienatation[i];
	}

	//Getting Trigger Time
	dcmTriggerTime = GetAttributeValue ATTRIBUTE_TriggerTime(dcmDataSet);

	std::string cardImgNum = READTAG(TAG_CardiacNumberOfImages);
	sscanf(cardImgNum.c_str(), "%d", &dcmCardiacNumberOfImages);
	if (dcmCardiacNumberOfImages < 1) dcmCardiacNumberOfImages = 1;
	
	//Read Patients data strings
	birthdate = READTAG(TAG_PatientsBirthDate);
	date = READTAG(TAG_StudyDate);
	description = READTAG(TAG_SeriesDescription);
	patientName = READTAG(TAG_PatientsName);
	photometricInterpretation = READTAG(TAG_PhotometricInterpretation);
	
	//Create Slice
	albaDicomSlice *newSlice = new albaDicomSlice(fileName, dcmImageOrientationPatient, dcmImagePositionPatient, description.c_str(), date.c_str(), patientName.c_str(), birthdate.c_str(), dcmCardiacNumberOfImages, dcmTriggerTime);
	newSlice->SetModality(dcmModality.c_str());
	newSlice->SetPhotometricInterpretation(photometricInterpretation.c_str());
	newSlice->SetSeriesID(dcmSeriesInstanceUID.c_str());
	newSlice->SetStudyID(dcmStudyInstanceUID.c_str());
	newSlice->SetAcquisitionNumber(dcmAcquisitionNumber.c_str());
	newSlice->SetImageType(dcmImageType.c_str());
	newSlice->SetSliceSize(imageSize);
		
	return newSlice;
}

//----------------------------------------------------------------------------
template <uint16_t A, uint16_t B> double albaOpImporterDicom::GetAttributeValue(gdcm::DataSet &dcmDataSet)
{
	gdcm_ns::Tag tag(A, B);
	if (dcmDataSet.FindDataElement(tag))
	{
		gdcm::Attribute<A, B> attr;
		const gdcm::DataElement& de = dcmDataSet.GetDataElement(tag);
		attr.SetFromDataElement(de);
		return attr.GetValue();
	}
	else 
		return 0;
}

//----------------------------------------------------------------------------
void albaOpImporterDicom::CreateSliders()
{
	int numOfSlices = m_SelectedSeries->GetSlicesNum();
	int cardiacImageNum = m_SelectedSeries->GetCardiacImagesNum();
	m_CurrentSlice = 0;
	if(m_LoadGuiLeft)
	{
		m_LoadPage->RemoveGuiLowerLeft(m_LoadGuiLeft);
		delete m_LoadGuiLeft;
		m_LoadGuiLeft = new albaGUI(this);
		m_SliceScannerLoadPage=m_LoadGuiLeft->Slider(ID_SCAN_SLICE,_("Slice #"),&m_CurrentSlice,0, numOfSlices -1,"",true);
		m_LoadGuiLeft->Enable(ID_SCAN_SLICE, numOfSlices > 1);
		if (cardiacImageNum > 1)
			m_LoadGuiLeft->Slider(ID_SCAN_TIME, _("Time "), &m_CurrentTime, 0, cardiacImageNum);
		m_LoadGuiLeft->Label("");
		m_LoadGuiLeft->Bool(ID_SHOW_TEXT, "Show position info", &m_ShowOrientationPosition, 1, _("Shows position and orientation"));
		m_LoadPage->AddGuiLowerLeft(m_LoadGuiLeft);
		m_LoadPage->Update();
	}

	if(m_CropGuiLeft)
	{
		m_CropPage->RemoveGuiLowerLeft(m_CropGuiLeft);
		delete m_CropGuiLeft;
		m_CropGuiLeft = new albaGUI(this);
		m_SliceScannerCropPage=m_CropGuiLeft->Slider(ID_SCAN_SLICE,_("Slice #"),&m_CurrentSlice,0, numOfSlices -1,"",true);
		m_CropGuiLeft->Enable(ID_SCAN_SLICE, numOfSlices > 1);
		if (cardiacImageNum > 1)
			m_CropGuiLeft->Slider(ID_SCAN_TIME, _("Time "), &m_CurrentTime, 0, cardiacImageNum);
		m_CropPage->AddGuiLowerLeft(m_CropGuiLeft);
		m_CropPage->Update();
	}
}
//----------------------------------------------------------------------------
int albaOpImporterDicom::GetSliceIDInSeries(int timeId, int heigthId)
{
	albaDicomSlice *firstDicomListElement;
	firstDicomListElement = (albaDicomSlice *)m_SelectedSeries->GetSlice(0);
	int timeFrames =  firstDicomListElement->GetNumberOfCardiacImages();

	int dicomFilesNumber = m_SelectedSeries->GetSlicesNum();
	int numSlicesPerTS = dicomFilesNumber / timeFrames;
	return timeId*numSlicesPerTS + heigthId;
}
//----------------------------------------------------------------------------
void albaOpImporterDicom::GenerateSliceTexture(int imageID)
{
	double  range[2];
	albaString text;

	vtkDEL(m_CurrentSliceID);

	albaDicomSlice* slice = m_SelectedSeries->GetSlice(imageID);
	assert(slice);

	m_CurrentSliceID = slice->GetNewVTKImageData();
	m_CurrentSliceID->Update();
	m_CurrentSliceID->GetBounds(m_SliceBounds);
	
	//Setting ranges, range should be the range of current slice and subrange should be inside current range
	m_CurrentSliceID->GetScalarRange(m_SliceRange);
	m_SliceSubRange[0] = MIN(m_SliceSubRange[0], m_SliceRange[0]);
	m_SliceSubRange[1] = MAX(m_SliceSubRange[1], m_SliceRange[1]);

	double origin[3], orientation[6];
	m_CurrentSliceID->GetOrigin(origin);
	slice->GetDcmImageOrientationPatient(orientation);

	text.Printf("Orientation: %f, %f, %f, %f, %f, %f \nPosition: %f, %f, %f",orientation[0], orientation[1], orientation[2], orientation[3], orientation[4], orientation[5], origin[0], origin[1], origin[2]);
	m_TextMapper->SetInput(text.GetCStr());
	m_TextMapper->Modified();

	m_CurrentSliceID->GetScalarRange(range);
	m_SliceTexture->SetInput(m_CurrentSliceID);
	m_SliceTexture->Modified();
		
	//Invert gray scale for Photometric Interpretation MONOCHROME1
	if(wxString(slice->GetPhotometricInterpretation().GetCStr()).Contains("MONOCHROME1"))
		lutPreset(20,m_SliceLookupTable);
	else
		lutPreset(4, m_SliceLookupTable);
	
	m_SliceLookupTable->SetTableRange(range);
	m_SliceLookupTable->Build();

	m_SliceTexture->MapColorScalarsThroughLookupTableOn();
	m_SliceTexture->SetLookupTable((vtkLookupTable *)m_SliceLookupTable);
}

//----------------------------------------------------------------------------
void albaOpImporterDicom::Crop(vtkImageData *slice)
{
	if (m_CropEnabled)
	{
		slice->SetUpdateExtent(m_CropExtent);
		slice->Crop();
	}
}

//----------------------------------------------------------------------------
void albaOpImporterDicom::CalculateCropExtent()
{
	double spacing[3], crop_bounds[6], sliceOrigin[3];
	int sliceExtent[6];

	m_CurrentSliceID->GetExtent(sliceExtent);
	m_CurrentSliceID->GetSpacing(spacing);
	m_SelectedSeries->GetSlice(0)->GetImagePositionPatient(sliceOrigin);
	
	if (m_DicomInteractor)
		m_DicomInteractor->GetPlaneBounds(crop_bounds);
	else
		m_CurrentSliceID->GetBounds(crop_bounds);

	//Align cropBounds to current grid
	m_CropExtent[0] = round(crop_bounds[0] / spacing[0]);
	m_CropExtent[1] = round(crop_bounds[1] / spacing[0]);
	m_CropExtent[2] = round(crop_bounds[2] / spacing[1]);
	m_CropExtent[3] = round(crop_bounds[3] / spacing[1]);
	m_CropExtent[4] = m_CropExtent[5] = 0;
		
	//Enable/disable Crop
	m_CropEnabled = false;
	if(m_CropPage)
		for (int i = 0; i < 4; i++)
			if (sliceExtent[i] != m_CropExtent[i])
			{
				m_CropEnabled = true;
				break;
			}
}

//----------------------------------------------------------------------------
void albaOpImporterDicom::SetPlaneDims()
{	
	double diffx,diffy;
	diffx=m_SliceBounds[1]-m_SliceBounds[0];
	diffy=m_SliceBounds[3]-m_SliceBounds[2];

	m_SlicePlane->SetOrigin(0,0,0); 
	m_SlicePlane->SetPoint1(diffx,0,0);
	m_SlicePlane->SetPoint2(0,diffy,0);
}

//----------------------------------------------------------------------------
void albaOpImporterDicom::ImportDicomTags()
{
	if (m_TagArray == NULL) 
		albaNEW(m_TagArray);
	m_TagArray->SetName("TagArray");

	const char *sliceABSFileName = m_SelectedSeries->GetSlice(0)->GetSliceABSFileName();

	gdcm::Reader dcmReader;
	dcmReader.SetFileName(sliceABSFileName);

	if (!dcmReader.CanRead())
	{
		albaLogMessage("File <%s> can not be opened and will be skipped", sliceABSFileName);
		return;
	}

	std::set<gdcm::Tag> TagsToRead;

	TagsToRead.insert(TAG_PatientsName);
	TagsToRead.insert(TAG_PatientsSex);
	TagsToRead.insert(TAG_PatientsBirthDate);
	TagsToRead.insert(TAG_PatientsWeight);
	TagsToRead.insert(TAG_PatientsAge);
	TagsToRead.insert(TAG_PatientID);
	TagsToRead.insert(TAG_InstitutionName);
	TagsToRead.insert(TAG_StudyDescription);
	TagsToRead.insert(TAG_SeriesDescription);
	TagsToRead.insert(TAG_AcquisitionDate);
	TagsToRead.insert(TAG_PixelSpacing);
	TagsToRead.insert(TAG_ProtocolName);
	TagsToRead.insert(TAG_ManufacturersModelName);
	TagsToRead.insert(TAG_Modality);
	TagsToRead.insert(TAG_Manufacturer);
	TagsToRead.insert(TAG_KVP);
	TagsToRead.insert(TAG_XRayTubeCurrent);
	TagsToRead.insert(TAG_FocalSpots);
	TagsToRead.insert(TAG_FilterType);
	TagsToRead.insert(TAG_SliceThickness);
	TagsToRead.insert(TAG_TableHeight);
	TagsToRead.insert(TAG_ExposureTime);
	TagsToRead.insert(TAG_SpiralPitchFactor);
	TagsToRead.insert(TAG_SpacingBetweenSlices);
	TagsToRead.insert(TAG_ConvolutionKernel);


	InsertAppSpecificTagsToReadList(TagsToRead);
	
	if (!dcmReader.ReadSelectedTags(TagsToRead))
	{
		albaLogMessage("Cannot read tags from file:<%s>, this file will be skipped", sliceABSFileName);
		return;
	}

	//Obtain dataset information from Dicom file (loaded into memory)
	gdcm::DataSet & dcmDataSet = dcmReader.GetFile().GetDataSet();

	std::string tagString;

	READ_AND_SET_TAGARRAY(TAG_PatientsName, "PatientsName");
	READ_AND_SET_TAGARRAY(TAG_PatientsSex, "PatientsSex");
	READ_AND_SET_TAGARRAY(TAG_PatientsBirthDate, "PatientsBirthDate");
	READ_AND_SET_TAGARRAY(TAG_PatientsWeight, "PatientsWeight");
	READ_AND_SET_TAGARRAY(TAG_PatientID, "PatientID");
	READ_AND_SET_TAGARRAY(TAG_PixelSpacing, "PixelSpacing");
	READ_AND_SET_TAGARRAY(TAG_PatientsAge, "PatientsAge");
	READ_AND_SET_TAGARRAY(TAG_InstitutionName, "InstitutionName");
	READ_AND_SET_TAGARRAY(TAG_StudyDescription, "StudyDescription");
	READ_AND_SET_TAGARRAY(TAG_SeriesDescription, "SeriesDescription");
	READ_AND_SET_TAGARRAY(TAG_AcquisitionDate, "AcquisitionDate");
	READ_AND_SET_TAGARRAY(TAG_ProtocolName, "ProtocolName");
	READ_AND_SET_TAGARRAY(TAG_Modality, "Modality");
	READ_AND_SET_TAGARRAY(TAG_Manufacturer, "Manufacturer");
	READ_AND_SET_TAGARRAY(TAG_ManufacturersModelName, "ManufacturersModelName");
	READ_AND_SET_TAGARRAY(TAG_KVP, "KVP");
	READ_AND_SET_TAGARRAY(TAG_XRayTubeCurrent, "XRayTubeCurrent");
	READ_AND_SET_TAGARRAY(TAG_FocalSpots, "FocalSpots");
	READ_AND_SET_TAGARRAY(TAG_FilterType, "FilterType");
	READ_AND_SET_TAGARRAY(TAG_SliceThickness, "SliceThickness");
	READ_AND_SET_TAGARRAY(TAG_TableHeight, "TableHeight");
	READ_AND_SET_TAGARRAY(TAG_ExposureTime, "ExposureTime");
	READ_AND_SET_TAGARRAY(TAG_TotalCollimationWidth, "TotalCollimationWidth");
	READ_AND_SET_TAGARRAY(TAG_SpiralPitchFactor, "SpiralPitchFactor");
	READ_AND_SET_TAGARRAY(TAG_SpacingBetweenSlices, "SpacingBetweenSlices");
	READ_AND_SET_TAGARRAY(TAG_ConvolutionKernel, "ConvolutionKernel");

	ReadAndSetAppSpecificTags(m_TagArray,dcmDataSet);

  m_TagArray->SetTag(albaTagItem("VME_NATURE", "NATURAL"));
}
//----------------------------------------------------------------------------
void albaOpImporterDicom::OnStudySelect()
{
	if (m_StudyListbox)
	{
		if (m_SelectedStudy != m_StudyListbox->GetSelection())
		{
			m_SelectedStudy = m_StudyListbox->GetSelection();
			FillSeriesListBox();
			m_SeriesListbox->Select(0);
			SelectSeries(m_StudyList->GetStudy(m_SelectedStudy)->GetSeries(0));
		}
	}
}
//----------------------------------------------------------------------------
void albaOpImporterDicom::SelectSeries(albaDicomSeries * selectedSeries)
{
	if (m_SelectedSeries != selectedSeries)
	{
		wxBusyCursor *busyCursor = NULL;
		if (!m_TestMode)
			busyCursor = new wxBusyCursor();

		selectedSeries->SortSlices();
		m_SelectedSeries = selectedSeries;
		int numberOfSlices = m_SelectedSeries->GetSlicesNum();
		
		//Reset Z Bounds
		m_ZCropBounds[0] = 0;
		m_ZCropBounds[1] = numberOfSlices - 1;
				
		if (!this->m_TestMode)
		{
			CreateSliders();

			if (m_LoadPage)
			{
				m_LoadPage->RemoveGuiLowerCenter(m_LoadGuiCenter);
				m_LoadGuiCenter = new albaGUI(this);
				m_LoadGuiCenter->Divider();

				if (numberOfSlices > 1 && GetSetting() && !GetSetting()->GetAutoVMEType())
				{
					m_OutputType = 0;
					wxString typeArrayVolumeImage[2] = { _("Volume"),_("Images") };
					m_LoadGuiCenter->Radio(ID_VME_TYPE, "VME", &m_OutputType, 2, typeArrayVolumeImage, 1, "");
				}
				else if (numberOfSlices == 1 || (GetSetting() && GetSetting()->GetOutputType() == TYPE_IMAGE))
				{
					m_LoadGuiCenter->Label("Output type: Image");
					m_LoadGuiCenter->Label("");
					m_LoadGuiCenter->Label("");
					m_OutputType = TYPE_IMAGE;
				}
				else
				{
					m_LoadGuiCenter->Label("Output type: Volume");
					m_OutputType = TYPE_VOLUME;
				}

				m_LoadPage->AddGuiLowerCenter(m_LoadGuiCenter);
				m_LoadPage->Update();

				//Set Z Bounds in Crop page
				if (!this->m_TestMode && m_CropPage)
					m_CropPage->SetZCropBounds(m_ZCropBounds[0], m_ZCropBounds[1]);
			}
		}

		GenerateSliceTexture(0);
		SetPlaneDims();
		CameraReset();
		
		cppDEL(busyCursor);
	}
}
//----------------------------------------------------------------------------
void albaOpImporterDicom::OnWizardChangePage( albaEvent * e )
{

	if(m_Wizard->GetCurrentPage()==m_LoadPage )//From Load page
	{
		if (m_CropPage)
		{
			//get the current windowing in order to maintain subrange thought the wizard pages 
			m_LoadPage->GetWindowing(m_SliceRange, m_SliceSubRange);

			m_DicomInteractor->SetSliceBounds(m_SliceBounds);
			m_DicomInteractor->PlaneVisibilityOn();

			m_CropPage->RemoveGuiLowerCenter(m_CropGuiCenter);
			m_CropGuiCenter = new albaGUI(this);
			m_CropGuiCenter->Divider();

			m_CropGuiCenter->Label("Name:", true);

			albaDicomSlice * sliceData = m_SelectedSeries->GetSlice(0);

			if (sliceData->GetDescription() != "")
				m_CropGuiCenter->Bool(ID_UPDATE_NAME, "Description", &m_DescrInName, 1);
			if (sliceData->GetPatientName() != "")
				m_CropGuiCenter->Bool(ID_UPDATE_NAME, "Patient Name", &m_PatientNameInName, 1);
			m_CropGuiCenter->Bool(ID_UPDATE_NAME, "Image Size", &m_SizeInName, 1);

			m_CropGuiCenter->Label("Result:");
			m_CropGuiCenter->Label(&m_VMEName);

			m_CropPage->AddGuiLowerCenter(m_CropGuiCenter);
			m_CropPage->Update();
		}
		SetVMEName();
	}

	if (m_Wizard->GetCurrentPage() == m_CropPage)//From Crop page to build page
	{
		//get the current windowing in order to maintain subrange thought the wizard pages 
		m_CropPage->GetWindowing(m_SliceRange, m_SliceSubRange);
		m_DicomInteractor->PlaneVisibilityOff();

		//TODO RESTORE THIS
		wxWindow* NextButton = m_Wizard->FindWindowById(wxID_FORWARD);
		NextButton->SetLabel("Crop >");
	}
	
	CameraReset();
	GuiUpdate();
}
//----------------------------------------------------------------------------
void albaOpImporterDicom::OnRangeModified()
{
	double minMax[2];
	m_CropPage->GetZCropBounds(minMax);

	m_ZCropBounds[0] = round(minMax[0]);
	m_ZCropBounds[1] = round(minMax[1]);
	
	if(m_ZCropBounds[0] > m_CurrentSlice || m_CurrentSlice > m_ZCropBounds[1])
	{
		m_CurrentSlice = m_ZCropBounds[0];
		OnChangeSlice();
	}
	GuiUpdate();
}
//----------------------------------------------------------------------------
void albaOpImporterDicom::OnChangeSlice()
{
	// show the current slice
	int currImageId = GetSliceIDInSeries(m_CurrentTime, m_CurrentSlice);
	if(m_CurrentImageID == currImageId)
	{
		return;
	}
	if (currImageId != -1) 
	{
		m_CurrentImageID = currImageId;
		GenerateSliceTexture(currImageId);
	}
	if (m_CropPage)
	{
		m_SliceScannerLoadPage->SetValue(m_CurrentSlice);
		m_SliceScannerLoadPage->Update();
		m_SliceScannerCropPage->SetValue(m_CurrentSlice);
		m_SliceScannerCropPage->Update();
	}

	CameraUpdate();
	GuiUpdate();
}

//----------------------------------------------------------------------------
int albaOpImporterDicom::SelectSeriesWithMoreSlices()
{
	
	albaDicomSeries * seriesToSelect = NULL;
	int maxSlices = VTK_INT_MIN;
	for (int i = 0; i < m_StudyList->GetStudiesNum(); i++)
	{
		albaDicomStudy * study = m_StudyList->GetStudy(i);
		for (int j = 0; j<study->GetSeriesNum();j++)
		{
			albaDicomSeries * series = study->GetSeries(j);

			if(series->GetSlicesNum()>maxSlices)
			{
				seriesToSelect = series;
				maxSlices = series->GetSlicesNum();
			}
		};
	}

	if (seriesToSelect == NULL)
		return ALBA_ERROR;

	SelectSeries(seriesToSelect);

	return ALBA_OK;
}

//----------------------------------------------------------------------------
char ** albaOpImporterDicom::GetIcon()
{
#include "pic/MENU_IMPORT_DICOM.xpm"
	return MENU_IMPORT_DICOM_xpm;
}

/////////////////////////////albaDicomStudyList////////////////////////////////
//----------------------------------------------------------------------------
albaDicomStudyList::~albaDicomStudyList()
{
		for (int i = 0; i < m_Studies.size(); i++)
		cppDEL(m_Studies[i]);
}
//----------------------------------------------------------------------------
void albaDicomStudyList::AddSlice(albaDicomSlice *slice, albaGUIDicomSettings* settings)
{
	albaDicomStudy *study=NULL;

	albaString studyID = slice->GetStudyID();
	for (int i = 0; i < m_Studies.size() && !study; i++)
		if (studyID == m_Studies[i]->GetStudyID())
		{
			study = m_Studies[i];
			break;
		}

	//if the study does not exist we create a new study and push it back on study vector
	if (study == NULL)
	{
		study = new albaDicomStudy(slice->GetStudyID());
		m_Studies.push_back(study);
	}

	study->AddSlice(slice, settings);
}
//----------------------------------------------------------------------------
albaDicomStudy * albaDicomStudyList::GetStudy(int num)
{
	return m_Studies[num];
}
//----------------------------------------------------------------------------
int albaDicomStudyList::GetSeriesTotalNum()
{
	int total = 0;
	for (int i = 0; i < m_Studies.size(); i++)
		total += m_Studies[i]->GetSeriesNum();
	
	return total;
}

//----------------------------------------------------------------------------
void albaDicomStudyList::RemoveSingleImagesFromList()
{
	for (int i = 0; i < m_Studies.size();) //no increment counter increment should not be happen if the current element is erased.
	{
		m_Studies[i]->RemoveSingleImagesFromSeries();

		if (m_Studies[i]->GetSeriesNum() == 0)
		{
			cppDEL(m_Studies[i]);
			m_Studies.erase(m_Studies.begin() + i);
		}
		else
			i++;
	}
}

///////////////////////////////albaDicomStudy//////////////////////////////////
//----------------------------------------------------------------------------
albaDicomStudy::~albaDicomStudy()
{
	for (int i = 0; i < m_Series.size(); i++)
		cppDEL(m_Series[i]);
}
//----------------------------------------------------------------------------
void albaDicomStudy::AddSlice(albaDicomSlice *slice, albaGUIDicomSettings* settings)
{
	albaDicomSeries *series = NULL;

	albaString serieID = slice->GetSeriesID();
	albaString acqusitionNumber = slice->GetAcquisitionNumber();
	albaString imageType = slice->GetImageType();
	int *sliceDim = slice->GetSliceSize();

	for (int i = 0; i < m_Series.size() && !series; i++)
		if (serieID == m_Series[i]->GetSerieID() && m_Series[i]->GetImageType() == imageType &&
				sliceDim[0] == m_Series[i]->GetDimensions()[0] && sliceDim[1] == m_Series[i]->GetDimensions()[1])
		{
			if (m_Series[i]->GetAcquisitionNumber() != acqusitionNumber)
				int x = 0;

			if (m_Series[i]->GetAcquisitionNumber() == acqusitionNumber || m_AcquisitionNumberLastChoice == albaGUIDicomSettings::MERGE_DIFFERNT_ACQUISITION_NUMBER)
			{
				series = m_Series[i];
				break;
			}
			else if (m_AcquisitionNumberLastChoice != albaGUIDicomSettings::SPLIT_DIFFERNT_ACQUISITION_NUMBER)
			{
				m_AcquisitionNumberLastChoice = settings->GetAcquisitionNumberStrategy();
				if (m_AcquisitionNumberLastChoice == albaGUIDicomSettings::MERGE_DIFFERNT_ACQUISITION_NUMBER)
				{
					series = m_Series[i];
					break;
				}
			}
		}

	//if the study does not exist we create a new study and push it back on study vector
	if (series == NULL)
	{
		series = new albaDicomSeries(serieID,acqusitionNumber, imageType);
		m_Series.push_back(series);
	}

	series->AddSlice(slice);
}

//----------------------------------------------------------------------------
void albaDicomStudy::RemoveSingleImagesFromSeries()
{
	for (int i = 0; i < m_Series.size();) //no increment counter increment should not be happen if the current element is erased.
	{
		if (m_Series[i]->GetSlicesNum() == 1)
		{
			cppDEL(m_Series[i]);
			m_Series.erase(m_Series.begin() + i);
		}
		else
			i++;
	}
}

///////////////////////////////albaDicomSeries//////////////////////////////////
//----------------------------------------------------------------------------
albaDicomSeries::~albaDicomSeries()
{
	for (int i = 0; i < m_Slices.size(); i++)
		cppDEL(m_Slices[i]);
}
//----------------------------------------------------------------------------
void albaDicomSeries::AddSlice(albaDicomSlice *slice)
{
	if (m_Slices.size() == 0)
	{
		int *dim = slice->GetSliceSize();
		m_Dimensions[0] = dim[0];
		m_Dimensions[1] = dim[1];
		m_Dimensions[2] = 1;

		m_CardiacImagesNum = slice->GetNumberOfCardiacImages();
	}
	
	m_IsRotated = IsRotated(slice->GetDcmImageOrientationPatient());

	m_Slices.push_back(slice);
}
//----------------------------------------------------------------------------
bool albaDicomSeries::IsRotated(const double dcmImageOrientationPatient[6])
{
	// check if the dataset is rotated: => different from 1. 0. 0. 0. 1. 0.
	return !( fabs(dcmImageOrientationPatient[0] - 1.0) < EPSILON && fabs(dcmImageOrientationPatient[1]) < EPSILON &&
						fabs(dcmImageOrientationPatient[2])				< EPSILON && fabs(dcmImageOrientationPatient[3]) < EPSILON &&
						fabs(dcmImageOrientationPatient[4] - 1.0) < EPSILON && fabs(dcmImageOrientationPatient[5]) < EPSILON);
}
//----------------------------------------------------------------------------
bool SortSliceCompareFunction(albaDicomSlice *i, albaDicomSlice *j) 
{ 
	//Sort by time and by unrotated Z-value
	if (i->GetTriggerTime() < j->GetTriggerTime())
		return true;
	else if (i->GetTriggerTime() < j->GetTriggerTime())
		return false;
	else
		return (i->GetUnrotatedOrigin()[2] < j->GetUnrotatedOrigin()[2]);
}
//----------------------------------------------------------------------------
void albaDicomSeries::SortSlices()
{
	std::sort(m_Slices.begin(), m_Slices.end(), SortSliceCompareFunction);
}

//----------------------------------------------------------------------------
void albaDicomSlice::GetDicomSpacing(gdcm::DataSet &dcmDataSet, double * dcmPixelSpacing)
{
	//Try to get pixel spacing as spacing used on MR/CT or on calibrated CR
	if (dcmDataSet.FindDataElement(TAG_PixelSpacing))
	{
		gdcm::Attribute ATTRIBUTE_PixelSpacing at;
		at.SetFromDataSet(dcmDataSet);

		dcmPixelSpacing[0] = at.GetValue(0);
		dcmPixelSpacing[1] = at.GetValue(1);
		dcmPixelSpacing[2] = 1.0;
	}
	//Try to get Nominal Scanned pixel spacing as spacing used on SC
	else if (dcmDataSet.FindDataElement(TAG_NominalScannedPixelSpacing))
	{
		gdcm::Attribute ATTRIBUTE_PixelSpacing at;
		at.SetFromDataSet(dcmDataSet);

		dcmPixelSpacing[0] = at.GetValue(0);
		dcmPixelSpacing[1] = at.GetValue(1);
		dcmPixelSpacing[2] = 1.0;
	}
	//Try to get Imager pixel spacing as spacing used on CR/Xray/DX calculated on front plane of the detector
	else if (dcmDataSet.FindDataElement(TAG_ImagerPixelSpacing))
	{
		gdcm::Attribute ATTRIBUTE_ImagerPixelSpacing at;
		at.SetFromDataSet(dcmDataSet);

		dcmPixelSpacing[0] = at.GetValue(0);
		dcmPixelSpacing[1] = at.GetValue(1);
		dcmPixelSpacing[2] = 1.0;
	}
	//Unable to get element: Setting default value
	else
	{
		dcmPixelSpacing[0] = dcmPixelSpacing[1] = dcmPixelSpacing[2] = 1.0;
	}
	
}
//----------------------------------------------------------------------------
vtkImageData* albaDicomSlice::GetNewVTKImageData()
{
	double value, minValue = VTK_INT_MAX, maxValue = VTK_INT_MIN;
	bool hasPadding;
	double dcmSlope, dcmIntercept;
	double dcmPixelSpacing[3];
	
	gdcm::ImageReader dcmReader;
	dcmReader.SetFileName(m_SliceABSFileName.GetCStr());
	if (!dcmReader.Read())
	{
		albaLogMessage("Failed on image reading of file: ", m_SliceABSFileName.GetCStr());
		return NULL;
	}

	const gdcm::Image &dcmImage = dcmReader.GetImage();

	//Getting spacing
	GetDicomSpacing(dcmReader.GetFile().GetDataSet(), dcmPixelSpacing);

	//Getting rescale factors 
	const gdcm::PixelFormat &pixeltype = dcmImage.GetPixelFormat();
	dcmIntercept = dcmImage.GetIntercept();
	dcmSlope = dcmImage.GetSlope();
	
	//Creating rescaler
	gdcm::Rescaler rescaler;
	rescaler.SetIntercept(dcmIntercept);
	rescaler.SetSlope(dcmSlope);
	rescaler.SetPixelFormat(pixeltype);

	//Getting scalarType
	gdcm::PixelFormat::ScalarType dcmScalarType;
	dcmScalarType = rescaler.ComputeInterceptSlopePixelType();
			
	//Creating ImageData
	vtkImageData *imageData;
	vtkNEW(imageData);
	imageData->SetDimensions(m_SliceSize[0], m_SliceSize[1], 1);
	imageData->SetWholeExtent(0, m_SliceSize[0] - 1, 0, m_SliceSize[1] - 1, 0, 0);
	imageData->SetUpdateExtent(0, m_SliceSize[0] - 1, 0, m_SliceSize[1] - 1, 0, 0);
	imageData->SetExtent(imageData->GetUpdateExtent());
	imageData->SetNumberOfScalarComponents(1);
	imageData->SetSpacing(dcmPixelSpacing[0],dcmPixelSpacing[1],dcmPixelSpacing[2]);
	imageData->SetOrigin(m_ImagePositionPatient);

	//Setting Scalars Type and creating scalars and setting their name
	int computeVTKScalarType = ComputeVTKScalarType(dcmScalarType);
	if (computeVTKScalarType==0)
	{
		albaLogMessage("Unsupported pixel scalar format: ", m_SliceABSFileName.GetCStr());
		return NULL;
	}
	imageData->SetScalarType(computeVTKScalarType);
	imageData->AllocateScalars();
	imageData->GetPointData()->GetScalars()->SetName("Scalars");

	char *scalarPointer = (char *)imageData->GetPointData()->GetScalars()->GetVoidPointer(0);

	//Rescaling pixel type if required
	if (dcmSlope != 1.0 || dcmIntercept != 0.0)
	{
		unsigned long len = dcmImage.GetBufferLength();
		char * copy = new char[len];
	
		dcmImage.GetBuffer(copy);
		if (!rescaler.Rescale(scalarPointer, copy, len))
		{
			albaLogMessage("Error Could not Rescale on file %s",m_SliceABSFileName.GetCStr());
			delete[] copy;
			return NULL;
		}
		delete[] copy;
	}
	else
	{
		dcmImage.GetBuffer(scalarPointer);
	}
	
	imageData->Update();
	return imageData;
}

//----------------------------------------------------------------------------
int albaDicomSlice::ComputeVTKScalarType(int scalarType)
{
	switch (scalarType)
	{
		case gdcm::PixelFormat::INT8:
			return VTK_CHAR;
		case gdcm::PixelFormat::UINT8:
			return VTK_UNSIGNED_CHAR;
		case gdcm::PixelFormat::INT16:
			return VTK_SHORT;
		case gdcm::PixelFormat::UINT16:
			return VTK_UNSIGNED_SHORT;
		case gdcm::PixelFormat::INT32:
			return VTK_INT;
		case gdcm::PixelFormat::UINT32:
			return VTK_UNSIGNED_INT;
		case gdcm::PixelFormat::INT12:
			return VTK_SHORT;
		case gdcm::PixelFormat::UINT12:
			return VTK_UNSIGNED_SHORT;
		case gdcm::PixelFormat::FLOAT32:
			return VTK_FLOAT;
		case gdcm::PixelFormat::FLOAT64:
			return VTK_DOUBLE;
// 		case gdcm::PixelFormat::SINGLEBIT:
// 			return VTK_BIT;
		default:
			return 0;
	}
}


/////////////////////////////albaDicomSlice////////////////////////////////
//----------------------------------------------------------------------------
void albaDicomSlice::ComputeUnrotatedOrigin()
{
	double *origin = m_ImagePositionPatient;
	double rotPos[4] = { origin[0], origin[1], origin[2], 1.0 };

	albaMatrix matr;
	matr.SetFromDirectionCosines(m_ImageOrientationPatient);
	matr.Invert();

	double *unRotOrigin = matr.GetVTKMatrix()->MultiplyDoublePoint(rotPos);
	m_UnrotatedOrigin[0] = unRotOrigin[0];
	m_UnrotatedOrigin[1] = unRotOrigin[1];
	m_UnrotatedOrigin[2] = unRotOrigin[2];
}

