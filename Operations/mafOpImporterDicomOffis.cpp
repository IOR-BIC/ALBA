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

#include "wx/listimpl.cpp"
#include "wx/busyinfo.h"
#include <wx/listctrl.h>

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
#include "mafMatrixPipe.h"

#include "vtkMAFVolumeResample.h"
#include "vtkMAFSmartPointer.h"
#include "vtkDirectory.h"
#include "vtkWindowLevelLookupTable.h"
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
#include "vtkProbeFilter.h"
#include "vtkRenderer.h"
#include "vtkMAFRGSliceAccumulate.h"
#include "vtkTransform.h"
#include "vtkImageData.h"
#include "vtkAppendFilter.h"
#include "vtkExtractVOI.h"
#include "vtkImageClip.h"
#include "vtkImageDataGeometryFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkAppendPolyData.h" 
#include "vtkFloatArray.h"
#include "vtkCellData.h"
#include "vtkIdTypeArray.h"
#include "vtkCellArray.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"
#include "vtkIntArray.h"
#include "vtkFloatArray.h"

#include "vtkDataSetWriter.h"
#include "vtkRectilinearGrid.h"
#include "vtkPointData.h"
#include "vtkShortArray.h"
#include "vtkUnsignedShortArray.h"
#include "vtkCharArray.h"
#include "vtkImageFlip.h"

#include "vtkMath.h"

#include "windows.h"
#include <algorithm>

#include "mmaMaterial.h"
#include "mafGUILutPreset.h"



//BES: 4.7.2009 - VS 2008 cannot compile it due to the following error
//C:\MAF\Medical\Libraries\Offis\Sources\dcmtk-3.5.4\config\include\dcmtk/config/cfwin32.h(362) : error C2371: 'ssize_t' : redefinition; different basic types
//  C:\MAF\openMAF\Libraries\wxWin\Sources\include\wx/defs.h(1018) : see declaration of 'ssize_t'
//this here is to patch it
#if _MSC_VER >= 1500
#define ssize_t VS2008_ssize_t_HACK 
#endif

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#if _MSC_VER >= 1500
#undef VS2008_ssize_t_HACK
#endif

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

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"
#include "mafDicomCardiacMRIHelper.h"
#include "vnl/vnl_vector.h"
#include "time.h"
#include "vtkImageReslice.h"
#include "mafProgressBarHelper.h"

// copied from wx/list.h : needed to make Visual Assist X work correctly 
// with this class (al least in version 10.5.1711)
#define WX_DECLARE_LIST_3(T, Tbase, name, nodetype, classexp)               \
	WX_DECLARE_LIST_4(T, Tbase, name, nodetype, classexp, WX_LIST_PTROP_NONE)
#define WX_DECLARE_LIST_PTR_3(T, Tbase, name, nodetype, classexp)        \
	WX_DECLARE_LIST_4(T, Tbase, name, nodetype, classexp, WX_LIST_PTROP)

#define WX_DECLARE_LIST_2(elementtype, listname, nodename, classexp)        \
	WX_DECLARE_LIST_3(elementtype, elementtype, listname, nodename, classexp)
#define WX_DECLARE_LIST_PTR_2(elementtype, listname, nodename, classexp)        \
	WX_DECLARE_LIST_PTR_3(elementtype, elementtype, listname, nodename, classexp)

