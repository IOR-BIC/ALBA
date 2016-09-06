/*=========================================================================

Program: MAF2
Module: mafOpImporterDicomOffis
Authors: Matteo Giacomoni, Roberto Mucci , Stefano Perticoni, Gianluigi Crimi

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <algorithm>

#include "wx/busyinfo.h"
#include "wx/listctrl.h"
#include "wx/dir.h"

#include "mafOpImporterDicomOffis.h"
#include "mafGUIWizardPageNew.h"
#include "mafGUIValidator.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafInteractorDICOMImporter.h"
#include "mafTagArray.h"
#include "mafVMEImage.h"
#include "mafVMEVolumeGray.h"
#include "mafGUICheckListBox.h"
#include "mafGUIDicomSettings.h"
#include "mafGUISettingsAdvanced.h"
#include "mafVMEItemVTK.h"
#include "mafDataVector.h"
#include "mafTransform.h"
#include "mafTransformFrame.h"
#include "mafSmartPointer.h"
#include "mafVMEGroup.h"
#include "mmaMaterial.h"
#include "mafGUILutPreset.h"
#include "vtkImageReslice.h"
#include "mafProgressBarHelper.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPlaneSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkTexture.h"
#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkProperty.h"
#include "vtkOutlineFilter.h"
#include "vtkImageData.h"
#include "vtkPolyData.h"
#include "vtkStructuredPoints.h"
#include "vtkRenderer.h"
#include "vtkMAFRGSliceAccumulate.h"
#include "vtkTransform.h"
#include "vtkImageData.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"
#include "vtkRectilinearGrid.h"
#include "vtkUnsignedShortArray.h"
#include "vtkPointData.h"

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcdebug.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */
#include "dcmtk/dcmdata/dcistrmz.h"    /* for dcmZlibExpectRFC1950Encoding */
#include "dcmtk/dcmimgle/dcmimage.h."
#include "dcmtk/dcmjpeg/djdecode.h."
#include "dcmtk/dcmdata/dcrledrg.h"
#include "dcmtk/ofstd/ofstdinc.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpImporterDicomOffis);

//----------------------------------------------------------------------------
// constants :
enum
{
	TYPE_VOLUME,
	TYPE_IMAGE,
};

#define EPSILON 1e-7

