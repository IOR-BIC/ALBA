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

#include "wx/listimpl.cpp"
#include "wx/busyinfo.h"
#include "wx/listctrl.h"
#include "wx/dir.h"

#include "mafOpImporterDicomOffis.h"
#include "mafGUIWizardPageNew.h"
#include "mafGUIValidator.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafInteractorDICOMImporter.h"
#include "mafTagArray.h"
#include "mafRWI.h"
#include "mafVMEImage.h"
#include "mafVMEVolumeGray.h"
#include "mafGUICheckListBox.h"
#include "mafGUIDicomSettings.h"
#include "mafGUIButton.h"
#include "mafGUISettingsAdvanced.h"
#include "mafVMEItemVTK.h"
#include "mafDataVector.h"
#include "mafTransform.h"
#include "mafTransformFrame.h"
#include "mafSmartPointer.h"
#include "mafVMEGroup.h"
#include "mafDicomCardiacMRIHelper.h"
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
#include "vtkMath.h"
#include "vtkImageFlip.h"
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
#include "vnl/vnl_vector.h"
#include "vtkProbeFilter.h"


//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpImporterDicomOffis);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// constants :
//----------------------------------------------------------------------------
enum DICOM_IMPORTER_MODALITY
{
	GIZMO_RESIZING,
	GIZMO_DONE
};
enum
{
	TYPE_VOLUME,
	TYPE_IMAGE,
};
enum
{
	STANDARD_MODALITY,
	CINEMATIC_MODALITY
};