#define WX_DECLARE_LIST(elementtype, listname)                              \
	typedef elementtype _WX_LIST_ITEM_TYPE_##listname;                      \
	WX_DECLARE_LIST_2(elementtype, listname, wx##listname##Node, class)
// end copy

WX_DECLARE_LIST(mafDicomSlice, medDicomSeriesSliceList);

void mafOpImporterDicomOffis::PrintDicomList(medDicomSeriesSliceList *dicomList)
{
	std::ostringstream stringStream;

	for (int i = 0; i < dicomList->GetCount(); i++) 
	{
		stringStream << "i: " << i << "  absFilName: "  << \
			dicomList->Item(i)->GetData()->GetSliceABSFileName() << std::endl;    
	}

	mafLogMessage(stringStream.str().c_str());
}
//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpImporterDicomOffis);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// constants :
//----------------------------------------------------------------------------
#define FIRST_SELECTION 0
#define START_PROGRESS_BAR 0


enum VOLUME_SIDE
{
	NON_VALID_SIDE = 0,
	LEFT_SIDE,
	RIGHT_SIDE,
};
enum DICOM_IMPORTER_MODALITY
{
	CROP_SELECTED,
	ADD_CROP_ITEM,
	GIZMO_NOT_EXIST,
	GIZMO_RESIZING,
	GIZMO_DONE
};

enum
{
	TYPE_VOLUME = 0,
	TYPE_IMAGE = 1,
};


/** methods used to sort medDICOMListElement based on custom criteria */
int CompareX(const mafDicomSlice **arg1,const mafDicomSlice **arg2);
int CompareY(const mafDicomSlice **arg1,const mafDicomSlice **arg2);
int CompareZ(const mafDicomSlice **arg1,const mafDicomSlice **arg2);
int CompareTriggerTime(const mafDicomSlice **arg1,const mafDicomSlice **arg2);
int CompareImageNumber(const mafDicomSlice **arg1,const mafDicomSlice **arg2);

WX_DEFINE_LIST(medDicomSeriesSliceList);
//----------------------------------------------------------------------------
mafOpImporterDicomOffis::mafOpImporterDicomOffis(wxString label):
mafOp(label)
	//----------------------------------------------------------------------------
{
	m_OpType = OPTYPE_IMPORTER;
	m_Canundo = true;

	for (int i = 0; i < 6; i++) 
		m_SliceBounds[i] = 0;

	m_ZCropBounds[0] = 0;
	m_ZCropBounds[1] = 0;

	m_PatientPosition = "";

	m_SelectedSeriesID.clear();
	m_SelectedSeriesID.resize(3);

	m_Wizard = NULL;
	m_LoadPage = NULL;
	m_CropPage = NULL;
	m_BuildPage = NULL;
	m_ReferenceSystemPage = NULL;
	m_ImagesGroup = NULL;

	m_BuildGuiLeft = NULL;
	m_ReferenceSystemGuiLeft = NULL;
	m_CropGuiLeft = NULL;
	m_LoadGuiLeft = NULL;
	m_LoadGuiUnderLeft = NULL;
	m_CropGuiCenter = NULL;
	m_LoadGuiCenter = NULL;

	m_TimeScannerLoadPage = NULL;
	m_TimeScannerCropPage = NULL;
	m_TimeScannerBuildPage = NULL;
	m_TimeScannerReferenceSystemPage = NULL;

	m_DICOMDirectoryReader = NULL;
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

	m_NumberOfStudies = 0;
	m_NumberOfSlices = -1;
	m_StudyListbox = NULL;
	//	m_SeriesListbox = NULL;
	m_SeriesListctrl = NULL;
	m_SelectedSeriesSlicesList = NULL;

	m_DicomDirectoryABSFileName = "";
	m_DicomReaderModality = -1;

	m_HighBit = 0;
	m_RescaleIntercept = 0;

	m_BuildStepValue = 0;
	m_OutputType = 0;
	m_RadioButton = 0;

	m_SliceScannerBuildPage = NULL;
	m_SliceScannerReferenceSystemPage = NULL;
	m_SliceScannerCropPage = NULL;
	m_SliceScannerLoadPage = NULL;

	m_TagArray = NULL;

	m_BoxCorrect = false;
	m_CropFlag = false;
	m_CropExecuted = false;

	m_ConstantRotation = true;
	m_ZCrop = true;
	m_SideToBeDragged = 0; 
	m_mem_is_almost_full = false;

	m_GizmoStatus = GIZMO_NOT_EXIST;

	m_Image = NULL;
	m_Volume = NULL;

	m_SortAxes = 2;

	m_VolumeName = "";

	m_VolumeSide=LEFT_SIDE;

	m_DicomModalityListBox = NULL;

	m_DicomInteractor = NULL;

	m_CurrentSlice = VTK_INT_MAX;

	m_ResampleFlag = FALSE;

	m_DiscardPosition = FALSE;

	m_RescaleTo16Bit = FALSE;

	m_ApplyRotation = false;

	m_SelectedReferenceSystem = mafDicomSlice::ID_RS_XY;
	m_GlobalReferenceSystem = mafDicomSlice::ID_RS_XY;
	m_SwapReferenceSystem = FALSE;
	m_SwapAllReferenceSystem = FALSE;
	m_ApplyToAllReferenceSystem = FALSE;

	m_TotalDicomRange[0]=0;
	m_TotalDicomRange[1]=1;

	m_CurrentImageID = 0;

	m_SkipAllNoPosition=false;

	m_Showspacing = 0;

}
//----------------------------------------------------------------------------
mafOpImporterDicomOffis::~mafOpImporterDicomOffis()
	//----------------------------------------------------------------------------
{
	vtkDEL(m_SliceActor);

	mafDEL(m_TagArray);
	mafDEL(m_Image);
	mafDEL(m_Volume);

	m_ImagesGroup = NULL;

}
//----------------------------------------------------------------------------
mafOp *mafOpImporterDicomOffis::Copy()
	//----------------------------------------------------------------------------
{
	mafOpImporterDicomOffis *importer = new mafOpImporterDicomOffis(m_Label);
	importer->m_ResampleFlag = m_ResampleFlag;
	importer->m_DicomDirectoryABSFileName = m_DicomDirectoryABSFileName;
	importer->m_DiscardPosition = m_DiscardPosition;
	importer->m_RescaleTo16Bit = m_RescaleTo16Bit;

	return importer;
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::OpRun()
	//----------------------------------------------------------------------------
{
	m_BuildStepValue = ((mafGUIDicomSettings*)GetSetting())->GetBuildStep();
	m_DiscardPosition = ((mafGUIDicomSettings*)GetSetting())->EnableDiscardPosition();
	m_ResampleFlag = ((mafGUIDicomSettings*)GetSetting())->EnableResampleVolume();
	m_RescaleTo16Bit = ((mafGUIDicomSettings*)GetSetting())->EnableRescaleTo16Bit();

	CreateGui();
	CreateSliceVTKPipeline();

	m_Wizard = new mafGUIWizard(_("DICOM Importer"));
	m_Wizard->SetListener(this);

	CreateLoadPage();
	CreateCropPage();
	CreateBuildPage();
	CreateReferenceSystemPage();
	m_Wizard->SetButtonString("Crop >");
	EnableSliceSlider(false);
	EnableTimeSlider(false);

	//Create a chain between pages
	m_LoadPage->SetNextPage(m_CropPage);
	m_CropPage->SetNextPage(m_BuildPage);
	UpdateReferenceSystemPageConnection();
	m_Wizard->SetFirstPage(m_LoadPage);

	bool result = false;
	bool firstTime = true;
	do 
	{
		if (m_DicomDirectoryABSFileName == "")
		{	
			bool useDefaultFolder = false;
			wxString lastDicomDir = "";
			if (((mafGUIDicomSettings*)GetSetting())->GetUseDefaultDicomFolder() == TRUE && ((mafGUIDicomSettings*)GetSetting())->GetDefaultDicomFolder() != "UNEDFINED_DicomFolder")
			{

				lastDicomDir = ((mafGUIDicomSettings*)GetSetting())->GetDefaultDicomFolder().GetCStr();

				//Check if default folder exist
				if (::wxDirExists(lastDicomDir))
				{
					useDefaultFolder = true;
				}

			}
			else if (((mafGUIDicomSettings*)GetSetting())->GetLastDicomDir() != "UNEDFINED_m_LastDicomDir")
			{
				lastDicomDir = ((mafGUIDicomSettings*)GetSetting())->GetLastDicomDir();
			}
			else if (lastDicomDir == "UNEDFINED_m_LastDicomDir")
			{
				lastDicomDir = mafGetLastUserFolder().c_str();		
			};

			//User should choice a folder
			if (!useDefaultFolder)
			{
				wxDirDialog dialog(m_Wizard->GetParent(),"", lastDicomDir,wxRESIZE_BORDER, m_Wizard->GetPosition());
				dialog.SetReturnCode(wxID_OK);
				int ret_code = dialog.ShowModal();


				if (ret_code == wxID_OK)
				{
					wxString path = dialog.GetPath();
					((mafGUIDicomSettings*)GetSetting())->SetLastDicomDir(path);
					m_DicomDirectoryABSFileName = path.c_str();
					GuiUpdate();
					result = OpenDir();
					if (!result)
					{
						OpStop(OP_RUN_CANCEL);
						return;
					}
				}
				else
				{
					OpStop(OP_RUN_CANCEL);
					return;
				}
			}
			else//Importer use the default folder
			{
				m_DicomDirectoryABSFileName = lastDicomDir.c_str();
				GuiUpdate();
				result = OpenDir();
				if (!result)
				{
					OpStop(OP_RUN_CANCEL);
					return;
				}
			}

		}
		else
		{
			GuiUpdate();
			result = OpenDir();
			if (result == false)
			{
				m_DicomDirectoryABSFileName = "";
			}
		}
	} while(!result);


	int wizardResult = RunWizard();
	OpStop(wizardResult);
}
//----------------------------------------------------------------------------
int mafOpImporterDicomOffis::RunWizard()
	//----------------------------------------------------------------------------
{
	
	if(m_Wizard->Run())
	{
		int result;
		switch (m_OutputType)
		{
		case TYPE_VOLUME: 
			{

				if (m_SeriesIDContainsRotationsMap[m_SelectedSeriesID])
				{
					if(!this->m_TestMode)
					{
							m_ApplyRotation = true;
					}
				}

				//rescale to 16 bit all the rest of the dataset
				if(m_RescaleTo16Bit == TRUE && m_HighBit == 11)
				{
					int i=0, size = m_SelectedSeriesSlicesList->size();

					mafProgressBarHelper progressHelper(m_Listener);
					progressHelper.SetTextMode(m_TestMode);
					progressHelper.InitProgressBar("Conversion to Unsigned Short: please wait...");

					for(int slice_num=0;slice_num<size;slice_num++)
					{
						RescaleTo16Bit(m_SelectedSeriesSlicesList->Item(slice_num)->GetData()->GetVTKImageData());

						progressHelper.UpdateProgressBar(slice_num * 100 / (double)size);	
					}
					progressHelper.CloseProgressBar();
				}

				if(m_DicomReaderModality != mafGUIDicomSettings::ID_CMRI_MODALITY)
					result = BuildOutputVMEGrayVolumeFromDicom();
				else
					result = BuildOutputVMEGrayVolumeFromDicomCineMRI();

				break;
			}
		case TYPE_IMAGE:
			{
				if (m_SeriesIDContainsRotationsMap[m_SelectedSeriesID])
				{
					if(!this->m_TestMode)
					{
						//int answer = wxMessageBox( "Dicom dataset contains rotated images - Apply rotation?", "Warning", wxYES_NO, NULL);
						//if (answer == wxNO)
						//{
						//	m_ApplyRotation = false;
						//}
						//else if (answer == wxYES)
						//{
							m_ApplyRotation = true;
						//}
					}
				}


				if(m_DicomReaderModality != mafGUIDicomSettings::ID_CMRI_MODALITY)
					result = BuildOutputVMEImagesFromDicom();
				else
					result = BuildOutputVMEImagesFromDicomCineMRI();

				// if output is an image
				ApplyReferenceSystem();

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
	//----------------------------------------------------------------------------
{
	if(m_Output != NULL)
	{
		m_Output->ReparentTo(m_Input);
	}
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::OpStop(int result)
	//----------------------------------------------------------------------------
{
	Destroy();
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::Destroy()
	//----------------------------------------------------------------------------
{
	if(m_DicomInteractor)
		m_Mouse->RemoveObserver(m_DicomInteractor);

	if(!this->m_TestMode) {
		//		m_SeriesListbox->Clear();
		m_SeriesListctrl->ClearAll();
	}

	std::map<std::vector<mafString>,medDicomSeriesSliceList*>::iterator it;
	for ( it=m_SeriesIDToSlicesListMap.begin() ; it != m_SeriesIDToSlicesListMap.end(); it++ )
	{
		m_SeriesIDToSlicesListMap[(*it).first]->DeleteContents(TRUE);
		delete m_SeriesIDToSlicesListMap[(*it).first];
	}

	m_SeriesIDToSlicesListMap.clear();

	std::map<std::vector<mafString>,mafDicomCardiacMRIHelper*>::iterator it2;
	for ( it2=m_SeriesIDToCardiacMRIHelperMap.begin() ; it2 != m_SeriesIDToCardiacMRIHelperMap.end(); it2++ )
	{
		cppDEL(m_SeriesIDToCardiacMRIHelperMap[(*it2).first]);
	}

	m_SeriesIDToCardiacMRIHelperMap.clear();

	if(m_LoadPage)
	{
		m_LoadPage->GetRWI()->m_RenFront->RemoveActor(m_SliceActor);
		if(((mafGUIDicomSettings*)GetSetting())->EnableVisualizationPosition())
		{
			m_LoadPage->GetRWI()->m_RenFront->RemoveActor(m_TextActor);
		}
	}

	if(m_CropPage)
	{
		m_CropPage->GetRWI()->m_RenFront->RemoveActor(m_CropActor);  
		if(((mafGUIDicomSettings*)GetSetting())->EnableVisualizationPosition())
		{
			m_LoadPage->GetRWI()->m_RenFront->RemoveActor(m_TextActor);
		}
	}

	if(m_BuildPage)
	{
		m_BuildPage->GetRWI()->m_RenFront->RemoveActor(m_SliceActor);
		if(((mafGUIDicomSettings*)GetSetting())->EnableVisualizationPosition())
		{
			m_LoadPage->GetRWI()->m_RenFront->RemoveActor(m_TextActor);
		}
	}

	if(m_ReferenceSystemPage)
	{
		m_ReferenceSystemPage->GetRWI()->m_RenFront->RemoveActor(m_SliceActor);
		if(((mafGUIDicomSettings*)GetSetting())->EnableVisualizationPosition())
		{
			m_BuildPage->GetRWI()->m_RenFront->RemoveActor(m_TextActor);
		}
	}

	vtkDEL(m_SliceTexture);
	vtkDEL(m_DICOMDirectoryReader);
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

	if(!this->m_TestMode)
	{
		cppDEL(m_LoadGuiLeft);
		cppDEL(m_LoadGuiUnderLeft);
		cppDEL(m_LoadGuiUnderCenter);
		cppDEL(m_CropGuiLeft);
		cppDEL(m_CropGuiCenter);
		cppDEL(m_BuildGuiLeft); 
		cppDEL(m_BuildGuiUnderLeft);
		cppDEL(m_BuildGuiCenter);
		cppDEL(m_ReferenceSystemGuiLeft); 
		cppDEL(m_ReferenceSystemGuiUnderLeft);
		//cppDEL(m_ReferenceSystemGuiCenter);
		cppDEL(m_LoadPage);
		cppDEL(m_CropPage);
		cppDEL(m_BuildPage);
		cppDEL(m_ReferenceSystemPage);
		cppDEL(m_Wizard);
	}
}

//----------------------------------------------------------------------------
int mafOpImporterDicomOffis::BuildOutputVMEImagesFromDicom()
	//----------------------------------------------------------------------------
{
	int step;

	if(m_BuildStepValue == 0)
		step = 1;
	else if (m_BuildStepValue == 1)
		step = m_BuildStepValue << 1;
	else
		step = m_BuildStepValue + 1;

	int cropInterval = (m_ZCropBounds[1]+1 - m_ZCropBounds[0]);
	int n_slices = cropInterval / step;

	if(cropInterval % step != 0)
	{
		n_slices+=1;
	}

	mafProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Building images: please wait...");

	ImportDicomTags();
	int count,s_count;
	mafNEW(m_ImagesGroup);

	m_ImagesGroup->SetName(wxString::Format("%s images",m_VolumeName));
	m_ImagesGroup->ReparentTo(m_Input);

	for (count = m_ZCropBounds[0], s_count = 0; count < m_ZCropBounds[1]+1; count += step)
	{
		if (s_count == n_slices) {break;}
		GenerateSliceTexture(count);

		double spacing[3];
		vtkMAFSmartPointer<vtkImageData> im;
		im->DeepCopy(m_SliceTexture->GetInput());
		if(!this->m_TestMode)
		{
			double scaleFactor;
			mafGUISettingsAdvanced *guiAdv = new mafGUISettingsAdvanced(this);

			if(guiAdv->GetConversionType() == mafGUIDicomSettings::NONE)
			{
				scaleFactor = 1.0;
			}
			else if(guiAdv->GetConversionType() == mafGUIDicomSettings::mm2m)
			{
				scaleFactor = 0.001;
			}
			im->GetSpacing(spacing);
			spacing[0]*=scaleFactor;
			spacing[1]*=scaleFactor;
			spacing[2]*=scaleFactor;
			im->SetSpacing(spacing);
			im->Update();
		}

		mafSmartPointer<mafVMEImage> image;
		wxString name = m_VolumeName;
		name.Append(wxString::Format("_%d", count));
		image->SetName(name.c_str());
		image->SetData(im,0);
		image->GetTagArray()->DeepCopy(m_TagArray);

		
		image->GetMaterial()->m_ColorLut->DeepCopy(m_SliceTexture->GetLookupTable());
		
		mafTagItem tag_Nature;
		tag_Nature.SetName("VME_NATURE");
		tag_Nature.SetValue("NATURAL");
		image->GetTagArray()->SetTag(tag_Nature);

		mafTagItem tag_Surgeon;
		tag_Surgeon.SetName("SURGEON_NAME");
		tag_Surgeon.SetValue(m_SurgeonName.GetCStr());
		image->GetTagArray()->SetTag(tag_Surgeon);

		if(!this->m_TestMode)
		{
			if(((mafGUIDicomSettings*)GetSetting())->EnableChangeSide())
			{
				mafTagItem tagSide;
				tagSide.SetName("VOLUME_SIDE");
				if(m_VolumeSide==LEFT_SIDE)
					tagSide.SetValue("LEFT");
				else if(m_VolumeSide==RIGHT_SIDE)
					tagSide.SetValue("RIGHT");
				else if(m_VolumeSide==NON_VALID_SIDE)
					tagSide.SetValue("NON_VALID_SIDE");
				image->GetTagArray()->SetTag(tagSide);
			}
		}

		m_ImagesGroup->AddChild(image);
		s_count++;

		progressHelper.UpdateProgressBar(count * 100 / m_DICOMDirectoryReader->GetNumberOfFiles());
	}

	if(m_ImagesGroup != NULL)
	{
		m_Output = m_ImagesGroup;
	}

	return OP_RUN_OK;
}

//----------------------------------------------------------------------------
int mafOpImporterDicomOffis::BuildOutputVMEImagesFromDicomCineMRI()
	//----------------------------------------------------------------------------
{
	int step;

	if(m_BuildStepValue == 0)
		step = 1;
	else if (m_BuildStepValue == 1)
		step = m_BuildStepValue << 1;
	else
		step = m_BuildStepValue + 1;

	int cropInterval = (m_ZCropBounds[1]+1 - m_ZCropBounds[0]);
	int n_slices = cropInterval / step;

	if(cropInterval % step != 0)
	{
		n_slices+=1;
	}

	mafProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Building images: please wait...");

	ImportDicomTags();

	mafNEW(m_ImagesGroup);

	m_ImagesGroup->SetName(wxString::Format("%s images",m_VolumeName));
	m_ImagesGroup->ReparentTo(m_Input);

	//create all the animated images
	for (int i = m_ZCropBounds[0]; i < m_ZCropBounds[1]+1;i += step)
	{
		mafSmartPointer<mafVMEImage> image;
		wxString name = m_VolumeName;
		name.Append(wxString::Format("_%d_%d", i, m_NumberOfTimeFrames));
		image->SetName(name.c_str());
		m_ImagesGroup->AddChild(image);
	}

	int totalNumberOfImages = (m_ZCropBounds[1]+1)*m_NumberOfTimeFrames;
	int progressCounter = 0;

	// for every timestamp
	for (int ts = 0; ts < m_NumberOfTimeFrames; ts++)
	{
		progressHelper.UpdateProgressBar(ts * 100 / m_NumberOfTimeFrames);

		// get the time stamp from the dicom tag;
		// timestamp is in ms
		int probeHeigthId = 0;    
		int tsImageId = GetSliceIDInSeries(ts, probeHeigthId);
		if (tsImageId == -1) 
		{
			assert(FALSE);
		}

		mafDicomSlice *element0;
		element0 = (mafDicomSlice *)m_SelectedSeriesSlicesList->\
			Item(tsImageId)->GetData();
		mafTimeStamp dcmTriggerTime = (mafTimeStamp)(element0->GetDcmTriggerTime());

		for (int sourceVolumeSliceId = m_ZCropBounds[0], targetVolumeSliceId = 0; sourceVolumeSliceId < m_ZCropBounds[1]+1; sourceVolumeSliceId += step)
		{
			progressHelper.UpdateProgressBar(progressCounter * 100 / totalNumberOfImages);
			

			if (targetVolumeSliceId == n_slices) {break;}

			// show the current slice
			int currImageId = GetSliceIDInSeries(ts, sourceVolumeSliceId);
			if (currImageId != -1) 
			{
				// update v_texture ivar
				GenerateSliceTexture(currImageId);
			}

			double spacing[3];
			vtkMAFSmartPointer<vtkImageData> im;
			im->DeepCopy(m_SliceTexture->GetInput());
			if(!this->m_TestMode)
			{
				double scaleFactor;
				mafGUISettingsAdvanced *guiAdv = new mafGUISettingsAdvanced(this);

				if(guiAdv->GetConversionType() == mafGUIDicomSettings::NONE)
				{
					scaleFactor = 1.0;
				}
				else if(guiAdv->GetConversionType() == mafGUIDicomSettings::mm2m)
				{
					scaleFactor = 0.001;
				}
				im->GetSpacing(spacing);
				spacing[0]*=scaleFactor;
				spacing[1]*=scaleFactor;
				spacing[2]*=scaleFactor;
				im->SetSpacing(spacing);
				im->Update();
			}

			mafVMEImage *image = NULL;
			image = mafVMEImage::SafeDownCast(m_ImagesGroup->GetChild(targetVolumeSliceId));
			assert(image);	

			image->SetData(im,dcmTriggerTime);

			if (m_SeriesIDContainsRotationsMap[m_SelectedSeriesID] == true  && m_ApplyRotation)
			{
				mafDicomSlice* slice = NULL;

				slice = m_SelectedSeriesSlicesList->Item(currImageId)->GetData();

				assert(slice);

				vtkMatrix4x4 *sliceOrientationMatrix = vtkMatrix4x4::New();

				slice->GetOrientation(sliceOrientationMatrix);

				double slicePosition[3] = {-999, -999, -999};

				slice->GetDcmImagePositionPatient(slicePosition);

				double sliceVtkDataCenter[3] = {-999, -999, -999};

				slice->GetVTKImageData()->GetCenter(sliceVtkDataCenter);

				vtkTransform *tr = vtkTransform::New();
				tr->PostMultiply();
				tr->Translate(-sliceVtkDataCenter[0], -sliceVtkDataCenter[1],-sliceVtkDataCenter[2]);
				tr->Concatenate(sliceOrientationMatrix);
				\
					mafSmartPointer<mafTransform> boxPose;
				boxPose->SetMatrix(tr->GetMatrix());
				boxPose->Update();

				image->SetAbsMatrix(boxPose->GetMatrix(),dcmTriggerTime);

				sliceOrientationMatrix->Delete();
				tr->Delete();

			}

			m_ImagesGroup->GetChild(targetVolumeSliceId)->GetTagArray()->DeepCopy(m_TagArray);

			mafTagItem tag_Nature;
			tag_Nature.SetName("VME_NATURE");
			tag_Nature.SetValue("NATURAL");
			m_ImagesGroup->GetChild(targetVolumeSliceId)->GetTagArray()->SetTag(tag_Nature);

			mafTagItem tag_Surgeon;
			tag_Surgeon.SetName("SURGEON_NAME");
			tag_Surgeon.SetValue(m_SurgeonName.GetCStr());
			m_ImagesGroup->GetChild(targetVolumeSliceId)->GetTagArray()->SetTag(tag_Surgeon);

			if(!this->m_TestMode)
			{
				if(((mafGUIDicomSettings*)GetSetting())->EnableChangeSide())
				{
					mafTagItem tagSide;
					tagSide.SetName("VOLUME_SIDE");
					if(m_VolumeSide==LEFT_SIDE)
						tagSide.SetValue("LEFT");
					else if(m_VolumeSide==RIGHT_SIDE)
						tagSide.SetValue("RIGHT");
					else if(m_VolumeSide==NON_VALID_SIDE)
						tagSide.SetValue("NON_VALID_SIDE");
					m_ImagesGroup->GetChild(targetVolumeSliceId)->GetTagArray()->SetTag(tagSide);
				}
			}
			targetVolumeSliceId++;
			progressCounter++;
		}
	}

	if(m_ImagesGroup != NULL)
	{
		m_Output = m_ImagesGroup;
	}

	return OP_RUN_OK;
}

//----------------------------------------------------------------------------
int mafOpImporterDicomOffis::BuildOutputVMEGrayVolumeFromDicom()
	//----------------------------------------------------------------------------
{
	bool sliceskippedistrue = true;
	std::map <mafString,int> dcm_dim;
	dcm_dim = m_SeriesIDstringToSeriesDimensionMap[m_SelectedSeriesID];

	std::map <mafString,int>::iterator iter;
	iter = dcm_dim.begin();

	int max_value = iter->second;
	mafString str = iter->first;
	for( ; iter != dcm_dim.end(); ++iter) {
		if( iter->second > max_value) {
			max_value = iter->second;
			str = iter->first;
		}
	}
	int dim_img_check[3];
	dim_img_check[2]=1;
	const char * dcm_dim_name = str.GetCStr();
	char bufx[256];
	char bufy[256];
	int ecounter=0;
	int icounter=0;
	while(dcm_dim_name[ecounter] != 'x')
	{
		bufy[icounter] = dcm_dim_name[ecounter];
		ecounter++;
		icounter++;
	}
	ecounter++;
	icounter = 0;
	// x dimension
	dim_img_check[1] = atoi(bufy);
	while(dcm_dim_name[ecounter] != 'x')
	{
		bufx[icounter] = dcm_dim_name[ecounter];
		ecounter++;
		icounter++;
	}
	// x dimension
	dim_img_check[0] = atoi(bufx);

	int step;
	if(m_BuildStepValue == 0)
		step = 1;
	else if (m_BuildStepValue == 1)
		step = m_BuildStepValue << 1;
	else
		step = m_BuildStepValue + 1;

	int cropInterval = (m_ZCropBounds[1]+1 - m_ZCropBounds[0]);

	int n_slices = (cropInterval/ step);
	if(cropInterval % step != 0)
	{
		n_slices+=1;
	}
	 
	int count,s_count;
	int numSliceToSkip = 0;
	bool *sliceToSkip = new bool[n_slices+1];
	for (int i=0;i<n_slices+1;i++)
	{
		sliceToSkip[i] = false;
	}

	int dim_img[3];
//	int dim_img_check[3];
//	bool storedfirstimagedimension = false;
	for (count = m_ZCropBounds[0], s_count = 0; count < m_ZCropBounds[1]+1; count += step)
	{  
//		if(storedfirstimagedimension) 
//		{
			vtkImageData *image = m_SelectedSeriesSlicesList->Item(count)->GetData()->GetVTKImageData();
			image->Update();
			image->GetDimensions(dim_img);
			//for (int i=0;i<3;i++)
			//{
		   //if (dim_img[i] != dim_img_check[i] )
			if ( ( (dim_img[0] != dim_img_check[0]) && (dim_img[0] != dim_img_check[1]) ) ||  
				( (dim_img[1] != dim_img_check[1]) && (dim_img[1] != dim_img_check[0]) ) )
				{
					if(sliceskippedistrue) {
						sliceskippedistrue = false;
					    int result = wxMessageBox(_("Some slices have different dimension! They will be skipped! Do you want to continue?"),"",wxOK|wxCENTRE|wxCANCEL);
					    if (result == wxCANCEL) {
                            return OP_RUN_CANCEL;
					    }
					}

					//wxMessageBox("Some slices have different dimension! They will be skipped!");
					mafLogMessage("SLICE SKIPPED: %d",count);
					numSliceToSkip++;
					sliceToSkip[count] = true;
					break;
				}
			//}
//		}
		//else
		//{
		//	storedfirstimagedimension = true;
		//	vtkImageData *image = m_SelectedSeriesSlicesList->Item(count)->GetData()->GetVTKImageData();
		//	image->Update();
		//	image->GetDimensions(dim_img_check);
		//}
	}
	
	bool applyCorrectionToRotateZcoordinate = false;
	if (m_SeriesIDContainsRotationsMap[m_SelectedSeriesID] == true  && m_ApplyRotation)
	{
		applyCorrectionToRotateZcoordinate = true;
		bool forcedToExit = false;
		bool storedFirstCosinDirector = false;
		double cosinDirectorToCheck[6];
//		int dimensionsToCheck[3];
		for (count = m_ZCropBounds[0], s_count = 0; count < m_ZCropBounds[1]+1; count += step)
		{        
			if (storedFirstCosinDirector)
			{
				double cosinDirector[6];
				m_SelectedSeriesSlicesList->Item(count)->GetData()->GetDcmImageOrientationPatient(cosinDirector);

				for (int i=0;i<6;++i)
				{
					if (cosinDirector[i] != cosinDirectorToCheck[i] && applyCorrectionToRotateZcoordinate)
					{
						int result = wxMessageBox(_("WARNING: slices aren't parallel in Z axis. Do you want force this?"),"",wxOK|wxCENTRE|wxCANCEL);
						if (result == wxOK)
						{
							applyCorrectionToRotateZcoordinate = true;
						}
						else
						{
							applyCorrectionToRotateZcoordinate = false;
						}

						break;
					}
				}

				//int dim[3];
				//vtkImageData *image = m_SelectedSeriesSlicesList->Item(count)->GetData()->GetVTKImageData();
				//image->Update();
				//image->GetDimensions(dim);

				//for (int i=0;i<3;i++)
				//{
				//	if (dim[i] != dimensionsToCheck[i])
				//	{
				//		wxMessageBox("A slice have different dimensions! This slice will be skipped!");
				//		mafLogMessage("SLICE SKIPPED: %d",m_SelectedSeriesSlicesList->Item(count)->GetData()->GetDcmInstanceNumber());

				//		numSliceToSkip++;
				//		sliceToSkip[count] = true;
				//		break;
				//	}
				//}

			}
			else
			{
				m_SelectedSeriesSlicesList->Item(count)->GetData()->GetDcmImageOrientationPatient(cosinDirectorToCheck);

				//vtkImageData *image = m_SelectedSeriesSlicesList->Item(count)->GetData()->GetVTKImageData();
				//image->Update();
				//image->GetDimensions(dimensionsToCheck);

				storedFirstCosinDirector = true;
			}
		}
	}


	mafProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Building volume: please wait...");

	n_slices -= numSliceToSkip;
	vtkMAFSmartPointer<vtkMAFRGSliceAccumulate> accumulate;
	accumulate->SetNumberOfSlices(n_slices);
	accumulate->BuildVolumeOnAxes(m_SortAxes);

	double oldPosTrasformed = -1.0;
	double oldOrigin[3];

	for (count = m_ZCropBounds[0], s_count = 0; count < m_ZCropBounds[1]+1; count += step)
	{
		if (sliceToSkip[count-m_ZCropBounds[0]])
		{
			//break;
			continue;
		}

		if (s_count == n_slices) {
			break;
		}

		GenerateSliceTexture(count);

		if (applyCorrectionToRotateZcoordinate)
		{
			if (s_count != 0)
			{
				double originSlice[3];
				m_SelectedSeriesSlicesList->Item(count)->GetData()->GetDcmImagePositionPatientOriginal(originSlice);

				vtkImageData *im = m_SliceTexture->GetInput();
				im->Update();

				double originTexture[3];
				im->GetOrigin(originTexture);

				double newPosTransformed;

				double spacing = sqrt(vtkMath::Distance2BetweenPoints(originSlice,oldOrigin));

				mafLogMessage("SPACING: %3f",spacing);

				if (oldOrigin[m_SortAxes] < originSlice[m_SortAxes])
				{
					newPosTransformed = oldPosTrasformed + spacing;
				}
				else
				{
					newPosTransformed = oldPosTrasformed - spacing;
				}

				oldPosTrasformed = newPosTransformed;

				for (int i=0;i<3;++i)
				{
					oldOrigin[i] = originSlice[i];
				}

				originTexture[m_SortAxes] = newPosTransformed;

				im->SetOrigin(originTexture);
				im->Update();
			}
			else
			{
				double originSlice[3];
				m_SelectedSeriesSlicesList->Item(count)->GetData()->GetDcmImagePositionPatientOriginal(originSlice);

				oldPosTrasformed = originSlice[m_SortAxes];

				for (int i=0;i<3;++i)
				{
					oldOrigin[i] = originSlice[i];
				}
			}
		}

		double orientation[6] = {1.0,0.0,0.0,0.0,1.0,0.0};
		m_SelectedSeriesSlicesList->Item(m_ZCropBounds[0])->GetData()->GetDcmImageOrientationPatient(orientation);

		accumulate->SetSlice(s_count,m_SliceTexture->GetInput(), orientation);
		s_count++;

		progressHelper.UpdateProgressBar(count * 100 / (m_ZCropBounds[1]+1));
	}

	delete []sliceToSkip;

	mafNEW(m_Volume);

	accumulate->Update();

	//to reduce the use of memory bug #2305
	//vtkMAFSmartPointer<vtkRectilinearGrid> rg_out;
	vtkRectilinearGrid *rg_out;
	rg_out=accumulate->GetOutput();
	rg_out->Update();

	double b[6];
	rg_out->GetBounds(b);

	if(!this->m_TestMode)
	{
		mafGUISettingsAdvanced *guiAdv = new mafGUISettingsAdvanced(this);

		double scaleFactor;
		if(guiAdv->GetConversionType() == mafGUIDicomSettings::NONE)
		{
			scaleFactor = 1.0;
		}
		else if(guiAdv->GetConversionType() == mafGUIDicomSettings::mm2m)
		{
			scaleFactor = 0.001;
		}

		vtkDataArray *daVector[3] = {rg_out->GetXCoordinates(), rg_out->GetYCoordinates(), rg_out->GetZCoordinates()};

		for (int arrayId = 0; arrayId<3; arrayId ++)
		{
			int numTuples = daVector[arrayId]->GetNumberOfTuples();
			for (int tupleId = 0; tupleId<numTuples; tupleId++)
			{
				double oldVal = daVector[arrayId]->GetComponent(tupleId, 0);
				daVector[arrayId]->SetComponent(tupleId, 0, oldVal * scaleFactor);
			}
		}
		rg_out->Modified();
		delete guiAdv;
	}
	m_Volume->SetDataByDetaching(rg_out,0);

	//if (m_SeriesIDContainsRotationsMap[m_SelectedSeriesID] == true && m_ApplyRotation)
	//{

		double orientation[6] = {1.0,0.0,0.0,0.0,1.0,0.0};
		m_SelectedSeriesSlicesList->Item(m_ZCropBounds[0])->GetData()->GetDcmImageOrientationPatient(orientation);

		//transform direction cosines to be used to set vtkMatrix
		/*
		[ orientation[0] orientation[3] dst_nrm_dircos_x  0 ] 
		[ orientation[1] orientation[4] dst_nrm_dircos_y  0 ]
		[ orientation[2] orientation[5] dst_nrm_dircos_z  0 ]
		[ 0                 0                 0           1 ]*/

		double dst_nrm_dircos_x = orientation[1] * orientation[5] - orientation[2] * orientation[4]; 
		double dst_nrm_dircos_y = orientation[2] * orientation[3] - orientation[0] * orientation[5];
		double dst_nrm_dircos_z = orientation[0] * orientation[4] - orientation[1] * orientation[3]; 

		vtkMatrix4x4 *mat = vtkMatrix4x4::New();
		mat->Identity();

		mat->SetElement(0,0,orientation[0]);
		mat->SetElement(1,0,orientation[1]);
		mat->SetElement(2,0,orientation[2]);
		mat->SetElement(3,0,0);
		mat->SetElement(0,1,orientation[3]);
		mat->SetElement(1,1,orientation[4]);
		mat->SetElement(2,1,orientation[5]);
		mat->SetElement(3,1,0);
		mat->SetElement(0,2,dst_nrm_dircos_x);
		mat->SetElement(1,2,dst_nrm_dircos_y);
		mat->SetElement(2,2,dst_nrm_dircos_z);
		mat->SetElement(3,2,0);
		mat->SetElement(3,3,1);

		mafSmartPointer<mafTransform> boxPose;
		boxPose->SetMatrix(mat);     
		boxPose->Update();
		m_Volume->SetAbsMatrix(boxPose->GetMatrix());
	//}

	if(m_ResampleFlag == TRUE)
	{
		ResampleVolume();
	}

	ImportDicomTags(); 
	//Copy inside the first VME item of m_Volume the CT volume and Dicom's tags
	m_Volume->GetTagArray()->DeepCopy(m_TagArray);
	mafDEL(m_TagArray);
	mafTagItem tag_Nature;
	tag_Nature.SetName("VME_NATURE");
	tag_Nature.SetValue("NATURAL");
	m_Volume->GetTagArray()->SetTag(tag_Nature);

	mafTagItem tag_Surgeon;
	tag_Surgeon.SetName("SURGEON_NAME");
	tag_Surgeon.SetValue(m_SurgeonName.GetCStr());
	m_Volume->GetTagArray()->SetTag(tag_Surgeon);

	if(!this->m_TestMode)
	{
		if(((mafGUIDicomSettings*)GetSetting())->EnableChangeSide())
		{
			mafTagItem tagSide;
			tagSide.SetName("VOLUME_SIDE");
			if(m_VolumeSide==LEFT_SIDE)
				tagSide.SetValue("LEFT");
			else if(m_VolumeSide==RIGHT_SIDE)
				tagSide.SetValue("RIGHT");
			else if(m_VolumeSide==NON_VALID_SIDE)
				tagSide.SetValue("NON_VALID_SIDE");
			m_Volume->GetTagArray()->SetTag(tagSide);
		}
	}

	m_Volume->SetName(m_VolumeName);  

	if(m_Volume != NULL)
	{
		m_Output = m_Volume;
	}
	return OP_RUN_OK;
}

//----------------------------------------------------------------------------
int mafOpImporterDicomOffis::BuildOutputVMEGrayVolumeFromDicomCineMRI()
	//----------------------------------------------------------------------------
{
	int step;
	if(m_BuildStepValue == 0)
		step = 1;
	else if (m_BuildStepValue == 1)
		step = m_BuildStepValue << 1;
	else
		step = m_BuildStepValue + 1;

	int cropInterval = (m_ZCropBounds[1]+1 - m_ZCropBounds[0]);
	int n_slices = cropInterval / step;
	if(cropInterval % step != 0)
	{
		n_slices+=1;
	}

	mafProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Building volume: please wait...");

	// create the time varying vme
	mafNEW(m_Volume);
	int currImageId = 0;
	int totalNumberOfImages = (m_ZCropBounds[1]+1)*m_NumberOfTimeFrames;
	int progressCounter = 0;

	int numSliceToSkip = 0;
	bool *sliceToSkip = new bool[n_slices];
	for (int i=0;i<n_slices;i++)
	{
		sliceToSkip[i] = false;
	}

	// for every timestamp

	bool applyCorrectionToRotateZcoordinate = false;
	if (m_SeriesIDContainsRotationsMap[m_SelectedSeriesID] == true  && m_ApplyRotation)
	{
		applyCorrectionToRotateZcoordinate = true;
		bool forcedToExit = false;
		bool storedFirstCosinDirector = false;
		double cosinDirectorToCheck[6];
		for (int ts = 0; ts < m_NumberOfTimeFrames; ts++)
		{
			int dimensionsToCheck[3];
			int probeHeigthId = 0;
			for (int sourceVolumeSliceId = m_ZCropBounds[0], targetVolumeSliceId = 0; sourceVolumeSliceId < m_ZCropBounds[1]+1; sourceVolumeSliceId += step)
			{
				// show the current slice
				currImageId = GetSliceIDInSeries(ts, probeHeigthId);

				if (storedFirstCosinDirector)
				{
					double cosinDirector[6];
					m_SelectedSeriesSlicesList->Item(currImageId)->GetData()->GetDcmImageOrientationPatient(cosinDirector);

					for (int i=0;i<6;++i)
					{
						if (cosinDirector[i] != cosinDirectorToCheck[i] && applyCorrectionToRotateZcoordinate)
						{
							int result = wxMessageBox(_("WARNING: slices aren't parallel in Z axis. Do you want force this?"),"",wxOK|wxCENTRE|wxCANCEL);
							if (result == wxOK)
							{
								applyCorrectionToRotateZcoordinate = true;
							}
							else
							{
								applyCorrectionToRotateZcoordinate = false;
							}

							break;
						}
					}

					int dim[3];
					vtkImageData *image = m_SelectedSeriesSlicesList->Item(currImageId)->GetData()->GetVTKImageData();
					image->Update();
					image->GetDimensions(dim);

					if (!sliceToSkip[sourceVolumeSliceId])
					{
						for (int i=0;i<3;i++)
						{
							if (dim[i] != dimensionsToCheck[i])
							{
								wxMessageBox("A slice have different dimensions! This slice will be skipped!");
								mafLogMessage("SLICE SKIPPED: %d",m_SelectedSeriesSlicesList->Item(currImageId)->GetData()->GetDcmInstanceNumber());

								numSliceToSkip++;
								sliceToSkip[sourceVolumeSliceId] = true;
								break;
							}
						}
					}

				}
				else
				{
					m_SelectedSeriesSlicesList->Item(currImageId)->GetData()->GetDcmImageOrientationPatient(cosinDirectorToCheck);

					vtkImageData *image = m_SelectedSeriesSlicesList->Item(currImageId)->GetData()->GetVTKImageData();
					image->Update();
					image->GetDimensions(dimensionsToCheck);

					storedFirstCosinDirector = true;
				}

				targetVolumeSliceId++;
				probeHeigthId++;

			}

		}
	}

	for (int ts = 0; ts < m_NumberOfTimeFrames; ts++)
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
		if (tsImageId == -1) 
		{
			assert(FALSE);
		}
		mafDicomSlice *element0;
		element0 = (mafDicomSlice *)m_SelectedSeriesSlicesList->Item(tsImageId)->GetData();
		mafTimeStamp tsDouble = (mafTimeStamp)(element0->GetDcmTriggerTime());

		double oldPosTrasformed = -1.0;
		double oldOrigin[3];

		for (int sourceVolumeSliceId = m_ZCropBounds[0], targetVolumeSliceId = 0; sourceVolumeSliceId < m_ZCropBounds[1]+1; sourceVolumeSliceId += step)
		{
			progressHelper.UpdateProgressBar(progressCounter * 100 / totalNumberOfImages);
			
			if (targetVolumeSliceId == n_slices) {break;}

			// show the current slice
			currImageId = GetSliceIDInSeries(ts, probeHeigthId);

			if (currImageId != -1) 
			{
				// update v_texture ivar
				GenerateSliceTexture(currImageId);
			}

			if (applyCorrectionToRotateZcoordinate)
			{
				if (targetVolumeSliceId != 0)
				{
					double originSlice[3];
					m_SelectedSeriesSlicesList->Item(currImageId)->GetData()->GetDcmImagePositionPatientOriginal(originSlice);

					vtkImageData *im = m_SliceTexture->GetInput();
					im->Update();

					double originTexture[3];
					im->GetOrigin(originTexture);

					double newPosTransformed;

					double spacing = sqrt(vtkMath::Distance2BetweenPoints(originSlice,oldOrigin));

#ifdef _DEBUG
					mafLogMessage("SPACING: %3f",spacing);
#endif // _DEBUG

					if (oldOrigin[m_SortAxes] < originSlice[m_SortAxes])
					{
						newPosTransformed = oldPosTrasformed + spacing;
					}
					else
					{
						newPosTransformed = oldPosTrasformed - spacing;
					}

					oldPosTrasformed = newPosTransformed;

					for (int i=0;i<3;++i)
					{
						oldOrigin[i] = originSlice[i];
					}

					originTexture[m_SortAxes] = newPosTransformed;

					im->SetOrigin(originTexture);
					im->Update();
				}
				else
				{
					double originSlice[3];
					m_SelectedSeriesSlicesList->Item(currImageId)->GetData()->GetDcmImagePositionPatientOriginal(originSlice);

					oldPosTrasformed = originSlice[m_SortAxes];

					for (int i=0;i<3;++i)
					{
						oldOrigin[i] = originSlice[i];
					}
				}
			}

			vtkImageData *imageData = NULL;
			imageData = m_SliceTexture->GetInput();
			assert(imageData);

			vtkImageData *bufferImageData = vtkImageData::New();
			bufferImageData->DeepCopy(imageData);
			imageDataVector.push_back(bufferImageData);

			std::ostringstream stringStream;

			double spacing[3] = {0,0,0};
			imageData->GetSpacing(spacing);

			int dim[3] = {0,0,0};
			imageData->GetDimensions(dim);

			double origin[3] =  {0,0,0};
			imageData->GetOrigin(origin);

			stringStream <<  "ts: " << ts << \
				"  probeHeightId: " << probeHeigthId<< \
				"  currImageId: " << currImageId << \
				"  origin: " << origin[0] << " " << origin[1] << " " << origin[2] << " " <<\
				"  spacing: " << spacing[0]<< " " << spacing[1] << " " << spacing[2]<< " "
				"  dimensions: " << dim[0]<< " " << dim[1] << " " << dim[2]<< " " << std::endl;          

			mafLogMessage(stringStream.str().c_str());

			targetVolumeSliceId++;
			progressCounter++;
			probeHeigthId++;

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
		accumulator->SetNumberOfSlices(m_NumberOfSlices - numSliceToSkip);

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

			double orientation[6] = {1.0,0.0,0.0,0.0,1.0,0.0};
			m_SelectedSeriesSlicesList->Item(m_ZCropBounds[0])->GetData()->GetDcmImageOrientationPatient(orientation);
			accumulator->SetSlice(i, imageDataVector[currentMapElement->second],orientation);
			i++;
		}

		accumulator->Update();

		vtkMAFSmartPointer<vtkRectilinearGrid> rg_out;
		rg_out->DeepCopy(accumulator->GetOutput());
		rg_out->Update();

		if(!this->m_TestMode)
		{
			mafGUISettingsAdvanced *guiAdv = new mafGUISettingsAdvanced(this);

			double scaleFactor;
			if(guiAdv->GetConversionType() == mafGUIDicomSettings::NONE)
			{
				scaleFactor = 1.0;
			}
			else if(guiAdv->GetConversionType() == mafGUIDicomSettings::mm2m)
			{
				scaleFactor = 0.001;
			}

			vtkDataArray *daVector[3] = {rg_out->GetXCoordinates(), rg_out->GetYCoordinates(), rg_out->GetZCoordinates()};

			for (int arrayId = 0; arrayId<3; arrayId ++)
			{

				int numTuples = daVector[arrayId]->GetNumberOfTuples();

				for (int tupleId = 0; tupleId<numTuples; tupleId++)
				{
					double oldVal = daVector[arrayId]->GetComponent(tupleId, 0);
					daVector[arrayId]->SetComponent(tupleId, 0, oldVal * scaleFactor);
				}
			}
			rg_out->Modified();
		}
		m_Volume->SetDataByDetaching(rg_out,tsDouble);
		m_Volume->Update();

		if(m_ResampleFlag == TRUE)
		{
			ResampleVolume();
		}

		if (m_SeriesIDContainsRotationsMap[m_SelectedSeriesID] == true  && m_ApplyRotation)
		{

			mafDicomSlice* slice = NULL;

			slice = m_SelectedSeriesSlicesList->Item(m_ZCropBounds[0])->GetData();

			assert(slice);

			vtkMatrix4x4 *mat = vtkMatrix4x4::New();

			slice->GetOrientation(mat);

			mafSmartPointer<mafTransform> boxPose;
			boxPose->SetMatrix(mat);

			double pos[3];
			slice->GetDcmImagePositionPatient(pos);

			boxPose->SetPosition(pos);
			boxPose->Update();

			m_Volume->SetAbsMatrix(boxPose->GetMatrix(),tsDouble);

			mat->Delete();
		}


		for (int i = 0; i < imageDataVector.size(); i++)
		{
			imageDataVector[i]->Delete();
		}

		imageDataVector.clear();

	}

	delete []sliceToSkip;

	// update m_tag_array ivar
	ImportDicomTags();
	m_Volume->GetTagArray()->DeepCopy(m_TagArray);
	vtkDEL(m_TagArray);

	mafTagItem tag_Nature;
	tag_Nature.SetName("VME_NATURE");
	tag_Nature.SetValue("NATURAL");
	m_Volume->GetTagArray()->SetTag(tag_Nature);

	mafTagItem tag_Surgeon;
	tag_Surgeon.SetName("SURGEON_NAME");
	tag_Surgeon.SetValue(m_SurgeonName.GetCStr());
	m_Volume->GetTagArray()->SetTag(tag_Surgeon);

	if(!this->m_TestMode)
	{
		if(((mafGUIDicomSettings*)GetSetting())->EnableChangeSide())
		{
			mafTagItem tagSide;
			tagSide.SetName("VOLUME_SIDE");
			if(m_VolumeSide==LEFT_SIDE)
				tagSide.SetValue("LEFT");
			else if(m_VolumeSide==RIGHT_SIDE)
				tagSide.SetValue("RIGHT");
			else if(m_VolumeSide==NON_VALID_SIDE)
				tagSide.SetValue("NON_VALID_SIDE");
			m_Volume->GetTagArray()->SetTag(tagSide);
		}
	}

	m_Volume->SetName(m_VolumeName);

	if(m_Volume != NULL)
	{
		m_Output = m_Volume;
	}
	return OP_RUN_OK;
}


//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::CreateLoadPage()
	//----------------------------------------------------------------------------
{
	m_LoadPage = new mafGUIWizardPageNew(m_Wizard,medUSEGUI|medUSERWI);
	m_LoadGuiLeft = new mafGUI(this);
	m_LoadGuiUnderLeft = new mafGUI(this);
	m_LoadGuiUnderCenter = new mafGUI(this);
	m_Showspacing = 0;
	m_LoadGuiCenter = new mafGUI(this);
	if(((mafGUIDicomSettings*)GetSetting())->GetShowAdvancedOptionSorting() == TRUE)
	{
		wxString choices[3] = {_("X"),_("Y"),_("Z")};
		m_LoadGuiCenter->Radio(ID_SORT_AXIS,_("Sort type:"),&m_SortAxes,3,choices);
		m_LoadGuiCenter->Bool(ID_SHOW_SP, "Uniform spacing", &m_Showspacing, 1, _("Uniform spacing"));
		m_LoadGuiCenter->Enable(ID_SHOW_SP,false);
		m_LoadGuiCenter->Label("");
	}

	m_SliceScannerLoadPage=m_LoadGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,m_CurrentSlice,"",((mafGUIDicomSettings*)GetSetting())->EnableNumberOfSlice() != FALSE);
	m_SliceScannerLoadPage->SetPageSize(1);
	if(((mafGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
	{
		m_TimeScannerLoadPage=m_LoadGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,VTK_INT_MAX);
		m_TimeScannerLoadPage->SetPageSize(1);
	}

	m_StudyListbox  = m_LoadGuiUnderLeft->ListBox(ID_STUDY_SELECT,_("study"),80,"",wxLB_HSCROLL,190);
	//	m_StudyListctrl = m_LoadGuiUnderLeft->ListCtrl(ID_STUDY_SELECT,_("study"),80,"",wxLC_LIST|wxLC_SINGLE_SEL|wxLC_SORT_ASCENDING,190);

	//  m_SeriesListbox  = m_LoadGuiUnderCenter->ListBox(ID_SERIES_SELECT,_("series"),80,"",wxLB_HSCROLL|wxLB_SORT,190);
	//	m_SeriesListctrl = m_LoadGuiUnderCenter->ListCtrl(ID_SERIES_SELECT,_("series"),80,"",wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_SORT_ASCENDING,190);
	m_SeriesListctrl = m_LoadGuiUnderCenter->ListCtrl(ID_SERIES_SELECT,_("series"),80,"",wxLC_SMALL_ICON|wxLC_SINGLE_SEL,190);

	m_LoadGuiLeft->FitGui();
	m_LoadGuiUnderLeft->FitGui();
	m_LoadGuiUnderCenter->FitGui();
	m_LoadPage->AddGuiLowerLeft(m_LoadGuiLeft);
	m_LoadPage->AddGuiLowerUnderLeft(m_LoadGuiUnderLeft);
	m_LoadPage->AddGuiLowerUnderCenter(m_LoadGuiUnderCenter);
	m_LoadPage->AddGuiLowerCenter(m_LoadGuiCenter);

	m_LoadPage->GetRWI()->m_RwiBase->SetMouse(m_Mouse);
	m_LoadPage->GetRWI()->m_RenFront->AddActor(m_SliceActor);
	if(((mafGUIDicomSettings*)GetSetting())->EnableVisualizationPosition())
	{
		m_LoadPage->GetRWI()->m_RenFront->AddActor(m_TextActor);
	}
}

int wxCALLBACK _myCompareFunction(long item1, long item2, long WXUNUSED(sortData))
{
	// inverse the order
	if (item1 < item2)
		return -1;
	if (item1 > item2)
		return 1;

	return 0;
}


//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::CreateCropPage()
	//----------------------------------------------------------------------------
{
	m_ZCrop = ((mafGUIDicomSettings*)GetSetting())->EnableZCrop() != FALSE;
	m_CropPage = new mafGUIWizardPageNew(m_Wizard,medUSEGUI|medUSERWI,m_ZCrop);
	m_CropPage->SetListener(this);
	m_CropGuiLeft = new mafGUI(this);
	m_CropGuiCenter = new mafGUI(this);

	m_SliceScannerCropPage=m_CropGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,VTK_INT_MAX,"",((mafGUIDicomSettings*)GetSetting())->EnableNumberOfSlice() != FALSE);
	m_SliceScannerCropPage->SetPageSize(1);
	if(((mafGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
	{
		m_TimeScannerCropPage=m_CropGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,VTK_INT_MAX);
		m_TimeScannerCropPage->SetPageSize(1);
	}

	if(((mafGUIDicomSettings*)GetSetting())->EnableChangeSide())
	{
		wxString sideChoices[3] = {_("non-valid"),_("Left"),_("Right")};
		m_CropGuiCenter->Combo(ID_VOLUME_SIDE,_("volume side"),&m_VolumeSide,3,sideChoices);
	}

	m_CropGuiLeft->FitGui();
	m_CropGuiCenter->FitGui();
	m_CropPage->AddGuiLowerLeft(m_CropGuiLeft);
	m_CropPage->AddGuiLowerCenter(m_CropGuiCenter);

	m_CropPage->GetRWI()->m_RwiBase->SetMouse(m_Mouse);
	m_CropPage->GetRWI()->m_RenFront->AddActor(m_SliceActor);
	m_CropPage->GetRWI()->m_RenFront->AddActor(m_CropActor);
	if(((mafGUIDicomSettings*)GetSetting())->EnableVisualizationPosition())
	{
		m_CropPage->GetRWI()->m_RenFront->AddActor(m_TextActor);
	}
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::CreateBuildPage()
	//----------------------------------------------------------------------------
{
	m_BuildPage = new mafGUIWizardPageNew(m_Wizard,medUSEGUI|medUSERWI);
	m_BuildGuiLeft = new mafGUI(this);
	m_BuildGuiUnderLeft = new mafGUI(this);
	m_BuildGuiCenter = new mafGUI(this);

	m_SliceScannerBuildPage=m_BuildGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,VTK_INT_MAX,"",((mafGUIDicomSettings*)GetSetting())->EnableNumberOfSlice() != FALSE);
	m_SliceScannerBuildPage->SetPageSize(1);

	m_TimeScannerBuildPage=m_BuildGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,VTK_INT_MAX);
	m_TimeScannerBuildPage->SetPageSize(1);

	m_BuildGuiCenter->Divider();
	if(((mafGUIDicomSettings*)GetSetting())->AutoVMEType())
		m_OutputType = ((mafGUIDicomSettings*)GetSetting())->GetVMEType(); 
	else
	{
		// Handles various types of Vme selected in the DICOM Advanced Settings:
		// If the user launch an event by changing the radio button the right value is adjusted later on. (Brazzale, 27.07.2010)
		bool type_volume = ((mafGUIDicomSettings*)GetSetting())->EnableToRead("VOLUME");
		bool type_image = ((mafGUIDicomSettings*)GetSetting())->EnableToRead("IMAGE");
		wxString typeArrayVolumeImage[2] = { _("Volume"),_("Image")};
		if ((type_volume && type_image) || (!type_volume && !type_image))
		{
			m_BuildGuiCenter->Radio(ID_VME_TYPE, "VME output", &m_OutputType, 2, typeArrayVolumeImage, 1, ""/*, wxRA_SPECIFY_ROWS*/);
		}
		else if (type_volume && !type_image)
		{
			m_OutputType = 0;
			m_BuildGuiCenter->Radio(ID_VME_TYPE, "VME output", &m_OutputType, 1, typeArrayVolumeImage, 1, ""/*, wxRA_SPECIFY_ROWS*/);
			m_BuildGuiCenter->Enable(ID_VME_TYPE,0);
		}
		else if (!type_volume &&  type_image)
		{
			m_OutputType = 1;
			m_BuildGuiCenter->Radio(ID_VME_TYPE, "VME output", &m_RadioButton, 1, typeArrayVolumeImage+1, 1, ""/*, wxRA_SPECIFY_ROWS*/);
			m_BuildGuiCenter->Enable(ID_VME_TYPE,0);
		}
	}

	if(((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::TRADITIONAL)
	{
		m_BuildGuiUnderLeft->String(ID_VOLUME_NAME," VME name",&m_VolumeName);
	}
//	else if (((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::DESCRIPTION_DATE)
	else if (((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::DESCRIPTION_NUMSLICES)
	{
		/*m_VolumeName = m_SelectedSeriesSlicesList->Item(0)->GetData()->GetDescription();
		m_VolumeName << "_";
		m_VolumeName << m_SelectedSeriesSlicesList->Item(0)->GetData()->GetDate();*/
		m_BuildGuiUnderLeft->String(ID_VOLUME_NAME," VME name",&m_VolumeName);
	}
	else if (((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::CUSTOM)
	{
		m_BuildGuiUnderLeft->String(ID_VOLUME_NAME," VME name",&m_VolumeName);
	}


	m_BuildGuiLeft->FitGui();
	m_BuildGuiUnderLeft->FitGui();
	m_BuildPage->AddGuiLowerLeft(m_BuildGuiLeft);
	m_BuildPage->AddGuiLowerCenter(m_BuildGuiUnderLeft);

	m_BuildPage->GetRWI()->m_RwiBase->SetMouse(m_Mouse);
	m_BuildPage->GetRWI()->m_RenFront->AddActor(m_SliceActor);
	if(((mafGUIDicomSettings*)GetSetting())->EnableVisualizationPosition())
	{
		m_BuildPage->GetRWI()->m_RenFront->AddActor(m_TextActor);
	}
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::CreateReferenceSystemPage()
	//----------------------------------------------------------------------------
{
	m_ReferenceSystemPage = new mafGUIWizardPageNew(m_Wizard,medUSEGUI|medUSERWI);
	m_ReferenceSystemGuiLeft = new mafGUI(this);
	m_ReferenceSystemGuiUnderLeft = new mafGUI(this);
	//m_ReferenceSystemGuiCenter = new mafGUI(this);

	m_SliceScannerReferenceSystemPage=m_ReferenceSystemGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,VTK_INT_MAX,"",((mafGUIDicomSettings*)GetSetting())->EnableNumberOfSlice() != FALSE);
	m_SliceScannerReferenceSystemPage->SetPageSize(1);

	m_TimeScannerReferenceSystemPage=m_ReferenceSystemGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,VTK_INT_MAX);
	m_TimeScannerReferenceSystemPage->SetPageSize(1);

	wxString choices[3];
	choices[0] = _T("XY");
	choices[1] = _T("XZ");
	choices[2] = _T("YZ");
	//m_ReferenceSystemGuiCenter->Radio(ID_RS_SELECT,"Ref.Sys.",&m_SelectedReferenceSystem,3,choices,1,"Select reference system\nfor image position and orientation.");
	//m_ReferenceSystemGuiCenter->Divider();
	m_ReferenceSystemGuiUnderLeft->Radio(ID_RS_SELECT,"ref.sys.",&m_SelectedReferenceSystem,3,choices,1,"Select reference system for image position and orientation.");
	m_ReferenceSystemGuiUnderLeft->Bool(ID_RS_SWAP,"swap",&m_SwapReferenceSystem,1,"specify if the reference system is swapped or not (e.g. xy to yx)");
	m_ReferenceSystemGuiUnderLeft->Bool(ID_RS_SWAPALL,"swap all",&m_SwapAllReferenceSystem,1,"specify if the reference system is swapped or not (e.g. xy to yx)");
	m_ReferenceSystemGuiUnderLeft->Bool(ID_RS_APPLYTOALL,"apply to all",&m_ApplyToAllReferenceSystem,1,"specify if the current reference system is applied to all images");
	m_ReferenceSystemGuiLeft->FitGui();
	m_ReferenceSystemGuiUnderLeft->FitGui();
	m_ReferenceSystemPage->AddGuiLowerLeft(m_ReferenceSystemGuiLeft);
	m_ReferenceSystemPage->AddGuiLowerCenter(m_ReferenceSystemGuiUnderLeft);

	m_ReferenceSystemPage->GetRWI()->m_RwiBase->SetMouse(m_Mouse);
	m_ReferenceSystemPage->GetRWI()->m_RenFront->AddActor(m_SliceActor);
	if(((mafGUIDicomSettings*)GetSetting())->EnableVisualizationPosition())
	{
		m_ReferenceSystemPage->GetRWI()->m_RenFront->AddActor(m_TextActor);
	}
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::GuiUpdate()
	//----------------------------------------------------------------------------
{
	m_LoadGuiLeft->Update();
	m_LoadGuiUnderLeft->Update();

	m_CropGuiLeft->Update();
	m_CropGuiCenter->Update();

	m_BuildGuiLeft->Update();
	m_BuildGuiUnderLeft->Update();
	m_BuildGuiCenter->Update();

	m_ReferenceSystemGuiLeft->Update();
	m_ReferenceSystemGuiUnderLeft->Update();
	//m_ReferenceSystemGuiCenter->Update();

	m_LoadGuiCenter->Update();
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::CreateGui()
	//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mafOpImporterDicomOffis::OpenDir()
	//----------------------------------------------------------------------------
{
	ResetStructure();

	wxBusyCursor *busyCursor = NULL; 
	if (!m_TestMode)
	{
		busyCursor = new wxBusyCursor();
	}

	bool successful = BuildDicomFileList(m_DicomDirectoryABSFileName.GetCStr());

	cppDEL(busyCursor);

	if (!successful)
	{
		return false;
	}

	if(!this->m_TestMode)
	{
		if(m_NumberOfStudies>0)
		{
			m_StudyListbox->SetSelection(FIRST_SELECTION);
			//			m_StudyListctrl->SetItemState(FIRST_SELECTION, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);

			UpdateStudyListBox();

			OnEvent(&mafEvent(this, ID_STUDY_SELECT));

			if(((mafGUIDicomSettings*)GetSetting())->AutoCropPosition())
			{
				AutoPositionCropPlane();
			}
			else
			{
				m_CropPlane->SetOrigin(0.0,0.0,0.0);
				m_CropPlane->SetPoint1(m_SliceBounds[1]-m_SliceBounds[0],0.0,0.0);
				m_CropPlane->SetPoint2(0.0,m_SliceBounds[3]-m_SliceBounds[2],0.0);
				m_CropPage->GetRWI()->CameraReset();
			}
			m_BoxCorrect=true;
			m_LoadPage->GetRWI()->CameraReset();

			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::ReadDicom() 
	//----------------------------------------------------------------------------
{
	if (this->m_TestMode)
	{
		m_SelectedSeriesID = m_SeriesIDToSlicesListMap.begin()->first;
	}

	m_SelectedSeriesSlicesList = m_SeriesIDToSlicesListMap[m_SelectedSeriesID];


	std::map <mafString,int> dcm_dim;
	dcm_dim = m_SeriesIDstringToSeriesDimensionMap[m_SelectedSeriesID];

	std::map <mafString,int>::iterator iter;
	iter = dcm_dim.begin();

	int max_value = iter->second;
	mafString str = iter->first;
	for( ; iter != dcm_dim.end(); ++iter) {
		if( iter->second > max_value) {
			max_value = iter->second;
			str = iter->first;
		}
	}
	int dim_img_check[3];
	dim_img_check[2]=1;
	const char * dcm_dim_name = str.GetCStr();
	char bufx[256];
	char bufy[256];
	int ecounter=0;
	int icounter=0;
	while(dcm_dim_name[ecounter] != 'x')
	{
      bufy[icounter] = dcm_dim_name[ecounter];
	  ecounter++;
	  icounter++;
	}
	ecounter++;
	icounter = 0;
	// x dimension
	dim_img_check[1] = atoi(bufy);
	while(dcm_dim_name[ecounter] != 'x')
	{
		bufx[icounter] = dcm_dim_name[ecounter];
		ecounter++;
		icounter++;
	}
	// x dimension
	dim_img_check[0] = atoi(bufx);

	int n_slices = m_SelectedSeriesSlicesList->GetCount();
	bool *sliceToSkip = new bool[n_slices+1];
	for (int i=0;i<n_slices+1;i++)
	{
		sliceToSkip[i] = false;
	}

	int dim_img[3];
	for (int count = 0; count < n_slices; count++)
	{  
		vtkImageData *image = m_SelectedSeriesSlicesList->Item(count)->GetData()->GetVTKImageData();
		image->Update();
		image->GetDimensions(dim_img);
		for (int i=0;i<3;i++)
		{
	//		if (dim_img[i] != dim_img_check[i])
			// I don't want to skip the rotated images
			if ( ( (dim_img[0] != dim_img_check[0]) && (dim_img[0] != dim_img_check[1]) ) ||  
				( (dim_img[1] != dim_img_check[1]) && (dim_img[1] != dim_img_check[0]) ) )
			{
				sliceToSkip[count] = true;
				break;
			}
		}
	}


	// sort dicom slices
	if(m_SelectedSeriesSlicesList->GetCount() > 1)
	{
		double previousSlicePosition[3],nextSlicePosition[3],deltaXYZPositionFromPreviousToNextSlice[3];
		mafDicomSlice *previousSlice;
		mafDicomSlice *nextSlice;

		deltaXYZPositionFromPreviousToNextSlice[0] = 0.0;
		deltaXYZPositionFromPreviousToNextSlice[1] = 0.0;
		deltaXYZPositionFromPreviousToNextSlice[2] = 0.0;

		// This algorithm must be able to skip the wrong vtkimagedata from the series
		for (int i=0;i<m_SelectedSeriesSlicesList->GetCount()-1;++i)
		{
			int previousSliceID = 0;
			int nextSliceID = 0;
			if(sliceToSkip[i]) {
			    continue;
			} 
			else 
			{
                previousSliceID = i;
				int j;
                for(j=i+1;j<m_SelectedSeriesSlicesList->GetCount();++j) 
				{
					if(sliceToSkip[j] == false) {
						nextSliceID = j;
						break;
					}
				}
				if(j==m_SelectedSeriesSlicesList->GetCount()) {
					nextSliceID = previousSliceID;
				}
			}


			previousSlice = (mafDicomSlice *)m_SelectedSeriesSlicesList->Item(previousSliceID)->GetData();
			nextSlice = (mafDicomSlice *)m_SelectedSeriesSlicesList->Item(nextSliceID)->GetData();

			previousSlice->GetDcmImagePositionPatient(previousSlicePosition);
			nextSlice->GetDcmImagePositionPatient(nextSlicePosition);

			deltaXYZPositionFromPreviousToNextSlice[0] += fabs(previousSlicePosition[0] - nextSlicePosition[0]);
			deltaXYZPositionFromPreviousToNextSlice[1] += fabs(previousSlicePosition[1] - nextSlicePosition[1]);
			deltaXYZPositionFromPreviousToNextSlice[2] += fabs(previousSlicePosition[2] - nextSlicePosition[2]);
		}

		deltaXYZPositionFromPreviousToNextSlice[0] /= (m_SelectedSeriesSlicesList->GetCount()-1); 
		deltaXYZPositionFromPreviousToNextSlice[1] /= (m_SelectedSeriesSlicesList->GetCount()-1);
		deltaXYZPositionFromPreviousToNextSlice[2] /= (m_SelectedSeriesSlicesList->GetCount()-1);

		if(deltaXYZPositionFromPreviousToNextSlice[0] > deltaXYZPositionFromPreviousToNextSlice[1] && 
			deltaXYZPositionFromPreviousToNextSlice[0] > deltaXYZPositionFromPreviousToNextSlice[2])
			m_SortAxes = 0;
		else if(deltaXYZPositionFromPreviousToNextSlice[1] > deltaXYZPositionFromPreviousToNextSlice[0] &&
			deltaXYZPositionFromPreviousToNextSlice[1] > deltaXYZPositionFromPreviousToNextSlice[2])
			m_SortAxes = 1;
		else if(deltaXYZPositionFromPreviousToNextSlice[2] > deltaXYZPositionFromPreviousToNextSlice[0] &&
			deltaXYZPositionFromPreviousToNextSlice[2] > deltaXYZPositionFromPreviousToNextSlice[1])
			m_SortAxes = 2;

		// The sorting direction computed in the previous algorithm must be updated in the GUI 
		GuiUpdate();

	}

	delete[] sliceToSkip;

	// REFACTOR TODO:
	// this is needed in order for regression test data:
	// \Medical_Parabuild\Testing\unittestData\DicomUnpacker\TestDicomUnpacker\
	// to work
	// but it is braking CineMRI p09 p20 and SE10 CineMRI reading so i set it to
	// enable for modality other than cine MRI
	if (m_DicomReaderModality != mafGUIDicomSettings::ID_CMRI_MODALITY)
	{
		switch (m_SortAxes)
		{
		case 0:
			m_SelectedSeriesSlicesList->Sort(CompareX);
			break;
		case 1:
			m_SelectedSeriesSlicesList->Sort(CompareY);
			break;
		case 2:
			m_SelectedSeriesSlicesList->Sort(CompareZ);
			break;
		}
	}

	bool spaceisuniform = true;
	if(m_SelectedSeriesSlicesList->GetCount() > 1)
	{
		double previousSlicePosition[3],nextSlicePosition[3];
		double newdistance;
		double refdistance;
		mafDicomSlice *previousSlice;
		mafDicomSlice *nextSlice;

		for (int i=0;i<m_SelectedSeriesSlicesList->GetCount()-1;++i)
		{
			int previousSliceID = i;
			int nextSliceID = i+1;

		    previousSlice = (mafDicomSlice *)m_SelectedSeriesSlicesList->Item(previousSliceID)->GetData();
			nextSlice = (mafDicomSlice *)m_SelectedSeriesSlicesList->Item(nextSliceID)->GetData();

			previousSlice->GetDcmImagePositionPatient(previousSlicePosition);
			nextSlice->GetDcmImagePositionPatient(nextSlicePosition);
		    
			newdistance = fabs(nextSlicePosition[m_SortAxes] - previousSlicePosition[m_SortAxes]);
			if(i==0) refdistance = newdistance;
			if( fabs(newdistance - refdistance) > 1.e-03 ) spaceisuniform = false;
		}
	}

	if(spaceisuniform) {
		m_Showspacing = 1;
	}
	else 
	{
        m_Showspacing = 0;
	}

	if (!this->m_TestMode)
	{
	    GuiUpdate();
	}


	m_NumberOfTimeFrames = ((mafDicomSlice *)m_SelectedSeriesSlicesList->\
		Item(0)->GetData())->GetDcmCardiacNumberOfImages();

	if(m_DicomReaderModality == mafGUIDicomSettings::ID_CMRI_MODALITY) //If cMRI
		m_NumberOfSlices = m_SelectedSeriesSlicesList->GetCount() / m_NumberOfTimeFrames;
	else
		m_NumberOfSlices = m_SelectedSeriesSlicesList->GetCount();

	//Set bounds of ZCrop slider widget
	m_ZCropBounds[1] = m_NumberOfSlices-1;
	if (!this->m_TestMode && m_ZCrop)
	{
		m_CropPage->SetZCropBounds(0, m_ZCropBounds[1]);
	}


	// reset the current slice number to view the first slice
	m_CurrentSlice = 0;
	m_CurrentTime = 0;
	m_CropFlag = false;
	int currImageId = GetSliceIDInSeries(m_CurrentTime, m_CurrentSlice);

	if(!this->m_TestMode)
	{
		if (currImageId != -1) 
		{
			// show the selected slice
			GenerateSliceTexture(currImageId);
			ShowSlice();
			CameraReset();
			ResetSliders();
			CameraUpdate();
		}
	}

	//modified by STEFY 9-7-2003(begin)
	ImportDicomTags();
	mafTagItem *patient_name;
	mafTagItem *patient_id;

	const char* p_name;
	double p_id = 0;

	bool PatientsNameTagIsPresent = false;
	PatientsNameTagIsPresent = m_TagArray->IsTagPresent("PatientsName");
	if (PatientsNameTagIsPresent)
	{
		patient_name = m_TagArray->GetTag("PatientsName");
		p_name = patient_name->GetValue();
	}
	else 
		p_name = NULL;

	bool PatientIDTagIsPresent = false;
	PatientIDTagIsPresent = m_TagArray->IsTagPresent("PatientID");
	if (PatientIDTagIsPresent)
	{
		patient_id = m_TagArray->GetTag("PatientID");
		p_id = patient_id->GetValueAsDouble();
		m_Identifier = mafString(p_id);
	}
	if (p_name)
		m_PatientName = p_name;

	int tmp = m_PatientName.FindChr('^');
	if(tmp != -1 && tmp >= 0 && tmp < m_PatientName.GetSize())
		m_PatientName[tmp] = ' ';
}

//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::OnEvent(mafEventBase *maf_event) 
	//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
		case ID_SORT_AXIS:
			{
				if (m_DicomReaderModality != mafGUIDicomSettings::ID_CMRI_MODALITY)
				{
					switch (m_SortAxes)
					{
					case 0:
						m_SelectedSeriesSlicesList->Sort(CompareX);
						break;
					case 1:
						m_SelectedSeriesSlicesList->Sort(CompareY);
						break;
					case 2:
						m_SelectedSeriesSlicesList->Sort(CompareZ);
						break;
					}
				}

				// reset the current slice number to view the first slice
				m_CurrentSlice = 0;
				m_CurrentTime = 0;
				m_CropFlag = false;
				int currImageId = GetSliceIDInSeries(m_CurrentTime, m_CurrentSlice);

				if(!this->m_TestMode)
				{
					if (currImageId != -1) 
					{
						// show the selected slice
						GenerateSliceTexture(currImageId);
						ShowSlice();
						CameraReset();
						ResetSliders();
						CameraUpdate();
					}
				}
			}
			break;
		case ID_VOLUME_SIDE:
			{
				if(((mafGUIDicomSettings*)GetSetting())->AutoCropPosition())
					AutoPositionCropPlane();
			}
			break;
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
				/* This is a ack, beacouse that "genius" of wx  send the change event 
				before page show, so we need to duplicate the code here in order to 
				manage the camera update */
				m_Wizard->GetCurrentPage()->Show();
				m_Wizard->GetCurrentPage()->SetFocus();
				m_Wizard->GetCurrentPage()->Update();
				CameraReset();
			}
			break;
		case ID_UNDO_CROP:
			{
				OnUndoCrop();
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
		case ID_SCAN_SLICE:
			{
				OnScanSlice();
			}
			break;
		case ID_SCAN_TIME:
			{
				// show the current slice
				OnScanTime();
			}
			break;
		case ID_CROP:
			{     
				Crop();
			}
			break;
		case ID_VME_TYPE:
			{
				OnVmeTypeSelected();
				if (m_Wizard->GetCurrentPage()==m_BuildPage && m_OutputType == mafGUIDicomSettings::ID_IMAGE)//Check the type to determine the next step
				{
					m_Wizard->SetButtonString("Reference >");
					m_ReferenceSystemPage->UpdateActor();
					m_Wizard->Update();
				}
				else
				{
					m_Wizard->SetButtonString("Finish");
					m_Wizard->Update();
				}
			}
			break;
		case ID_RS_SELECT:
			{
				OnReferenceSystemSelected();
			}
			break;
		case ID_RS_SWAP:
			{
				OnSwapReferenceSystemSelected();
			}
			break;
		case ID_RS_APPLYTOALL:
			{
				if(m_ApplyToAllReferenceSystem)
				{
					m_SwapAllReferenceSystem = m_SwapReferenceSystem;
					m_GlobalReferenceSystem = m_SelectedReferenceSystem;
				}
			}
			// BEWARE NO BRAK!!! break;
		case ID_RS_SWAPALL:
			{
				if(m_SwapAllReferenceSystem || m_ApplyToAllReferenceSystem)
				{
					m_ReferenceSystemGuiUnderLeft->Enable(ID_RS_SWAP,false);
				}
				else
				{
					m_ReferenceSystemGuiUnderLeft->Enable(ID_RS_SWAP,true);
				}
				UpdateReferenceSystemVariables();
				m_ReferenceSystemGuiUnderLeft->Update();
				m_ReferenceSystemPage->Update();
				m_ReferenceSystemPage->UpdateActor();
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
void mafOpImporterDicomOffis::OnUndoCrop()
	//----------------------------------------------------------------------------
{
	m_CropFlag = false;
	int currImageId = GetSliceIDInSeries(m_CurrentTime, m_CurrentSlice);
	if (currImageId != -1) 
	{
		GenerateSliceTexture(currImageId);
		ShowSlice();
	}
	double diffx,diffy,boundsCamera[6];
	diffx=m_SliceBounds[1]-m_SliceBounds[0];
	diffy=m_SliceBounds[3]-m_SliceBounds[2];
	boundsCamera[0]=0.0;
	boundsCamera[1]=diffx;
	boundsCamera[2]=0.0;
	boundsCamera[3]=diffy;
	boundsCamera[4]=0.0;
	boundsCamera[5]=0.0;
	m_LoadPage->GetRWI()->CameraReset(boundsCamera);
	m_LoadPage->GetRWI()->CameraUpdate();
	m_CropPage->GetRWI()->CameraReset(boundsCamera);
	m_CropPage->GetRWI()->CameraUpdate();
	m_BuildPage->GetRWI()->CameraReset(boundsCamera);
	m_BuildPage->GetRWI()->CameraUpdate();
	m_ReferenceSystemPage->GetRWI()->CameraReset(boundsCamera);
	m_ReferenceSystemPage->GetRWI()->CameraUpdate();
	m_CropActor->VisibilityOn();
	m_CropExecuted=false;
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::Crop()
	//----------------------------------------------------------------------------
{
	if( !m_BoxCorrect )
	{
		wxMessageBox("Error on selecting the box");
		return;
	}

	m_CropFlag = true;
	int currImageId = GetSliceIDInSeries(m_CurrentTime, m_CurrentSlice);
	if (currImageId != -1) 
	{
		GenerateSliceTexture(currImageId);
		ShowSlice();
	}
	m_CropActor->VisibilityOff();
	m_CropExecuted=true;

	double diffx,diffy,boundsCamera[6];
	diffx=m_SliceBounds[1]-m_SliceBounds[0];
	diffy=m_SliceBounds[3]-m_SliceBounds[2];

	boundsCamera[0]=0.0;
	boundsCamera[1]=diffx;
	boundsCamera[2]=0.0;
	boundsCamera[3]=diffy;
	boundsCamera[4]=0.0;
	boundsCamera[5]=0.0;

	m_CropPage->GetRWI()->CameraReset(boundsCamera);
	m_CropPage->GetRWI()->CameraUpdate();
	m_LoadPage->GetRWI()->CameraReset(boundsCamera);
	m_LoadPage->GetRWI()->CameraUpdate();
	m_BuildPage->GetRWI()->CameraReset(boundsCamera);
	m_BuildPage->GetRWI()->CameraUpdate();
	m_ReferenceSystemPage->GetRWI()->CameraReset(boundsCamera);
	m_ReferenceSystemPage->GetRWI()->CameraUpdate();

	//Modify name
	double spacing[3];
	int cropInterval = (m_ZCropBounds[1]+1 - m_ZCropBounds[0]);
	m_SelectedSeriesSlicesList->Item(currImageId)->GetData()->GetVTKImageData()->GetSpacing(spacing);

	double pixelDimX = diffx/spacing[0] + 1;
	double pixelDimY = diffy/spacing[0] + 1;


	if (((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::TRADITIONAL)
	{
		//---------------------------------------------------------------
		long item = -1;
		long myitem = 0;
		for ( ;; )
		{
			myitem = item;
			item = m_SeriesListctrl->GetNextItem(item,wxLIST_NEXT_ALL,wxLIST_STATE_SELECTED);
			if ( item == -1 )
				break;
		}
		//string selected from the listctrl
		wxString seriesName = m_SeriesListctrl->GetItemText(myitem);
		//---------------------------------------------------------------

		//	  wxString  seriesName = m_SeriesListbox->GetString(m_SeriesListbox->GetSelection());
		m_VolumeName = seriesName.Mid(0,seriesName.find_last_of('_'));
		m_VolumeName.Append(wxString::Format("_%ix%ix%i", (int)pixelDimX, (int)pixelDimY, cropInterval));
	}
	//else if (((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::DESCRIPTION_DATE)
	else if (((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::DESCRIPTION_NUMSLICES)
	{
		m_VolumeName = m_SelectedSeriesSlicesList->Item(0)->GetData()->GetDescription();
		/*m_VolumeName << "_";
		m_VolumeName << m_SelectedSeriesSlicesList->Item(0)->GetData()->GetDate();*/
		m_VolumeName.Append(wxString::Format("_%ix%ix%i", (int)pixelDimX, (int)pixelDimY, cropInterval));
	}
	else if (((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::CUSTOM)
	{
		//bool separator = false;
		m_VolumeName = "";
		//if (((mafGUIDicomSettings*)GetSetting())->GetEnabledCustomName(mafGUIDicomSettings::ID_SERIES))
		//{
			//---------------------------------------------------------------
			long item = -1;
			long myitem = 0;
			for ( ;; )
			{
				myitem = item;
				item = m_SeriesListctrl->GetNextItem(item,wxLIST_NEXT_ALL,wxLIST_STATE_SELECTED);
				if ( item == -1 )
					break;
			}
			//---------------------------------------------------------------
			//string selected from the listctrl
			wxString seriesName = m_SeriesListctrl->GetItemText(myitem);
			//---------------------------------------------------------------

			//      wxString  seriesName = m_SeriesListbox->GetString(m_SeriesListbox->GetSelection());
			m_VolumeName = seriesName;
		//	separator = true;
		//}
		//if (((mafGUIDicomSettings*)GetSetting())->GetEnabledCustomName(mafGUIDicomSettings::ID_PATIENT_NAME))
		//{
		//	if (separator)
		//	{
		//		m_VolumeName << "_";
		//	}
		//	m_VolumeName << m_SelectedSeriesSlicesList->Item(0)->GetData()->GetPatientName();
		//	separator = true;
		//}
		//if (((mafGUIDicomSettings*)GetSetting())->GetEnabledCustomName(mafGUIDicomSettings::ID_DESCRIPTION))
		//{
		//	if (separator)
		//	{
		//		m_VolumeName << "_";
		//	}
		//	m_VolumeName << m_SelectedSeriesSlicesList->Item(0)->GetData()->GetDescription();
		//	separator = true;
		//}
		//if (((mafGUIDicomSettings*)GetSetting())->GetEnabledCustomName(mafGUIDicomSettings::ID_BIRTHDATE))
		//{
		//	if (separator)
		//	{
		//		m_VolumeName << "_";
		//	}
		//	m_VolumeName << m_SelectedSeriesSlicesList->Item(0)->GetData()->GetPatientBirthday();
		//	separator = true;
		//}
		if (((mafGUIDicomSettings*)GetSetting())->GetEnabledCustomName(mafGUIDicomSettings::ID_NUM_SLICES))
		{
//			if (separator)
//			{
//				m_VolumeName << "_";
//			}
			/*wxString  seriesName = m_SeriesListbox->GetString(m_SeriesListbox->GetSelection());*/
			m_VolumeName.Append(wxString::Format("_%ix%ix%i", (int)pixelDimX, (int)pixelDimY, cropInterval));
		//	separator = true;
		}
	}
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::AutoPositionCropPlane()
	//----------------------------------------------------------------------------
{
	int currImageId = GetSliceIDInSeries(m_CurrentTime, m_CurrentSlice);

	m_SelectedSeriesSlicesList->Item(currImageId)->GetData()->GetVTKImageData()->Update();
	m_SelectedSeriesSlicesList->Item(currImageId)->GetData()->GetVTKImageData()->GetBounds(m_SliceBounds);

	double diffY,diffX;
	diffY=m_SliceBounds[3]-m_SliceBounds[2];
	diffX=m_SliceBounds[1]-m_SliceBounds[0];

	if(m_VolumeSide==RIGHT_SIDE)
	{
		m_CropPlane->SetOrigin(0.0,diffY/4,0.0);
		m_CropPlane->SetPoint1(diffX/2,diffY/4,0.0);
		m_CropPlane->SetPoint2(0.0,(diffY/4)*3,0.0);
		m_CropPlane->Update();
	}
	else if(m_VolumeSide==LEFT_SIDE)
	{
		m_CropPlane->SetOrigin(diffX/2,diffY/4,0.0);
		m_CropPlane->SetPoint1(diffX,diffY/4,0.0);
		m_CropPlane->SetPoint2(diffX/2,(diffY/4)*3,0.0);
		m_CropPlane->Update();
	}
	else if (m_VolumeSide==NON_VALID_SIDE)
	{
		m_CropPlane->SetOrigin(0.0,0.0,0.0);
		m_CropPlane->SetPoint1(m_SliceBounds[1]-m_SliceBounds[0],0.0,0.0);
		m_CropPlane->SetPoint2(0.0,m_SliceBounds[3]-m_SliceBounds[2],0.0);
		m_CropPage->GetRWI()->CameraReset();
	}

	m_CropPage->GetRWI()->CameraReset();
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::CameraUpdate()
	//----------------------------------------------------------------------------
{
	if(m_Wizard->GetCurrentPage() == m_LoadPage)
	{
		m_LoadPage->UpdateActor();
	}
	else if(m_Wizard->GetCurrentPage() == m_CropPage)
	{
		m_CropPage->UpdateActor();
	}
	else if(m_Wizard->GetCurrentPage() == m_BuildPage)
	{
		m_BuildPage->UpdateActor();
	}
	else if(m_Wizard->GetCurrentPage() == m_ReferenceSystemPage)
	{
		m_ReferenceSystemPage->UpdateActor();
	}
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::CameraReset()
	//----------------------------------------------------------------------------
{
	m_LoadPage->UpdateWindowing(m_TotalDicomRange,m_TotalDicomSubRange);
	m_LoadPage->GetRWI()->CameraReset();
	m_CropPage->UpdateWindowing(m_TotalDicomRange,m_TotalDicomSubRange);
	m_CropPage->GetRWI()->CameraReset();
	m_BuildPage->UpdateWindowing(m_TotalDicomRange,m_TotalDicomSubRange);
	m_BuildPage->GetRWI()->CameraReset();
	m_ReferenceSystemPage->GetRWI()->CameraReset();
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::EnableSliceSlider(bool enable)
	//----------------------------------------------------------------------------
{
	m_LoadGuiLeft->Enable(ID_SCAN_SLICE,enable);
	m_BuildGuiLeft->Enable(ID_SCAN_SLICE,enable);
	m_CropGuiLeft->Enable(ID_SCAN_SLICE,enable);
	m_ReferenceSystemGuiLeft->Enable(ID_SCAN_SLICE,enable);
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::EnableTimeSlider(bool enable)
	//----------------------------------------------------------------------------
{
	if(((mafGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
	{
		m_LoadGuiLeft->Enable(ID_SCAN_TIME,enable);
		m_BuildGuiLeft->Enable(ID_SCAN_TIME,enable);
		m_ReferenceSystemGuiLeft->Enable(ID_SCAN_TIME,enable);
		m_CropGuiLeft->Enable(ID_SCAN_TIME,enable);
	}
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::CreateSliceVTKPipeline()
	//----------------------------------------------------------------------------
{
	vtkNEW(m_DICOMDirectoryReader);

	vtkNEW(m_SliceLookupTable);

	vtkNEW(m_SliceTexture);
	m_SliceTexture->InterpolateOn();

	vtkNEW(m_SlicePlane);

	vtkNEW(m_SliceMapper);
	m_SliceMapper->SetInput(m_SlicePlane->GetOutput());

	vtkNEW(m_SliceActor);
	m_SliceActor->SetMapper(m_SliceMapper);
	m_SliceActor->SetTexture(m_SliceTexture); 
	m_SliceActor->VisibilityOff();

	// text stuff
	m_Text = "Orientation: ";
	m_TextMapper = vtkTextMapper::New();
	m_TextMapper->SetInput(m_Text.c_str()); 
	m_TextMapper->GetTextProperty()->AntiAliasingOn();

	m_TextActor = vtkActor2D::New();
	m_TextActor->GetProperty()->SetColor(0.8,0,0);
	m_TextActor->SetMapper(m_TextMapper);
	m_TextActor->SetPosition(3,3);  
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
void mafOpImporterDicomOffis::FillStudyListBox(mafString studyUID)
	//----------------------------------------------------------------------------
{
	bool newStudy = true;
	int studyCounter = m_StudyListbox->GetCount();
	//	int studyCounter = m_StudyListctrl->GetItemCount();
	mafString studyName = "study_";
	studyName.Append(wxString::Format("%i", studyCounter));
	for (int n = 0; n < studyCounter; n++)
	{
		mafString *st = (mafString *)m_StudyListbox->GetClientData(n);
		//	mafString *st = (mafString *)m_StudyListctrl->GetItemData(n);

		m_SelectedSeriesID.at(0) = st->GetCStr();
		if (m_SelectedSeriesID.at(0).Compare(studyUID) == 0)
		{
			newStudy = false;
			break;
		}
	}
	if (newStudy)
	{ 
		m_StudyListbox->Append(studyName.GetCStr());
		//	m_StudyListctrl->InsertItem(0,studyName.GetCStr());

		mafString *ms = new mafString(studyUID.GetCStr());

		m_StudyListbox->SetClientData(studyCounter, (void *) ms);

		//	    m_StudyListctrl->SetItemData(studyCounter,idstudy);
	}
}

//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::UpdateStudyListBox()
	//----------------------------------------------------------------------------
{
	for (int n = 0; n < m_StudyListbox->GetCount(); n++)
		//	for (int n = 0; n < m_StudyListctrl->GetItemCount(); n++)
	{
		int counter = 0;

		mafString study = m_StudyListbox->GetString(n);
		//		mafString study = m_StudyListctrl->GetItemText(n);

		mafString *st = (mafString *)m_StudyListbox->GetClientData(n);
		//		mafString *st = (mafString *)m_StudyListctrl->GetItemData(n);

		m_SelectedSeriesID.at(0) = st->GetCStr();

		std::map<std::vector<mafString>,medDicomSeriesSliceList*>::iterator it;
		for ( it=m_SeriesIDToSlicesListMap.begin() ; it != m_SeriesIDToSlicesListMap.end(); it++ )
		{
			if ((*it).first.at(0).Compare(m_SelectedSeriesID.at(0)) == 0)
			{ 
				m_SelectedSeriesID.at(1) = (*it).first.at(1);
				m_SelectedSeriesID.at(2) = (*it).first.at(2);
				if (m_SeriesIDToSlicesListMap.find(m_SelectedSeriesID) != m_SeriesIDToSlicesListMap.end())
				{
					counter++;
				}
			}
		}

		study.Append(wxString::Format("_%i", counter));
		m_StudyListbox->SetString(n, study.GetCStr());
		//		m_StudyListctrl->SetItemText((long)n,study.GetCStr());

	}
}

//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::FillSeriesListBox()
	//----------------------------------------------------------------------------
{
	int counter = 0;
	//	m_SeriesListbox->Clear();
	m_SeriesListctrl->ClearAll();

	std::map<std::vector<mafString>,medDicomSeriesSliceList*>::iterator it;
	for ( it=m_SeriesIDToSlicesListMap.begin() ; it != m_SeriesIDToSlicesListMap.end(); it++ )
	{
		if ((*it).first.at(0).Compare(m_SelectedSeriesID.at(0)) == 0)
		{ 
			m_SelectedSeriesID.at(1) = (*it).first.at(1);
			m_SelectedSeriesID.at(2) = (*it).first.at(2);
			if (m_SeriesIDToSlicesListMap.find(m_SelectedSeriesID) != m_SeriesIDToSlicesListMap.end())
			{
				m_SelectedSeriesSlicesList = m_SeriesIDToSlicesListMap[m_SelectedSeriesID];
				int idser = m_SeriesIDstringToSeriesIDint[m_SelectedSeriesID];
				int numberOfImages = 0;

				int numberOfTimeFrames = ((mafDicomSlice *)m_SelectedSeriesSlicesList->Item(0)->GetData())->GetDcmCardiacNumberOfImages();
				if(numberOfTimeFrames > 1) //If cMRI
					numberOfImages = m_SelectedSeriesSlicesList->GetCount() / numberOfTimeFrames;
				else
					numberOfImages = m_SelectedSeriesSlicesList->GetCount();


				mafString seriesName = "";

				if(((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::TRADITIONAL)
				{
					seriesName = m_SelectedSeriesID.at(2);
					seriesName.Append(wxString::Format("x%i", numberOfImages));
				}
				//else if (((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::DESCRIPTION_DATE)
				else if (((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::DESCRIPTION_NUMSLICES)
				{
					seriesName = m_SelectedSeriesID.at(2);
					seriesName.Append(wxString::Format("x%i", numberOfImages));
				}
				else if (((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::CUSTOM)
				{
					bool separator = false;
					seriesName << m_SelectedSeriesID.at(2);
					seriesName << "x_";
					//if (((mafGUIDicomSettings*)GetSetting())->GetEnabledCustomName(mafGUIDicomSettings::ID_SERIES))
					//{
					//    seriesName << m_SelectedSeriesID.at(2);
					//    seriesName.Append(wxString::Format("x%i", numberOfImages));
					//	separator = true;
					//}
					if (((mafGUIDicomSettings*)GetSetting())->GetEnabledCustomName(mafGUIDicomSettings::ID_DESCRIPTION))
					{
						if (separator)
						{
							seriesName << "_";
						}
						seriesName << m_SelectedSeriesSlicesList->Item(0)->GetData()->GetDescription();
						separator = true;
					}
					if (((mafGUIDicomSettings*)GetSetting())->GetEnabledCustomName(mafGUIDicomSettings::ID_PATIENT_NAME))
					{
						if (separator)
						{
							seriesName << "_";
						}
						seriesName << m_SelectedSeriesSlicesList->Item(0)->GetData()->GetPatientName();
						separator = true;
					}
					if (((mafGUIDicomSettings*)GetSetting())->GetEnabledCustomName(mafGUIDicomSettings::ID_BIRTHDATE))
					{
						if (separator)
						{
							seriesName << "_";
						}
						seriesName << m_SelectedSeriesSlicesList->Item(0)->GetData()->GetPatientBirthday();
						separator = true;
					}
					//if (((mafGUIDicomSettings*)GetSetting())->GetEnabledCustomName(mafGUIDicomSettings::ID_NUM_SLICES))
					//{
					//  //  if (separator)
					////	{
					//		seriesName << "_";
					////	}
					////	///*wxString  seriesName = m_SeriesListbox->GetString(m_SeriesListbox->GetSelection());*/
					//	seriesName.Append(wxString::Format("_%ix%ix%i", (int)pixelDimX, (int)pixelDimY, numberOfImages));
					//	//separator = true;
					//}
				}

				//      m_SeriesListbox->Append(seriesName.GetCStr());
				//      m_SeriesListbox->SetClientData(counter,(void *)m_SeriesIDToSlicesListMap[m_SelectedSeriesID]/*filesList*/); 
				//      wxColour itemREDcolour = wxColour(255,1,1);

				//      TODO
				m_SeriesListctrl->InsertItem((long) counter,seriesName.GetCStr());
				//		m_SeriesListctrl->SetClientData((void *)m_SeriesIDToSlicesListMap[m_SelectedSeriesID]/*filesList*/);
				long ptclientdata = (long) m_SeriesIDToSlicesListMap[m_SelectedSeriesID];

				m_SeriesListctrl->SetItemData((long) counter,(long) idser);
				wxColour itemREDcolour = wxColour(255,1,1);
				if(numberOfImages < 6) 
				{
					m_SeriesListctrl->SetItemTextColour((long) counter,itemREDcolour);
				}

					counter++;
			}
		} 
	}

	m_SeriesListctrl->SortItems(_myCompareFunction, 0);
}

//----------------------------------------------------------------------------
bool mafOpImporterDicomOffis::BuildDicomFileList(const char *dicomDirABSPath)
	//----------------------------------------------------------------------------
{   
	bool errorOccurred = false;
	mafString currentSliceABSDirName = dicomDirABSPath;

	if (m_DICOMDirectoryReader->Open(currentSliceABSDirName.GetCStr()) == 0)
	{
		if(!this->m_TestMode)
		{
			wxMessageBox(wxString::Format("Directory <%s> can not be opened",currentSliceABSDirName.GetCStr()),"Warning!!");
		}
		return false;
	}
	// ------------------------------------------------------------

	mafProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Reading DICOM directory: please wait...");



	// foreach dicom directory file
	int img_pos_result = wxNO; // added by Losi to avoid exiting series without image position

	if(!ReadDicomFileList(currentSliceABSDirName,&progressHelper)) {
		return false;
	}

	progressHelper.CloseProgressBar();

	// start handling files
	if(m_NumberOfStudies == 0)
	{
		if (!this->m_TestMode)
		{
			wxString msg = "No study found!";
			wxMessageBox(msg,"Confirm", wxOK , NULL);
		}
		return false;
	}
	else
	{
		if (errorOccurred)
		{
			if(!this->m_TestMode)
			{
				wxMessageBox("Some errors occurred while importing data. Please check the log area for details.");
			}
		}

		std::map<std::vector<mafString>,medDicomSeriesSliceList*>::iterator it;

		int numberOfSeries = m_SeriesIDToSlicesListMap.size();

		// DEBUG
		std::ostringstream stringStream;
		stringStream << "Found " << numberOfSeries << " dicom series" << std::endl;
		mafLogMessage(stringStream.str().c_str());

		//---------------------------------------
		// Cardiac MRI Handling
		//---------------------------------------

		std::vector< std::vector<mafString> > seriesToDelete;
		// foreach series
		for ( it=m_SeriesIDToSlicesListMap.begin() ; it != m_SeriesIDToSlicesListMap.end(); it++ )
		{
			medDicomSeriesSliceList *currentSeries = NULL;
			currentSeries = m_SeriesIDToSlicesListMap[(*it).first];
			assert(currentSeries);

			// check if the first slice is cineMRI
			mafDicomSlice* firstSlice = NULL;
			firstSlice = currentSeries->Item(0)->GetData();
			assert(firstSlice);

			bool isCardiacMRI = (firstSlice->GetDcmCardiacNumberOfImages() > 1);

			if (isCardiacMRI)
			{
				vector<string> absFileNames;
				for (int i = 0; i < currentSeries->size(); i++) 
				{
					mafDicomSlice* slice = NULL;
					slice = currentSeries->Item(i)->GetData();
					assert(slice);
					absFileNames.push_back(slice->GetSliceABSFileName());
				}

				// create a cardiac mri helper for the series and initialize it
				mafDicomCardiacMRIHelper *currentHelper = new mafDicomCardiacMRIHelper();
				currentHelper->SetListener(this);
				currentHelper->SetModeToInputDicomSlicesABSFileNamesVector();
				currentHelper->SetInputDicomSlicesABSFileNamesVector(absFileNames);

				std::vector<mafString> seriesId = (*it).first;

				if (currentHelper->ParseDicomDirectory() != MAF_OK)
				{
					seriesToDelete.push_back(seriesId);
					continue;
				}

				m_SeriesIDToCardiacMRIHelperMap[seriesId] = currentHelper;

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
					int numSlicesInSeries = currentSeries->size();

					// get the first slice in series
					mafDicomSlice* firstSlice = NULL;
					firstSlice = currentSeries->Item(0)->GetData();
					assert(firstSlice);

					int cardiacTimeFrames = -1;
					cardiacTimeFrames = firstSlice->GetDcmCardiacNumberOfImages();
					assert(cardiacTimeFrames > 1);

					int planesPerFrame = -1;
					planesPerFrame = numSlicesInSeries / cardiacTimeFrames;

					progressHelper.InitProgressBar("Applying Cardiac MRI correction, please wait...");

					for (int timeID = 0; timeID < cardiacTimeFrames; timeID++)
					{
						for (int planeID = 0; planeID < planesPerFrame ; planeID++)
						{						
							progressHelper.UpdateProgressBar((timeID) * 100 / ((double)(cardiacTimeFrames )));

							int itemID = fileNumberForPlaneIFrameJIdPlaneMatrix(planeID, timeID); 

							mafDicomSlice* currentSlice = NULL;
							currentSlice = currentSeries->Item(itemID)->GetData();
							assert(currentSlice);

							// correction in place
							vtkImageData *imageData = NULL;
							imageData = currentSlice->GetVTKImageData();
							assert(imageData);		

							double center[3] = {-9999,-9999,-9999};
							imageData->GetCenter(center);

							double bounds[6] = {-9999,-9999,-9999,-9999,-9999,-9999};
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

							bool flipUpDownCurrent = (flipUpDownFlag(planeID,0) == 1 ? true : false);
							bool flipLeftRightCurrent  = (flipLeftRightFlag(planeID,0) == 1 ? true : false);

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
							double p3[3] = {p(0),p(1),p(2)};
							currentSlice->SetDcmImagePositionPatient(p3);

							vnl_vector<double> xv = newXVersorsSingleFrameIdPlaneMatrix.get_row(planeID);
							assert(xv.size() == 3);

							vnl_vector<double> yv = newYVersorsSingleFrameIdPlaneMatrix.get_row(planeID);
							assert(yv.size() == 3);

							double orientation[6] = {xv(0),xv(1),xv(2),yv(0),yv(1),yv(2)};
							currentSlice->SetDcmImageOrientationPatient(orientation);

							currentSlice->GetVTKImageData()->SetOrigin(0,0,bounds[4]);

						}			
					}
				}
			}
		}

		for (int i=0;i<seriesToDelete.size();++i)
		{
			m_SeriesIDToCardiacMRIHelperMap.erase(seriesToDelete[i]);
			m_SeriesIDToSlicesListMap.erase(seriesToDelete[i]);

			wxMessageBox("ERROR during reading series");
			mafLogMessage("ERROR during reading series : %s %s",seriesToDelete[i].at(0).GetCStr(),seriesToDelete[i].at(1).GetCStr());

			if (m_SeriesIDToSlicesListMap.size()==0)
			{
				return false;
			}

		}

		return true;
	}
}


//----------------------------------------------------------------------------
bool mafOpImporterDicomOffis::ReadDicomFileList(mafString& currentSliceABSDirName, mafProgressBarHelper *progressHelper) 
{

	int seriesIndex = 0;
	int sliceNum = -1;
	double lastZPos = 0;
	long int dcmInstanceNumber = -1;
	long int dcmCardiacNumberOfImages = -1;
	double dcmTriggerTime = -1.0;
	double dcmImageOrientationPatient[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
	double dcmImagePositionPatient[3] = {0.0,0.0,0.0};
	double dcmImagePositionPatient_old[3] = {0.0,0.0,0.0};
	bool enableToRead = true; //true for test mode
	bool errorOccurred = false;
	double lastDistance = 0;
	mafString lastFileName = "";

	double percentageTolerance = 0;
	double scalarTolerance = 0;
	bool enableScalarTolerance = false;
	bool enablePercentageTolerance = false;

	if(!this->m_TestMode)
	{
		enableScalarTolerance = ((mafGUIDicomSettings*)GetSetting())->EnableScalarTolerance() != FALSE;
		enablePercentageTolerance = ((mafGUIDicomSettings*)GetSetting())->EnablePercentageTolerance() != FALSE;

		if (enableScalarTolerance)
			scalarTolerance = ((mafGUIDicomSettings*)GetSetting())->GetScalarTolerance();

		if (enablePercentageTolerance)
			percentageTolerance = ((mafGUIDicomSettings*)GetSetting())->GetPercentageTolerance();
	}

	m_DicomReaderModality = -1;
	DcmFileFormat dicomImg;   

	time_t start,end;

	for (int i=0; i < m_DICOMDirectoryReader->GetNumberOfFiles(); i++) {

		//It works only on windows platform
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
		DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;

		if(!this->m_TestMode)
		{
			if(physMemUsed > 0.6*memInfo.ullTotalPhys && !(m_mem_is_almost_full==true) ) {
				int answer = wxMessageBox( "The allocated memory is the 60% of the physical memory! Do you want to continue?", "Warning", wxYES_NO, NULL);
				if (answer == wxNO)
				{
					return true;
				}
				else if (answer == wxYES)
				{
					m_mem_is_almost_full=true;
				}
			}
		}


		time(&start);

		//int nfiles = m_DICOMDirectoryReader->GetNumberOfFiles();

		if ((strcmp(m_DICOMDirectoryReader->GetFile(i),".") == 0) ||\
			(strcmp(m_DICOMDirectoryReader->GetFile(i),"..") == 0)) 
		{
			// skip non dicom files
			continue;
		}
		else
		{
			sliceNum++;
			mafString currentSliceABSFileName = "";
			mafString currentSliceLocalFileName = m_DICOMDirectoryReader->GetFile(i);
			mafString currentSliceABSSubDirName = "";

			//currentSliceABSFileName.Append(dicomDirABSPath);
			currentSliceABSFileName = currentSliceABSDirName.GetCStr();
			currentSliceABSFileName.Append("\\");
			currentSliceABSFileName.Append(currentSliceLocalFileName);

			bool wxdir_exist = wxDirExists(currentSliceABSFileName);

			// Check if currentSliceABSFileName is a directory or a file
			if(wxdir_exist) {

				currentSliceABSSubDirName.Append(currentSliceABSFileName);

				//Open the Subfolder
				if (m_DICOMDirectoryReader->Open(currentSliceABSSubDirName.GetCStr()) == 0)
				{
					if(!this->m_TestMode)
					{
						wxMessageBox(wxString::Format("Directory <%s> can not be opened",currentSliceABSSubDirName.GetCStr()),"Warning!!");
					}
					return false;
				}

				ReadDicomFileList(currentSliceABSSubDirName,progressHelper); //recursive function

				//Re-Open the folder
				if (m_DICOMDirectoryReader->Open(currentSliceABSDirName.GetCStr()) == 0)
				{
					if(!this->m_TestMode)
					{
						wxMessageBox(wxString::Format("Directory <%s> can not be opened",currentSliceABSDirName.GetCStr()),"Warning!!");
					}
					return false;
				}

				continue;
			}


			DJDecoderRegistration::registerCodecs(); // register JPEG codecs
			DcmRLEDecoderRegistration ::registerCodecs(OFFalse, OFFalse,OFFalse); // register RLE codecs
			OFCondition status = dicomImg.loadFile(currentSliceABSFileName);//load data into offis structure

			if (!status.good())
			{
				if(!this->m_TestMode)
				{
					wxLogMessage(wxString::Format("File <%s> can not be opened",currentSliceABSFileName));
					errorOccurred = true;
					sliceNum--;
				}
				continue;
			}

			m_CurrentSliceABSFileName = currentSliceABSFileName; 

			DcmDataset *dicomDataset = dicomImg.getDataset();//obtain dataset information from dicom file (loaded into memory)

			// decompress data set if compressed
			OFCondition error = dicomDataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);

			DJDecoderRegistration::cleanup(); // deregister JPEG codecs
			DcmRLEDecoderRegistration::cleanup();

			if (!error.good())
			{
				wxLogMessage(wxString::Format("Error decoding the image <%s>",currentSliceABSFileName));
				errorOccurred = true;
				//return false;
				continue;
			}

			const char *option = "?";
			dicomDataset->findAndGetString(DCM_ScanOptions,option);
			wxString scanOption = option;

			if (scanOption.Find("SCOUT") != -1)//check if it is a scout image
			{
				continue;
			}

			//now using findAndGet* methods to get dicom information
			long int val_long;
			dicomDataset->findAndGetLongInt(DCM_Columns, val_long); 

			// width
			int dcmColumns = val_long;
			dicomDataset->findAndGetLongInt(DCM_Rows, val_long);

			// height
			int dcmRows = val_long;

			dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,dcmImagePositionPatient[0],0);
			dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,dcmImagePositionPatient[1],1);
			dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,dcmImagePositionPatient[2],2);

			//Position Check
			int useDefaultPos=false;

			if( dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,dcmImagePositionPatient[2]).bad() && !m_SkipAllNoPosition)
			{
				int img_pos_result = ((mafGUIDicomSettings*)GetSetting())->GetDCMImagePositionPatientExceptionHandling();

				if (img_pos_result == 0) //skip all
				{
					errorOccurred = true;
					sliceNum--;
					std::ostringstream stringStream;
					stringStream << "Cannot read dicom tag DCM_ImagePositionPatient. Skip all."<< std::endl; 
				    mafLogMessage(stringStream.str().c_str());
					m_SkipAllNoPosition=true;
					continue;
				}
				else  //default position
				{
					useDefaultPos=true;
					std::ostringstream stringStream;
					stringStream << "Cannot read dicom tag DCM_ImagePositionPatient. Use default position."<< std::endl; 
					mafLogMessage(stringStream.str().c_str());
				}
			}
			else if (dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,dcmImagePositionPatient[2]).bad()) 
			{
				//Skip all Selected
				sliceNum--;
				continue;
			}
			dicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient,dcmImageOrientationPatient[0],0);
			dicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient,dcmImageOrientationPatient[1],1);
			dicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient,dcmImageOrientationPatient[2],2);
			dicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient,dcmImageOrientationPatient[3],3);
			dicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient,dcmImageOrientationPatient[4],4);
			dicomDataset->findAndGetFloat64(DCM_ImageOrientationPatient,dcmImageOrientationPatient[5],5);

			bool currentSliceIsRotated = false;
			currentSliceIsRotated = IsRotated(dcmImageOrientationPatient);

			double dcmPixelSpacing[3];
			dcmPixelSpacing[2] = 1;


			GetDicomSpacing(dicomDataset, dcmPixelSpacing);


			double dcmRescaleSlope;
			if(dicomDataset->findAndGetFloat64(DCM_RescaleSlope,dcmRescaleSlope).bad())
			{
				//Unable to get element: DCM_RescaleSlope[0];
				dcmRescaleSlope = 1;
			} 

			long dcmHighBit; 
			double dcmRescaleIntercept;
			if(dicomDataset->findAndGetLongInt(DCM_HighBit,dcmHighBit).bad())
			{
				//Unable to get element: DCM_RescaleIntercept[0];
				dcmHighBit = 0;
			} 
			m_HighBit = dcmHighBit;

			if(dicomDataset->findAndGetFloat64(DCM_RescaleIntercept,dcmRescaleIntercept).bad())
			{
				//Unable to get element: DCM_RescaleIntercept[0];
				dcmRescaleIntercept = 0;
			}
			m_RescaleIntercept = dcmRescaleIntercept;

			vtkMAFSmartPointer<vtkImageData> dicomSliceVTKImageData;
			dicomSliceVTKImageData->SetDimensions(dcmRows, dcmColumns,1);
			dicomSliceVTKImageData->SetWholeExtent(0,dcmColumns-1,0,dcmRows-1,0,0);
			dicomSliceVTKImageData->SetUpdateExtent(0,dcmColumns-1,0,dcmRows-1,0,0);
			dicomSliceVTKImageData->SetExtent(dicomSliceVTKImageData->GetUpdateExtent());
			dicomSliceVTKImageData->SetNumberOfScalarComponents(1);
			dicomSliceVTKImageData->SetSpacing(dcmPixelSpacing);

			long dcmPixelRepresentation;
			dicomDataset->findAndGetLongInt(DCM_PixelRepresentation,dcmPixelRepresentation);
			dicomDataset->findAndGetLongInt(DCM_BitsAllocated,val_long);

			long dcmLargestImagePixelValue;
			long dcmSmallestImagePixelValue;
			dicomDataset->findAndGetLongInt(DCM_SmallestImagePixelValue, dcmSmallestImagePixelValue);
			dicomDataset->findAndGetLongInt(DCM_LargestImagePixelValue, dcmLargestImagePixelValue);

			// These two lines were used for DP vertical app:
			// removed to fix bug http://bugzilla.hpc.cineca.it/show_bug.cgi?id=2079
			//if (dcmSmallestImagePixelValue == dcmLargestImagePixelValue)
			//  dcmRescaleIntercept = 0;

      const char *dcmModality = "?";
			dicomDataset->findAndGetString(DCM_Modality,dcmModality);

      long rappresentationMax;
      long rappresentationMin;
			if(val_long==16 && dcmPixelRepresentation == 0 )
			{
				dicomSliceVTKImageData->SetScalarType(VTK_UNSIGNED_SHORT);
			}
			else if (val_long == 16 && dcmPixelRepresentation == 1)
			{
				dicomSliceVTKImageData->SetScalarType(VTK_SHORT);

        rappresentationMax = VTK_SHORT_MAX;
        rappresentationMin = VTK_SHORT_MIN;

			}
			else if(val_long==8 && dcmPixelRepresentation == 0)
			{
				dicomSliceVTKImageData->SetScalarType(VTK_UNSIGNED_CHAR);
			}
			else if(val_long==8 && dcmPixelRepresentation == 1)
			{
				dicomSliceVTKImageData->SetScalarType(VTK_CHAR);
			}

			dicomSliceVTKImageData->AllocateScalars();
			dicomSliceVTKImageData->GetPointData()->GetScalars()->SetName("Scalars");
			dicomSliceVTKImageData->Update();

			const Uint16 *dicom_buf_short = NULL; 
			const Uint8* dicom_buf_char = NULL;
			int min = VTK_INT_MAX;
			int max = VTK_INT_MIN;

      long paddingValue = 0;
      if (dicomDataset->findAndGetLongInt(DCM_PixelPaddingValue,paddingValue).bad())
      {
        paddingValue = 0;
      }
      


			if (val_long==16) 
			{ 
				dicomDataset->findAndGetUint16Array(DCM_PixelData, dicom_buf_short); 
				int counter=0; 
				for(int y=0;y<dcmRows;y++) 
				{ 
					for(int x=0;x<dcmColumns;x++) 
					{ 
            
						dicomSliceVTKImageData->GetPointData()->GetScalars()->SetTuple1(counter, dicom_buf_short[dcmColumns*y+x]);
						counter++;

            if (paddingValue != 0 && paddingValue ==  dicom_buf_short[dcmColumns*y+x])
            {
              continue;
            }

						if (dicom_buf_short[dcmColumns*y+x] > max)
						{
							max = dicom_buf_short[dcmColumns*y+x];
						}
						if (dicom_buf_short[dcmColumns*y+x] < min)
						{
							min = dicom_buf_short[dcmColumns*y+x];
						}
					} 

				} 
			} 
			else 
			{ 
				dicomDataset->findAndGetUint8Array(DCM_PixelData, dicom_buf_char); 
				int counter=0; 
				for(int y=0;y<dcmRows;y++) 
				{ 
					for(int x=0;x<dcmColumns;x++) 
					{ 
						dicomSliceVTKImageData->GetPointData()->GetScalars()->SetTuple1(counter, dicom_buf_char[dcmColumns*y+x]); 
						counter++;

            if (paddingValue != 0 && paddingValue ==  dicom_buf_char[dcmColumns*y+x])
            {
              continue;
            }
						if (dicom_buf_char[dcmColumns*y+x] > max)
						{
							max = dicom_buf_char[dcmColumns*y+x];
						}
						if (dicom_buf_char[dcmColumns*y+x] < min)
						{
							min = dicom_buf_char[dcmColumns*y+x];
						}
					} 
				} 
			}

      long correction = 0;
      if (strcmp((char *)dcmModality, "CT" ) != 0 && paddingValue != 0)
      {
        if (val_long==16) 
			  { 
				  dicomDataset->findAndGetUint16Array(DCM_PixelData, dicom_buf_short); 
				  int counter=0; 
				  for(int y=0;y<dcmRows;y++) 
				  { 
					  for(int x=0;x<dcmColumns;x++) 
					  { 
              long value = dicom_buf_short[dcmColumns*y+x];
              if (value == paddingValue)
              {
                dicomSliceVTKImageData->GetPointData()->GetScalars()->SetTuple1(counter, min);
              }
						  counter++;
					  } 

				  } 
			  } 
			  else 
			  { 
				  dicomDataset->findAndGetUint8Array(DCM_PixelData, dicom_buf_char); 
				  int counter=0; 
				  for(int y=0;y<dcmRows;y++) 
				  { 
					  for(int x=0;x<dcmColumns;x++) 
					  { 
              long value = dicom_buf_short[dcmColumns*y+x];
              if (value == paddingValue)
              {
						    dicomSliceVTKImageData->GetPointData()->GetScalars()->SetTuple1(counter, min); 
              }
              counter++;
						 
					  } 
				  } 
			  }
      }

			dicomSliceVTKImageData->Update();

			if (dcmRescaleSlope != 1 || dcmRescaleIntercept != 0)
			{
				//If these tags aren't defined it's necessary to compute smallest and largest values
				if (dcmSmallestImagePixelValue == 0 && dcmLargestImagePixelValue == 0)
				{
					dcmSmallestImagePixelValue = min;
					dcmLargestImagePixelValue = max;
				}

				vtkDataArray *scalarsRescaled = NULL;
				int newMaxValue = dcmLargestImagePixelValue*dcmRescaleSlope+dcmRescaleIntercept;
				int newMinValue = dcmSmallestImagePixelValue*dcmRescaleSlope+dcmRescaleIntercept;
				if (newMaxValue <= VTK_UNSIGNED_CHAR_MAX && newMinValue >= VTK_UNSIGNED_CHAR_MIN)
				{
					scalarsRescaled = vtkUnsignedCharArray::New();
				}
				else if (newMaxValue <= VTK_CHAR_MAX && newMinValue >= VTK_CHAR_MIN)
				{
					scalarsRescaled = vtkCharArray::New();
				}
				else if (newMaxValue <= VTK_UNSIGNED_SHORT_MAX && newMinValue >= VTK_UNSIGNED_SHORT_MIN)
				{
					scalarsRescaled = vtkUnsignedShortArray::New();
				}
				else if (newMaxValue <= VTK_SHORT_MAX && newMinValue >= VTK_SHORT_MIN)
				{
					scalarsRescaled = vtkShortArray::New();
				}


				if (scalarsRescaled == NULL)
				{
					if(!this->m_TestMode)
					{
						wxLogMessage(wxString::Format("Inconsistent scalar values. Can not import file <%s>",currentSliceABSFileName));
						errorOccurred = true;
						continue;
					}
				}

				scalarsRescaled->SetName("Scalars");

				int scalarType = dicomSliceVTKImageData->GetScalarType();

				if (dicomSliceVTKImageData->GetScalarType() == VTK_UNSIGNED_SHORT)
				{
					vtkUnsignedShortArray *scalars=vtkUnsignedShortArray::SafeDownCast(dicomSliceVTKImageData->GetPointData()->GetScalars());
					scalarsRescaled->SetNumberOfTuples(scalars->GetNumberOfTuples());
					for(int indexScalar=0;indexScalar<dicomSliceVTKImageData->GetPointData()->GetScalars()->GetNumberOfTuples();indexScalar++)
					{
						scalarsRescaled->SetTuple1(indexScalar,scalars->GetTuple1(indexScalar)*dcmRescaleSlope+dcmRescaleIntercept);//modify scalars using slope and intercept
					}
				}
				else if (dicomSliceVTKImageData->GetScalarType() == VTK_SHORT)
				{
					vtkShortArray *scalars=vtkShortArray::SafeDownCast(dicomSliceVTKImageData->GetPointData()->GetScalars());
					scalarsRescaled->SetNumberOfTuples(scalars->GetNumberOfTuples());
					for(int indexScalar=0;indexScalar<dicomSliceVTKImageData->GetPointData()->GetScalars()->GetNumberOfTuples();indexScalar++)
					{
						scalarsRescaled->SetTuple1(indexScalar,scalars->GetTuple1(indexScalar)*dcmRescaleSlope+dcmRescaleIntercept);//modify scalars using slope and intercept
					}
				}
				else if (dicomSliceVTKImageData->GetScalarType() == VTK_CHAR)
				{
					vtkCharArray *scalars=vtkCharArray::SafeDownCast(dicomSliceVTKImageData->GetPointData()->GetScalars());
					scalarsRescaled->SetNumberOfTuples(scalars->GetNumberOfTuples());
					for(int indexScalar=0;indexScalar<dicomSliceVTKImageData->GetPointData()->GetScalars()->GetNumberOfTuples();indexScalar++)
					{
						scalarsRescaled->SetTuple1(indexScalar,scalars->GetTuple1(indexScalar)*dcmRescaleSlope+dcmRescaleIntercept);//modify scalars using slope and intercept
					}
				}
				else if (dicomSliceVTKImageData->GetScalarType() == VTK_UNSIGNED_CHAR)
				{
					vtkUnsignedCharArray *scalars=vtkUnsignedCharArray::SafeDownCast(dicomSliceVTKImageData->GetPointData()->GetScalars());
					scalarsRescaled->SetNumberOfTuples(scalars->GetNumberOfTuples());
					for(int indexScalar=0;indexScalar<dicomSliceVTKImageData->GetPointData()->GetScalars()->GetNumberOfTuples();indexScalar++)
					{
						scalarsRescaled->SetTuple1(indexScalar,scalars->GetTuple1(indexScalar)*dcmRescaleSlope+dcmRescaleIntercept);//modify scalars using slope and intercept
					}
				}

				dicomSliceVTKImageData->GetPointData()->SetScalars(scalarsRescaled);
				dicomSliceVTKImageData->Update();

				vtkDEL(scalarsRescaled);
			}

			const char *dcmPatientPosition = "?";
			dicomDataset->findAndGetString(DCM_PatientPosition,dcmPatientPosition);
			m_PatientPosition = dcmPatientPosition;

			const char *dcmStudyInstanceUID = "?";
			dicomDataset->findAndGetString(DCM_StudyInstanceUID,dcmStudyInstanceUID);

			const char *dcmSeriesInstanceUID = "?";
			dicomDataset->findAndGetString(DCM_SeriesInstanceUID,dcmSeriesInstanceUID);

			//vector of string composed by:
			//-studyUID
			//-seriesUID
			//-name to applied to the "series listbox"
			std::vector<mafString> seriesId;
			seriesId.push_back(dcmStudyInstanceUID);
			seriesId.push_back(dcmSeriesInstanceUID);
			mafString seriesName = "series_";
			seriesName.Append(dcmModality);
			seriesName.Append("_");

			bool seriesExist = false;
			int seriesCounter = 0;

			if (!this->m_TestMode)
			{
				enableToRead = ((mafGUIDicomSettings*)GetSetting())->EnableToRead((char*)dcmModality);
			}

			//------------------------
			// (Start) Not MR handling
			// REFACTORING TODO: Refactor toward strategy when regression will be available
			//------------------------
			if (enableToRead && strcmp((char *)dcmModality, "MR" ) != 0)
			{
				wxString stringMode = dcmModality;
				if(stringMode.Find("SCOUT") != -1)
				{
					continue;
				}

				std::map<std::vector<mafString>,medDicomSeriesSliceList*>::iterator it;
				for ( it=m_SeriesIDToSlicesListMap.begin() ; \
					it != m_SeriesIDToSlicesListMap.end(); it++ )
				{
					if(seriesId.at(0) == (*it).first.at(0))
					{
						seriesCounter++;
						if (seriesId.at(1) == (*it).first.at(1))
						{
							seriesId.push_back((*it).first.at(2));
							seriesExist = true;
							break;
						}
					}
				}

				// if the series does not exists already
				if (!seriesExist)
				{
					bool containsRotations = currentSliceIsRotated;
					m_SeriesIDContainsRotationsMap[seriesId] = containsRotations;

					m_NumberOfStudies++;
					// the study is not present into the listbox, so need to create new
					// list of files related to the new studyID
					medDicomSeriesSliceList *dicomSeries = new medDicomSeriesSliceList;
					m_DicomReaderModality=-1;

					if(useDefaultPos)
					{
						dcmImagePositionPatient[0] = 0.0;
						dcmImagePositionPatient[1] = 0.0;
						dcmImagePositionPatient[2] = 0.0;
					} 
					else
					{
						dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,dcmImagePositionPatient[0],0);
						dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,dcmImagePositionPatient[1],1);
						dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,dcmImagePositionPatient[2],2);
					}

					lastZPos = dcmImagePositionPatient[2];
					dicomSliceVTKImageData->SetOrigin(dcmImagePositionPatient);
					dicomSliceVTKImageData->Update();

					const char *date,*description,*patientName,*birthdate,*photometricInterpretation;
					FindAndGetDicomStrings(dicomDataset, birthdate, date, description, patientName, photometricInterpretation);

					if (!this->m_TestMode)
					{
						if(((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::TRADITIONAL)
						{
							seriesName.Append(wxString::Format("%i_%ix%i",seriesCounter, dcmRows, dcmColumns));
						}
					//	else if (((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::DESCRIPTION_DATE)
						else if (((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::DESCRIPTION_NUMSLICES)
						{
							//seriesName = (wxString::Format("%s_%s_%d",description,date,seriesIndex));
							seriesName = (wxString::Format("%s_%ix%i",description,dcmRows,dcmColumns));
							seriesIndex++;
						}
						else
						{
							//seriesName.Append(wxString::Format("%i_%ix%i",seriesCounter, dcmRows, dcmColumns));
							seriesName.Append(wxString::Format("%i",seriesCounter));
							//seriesName.Append(description);
						}
					}
					else
					{
						seriesName.Append(wxString::Format("%i_%ix%i",seriesCounter, dcmRows, dcmColumns));
					}

					seriesId.push_back(seriesName);

					dicomSeries->Append(new mafDicomSlice\
						(m_CurrentSliceABSFileName,dcmImagePositionPatient, dcmImageOrientationPatient, \
						dicomSliceVTKImageData,description,date,patientName,birthdate));

					((mafDicomSlice *)dicomSeries->Last()->GetData())->SetDcmModality(dcmModality);
					((mafDicomSlice *)dicomSeries->Last()->GetData())->SetPhotometricInterpretation(photometricInterpretation);


					m_SeriesIDToSlicesListMap.insert\
						(std::pair<std::vector<mafString>,medDicomSeriesSliceList*>\
						(seriesId,dicomSeries));

					m_SeriesIDstringToSeriesIDint.insert(std::pair<std::vector<mafString>,int>(seriesId,seriesCounter));
					
					m_dcm_dim.clear();
					mafString dimname;
					dimname.Append(wxString::Format("%ix%ix",dcmRows, dcmColumns));
					m_dcm_dim.insert(std::pair<mafString,int>(dimname,1));
					m_SeriesIDstringToSeriesDimensionMap.insert(std::pair<std::vector<mafString>,std::map<mafString,int>>(seriesId,m_dcm_dim));

					if (!this->m_TestMode)
					{
						FillStudyListBox(seriesId.at(0));
					}
				}
				else // series exists already
				{
					bool currentSeriesContainsRotations = m_SeriesIDContainsRotationsMap[seriesId];

					if (currentSliceIsRotated && !currentSeriesContainsRotations)
					{
						m_SeriesIDContainsRotationsMap[seriesId] = true;
					}

					if(useDefaultPos)
					{
						dcmImagePositionPatient[0] = 0.0;
						dcmImagePositionPatient[1] = 0.0;
						dcmImagePositionPatient[2] = 0.0;
					} 
					else
					{
						dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,dcmImagePositionPatient[0],0);
						dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,dcmImagePositionPatient[1],1);
						dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,dcmImagePositionPatient[2],2);
					}

					dicomSliceVTKImageData->SetOrigin(dcmImagePositionPatient);
					dicomSliceVTKImageData->Update();

					if  (sliceNum > 1)
					{
						double distancePercentage = ((lastDistance - (fabs(lastZPos - dcmImagePositionPatient[2])))*100)/lastDistance;

						// Check if slices are under tolerance distance
						if ((enableScalarTolerance && (fabs(lastZPos - dcmImagePositionPatient[2]) < scalarTolerance)) || (enablePercentageTolerance) && (distancePercentage > percentageTolerance))
						{
							wxLogMessage(wxString::Format("Warning: file <%s> and <%s> are under distance tolerance.",currentSliceABSFileName.GetCStr(),lastFileName.GetCStr()));
							int answer = wxMessageBox(wxString::Format("Found 2 slices under distance tolerance. Please check the log area for details. Continue?"),"Warning!!", wxYES_NO, NULL);
							if (answer == wxNO)
							{
								progressHelper->CloseProgressBar();
								return false;
							}

						}
						else
						{
							lastDistance = fabs(lastZPos - dcmImagePositionPatient[2]);
						}           
					}

					lastZPos = dcmImagePositionPatient[2];

					const char *date,*description,*patientName,*birthdate, *photometricInterpretation;
					FindAndGetDicomStrings(dicomDataset, birthdate, date, description, patientName, photometricInterpretation);

					m_SeriesIDToSlicesListMap[seriesId]->Append(\
						new mafDicomSlice(m_CurrentSliceABSFileName,dcmImagePositionPatient, \
						dcmImageOrientationPatient, dicomSliceVTKImageData,description,date,patientName,birthdate));

					((mafDicomSlice *)m_SeriesIDToSlicesListMap[seriesId]->Last()->GetData())->SetDcmModality(dcmModality);
					((mafDicomSlice *)m_SeriesIDToSlicesListMap[seriesId]->Last()->GetData())->SetPhotometricInterpretation(photometricInterpretation);

					mafString dimname;
					dimname.Append(wxString::Format("%ix%ix",dcmRows, dcmColumns));
					std::map<mafString,int>::iterator it;
					it = m_dcm_dim.find(dimname);
					if( it == m_dcm_dim.end()) {
					  m_dcm_dim.insert(std::pair<mafString,int>(dimname,1));
					}
					else {
                       it->second++;
					}
					m_SeriesIDstringToSeriesDimensionMap.at(seriesId) = m_dcm_dim;

				}
			}
			//------------------------
			// (End) Not MR handling
			// REFACTORING TODO: Refactor toward strategy when regression will be available
			//------------------------

			//------------------------
			// (Start) Cine (MR) handling
			// REFACTORING TODO: Refactor toward strategy when regression will be available
			//------------------------
			else if ( enableToRead && strcmp( (char *)dcmModality, "MR" ) == 0)
			{
				// MR and CineMRHandling
				seriesExist = false;
				seriesCounter = 0;
				std::map<std::vector<mafString>,medDicomSeriesSliceList*>::iterator it;
				for ( it=m_SeriesIDToSlicesListMap.begin() ; it != m_SeriesIDToSlicesListMap.end(); it++ )
				{
					if(seriesId.at(0) == (*it).first.at(0))
					{
						seriesCounter++;

						if (seriesId.at(1) == (*it).first.at(1))
						{
							seriesId.push_back((*it).first.at(2));
							seriesExist = true;
							break;
						}
					}
				}

				// if series does not exists already
				if (!seriesExist)
				{
					m_NumberOfStudies++;

					bool containsRotations = currentSliceIsRotated;
					m_SeriesIDContainsRotationsMap[seriesId] = containsRotations;

					// the study is not present into the listbox, so need to create new
					// list of files related to the new studyID
					medDicomSeriesSliceList *dicomSeries = new medDicomSeriesSliceList;

					m_DicomReaderModality=-1;

					if(useDefaultPos)
					{
						dcmImagePositionPatient[0] = 0.0;
						dcmImagePositionPatient[1] = 0.0;
						dcmImagePositionPatient[2] = 0.0;
					} 
					else
					{
						dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,dcmImagePositionPatient[0],0);
						dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,dcmImagePositionPatient[1],1);
						dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,dcmImagePositionPatient[2],2);
					}

					dicomSliceVTKImageData->SetOrigin(dcmImagePositionPatient);
					dicomSliceVTKImageData->Update();

					dicomDataset->findAndGetLongInt(DCM_InstanceNumber,dcmInstanceNumber);
					dicomDataset->findAndGetLongInt(DCM_CardiacNumberOfImages,dcmCardiacNumberOfImages);
					dicomDataset->findAndGetFloat64(DCM_TriggerTime,dcmTriggerTime);
					lastZPos = dcmImagePositionPatient[2];

					if(dcmCardiacNumberOfImages>1)
					{
						if (m_DicomReaderModality==-1)
						{  
							m_DicomReaderModality=mafGUIDicomSettings::ID_CMRI_MODALITY;              
						}
						else if(m_DicomReaderModality!=mafGUIDicomSettings::ID_CMRI_MODALITY)
						{
							if(!this->m_TestMode)
							{
								//cppDEL(busyInfo);
								wxString msg = _("cMRI damaged !");
								wxMessageBox(msg,"Confirm", wxOK , NULL);								
							}
							return false;
						}
					}
					else
					{
						if (m_DicomReaderModality==-1)
							m_DicomReaderModality=mafGUIDicomSettings::ID_MRI_MODALITY;
						else if(m_DicomReaderModality!=mafGUIDicomSettings::ID_MRI_MODALITY)
						{
							if(!this->m_TestMode)
							{
								//cppDEL(busyInfo);
								wxString msg = _("cMRI damaged !");
								wxMessageBox(msg,"Confirm", wxOK , NULL);
							}
							return false;                
						}
					}

					const char *date,*description,*patientName,*birthdate, *photometricInterpretation;
					FindAndGetDicomStrings(dicomDataset, birthdate, date, description, patientName, photometricInterpretation);
					
					if(((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::TRADITIONAL)
					{
						seriesName.Append(wxString::Format("%i_%ix%i",seriesCounter, dcmRows, dcmColumns));
					}
					//else if (((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::DESCRIPTION_DATE)
					else if (((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::DESCRIPTION_NUMSLICES)
					{
						//seriesName = (wxString::Format("%s_%s_%d",description,date,seriesIndex));
						seriesName = (wxString::Format("%s_%ix%i",description,dcmRows,dcmColumns));
						seriesIndex++;
					}
					else//CUSTOM
					{
						//seriesName.Append(wxString::Format("%i_%ix%i",seriesCounter, dcmRows, dcmColumns));
						//seriesName.Append(description);
						seriesName.Append(wxString::Format("%i",seriesCounter));
					}

					seriesId.push_back(seriesName);

					dicomSeries->Append(new mafDicomSlice\
						(m_CurrentSliceABSFileName,dcmImagePositionPatient, dcmImageOrientationPatient, \
						dicomSliceVTKImageData,description,date,patientName,birthdate, dcmInstanceNumber, dcmCardiacNumberOfImages, dcmTriggerTime));
					((mafDicomSlice *)dicomSeries->Last()->GetData())->SetDcmModality(dcmModality);
					((mafDicomSlice *)dicomSeries->Last()->GetData())->SetPhotometricInterpretation(photometricInterpretation);


					m_SeriesIDToSlicesListMap.insert\
						(std::pair<std::vector<mafString>,medDicomSeriesSliceList*>\
						(seriesId,dicomSeries));

					m_SeriesIDstringToSeriesIDint.insert(std::pair<std::vector<mafString>,int>(seriesId,seriesCounter));

					m_dcm_dim.clear();
					mafString dimname;
					dimname.Append(wxString::Format("%ix%ix",dcmRows, dcmColumns));
					m_dcm_dim.insert(std::pair<mafString,int>(dimname,1));
					m_SeriesIDstringToSeriesDimensionMap.insert(std::pair<std::vector<mafString>,std::map<mafString,int>>(seriesId,m_dcm_dim));

					if (!this->m_TestMode)
					{
						FillStudyListBox(seriesId.at(0));
					}	
				}
				else // if series exists already
				{
					bool currentSeriesContainsRotations = m_SeriesIDContainsRotationsMap[seriesId];

					if (currentSliceIsRotated && !currentSeriesContainsRotations)
					{
						m_SeriesIDContainsRotationsMap[seriesId] = true;
					}

					if(useDefaultPos)
					{
						dcmImagePositionPatient[0] = 0.0;
						dcmImagePositionPatient[1] = 0.0;
						dcmImagePositionPatient[2] = 0.0;
					} 
					else
					{
						dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,dcmImagePositionPatient[0],0);
						dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,dcmImagePositionPatient[1],1);
						dicomDataset->findAndGetFloat64(DCM_ImagePositionPatient,dcmImagePositionPatient[2],2);
					}

					dicomSliceVTKImageData->SetOrigin(dcmImagePositionPatient);
					dicomSliceVTKImageData->Update();

					dicomDataset->findAndGetLongInt(DCM_InstanceNumber,dcmInstanceNumber);
					dicomDataset->findAndGetLongInt(DCM_CardiacNumberOfImages,dcmCardiacNumberOfImages);
					dicomDataset->findAndGetFloat64(DCM_TriggerTime,dcmTriggerTime);

					const char *date,*description,*patientName,*birthdate, *photometricInterpretation;
					FindAndGetDicomStrings(dicomDataset, birthdate, date, description, patientName, photometricInterpretation);

					m_SeriesIDToSlicesListMap[seriesId]->Append\
						(new mafDicomSlice(m_CurrentSliceABSFileName,dcmImagePositionPatient,dcmImageOrientationPatient ,\
						dicomSliceVTKImageData,description,date,patientName,birthdate,dcmInstanceNumber,dcmCardiacNumberOfImages,dcmTriggerTime));
					((mafDicomSlice *)m_SeriesIDToSlicesListMap[seriesId]->Last()->GetData())->SetDcmModality(dcmModality);
					((mafDicomSlice *)m_SeriesIDToSlicesListMap[seriesId]->Last()->GetData())->SetPhotometricInterpretation(photometricInterpretation);

					mafString dimname;
					dimname.Append(wxString::Format("%ix%ix",dcmRows, dcmColumns));
					std::map<mafString,int>::iterator it;
					it = m_dcm_dim.find(dimname);
					if( it == m_dcm_dim.end()) {
						m_dcm_dim.insert(std::pair<mafString,int>(dimname,1));
					}
					else {
						it->second++;
					}
					m_SeriesIDstringToSeriesDimensionMap.at(seriesId) = m_dcm_dim;
				}
			}

			//------------------------
			// (End) Cine (MR) handling
			// REFACTORING TODO: Refactor toward strategy when regression will be available
			//------------------------

			progressHelper->UpdateProgressBar(i * 100 / m_DICOMDirectoryReader->GetNumberOfFiles());
				
			dicomImg.clear();
			seriesId.clear();
		}

		for (int i=0;i<3;++i)
		{
			dcmImagePositionPatient_old[i] = dcmImagePositionPatient[i];
		}

		lastFileName = m_CurrentSliceABSFileName;

		time(&end);

		double elapsedTime = difftime(end, start);

		// needed to refresh the busy info
		//if (elapsedTime > 0.5)
		//{
		//	if (!m_TestMode)
		//	{
		//		cppDEL(busyInfo);
		//		busyInfo = new wxBusyInfo(busyMessage);
		//	}
		//}
	}
	//end reading files 

	return true;
}

//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::FindAndGetDicomStrings(DcmDataset * dicomDataset, const char *&birthdate, const char *&date, const char *&description, const char *&patientName, const char *&photometricInterpretation)
{
	dicomDataset->findAndGetString(DCM_PatientsBirthDate, birthdate);
	dicomDataset->findAndGetString(DCM_StudyDate, date);
	dicomDataset->findAndGetString(DCM_SeriesDescription, description);
	dicomDataset->findAndGetString(DCM_PatientsName, patientName);
	dicomDataset->findAndGetString(DCM_PhotometricInterpretation, photometricInterpretation);
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
void mafOpImporterDicomOffis::ResetStructure()
	//----------------------------------------------------------------------------
{
	if(!this->m_TestMode)
	{
		EnableSliceSlider(false);
		EnableTimeSlider(false);
		//		m_SeriesListbox->Clear();
		m_SeriesListctrl->ClearAll();
	}

	std::map<std::vector<mafString>,medDicomSeriesSliceList*>::iterator it;
	for ( it=m_SeriesIDToSlicesListMap.begin() ; it != m_SeriesIDToSlicesListMap.end(); it++ )
	{
		m_SeriesIDToSlicesListMap[(*it).first]->DeleteContents(TRUE);
		delete m_SeriesIDToSlicesListMap[(*it).first];
	}

	m_SeriesIDToSlicesListMap.clear();

	if(!this->m_TestMode)
	{
		m_StudyListbox->Clear();
		//		m_StudyListctrl->ClearAll();
		//		m_SeriesListbox->Clear();
		m_SeriesListctrl->ClearAll();
	}
	m_NumberOfStudies		= 0;
	m_NumberOfSlices	= 0;
	m_CurrentSlice		= 0;
	m_NumberOfTimeFrames = 0;
	m_CurrentTime				= 0; 
	m_DicomReaderModality			= -1;

	m_CropFlag				= false;

	mafYield();
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::ResetSliders()
	//----------------------------------------------------------------------------
{
	if(m_LoadGuiLeft)
	{
		m_LoadPage->RemoveGuiLowerLeft(m_LoadGuiLeft);
		delete m_LoadGuiLeft;
		m_LoadGuiLeft = new mafGUI(this);
		m_SliceScannerLoadPage=m_LoadGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,m_NumberOfSlices-1,"",((mafGUIDicomSettings*)GetSetting())->EnableNumberOfSlice() != FALSE);
		m_SliceScannerLoadPage->SetPageSize(1);
		if(((mafGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
		{
			m_TimeScannerLoadPage=m_LoadGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,max(m_NumberOfTimeFrames - 1,0));
			m_TimeScannerLoadPage->SetPageSize(1);
			m_LoadGuiLeft->Enable(ID_SCAN_TIME,(m_NumberOfTimeFrames>0));
		}
		m_LoadPage->AddGuiLowerLeft(m_LoadGuiLeft);
	}

	if(m_CropGuiLeft)
	{
		m_CropPage->RemoveGuiLowerLeft(m_CropGuiLeft);
		delete m_CropGuiLeft;
		m_CropGuiLeft = new mafGUI(this);
		m_SliceScannerCropPage=m_CropGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,m_NumberOfSlices-1,"",((mafGUIDicomSettings*)GetSetting())->EnableNumberOfSlice() != FALSE);
		m_SliceScannerCropPage->SetPageSize(1);
		if(((mafGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
		{
			m_TimeScannerCropPage=m_CropGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,max(m_NumberOfTimeFrames - 1,0));
			m_TimeScannerCropPage->SetPageSize(1);
			m_CropGuiLeft->Enable(ID_SCAN_TIME,(m_NumberOfTimeFrames>0));
		}
		m_CropPage->AddGuiLowerLeft(m_CropGuiLeft);
	}


	if(m_BuildGuiLeft)
	{
		m_BuildPage->RemoveGuiLowerLeft(m_BuildGuiLeft);
		delete m_BuildGuiLeft;
		m_BuildGuiLeft = new mafGUI(this);
		m_SliceScannerBuildPage=m_BuildGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,m_NumberOfSlices-1,"",((mafGUIDicomSettings*)GetSetting())->EnableNumberOfSlice() != FALSE);
		m_SliceScannerBuildPage->SetPageSize(1);
		if(((mafGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
		{
			m_TimeScannerBuildPage=m_BuildGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,max(m_NumberOfTimeFrames - 1,0));
			m_TimeScannerBuildPage->SetPageSize(1);
			m_BuildGuiLeft->Enable(ID_SCAN_TIME,(m_NumberOfTimeFrames>0));
		}
		m_BuildPage->AddGuiLowerLeft(m_BuildGuiLeft);
	}

	if(m_ReferenceSystemGuiLeft)
	{
		m_ReferenceSystemPage->RemoveGuiLowerLeft(m_ReferenceSystemGuiLeft);
		delete m_ReferenceSystemGuiLeft;
		m_ReferenceSystemGuiLeft = new mafGUI(this);
		m_SliceScannerReferenceSystemPage=m_ReferenceSystemGuiLeft->Slider(ID_SCAN_SLICE,_("slice #"),&m_CurrentSlice,0,m_NumberOfSlices-1,"",((mafGUIDicomSettings*)GetSetting())->EnableNumberOfSlice() != FALSE);
		m_SliceScannerReferenceSystemPage->SetPageSize(1);
		if(((mafGUIDicomSettings*)GetSetting())->EnableNumberOfTime())
		{
			m_TimeScannerReferenceSystemPage=m_ReferenceSystemGuiLeft->Slider(ID_SCAN_TIME,_("time "),&m_CurrentTime,0,max(m_NumberOfTimeFrames - 1,0));
			m_TimeScannerReferenceSystemPage->SetPageSize(1);
			m_ReferenceSystemGuiLeft->Enable(ID_SCAN_TIME,(m_NumberOfTimeFrames>0));
		}
		m_ReferenceSystemPage->AddGuiLowerLeft(m_ReferenceSystemGuiLeft);
	}
}
//----------------------------------------------------------------------------
int mafOpImporterDicomOffis::GetSliceIDInSeries(int timeId, int heigthId)
	//----------------------------------------------------------------------------
{
	if (m_DicomReaderModality != mafGUIDicomSettings::ID_CMRI_MODALITY)
		return heigthId;

	if (this->m_TestMode)
	{
		m_SelectedSeriesID = m_SeriesIDToSlicesListMap.begin()->first;
	}
	m_SelectedSeriesSlicesList = m_SeriesIDToSlicesListMap\
		[m_SelectedSeriesID];

	mafDicomSlice *firstDicomListElement;
	firstDicomListElement = (mafDicomSlice *)m_SelectedSeriesSlicesList->\
		Item(0)->GetData();
	int timeFrames =  firstDicomListElement->GetDcmCardiacNumberOfImages();

	int dicomFilesNumber = m_SelectedSeriesSlicesList->GetCount();

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
	helper = m_SeriesIDToCardiacMRIHelperMap[m_SelectedSeriesID];
	mafLogMessage("%d",__LINE__);
	assert(helper);

	vnl_matrix<double> planeIFrameJFileNumberMatrix = helper->GetFileNumberForPlaneIFrameJIdPlaneMatrix();
	return (planeIFrameJFileNumberMatrix(heigthId, timeId)); 
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::GenerateSliceTexture(int imageID)
	//----------------------------------------------------------------------------
{
	// Description:
	// read the slice number 'slice_num' and generate the texture
	double spacing[3], crop_bounds[6], range[2];
	m_Text = "";

	mafDicomSlice* slice = NULL;
	slice = m_SelectedSeriesSlicesList->Item(imageID)->GetData();
	assert(slice);

	slice->GetVTKImageData()->Update();
	slice->GetVTKImageData()->GetBounds(m_SliceBounds);



	double Origin[3];
	slice->GetVTKImageData()->GetOrigin(Origin);

	double orientation[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
	slice->GetDcmImageOrientationPatient(orientation);
	m_Text.append(wxString::Format("Orientation: %f, %f, %f, %f, %f, %f \nPosition: %f, %f, %f",orientation[0], orientation[1], orientation[2], orientation[3], orientation[4], orientation[5], Origin[0], Origin[1], Origin[2]));
	m_TextMapper->SetInput(m_Text.c_str());
	m_TextMapper->Modified();

	// AACC 26-10-2010: Hack to make it work with ATI RADEON Driver
	m_CropActor->GetMapper()->Modified();
	// End of hack

	if (m_CropFlag) 
	{
		// this condition is entered even if no crop is performed (?)
		slice->GetVTKImageData()->GetSpacing(spacing);

		m_CropPlane->Update();
		m_CropPlane->GetOutput()->GetBounds(crop_bounds);
				
		//Align cropBounds to current grid
		crop_bounds[0]=round(crop_bounds[0]/spacing[0])*spacing[0];
		crop_bounds[1]=round(crop_bounds[1]/spacing[0])*spacing[0];
		crop_bounds[2]=round(crop_bounds[2]/spacing[1])*spacing[1];
		crop_bounds[3]=round(crop_bounds[3]/spacing[1])*spacing[1];

		crop_bounds[0]+=Origin[0];
		crop_bounds[1]+=Origin[0];
		crop_bounds[2]+=Origin[1];
		crop_bounds[3]+=Origin[1];



		crop_bounds[4] = m_SliceBounds[4];
		crop_bounds[5] = m_SliceBounds[5];

		if(crop_bounds[1] > m_SliceBounds[1]) 
			crop_bounds[1] = m_SliceBounds[1];
		if(crop_bounds[3] > m_SliceBounds[3]) 
			crop_bounds[3] = m_SliceBounds[3];

		if(crop_bounds[5] > m_SliceBounds[5]) 
			crop_bounds[5] = m_SliceBounds[5];

		double dim_x_clip = round(((crop_bounds[1] - crop_bounds[0]) / spacing[0]))+1;
		double dim_y_clip = round(((crop_bounds[3] - crop_bounds[2]) / spacing[1]))+1;

		// double dim_x_clip = ceil((double)(((crop_bounds[1] - crop_bounds[0]) / spacing[0]) + 1));
		// double dim_y_clip = ceil((double)(((crop_bounds[3] - crop_bounds[2]) / spacing[1]) + 1));

		vtkMAFSmartPointer<vtkStructuredPoints> clip;

		double origin[3] = {crop_bounds[0], crop_bounds[2], crop_bounds[4]};

		clip->SetOrigin(origin[0], origin[1], origin[2]);// Origin[m_SortAxes]);	
		clip->SetSpacing(spacing);

		int dimension[3] = {dim_x_clip, dim_y_clip, 1};
		clip->SetDimensions(dimension);

		std::ostringstream stringStream;
		stringStream << "**clip** origin: " << origin[0] << " " << origin[1] << " " << origin[2] << " " << std::endl;          
		stringStream << "**clip** dimension: " << dimension[0] << " " << dimension[1] << " " << dimension[2] << " " << std::endl;          
		// mafLogMessage(stringStream.str().c_str());

		clip->Update();

		vtkMAFSmartPointer<vtkProbeFilter> probe;
		probe->SetInput(clip);

		vtkImageData *imageData = NULL;
		imageData = slice->GetVTKImageData();
		assert(imageData != NULL);

		imageData->GetOrigin(origin);
		imageData->GetDimensions(dimension);

		stringStream.clear();
		stringStream << "**inputImageData** origin: " << origin[0] << " " << origin[1] << " " << origin[2] << " " << std::endl;          
		stringStream << "**inputImageData** dimension: " << dimension[0] << " " << dimension[1] << " " << dimension[2] << " " << std::endl;          
		// mafLogMessage(stringStream.str().c_str());

		probe->SetSource(imageData);
		probe->Update();
		probe->GetOutput()->GetBounds(m_SliceBounds);

		//rescale to 16 bit
		if(m_RescaleTo16Bit == TRUE && m_HighBit == 11)
		{
			RescaleTo16Bit(slice->GetVTKImageData());
		}
		m_SliceTexture->SetInput((vtkImageData *)probe->GetOutput());
	} 
	else 
	{
		//rescale to 16 bit
		if(m_RescaleTo16Bit == TRUE && m_HighBit == 11)
		{
			RescaleTo16Bit(slice->GetVTKImageData());
		}

		slice->GetVTKImageData()->GetScalarRange(range);
		m_SliceTexture->SetInput(slice->GetVTKImageData());
	}

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
void mafOpImporterDicomOffis::ShowSlice()
	//----------------------------------------------------------------------------
{	
	double diffx,diffy;
	diffx=m_SliceBounds[1]-m_SliceBounds[0];
	diffy=m_SliceBounds[3]-m_SliceBounds[2];

	m_SlicePlane->SetOrigin(0,0,0);
	m_SlicePlane->SetPoint1(diffx,0,0);
	m_SlicePlane->SetPoint2(0,diffy,0);
	m_SliceActor->VisibilityOn();
}
//----------------------------------------------------------------------------
vtkImageData* mafOpImporterDicomOffis::GetSliceImageDataFromLocalDicomFileName(mafString sliceName)
	//----------------------------------------------------------------------------
{
	assert(m_SelectedSeriesSlicesList);

	wxString name, path, short_name, ext;
	for (int i = 0; i < m_SelectedSeriesSlicesList->GetCount(); i++)
	{
		name = m_SelectedSeriesSlicesList->Item(i)->GetData()->GetSliceABSFileName();
		wxSplitPath(name, &path, &short_name, &ext);
		if (sliceName.Compare(short_name) == 0)
		{
			m_SelectedSeriesSlicesList->Item(i)->GetData()->GetVTKImageData()->Update();
			return m_SelectedSeriesSlicesList->Item(i)->GetData()->GetVTKImageData();
		}
		else 
		{
			//if dicom file has not extension fit wxSplitPath error
			short_name = short_name + "." + ext;
			if (sliceName.Compare(short_name) == 0)
			{
				m_SelectedSeriesSlicesList->Item(i)->GetData()->GetVTKImageData()->Update();
				return m_SelectedSeriesSlicesList->Item(i)->GetData()->GetVTKImageData();
			}
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::ImportDicomTags()
	//----------------------------------------------------------------------------
{
	if (m_TagArray == NULL) 
		mafNEW(m_TagArray);

	m_TagArray->SetName("TagArray");

	DcmFileFormat dicomImg;  
	DJDecoderRegistration::registerCodecs(); // register JPEG codecs
	DcmRLEDecoderRegistration::registerCodecs();
	OFCondition status = dicomImg.loadFile(m_CurrentSliceABSFileName);//load data into offis structure

	if (!status.good()) 
	{
		if(!this->m_TestMode)
		{
			mafLogMessage(wxString::Format("File <%s> can not be opened",m_CurrentSliceABSFileName),"Warning!!");
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
	while (status.good())
	{
		try
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

					if (tagName.compare("PixelData") == 0)
						m_TagArray->SetTag(mafTagItem(tagName.c_str(),""));
					else
					{
						m_TagArray->SetTag(mafTagItem(tagName.c_str(),string.c_str()));
						if (tagName.compare("PatientPosition") == 0)
						{
							m_PatientPosition = string.c_str();
						}
					}         
				}
			}
		}
		catch (...)
		{
			mafLogMessage("Can not read Dicom tag.");
		}
		status = ds->nextObject(stack, OFTrue);
	} 
	dicomImg.clear();
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::ResampleVolume()
	//----------------------------------------------------------------------------
{
	double m_VolumePosition[3],m_VolumeOrientation[3], m_VolumeSpacing[3];
	m_VolumeSpacing[0] = m_VolumeSpacing[1] = m_VolumeSpacing[2] = VTK_DOUBLE_MAX;

	m_VolumePosition[0]    = m_VolumePosition[1]    = m_VolumePosition[2]    = 0;
	m_VolumeOrientation[0] = m_VolumeOrientation[1] = m_VolumeOrientation[2] = 0;

	mafVMEVolumeGray *tmpVmeVolumeGray;
	mafNEW(tmpVmeVolumeGray);

	mafSmartPointer<mafTransform> box_pose;
	box_pose->SetOrientation(m_VolumeOrientation);
	box_pose->SetPosition(m_VolumePosition);

	mafSmartPointer<mafTransformFrame> local_pose;
	local_pose->SetInput(box_pose);

	mafSmartPointer<mafTransformFrame> output_to_input;

	// In a future version if not a "Natural" data the filter should operate in place.
	mafString new_vme_name = "resampled_";
	new_vme_name += m_VolumeName;

	tmpVmeVolumeGray->SetMatrix(box_pose->GetMatrix());

	double volumeBounds[6];
	vtkRectilinearGrid *rgrid;
	rgrid = vtkRectilinearGrid::SafeDownCast(m_Volume->GetVolumeOutput()->GetVTKData());
	rgrid->Update();
	rgrid->GetBounds(volumeBounds);

	for (int xi = 1; xi < rgrid->GetXCoordinates()->GetNumberOfTuples (); xi++)
	{
		double spcx = rgrid->GetXCoordinates()->GetTuple1(xi)-rgrid->GetXCoordinates()->GetTuple1(xi-1);
		if (m_VolumeSpacing[0] > spcx)
			m_VolumeSpacing[0] = spcx;
	}

	for (int yi = 1; yi < rgrid->GetYCoordinates()->GetNumberOfTuples (); yi++)
	{
		double spcy = rgrid->GetYCoordinates()->GetTuple1(yi)-rgrid->GetYCoordinates()->GetTuple1(yi-1);
		if (m_VolumeSpacing[1] > spcy)
			m_VolumeSpacing[1] = spcy;
	}

	for (int zi = 1; zi < rgrid->GetZCoordinates()->GetNumberOfTuples (); zi++)
	{
		double spcz = rgrid->GetZCoordinates()->GetTuple1(zi)-rgrid->GetZCoordinates()->GetTuple1(zi-1);
		if (m_VolumeSpacing[2] > spcz)
			m_VolumeSpacing[2] = spcz;
	}

	int output_extent[6];
	output_extent[0] = 0;
	output_extent[1] = (volumeBounds[1] - volumeBounds[0]) / m_VolumeSpacing[0];
	output_extent[2] = 0;
	output_extent[3] = (volumeBounds[3] - volumeBounds[2]) / m_VolumeSpacing[1];
	output_extent[4] = 0;
	output_extent[5] = (volumeBounds[5] - volumeBounds[4]) / m_VolumeSpacing[2];

	double w,l,sr[2];
	for (int i = 0; i < m_Volume->GetDataVector()->GetNumberOfItems(); i++)
	{
		if (mafVMEItemVTK *input_item = mafVMEItemVTK::SafeDownCast(m_Volume->GetDataVector()->GetItemByIndex(i)))
		{
			if (vtkDataSet *input_data = input_item->GetData())
			{
				// the resample filter
				vtkMAFSmartPointer<vtkMAFVolumeResample> resampler;
				double m_ZeroPadValue = 0.;
				resampler->SetZeroValue(m_ZeroPadValue);

				// Set the target be vme's parent frame. And Input frame to the root. I've to 
				// set at each iteration since I'm using the SetMatrix, which doesn't support
				// transform pipelines.
				mafSmartPointer<mafMatrix> output_parent_abs_pose;
				m_Input->GetOutput()->GetAbsMatrix(*output_parent_abs_pose.GetPointer(),input_item->GetTimeStamp());
				local_pose->SetInputFrame(output_parent_abs_pose);

				mafSmartPointer<mafMatrix> input_parent_abs_pose;
				m_Input->GetOutput()->GetAbsMatrix(*input_parent_abs_pose.GetPointer(),input_item->GetTimeStamp());
				local_pose->SetTargetFrame(input_parent_abs_pose);
				local_pose->Update();

				mafSmartPointer<mafMatrix> output_abs_pose;
				m_Volume->GetOutput()->GetAbsMatrix(*output_abs_pose.GetPointer(),input_item->GetTimeStamp());
				output_to_input->SetInputFrame(box_pose->GetMatrixPointer());

				mafSmartPointer<mafMatrix> input_abs_pose;
				m_Input->GetOutput()->GetAbsMatrix(*input_abs_pose.GetPointer(),input_item->GetTimeStamp());
				output_to_input->SetTargetFrame(input_abs_pose);
				output_to_input->Update();

				double origin[3];
				origin[0] = volumeBounds[0];
				origin[1] = volumeBounds[2];
				origin[2] = volumeBounds[4];

				output_to_input->TransformPoint(origin,origin);
				resampler->SetVolumeOrigin(origin[0],origin[1],origin[2]);
				vtkMatrix4x4 *mat = output_to_input->GetMatrix().GetVTKMatrix();

				// extract versors
				double x_axis[3],y_axis[3];

				mafMatrix::GetVersor(0,mat,x_axis);
				mafMatrix::GetVersor(1,mat,y_axis);

				resampler->SetVolumeAxisX(x_axis);
				resampler->SetVolumeAxisY(y_axis);

				vtkMAFSmartPointer<vtkStructuredPoints> outputSPData;
				outputSPData->SetSpacing(m_VolumeSpacing);
				// TODO: here I probably should allow a data type casting... i.e. a GUI widget
				outputSPData->SetScalarType(input_data->GetPointData()->GetScalars()->GetDataType());
				outputSPData->SetExtent(output_extent);
				outputSPData->SetUpdateExtent(output_extent);

				input_data->GetScalarRange(sr);

				w = sr[1] - sr[0];
				l = (sr[1] + sr[0]) * 0.5;

				resampler->SetWindow(w);
				resampler->SetLevel(l);
				resampler->SetInput(input_data);
				resampler->SetOutput(outputSPData);
				resampler->AutoSpacingOff();
				resampler->Update();

				outputSPData->SetSource(NULL);
				if(m_DiscardPosition == TRUE)
				{
					outputSPData->SetOrigin(0,0,0);
				}
				else
				{
					outputSPData->SetOrigin(volumeBounds[0],volumeBounds[2],volumeBounds[4]);
				}
				tmpVmeVolumeGray->SetDataByDetaching(outputSPData, input_item->GetTimeStamp());
				tmpVmeVolumeGray->Update();
			}
		}
	}
	m_Volume->DeepCopy(tmpVmeVolumeGray);
	m_Volume->Update();

	mafDEL(tmpVmeVolumeGray);
}
//----------------------------------------------------------------------------
void mafOpImporterDicomOffis::RescaleTo16Bit(vtkImageData *dataSet)
	//----------------------------------------------------------------------------
{
	if(dataSet->GetScalarType() == VTK_UNSIGNED_SHORT) return;

	int i=0, size = dataSet->GetNumberOfPoints();
	vtkMAFSmartPointer<vtkUnsignedShortArray> newScalars;
	for(;i<size;i++)
	{
		double value = dataSet->GetPointData()->GetScalars()->GetTuple1(i);
		value = (value - m_RescaleIntercept) * ((double)VTK_UNSIGNED_SHORT_MAX) / (4095.);
    if (value < -1024)
    {
      value = -1024.0;
    }
		newScalars->InsertNextTuple1(value);
	}
	dataSet->GetPointData()->SetScalars(newScalars);
	dataSet->SetScalarTypeToUnsignedShort();
	dataSet->Update();

}
//----------------------------------------------------------------------------
int CompareX(const mafDicomSlice **arg1,const mafDicomSlice **arg2)
	//----------------------------------------------------------------------------
{
	// compare the x coordinate of both arguments
	// return:

	double loc[3] = {-9999,-9999,-9999};

	(*(mafDicomSlice **)arg1)->GetDcmImagePositionPatient(loc);
	double x1 = loc[0];

	(*(mafDicomSlice **)arg2)->GetDcmImagePositionPatient(loc);
	double x2 = loc[0];

	if (x1 > x2)
		return 1;
	if (x1 < x2)
		return -1;
	else
		return 0;
}
//----------------------------------------------------------------------------
int CompareY(const mafDicomSlice **arg1,const mafDicomSlice **arg2)
	//----------------------------------------------------------------------------
{
	// compare the y coordinate of both arguments
	// return:

	double loc[3] = {-9999,-9999,-9999};

	(*(mafDicomSlice **)arg1)->GetDcmImagePositionPatient(loc);
	double y1 = loc[1];

	(*(mafDicomSlice **)arg2)->GetDcmImagePositionPatient(loc);
	double y2 = loc[1];

	if (y1 > y2)
		return 1;
	if (y1 < y2)
		return -1;
	else
		return 0;
}
//----------------------------------------------------------------------------
int CompareZ(const mafDicomSlice **arg1,const mafDicomSlice **arg2)
	//----------------------------------------------------------------------------
{
	// compare the z coordinate of both arguments
	// return:


	double loc[3] = {-9999,-9999,-9999};

	(*(mafDicomSlice **)arg1)->GetDcmImagePositionPatient(loc);
	double z1 = loc[2];

	(*(mafDicomSlice **)arg2)->GetDcmImagePositionPatient(loc);
	double z2 = loc[2];

	if (z1 > z2)
		return 1;
	if (z1 < z2)
		return -1;
	else
		return 0;
}
//----------------------------------------------------------------------------
int CompareTriggerTime(const mafDicomSlice **arg1,const mafDicomSlice **arg2)
	//----------------------------------------------------------------------------
{
	// compare the trigger time of both arguments
	// return:
	float t1 = (*(mafDicomSlice **)arg1)->GetDcmTriggerTime();
	float t2 = (*(mafDicomSlice **)arg2)->GetDcmTriggerTime();;
	if (t1 > t2)
		return 1;
	if (t1 < t2)
		return -1;
	else
		return 0;
}

//----------------------------------------------------------------------------
int CompareImageNumber(const mafDicomSlice **arg1,const mafDicomSlice **arg2)
	//----------------------------------------------------------------------------
{
	// compare the trigger time of both arguments
	// return:
	float i1 = (*(mafDicomSlice **)arg1)->GetDcmInstanceNumber();
	float i2 = (*(mafDicomSlice **)arg2)->GetDcmInstanceNumber();;
	if (i1 > i2)
		return 1;
	if (i1 < i2)
		return -1;
	else
		return 0;
}

void mafOpImporterDicomOffis::OnVmeTypeSelected()
{
	m_OutputType = m_RadioButton;
	
	UpdateReferenceSystemPageConnection();
}

void mafOpImporterDicomOffis::OnReferenceSystemSelected()
{
	if(!m_ApplyToAllReferenceSystem)
	{
		((mafDicomSlice *)m_SelectedSeriesSlicesList->Item(m_CurrentSlice)->GetData())->SetReferenceSystem(m_SelectedReferenceSystem);

	}
	else
	{
		m_GlobalReferenceSystem = m_SelectedReferenceSystem;
	}

}
void mafOpImporterDicomOffis::OnSwapReferenceSystemSelected()
{
	if(!m_ApplyToAllReferenceSystem)
	{
		((mafDicomSlice *)m_SelectedSeriesSlicesList->Item(m_CurrentSlice)->GetData())->SetSwapReferenceSystem(m_SwapReferenceSystem);
	}
	else// if(m_ApplyToAllReferenceSystem)
	{
		m_SwapAllReferenceSystem = m_SwapReferenceSystem;
		m_ReferenceSystemGuiUnderLeft->Update();
		m_ReferenceSystemPage->Update();
		m_ReferenceSystemPage->UpdateActor();
	}
}

void mafOpImporterDicomOffis::OnStudySelect()
{
	mafString *st = (mafString *)m_StudyListbox->GetClientData(m_StudyListbox->GetSelection());

	////---------------------------------------------------------------
	//long item = -1;
	//long myitem = 0;
	//for ( ;; )
	//{
	//	myitem = item;
	//	item = m_StudyListctrl->GetNextItem(item,wxLIST_NEXT_ALL,wxLIST_STATE_SELECTED);
	//	if ( item == -1 )
	//		break;
	//}
	////---------------------------------------------------------------
	//mafString *st = (mafString *)m_StudyListctrl->GetItemData(myitem);

	m_SelectedSeriesID.at(0) = st->GetCStr();
	if (m_SelectedSeriesID.at(0).Compare(m_StudyListbox->GetString(m_StudyListbox->GetSelection())) != 0)
		//	if (m_SelectedSeriesID.at(0).Compare(m_StudyListctrl->GetItemText(myitem)) != 0)
	{
		FillSeriesListBox();
		m_SeriesListctrl->SetItemState(FIRST_SELECTION, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
		//		m_SeriesListbox->SetSelection(FIRST_SELECTION);
		OnEvent(&mafEvent(this, ID_SERIES_SELECT));
	}
}

void mafOpImporterDicomOffis::GetDicomRange(double *range)
{
	double sliceRange[2];

	range[0]=MAXDOUBLE;
	range[1]=MINDOUBLE;

	for(int imageID=0;imageID<m_SelectedSeriesSlicesList->size();imageID++)
	{  
		mafDicomSlice* slice = NULL;

		slice = m_SelectedSeriesSlicesList->Item(imageID)->GetData();
		assert(slice);

		slice->GetVTKImageData()->Update();
		slice->GetVTKImageData()->GetScalarRange(sliceRange);

		if (sliceRange[0]<range[0]) range[0]=sliceRange[0];
		if (sliceRange[1]>range[1]) range[1]=sliceRange[1];
	}


}

void mafOpImporterDicomOffis::OnSeriesSelect()
{
	mafString *st = (mafString *)m_StudyListbox->GetClientData(m_StudyListbox->GetSelection());

	////---------------------------------------------------------------
	long item = -1;
	long myitem = 0;
	for ( ;; )
	{
		myitem = item;
		item = m_SeriesListctrl->GetNextItem(item,wxLIST_NEXT_ALL,wxLIST_STATE_SELECTED);
		if ( item == -1 )
			break;
	}
	////---------------------------------------------------------------
	//mafString *st = (mafString *)m_StudyListctrl->GetItemData(myitem);

	m_SelectedSeriesID.at(0) = st->GetCStr();

	//  string selected from the listbox
	//	wxString seriesName = m_SeriesListbox->GetString(m_SeriesListbox->GetSelection());

	//  string selected from the listctrl
	wxString seriesName = m_SeriesListctrl->GetItemText(myitem);

	mafString tmp;


	if(((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::TRADITIONAL)
	{
		tmp = seriesName.SubString(0, seriesName.find_last_of("x")-1);
	}
	//else if (((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::DESCRIPTION_DATE)
	else if (((mafGUIDicomSettings*)GetSetting())->GetOutputNameFormat() == mafGUIDicomSettings::DESCRIPTION_NUMSLICES)
	{
		//tmp = seriesName;
		tmp = seriesName.SubString(0, seriesName.find_last_of("x")-1);
	}
	else
	{
		tmp = seriesName.SubString(0, seriesName.find_first_of("x")-1);
	}
	m_SelectedSeriesID.at(2) = tmp;

	std::map<std::vector<mafString>,medDicomSeriesSliceList*>::iterator it;
	for ( it=m_SeriesIDToSlicesListMap.begin() ; it != m_SeriesIDToSlicesListMap.end(); it++ )
	{
		if ((*it).first.at(0).Compare(m_SelectedSeriesID.at(0)) == 0)
		{ 
			if ((*it).first.at(2).Compare(m_SelectedSeriesID.at(2)) == 0)
			{
				m_SelectedSeriesID.at(1) = (*it).first.at(1);
				break;
			}
		}
	}

	if(!this->m_TestMode)
	{
		m_BuildGuiLeft->Update();
		EnableSliceSlider(true);

		m_SelectedSeriesSlicesList = m_SeriesIDToSlicesListMap[m_SelectedSeriesID];

		mafDicomSlice *element0;
		element0 = (mafDicomSlice *)m_SelectedSeriesSlicesList->Item(0)->GetData();

		int numberOfImages =  element0->GetDcmCardiacNumberOfImages();
		m_DicomReaderModality=-1;
		if(numberOfImages>1)
		{
			m_DicomReaderModality=mafGUIDicomSettings::ID_CMRI_MODALITY;
			EnableTimeSlider(true);
		}
	}
	ReadDicom();


	GetDicomRange(m_TotalDicomRange);
	m_TotalDicomSubRange[0]=m_TotalDicomRange[0];
	m_TotalDicomSubRange[1]=m_TotalDicomRange[1];
	CameraReset();

	if(((mafGUIDicomSettings*)GetSetting())->AutoCropPosition())
	{
		AutoPositionCropPlane();
	}
	else
	{
		m_CropPlane->SetOrigin(0.0,0.0,0.0);
		m_CropPlane->SetPoint1(m_SliceBounds[1]-m_SliceBounds[0],0.0,0.0);
		m_CropPlane->SetPoint2(0.0,m_SliceBounds[3]-m_SliceBounds[2],0.0);
		m_CropPage->GetRWI()->CameraReset();
	}
}

void mafOpImporterDicomOffis::OnWizardChangePage( mafEvent * e )
{

	if(m_Wizard->GetCurrentPage()==m_LoadPage)//From Load page to Crop Page
	{
		//get the current windowing in order to maintain subrange thought the 
		//wizard pages 
		m_LoadPage->GetWindowing(m_TotalDicomRange,m_TotalDicomSubRange);

		if(m_NumberOfStudies<1)
		{
			m_Wizard->EnableChangePageOff();
			wxMessageBox(_("No study found!"));
			return;
		}
		else
		{
			m_Wizard->SetButtonString("Build >");
			m_Wizard->EnableChangePageOn();
			m_CropPage->UpdateActor();
		}
	}

	if (m_Wizard->GetCurrentPage()==m_CropPage)//From Crop page to build page
	{
		//get the current windowing in order to maintain subrange thought the 
		//wizard pages 
		m_CropPage->GetWindowing(m_TotalDicomRange,m_TotalDicomSubRange);
		if (e->GetBool())
		{
			if(m_CropPage)
				Crop();

			m_BuildPage->UpdateActor();

			if(m_ZCropBounds[0] > m_CurrentSlice || m_CurrentSlice > m_ZCropBounds[1])
			{
				m_CurrentSlice = m_ZCropBounds[0];
			}
			m_SliceScannerBuildPage->SetValue(m_CurrentSlice);
			//if only 1 slice, disable radio widget and create a VMEImage
			if (m_ZCropBounds[1]+1 - m_ZCropBounds[0] >1 )
			{
				m_BuildPage->AddGuiLowerUnderLeft(m_BuildGuiCenter);
				m_BuildPage->Update();
				GuiUpdate();
				OnVmeTypeSelected();
			}
			else
				m_OutputType = 2;

			if (/*m_Wizard->GetCurrentPage()==m_BuildPage &&*/ m_OutputType == mafGUIDicomSettings::ID_IMAGE)//Check the type to determine the next step
			{
				m_Wizard->SetButtonString("Reference >");
				m_ReferenceSystemPage->UpdateActor();
				m_Wizard->Update();
			}
			else
			{
				m_Wizard->SetButtonString("Finish");
				m_Wizard->Update();
			}
		} 
		else
		{
			m_Wizard->SetButtonString("Crop >"); 
			m_LoadPage->UpdateActor();
		}
	}

	//From build page to crop page
	if (m_Wizard->GetCurrentPage()==m_BuildPage && (!e->GetBool()))
	{
		//get the current windowing in order to maintain subrange thought the 
		//wizard pages 
		m_BuildPage->GetWindowing(m_TotalDicomRange,m_TotalDicomSubRange);

		OnUndoCrop();
		m_Wizard->SetButtonString("Build >");
		m_BuildPage->RemoveGuiLowerUnderLeft(m_BuildGuiCenter);
		m_CropPage->UpdateActor();
	}

	if (m_Wizard->GetCurrentPage()==m_CropPage)//From Crop page to any other
	{
		m_CropActor->VisibilityOff();
	}
	else
	{
		if(!m_CropExecuted)
			m_CropActor->VisibilityOn();
		else
			m_CropActor->VisibilityOff();
	}

	if (m_Wizard->GetCurrentPage()==m_BuildPage && e->GetBool())//From build page to reference system page
	{
		//m_Wizard->SetButtonString("Reference >");
		m_ReferenceSystemPage->UpdateActor();
		m_ReferenceSystemPage->GetRWI()->CameraReset();
	}
	if (m_Wizard->GetCurrentPage()==m_ReferenceSystemPage && (!e->GetBool()))//From reference system page to build page
	{
		m_Wizard->SetButtonString("Reference >");
		m_BuildPage->UpdateActor();
		m_BuildPage->GetRWI()->CameraReset();
	}

	GuiUpdate();
}

void mafOpImporterDicomOffis::OnMouseDown( mafEvent * e )
{
	if(m_CropExecuted==false)
	{
		if (m_Wizard->GetCurrentPage()==m_CropPage)
		{
			//long handle_id = e->GetArg();
			double pos[3];
			vtkPoints *p = (vtkPoints *)e->GetVtkObj();
			p->GetPoint(0,pos);

			//calcola altezza rettangolo
			double b[6];
			m_CropPlane->GetOutput()->GetBounds(b);
			double dx = (b[1] - b[0]) / 5;
			double dy = (b[3] - b[2]) / 5;

			double O[3], P1[3], P2[3];
			//Modified by Matteo 21/07/2006
			//Caso di default P1 in alto a SX e P2 in basso a DX
			m_CropPlane->GetOrigin(O);
			m_CropPlane->GetPoint1(P1);
			m_CropPlane->GetPoint2(P2);
			//Se non siamo nel caso di default modifichiamo in modo da ritornare in quel caso
			if(P2[0]<P1[0] && P2[1]<P1[1])//Caso P1 in basso a DX e P2 in alto a SX
			{
				O[0] = P2[0];
				O[1] = P1[1];
				double tempx=P1[0];
				double tempy=P1[1];
				P1[0] = P2[0];
				P1[1] = P2[1];
				P2[0] = tempx;
				P2[1] = tempy;
				m_CropPlane->SetOrigin(O);
				m_CropPlane->SetPoint1(P1);
				m_CropPlane->SetPoint2(P2);
			}
			else if(P1[0]<P2[0] && P1[1]>P2[1])//Caso P1 in basso a SX e P2 in alto a DX
			{
				O[0] = P1[0];
				O[1] = P1[1];
				double tempy=P1[1];
				P1[1] = P2[1];
				P2[1] = tempy;
				m_CropPlane->SetOrigin(O);
				m_CropPlane->SetPoint1(P1);
				m_CropPlane->SetPoint2(P2);
			}
			else if(P1[0]>P2[0] && P1[1]<P2[1])//Caso P1 in alto a DX e P2 in basso a SX
			{
				O[0] = P2[0];
				O[1] = P2[1];
				double tempx=P1[0];
				P1[0] = P2[0];
				P2[0] = tempx;
				m_CropPlane->SetOrigin(O);
				m_CropPlane->SetPoint1(P1);
				m_CropPlane->SetPoint2(P2);
			}
			//End Matteo
			if (m_GizmoStatus == GIZMO_NOT_EXIST)
			{
				m_GizmoStatus = GIZMO_RESIZING;
				m_CropActor->VisibilityOn();

				pos[2] = 0;
				m_CropPlane->SetOrigin(pos);
				m_CropPlane->SetPoint1(pos[0], pos[1], pos[2]);
				m_CropPlane->SetPoint2(pos[0], pos[1], pos[2]);
			}
			else if (m_GizmoStatus == GIZMO_DONE)
			{
				//	  8------------1----------2--->x
				//		|												|
				//		7												3
				//		|												|
				//		6------------5----------4
				//		|
				//	  v y

				if (P1[0] + dx/2 <= pos[0] &&  pos[0] <= P2[0] - dx/2 &&
					P1[1] - dy/2 <= pos[1] && pos[1] <= P1[1] + dy/2)
				{
					m_SideToBeDragged = 1;
				}
				else if (P2[0] - dx/2 <= pos[0] && pos[0] <= P2[0] + dx/2 &&
					P1[1] - dy/2 <= pos[1] && pos[1] <= P1[1] + dy/2)
				{
					m_SideToBeDragged = 2;
				}
				else if (P2[0] - dx/2 <= pos[0] && pos[0] <= P2[0] + dx/2 &&
					P2[1] - dy/2 >= pos[1] && pos[1] >= P1[1] + dy/2)
				{
					m_SideToBeDragged = 3;
				}
				else if (P2[0] - dx/2 <= pos[0] && pos[0] <= P2[0] + dx/2 &&
					P2[1] - dy/2 <= pos[1] && pos[1] <= P2[1] + dy/2)
				{
					m_SideToBeDragged = 4;
				}
				else if (P1[0] + dx/2 <= pos[0] && pos[0] <= P2[0] - dx/2 &&
					P2[1] - dy/2 <= pos[1] && pos[1] <= P2[1] + dy/2)
				{
					m_SideToBeDragged = 5;
				}
				else if (P1[0] - dx/2 <= pos[0] && pos[0] <= P1[0] + dx/2 &&
					P2[1] - dy/2 <= pos[1] && pos[1] <= P2[1] +dy/2)
				{
					m_SideToBeDragged = 6;
				}
				else if (P1[0] - dx/2 <= pos[0] && pos[0] <= P1[0] + dx/2 &&
					P2[1] - dy/2 >= pos[1] && pos[1] >= P1[1] + dy/2)
				{
					m_SideToBeDragged = 7;
				}
				else if (P1[0] - dx/2 <= pos[0] && pos[0] <= P1[0] + dx/2 &&
					P1[1] - dy/2 <= pos[1] && pos[1] <= P1[1] + dy/2)
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
			}
			m_CropPage->UpdateActor();
			m_CropPage->GetRWI()->CameraUpdate();
		}
	}

	if (m_Wizard->GetCurrentPage()==m_LoadPage)
	{
		m_LoadPage->UpdateActor();
		m_LoadPage->GetRWI()->CameraUpdate();
	}
	else if (m_Wizard->GetCurrentPage()==m_BuildPage)
	{
		m_BuildPage->UpdateActor();
		m_BuildPage->GetRWI()->CameraUpdate();
	}
}

void mafOpImporterDicomOffis::OnMouseMove( mafEvent * e )
{
	if(m_CropExecuted==false)
	{
		if (m_Wizard->GetCurrentPage()==m_CropPage)
		{
			double pos[3], oldO[3], oldP1[3], oldP2[3];
			vtkPoints *p = (vtkPoints *)e->GetVtkObj();
			p->GetPoint(0,pos);

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
			m_CropPage->GetRWI()->CameraUpdate();
		}
	}
}

void mafOpImporterDicomOffis::OnRangeModified()
{
	double fractpart, intpart;
	double minMax[2];
	m_CropPage->GetZCropBounds(minMax);

	//approximate form int to double
	fractpart = modf (minMax[0] , &intpart);
	fractpart >= 0.5 ?  m_ZCropBounds[0] = ceil(minMax[0]) : m_ZCropBounds[0] = floor(minMax[0]);
	fractpart = modf (minMax[1] , &intpart);
	fractpart >= 0.5 ?  m_ZCropBounds[1] = ceil(minMax[1]) : m_ZCropBounds[1] = floor(minMax[1]);

	m_SliceScannerBuildPage->SetRange(m_ZCropBounds[0], m_ZCropBounds[1]);
	m_SliceScannerReferenceSystemPage->SetRange(m_ZCropBounds[0], m_ZCropBounds[1]);

	if(m_ZCropBounds[0] > m_CurrentSlice || m_CurrentSlice > m_ZCropBounds[1])
	{
		m_CurrentSlice = m_ZCropBounds[0];
		m_SliceScannerBuildPage->SetValue(m_CurrentSlice);
		m_SliceScannerReferenceSystemPage->SetValue(m_CurrentSlice);
		// show the current slice
		int currImageId = GetSliceIDInSeries(m_CurrentTime, m_CurrentSlice);
		if (currImageId != -1) 
		{
			GenerateSliceTexture(currImageId);
			ShowSlice();
			CameraUpdate();
		}
		m_SliceScannerLoadPage->SetValue(m_CurrentSlice);
		m_SliceScannerLoadPage->Update();
		m_SliceScannerCropPage->SetValue(m_CurrentSlice);
		m_SliceScannerCropPage->Update();
		m_SliceScannerBuildPage->SetValue(m_CurrentSlice);
		m_SliceScannerBuildPage->Update();
		m_SliceScannerReferenceSystemPage->SetValue(m_CurrentSlice);
		m_SliceScannerReferenceSystemPage->Update();
	}
	GuiUpdate();
}

void mafOpImporterDicomOffis::OnMouseUp()
{
	if(m_CropExecuted==false)
	{
		if (m_Wizard->GetCurrentPage()==m_CropPage)
		{
			if (m_GizmoStatus == GIZMO_RESIZING)
				m_GizmoStatus = 	GIZMO_DONE;
			else if (m_GizmoStatus == GIZMO_DONE)
				m_SideToBeDragged = 0;

			double p1[3], p2[3], origin[3];
			m_CropPlane->GetPoint1(p1);
			m_CropPlane->GetPoint2(p2);
			m_CropPlane->GetOrigin(origin);

			if( (p1[0] == p2[0] && p1[1] == p2[1] && p1[2] == p2[2])  ||
				(p1[0] == origin[0] && p1[1] == origin[1] && p1[2] == origin[2]) ||
				(p2[0] == origin[0] && p2[1] == origin[1] && p2[2] == origin[2])
				)
			{
				m_BoxCorrect = false;
			}
			else
				m_BoxCorrect = true;   
		}
	}
}

void mafOpImporterDicomOffis::OnScanSlice()
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
		ShowSlice();
		CameraUpdate();
	}

	m_SliceScannerLoadPage->SetValue(m_CurrentSlice);
	m_SliceScannerLoadPage->Update();
	m_SliceScannerCropPage->SetValue(m_CurrentSlice);
	m_SliceScannerCropPage->Update();
	m_SliceScannerBuildPage->SetValue(m_CurrentSlice);
	m_SliceScannerBuildPage->Update();
	m_SliceScannerReferenceSystemPage->SetValue(m_CurrentSlice);
	m_SliceScannerReferenceSystemPage->Update();
	// windows 7 64 bit / nvidia graphic card patch: otherwise scan slice will not work
	// as expected and slices will be black while scanning until mouse dowin
	// is performed in view
	//<patch>
	if (m_Wizard->GetCurrentPage()==m_LoadPage)
	{
		m_LoadPage->UpdateActor();
		m_LoadPage->GetRWI()->CameraUpdate();
	}
	else if (m_Wizard->GetCurrentPage()==m_CropPage)
	{
		m_CropPage->UpdateActor();
		m_CropPage->GetRWI()->CameraUpdate();
	}
	else if (m_Wizard->GetCurrentPage()==m_BuildPage)
	{
		m_BuildPage->UpdateActor();
		m_BuildPage->GetRWI()->CameraUpdate();
	}
	else if (m_Wizard->GetCurrentPage()==m_ReferenceSystemPage)
	{
		UpdateReferenceSystemVariables();
		m_ReferenceSystemGuiUnderLeft->Update();
		m_ReferenceSystemPage->Update();
		m_ReferenceSystemPage->UpdateActor();
		m_ReferenceSystemPage->GetRWI()->CameraUpdate();
	}
	//</patch>
	GuiUpdate();
}

void mafOpImporterDicomOffis::UpdateReferenceSystemVariables()
{
	if(!m_ApplyToAllReferenceSystem)
	{
		m_SelectedReferenceSystem = ((mafDicomSlice *)m_SelectedSeriesSlicesList->Item(m_CurrentSlice)->GetData())->GetReferenceSystem();
		if(!m_SwapAllReferenceSystem)
		{
			m_SwapReferenceSystem = ((mafDicomSlice *)m_SelectedSeriesSlicesList->Item(m_CurrentSlice)->GetData())->GetSwapReferenceSystem();
		}
		else
		{
			m_SwapReferenceSystem = m_SwapAllReferenceSystem;
		}
	}
	else
	{
		m_SelectedReferenceSystem = m_GlobalReferenceSystem;
		m_SwapReferenceSystem = m_SwapAllReferenceSystem;
	}
}

void mafOpImporterDicomOffis::OnScanTime()
{
	int currImageId = GetSliceIDInSeries(m_CurrentTime, m_CurrentSlice);
	if (currImageId != -1) 
	{
		GenerateSliceTexture(currImageId);
		ShowSlice();
		CameraUpdate();
	}
	m_TimeScannerLoadPage->SetValue(m_CurrentTime);
	m_TimeScannerLoadPage->Update();
	m_TimeScannerCropPage->SetValue(m_CurrentTime);
	m_TimeScannerCropPage->Update();
	m_TimeScannerBuildPage->SetValue(m_CurrentTime);
	m_TimeScannerBuildPage->Update();
	m_TimeScannerReferenceSystemPage->SetValue(m_CurrentTime);
	m_TimeScannerReferenceSystemPage->Update();

	GuiUpdate();

	// windows 7 64 bit / nvidia graphic card patch: otherwise scan time will not work
	// as expected and slices will be black while scanning until mouse dowin
	// is performed in view
	// </patch>
	if (m_Wizard->GetCurrentPage()==m_LoadPage)
	{
		m_LoadPage->UpdateActor();
		m_LoadPage->GetRWI()->CameraUpdate();
	}
	else if (m_Wizard->GetCurrentPage()==m_CropPage)
	{
		m_CropPage->UpdateActor();
		m_CropPage->GetRWI()->CameraUpdate();
	}
	else if (m_Wizard->GetCurrentPage()==m_BuildPage)
	{
		m_BuildPage->UpdateActor();
		m_BuildPage->GetRWI()->CameraUpdate();
	}
	else if (m_Wizard->GetCurrentPage()==m_ReferenceSystemPage)
	{
		m_ReferenceSystemPage->UpdateActor();
		m_ReferenceSystemPage->GetRWI()->CameraUpdate();
	}

	// </patch>

}

bool mafOpImporterDicomOffis::IsRotated( double dcmImageOrientationPatient[6] )
{
	// check if the dataset is rotated: => different from 1. 0. 0. 0. 1. 0.
	/* FROM David Clunie's note
	2.2.2 Orientation of DICOM images
	Another question that is frequently asked in comp.protocols.dicom is how to determine which side of an image is which 
	(e.g. left, right) and so on. The short answer is that for projection radiographs this is specified explicitly using 
	the Patient Orientation attribute, and for cross-sectional images it needs to be derived from the Image Orientation (Patient)
	direction cosines. In the standard these are explained as follows:

	"C.7.6.1.1.1 Patient Orientation. The Patient Orientation (0020,0020) relative to the image plane shall be specified by two values 
	that designate the anatomical direction of the positive row axis (left to right) and the positive column axis (top to bottom).
	The first entry is the direction of the rows, given by the direction of the last pixel in the first row from the first pixel
	in that row. The second entry is the direction of the columns, given by the direction of the last pixel in the first column from 
	the first pixel in that column. Anatomical direction shall be designated by the capital letters: A (anterior), P (posterior),
	R (right), L (left), H (head), F (foot). Each value of the orientation attribute shall contain at least one of these characters.
	If refinements in the orientation descriptions are to be specified, then they shall be designated by one or two additional letters
	in each value. Within each value, the letters shall be ordered with the principal orientation designated in the first character." 
	"C.7.6.2.1.1 Image Position And Image Orientation. The Image Position (0020,0032) specifies the x, y, and z coordinates of the upper 
	left hand corner of the image; it is the center of the first voxel transmitted. Image Orientation (0020,0037) specifies the direction 
	cosines of the first row and the first column with respect to the patient. These Attributes shall be provide as a pair. Row value for 
	the x, y, and z axes respectively followed by the Column value for the x, y, and z axes respectively. The direction of the axes is 
	defined fully by the patient's orientation. The x-axis is increasing to the left hand side of the patient. The y-axis is increasing 
	to the posterior side of the patient. The z-axis is increasing toward the head of the patient. The patient based coordinate system 
	is a right handed system, i.e. the vector cross product of a unit vector along the positive x-axis and a unit vector along the 
	positive y-axis is equal to a unit vector along the positive z-axis." */

	return !( \
		fabs(dcmImageOrientationPatient[0] - 1.0) < 0.0001 && \
		fabs(dcmImageOrientationPatient[4] - dcmImageOrientationPatient[0]) < 0.0001 &&\
		fabs(dcmImageOrientationPatient[1] - 0.0) < 0.0001 &&\
		fabs(dcmImageOrientationPatient[1] - dcmImageOrientationPatient[2]) < 0.0001 &&\
		fabs(dcmImageOrientationPatient[1] - dcmImageOrientationPatient[3]) < 0.0001 &&\
		fabs(dcmImageOrientationPatient[1] - dcmImageOrientationPatient[5]) < 0.0001 \
		);
}

void mafDicomSlice::SetVTKImageData( vtkImageData *data )
{
	vtkDEL(m_Data);
	m_Data = vtkImageData::New();
	m_Data->DeepCopy(data);
}

void mafDicomSlice::GetOrientation( vtkMatrix4x4 * matrix )
{
	assert(matrix);

	double orientation[6] = {0,0,0,0,0,0};

	this->GetDcmImageOrientationPatient(orientation);

	double Vx0 = orientation[0];
	double Vx1 = orientation[1];
	double Vx2 = orientation[2];

	double Vy0 = orientation[3];
	double Vy1 = orientation[4];
	double Vy2 = orientation[5];

	double Vz0 = Vx1 * Vy2 - Vx2 * Vy1;
	double Vz1 = Vx2 * Vy0 - Vx0 * Vy2;
	double Vz2 = Vx0 * Vy1 - Vx1 * Vy0;

	matrix->Identity();

	matrix->SetElement(0,0,Vx0);
	matrix->SetElement(1,0,Vx1);
	matrix->SetElement(2,0,Vx2);
	matrix->SetElement(3,0,0);

	matrix->SetElement(0,1,Vy0);
	matrix->SetElement(1,1,Vy1);			
	matrix->SetElement(2,1,Vy2);
	matrix->SetElement(3,1,0);

	matrix->SetElement(0,2,Vz0);
	matrix->SetElement(1,2,Vz1);
	matrix->SetElement(2,2,Vz2);
	matrix->SetElement(3,2,0);
	matrix->SetElement(3,3,1);
}

void mafOpImporterDicomOffis::UpdateReferenceSystemPageConnection()
{
	if(m_OutputType == mafGUIDicomSettings::ID_IMAGE)
	{
		m_BuildPage->SetNextPage(m_ReferenceSystemPage);
	}
	else
	{
		m_BuildPage->SetNext(NULL);
		m_ReferenceSystemPage->SetPrev(NULL);
	}
}

void mafOpImporterDicomOffis::ApplyReferenceSystem()
{
	// Apply reference system to dicom slices

	// Operate on patient position

	// This step will be executed after output build
	// So it apply only transform on the images mafMatrix!

	// create the transform matrix

	/**
	// 
	// xy->xy
	// no transform
	vtkTransform *xy2xy = vtkTransform::New();
	if(!m_SwapReferenceSystem)
	{
	//No transform
	}
	else
	{
	xy2xy->RotateY(180);
	xy2xy->RotateZ(90);
	}
	xy2xy->Update();

	//xy->xz
	vtkTransform *xy2xz = vtkTransform::New();

	if(!m_SwapReferenceSystem)
	{
	xy2xz->RotateX(90);
	}
	else
	{
	xy2xz->RotateX(-90);
	xy2xz->RotateZ(-90);
	}
	xy2xz->Update();

	//xy->yz
	vtkTransform *xy2yz = vtkTransform::New();
	if(!m_SwapReferenceSystem)
	{
	//xy2yz->RotateWXYZ(-90,0,1,1);
	xy2yz->RotateY(90);
	xy2yz->RotateZ(90);
	}
	else
	{
	xy2yz->RotateY(-90);
	}
	xy2yz->Update();
	*/

	for(int s = 0; s < m_SelectedSeriesSlicesList->size(); s++)
	{
		mafDicomSlice* currSlice =  (mafDicomSlice*)(m_SelectedSeriesSlicesList->Item(s)->GetData());

		// Get the reference system
		int refSys;
		if(!m_ApplyToAllReferenceSystem)
		{
			refSys = currSlice->GetReferenceSystem();
		}
		else
		{
			refSys = m_GlobalReferenceSystem;
		}

		// Get the swap variable
		int swap = currSlice->GetSwapReferenceSystem() || m_SwapAllReferenceSystem;

		mafVMEImage* image = mafVMEImage::SafeDownCast(m_ImagesGroup->GetChild(s));
		if((refSys != mafDicomSlice::ID_RS_XY || swap) && (image != NULL))
		{
			mafTimeStamp dcmTriggerTime = (mafTimeStamp)(currSlice->GetDcmTriggerTime());

			vtkTransform *dummyTransform = vtkTransform::New();
			switch(refSys)
			{
			case mafDicomSlice::ID_RS_XY:
				{
					// Flip Y with Z
					//dummyTransform->DeepCopy(xy2xy);
					if(swap)
					{
						dummyTransform->RotateY(180);
						dummyTransform->RotateZ(90);
					}

				}break;
			case mafDicomSlice::ID_RS_XZ:
				{
					// Flip Y with Z
					//dummyTransform->DeepCopy(xy2xz);
					if(!swap)
					{
						dummyTransform->RotateX(90);
					}
					else
					{
						dummyTransform->RotateX(-90);
						dummyTransform->RotateZ(-90);
					}
				}break;
			case mafDicomSlice::ID_RS_YZ:
				{
					// Flip X with Y
					//dummyTransform->DeepCopy(xy2yz);
					if(!swap)
					{
						//xy2yz->RotateWXYZ(-90,0,1,1);
						dummyTransform->RotateY(90);
						dummyTransform->RotateZ(90);
					}
					else
					{
						dummyTransform->RotateY(-90);
					}
				}break;
			}
			dummyTransform->Update();
			dummyTransform->PreMultiply();
			dummyTransform->Concatenate(image->GetMatrixPipe()->GetMatrix().GetVTKMatrix());
			dummyTransform->Update();

			mafSmartPointer<mafTransform> boxPose;
			boxPose->SetMatrix(dummyTransform->GetMatrix());
			boxPose->Update();

			image->SetAbsMatrix(boxPose->GetMatrix());
			image->Update();

			dummyTransform->Delete();
		}
	}
}