//----------------------------------------------------------------------------
mafOpImporterDicomOffis::mafOpImporterDicomOffis(wxString label):
mafOp(label)
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
	//other pointers
	m_DicomInteractor = NULL;
	m_TagArray = NULL;
	m_StudyList = NULL;
	m_SelectedSeries = NULL;
	//variables
	m_OutputType = TYPE_VOLUME;
	m_ConstantRotation = true;
	m_DescrInName = m_SizeInName = m_PatientNameInName = true;
	m_VMEName = "";
	m_CurrentSlice = VTK_INT_MAX;
	m_TotalDicomRange[0]=0;
	m_TotalDicomRange[1]=1;
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
mafGUIDicomSettings* mafOpImporterDicomOffis::GetSetting()
{
	return (mafGUIDicomSettings*) Superclass::GetSetting();
}
//----------------------------------------------------------------------------
mafOpImporterDicomOffis::~mafOpImporterDicomOffis()
{
	vtkDEL(m_SliceActor);
	mafDEL(m_Output);
}
//----------------------------------------------------------------------------
mafOp *mafOpImporterDicomOffis::Copy()
{
	mafOpImporterDicomOffis *importer = new mafOpImporterDicomOffis(m_Label);
	return importer;
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::OpRun()
{
	m_Wizard = new mafGUIWizard(_("DICOM Importer"));
	m_Wizard->SetListener(this);
	m_Wizard->SetButtonString("Crop >");
		
	wxString lastDicomDir = GetSetting()->GetLastDicomDir();
		
	if (lastDicomDir == "UNEDFINED_m_LastDicomDir")
		lastDicomDir = mafGetLastUserFolder().c_str();		
			
	wxDirDialog dialog(m_Wizard->GetParent(),"", lastDicomDir,wxRESIZE_BORDER, m_Wizard->GetPosition());
	dialog.SetReturnCode(wxID_OK);
	int ret_code = dialog.ShowModal();

	if (ret_code == wxID_OK)
	{
		CreateSliceVTKPipeline(); 
		
		CreateLoadPage();
		CreateCropPage();
		m_LoadPage->SetNextPage(m_CropPage);
		m_Wizard->SetFirstPage(m_LoadPage);

		wxString path = dialog.GetPath();
		GetSetting()->SetLastDicomDir(path);
		GuiUpdate();

		if (OpenDir(path.c_str()))
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
int mafOpImporterDicomOffis::RunWizard()
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
void mafOpImporterDicomOffis::OpDo()
{
	if(m_Output != NULL)
	{
		m_Output->ReparentTo(m_Input);
	}
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::OpStop(int result)
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

	mafDEL(m_TagArray);
	mafDEL(m_DicomInteractor);
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

	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
int mafOpImporterDicomOffis::BuildVMEImagesOutput()
{
	mafVMEGroup       *imagesGroupOuput;
	int nFrames = m_SelectedSeries->GetCardiacImagesNum();

	int step = GetSetting()->GetBuildStep() + 1;
	int cropInterval = (m_ZCropBounds[1]+1 - m_ZCropBounds[0]);
	
	mafProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Building images: please wait...");

	mafNEW(imagesGroupOuput);

	imagesGroupOuput->SetName(wxString::Format("%s images",m_VMEName));
	imagesGroupOuput->ReparentTo(m_Input);

	int parsedSlices = 0;

	//Loop foreach slice
	for (int i = m_ZCropBounds[0]; i < m_ZCropBounds[1]+1; i += step)
	{
		mafString name;
		name.Printf("%s s:%d", m_VMEName.GetCStr(), i);
		mafSmartPointer<mafVMEImage> vmeImage;
		vmeImage->SetName(name);
		vmeImage->GetTagArray()->DeepCopy(m_TagArray);
		vmeImage->GetMaterial()->m_ColorLut->DeepCopy(m_SliceTexture->GetLookupTable());

		//loop foreach time
		for (int t = 0; t < nFrames; t++)
		{
			int sliceID = GetSliceIDInSeries(t, i);
			mafDicomSlice * currentSlice = m_SelectedSeries->GetSlice(sliceID);
			mafTimeStamp triggerTime = currentSlice->GetDcmTriggerTime();

			vtkImageData *image = currentSlice->GetVTKImageData();
			Crop(image);
			image->SetOrigin(currentSlice->GetUnrotatedOrigin());

			//Set data at specific time
			vmeImage->SetData(image, triggerTime);
		}
		
		//Setting orientation matrix
		mafDicomSlice *sliceAtZeroTime = m_SelectedSeries->GetSlice(GetSliceIDInSeries(0, i));
		mafMatrix orientationMatrix;
		orientationMatrix.SetFromDirectionCosines(sliceAtZeroTime->GetDcmImageOrientationPatient());
		vmeImage->SetAbsMatrix(orientationMatrix);

		imagesGroupOuput->AddChild(vmeImage);

		progressHelper.UpdateProgressBar(i * 100 / m_SelectedSeries->GetSlicesNum());
	}

	m_Output = imagesGroupOuput;

	return OP_RUN_OK;
}

//----------------------------------------------------------------------------
int mafOpImporterDicomOffis::BuildVMEVolumeGrayOutput()
{
	mafVMEVolumeGray	*VolumeOut;
	mafDicomSlice* firstSlice = m_SelectedSeries->GetSlice(0);
	
	int nFrames = m_SelectedSeries->GetCardiacImagesNum();
	int step = m_TestMode ? 1 : GetSetting()->GetBuildStep() + 1;
	int cropInterval = (m_ZCropBounds[1]+1 - m_ZCropBounds[0]);
	int SlicesPerFrame = (cropInterval / step);
	
	if(cropInterval % step != 0)
		SlicesPerFrame+=1;

	int totalNumberOfImages = SlicesPerFrame*nFrames;
	int parsedSlices = 0;

	mafProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Building volume: please wait...");

	mafNEW(VolumeOut);
	
	//Loop foreach time 
	for (int t = 0; t < nFrames; t++)
	{
		int firstSliceAtTimeID = GetSliceIDInSeries(t, 0);
		mafTimeStamp triggerTime = m_SelectedSeries->GetSlice(firstSliceAtTimeID)->GetDcmTriggerTime();

		vtkMAFSmartPointer<vtkMAFRGSliceAccumulate> accumulate;
		accumulate->SetNumberOfSlices(SlicesPerFrame);

		int accumSliceN = 0;
		//Loop foreach slice
		for (int i = m_ZCropBounds[0], s_id=0; i < m_ZCropBounds[1] + 1; i += step)
		{
			int sliceID = GetSliceIDInSeries(t, i);
			parsedSlices++;

			mafDicomSlice * slice = m_SelectedSeries->GetSlice(sliceID);
			vtkImageData *image = slice->GetVTKImageData();
			Crop(image);

			accumulate->SetSlice(accumSliceN, image, slice->GetUnrotatedOrigin());

			progressHelper.UpdateProgressBar(parsedSlices * 100 / totalNumberOfImages);
			accumSliceN++;
		}
		accumulate->Update();

		vtkRectilinearGrid *rg_out;
		rg_out = accumulate->GetOutput();
		rg_out->Update();

		//Set data at specific time
		VolumeOut->SetDataByDetaching(rg_out, triggerTime);
	}

	//Setting orientation matrix
	mafMatrix orientationMatrix;
	orientationMatrix.SetFromDirectionCosines(firstSlice->GetDcmImageOrientationPatient());
	VolumeOut->SetAbsMatrix(orientationMatrix);
		
	//Copy Dicom's tags inside the VME 
	VolumeOut->GetTagArray()->DeepCopy(m_TagArray);
	VolumeOut->SetName(m_VMEName);  
	m_Output = VolumeOut;

	return OP_RUN_OK;
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::CreateLoadPage()
{
	m_LoadPage = new mafGUIWizardPageNew(m_Wizard,medUSEGUI|medUSERWI);
	m_LoadGuiLeft = new mafGUI(this);
	m_LoadGuiUnderLeft = new mafGUI(this);
	m_LoadGuiUnderCenter = new mafGUI(this);
	m_LoadGuiCenter = new mafGUI(this);

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
void mafOpImporterDicomOffis::CreateCropPage()
{
	m_CropPage = new mafGUIWizardPageNew(m_Wizard,medUSEGUI|medUSERWI,true);
	m_CropPage->SetListener(this);
	m_CropGuiLeft = new mafGUI(this);
	m_CropGuiCenter = new mafGUI(this);

	m_CropGuiLeft->FitGui();
	m_CropGuiCenter->FitGui();
	m_CropPage->AddGuiLowerLeft(m_CropGuiLeft);
	m_CropPage->AddGuiLowerCenter(m_CropGuiCenter);

	m_CropPage->GetRWI()->m_RwiBase->SetMouse(m_Mouse);
	m_CropPage->GetRWI()->m_RenFront->AddActor(m_SliceActor);
	m_DicomInteractor->SetRWI(m_CropPage->GetRWI());
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::GuiUpdate()
{
	m_LoadGuiLeft->Update();
	m_LoadGuiUnderLeft->Update();
	m_LoadGuiCenter->Update();
	m_CropGuiLeft->Update();
	m_CropGuiCenter->Update();
}
//----------------------------------------------------------------------------
bool mafOpImporterDicomOffis::OpenDir(const char *dirPath)
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
		CameraReset();
		OnStudySelect();
	}
	else 
		SelectSeries(m_StudyList->GetStudy(0)->GetSeries(0));

	return true;
}

//----------------------------------------------------------------------------
vtkImageData * mafOpImporterDicomOffis::GetSliceInCurrentSeries(int id)
{
	return	m_SelectedSeries->GetSlice(id)->GetVTKImageData();
}

//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::OnEvent(mafEventBase *maf_event) 
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch (e->GetId())
		{
			case ID_RANGE_MODIFIED:
				OnRangeModified();
			break;
			case mafGUIWizard::MED_WIZARD_CHANGE_PAGE:
				OnWizardChangePage(e);
			break;
			case mafGUIWizard::MED_WIZARD_CHANGED_PAGE:
				/* This is a ack, because that "genius" of wx  send the change event
				before page show, so we need to duplicate the code here in order to
				manage the camera update */
				m_Wizard->GetCurrentPage()->Show();
				m_Wizard->GetCurrentPage()->SetFocus();
				m_Wizard->GetCurrentPage()->Update();
				CameraReset();
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
				mafEventMacro(*e);
		}
	}
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::SetVMEName()
{
	mafDicomSlice * sliceData = m_SelectedSeries->GetSlice(0);
	m_VMEName = sliceData->GetDcmModality();
	if (sliceData->GetDescription() != "" && m_DescrInName)
	{
		m_VMEName += " ";
		m_VMEName += sliceData->GetDescription();
	}
	if (sliceData->GetPatientName() != "" && m_PatientNameInName)
	{
		mafString patientName = sliceData->GetPatientName();
		patientName.Replace('^', ' ');
		m_VMEName += " ";
		m_VMEName += patientName;
	}
	if (m_SizeInName)
	{
		mafString size;
		int *dims=sliceData->GetVTKImageData()->GetDimensions();
		if (sliceData->GetDcmCardiacNumberOfImages() > 1)
			size.Printf(" %dx%dx%d f:%d", dims[0], dims[1], m_SelectedSeries->GetSlicesNum(), sliceData->GetDcmCardiacNumberOfImages());
		else
			size.Printf(" %dx%dx%d", dims[0], dims[1], m_SelectedSeries->GetSlicesNum());
		m_VMEName += size;
	}

	if(m_CropGuiCenter)
		m_CropGuiCenter->Update();
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::CameraUpdate()
{
	if(m_Wizard->GetCurrentPage() == m_LoadPage)
	{
		m_LoadPage->UpdateActor();
		m_LoadPage->GetRWI()->CameraUpdate();
	}
	else
	{
		m_CropPage->UpdateActor();
		m_CropPage->GetRWI()->CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::CameraReset()
{
	if (m_LoadPage && m_Wizard->GetCurrentPage() == m_LoadPage)
	{
		m_LoadPage->UpdateWindowing(m_TotalDicomRange, m_TotalDicomSubRange);
		m_LoadPage->GetRWI()->CameraReset();
	}
	else if(m_CropPage)
	{
		m_CropPage->UpdateWindowing(m_TotalDicomRange, m_TotalDicomSubRange);
		m_CropPage->GetRWI()->CameraReset();
	}
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::CreateSliceVTKPipeline()
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
		mafNEW(m_DicomInteractor);
		m_DicomInteractor->SetListener(this);
		m_Mouse->AddObserver(m_DicomInteractor, MCH_INPUT);
	}
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::FillStudyListBox()
{
	mafString studyName;
	for (int n = 0; n < m_StudyList->GetStudiesNum(); n++)
	{
		studyName.Printf("Study %d", n);
		m_StudyListbox->Append(studyName.GetCStr());
	}
	m_StudyListbox->SetSelection(0);
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::FillSeriesListBox()
{
	int counter = 0;
	m_SeriesListbox->Clear();

	mafDicomStudy *study = m_StudyList->GetStudy(m_SelectedStudy);
	mafString seriesName;

	for (int i = 0; i < study->GetSeriesNum(); i++)
	{
		mafDicomSeries *series = study->GetSeries(i);

		const int *dim=series->GetDimensions();
		int framesNum = series->GetCardiacImagesNum();
		
		if (framesNum > 1)
			seriesName.Printf("Series %dx%dx%d f%d", dim[0], dim[1], framesNum / series->GetSlicesNum(), framesNum);
		else
			seriesName.Printf("Series %dx%dx%d", dim[0], dim[1], series->GetSlicesNum());

		m_SeriesListbox->Append(seriesName.GetCStr());
	}
}
//----------------------------------------------------------------------------
bool mafOpImporterDicomOffis::LoadDicomFromDir(const char *dicomDirABSPath)
{   
	wxDir dir;
	wxArrayString allFiles;

	//Reading file list in folder and sub folders
	dir.GetAllFiles(dicomDirABSPath, &allFiles, wxEmptyString, wxDIR_FILES | wxDIR_DIRS);
	int fileNumber = allFiles.size();

	mafProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Reading DICOM directory: please wait...");

	m_StudyList = new mafDicomStudyList();
		
	//Register Codecs before reading Dicom dir
	DJDecoderRegistration::registerCodecs(); // register JPEG Codecs
	DcmRLEDecoderRegistration::registerCodecs(OFFalse, OFFalse, OFFalse); // register RLE Codecs

	for (int i = 0; i < fileNumber; i++)
	{
		mafDicomSlice *slice= ReadDicomSlice((allFiles[i]).c_str());
		if (slice)
			m_StudyList->AddSlice(slice);
		progressHelper.UpdateProgressBar( i*100 / fileNumber);
	}

	// unregister JPEG Codecs
	DJDecoderRegistration::cleanup(); 
	DcmRLEDecoderRegistration::cleanup();

	progressHelper.CloseProgressBar();
	
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

	mafLogMessage("Found %d Dicom series", m_StudyList->GetSeriesTotalNum());

	return m_StudyList->GetStudiesNum()>0;
}
//----------------------------------------------------------------------------
mafDicomSlice *mafOpImporterDicomOffis::ReadDicomSlice(mafString fileName)
{
	double lastZPos = 0;
	long int dcmInstanceNumber = -1;
	long int dcmCardiacNumberOfImages = -1;
	const char *dcmModality, *dcmStudyInstanceUID, *dcmSeriesInstanceUID, *dcmScanOptions;
	const char *date, *description, *patientName, *birthdate, *photometricInterpretation;
	double dcmTriggerTime = -1.0;
	double defaulOrienatation[6] = { 1.0,0.0,0.0,0.0,1.0,1.0 };
	double dcmImageOrientationPatient[6];
	double dcmImagePositionPatient[3] = {0.0,0.0,0.0};
	DcmFileFormat dicomImg;   
	
	//Load data into Offis structure
	OFCondition status = dicomImg.loadFile(fileName);
	if (!status.good())
	{
		mafLogMessage("File <%s> can not be opened",fileName);
		return NULL;
	}

	//Obtain dataset information from Dicom file (loaded into memory)
	DcmDataset *dicomDataset = dicomImg.getDataset();

	//Decompress data set if compressed
	OFCondition error = dicomDataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
	if (!error.good())
	{
		mafLogMessage("Error decoding the image <%s>",fileName);
		return NULL;
	}

	dicomDataset->findAndGetString(DCM_ScanOptions,dcmScanOptions);
	dicomDataset->findAndGetString(DCM_Modality, dcmModality);

	if (wxString(dcmScanOptions).Find("SCOUT") != -1 || wxString(dcmModality).Find("SCOUT") != -1)
	{
		mafLogMessage("Error decoding the image <%s>", fileName);
		return NULL;
	}

	//Read Study-Series IDs
	dicomDataset->findAndGetString(DCM_StudyInstanceUID, dcmStudyInstanceUID);
	dicomDataset->findAndGetString(DCM_SeriesInstanceUID, dcmSeriesInstanceUID);
		
	//Try to read image position patient form Dicom
	if (dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient, dcmImagePositionPatient[0], 0).bad())
	{
		if (GetSetting()->GetDCMImagePositionPatientExceptionHandling() == mafGUIDicomSettings::APPLY_DEFAULT_POSITION)
		{
			mafLogMessage("Cannot read Dicom tag ImagePositionPatient on %s\nUse default position.", fileName.GetCStr());
		}
		else // mafGUIDicomSettings::SKIP_ALL 
		{
			mafLogMessage("Cannot read Dicom tag ImagePositionPatient on %s\nSkip Slice.", fileName.GetCStr());
			return NULL;
		}
	}
	else
	{
		//Read the rest of the vector
		dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient, dcmImagePositionPatient[1], 1);
		dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient, dcmImagePositionPatient[2], 2);
	}

	//Read Image Orientation Patient
	for (int i = 0; i < 6; i++)
		if (dicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient, dcmImageOrientationPatient[i], i).bad())
			dcmImageOrientationPatient[i] = defaulOrienatation[i];

	
	//Read Cine MR related stuff
	dicomDataset->findAndGetLongInt(DCM_InstanceNumber,dcmInstanceNumber);
	dicomDataset->findAndGetFloat64(DCM_TriggerTime,dcmTriggerTime);
	if (dicomDataset->findAndGetLongInt(DCM_CardiacNumberOfImages, dcmCardiacNumberOfImages).bad())
		dcmCardiacNumberOfImages = 1;
	
	//Read slice 
	dicomDataset->findAndGetString(DCM_PatientsBirthDate, birthdate);
	dicomDataset->findAndGetString(DCM_StudyDate, date);
	dicomDataset->findAndGetString(DCM_SeriesDescription, description);
	dicomDataset->findAndGetString(DCM_PatientsName, patientName);
	dicomDataset->findAndGetString(DCM_PhotometricInterpretation, photometricInterpretation);

	//Read image data
	vtkImageData *dicomSliceVTKImageData = CreateImageData(dicomDataset, dcmImagePositionPatient);
	if(dicomSliceVTKImageData == NULL)
	{
		mafLogMessage("Cannot read Dicom Image on %s\nSkip Slice.", fileName.GetCStr());
		return NULL;
	}
			
	//Create Slice
	mafDicomSlice *newSlice = new mafDicomSlice(fileName, dcmImageOrientationPatient, dicomSliceVTKImageData, description, date, patientName, birthdate, dcmInstanceNumber, dcmCardiacNumberOfImages, dcmTriggerTime);
	newSlice->SetDcmModality(dcmModality);
	newSlice->SetPhotometricInterpretation(photometricInterpretation);
	newSlice->SetSeriesID(dcmSeriesInstanceUID);
	newSlice->SetStudyID(dcmStudyInstanceUID);
		
	return newSlice;
}

#define VALUE_FROM_BUFFERS(pixelRep, ucharBuf, ushortBuf, id) (pixelRep ?  (ucharBuf ? (Sint8)ucharBuf[id] : (Sint16)ushortBuf[id]) : (ucharBuf ? ucharBuf[id] : ushortBuf[id]))

//----------------------------------------------------------------------------
vtkImageData * mafOpImporterDicomOffis::CreateImageData(DcmDataset * dicomDataset, double * dcmImagePositionPatient)
{
	long dcmColumns, dcmRows, dcmBitPerPixel, nPixel, dcmPixelRepresentation, dcmPaddingValue;
	double value, minValue = VTK_INT_MAX, maxValue = VTK_INT_MIN;
	bool hasPadding;
	double dcmRescaleSlope, dcmRescaleIntercept;
	double dcmPixelSpacing[3];
	const Uint16 *uShortBuffer = NULL;
	const Uint8 *uCharBuffer = NULL;
	
	//getting image size
	dicomDataset->findAndGetLongInt(DCM_Rows, dcmRows);
	dicomDataset->findAndGetLongInt(DCM_Columns, dcmColumns);
	nPixel = dcmRows*dcmColumns;

	if (nPixel == 0)
		return NULL;

	//Getting pixel type
	dicomDataset->findAndGetLongInt(DCM_PixelRepresentation, dcmPixelRepresentation);
	dicomDataset->findAndGetLongInt(DCM_BitsAllocated, dcmBitPerPixel);

	//getting spacing
	GetDicomSpacing(dicomDataset, dcmPixelSpacing);
	dcmPixelSpacing[2] = 1;

	//Getting rescale factors, if the tag is not present s
	if (dicomDataset->findAndGetFloat64(DCM_RescaleSlope, dcmRescaleSlope).bad())
		dcmRescaleSlope = 1.0;
	if (dicomDataset->findAndGetFloat64(DCM_RescaleIntercept, dcmRescaleIntercept).bad())
		dcmRescaleIntercept = 0.0;

	if (dicomDataset->findAndGetLongInt(DCM_PixelPaddingValue, dcmPaddingValue).bad())
		hasPadding = false;
	else
		hasPadding = true;

	if (dcmBitPerPixel == 16)
		dicomDataset->findAndGetUint16Array(DCM_PixelData, uShortBuffer);
	else
		dicomDataset->findAndGetUint8Array(DCM_PixelData, uCharBuffer);
				
	//getting min and max
	for (int i = 0; i < nPixel; i++)
	{
		value = VALUE_FROM_BUFFERS(dcmPixelRepresentation, uCharBuffer, uShortBuffer , i);
		minValue = MIN(minValue, value);
		maxValue = MAX(maxValue, value);
	}
	//Rescaling min and max
	minValue = minValue*dcmRescaleSlope + dcmRescaleIntercept;
	maxValue = maxValue*dcmRescaleSlope + dcmRescaleIntercept;

	//Creating ImageData
	vtkImageData *imageData;
	vtkNEW(imageData);
	imageData->SetDimensions(dcmRows, dcmColumns, 1);
	imageData->SetWholeExtent(0, dcmColumns - 1, 0, dcmRows - 1, 0, 0);
	imageData->SetUpdateExtent(0, dcmColumns - 1, 0, dcmRows - 1, 0, 0);
	imageData->SetExtent(imageData->GetUpdateExtent());
	imageData->SetNumberOfScalarComponents(1);
	imageData->SetSpacing(dcmPixelSpacing);
	imageData->SetOrigin(dcmImagePositionPatient);

	//Setting Scalars Type and name
	if (dcmBitPerPixel == 8 && minValue >= VTK_UNSIGNED_CHAR_MIN && maxValue <= VTK_UNSIGNED_CHAR_MAX)
		imageData->SetScalarType(VTK_UNSIGNED_CHAR);
	else if (dcmBitPerPixel == 8 && minValue >= VTK_CHAR_MIN && maxValue <= VTK_CHAR_MAX)
		imageData->SetScalarType(VTK_CHAR);
	else if (minValue >= VTK_UNSIGNED_SHORT_MIN && maxValue <= VTK_UNSIGNED_SHORT_MAX)
		imageData->SetScalarType(VTK_UNSIGNED_SHORT);
	else if (minValue >= VTK_SHORT_MIN && maxValue <= VTK_SHORT_MAX)
		imageData->SetScalarType(VTK_SHORT);
	else
		imageData->SetScalarType(VTK_FLOAT);

	imageData->AllocateScalars();
	imageData->GetPointData()->GetScalars()->SetName("Scalars");
	
	vtkDataArray *scalars = imageData->GetPointData()->GetScalars();


	if (dcmBitPerPixel == 16)
	{
		for (int i = 0; i < nPixel; i++)
		{
			value = VALUE_FROM_BUFFERS(dcmPixelRepresentation,uCharBuffer,uShortBuffer, i);
			
			if (hasPadding && value == dcmPaddingValue)
				value = minValue; //using min as padding value
			else
				value = value*dcmRescaleSlope + dcmRescaleIntercept;

			scalars->SetTuple(i, &value);
		}
	}

	imageData->Update();

	return imageData;
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::GetDicomSpacing(DcmDataset * dicomDataset, double * dcmPixelSpacing)
{
	for (int i = 0; i < 2; i++)
	{
		//Try to get pixel spacing as spacing used on MR/CT or on calibrated CR
		if (!dicomDataset->findAndGetFloat64(DCM_PixelSpacing, dcmPixelSpacing[i], i).bad())
			continue;

		//Try to get Nominal Scanned pixel spacing as spacing used on SC
		if (!dicomDataset->findAndGetFloat64(DCM_NominalScannedPixelSpacing, dcmPixelSpacing[i], i).bad())
			continue;

		//Try to get Imager pixel spacing as spacing used on CR/Xray/DX calculated on front plane of the detector
		if (!dicomDataset->findAndGetFloat64(DCM_ImagerPixelSpacing, dcmPixelSpacing[i], i).bad())
			continue;

		//Try to get Detector Element spacing, this values should not be used for set image spacing, 
		//but if we does not have any of the previous values this is better than a default value
		if (!dicomDataset->findAndGetFloat64(DCM_DetectorElementSpacing, dcmPixelSpacing[i], i).bad())
			continue;
						
		//Unable to get element: Setting default value
		dcmPixelSpacing[i] = 1.0;
	}
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::CreateSliders()
{
	int numOfSlices = m_SelectedSeries->GetSlicesNum();
	int cardiacImageNum = m_SelectedSeries->GetCardiacImagesNum();
	m_CurrentSlice = 0;
	if(m_LoadGuiLeft)
	{
		m_LoadPage->RemoveGuiLowerLeft(m_LoadGuiLeft);
		delete m_LoadGuiLeft;
		m_LoadGuiLeft = new mafGUI(this);
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
		m_CropGuiLeft = new mafGUI(this);
		m_SliceScannerCropPage=m_CropGuiLeft->Slider(ID_SCAN_SLICE,_("Slice #"),&m_CurrentSlice,0, numOfSlices -1,"",true);
		m_CropGuiLeft->Enable(ID_SCAN_SLICE, numOfSlices > 1);
		if (cardiacImageNum > 1)
			m_CropGuiLeft->Slider(ID_SCAN_TIME, _("Time "), &m_CurrentTime, 0, cardiacImageNum);
		m_CropPage->AddGuiLowerLeft(m_CropGuiLeft);
		m_CropPage->Update();
	}
}
//----------------------------------------------------------------------------
int mafOpImporterDicomOffis::GetSliceIDInSeries(int timeId, int heigthId)
{
	mafDicomSlice *firstDicomListElement;
	firstDicomListElement = (mafDicomSlice *)m_SelectedSeries->GetSlice(0);
	int timeFrames =  firstDicomListElement->GetDcmCardiacNumberOfImages();

	int dicomFilesNumber = m_SelectedSeries->GetSlicesNum();
	int numSlicesPerTS = dicomFilesNumber / timeFrames;
	return timeId*numSlicesPerTS + heigthId;
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::GenerateSliceTexture(int imageID)
{
	double  range[2];
	mafString text;

	mafDicomSlice* slice = m_SelectedSeries->GetSlice(imageID);
	assert(slice);

	slice->GetVTKImageData()->Update();
	slice->GetVTKImageData()->GetBounds(m_SliceBounds);
	
	double origin[3], orientation[6];
	slice->GetVTKImageData()->GetOrigin(origin);
	slice->GetDcmImageOrientationPatient(orientation);

	text.Printf("Orientation: %f, %f, %f, %f, %f, %f \nPosition: %f, %f, %f",orientation[0], orientation[1], orientation[2], orientation[3], orientation[4], orientation[5], origin[0], origin[1], origin[2]);
	m_TextMapper->SetInput(text.GetCStr());
	m_TextMapper->Modified();

	slice->GetVTKImageData()->GetScalarRange(range);
	m_SliceTexture->SetInput(slice->GetVTKImageData());
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
void mafOpImporterDicomOffis::Crop(vtkImageData *slice)
{
	if (m_CropEnabled)
	{
		slice->SetUpdateExtent(m_CropExtent);
		slice->Crop();
	}
}

//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::CalculateCropExtent()
{
	double spacing[3], crop_bounds[6], sliceOrigin[3];
	int sliceExtent[6];

	vtkImageData* slice = m_SelectedSeries->GetSlice(0)->GetVTKImageData();
	slice->GetExtent(sliceExtent);
	slice->GetOrigin(sliceOrigin);
	slice->GetSpacing(spacing);

	if (m_DicomInteractor)
		m_DicomInteractor->GetPlaneBounds(crop_bounds);
	else
		slice->GetBounds(crop_bounds);

	//Align cropBounds to current grid
	m_CropExtent[0] = round(crop_bounds[0] / spacing[0]);
	m_CropExtent[1] = round(crop_bounds[1] / spacing[0]);
	m_CropExtent[2] = round(crop_bounds[2] / spacing[1]);
	m_CropExtent[3] = round(crop_bounds[3] / spacing[1]);
	m_CropExtent[4] = m_CropExtent[5] = 0;
		
	//Enable/disable Crop
	m_CropEnabled = false;
	for (int i = 0; i < 4; i++)
		if (sliceExtent[i] != m_CropExtent[i])
			m_CropEnabled = true;
}

//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::SetPlaneDims()
{	
	double diffx,diffy;
	diffx=m_SliceBounds[1]-m_SliceBounds[0];
	diffy=m_SliceBounds[3]-m_SliceBounds[2];

	m_SlicePlane->SetOrigin(0,0,0);
	m_SlicePlane->SetPoint1(diffx,0,0);
	m_SlicePlane->SetPoint2(0,diffy,0);
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::ImportDicomTags()
{
	if (m_TagArray == NULL) 
		mafNEW(m_TagArray);

	m_TagArray->SetName("TagArray");

	DcmFileFormat dicomImg;  
	DJDecoderRegistration::registerCodecs(); // register JPEG codecs
	DcmRLEDecoderRegistration::registerCodecs();
	const char *sliceABSFileName = m_SelectedSeries->GetSlice(0)->GetSliceABSFileName();
	OFCondition status = dicomImg.loadFile(sliceABSFileName);//load data into offis structure

	if (!status.good()) 
	{
		if(!this->m_TestMode)
		{
			mafLogMessage(wxString::Format("File <%s> can not be opened", sliceABSFileName),"Warning!!");
		}
		return;
	}
	DcmDataset *ds = dicomImg.getDataset();//obtain dataset information from dicom file (loaded into memory)

	// decompress data set if compressed
	ds->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
	DJDecoderRegistration::cleanup(); // deregister JPEG codecs
	DcmRLEDecoderRegistration::cleanup();
	OFString string;
	DcmStack stack;
	DcmObject *dobject = NULL;
	DcmElement *delem = NULL;
	status = ds->nextObject(stack, OFTrue);
	try
	{
		while (status.good())
		{
			dobject = stack.top();
			if (dobject->verify().good())
			{ 
				DcmTag tag = dobject->getTag();
				wxString tagName = tag.getTagName();

				delem = dynamic_cast<DcmElement*>(dobject);
				if (delem != NULL)
				{
					delem->getOFStringArray(string);

					if (tagName.compare("PixelData") != 0)
						m_TagArray->SetTag(mafTagItem(tagName.c_str(),string.c_str()));
				}
			}
			status = ds->nextObject(stack, OFTrue);
		}		
	} 
	catch (...)
	{
		mafLogMessage("Can not read Dicom tag.");
	}
		
	m_TagArray->SetTag(mafTagItem("VME_NATURE", "NATURAL"));
	
	dicomImg.clear();
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::OnStudySelect()
{
	if (m_SelectedStudy != m_StudyListbox->GetSelection())
	{
		m_SelectedStudy = m_StudyListbox->GetSelection();
		FillSeriesListBox();
		m_SeriesListbox->Select(0);
		SelectSeries(m_StudyList->GetStudy(m_SelectedStudy)->GetSeries(0));
	}
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::GetDicomRange(double *range)
{
	double sliceRange[2];

	range[0]=MAXDOUBLE;
	range[1]=MINDOUBLE;

	for(int imageID=0;imageID<m_SelectedSeries->GetSlicesNum();imageID++)
	{  
		mafDicomSlice* slice = NULL;

		slice = m_SelectedSeries->GetSlice(imageID);
		assert(slice);

		slice->GetVTKImageData()->Update();
		slice->GetVTKImageData()->GetScalarRange(sliceRange);

		if (sliceRange[0]<range[0]) range[0]=sliceRange[0];
		if (sliceRange[1]>range[1]) range[1]=sliceRange[1];
	}
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::SelectSeries(mafDicomSeries * selectedSeries)
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

			m_LoadPage->RemoveGuiLowerCenter(m_LoadGuiCenter);
			m_LoadGuiCenter = new mafGUI(this);
			m_LoadGuiCenter->Divider();

			if (numberOfSlices > 1 && !GetSetting()->AutoVMEType())
			{
				wxString typeArrayVolumeImage[2] = { _("Volume"),_("Images") };
				m_LoadGuiCenter->Radio(ID_VME_TYPE, "VME output", &m_OutputType, 2, typeArrayVolumeImage, 1, "");
			}
			else if (numberOfSlices == 1 || GetSetting()->GetVMEType() == TYPE_IMAGE)
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
			if (!this->m_TestMode)
				m_CropPage->SetZCropBounds(m_ZCropBounds[0], m_ZCropBounds[1]);
		}

		GetDicomRange(m_TotalDicomRange);
		m_TotalDicomSubRange[0] = m_TotalDicomRange[0];
		m_TotalDicomSubRange[1] = m_TotalDicomRange[1];

		GenerateSliceTexture(0);
		SetPlaneDims();
		CameraReset();
		
		cppDEL(busyCursor);
	}
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::OnWizardChangePage( mafEvent * e )
{

	if(m_Wizard->GetCurrentPage()==m_LoadPage)//From Load page
	{
		//get the current windowing in order to maintain subrange thought the wizard pages 
		m_LoadPage->GetWindowing(m_TotalDicomRange,m_TotalDicomSubRange);

		m_DicomInteractor->SetSliceBounds(m_SliceBounds);
		m_DicomInteractor->PlaneVisibilityOn();

		m_CropPage->RemoveGuiLowerCenter(m_CropGuiCenter);
		m_CropGuiCenter = new mafGUI(this);
		m_CropGuiCenter->Divider();

		m_CropGuiCenter->Label("Name:", true);

		mafDicomSlice * sliceData = m_SelectedSeries->GetSlice(0);

		if (sliceData->GetDescription() != "")
			m_CropGuiCenter->Bool(ID_UPDATE_NAME, "Description", &m_DescrInName,1);
		if (sliceData->GetPatientName() != "")
			m_CropGuiCenter->Bool(ID_UPDATE_NAME, "Patient Name", &m_PatientNameInName,1);
		m_CropGuiCenter->Bool(ID_UPDATE_NAME, "Image Size", &m_SizeInName,1);

		m_CropGuiCenter->Label("Result:");
		m_CropGuiCenter->Label(&m_VMEName);

		m_CropPage->AddGuiLowerCenter(m_CropGuiCenter);
		m_CropPage->Update();
		SetVMEName();
	}

	if (m_Wizard->GetCurrentPage() == m_CropPage)//From Crop page to build page
	{
		//get the current windowing in order to maintain subrange thought the wizard pages 
		m_CropPage->GetWindowing(m_TotalDicomRange, m_TotalDicomSubRange);
		m_DicomInteractor->PlaneVisibilityOff();
		m_Wizard->SetButtonString("Crop >");
	}
	
	CameraReset();
	GuiUpdate();
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::OnRangeModified()
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
void mafOpImporterDicomOffis::OnChangeSlice()
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

	m_SliceScannerLoadPage->SetValue(m_CurrentSlice);
	m_SliceScannerLoadPage->Update();
	m_SliceScannerCropPage->SetValue(m_CurrentSlice);
	m_SliceScannerCropPage->Update();

	CameraUpdate();
	GuiUpdate();
}

/////////////////////////////mafDicomStudyList////////////////////////////////
//----------------------------------------------------------------------------
mafDicomStudyList::~mafDicomStudyList()
{
		for (int i = 0; i < m_Studies.size(); i++)
		cppDEL(m_Studies[i]);
}
//----------------------------------------------------------------------------
void mafDicomStudyList::AddSlice(mafDicomSlice *slice)
{
	mafDicomStudy *study=NULL;

	mafString studyID = slice->GetStudyID();
	for (int i = 0; i < m_Studies.size() && !study; i++)
		if (studyID == m_Studies[i]->GetStudyID())
		{
			study = m_Studies[i];
			break;
		}

	//if the study does not exist we create a new study and push it back on study vector
	if (study == NULL)
	{
		study = new mafDicomStudy(slice->GetStudyID());
		m_Studies.push_back(study);
	}

	study->AddSlice(slice);
}
//----------------------------------------------------------------------------
mafDicomStudy * mafDicomStudyList::GetStudy(int num)
{
	return m_Studies[num];
}
//----------------------------------------------------------------------------
int mafDicomStudyList::GetSeriesTotalNum()
{
	int total = 0;
	for (int i = 0; i < m_Studies.size(); i++)
		total += m_Studies[i]->GetSeriesNum();
	
	return total;
}

///////////////////////////////mafDicomStudy//////////////////////////////////
//----------------------------------------------------------------------------
mafDicomStudy::~mafDicomStudy()
{
	for (int i = 0; i < m_Series.size(); i++)
		cppDEL(m_Series[i]);
}
//----------------------------------------------------------------------------
void mafDicomStudy::AddSlice(mafDicomSlice *slice)
{
	mafDicomSeries *series = NULL;

	mafString serieID = slice->GetSeriesID();
	for (int i = 0; i < m_Series.size() && !series; i++)
		if (serieID == m_Series[i]->GetSerieID())
		{
			series = m_Series[i];
			break;
		}

	//if the study does not exist we create a new study and push it back on study vector
	if (series == NULL)
	{
		series = new mafDicomSeries(slice->GetSeriesID());
		m_Series.push_back(series);
	}

	series->AddSlice(slice);
}

///////////////////////////////mafDicomSeries//////////////////////////////////
//----------------------------------------------------------------------------
mafDicomSeries::~mafDicomSeries()
{
	for (int i = 0; i < m_Slices.size(); i++)
		cppDEL(m_Slices[i]);
}
//----------------------------------------------------------------------------
void mafDicomSeries::AddSlice(mafDicomSlice *slice)
{
	if (m_Slices.size() == 0)
	{
		slice->GetVTKImageData()->GetDimensions(m_Dimensions);
		m_CardiacImagesNum = slice->GetDcmCardiacNumberOfImages();
	}
	else
	{
		int *dim = slice->GetVTKImageData()->GetDimensions();
		
		//Check dimension
		if ( (dim[0] != m_Dimensions[0]) && (dim[1] != m_Dimensions[1]) )
		{
			mafLogMessage("Image :%s\nhave different size than other images in the same series and will be skipped", slice->GetSliceABSFileName());
			return;
		}
	}
	
	if (IsRotated(slice->GetDcmImageOrientationPatient()))
		m_IsRotated = true;

	m_Slices.push_back(slice);
}
//----------------------------------------------------------------------------
bool mafDicomSeries::IsRotated(const double dcmImageOrientationPatient[6])
{
	// check if the dataset is rotated: => different from 1. 0. 0. 0. 1. 0.
	return !( fabs(dcmImageOrientationPatient[0] - 1.0) < EPSILON && fabs(dcmImageOrientationPatient[1]) < EPSILON &&
						fabs(dcmImageOrientationPatient[2])				< EPSILON && fabs(dcmImageOrientationPatient[3]) < EPSILON &&
						fabs(dcmImageOrientationPatient[4] - 1.0) < EPSILON && fabs(dcmImageOrientationPatient[5]) < EPSILON);
}
//----------------------------------------------------------------------------
bool SortSliceCompareFunction(mafDicomSlice *i, mafDicomSlice *j) 
{ 
	//Sort by time and by unrotated Z-value
	if (i->GetDcmTriggerTime() < j->GetDcmTriggerTime())
		return true;
	else if (i->GetDcmTriggerTime() < j->GetDcmTriggerTime())
		return false;
	else
		return (i->GetUnrotatedOrigin()[2] < j->GetUnrotatedOrigin()[2]);
}
//----------------------------------------------------------------------------
void mafDicomSeries::SortSlices()
{
	std::sort(m_Slices.begin(), m_Slices.end(), SortSliceCompareFunction);
}

/////////////////////////////mafDicomSlice////////////////////////////////
//----------------------------------------------------------------------------
void mafDicomSlice::CalculateUnrotatedOrigin()
{
	double *origin = m_ImageData->GetOrigin();
	double rotPos[4] = { origin[0], origin[1], origin[2], 1.0 };

	mafMatrix matr;
	matr.SetFromDirectionCosines(m_DcmImageOrientationPatient);
	matr.Invert();

	double *unRotOrigin = matr.GetVTKMatrix()->MultiplyDoublePoint(rotPos);
	m_UnrotatedOrigin[0] = unRotOrigin[0];
	m_UnrotatedOrigin[1] = unRotOrigin[1];
	m_UnrotatedOrigin[2] = unRotOrigin[2];
}