//----------------------------------------------------------------------------
mafOpImporterDicomOffis::mafOpImporterDicomOffis(wxString label):
mafOp(label)
{
	m_OpType = OPTYPE_IMPORTER;
	m_Canundo = true;

	for (int i = 0; i < 6; i++) 
		m_SliceBounds[i] = 0;

	m_ZCropBounds[0] = 0;
	m_ZCropBounds[1] = 0;

	m_Wizard = NULL;
	m_LoadPage = NULL;
	m_CropPage = NULL;
	m_ImagesGroup = NULL;

	m_CropGuiLeft = NULL;
	m_LoadGuiLeft = NULL;
	m_LoadGuiUnderLeft = NULL;
	m_CropGuiCenter = NULL;
	m_LoadGuiCenter = NULL;
	m_SliceScannerCropPage = NULL;
	m_SliceScannerLoadPage = NULL;
	m_TimeScannerLoadPage = NULL;
	m_TimeScannerCropPage = NULL;

	m_SliceLookupTable = NULL;
	m_SlicePlane = NULL;
	m_SliceMapper = NULL;
	m_SliceTexture = NULL;
	m_SliceActor = NULL;
	m_SliceActorInCropPage = NULL;
	m_CropPlane = NULL;
	m_CropActor = NULL;
	m_TextActor=NULL;
	m_TextMapper=NULL;

	m_StudyListbox = NULL;
	m_SeriesListbox = NULL;
	
	m_DicomReaderModality = STANDARD_MODALITY;
	m_OutputType = TYPE_VOLUME;
	
	m_TagArray = NULL;
	m_StudyList = NULL;
		
	m_ConstantRotation = true;
	m_SideToBeDragged = 0; 

	m_GizmoStatus = GIZMO_DONE;

	m_Image = NULL;
	m_Volume = NULL;

	m_SortAxes = 2;

	m_VMEName = "";

	m_DicomModalityListBox = NULL;

	m_DicomInteractor = NULL;

	m_CurrentSlice = VTK_INT_MAX;

	m_TotalDicomRange[0]=0;
	m_TotalDicomRange[1]=1;

	m_CurrentImageID = 0;

	m_SkipAllNoPosition=false;

	m_ShowOrientationPosition = 0;
	m_SelectedStudy = -1;
	m_SelectedSeries = NULL;
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

	mafDEL(m_Image);
	mafDEL(m_Volume);

	m_ImagesGroup = NULL;
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
	CreateGui();
	CreateSliceVTKPipeline();

	m_Wizard = new mafGUIWizard(_("DICOM Importer"));
	m_Wizard->SetListener(this);

	CreateLoadPage();
	CreateCropPage();
	m_Wizard->SetButtonString("Crop >");
	
	//Create a chain between pages
	m_LoadPage->SetNextPage(m_CropPage);
	m_Wizard->SetFirstPage(m_LoadPage);

	wxString lastDicomDir = GetSetting()->GetLastDicomDir();
		
	if (lastDicomDir == "UNEDFINED_m_LastDicomDir")
		lastDicomDir = mafGetLastUserFolder().c_str();		
			
	wxDirDialog dialog(m_Wizard->GetParent(),"", lastDicomDir,wxRESIZE_BORDER, m_Wizard->GetPosition());
	dialog.SetReturnCode(wxID_OK);
	int ret_code = dialog.ShowModal();

	if (ret_code == wxID_OK)
	{
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
			{

				if(m_DicomReaderModality == STANDARD_MODALITY)
					result = BuildOutputVMEGrayVolumeFromDicom();
				else
					result = BuildOutputVMEGrayVolumeFromDicomCineMRI();

				break;
			}
		case TYPE_IMAGE:
			{
				if(m_DicomReaderModality == STANDARD_MODALITY)
					result = BuildOutputVMEImagesFromDicom();
				else
					result = BuildOutputVMEImagesFromDicomCineMRI();
				break;
			}
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

	if (m_CropPage)
	{
		m_CropPage->GetRWI()->m_RenFront->RemoveActor(m_CropActor);
	}

	vtkDEL(m_SliceTexture);
	vtkDEL(m_SliceLookupTable);
	vtkDEL(m_SlicePlane);
	vtkDEL(m_SliceMapper);
	vtkDEL(m_SliceActor);
	vtkDEL(m_CropPlane);
	vtkDEL(m_CropActor);

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
int mafOpImporterDicomOffis::BuildOutputVMEImagesFromDicom()
{
	int step= GetSetting()->GetBuildStep() + 1;

	int cropInterval = (m_ZCropBounds[1]+1 - m_ZCropBounds[0]);
	int n_slices = cropInterval / step;

	if(cropInterval % step != 0)
	{
		n_slices+=1;
	}

	mafProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Building images: please wait...");

	int count,s_count;
	mafNEW(m_ImagesGroup);

	m_ImagesGroup->SetName(wxString::Format("%s images",m_VMEName));
	m_ImagesGroup->ReparentTo(m_Input);

	for (count = m_ZCropBounds[0], s_count = 0; count < m_ZCropBounds[1]+1; count += step)
	{

		vtkImageData *image = m_SelectedSeries->GetSlice(count)->GetVTKImageData();
		Crop(image);

		mafSmartPointer<mafVMEImage> vmeImage;
		mafString name;
		name.Printf("%s_%d", m_VMEName.GetCStr(), count);
		
		vmeImage->SetName(name);
		vmeImage->SetData(image,0);
		vmeImage->GetTagArray()->DeepCopy(m_TagArray);
				
		vmeImage->GetMaterial()->m_ColorLut->DeepCopy(m_SliceTexture->GetLookupTable());
		
		m_ImagesGroup->AddChild(vmeImage);
		s_count++;

		progressHelper.UpdateProgressBar(count * 100 / m_SelectedSeries->GetSlicesNum());
	}

	m_Output = m_ImagesGroup;

	return OP_RUN_OK;
}
//----------------------------------------------------------------------------
int mafOpImporterDicomOffis::BuildOutputVMEImagesFromDicomCineMRI()
{
	int step = GetSetting()->GetBuildStep() + 1;

	int cropInterval = (m_ZCropBounds[1]+1 - m_ZCropBounds[0]);
	int n_slices = cropInterval / step;

	if(cropInterval % step != 0)
	{
		n_slices+=1;
	}

	mafProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Building images: please wait...");

	mafNEW(m_ImagesGroup);

	bool isRotated = m_SelectedSeries->IsRotated();
	int cardiacImageNum = m_SelectedSeries->GetCardiacImagesNum();
	
	m_ImagesGroup->SetName(wxString::Format("%s images",m_VMEName));
	m_ImagesGroup->ReparentTo(m_Input);

	//create all the animated images
	for (int i = m_ZCropBounds[0]; i < m_ZCropBounds[1]+1;i += step)
	{
		mafSmartPointer<mafVMEImage> image;
		mafString name;
		name.Printf("%s_%d_%d", m_VMEName.GetCStr(), i, cardiacImageNum);
		image->SetName(name);
		m_ImagesGroup->AddChild(image);
	}

	int totalNumberOfImages = (m_ZCropBounds[1]+1)*cardiacImageNum;
	int progressCounter = 0;

	// for every timestamp
	for (int ts = 0; ts < cardiacImageNum; ts++)
	{
		progressHelper.UpdateProgressBar(ts * 100 / cardiacImageNum);

		// get the time stamp from the dicom tag;
		// timestamp is in ms
		int tsImageId = GetSliceIDInSeries(ts, 0);

		double dcmTriggerTime = m_SelectedSeries->GetSlice(tsImageId)->GetDcmTriggerTime();
		
		for (int sourceVolumeSliceId = m_ZCropBounds[0], targetVolumeSliceId = 0; sourceVolumeSliceId < m_ZCropBounds[1]+1; sourceVolumeSliceId += step)
		{
			progressHelper.UpdateProgressBar(progressCounter * 100 / totalNumberOfImages);

			//Get Slice Id
			int currImageId = GetSliceIDInSeries(ts, sourceVolumeSliceId);
			
 			vtkImageData *image = m_SelectedSeries->GetSlice(currImageId)->GetVTKImageData();
 			Crop(image);
 						
			mafVMEImage *vmeImage = mafVMEImage::SafeDownCast(m_ImagesGroup->GetChild(targetVolumeSliceId));

			vmeImage->SetData(image,dcmTriggerTime);

			if (isRotated)
			{
				mafDicomSlice* slice = m_SelectedSeries->GetSlice(currImageId);

				mafMatrix sliceOrientationMatrix;
				sliceOrientationMatrix.SetFromDirectionCosines(slice->GetDcmImageOrientationPatient());

				double sliceVtkDataCenter[3];
				slice->GetVTKImageData()->GetCenter(sliceVtkDataCenter);

				vtkTransform *tr = vtkTransform::New();
				tr->PostMultiply();
				tr->Translate(-sliceVtkDataCenter[0], -sliceVtkDataCenter[1],-sliceVtkDataCenter[2]);
				tr->Concatenate(sliceOrientationMatrix.GetVTKMatrix());
				tr->Update();
								
				vmeImage->SetAbsMatrix(tr->GetMatrix(),dcmTriggerTime);
				tr->Delete();
			}

			m_ImagesGroup->GetChild(targetVolumeSliceId)->GetTagArray()->DeepCopy(m_TagArray);

			targetVolumeSliceId++;
			progressCounter++;
		}
	}

	m_Output = m_ImagesGroup;

	return OP_RUN_OK;
}
//----------------------------------------------------------------------------
int mafOpImporterDicomOffis::BuildOutputVMEGrayVolumeFromDicom()
{
	mafDicomSlice* firstSlice = m_SelectedSeries->GetSlice(0);
	bool skipDifferntDims = false;
	
	int step = GetSetting()->GetBuildStep() + 1;
	int cropInterval = (m_ZCropBounds[1]+1 - m_ZCropBounds[0]);
	int n_slices = (cropInterval/ step);

	if(cropInterval % step != 0)
		n_slices+=1;
	 
	int count,s_count;
	int numSliceToSkip = 0;
	bool *sliceToSkip = new bool[n_slices];
	memset(sliceToSkip, 0, sizeof(bool)*n_slices);

	int dim_img_check[3],dim_img[3];
	firstSlice->GetVTKImageData()->GetDimensions(dim_img_check);

	for (count = m_ZCropBounds[0], s_count = 0; count < m_ZCropBounds[1]+1; count += step)
	{  
			vtkImageData *image = m_SelectedSeries->GetSlice(count)->GetVTKImageData();
			image->Update();
			image->GetDimensions(dim_img);
			if ((dim_img[0] != dim_img_check[0]) && (dim_img[1] != dim_img_check[1])) 
				{
					if (!skipDifferntDims)
					{
						int result = wxMessageBox(_("Some slices have different dimension! They will be skipped! Do you want to continue?"), "", wxOK | wxCENTRE | wxCANCEL);
						if (result == wxCANCEL)
							return OP_RUN_CANCEL;
						else
							skipDifferntDims = true;
					}
					    					
					mafLogMessage("SLICE SKIPPED: %d",count);
					numSliceToSkip++;
					sliceToSkip[count] = true;
				}
	}
		
	mafProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Building volume: please wait...");

	n_slices -= numSliceToSkip;
	vtkMAFSmartPointer<vtkMAFRGSliceAccumulate> accumulate;
	accumulate->SetNumberOfSlices(n_slices);
	accumulate->BuildVolumeOnAxes(m_SortAxes);

	double orientation[6];
	firstSlice->GetDcmImageOrientationPatient(orientation);

	for (count = m_ZCropBounds[0], s_count = 0; count < m_ZCropBounds[1]+1; count += step)
	{
		if (sliceToSkip[count-m_ZCropBounds[0]])
			continue;

		vtkImageData *image = m_SelectedSeries->GetSlice(count)->GetVTKImageData();
		Crop(image);

		accumulate->SetSlice(s_count,image, orientation);
		s_count++;

		progressHelper.UpdateProgressBar(count * 100 / (m_ZCropBounds[1]+1));
	}

	delete []sliceToSkip;

	mafNEW(m_Volume);

	accumulate->Update();

	vtkRectilinearGrid *rg_out;
	rg_out=accumulate->GetOutput();
	rg_out->Update();

	m_Volume->SetDataByDetaching(rg_out,0);

	//Setting orientation matrix
	mafMatrix orientationMatrix;
	orientationMatrix.SetFromDirectionCosines(orientation);
	m_Volume->SetAbsMatrix(orientationMatrix);
		
	//Copy inside the first VME item of m_Volume the CT volume and Dicom's tags
	m_Volume->GetTagArray()->DeepCopy(m_TagArray);
	
	m_Volume->SetName(m_VMEName);  

	m_Output = m_Volume;

	return OP_RUN_OK;
}

//----------------------------------------------------------------------------
int mafOpImporterDicomOffis::BuildOutputVMEGrayVolumeFromDicomCineMRI()
{
	mafDicomSlice* firstSlice = m_SelectedSeries->GetSlice(0);
	bool skipDifferntDims = false;

	int step = GetSetting()->GetBuildStep() + 1;
	int cropInterval = (m_ZCropBounds[1]+1 - m_ZCropBounds[0]);
	int n_slices = cropInterval / step;
	
	if(cropInterval % step != 0)
		n_slices+=1;

	mafProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Building volume: please wait...");

	// create the time varying vme
	mafNEW(m_Volume);
	int currImageId = 0;
	int cardiacImageNum = m_SelectedSeries->GetCardiacImagesNum();
	int totalNumberOfImages = (m_ZCropBounds[1]+1)*cardiacImageNum;
	int progressCounter = 0;

	int numSliceToSkip = 0;
	bool *sliceToSkip = new bool[n_slices];
	memset(sliceToSkip, 0, sizeof(bool)*n_slices);

	int dim_img_check[3], dim_img[3];
	firstSlice->GetVTKImageData()->GetDimensions(dim_img_check);

	for (int ts = 0; ts < cardiacImageNum; ts++)
	{
		for (int sourceVolumeSliceId = m_ZCropBounds[0]; sourceVolumeSliceId < m_ZCropBounds[1]+1; sourceVolumeSliceId += step)
		{
			// show the current slice
			currImageId = GetSliceIDInSeries(ts, sourceVolumeSliceId);
					
			vtkImageData *image = m_SelectedSeries->GetSlice(currImageId)->GetVTKImageData();
			image->GetDimensions(dim_img);

			if ((dim_img[0] != dim_img_check[0]) && (dim_img[1] != dim_img_check[1]))
			{
				if (!skipDifferntDims)
				{
					int result = wxMessageBox(_("Some slices have different dimension! They will be skipped! Do you want to continue?"), "", wxOK | wxCENTRE | wxCANCEL);
					if (result == wxCANCEL)
						return OP_RUN_CANCEL;
					else
						skipDifferntDims = true;
				}

				mafLogMessage("SLICE SKIPPED: %d", currImageId);
				numSliceToSkip++;
				sliceToSkip[currImageId] = true;
			}
		}
	}


	for (int ts = 0; ts < cardiacImageNum; ts++)
	{
		// BEWARE:
		// the following data structure was added in order to fix bug :
		// http://bugzilla.hpc.cineca.it/show_bug.cgi?id=2088
		//
		// Regression test data for this bug is POLITECNICO and SA Cardiac MRI Dicom
		//
		// There is a nasty bug in some of our pipes that is preventing volumes with 
		// Z axis order different from increasing to work while standard vtk filters have
		// no problems at all with this issue: we successfully visualized this kind of
		// volumes with ParaView.
		//
		// For example if you have Z axes coordinate in your rectilinear grid like:
		// 8  5  3  1
		// some pipes will fail (for sure the mafPipeVolumeSlice_BES ).
		// This data structure is used to feed the slice accumulator with the right increasing 
		// z ordering ie:
		// 1 3 5 8
		std::vector<vtkImageData *> imageDataVector;

		// get the time stamp from the dicom tag;
		// timestamp is in ms
		int probeHeigthId = 0;    
		int tsImageId = GetSliceIDInSeries(ts, probeHeigthId);
	
		
		mafTimeStamp tsDouble=m_SelectedSeries->GetSlice(tsImageId)->GetDcmTriggerTime();

		double oldPosTrasformed = -1.0;
		double oldOrigin[3];

		for (int sourceVolumeSliceId = m_ZCropBounds[0]; sourceVolumeSliceId < m_ZCropBounds[1]+1; sourceVolumeSliceId += step)
		{
			progressHelper.UpdateProgressBar(progressCounter * 100 / totalNumberOfImages);

			// show the current slice
			currImageId = GetSliceIDInSeries(ts, sourceVolumeSliceId);

			vtkImageData *imageData = m_SelectedSeries->GetSlice(currImageId)->GetVTKImageData();
			Crop(imageData);

			imageDataVector.push_back(imageData);

			progressCounter++;
		}

		std::map<double , int> zToIDMap;

		for (int i = 0 ; i < imageDataVector.size() ; i++)
		{
			double origin[3] = {-999,-999,-999};
			imageDataVector[i]->GetOrigin(origin);

			double zOrigin = origin[2];
			zToIDMap[zOrigin] = i;
		}

		// Build item at timestamp ts    
		vtkMAFSmartPointer<vtkMAFRGSliceAccumulate> accumulator;

		// always build the volume on z-axis
		accumulator->BuildVolumeOnAxes(m_SortAxes);
		accumulator->SetNumberOfSlices(cardiacImageNum - numSliceToSkip);

		int i = 0;
		int j = 0;

		for( map<double,int>::iterator currentMapElement=zToIDMap.begin(); currentMapElement!=zToIDMap.end(); ++currentMapElement,++j)
		{
			if (sliceToSkip[j])
			{
				break;
			}
			double id = currentMapElement->second;
			double z = currentMapElement->first;

			double orientation[6];
			m_SelectedSeries->GetSlice(m_ZCropBounds[0])->GetDcmImageOrientationPatient(orientation);
			accumulator->SetSlice(i, imageDataVector[currentMapElement->second],orientation);
			i++;
		}

		accumulator->Update();

		vtkMAFSmartPointer<vtkRectilinearGrid> rg_out;
		rg_out->DeepCopy(accumulator->GetOutput());
		rg_out->Update();

		m_Volume->SetDataByDetaching(rg_out,tsDouble);
		m_Volume->Update();

		if (m_SelectedSeries->IsRotated())
		{

			mafDicomSlice* slice = m_SelectedSeries->GetSlice(m_ZCropBounds[0]);

			mafMatrix mat;
			mat.SetFromDirectionCosines(slice->GetDcmImageOrientationPatient());

			mafSmartPointer<mafTransform> boxPose;
			boxPose->SetMatrix(mat);

			double pos[3];
			slice->GetDcmImagePositionPatient(pos);

			boxPose->SetPosition(pos);
			boxPose->Update();

			m_Volume->SetAbsMatrix(boxPose->GetMatrix(),tsDouble);
		}

	}

	delete []sliceToSkip;

	// update m_tag_array ivar
	m_Volume->GetTagArray()->DeepCopy(m_TagArray);
	m_Volume->SetName(m_VMEName);

	m_Output = m_Volume;

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
	m_CropPage->GetRWI()->m_RenFront->AddActor(m_CropActor);
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
void mafOpImporterDicomOffis::CreateGui()
{
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
		if(m_StudyList->GetStudiesNum()>0)
		{
			FillStudyListBox();
			m_StudyListbox->SetSelection(0);
			OnStudySelect();

			m_LoadPage->GetRWI()->CameraReset();

			return true;
		}
	}
	return false;
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::OnEvent(mafEventBase *maf_event) 
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch (e->GetId())
		{
			case ID_RANGE_MODIFIED:
			{
				//ZCrop slider
				OnRangeModified();
			}
			break;
			case mafGUIWizard::MED_WIZARD_CHANGE_PAGE:
			{
				OnWizardChangePage(e);
			}
			break;
			case mafGUIWizard::MED_WIZARD_CHANGED_PAGE:
			{
				/* This is a ack, because that "genius" of wx  send the change event
				before page show, so we need to duplicate the code here in order to
				manage the camera update */
				m_Wizard->GetCurrentPage()->Show();
				m_Wizard->GetCurrentPage()->SetFocus();
				m_Wizard->GetCurrentPage()->Update();
				CameraReset();
			}
			break;
			case ID_STUDY_SELECT:
			{
				OnStudySelect();
			}
			break;
			case ID_SERIES_SELECT:
			{
				OnSeriesSelect();
			}
			break;
			case ID_SHOW_TEXT:
			{
				m_TextActor->SetVisibility(m_ShowOrientationPosition);
				m_LoadPage->GetRWI()->CameraUpdate();
			}
			break;
			case MOUSE_DOWN:
			{
				OnMouseDown(e);
			}
			break;
			case MOUSE_MOVE:  //resize gizmo
			{
				OnMouseMove(e);
			}
			break;
			case MOUSE_UP:  //block gizmo
			{
				OnMouseUp();
			}
			break;
			case ID_SCAN_TIME:
			case ID_SCAN_SLICE:
			{
				OnChangeSlice();
			}
			break;
			default:
			{
				mafEventMacro(*e);
			}
		}
	}
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::SetVMEName()
{
	mafDicomSlice * sliceData = m_SelectedSeries->GetSlice(0);
	m_VMEName = sliceData->GetDcmModality();
	if (sliceData->GetDescription() != "")
	{
		m_VMEName += " ";
		m_VMEName += sliceData->GetDescription();
	}
	if (sliceData->GetPatientName() != "")
	{
		m_VMEName += " ";
		m_VMEName += sliceData->GetPatientName();
	}
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::CameraUpdate()
{
	if(m_Wizard->GetCurrentPage() == m_LoadPage)
	{
		m_LoadPage->UpdateActor();
		m_LoadPage->GetRWI()->CameraUpdate();
	}
	else if(m_Wizard->GetCurrentPage() == m_CropPage)
	{
		m_CropPage->UpdateActor();
		m_CropPage->GetRWI()->CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::CameraReset()
{
	m_LoadPage->UpdateWindowing(m_TotalDicomRange,m_TotalDicomSubRange);
	m_LoadPage->GetRWI()->CameraReset();
	m_CropPage->UpdateWindowing(m_TotalDicomRange,m_TotalDicomSubRange);
	m_CropPage->GetRWI()->CameraReset();
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::CreateSliceVTKPipeline()
{
	vtkNEW(m_SliceLookupTable);

	vtkNEW(m_SliceTexture);
	m_SliceTexture->InterpolateOn();

	vtkNEW(m_SlicePlane);

	vtkNEW(m_SliceMapper);
	m_SliceMapper->SetInput(m_SlicePlane->GetOutput());

	vtkNEW(m_SliceActor);
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

	vtkNEW(m_CropPlane);

	vtkMAFSmartPointer<vtkOutlineFilter>	of;
	of->SetInput(((vtkDataSet *)(m_CropPlane->GetOutput())));

	vtkMAFSmartPointer<vtkPolyDataMapper> pdm;
	pdm->SetInput(of->GetOutput());

	vtkNEW(m_CropActor);
	m_CropActor->GetProperty()->SetColor(0.8,0,0);
	m_CropActor->VisibilityOff();
	m_CropActor->SetMapper(pdm);

	if(!this->m_TestMode)
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
		
		if (framesNum > 0)
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

	//---------------------------------------
	// Cardiac MRI Handling
	//---------------------------------------

	// foreach series
	for (int i = 0; i < m_StudyList->GetStudiesNum(); i++)
	{
		mafDicomStudy * study = m_StudyList->GetStudy(i);
		std::vector<int> seriesToDelete;

		for (int j = 0; j < study->GetSeriesNum(); j++)
		{
			mafDicomSeries *currentSeries = study->GetSeries(j);
			assert(currentSeries);

			// check if the first slice is cineMRI
			mafDicomSlice* firstSlice = NULL;
			firstSlice = currentSeries->GetSlice(0);
			assert(firstSlice);

			bool isCardiacMRI = (firstSlice->GetDcmCardiacNumberOfImages() > 1);

			if (isCardiacMRI)
			{
				vector<string> absFileNames;
				for (int i = 0; i < currentSeries->GetSlicesNum(); i++)
				{
					mafDicomSlice* slice = NULL;
					slice = currentSeries->GetSlice(i);
					assert(slice);
					absFileNames.push_back(slice->GetSliceABSFileName());
				}

				// create a cardiac mri helper for the series and initialize it
				mafDicomCardiacMRIHelper *currentHelper = new mafDicomCardiacMRIHelper();
				currentHelper->SetListener(this);
				currentHelper->SetModeToInputDicomSlicesABSFileNamesVector();
				currentHelper->SetInputDicomSlicesABSFileNamesVector(absFileNames);

				if (currentHelper->ParseDicomDirectory() != MAF_OK)
				{
					seriesToDelete.push_back(j);
					continue;
				}

				currentSeries->SetCardiacHelper(currentHelper);

				vnl_matrix<double> rotateFlag = currentHelper->GetRotateFlagIdPlaneMatrix();

				bool skipRotateFlagCorrection = false;
				if (rotateFlag.rows() == 0 && rotateFlag.columns() == 0)
				{
					skipRotateFlagCorrection = true;
				}

				vnl_matrix<double> flipLeftRightFlag = currentHelper->GetFlipLeftRightFlagIdPlaneMatrix();

				bool skipFlipLeftRightFlagCorrection = false;
				if (flipLeftRightFlag.rows() == 0 && flipLeftRightFlag.columns() == 0)
				{
					skipFlipLeftRightFlagCorrection = true;
				}

				vnl_matrix<double> flipUpDownFlag = currentHelper->GetFlipUpDownFlagIdPlaneMatrix();

				bool skipFlipUpDownFlagCorrection = false;
				if (flipUpDownFlag.rows() == 0 && flipUpDownFlag.columns() == 0)
				{
					skipFlipUpDownFlagCorrection = true;
				}

				bool skipCorrection = skipRotateFlagCorrection && skipFlipLeftRightFlagCorrection && skipFlipUpDownFlagCorrection;

				if (!skipCorrection)
				{
					vnl_matrix<double> fileNumberForPlaneIFrameJIdPlaneMatrix = currentHelper->GetFileNumberForPlaneIFrameJIdPlaneMatrix();

					vnl_matrix<double> newPositionSingleFrameIdPlaneMatrix = currentHelper->GetNewPositionSingleFrameIdPlaneMatrix();
					vnl_matrix<double> newXVersorsSingleFrameIdPlaneMatrix = currentHelper->GetNewXVersorsSingleFrameIdPlaneMatrix();
					vnl_matrix<double> newYVersorsSingleFrameIdPlaneMatrix = currentHelper->GetNewYVersorsSingleFrameIdPlaneMatrix();

					// for the current series
					assert(currentSeries);

					// for each slice
					int numSlicesInSeries = currentSeries->GetSlicesNum();

					// get the first slice in series
					mafDicomSlice* firstSlice = NULL;
					firstSlice = currentSeries->GetSlice(0);
					assert(firstSlice);

					int cardiacTimeFrames = -1;
					cardiacTimeFrames = firstSlice->GetDcmCardiacNumberOfImages();
					assert(cardiacTimeFrames > 1);

					int planesPerFrame = -1;
					planesPerFrame = numSlicesInSeries / cardiacTimeFrames;

					progressHelper.InitProgressBar("Applying Cardiac MRI correction, please wait...");

					for (int timeID = 0; timeID < cardiacTimeFrames; timeID++)
					{
						for (int planeID = 0; planeID < planesPerFrame; planeID++)
						{
							progressHelper.UpdateProgressBar((timeID) * 100 / ((double)(cardiacTimeFrames)));

							int itemID = fileNumberForPlaneIFrameJIdPlaneMatrix(planeID, timeID);

							mafDicomSlice* currentSlice = NULL;
							currentSlice = currentSeries->GetSlice(itemID);
							assert(currentSlice);

							// correction in place
							vtkImageData *imageData = NULL;
							imageData = currentSlice->GetVTKImageData();
							assert(imageData);

							double center[3] = { -9999,-9999,-9999 };
							imageData->GetCenter(center);

							double bounds[6] = { -9999,-9999,-9999,-9999,-9999,-9999 };
							imageData->GetBounds(bounds);

							vtkTransform *tr = vtkTransform::New();
							tr->PostMultiply();
							tr->Translate(-center[0], -center[1], -center[2]);

							tr->RotateZ(rotateFlag(planeID, 0));

							tr->Translate(center);

							vtkImageReslice *rs = vtkImageReslice::New();
							rs->SetInput(imageData);
							rs->SetResliceTransform(tr);
							rs->SetInterpolationModeToLinear();
							rs->Update();

							bool flipUpDownCurrent = (flipUpDownFlag(planeID, 0) == 1 ? true : false);
							bool flipLeftRightCurrent = (flipLeftRightFlag(planeID, 0) == 1 ? true : false);

							vtkImageFlip *flipLR = NULL;

							vtkImageFlip *flipUD = NULL;

							vtkImageData *flipLROutput = NULL;

							if (flipLeftRightCurrent)
							{
								flipLR = vtkImageFlip::New();
								flipLR->SetInput(rs->GetOutput());

								// validated against p20 regression test data
								// (it's the same axis as the p09 test data but  it shouldn't since it's flip left to right...)
								flipLR->SetFilteredAxes(1);
								flipLR->Update();

								flipLROutput = flipLR->GetOutput();

							}
							else
							{
								flipLROutput = rs->GetOutput();
							}

							vtkImageData *flipUDOutput = NULL;

							if (flipUpDownCurrent)
							{
								flipUD = vtkImageFlip::New();
								flipUD->SetInput(flipLROutput);

								// validated against p09 regression test data
								// (it's the same axis as the p20 test data bu  it shouldn't since it's flip up/down...)
								flipUD->SetFilteredAxes(1);
								flipUD->Update();
								flipUDOutput = flipUD->GetOutput();
							}
							else
							{
								flipUDOutput = flipLROutput;
							}


							vtkImageData *outputBuffer = vtkImageData::New();
							outputBuffer->DeepCopy(flipUDOutput);

							currentSlice->SetVTKImageData(outputBuffer);

							vtkDEL(flipUD);
							vtkDEL(flipLR);

							tr->Delete();
							rs->Delete();
							outputBuffer->Delete();

							vnl_vector<double> p = newPositionSingleFrameIdPlaneMatrix.get_row(planeID);
							assert(p.size() == 3);
							double p3[3] = { p(0),p(1),p(2) };
							currentSlice->SetDcmImagePositionPatient(p3);

							vnl_vector<double> xv = newXVersorsSingleFrameIdPlaneMatrix.get_row(planeID);
							assert(xv.size() == 3);

							vnl_vector<double> yv = newYVersorsSingleFrameIdPlaneMatrix.get_row(planeID);
							assert(yv.size() == 3);

							double orientation[6] = { xv(0),xv(1),xv(2),yv(0),yv(1),yv(2) };
							currentSlice->SetDcmImageOrientationPatient(orientation);

							currentSlice->GetVTKImageData()->SetOrigin(0, 0, bounds[4]);

						}
					}
				}
			}
		}

		for (int s = 0; s < seriesToDelete.size(); s++)
		{
			mafLogMessage("ERROR during Cardiac Series reading series");
			study->RemoveSeries(s);
		}
	}

	return true;
}
//----------------------------------------------------------------------------
std::vector<wxString> mafOpImporterDicomOffis::BuildFileListRecursive(wxString path)
{
	std::vector<wxString> fileList;
	wxDir dir;
	wxArrayString childFolders;
	wxArrayString childFiles;

	//cannot open dir return empty file list
	if (!dir.Open(path))
		return fileList;

	
		
	//Reading subdirectory list and call function recursive
	dir.GetAllFiles(path, &childFolders, wxEmptyString, wxDIR_DIRS);
	for (int i = 0; i < childFolders.size(); i++)
	{
		std::vector<wxString> recursiveList = BuildFileListRecursive(path + "\\" + childFolders[i]);
		fileList.reserve(fileList.size() + recursiveList.size()); // preallocate memory
		fileList.insert(fileList.end(), recursiveList.begin(), recursiveList.end());
	}

	//Reading file list and add the files to the file list
	dir.GetAllFiles(path, &childFiles, wxEmptyString, wxDIR_FILES | wxDIR_DIRS);
	fileList.reserve(fileList.size() + childFiles.size());
	for (int i = 0; i < childFiles.size(); i++)
	{
		wxString tmp = childFiles[i];
		fileList.push_back(path + "\\" + childFiles[i]);
		wxString tmp2 = fileList[i];
	}
	
	return fileList;
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
	double dcmImageOrientationPatient[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
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
	// We are using Image Position(Patient) (0020, 0032) Dicom tag to set Dicom slice position.
	// See here for the motivation behind this decision:
	// http://www.cmake.org/pipermail/insight-users/2005-September/014711.html
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
		dicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient, dcmImageOrientationPatient[i], i);
	
	//Read Cine MR related stuff
	dicomDataset->findAndGetLongInt(DCM_InstanceNumber,dcmInstanceNumber);
	dicomDataset->findAndGetLongInt(DCM_CardiacNumberOfImages,dcmCardiacNumberOfImages);
	dicomDataset->findAndGetFloat64(DCM_TriggerTime,dcmTriggerTime);

	//Read slice 
	dicomDataset->findAndGetString(DCM_PatientsBirthDate, birthdate);
	dicomDataset->findAndGetString(DCM_StudyDate, date);
	dicomDataset->findAndGetString(DCM_SeriesDescription, description);
	dicomDataset->findAndGetString(DCM_PatientsName, patientName);
	dicomDataset->findAndGetString(DCM_PhotometricInterpretation, photometricInterpretation);

	//Read image data
	vtkImageData *dicomSliceVTKImageData = CreateImageData(dicomDataset, dcmImagePositionPatient);
			
	//Create Slice
	mafDicomSlice *newSlice = new mafDicomSlice(fileName, dcmImagePositionPatient, dcmImageOrientationPatient, dicomSliceVTKImageData, description, date, patientName, birthdate, dcmInstanceNumber, dcmCardiacNumberOfImages, dcmTriggerTime);
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
		m_LoadGuiLeft->Bool(ID_SHOW_TEXT, "Show position info", &m_ShowOrientationPosition, 1, _("Shows position and orientation"));
		m_LoadGuiLeft->Enable(ID_SCAN_SLICE, numOfSlices > 1);
		if (cardiacImageNum > 0)
		{
			m_TimeScannerLoadPage = m_LoadGuiLeft->Slider(ID_SCAN_TIME, _("Time "), &m_CurrentTime, 0, cardiacImageNum);
			m_LoadGuiLeft->Enable(ID_SCAN_TIME, cardiacImageNum > 1);
		}
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
		if (cardiacImageNum > 0)
		{
			m_TimeScannerCropPage = m_CropGuiLeft->Slider(ID_SCAN_TIME, _("Time "), &m_CurrentTime, 0, cardiacImageNum);
			m_CropGuiLeft->Enable(ID_SCAN_TIME, cardiacImageNum > 1);
		}
		m_CropPage->AddGuiLowerLeft(m_CropGuiLeft);
		m_CropPage->Update();
	}
}
//----------------------------------------------------------------------------
int mafOpImporterDicomOffis::GetSliceIDInSeries(int timeId, int heigthId)
{
	if (m_DicomReaderModality == STANDARD_MODALITY)
		return heigthId;
		
	mafDicomSlice *firstDicomListElement;
	firstDicomListElement = (mafDicomSlice *)m_SelectedSeries->GetSlice(0);
	int timeFrames =  firstDicomListElement->GetDcmCardiacNumberOfImages();

	int dicomFilesNumber = m_SelectedSeries->GetSlicesNum();

	int numSlicesPerTS;

	if (dicomFilesNumber >= timeFrames)
	{
		numSlicesPerTS = dicomFilesNumber / timeFrames;
	} 
	else
	{
		numSlicesPerTS = timeFrames / dicomFilesNumber;
	}

	int maxHeigthId = numSlicesPerTS - 1; 
	int maxTimeId = timeFrames - 1; 

	if (heigthId < 0 || heigthId > maxHeigthId || timeId < 0 || timeId > maxTimeId )
	{
		return -1;
	}

	mafDicomCardiacMRIHelper *helper = NULL;
	helper = m_SelectedSeries->GetCardiacHelper();
	mafLogMessage("%d",__LINE__);
	assert(helper);

	vnl_matrix<double> planeIFrameJFileNumberMatrix = helper->GetFileNumberForPlaneIFrameJIdPlaneMatrix();
	return (planeIFrameJFileNumberMatrix(heigthId, timeId)); 
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

	// AACC 26-10-2010: Hack to make it work with ATI RADEON Driver
	m_CropActor->GetMapper()->Modified();
	// End of hack

	slice->GetVTKImageData()->GetScalarRange(range);
	m_SliceTexture->SetInput(slice->GetVTKImageData());
	m_SliceTexture->Modified();
		
	//Invert grayscale for Photometric Interpretation MONOCHROME1
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
	double spacing[3], inverseSpacing[3], crop_bounds[6], sliceBound[6], sliceOrigin[3];
	int sliceExtent[6];

	vtkImageData* slice = m_SelectedSeries->GetSlice(0)->GetVTKImageData();
	slice->GetExtent(sliceExtent);
	slice->GetOrigin(sliceOrigin);
	slice->GetSpacing(spacing);
	m_CropPlane->GetOutput()->GetBounds(crop_bounds);

	//Align cropBounds to current grid
	m_CropExtent[0] = round(crop_bounds[0] / spacing[0]);
	m_CropExtent[1] = round(crop_bounds[1] / spacing[0]);
	m_CropExtent[2] = round(crop_bounds[2] / spacing[1]);
	m_CropExtent[3] = round(crop_bounds[3] / spacing[1]);
	m_CropExtent[4] = m_CropExtent[5] = 0;

	//TODO Remove this control when the crop selector was updated
	m_CropExtent[0] = max(m_CropExtent[0], sliceExtent[0]);
	m_CropExtent[1] = min(m_CropExtent[1], sliceExtent[1]);
	m_CropExtent[2] = max(m_CropExtent[2], sliceExtent[2]);
	m_CropExtent[3] = min(m_CropExtent[3], sliceExtent[3]);

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
		OnSeriesSelect();
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
void mafOpImporterDicomOffis::OnSeriesSelect()
{
	mafDicomSeries * selectedSeries = m_StudyList->GetStudy(m_SelectedStudy)->GetSeries(m_SeriesListbox->GetSelection());
		
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

		//Set Reader Modality
		if (m_SelectedSeries->GetCardiacImagesNum() != 0)
			m_DicomReaderModality = CINEMATIC_MODALITY;
		else
			m_DicomReaderModality = STANDARD_MODALITY;
				
		if (!this->m_TestMode)
		{
			CreateSliders();

			m_LoadPage->RemoveGuiLowerCenter(m_LoadGuiCenter);
			m_LoadGuiCenter = new mafGUI(this);
			m_LoadGuiCenter->Divider();

			if (numberOfSlices > 1 && !GetSetting()->AutoVMEType())
			{
				wxString typeArrayVolumeImage[2] = { _("Volume"),_("Image") };
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

		m_CropPlane->SetOrigin(0.0, 0.0, 0.0);
		m_CropPlane->SetPoint1(m_SliceBounds[1] - m_SliceBounds[0], 0.0, 0.0);
		m_CropPlane->SetPoint2(0.0, m_SliceBounds[3] - m_SliceBounds[2], 0.0);

		m_CropActor->VisibilityOn();
	}

	if (m_Wizard->GetCurrentPage() == m_CropPage)//From Crop page to build page
	{
		//get the current windowing in order to maintain subrange thought the wizard pages 
		m_CropPage->GetWindowing(m_TotalDicomRange, m_TotalDicomSubRange);
		m_CropActor->VisibilityOff();
		m_Wizard->SetButtonString("Crop >");
	}
	
	CameraReset();
	GuiUpdate();
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::OnMouseDown( mafEvent * e )
{
	if (m_Wizard->GetCurrentPage() == m_CropPage)
	{
		//long handle_id = e->GetArg();
		double pos[3];
		vtkPoints *p = (vtkPoints *)e->GetVtkObj();
		p->GetPoint(0, pos);

		//calcola altezza rettangolo
		double b[6];
		m_CropPlane->GetOutput()->GetBounds(b);
		double dx = (b[1] - b[0]) / 5;
		double dy = (b[3] - b[2]) / 5;

		double O[3], P1[3], P2[3];
		//Caso di default P1 in alto a SX e P2 in basso a DX
		m_CropPlane->GetOrigin(O);
		m_CropPlane->GetPoint1(P1);
		m_CropPlane->GetPoint2(P2);
		//Se non siamo nel caso di default modifichiamo in modo da ritornare in quel caso
		if (P2[0] < P1[0] && P2[1] < P1[1])//Caso P1 in basso a DX e P2 in alto a SX
		{
			O[0] = P2[0];
			O[1] = P1[1];
			double tempx = P1[0];
			double tempy = P1[1];
			P1[0] = P2[0];
			P1[1] = P2[1];
			P2[0] = tempx;
			P2[1] = tempy;
		}
		else if (P1[0]<P2[0] && P1[1]>P2[1])//Caso P1 in basso a SX e P2 in alto a DX
		{
			O[0] = P1[0];
			O[1] = P1[1];
			double tempy = P1[1];
			P1[1] = P2[1];
			P2[1] = tempy;
		}
		else if (P1[0] > P2[0] && P1[1] < P2[1])//Caso P1 in alto a DX e P2 in basso a SX
		{
			O[0] = P2[0];
			O[1] = P2[1];
			double tempx = P1[0];
			P1[0] = P2[0];
			P2[0] = tempx;
		}
		m_CropPlane->SetOrigin(O);
		m_CropPlane->SetPoint1(P1);
		m_CropPlane->SetPoint2(P2);


		//	  8------------1----------2--->x
		//		|												|
		//		7												3
		//		|												|
		//		6------------5----------4
		//		|
		//	  v y

		if (P1[0] + dx / 2 <= pos[0] && pos[0] <= P2[0] - dx / 2 &&
			P1[1] - dy / 2 <= pos[1] && pos[1] <= P1[1] + dy / 2)
		{
			m_SideToBeDragged = 1;
		}
		else if (P2[0] - dx / 2 <= pos[0] && pos[0] <= P2[0] + dx / 2 &&
			P1[1] - dy / 2 <= pos[1] && pos[1] <= P1[1] + dy / 2)
		{
			m_SideToBeDragged = 2;
		}
		else if (P2[0] - dx / 2 <= pos[0] && pos[0] <= P2[0] + dx / 2 &&
			P2[1] - dy / 2 >= pos[1] && pos[1] >= P1[1] + dy / 2)
		{
			m_SideToBeDragged = 3;
		}
		else if (P2[0] - dx / 2 <= pos[0] && pos[0] <= P2[0] + dx / 2 &&
			P2[1] - dy / 2 <= pos[1] && pos[1] <= P2[1] + dy / 2)
		{
			m_SideToBeDragged = 4;
		}
		else if (P1[0] + dx / 2 <= pos[0] && pos[0] <= P2[0] - dx / 2 &&
			P2[1] - dy / 2 <= pos[1] && pos[1] <= P2[1] + dy / 2)
		{
			m_SideToBeDragged = 5;
		}
		else if (P1[0] - dx / 2 <= pos[0] && pos[0] <= P1[0] + dx / 2 &&
			P2[1] - dy / 2 <= pos[1] && pos[1] <= P2[1] + dy / 2)
		{
			m_SideToBeDragged = 6;
		}
		else if (P1[0] - dx / 2 <= pos[0] && pos[0] <= P1[0] + dx / 2 &&
			P2[1] - dy / 2 >= pos[1] && pos[1] >= P1[1] + dy / 2)
		{
			m_SideToBeDragged = 7;
		}
		else if (P1[0] - dx / 2 <= pos[0] && pos[0] <= P1[0] + dx / 2 &&
			P1[1] - dy / 2 <= pos[1] && pos[1] <= P1[1] + dy / 2)
		{
			m_SideToBeDragged = 8;
		}
		else
			//hai pickato in un punto che non corrisponde a nessun lato
			// => crea un nuovo gizmo
		{
			m_GizmoStatus = GIZMO_RESIZING;
			m_CropActor->VisibilityOn();

			pos[2] = 0;
			m_CropPlane->SetOrigin(pos);
			m_CropPlane->SetPoint1(pos[0], pos[1], pos[2]);
			m_CropPlane->SetPoint2(pos[0], pos[1], pos[2]);
			m_CropPlane->SetXResolution(10);
		}

		CameraUpdate();
	}
	
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::OnMouseMove( mafEvent * e )
{
	if (m_Wizard->GetCurrentPage() == m_CropPage)
	{
		double pos[3], oldO[3], oldP1[3], oldP2[3];
		vtkPoints *p = (vtkPoints *)e->GetVtkObj();
		p->GetPoint(0, pos);

		m_CropPlane->GetOrigin(oldO);
		m_CropPlane->GetPoint1(oldP1);
		m_CropPlane->GetPoint2(oldP2);

		if (m_GizmoStatus == GIZMO_RESIZING)
		{
			m_CropPlane->SetPoint1(oldO[0], pos[1], oldP1[2]);
			m_CropPlane->SetPoint2(pos[0], oldO[1], oldP1[2]);
		}
		else if (m_GizmoStatus == GIZMO_DONE)
		{
			if (m_SideToBeDragged == 0)
			{
			}
			else if (m_SideToBeDragged == 1)
				m_CropPlane->SetPoint1(oldP1[0], pos[1], oldP1[2]);
			else if (m_SideToBeDragged == 2)
			{
				m_CropPlane->SetPoint1(oldP1[0], pos[1], oldP1[2]);
				m_CropPlane->SetPoint2(pos[0], oldP2[1], oldP2[2]);
			}
			else if (m_SideToBeDragged == 3)
				m_CropPlane->SetPoint2(pos[0], oldP2[1], oldP2[2]);
			else if (m_SideToBeDragged == 4)
			{
				m_CropPlane->SetOrigin(oldO[0], pos[1], oldO[2]);
				m_CropPlane->SetPoint2(pos[0], pos[1], oldP2[2]);
			}
			else if (m_SideToBeDragged == 5)
			{
				m_CropPlane->SetOrigin(oldO[0], pos[1], oldO[2]);
				m_CropPlane->SetPoint2(oldP2[0], pos[1], oldP2[2]);
			}
			else if (m_SideToBeDragged == 6)
			{
				m_CropPlane->SetOrigin(pos[0], pos[1], oldO[2]);
				m_CropPlane->SetPoint1(pos[0], oldP1[1], oldP2[2]);
				m_CropPlane->SetPoint2(oldP2[0], pos[1], oldP2[2]);
			}
			else if (m_SideToBeDragged == 7)
			{
				m_CropPlane->SetOrigin(pos[0], oldO[1], oldO[2]);
				m_CropPlane->SetPoint1(pos[0], oldP1[1], oldP1[2]);
			}
			else if (m_SideToBeDragged == 8)
			{
				m_CropPlane->SetOrigin(pos[0], oldO[1], oldO[2]);
				m_CropPlane->SetPoint1(pos[0], pos[1], oldP1[2]);
			}
		}
		CameraUpdate();
	}
	
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
		// show the current slice
		int currImageId = GetSliceIDInSeries(m_CurrentTime, m_CurrentSlice);
		if (currImageId != -1) 
		{
			GenerateSliceTexture(currImageId);
			CameraUpdate();
		}
		m_SliceScannerLoadPage->SetValue(m_CurrentSlice);
		m_SliceScannerLoadPage->Update();
		m_SliceScannerCropPage->SetValue(m_CurrentSlice);
		m_SliceScannerCropPage->Update();
	}
	GuiUpdate();
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::OnMouseUp()
{
	if (m_Wizard->GetCurrentPage() == m_CropPage)
	{
		if (m_GizmoStatus == GIZMO_RESIZING)
			m_GizmoStatus = GIZMO_DONE;
		else if (m_GizmoStatus == GIZMO_DONE)
			m_SideToBeDragged = 0;

		double p1[3], p2[3], origin[3];
		m_CropPlane->GetPoint1(p1);
		m_CropPlane->GetPoint2(p2);
		m_CropPlane->GetOrigin(origin);

//   TODO CHECK
// 		if ((p1[0] == p2[0] && p1[1] == p2[1] && p1[2] == p2[2]) ||
// 				(p1[0] == origin[0] && p1[1] == origin[1] && p1[2] == origin[2]) ||
// 				(p2[0] == origin[0] && p2[1] == origin[1] && p2[2] == origin[2])
// 			)
// 		{
// 			m_BoxCorrect = false;
// 		}
// 		else
// 			m_BoxCorrect = true;
	}
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
//----------------------------------------------------------------------------
void mafDicomStudy::RemoveSeries(int seriesID)
{
	cppDEL(m_Series[seriesID]);
	m_Series.erase(m_Series.begin() + seriesID);
}

///////////////////////////////mafDicomSeries//////////////////////////////////
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
		
		//Check dimension accepts rotated images
		if (((dim[0] != m_Dimensions[0]) && (dim[0] != m_Dimensions[1])) ||
			  ((dim[1] != m_Dimensions[1]) && (dim[1] != m_Dimensions[0])))
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
	return !( fabs(dcmImageOrientationPatient[0] - 1.0) < 0.0001 && fabs(dcmImageOrientationPatient[1]) < 0.0001 &&
						fabs(dcmImageOrientationPatient[2])				< 0.0001 && fabs(dcmImageOrientationPatient[3]) < 0.0001 &&
						fabs(dcmImageOrientationPatient[4] - 1.0) < 0.0001 &&	fabs(dcmImageOrientationPatient[5]) < 0.0001 );
}
//----------------------------------------------------------------------------
bool SortSliceCompareFunction(mafDicomSlice *i, mafDicomSlice *j) 
{ 
	return (i->GetUnrotatedPos()[2]<j->GetUnrotatedPos()[2]); 
}
//----------------------------------------------------------------------------
void mafDicomSeries::SortSlices()
{
	std::sort(m_Slices.begin(), m_Slices.end(), SortSliceCompareFunction);
}

/////////////////////////////mafDicomSlice////////////////////////////////
//----------------------------------------------------------------------------
void mafDicomSlice::SetVTKImageData(vtkImageData *data)
{
	vtkDEL(m_Data);
	m_Data = vtkImageData::New();
	m_Data->DeepCopy(data);
}
//----------------------------------------------------------------------------
void mafDicomSlice::CalculateUnrotatedPos()
{
	double rotPos[4] = { m_DcmImagePositionPatient[0], m_DcmImagePositionPatient[1], m_DcmImagePositionPatient[2], 1.0 };

	mafMatrix matr;
	matr.SetFromDirectionCosines(m_DcmImageOrientationPatient);

	double *unRotPos = matr.GetVTKMatrix()->MultiplyDoublePoint(rotPos);
	m_UnrotatedPos[0] = unRotPos[0];
	m_UnrotatedPos[1] = unRotPos[1];
	m_UnrotatedPos[2] = unRotPos[2];
}
