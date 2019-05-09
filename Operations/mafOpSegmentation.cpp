/*=========================================================================
 Program: ALBA
 Module: mafOpSegmentation
 Authors: Eleonora Mambrini - Matteo Giacomoni, Gianluigi Crimi, Alberto Losi, Nicola Vanella
 ==========================================================================
 Copyright (c) BIC-IOR 2018 (https://github.com/IOR-BIC)

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE. See the above copyright notice for more information.
 =========================================================================*/

#include "mafDefines.h" 

//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "mafOpSegmentation.h"

#include "mafAbsMatrixPipe.h"
#include "mafAction.h"
#include "mafDeviceButtonsPadMouseDialog.h"
#include "mafDeviceManager.h"
#include "mafGUI.h"
#include "mafGUIButton.h"
#include "mafGUIDialog.h"
#include "mafGUIDockManager.h"
#include "mafGUIFloatSlider.h"
#include "mafGUIHolder.h"
#include "mafGUILutHistogramSwatch.h"
#include "mafGUILutPreset.h"
#include "mafGUILutSlider.h"
#include "mafGUIPicButton.h"
#include "mafGUIRollOut.h"
#include "mafGUIValidator.h"
#include "mafInteractionFactory.h"
#include "mafInteractor.h"
#include "mafInteractorCameraMove.h"
#include "mafInteractorCompositorMouse.h"
#include "mafInteractorGenericMouse.h"
#include "mafInteractorPER.h"
#include "mafInteractorPERBrushFeedback.h"
#include "mafInteractorPERScalarInformation.h"
#include "mafInteractorPicker.h"
#include "mafInteractorSER.h"
#include "mafInteractorSegmentationPicker.h"
#include "mafMatrix.h"
#include "mafOpVolumeResample.h"
#include "mafPics.h"
#include "mafPipeVolumeOrthoSlice.h"
#include "mafPipeVolumeOrthoSlice.h"
#include "mafRWI.h"
#include "mafTagArray.h"
#include "mafTagItem.h"
#include "mafTransformFrame.h"
#include "mafVME.h"
#include "mafVMEImage.h"
#include "mafVMEItemVTK.h"
#include "mafVMESegmentationVolume.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"
#include "mafViewSliceSegmentation.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"

#include "itkImage.h"
#include "itkImageToVTKImageFilter.h"
#include "itkVTKImageToImageFilter.h"
#include "itkVotingBinaryHoleFillingImageFilter.h"
#include "itkVotingBinaryIterativeHoleFillingImageFilter.h"

#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkCamera.h"
#include "vtkConnectivityFilter.h"
#include "vtkDoubleArray.h"
#include "vtkImageCast.h"
#include "vtkImageClip.h"
#include "vtkLookupTable.h"
#include "vtkMAFBinaryImageFloodFill.h"
#include "vtkMAFImageFillHolesRemoveIslands.h"
#include "vtkMAFVolumeOrthoSlicer.h"
#include "vtkMAFVolumeToClosedSmoothSurface.h"
#include "vtkMath.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty2D.h"
#include "vtkRectilinearGrid.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"
#include "vtkStructuredPoints.h"
#include "vtkStructuredPointsWriter.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkUnstructuredGrid.h"
#include "vtkVolumeProperty.h"

#include "wx/busyinfo.h"
#include "wx/sizer.h"

#define SPACING_PERCENTAGE_BOUNDS 0.1

enum INIT_MODALITY_TYPE
{
  GLOBAL_INIT = 0,
  RANGE_INIT,
	LOAD_INIT,
};

typedef  itk::Image< float, 3> RealImage;
typedef  itk::Image< unsigned char, 3> UCharImage;

static int m_CurrentVolumeDimensions[3] = {0,0,0};          //<Used to load only volume with the specified dimensions
static double m_CurrentVolumeSpacing[3] = {0,0,0};          //<Used to load only volume with the specified spacing
static double m_CurrentVolumeBounds[6] = {0,0,0,0,0,0};           //<Used to load only volume with the specified bounds
static bool m_CurrentVolumeParametersInitialized = false;
static int m_InputVtkType = 0;

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpSegmentation);

//----------------------------------------------------------------------------
mafOpSegmentation::mafOpSegmentation(const wxString &label) : mafOp(label)
{
  m_OpType	= OPTYPE_OP;
  m_Canundo	= true;  

  m_Volume = NULL;
  m_VolumeDimensions[0] = m_VolumeDimensions[1] = m_VolumeDimensions[2] = 0;
  m_VolumeSpacing[0] = m_VolumeSpacing[1] = m_VolumeSpacing[2] = 0.0;
  m_VolumeBounds[0] = m_VolumeBounds[1] = m_VolumeBounds[2] = m_VolumeBounds[3] = m_VolumeBounds[4] = m_VolumeBounds[5] = 0.0;
  m_VolumeParametersInitialized = false;

	m_SliceIndexByPlane[0] = m_SliceIndexByPlane[1] = m_SliceIndexByPlane[2] = 1;
  m_SlicePlane =  XY;

  m_NumSliceSliderEvents = 0;
  m_SliceSlider = NULL;

	m_ShowLabels = true;
  m_CurrentPhase = INIT_SEGMENTATION;

  m_Dialog       = NULL;
  m_View         = NULL;      
  m_OkButton     = NULL;
  m_CancelButton = NULL;

  m_LutWidget = NULL;
  m_LutSlider = NULL;
  m_ColorLUT  = NULL;

  m_SegmentationColorLUT = NULL;
  m_ManualColorLUT = NULL;

  m_OldVolumeParent = NULL;
  m_OutputSurface      =NULL;

  m_SER = NULL;
  m_DeviceManager = NULL;
	
	m_AppendingOpGui = NULL;

	for (int i = 0; i < 2; i++)
	{
		m_SegmentationOperationsGui[i] = NULL;
	}

	m_SegmentationVolume = NULL;

  //////////////////////////////////////////////////////////////////////////
  //Manual initializations
	m_InitModalityRadioBox = NULL;
  m_ManualBrushShapeRadioBox = NULL;
  m_BrushShape = CIRCLE_BRUSH_SHAPE;
  m_BrushSize = 1;
  m_ManualRefinementRegionsSize = 1;

  m_ManualBrushSizeText = NULL;
  m_ManualBrushSizeSlider = NULL;

  m_ManualRefinementComboBox = NULL;
  m_ManualRefinementRegionSizeText = NULL;

  m_SegmentationPicker = NULL;
	 
  m_EditPER = NULL;
 
  m_IsDrawing = false;
  //////////////////////////////////////////////////////////////////////////
  //Automatic initializations
	m_AutomaticMouseThreshold = m_Threshold[0] = m_Threshold [1]= 0.0;
	m_CurrentRange = 0;
	m_IgnoreRangeSelEvent = false;
  m_RangesGuiList = NULL;
  m_AutomaticThresholdTextActor = NULL;
  m_AutomaticThresholdTextMapper = NULL;
  m_AutomaticSliceTextActor = NULL;
  m_AutomaticSliceTextMapper = NULL;
  m_AutomaticScalarTextActor = NULL;
  m_AutomaticScalarTextMapper = NULL;
  m_InitPER = NULL;
  m_InitModality = GLOBAL_INIT;

  //////////////////////////////////////////////////////////////////////////
  //Refinement initializations
  m_MajorityThreshold = 9;

  m_RealDrawnImage = NULL;
  m_LastMouseMovePointID = 0;

  m_OldAutomaticThreshold = MAXINT;
  m_OldAutomaticUpperThreshold = MAXINT;

  m_RemovePeninsulaRegions = FALSE;

  m_OLdWindowingLow = -1;
  m_OLdWindowingLow = -1;

  m_GlobalFloodFill = FALSE;
  m_FloodErease = FALSE;

  m_ManualSegmentationTools  = 0;
  m_ManualBucketActions = 0;
}
//----------------------------------------------------------------------------
mafOpSegmentation::~mafOpSegmentation()
{
}
//----------------------------------------------------------------------------
bool mafOpSegmentation::Accept(mafVME *node)
{
  return (node && node->IsA("mafVMEVolumeGray") );
}
//----------------------------------------------------------------------------
mafOp *mafOpSegmentation::Copy()   
{
  return (new mafOpSegmentation(m_Label));
}
//----------------------------------------------------------------------------
void mafOpSegmentation::OpRun()   
{  
  //////////////////////////////////////////////////////////////////////////
  //Initialize of the volume matrix to the indentity
  m_Volume=mafVMEVolumeGray::SafeDownCast(m_Input);

  m_Volume->Update();
  m_Matrix = m_Volume->GetMatrixPipe()->GetMatrix();
  mafMatrix matrixNoTransf;
  matrixNoTransf.Identity();

  m_Volume->SetAbsMatrix(matrixNoTransf);
  m_Volume->GetOutput()->Update();
  //////////////////////////////////////////////////////////////////////////

  // interface:
	InitVolumeDimensions();

  CreateOpDialog();
  InitializeView();

  m_OldVolumeParent = m_Volume->GetParent();
  m_Volume->ReparentTo(m_Volume->GetRoot());

  m_View->VmeAdd(m_Volume);
  
  Init();

  int result = m_Dialog->ShowModal() == wxID_OK ? OP_RUN_OK : OP_RUN_CANCEL;

  DeleteOpDialog();
  m_Volume->SetMatrix(m_Matrix);
  m_Volume->GetOutput()->Update();
  OpStop(result);
}
//----------------------------------------------------------------------------
void mafOpSegmentation::OpDo()
{
	//GENERATIN SURFACE OUTPUT
  wxBusyCursor wait_cursor;
  wxBusyInfo wait(_("Wait! Generating Surface Output"));
	  
  vtkMAFSmartPointer<vtkMAFVolumeToClosedSmoothSurface> volToSurface;
  volToSurface->SetInput(m_SegmentationVolume->GetOutput()->GetVTKData());
  volToSurface->SetContourValue(127.5);
  volToSurface->Update();
  
  vtkPolyData *surface;
  surface=volToSurface->GetOutput();

  //Generating Surface VME
  mafNEW(m_OutputSurface);
  m_OutputSurface->SetName(wxString::Format("Segmentation Surface (%s)",m_Volume->GetName()).c_str());
  m_OutputSurface->SetData(surface,mafVMEVolumeGray::SafeDownCast(m_Input)->GetTimeStamp());
  m_OutputSurface->ReparentTo(m_Input);
  m_OutputSurface->Modified();
	m_OutputSurface->Update();
	vtkDEL(surface);
	mafDEL(m_OutputSurface);

  mafOp::OpDo();
}
//----------------------------------------------------------------------------
void mafOpSegmentation::OpUndo()
{
  if (m_SegmentationVolume)
  {
    m_SegmentationVolume->ReparentTo(NULL);
    mafDEL(m_SegmentationVolume);
  }
  if (m_OutputSurface)
  {
    m_OutputSurface->ReparentTo(NULL);
    mafDEL(m_OutputSurface);
  }

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafOpSegmentation::OpStop(int result)
{
  // Restore old windowing
  m_ColorLUT->SetTableRange(m_OLdWindowingLow,m_OLdWindowingHi);

  //remove vme now on cancel on ok vme will be removed by opdo method
  if (result == OP_RUN_CANCEL)
  {
    if (m_SegmentationVolume)
    {
			m_SegmentationVolume->ReparentTo(NULL);
      mafDEL(m_SegmentationVolume);
    }
  }

  ResetRefinementUndoList();
  ResetRefinementRedoList();

  mafEventMacro(mafEvent(this,result));
}

// INITIALIZATION ////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
void mafOpSegmentation::InitializeView()
{
	m_View->SetTextureInterpolate(false);
	m_View->SetTrilinearInterpolation(false);

	// setting Visual pipes
	m_View->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeOrthoSlice");
	m_View->PlugVisualPipe("mafVMEImage", "mafPipeImage3D");
	m_View->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice");
}
//----------------------------------------------------------------------------
void mafOpSegmentation::Init()
{
	InitSegmentationVolume();
	InitializeInteractors();
	InitVolumeSpacing();
	UpdateWindowing();
	UpdateSliderValidator();
	InitRanges();

	m_Helper.SetVolumes(m_Volume, m_SegmentationVolume);
	m_View->VmeShow(m_Volume, true);
	m_View->VmeSegmentationShow(m_SegmentationVolume, true);

	OnSelectSlicePlane();
	OnChangeThresholdType();
}
//----------------------------------------------------------------------------
void mafOpSegmentation::InitSegmentationVolume()
{
	mafNEW(m_SegmentationVolume);

	m_SegmentationVolume->SetName(wxString::Format("Segmentation Output (%s)", m_Volume->GetName()).c_str());
	lutPreset(4, m_SegmentationVolume->GetMaterial()->m_ColorLut);
	/*
	m_SegmentationVolume->GetMaterial()->m_ColorLut->SetTableRange(0, 255);
	m_SegmentationVolume->GetMaterial()->UpdateFromTables();
	*/
	vtkLookupTable *lut = m_SegmentationVolume->GetMaterial()->m_ColorLut;
	if (lut)
	{
		lut->SetNumberOfTableValues(2);
		lut->SetTableValue(0, 0.0, 0.0, 0.0, 0.0);
		lut->SetTableValue(1, 0.0, 0.0, 0.9, 1.0);
		lut->SetTableRange(0, 255);
		m_SegmentationVolume->GetMaterial()->UpdateFromTables();
	}

	vtkDataSet *volData = m_Volume->GetOutput()->GetVTKData();
	vtkDataSet *segData = volData->NewInstance();
	segData->DeepCopy(volData);

	//Workaround we add an epsilon to ensure segmentation volume is always visible
	//mafViewSegmentation slices adds the same amount to the slice to compensate
	if (vtkImageData::SafeDownCast(volData))
	{
		double newOrigin[3];
		((vtkImageData *)volData)->GetOrigin(newOrigin);
		newOrigin[0] += 0.0001;
		newOrigin[1] -= 0.0001;
		newOrigin[2] -= 0.0001;
		((vtkImageData *)segData)->SetOrigin(newOrigin);
	}
	else //Rectilinear Grid
	{
		int i;
		vtkRectilinearGrid * volRG = (vtkRectilinearGrid *)volData;
		vtkRectilinearGrid * segRG = (vtkRectilinearGrid *)segData;
		vtkDataArray * xCoord = volRG->GetXCoordinates();
		vtkDataArray * yCoord = volRG->GetYCoordinates();
		vtkDataArray * zCoord = volRG->GetZCoordinates();
		vtkDataArray *newXCoord, *newYCoord, *newZCoord;
		newXCoord = xCoord->NewInstance();
		newYCoord = yCoord->NewInstance();
		newZCoord = zCoord->NewInstance();
		vtkIdType xNTuples = xCoord->GetNumberOfTuples();
		vtkIdType yNTuples = yCoord->GetNumberOfTuples();
		vtkIdType zNTuples = zCoord->GetNumberOfTuples();
		newXCoord->SetNumberOfTuples(xNTuples);
		newYCoord->SetNumberOfTuples(yNTuples);
		newZCoord->SetNumberOfTuples(zNTuples);
		for (i = 0; i < xNTuples; i++)
			newXCoord->SetTuple1(i, xCoord->GetTuple1(i) + 0.0001);
		for (i = 0; i < yNTuples; i++)
			newYCoord->SetTuple1(i, yCoord->GetTuple1(i) - 0.0001);
		for (i = 0; i < zNTuples; i++)
			newZCoord->SetTuple1(i, zCoord->GetTuple1(i) - 0.0001);
		segRG->SetXCoordinates(newXCoord);
		segRG->SetYCoordinates(newYCoord);
		segRG->SetZCoordinates(newZCoord);
	}

	vtkDataArray *volScalars = volData->GetPointData()->GetScalars();
	vtkUnsignedCharArray *segScalars;
	vtkNEW(segScalars);
	segScalars->SetNumberOfTuples(volScalars->GetNumberOfTuples());
	segData->GetPointData()->SetScalars(segScalars);
	m_SegmentationVolume->SetData(segData, m_Volume->GetTimeStamp());

	m_SegmentationVolume->ReparentTo(m_Volume);
	m_SegmentationVolume->Update();

	m_View->VmeAdd(m_SegmentationVolume);
}
//----------------------------------------------------------------------------
void mafOpSegmentation::InitializeInteractors()
{
	//Create the device manager
	mafNEW(m_DeviceManager);
	m_DeviceManager->SetListener(this);
	m_DeviceManager->SetName("DialogDeviceManager");
	m_DeviceManager->Initialize();

	//Create the static event router and connect it
	mafNEW(m_SER);
	m_SER->SetName("StaticEventRouter");
	m_SER->SetListener(this);

	//Create a Mouse device
	mafPlugDevice<mafDeviceButtonsPadMouseDialog>("Mouse");
	m_DialogMouse = (mafDeviceButtonsPadMouseDialog *)m_DeviceManager->AddDevice("mafDeviceButtonsPadMouseDialog", false); // add as persistent device
	assert(m_DialogMouse);
	m_DialogMouse->SetName("DialogMouse");

	//Define the action for pointing and manipulating
	mafAction *pntAction = m_SER->AddAction("pntAction", -10);

	//create the positional event router
	mafNEW(m_EditPER);
	m_EditPER->SetName("m_EditingPER");
	m_EditPER->SetListener(this);

	assert(m_View);
	m_EditPER->SetRenderer(m_View->GetFrontRenderer());

	mafNEW(m_SegmentationPicker);

	m_SegmentationPicker->SetRenderer(m_View->GetFrontRenderer());
	m_SegmentationPicker->SetListener(this);

	m_View->GetRWI()->SetMouse(m_DialogMouse);
	m_View->SetMouse(m_DialogMouse);
	m_OldBehavior = m_Volume->GetBehavior();
	m_DialogMouse->SetView(m_View);

	m_Volume->SetBehavior(m_SegmentationPicker);

	m_SER->AddAction("pntEditingAction");
	pntAction = m_SER->GetAction("pntEditingAction");
	m_EditPER->AddObserver(m_SegmentationPicker);

	m_SER->AddAction("pntEditingAction");
	pntAction = m_SER->GetAction("pntEditingAction");

	mafNEW(m_InitPER);
	pntAction = m_SER->AddAction("pntActionAutomatic", -10);
	pntAction->BindDevice(m_DialogMouse);
	pntAction->BindInteractor(m_InitPER);
	m_InitPER->SetListener(this);
	m_InitPER->SetRenderer(m_View->GetFrontRenderer());
}
//----------------------------------------------------------------------------
void mafOpSegmentation::InitVolumeDimensions()
{
	vtkDataSet *inputDataSet = vtkDataSet::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_Volume)->GetOutput()->GetVTKData());

	if (inputDataSet)
	{
		inputDataSet->Update();

		m_Volume->GetOutput()->GetVTKData()->GetBounds(m_VolumeBounds);

		if (vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(inputDataSet))
			sp->GetDimensions(m_VolumeDimensions);
		else if (vtkRectilinearGrid *rg = vtkRectilinearGrid::SafeDownCast(inputDataSet))
			rg->GetDimensions(m_VolumeDimensions);

		m_VolumeParametersInitialized = true;


		//Setting start slicing on the center of the volume
		for (int i = 0; i < 3; i++)
			m_SliceIndexByPlane[i] = m_VolumeDimensions[i] / 2;
	}

	// Set static variable for load segmentation step
	if (m_VolumeParametersInitialized)
	{
		for (int i = 0; i < 6; i++)
			m_CurrentVolumeBounds[i] = m_VolumeBounds[i];
		for (int i = 0; i < 3; i++)
			m_CurrentVolumeDimensions[i] = m_VolumeDimensions[i];
		for (int i = 0; i < 3; i++)
			m_CurrentVolumeSpacing[i] = m_VolumeSpacing[i];

		m_CurrentVolumeParametersInitialized = true;
	}
}
//----------------------------------------------------------------------------
void mafOpSegmentation::InitVolumeSpacing()
{
	vtkDataSet *vme_data = m_Volume->GetOutput()->GetVTKData();

	m_VolumeSpacing[0] = 0;
	m_VolumeSpacing[1] = 0;
	m_VolumeSpacing[2] = 0;

	if (vtkImageData *image = vtkImageData::SafeDownCast(vme_data))
	{
		image->GetSpacing(m_VolumeSpacing);
	}
	else if (vtkRectilinearGrid *rgrid = vtkRectilinearGrid::SafeDownCast(vme_data))
	{
		for (int xi = 1; xi < rgrid->GetXCoordinates()->GetNumberOfTuples(); xi++)
		{
			double spcx = rgrid->GetXCoordinates()->GetTuple1(xi) - rgrid->GetXCoordinates()->GetTuple1(xi - 1);
			if (m_VolumeSpacing[0] < spcx && spcx != 0.0)
				m_VolumeSpacing[0] = spcx;
		}

		for (int yi = 1; yi < rgrid->GetYCoordinates()->GetNumberOfTuples(); yi++)
		{
			double spcy = rgrid->GetYCoordinates()->GetTuple1(yi) - rgrid->GetYCoordinates()->GetTuple1(yi - 1);
			if (m_VolumeSpacing[1] < spcy && spcy != 0.0)
				m_VolumeSpacing[1] = spcy;
		}

		for (int zi = 1; zi < rgrid->GetZCoordinates()->GetNumberOfTuples(); zi++)
		{
			double spcz = rgrid->GetZCoordinates()->GetTuple1(zi) - rgrid->GetZCoordinates()->GetTuple1(zi - 1);
			if (m_VolumeSpacing[2] < spcz && spcz != 0.0)
				m_VolumeSpacing[2] = spcz;
		}
	}
}
//----------------------------------------------------------------------------
void mafOpSegmentation::InitRanges()
{
	//Store the parameters
	AutomaticInfoRange range;
	range.m_StartSlice = 1;
	range.m_EndSlice = m_VolumeDimensions[2];
	range.m_Threshold[0] = m_Threshold[0];
	range.m_Threshold[1] = m_Threshold[1];

	m_RangesVector.push_back(range);

	m_RangesGuiList->Append(wxString::Format("[%d,%d] low:%.1f high:%.1f", range.m_StartSlice, range.m_EndSlice, m_Threshold[0], m_Threshold[1]));
	m_RangesGuiList->Select(0);
}

// DIALOG GUI
//----------------------------------------------------------------------------
void mafOpSegmentation::CreateOpDialog()
{
	//////////////////////////////////////////////////////////////////////////
	// Dialog

	// ...................................
	// .                 .               .
	// .                 .               .
	// .     View        .   GuiDialog   .
	// .                 .               .
	// ...................               .
	// .   LutSlider     .               .
	// ...................................

	// ...................................
	// . Ok Canc . SnipLabs   . Progress .
	// ...................................

	wxBusyCursor wait;
	wxPoint defPos = wxDefaultPosition;
	wxSize defSize = wxDefaultSize;

	m_Dialog = new mafGUIDialog(m_Label, mafCLOSEWINDOW);
	m_Dialog->SetListener(this);

	m_GuiDialog = new mafGUI(this);
	m_GuiDialog->Reparent(m_Dialog);

	//Change default frame to our dialog
	wxWindow* oldFrame = mafGetFrame();

	int fw, fh;
	oldFrame->GetSize(&fw, &fh);
	int dialogSize[2] = { 850, fh };

	mafSetFrame(m_Dialog);

	//////////////////////////////////////////////////////////////////////////
	//Create rendering view   
	m_View = new mafViewSliceSegmentation("View Segmentation Slice");
	m_View->Create();
	m_View->GetGui();

	m_View->VmeAdd(m_Input->GetRoot()); //add Root

	mafSetFrame(oldFrame);
	int offset = 200;
	int viewSize[2] = { dialogSize[1] - offset, dialogSize[1] - offset }; // 650x650

	if (viewSize[0] < dialogSize[0] - 200)
		viewSize[0] = dialogSize[0] - 200;

	int labelOffset[2] = { 110, 15 };

	//////////////////////////////////////////////////////////////////////////
	// Label to indicate the threshold of the slice  
	vtkNEW(m_AutomaticThresholdTextMapper);
	m_AutomaticThresholdTextMapper->SetInput(_("Threshold  = "));
	m_AutomaticThresholdTextMapper->GetTextProperty()->SetColor(0.0, 1.0, 1.0);
	m_AutomaticThresholdTextMapper->GetTextProperty()->AntiAliasingOff();

	vtkNEW(m_AutomaticThresholdTextActor);
	m_AutomaticThresholdTextActor->SetMapper(m_AutomaticThresholdTextMapper);
	m_AutomaticThresholdTextActor->SetPosition(3, viewSize[1] - labelOffset[1]);
	m_AutomaticThresholdTextActor->GetProperty()->SetColor(1.0, 0.0, 0.0);

	m_View->GetFrontRenderer()->AddActor(m_AutomaticThresholdTextActor);

	//////////////////////////////////////////////////////////////////////////
	// Label to indicate the scalar value

	vtkNEW(m_AutomaticScalarTextMapper);
	m_AutomaticScalarTextMapper->SetInput(_("Scalar  = "));
	m_AutomaticScalarTextMapper->GetTextProperty()->SetColor(0.0, 1.0, 0.0);
	m_AutomaticScalarTextMapper->GetTextProperty()->AntiAliasingOff();

	vtkNEW(m_AutomaticScalarTextActor);
	m_AutomaticScalarTextActor->SetMapper(m_AutomaticScalarTextMapper);
	m_AutomaticScalarTextActor->SetPosition(viewSize[0] - labelOffset[0], viewSize[1] - labelOffset[1]);
	m_AutomaticScalarTextActor->GetProperty()->SetColor(0.0, 1.0, 0.0);

	m_View->GetFrontRenderer()->AddActor(m_AutomaticScalarTextActor);

	//////////////////////////////////////////////////////////////////////////
	// Label to indicate the current slice

	vtkNEW(m_AutomaticSliceTextMapper);
	wxString text = wxString::Format("Slice = %d of %d", m_SliceIndex, m_VolumeDimensions[m_SlicePlane]);

	m_AutomaticSliceTextMapper->SetInput(text);
	m_AutomaticSliceTextMapper->GetTextProperty()->SetColor(1.0, 1.0, 0.0);
	m_AutomaticSliceTextMapper->GetTextProperty()->AntiAliasingOff();

	vtkNEW(m_AutomaticSliceTextActor);
	m_AutomaticSliceTextActor->SetMapper(m_AutomaticSliceTextMapper);
	m_AutomaticSliceTextActor->SetPosition(viewSize[0] - labelOffset[0], 5);
	m_AutomaticSliceTextActor->GetProperty()->SetColor(1.0, 1.0, 0.0);

	m_View->GetFrontRenderer()->AddActor(m_AutomaticSliceTextActor);
	//////////////////////////////////////////////////////////////////////////

	//InitializeInteractors();

	// Display window      
	m_View->m_Rwi->SetSize(0, 0, viewSize[0], viewSize[1]);
	m_View->m_Rwi->Show(true);

	//////////////////////////////////////////////////////////////////////////
	// Lut Slider
	int lutSliderHeight = 24;

	m_LutSlider = new mafGUILutSlider(m_Dialog, -1, wxPoint(0, 0), wxSize(viewSize[0], lutSliderHeight));
	m_LutSlider->SetListener(this);
	m_LutSlider->SetSize(viewSize[0], lutSliderHeight);
	m_LutSlider->SetMinSize(wxSize(viewSize[0], lutSliderHeight));

	wxBoxSizer *view_lut_Sizer = new wxBoxSizer(wxVERTICAL);
	view_lut_Sizer->Add(m_View->m_Rwi->m_RwiBase, 1, wxEXPAND | wxALL, 5);
	view_lut_Sizer->Add(m_LutSlider, 0, wxLEFT, 5);

	m_LutSlider->Update();

	//////////////////////////////////////////////////////////////////////////

	mafVMEOutputVolume *volumeOutput = mafVMEOutputVolume::SafeDownCast(m_Volume->GetOutput());
	m_ColorLUT = volumeOutput->GetMaterial()->m_ColorLut;
	double data[2];
	m_ColorLUT->GetTableRange(data);
	m_OLdWindowingLow = data[0];
	m_OLdWindowingHi = data[1];

	/////////////////////////////////////////////////////
	wxBoxSizer *view_gui_Sizer = new wxBoxSizer(wxHORIZONTAL);
	view_gui_Sizer->Add(view_lut_Sizer, 0, wxRIGHT);
	view_gui_Sizer->Add(m_GuiDialog, 0, wxRIGHT);

	//////////////////////////////////////////////////////////////////////////
	// SNIPPETTS LAB Sizer

	// OK CANCEL BTN + PROGRESS BAR Sizer
	wxBoxSizer *okCancel_progress_Sizer = new wxBoxSizer(wxHORIZONTAL);
	okCancel_progress_Sizer->SetMinSize(dialogSize[0], 20);

	m_OkButton = new mafGUIButton(m_Dialog, ID_OK, _("Ok"), defPos);
	m_OkButton->SetListener(this);
	m_OkButton->SetValidator(mafGUIValidator(this, ID_OK, m_OkButton));
	m_OkButton->Enable(false);

	m_CancelButton = new mafGUIButton(m_Dialog, ID_CANCEL, _("Cancel"), defPos);
	m_CancelButton->SetListener(this);
	m_CancelButton->SetValidator(mafGUIValidator(this, ID_CANCEL, m_CancelButton));

	m_SnippetsLabel = new wxStaticText(m_Dialog, -1, "", defPos, wxSize(dialogSize[0] - 166, 20), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
	m_SnippetsLabel->SetBackgroundColour(wxColor(254, 221, 134));
	wxFont boldFont = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
	boldFont.SetWeight(wxBOLD);
	m_SnippetsLabel->SetFont(boldFont);
	m_SnippetsLabel->SetLabel(_(" 'Left Click + Ctrl' to select lower threshold. 'Left Click + Alt' to select upper threshold"));

	m_ProgressBar = new wxGauge(m_Dialog, -1, 100, defPos, wxSize(200, 10));

	okCancel_progress_Sizer->Add(m_OkButton, 0, wxEXPAND | wxALL, 5);
	okCancel_progress_Sizer->Add(m_CancelButton, 0, wxEXPAND | wxALL, 5);
	okCancel_progress_Sizer->Add(m_SnippetsLabel, 0, wxEXPAND | wxALL, 5);
	okCancel_progress_Sizer->Add(m_ProgressBar, 0, wxEXPAND | wxALL, 5);

	//////////////////////////////////////////////////////////////////////////
	// Dialog

	m_Dialog->Add(view_gui_Sizer);
	m_Dialog->Add(okCancel_progress_Sizer);

	//////////////////////////////////////////////////////////////////////////
	// GuiDialog

	int guiDialogSize[2] = { 300, viewSize[1] };

	m_GuiDialog->SetMinSize(wxSize(guiDialogSize[0], guiDialogSize[1]));
	m_GuiDialog->SetSize(wxSize(guiDialogSize[0], guiDialogSize[1]));

	m_GuiDialog->Divider();

	//m_GuiDialog->Bool(ID_ENABLE_TRILINEAR_INTERPOLATION,"Interpolation",&m_TrilinearInterpolationOn,1,"Enable/Disable tri-linear interpolation on slices");

	CreateSliceNavigationGui();
	CreateInitSegmentationGui();
	CreateEditSegmentationGui();

	//////////////////////////////////////////////////////////////////////////
	// Segmentation Operations Phase Buttons

	m_GuiDialog->TwoButtons(ID_BUTTON_PREV, ID_BUTTON_NEXT, _("Init"), _("Edit"), wxEXPAND | wxALL);
	m_GuiDialog->Enable(ID_BUTTON_PREV, false);
	m_GuiDialog->Enable(ID_BUTTON_NEXT, true);
	m_GuiDialog->Divider(1);

	// Add Operation Phase Gui
	m_AppendingOpGui = new mafGUI(NULL);
	m_AppendingOpGui->AddGui(m_SegmentationOperationsGui[INIT_SEGMENTATION]);

	m_GuiDialog->AddGui(m_AppendingOpGui);

	//////////////////////////////////////////////////////////////////////////
	m_GuiDialog->FitGui();

	int x_pos = (fw - dialogSize[0]) / 2;
	int y_pos = (fh - dialogSize[1]) / 2;

	m_Dialog->SetSize(x_pos, y_pos, dialogSize[0], dialogSize[1]);

	m_ProgressBar->Show(true);
	m_GuiDialog->Update();

	UpdateWindowing();
}
//----------------------------------------------------------------------------
void mafOpSegmentation::CreateSliceNavigationGui()
{
	if (!m_GuiDialog)
		return;

	m_GuiDialog->Label("View", true);

	////////////////////////////////////////////////////////////////////////
	// SLICE SLIDER

	wxPoint defPos = wxDefaultPosition;

	int id_slider = m_GuiDialog->GetWidgetId(ID_SLICE_SLIDER);
	int id_text = m_GuiDialog->GetWidgetId(ID_SLICE_TEXT);

	// Slices ----------
	wxTextCtrl *slice_text = NULL;
	wxStaticText *slice_lab = NULL;
	wxStaticText *slice_foo = NULL;

	slice_lab = new wxStaticText(m_GuiDialog, -1, "Slice: ", defPos, wxSize(55, 18), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
	slice_foo = new wxStaticText(m_GuiDialog, -1, "", defPos, wxSize(8, 18), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
	slice_text = new wxTextCtrl(m_GuiDialog, ID_SLICE_SLIDER, "", defPos, wxSize(30, 18), wxTE_LEFT/*|wxTE_READONLY*/);
	slice_text->Enable(true);
	m_SliceText = slice_text;

	m_SliceIndex = 1;
	m_SliceSlider = new wxSlider(m_GuiDialog, ID_SLICE_SLIDER, m_SliceIndex, 1, m_VolumeDimensions[2], defPos, wxSize(64, 18));
	mafGUIButton *b_incr_slice = new mafGUIButton(m_GuiDialog, ID_SLICE_NEXT, ">", defPos, wxSize(18, 18));
	mafGUIButton *b_decr_slice = new mafGUIButton(m_GuiDialog, ID_SLICE_PREV, "<", defPos, wxSize(18, 18));

	slice_text->SetValidator(mafGUIValidator(this, ID_SLICE_TEXT, slice_text, &m_SliceIndex, m_SliceSlider, 1, m_VolumeDimensions[2]));
	m_SliceSlider->SetValidator(mafGUIValidator(this, ID_SLICE_SLIDER, m_SliceSlider, &m_SliceIndex, slice_text));

	b_incr_slice->SetValidator(mafGUIValidator(this, ID_SLICE_NEXT, b_incr_slice));
	b_decr_slice->SetValidator(mafGUIValidator(this, ID_SLICE_PREV, b_decr_slice));

	wxBoxSizer *slice_sizer = new wxBoxSizer(wxHORIZONTAL);
	slice_sizer->Add(slice_lab, 0, wxRIGHT);
	slice_sizer->Add(slice_text, 0);
	slice_sizer->Add(slice_foo, 0);
	slice_sizer->Add(b_decr_slice, 0);
	slice_sizer->Add(m_SliceSlider, 0);
	slice_sizer->Add(b_incr_slice, 0);

	m_GuiDialog->Add(slice_sizer, 0, wxALL, 5);

	//////////////////////////////////////////////////////////////////////////
	// SLICE PLANES

	wxString planes[3]{ "YZ","XZ","XY" };

	m_GuiDialog->Radio(ID_SLICE_PLANE, "Plane: ", &m_SlicePlane, 3, planes, 3, "Slice Plane");

	//////////////////////////////////////////////////////////////////////////

	m_GuiDialog->Bool(ID_SHOW_LABELS, "Show labels", &m_ShowLabels, 1);
	m_GuiDialog->Divider(1);

	m_SliceSlider->SetValue(1);
	m_SliceSlider->Update();
}
//----------------------------------------------------------------------------
void mafOpSegmentation::CreateInitSegmentationGui()
{
	mafGUI *currentGui = new mafGUI(this);

	////////////////////////////////////////////////////////////////////////
	// THRESHOLD TYPE Selection

	currentGui->Label("Threshold type", true);

	wxString choices[3] = { _("Global"),_("Range"), _("Load") };
	int w_id = currentGui->GetWidgetId(ID_INIT_MODALITY);

	m_InitModalityRadioBox = new wxRadioBox(currentGui, w_id, "", wxDefaultPosition, wxSize(170, -1), 3, choices, 3);
	m_InitModalityRadioBox->SetValidator(mafGUIValidator(currentGui, w_id, m_InitModalityRadioBox, &m_InitModality));

	wxBoxSizer *thresholdTypeLabSizer = new wxBoxSizer(wxHORIZONTAL);
	thresholdTypeLabSizer->Add(m_InitModalityRadioBox, 0, wxEXPAND, 2);

	currentGui->Add(thresholdTypeLabSizer);
	currentGui->Divider(1);

	// THRESHOLD GLOBAL

	currentGui->Label(_("Threshold"), true);

	std::vector<const char*> increaseLabels = { "+","+","+" };
	std::vector<const char*> decreaseLabels = { "-","-","-" };

	double sr[2];
	m_Volume->GetOutput()->GetVTKData()->GetScalarRange(sr);

	// Threshold 
	//[ + ] [ + ] [ + ]
	//[min][range][max]
	//[ - ] [ - ] [ - ] 
	m_Threshold[0] = ceil((sr[0] + sr[1]) / 2);
	m_Threshold[1] = sr[1];
	m_ThresholdSlider = new mafGUILutSlider(currentGui, -1, wxPoint(0, 0), wxSize(300, 24));
	m_ThresholdSlider->SetListener(this);
	m_ThresholdSlider->SetText(1, "Threshold");
	m_ThresholdSlider->SetRange(sr);
	m_ThresholdSlider->SetSubRange(m_Threshold);

	std::vector<int> increaseTrIDs = { ID_AUTO_INC_MIN_THRESHOLD, ID_AUTO_INC_MIDDLE_THRESHOLD,ID_AUTO_INC_MAX_THRESHOLD };
	currentGui->MultipleButtons(3, 3, increaseTrIDs, increaseLabels);

	currentGui->Add(m_ThresholdSlider);

	std::vector<int> decreaseTrIDs = { ID_AUTO_DEC_MIN_THRESHOLD, ID_AUTO_DEC_MIDDLE_THRESHOLD, ID_AUTO_DEC_MAX_THRESHOLD };
	currentGui->MultipleButtons(3, 3, decreaseTrIDs, decreaseLabels);
	currentGui->Divider(1);

	// THRESHOLD RANGES

	currentGui->Label(_("Slice range settings:"), true);

	m_RangesGuiList = currentGui->ListBox(ID_RANGE_SELECTION, "");
	currentGui->TwoButtons(ID_SPLIT_RANGE, ID_REMOVE_RANGE, ("Split"), _("Remove"));
	currentGui->Divider(1);

	// LOAD SEGMENTATION

	currentGui->Label(&m_VolumeName);
	currentGui->Button(ID_LOAD, "Load");
	currentGui->Divider(1);

	m_SegmentationOperationsGui[INIT_SEGMENTATION] = currentGui;
}
//----------------------------------------------------------------------------
void mafOpSegmentation::CreateEditSegmentationGui()
{
	mafGUI *currentGui = new mafGUI(this);

	//////////////////////////////////////////////////////////////////////////
	// Tool Selection

	currentGui->Label("Tools", true);

	std::vector<const char*> toolLabels = { "Brush", "Fill" };
	std::vector<const char*> toolImageNames = { "TOOL_PEN" , "TOOL_FILL" };
	std::vector<int> toolIds = { ID_MANUAL_TOOLS_BRUSH, ID_MANUAL_TOOLS_FILL };

	currentGui->MultipleImageButtons(2, 2, toolIds, toolLabels, toolImageNames, 0);
	currentGui->Divider(1);

	//////////////////////////////////////////////////////////////////////////
	// Brush Editing options

	// BRUSH SHAPE
	wxString shapes[2] = { "circle" , "square" };

	int w_id = currentGui->GetWidgetId(ID_MANUAL_BRUSH_SHAPE);
	wxStaticText *brushShapeLab = new wxStaticText(currentGui, w_id, "Shape");

	m_ManualBrushShapeRadioBox = new wxRadioBox(currentGui, w_id, "", wxDefaultPosition, wxSize(130, -1), 2, shapes, 2);
	m_ManualBrushShapeRadioBox->SetValidator(mafGUIValidator(currentGui, w_id, m_ManualBrushShapeRadioBox, &m_BrushShape));

	wxBoxSizer *brushShapesSizer = new wxBoxSizer(wxHORIZONTAL);
	brushShapesSizer->Add(brushShapeLab, 0, wxRIGHT, 5);
	brushShapesSizer->Add(m_ManualBrushShapeRadioBox, 0, wxRIGHT, 2);

	// BRUSH SIZE
	m_BrushSize = 1;

	wxStaticText *brushSizeLab = NULL;
	int id = currentGui->GetWidgetId(ID_MANUAL_BRUSH_SIZE);

	brushSizeLab = new wxStaticText(currentGui, id, "Size (unit)", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE);

	m_ManualBrushSizeText = new wxTextCtrl(currentGui, id, "", wxDefaultPosition, wxSize(40, 18));
	m_ManualBrushSizeSlider = new mafGUIFloatSlider(currentGui, id, m_BrushSize, 1, int(min(m_VolumeDimensions[0], m_VolumeDimensions[1]) / 2), wxDefaultPosition, wxDefaultSize);

	m_ManualBrushSizeText->SetValidator(mafGUIValidator(currentGui, id, m_ManualBrushSizeText, &m_BrushSize, m_ManualBrushSizeSlider, 1, int(min(m_VolumeDimensions[0], m_VolumeDimensions[1]) / 2)));
	m_ManualBrushSizeSlider->SetValidator(mafGUIValidator(currentGui, id, m_ManualBrushSizeSlider, &m_BrushSize, m_ManualBrushSizeText));

	m_BrushSize = 1;
	m_ManualBrushSizeSlider->SetValue(m_BrushSize);
	m_ManualBrushSizeText->SetValue("1");
	m_ManualBrushSizeSlider->SetNumberOfSteps(int(min(m_VolumeDimensions[0], m_VolumeDimensions[1]) / 4) - 2);
	m_ManualBrushSizeSlider->SetRange(1, int(min(m_VolumeDimensions[0], m_VolumeDimensions[1]) / 4), 1);
	m_ManualBrushSizeSlider->SetMax(int(min(m_VolumeDimensions[0], m_VolumeDimensions[1]) / 4));
	m_ManualBrushSizeSlider->SetMin(1);
	m_ManualBrushSizeSlider->Update();
	m_ManualBrushSizeText->Update();

	wxBoxSizer *brushSizeSizer = new wxBoxSizer(wxHORIZONTAL);
	brushSizeSizer->Add(brushSizeLab, 0, wxRIGHT, 5);
	brushSizeSizer->Add(m_ManualBrushSizeText, 0);
	brushSizeSizer->Add(m_ManualBrushSizeSlider, 0);

	m_BrushEditingSizer = new wxStaticBoxSizer(wxVERTICAL, currentGui, "Brush Options");
	m_BrushEditingSizer->Add(brushShapesSizer, 0, wxALL, 1);
	m_BrushEditingSizer->Add(brushSizeSizer, 0, wxALL, 1);

	//////////////////////////////////////////////////////////////////////////
	// Fill Editing options
	m_BucketEditingSizer = new wxStaticBoxSizer(wxVERTICAL, currentGui, "Bucket Options");

	w_id = currentGui->GetWidgetId(ID_MANUAL_BUCKET_GLOBAL);
	wxCheckBox *globalCheck = new wxCheckBox(currentGui, w_id, "Iterative");
	globalCheck->SetValidator(mafGUIValidator(currentGui, w_id, globalCheck, &m_GlobalFloodFill));

	m_ManualRangeSlider = new mafGUILutSlider(currentGui, -1, wxPoint(0, 0), wxSize(195, 24));
	m_ManualRangeSlider->SetListener(this);
	m_ManualRangeSlider->SetText(1, "Slices");
	m_ManualRangeSlider->SetRange(1, m_VolumeDimensions[2]);
	m_ManualRangeSlider->SetSubRange(1, m_VolumeDimensions[2]);

	m_BucketEditingSizer->Add(globalCheck, 0, wxALL, 1);
	m_BucketEditingSizer->Add(m_ManualRangeSlider, 0, wxALL, 1);

	//currentGui->Add(manualToolsVSizer, 0, wxALL, 1);
	currentGui->Add(m_BrushEditingSizer, wxALIGN_CENTER_HORIZONTAL);
	currentGui->Add(m_BucketEditingSizer, wxALIGN_CENTER_HORIZONTAL);
	/*currentGui->Bool(-1,"Global",&m_GlobalFloodFill,1,"");*/
	currentGui->TwoButtons(ID_MANUAL_UNDO, ID_MANUAL_REDO, "Undo", "Redo");

	EnableSizerContent(m_BucketEditingSizer, false);
	EnableSizerContent(m_BrushEditingSizer, true);

	//////////////////////////////////////////////////////////////////////////
	// Action: remove islands OR fill holes.
	/*
	m_RefinementSegmentationAction = ID_REFINEMENT_ISLANDS_REMOVE;
	wxString operations[2];
	operations[ID_REFINEMENT_ISLANDS_REMOVE] = wxString("Remove Islands");
	operations[ID_REFINEMENT_HOLES_FILL] = wxString("Fill Holes");

	currentGui->Combo(ID_REFINEMENT_ACTION, "Action", &m_RefinementSegmentationAction, 2, operations);

	// Size of islands/holes to be taken into consideration
	m_RefinementRegionsSize = 1;
	//currentGui->Integer(ID_REFINEMENT_REGIONS_SIZE, mafString("Size"), &m_RefinementRegionsSize, 0, MAXINT, mafString("Max size of islands/holes to be taken into consideration"));

	int stepsNumber = 10;
	w_id = currentGui->GetWidgetId(ID_MANUAL_REFINEMENT_REGIONS_SIZE);

	wxStaticText *sizeText = new wxStaticText(currentGui, w_id, "Size: ");

	wxTextCtrl *refinementRegionSizeText = new wxTextCtrl(currentGui, w_id, "", wxDefaultPosition, wxSize(36, 18), wxSUNKEN_BORDER, wxDefaultValidator, "Size:");
	wxSlider *sli = new wxSlider(currentGui, w_id, 1, 1, stepsNumber, wxDefaultPosition, wxSize(120, 18));

	refinementRegionSizeText->SetValidator(mafGUIValidator(currentGui, w_id, refinementRegionSizeText, &m_RefinementRegionsSize, sli, 1, stepsNumber)); //- if uncommented, remove also wxTE_READONLY from the text (in both places)
	sli->SetValidator(mafGUIValidator(currentGui, w_id, sli, &m_RefinementRegionsSize, refinementRegionSizeText));

	wxBoxSizer *regionSizeSizer = new wxBoxSizer(wxHORIZONTAL);
	regionSizeSizer->Add(sizeText, 0);
	regionSizeSizer->Add(refinementRegionSizeText, 0);
	regionSizeSizer->Add(sli, 0);

	currentGui->Add(regionSizeSizer);

	// Switch on/off the "apply on every slice" option
	m_RefinementEverySlice = 0;
	currentGui->Bool(ID_REFINEMENT_EVERY_SLICE, mafString("Global"), &m_RefinementEverySlice, 0, mafString("Apply refinement procedure on every slice"));

	m_RefinementIterative = 0;
	currentGui->Bool(ID_REFINEMENT_REMOVE_PENINSULA_REGIONS, mafString("Apply to peninsula regions"), &m_RemovePeninsulaRegions, 1, mafString("Apply refinement on peninsula regions"));
	currentGui->TwoButtons(ID_REFINEMENT_UNDO, ID_REFINEMENT_REDO, "Undo", "Redo");
	currentGui->Button(ID_REFINEMENT_APPLY, mafString("Apply"), "");
	currentGui->Divider(1);
	*/
	m_SegmentationOperationsGui[EDIT_SEGMENTATION] = currentGui;
}

//----------------------------------------------------------------------------
void mafOpSegmentation::EnableDisableGuiRange()
{
	bool rightModality = m_InitModality == RANGE_INIT;
	bool notBorder = !(m_SliceIndex == m_RangesVector[m_CurrentRange].m_StartSlice || m_SliceIndex == m_RangesVector[m_CurrentRange].m_EndSlice);

	m_SegmentationOperationsGui[INIT_SEGMENTATION]->Enable(ID_SPLIT_RANGE, rightModality && notBorder);
	m_SegmentationOperationsGui[INIT_SEGMENTATION]->Enable(ID_REMOVE_RANGE, rightModality && m_RangesVector.size() > 1);
	m_SegmentationOperationsGui[INIT_SEGMENTATION]->Enable(ID_RANGE_SELECTION, rightModality);
}
//----------------------------------------------------------------------------
void mafOpSegmentation::EnableSizerContent(wxSizer* sizer, bool enable)
{
	wxSizerItemList childList = sizer->GetChildren();
	for (int i = 0; i < childList.GetCount(); i++)
	{
		if (childList.Item(i)->GetData()->IsWindow())
		{
			childList.Item(i)->GetData()->GetWindow()->Enable(enable);
		}
		else if (childList.Item(i)->GetData()->IsSizer())
		{
			EnableSizerContent(childList.Item(i)->GetData()->GetSizer(), enable);
		}
	}
}

//----------------------------------------------------------------------------
void mafOpSegmentation::DeleteOpDialog()
{
	mafDEL(m_SegmentationPicker);

	m_Volume->SetBehavior(m_OldBehavior);
	m_Volume->Update();
	m_View->VmeShow(m_Volume, false);
	m_View->VmeRemove(m_Volume);
	//m_View->VmeSegmentationShow(m_SegmentationVolume, false);
	//m_View->VmeRemove(m_SegmentationVolume);

	//////////////////////////////////////////////////////////////////////////
	//Remove the threshold label
	if (m_AutomaticThresholdTextActor)
	{
		m_View->GetFrontRenderer()->RemoveActor(m_AutomaticThresholdTextActor);
	}
	vtkDEL(m_AutomaticThresholdTextActor);
	vtkDEL(m_AutomaticThresholdTextMapper);

	//////////////////////////////////////////////////////////////////////////
	//Remove the scalar label
	if (m_AutomaticScalarTextActor)
		m_View->GetFrontRenderer()->RemoveActor(m_AutomaticScalarTextActor);
	vtkDEL(m_AutomaticScalarTextActor);
	vtkDEL(m_AutomaticScalarTextMapper);

	//////////////////////////////////////////////////////////////////////////
	//Remove the slice label
	if (m_AutomaticSliceTextActor)
		m_View->GetFrontRenderer()->RemoveActor(m_AutomaticSliceTextActor);
	vtkDEL(m_AutomaticSliceTextActor);
	vtkDEL(m_AutomaticSliceTextMapper);

	//cppDEL(m_View);      
	cppDEL(m_OkButton);
	cppDEL(m_CancelButton);
	cppDEL(m_LutWidget);
	cppDEL(m_LutSlider);
	cppDEL(m_InitModalityRadioBox);
	cppDEL(m_ManualBrushShapeRadioBox);
	cppDEL(m_ManualBrushSizeSlider);

	cppDEL(m_SegmentationOperationsGui[INIT_SEGMENTATION]);
	cppDEL(m_SegmentationOperationsGui[EDIT_SEGMENTATION]);
	cppDEL(m_ProgressBar);
	cppDEL(m_SliceSlider);

	cppDEL(m_SnippetsLabel);

	cppDEL(m_AppendingOpGui);
	cppDEL(m_GuiDialog);
	cppDEL(m_Dialog);

	mafDEL(m_SegmentationPicker);

	if (m_DeviceManager)
		m_DeviceManager->Shutdown();

	mafDEL(m_DeviceManager);
	mafDEL(m_InitPER);
	mafDEL(m_EditPER);
	mafDEL(m_SER);
}

// UPDATE ////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void mafOpSegmentation::UpdateWindowing()
{
	mafVMEOutputVolume *volumeOutput = mafVMEOutputVolume::SafeDownCast(m_Volume->GetOutput());
	double sr[2], subR[2];
	volumeOutput->GetVTKData()->GetScalarRange(sr);
	mmaVolumeMaterial *currentSurfaceMaterial = volumeOutput->GetMaterial();
	currentSurfaceMaterial->m_ColorLut->GetTableRange(subR);

	volumeOutput->GetMaterial()->UpdateProp();
	m_LutSlider->SetRange(sr);
	m_LutSlider->SetSubRange(subR);
}
//----------------------------------------------------------------------------
void mafOpSegmentation::UpdateSliderValidator()
{
	int sliceMax = m_VolumeDimensions[m_SlicePlane];

	m_SliceSlider->SetRange(1, sliceMax);
	m_SliceText->SetValidator(mafGUIValidator(this, ID_SLICE_TEXT, m_SliceText, &m_SliceIndex, m_SliceSlider, 1, sliceMax));
	m_SliceIndex = m_SliceIndexByPlane[m_SlicePlane];
	m_SliceSlider->Update();
}
//----------------------------------------------------------------------------
void mafOpSegmentation::UpdateSlice()
{
	double posNew[3];
	UpdateThresholdLabel();
	UpdateSliceLabel();
	GetSliceOrigin(posNew);
	m_View->SetSlice(posNew);
	m_GuiDialog->Update();
	m_SliceIndexByPlane[m_SlicePlane] = m_SliceIndex;
}
//----------------------------------------------------------------------------
void mafOpSegmentation::UpdateSliceLabel()
{
	// Update slice text actor   
	wxString text = wxString::Format("Slice = %d of %d", m_SliceIndex, m_VolumeDimensions[m_SlicePlane]);
	m_AutomaticSliceTextMapper->SetInput(text);
}
//----------------------------------------------------------------------------
void mafOpSegmentation::UpdateThresholdLabel()
{
	if (m_CurrentPhase == INIT_SEGMENTATION && (m_InitModality != LOAD_INIT))
	{
		mafString text = wxString::Format("Threshold low:%.1f high:%.1f", m_Threshold[0], m_Threshold[1]);
		m_AutomaticThresholdTextMapper->SetInput(text.GetCStr());
	}
	else
		m_AutomaticThresholdTextMapper->SetInput("");
}
//----------------------------------------------------------------------------
void mafOpSegmentation::UpdateThresholdRealTimePreview()
{
	if (m_InitModality == GLOBAL_INIT || m_InitModality == RANGE_INIT)
		m_Helper.SliceThreshold(m_Threshold);

	m_View->CameraUpdate();
}

//----------------------------------------------------------------------------
bool mafOpSegmentation::ApplyRefinementFilter2(vtkStructuredPoints *inputImage, vtkStructuredPoints *outputImage)
{
  vtkMAFImageFillHolesRemoveIslands *filter = vtkMAFImageFillHolesRemoveIslands::New();
  filter->SetInput(inputImage);
  filter->SetEdgeSize(m_RefinementRegionsSize);
  filter->SetRemovePeninsulaRegions(m_RemovePeninsulaRegions == TRUE);
  if(m_RefinementSegmentationAction == ID_REFINEMENT_HOLES_FILL)
  {
    filter->SetAlgorithmToFillHoles();
  }
  else if(m_RefinementSegmentationAction == ID_REFINEMENT_ISLANDS_REMOVE)
  {
    filter->SetAlgorithmToRemoveIslands();
  }
  filter->Update();
  outputImage->DeepCopy(filter->GetOutput());
  outputImage->Update();
  filter->Delete();
  return true;
}
//----------------------------------------------------------------------------
int mafOpSegmentation::ApplyFloodFill(vtkImageData *inputImage, vtkImageData *outputImage, vtkIdType seed)
{
  vtkMAFBinaryImageFloodFill *filter = vtkMAFBinaryImageFloodFill::New();
  filter->SetInput(inputImage);
  filter->SetSeed(seed);
  
  filter->SetFillErase(m_FloodErease == TRUE);

  filter->Update();
  outputImage->DeepCopy(filter->GetOutput());
  outputImage->Update();

  vtkIdType next_seed_id = filter->GetCenter();

  filter->Delete();

  return next_seed_id;
}
//----------------------------------------------------------------------------
bool mafOpSegmentation::ApplyRefinementFilter(vtkStructuredPoints *inputImage, vtkStructuredPoints *outputImage)
{
  typedef itk::VotingBinaryHoleFillingImageFilter<UCharImage, UCharImage> ITKVotingHoleFillingFilter;
  ITKVotingHoleFillingFilter::Pointer holeFillingFilter = ITKVotingHoleFillingFilter::New();

  typedef itk::VotingBinaryIterativeHoleFillingImageFilter<UCharImage> ITKVotingIterativeHoleFillingFilter;
  ITKVotingIterativeHoleFillingFilter::Pointer iterativeHoleFillingFilter = ITKVotingIterativeHoleFillingFilter::New();

  vtkMAFSmartPointer<vtkStructuredPoints> refinedImage;


  vtkMAFSmartPointer<vtkImageCast> vtkImageToFloat;
  vtkImageToFloat->SetOutputScalarTypeToUnsignedChar();
  vtkImageToFloat->SetInput(inputImage);
  vtkImageToFloat->Modified();
  vtkImageToFloat->Update();
  //////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////
  //ITK pipeline to fill holes
  //////////////////////////////////////////////////////////////////////////
  typedef itk::VTKImageToImageFilter< UCharImage > ConvertervtkTOitk;
  ConvertervtkTOitk::Pointer vtkTOitk = ConvertervtkTOitk::New();
  vtkTOitk->SetInput( vtkImageToFloat->GetOutput() );
  vtkTOitk->Update();

  RealImage::SizeType indexRadius;
  indexRadius[0] = m_RefinementRegionsSize; // radius along x
  indexRadius[1] = m_RefinementRegionsSize; // radius along y
  indexRadius[2] = m_RefinementRegionsSize; // radius along z

  bool iterative = m_RefinementIterative && m_RefinementSegmentationAction==ID_REFINEMENT_ISLANDS_REMOVE;

  unsigned char scalarValue = 255;

  if(iterative)
  {
    iterativeHoleFillingFilter->SetInput( ((UCharImage*)vtkTOitk->GetOutput()) );
    iterativeHoleFillingFilter->SetRadius( indexRadius );
    if(m_RefinementSegmentationAction == ID_REFINEMENT_HOLES_FILL)
    {
      iterativeHoleFillingFilter->SetBackgroundValue( 0 );
      iterativeHoleFillingFilter->SetForegroundValue( scalarValue );
    }
    else
    {
      iterativeHoleFillingFilter->SetBackgroundValue( scalarValue );
      iterativeHoleFillingFilter->SetForegroundValue( 0 );
    }

  }
  else
  {
    holeFillingFilter->SetInput( ((UCharImage*)vtkTOitk->GetOutput()) );
    holeFillingFilter->SetRadius( indexRadius );
    holeFillingFilter->SetMajorityThreshold( m_MajorityThreshold ); 

    if(m_RefinementSegmentationAction == ID_REFINEMENT_HOLES_FILL)
    {
      holeFillingFilter->SetBackgroundValue( 0 );
      holeFillingFilter->SetForegroundValue( scalarValue );
    }
    else
    {
      holeFillingFilter->SetBackgroundValue( scalarValue );
      holeFillingFilter->SetForegroundValue( 0 );
    }
  }

  try
  {
    if(iterative)
      iterativeHoleFillingFilter->Update();
    else
      holeFillingFilter->Update();
  }
  catch ( itk::ExceptionObject &err )
  {
    std::cout << "ExceptionObject caught !" << std::endl; 
    std::cout << err << std::endl; 
  }

  typedef itk::ImageToVTKImageFilter< UCharImage > ConverteritkTOvtk;
  ConverteritkTOvtk::Pointer itkTOvtk = ConverteritkTOvtk::New();
  if(iterative)
    itkTOvtk->SetInput( iterativeHoleFillingFilter->GetOutput() ); 
  else
    itkTOvtk->SetInput( holeFillingFilter->GetOutput() );
  itkTOvtk->Update();
  //////////////////////////////////////////////////////////////////////////

  refinedImage = ((vtkStructuredPoints*)itkTOvtk->GetOutput());
  refinedImage->Update();

  outputImage->DeepCopy(refinedImage);
	
  return true;
}
//----------------------------------------------------------------------------

// EVENTS ////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void mafOpSegmentation::OnEvent(mafEventBase *maf_event)
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		if (e->GetSender() == m_SegmentationOperationsGui[INIT_SEGMENTATION])
			OnInitEvent(e);
		else if (e->GetSender() == m_SegmentationOperationsGui[EDIT_SEGMENTATION])
			OnEditSegmentationEvent(e);
		else if (e->GetSender() == m_SegmentationOperationsGui[EDIT_SEGMENTATION])
			OnRefinementSegmentationEvent(e);
		else if (e->GetSender() == m_SegmentationPicker && e->GetId() == mafInteractorSegmentationPicker::VME_ALT_PICKED)
		{
			//Picking during automatic segmentation
			if (m_CurrentPhase == INIT_SEGMENTATION)
			{
				m_Threshold[1] = m_AutomaticMouseThreshold;
				m_SegmentationOperationsGui[INIT_SEGMENTATION]->Update();
				m_Threshold[0] = min(m_Threshold[1], m_Threshold[0]);
				m_ThresholdSlider->SetSubRange(m_Threshold);
				OnThresholdUpate();
			}
			//Picking during manual segmentation
			else if (m_CurrentPhase == EDIT_SEGMENTATION)
			{
				if (m_ManualSegmentationTools == 1) // bucket
				{
					m_FloodErease = TRUE;
					OnEventFloodFill(e);
				}
				else // brush
				{
					StartDraw(e, true);
				}
			}
		}
		//SWITCH
		else switch (e->GetId())
		{
		case MOUSE_WHEEL:
		{
			if (m_CurrentPhase == EDIT_SEGMENTATION && m_ManualSegmentationTools == 0)
			{
				if (e->GetArg() < 0)
					m_BrushSize++;
				else
					m_BrushSize--;

				ApplyRealDrawnImage();
				m_Helper.DrawBrush((double *)e->GetPointer(), m_SlicePlane, m_BrushSize, m_BrushShape, m_BrushModality);
				m_View->CameraUpdate();
				m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Update();
				break;
			}
		}
		break;
		case MOUSE_MOVE:
		{
			if (e->GetSender() == m_InitPER)
			{
				m_AutomaticMouseThreshold = e->GetDouble();
				mafString text = wxString::Format("Scalar = %.3f", m_AutomaticMouseThreshold);
				m_AutomaticScalarTextMapper->SetInput(text.GetCStr());
			}
			else if (e->GetSender() == m_EditPER && m_ManualSegmentationTools == 0)
			{
					m_BrushModality = e->GetBool();
					ApplyRealDrawnImage();
					if(e->GetArg())
						m_Helper.DrawBrush((double *)e->GetPointer(), m_SlicePlane, m_BrushSize, m_BrushShape, m_BrushModality);
			}
			m_View->CameraUpdate();
		}
		break;
		case MOUSE_UP:
			if (m_IsDrawing)
			{
				CreateRealDrawnImage();
				m_IsDrawing = false;
			}
			break;
		case ID_BUTTON_NEXT:
			OnEditStep();
			break;
		case ID_BUTTON_PREV:
			OnPreviousStep();
			break;
		case ID_SLICE_NEXT:
			if (m_SliceIndex < m_SliceSlider->GetMax())
				m_SliceIndex++;
			OnUpdateSlice();
			break;
		case ID_SLICE_TEXT:
			if (m_SliceIndex > 1)
				m_SliceIndex--;
			OnUpdateSlice();
			break;
		case ID_SLICE_PREV:
			if (m_SliceIndex > 1)
				m_SliceIndex--;
			OnUpdateSlice();
			break;
		case ID_SLICE_SLIDER:
			OnUpdateSlice();
			break;
		case ID_SLICE_PLANE:
			OnSelectSlicePlane();
			break;
		case ID_SHOW_LABELS:
		{
			if (m_ShowLabels)
			{
				m_View->GetFrontRenderer()->AddActor(m_AutomaticThresholdTextActor);
				m_View->GetFrontRenderer()->AddActor(m_AutomaticScalarTextActor);
				m_View->GetFrontRenderer()->AddActor(m_AutomaticSliceTextActor);
			}
			else
			{
				m_View->GetFrontRenderer()->RemoveActor(m_AutomaticThresholdTextActor);
				m_View->GetFrontRenderer()->RemoveActor(m_AutomaticScalarTextActor);
				m_View->GetFrontRenderer()->RemoveActor(m_AutomaticSliceTextActor);
			}
			m_View->CameraUpdate();
		}
		break;
		case VME_PICKED:
		{
			//Picking during automatic segmentation
			if (m_CurrentPhase == INIT_SEGMENTATION)
			{
				m_Threshold[0] = m_AutomaticMouseThreshold;
				m_Threshold[1] = max(m_Threshold[1], m_Threshold[0]);
				m_SegmentationOperationsGui[INIT_SEGMENTATION]->Update();
				m_ThresholdSlider->SetSubRange(m_Threshold);
				UpdateThresholdLabel();
			}
			//Picking during manual segmentation
			if (m_CurrentPhase == EDIT_SEGMENTATION)
			{
				if (m_ManualSegmentationTools == 1) // bucket
				{
					m_FloodErease = FALSE;
					OnEventFloodFill(e);
				}
				else // brush
				{
					StartDraw(e, false);
				}
			}
			break;
		}
		case VME_PICKING:
			if (m_CurrentPhase == EDIT_SEGMENTATION && m_ManualSegmentationTools == 0)
			{
				m_Helper.DrawBrush((double *)e->GetPointer(), m_SlicePlane, m_BrushSize, m_BrushShape, m_BrushModality);
				m_View->CameraUpdate();
			}
			break;
		case ID_OK:
			m_Dialog->EndModal(wxID_OK);
			break;
		case ID_CANCEL:
			m_Dialog->EndModal(wxID_CANCEL);
			break;
		case ID_RANGE_MODIFIED:
		{
			//threshold slider
			if (e->GetSender() == m_ThresholdSlider)
			{
				OnThresholdUpate(e->GetId());
			}
			//Windowing
			else if (e->GetSender() == m_LutSlider)
			{
				double low, hi;
				m_LutSlider->GetSubRange(&low, &hi);
				m_ColorLUT->SetTableRange(low, hi);
				m_View->CameraUpdate();
			}
			else if (e->GetSender() == m_ManualRangeSlider)
			{
				double low, hi;
				m_ManualRangeSlider->GetSubRange(&low, &hi);

				if (m_SliceIndex > hi)
					m_SliceIndex = hi;

				if (m_SliceIndex < low)
					m_SliceIndex = low;

				OnUpdateSlice();
			}
			break;
		}
		case ID_LUT_CHOOSER:
		{
			double *sr;
			sr = m_ColorLUT->GetRange();
			m_LutSlider->SetSubRange((long)sr[0], (long)sr[1]);
			m_View->CameraUpdate();
		}
		break;
		case ID_ENABLE_TRILINEAR_INTERPOLATION:
			m_View->CameraUpdate();
			break;
		default:
			break;
		}
	}
}
//----------------------------------------------------------------------------
void mafOpSegmentation::OnEditStep()
{
	m_GuiDialog->Enable(ID_LOAD, false);

	UpdateThresholdLabel();

	m_CurrentPhase = EDIT_SEGMENTATION;
	
	m_AppendingOpGui->Remove(m_SegmentationOperationsGui[INIT_SEGMENTATION]);
	m_AppendingOpGui->AddGui(m_SegmentationOperationsGui[EDIT_SEGMENTATION]);
	m_AppendingOpGui->FitGui();

	m_GuiDialog->Enable(ID_BUTTON_PREV, true);
	m_GuiDialog->Enable(ID_BUTTON_NEXT, false);
	m_GuiDialog->FitGui();
	m_Dialog->Update();
	
	if (m_InitModality == GLOBAL_INIT)
		m_Helper.VolumeThreshold(m_Threshold);
	else if (m_InitModality == RANGE_INIT)
		m_Helper.VolumeThreshold(&m_RangesVector);

	//gui stuff
	//set brush cursor - enable drawing
	// brush size slider: min = 1; max = slice size
	m_SnippetsLabel->SetLabel(_(" 'Left Click' Draw. 'Left Click + Ctrl' Erase"));

	m_BrushSize = 1;
	m_ManualBrushSizeSlider->SetValue(m_BrushSize);
	m_ManualBrushSizeText->SetValue("1");
	m_ManualBrushSizeSlider->SetNumberOfSteps(int(min(m_VolumeDimensions[0], m_VolumeDimensions[1]) / 4) - 2);
	m_ManualBrushSizeSlider->SetRange(1, int(min(m_VolumeDimensions[0], m_VolumeDimensions[1]) / 4), 1);
	m_ManualBrushSizeSlider->SetMax(int(min(m_VolumeDimensions[0], m_VolumeDimensions[1]) / 4));
	m_ManualBrushSizeSlider->SetMin(1);
	m_ManualBrushSizeSlider->Update();
	m_ManualBrushSizeText->Update();

	m_View->CameraUpdate();

	m_SER->GetAction("pntActionAutomatic")->UnBindDevice(m_DialogMouse);
	m_SER->GetAction("pntActionAutomatic")->UnBindInteractor(m_InitPER);
	m_SER->GetAction("pntEditingAction")->BindInteractor(m_EditPER);
	m_SER->GetAction("pntEditingAction")->BindDevice(m_DialogMouse);

	m_AutomaticScalarTextMapper->SetInput("");

	wxCursor cursor = wxCursor(wxCURSOR_PENCIL);
	m_View->GetWindow()->SetCursor(cursor);

	m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_MANUAL_PICKING_MODALITY, m_SlicePlane);
	m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_MANUAL_UNDO, false);
	m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_MANUAL_REDO, false);
	m_GuiDialog->Enable(EDIT_SEGMENTATION, true);

	m_GuiDialog->Enable(ID_BUTTON_NEXT, true);
	
	m_ManualSegmentationTools = 0;
	m_View->GetWindow()->SetCursor(cursor);

	EnableSizerContent(m_BucketEditingSizer, false);
	EnableSizerContent(m_BrushEditingSizer, true);
	m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Update();

	UpdateSlice();
	m_View->CameraUpdate();
	CreateRealDrawnImage();
}
//----------------------------------------------------------------------------
void mafOpSegmentation::OnPreviousStep()
{
	int answer = wxMessageBox(_("Current changes will be lost if you go on previous Step \nDo you want to continue?"), _("Warning"), wxYES_NO | wxCENTER);
	if (answer == wxNO)
		return;

	OnEditStepExit();

	m_SnippetsLabel->SetLabel(_(" 'Left Click + Ctrl' to select lower threshold. 'Left Click + Alt' to select upper threshold"));
	m_CurrentPhase = INIT_SEGMENTATION;

	m_AppendingOpGui->Remove(m_SegmentationOperationsGui[EDIT_SEGMENTATION]);
	m_AppendingOpGui->AddGui(m_SegmentationOperationsGui[INIT_SEGMENTATION]);
	m_AppendingOpGui->FitGui();

	m_GuiDialog->Enable(ID_BUTTON_PREV, false);
	m_GuiDialog->Enable(ID_BUTTON_NEXT, true);
	m_GuiDialog->FitGui();
	m_Dialog->Update();

	OnThresholdUpate();
}

// INIT PHASE EVENTS
//----------------------------------------------------------------------------
void mafOpSegmentation::OnInitEvent(mafEvent *e)
{
	switch (e->GetId())
	{
	case ID_AUTO_INC_MIN_THRESHOLD:
	case ID_AUTO_INC_MAX_THRESHOLD:
	case ID_AUTO_INC_MIDDLE_THRESHOLD:
	case ID_AUTO_DEC_MIN_THRESHOLD:
	case ID_AUTO_DEC_MAX_THRESHOLD:
	case ID_AUTO_DEC_MIDDLE_THRESHOLD:
		OnThresholdUpate(e->GetId());
		break;
	case ID_AUTO_THRESHOLD:
	{
		m_ThresholdSlider->GetSubRange(m_Threshold);
		UpdateThresholdLabel();
	}
	break;
	case ID_INIT_MODALITY:
	{
		OnChangeThresholdType();
		OnThresholdUpate();
		m_GuiDialog->Enable(ID_BUTTON_NEXT, (m_InitModality == RANGE_INIT && m_RangesVector.size() > 0) || (m_InitModality == FALSE));
	}
	break;
	case ID_RANGE_SELECTION:
	{
		if (!m_IgnoreRangeSelEvent && m_RangesGuiList->GetSelection() != -1)//Check if a range is selected
		{
			m_CurrentRange = m_RangesGuiList->GetSelection();
			m_SliceIndex = (m_RangesVector[m_CurrentRange].m_StartSlice + m_RangesVector[m_CurrentRange].m_EndSlice) / 2;

			SetThresholdByRange();
			EnableDisableGuiRange();
		}
	}
	break;
	case ID_SPLIT_RANGE:
		OnSplitRange();
		break;
	case ID_REMOVE_RANGE:
		OnRemoveRange();
		break;
	case ID_LOAD:
		OnLoadSegmentation();
		break;
	default:
		mafEventMacro(*e);
		return;
	}
	m_GuiDialog->Update();
}
//----------------------------------------------------------------------------
void mafOpSegmentation::OnSelectSlicePlane()
{
	if (m_CurrentPhase == EDIT_SEGMENTATION)
	{
		ApplyRealDrawnImage();
		m_Helper.ApplySliceChangesToVolume(m_OldSlicePlane, m_OldSliceIndex);
	}
	UpdateSliderValidator();

	m_View->SetSliceAxis(m_SlicePlane);

	mafPipeVolumeOrthoSlice *pipeOrtho = (mafPipeVolumeOrthoSlice *)m_View->GetNodePipe(m_Volume);
	m_VolumeSlice = (vtkImageData*)(pipeOrtho->GetSlicer(pipeOrtho->GetSliceDirection())->GetOutput());

	pipeOrtho = (mafPipeVolumeOrthoSlice *)m_View->GetNodePipe(m_SegmentationVolume);
	pipeOrtho->SetSliceOpacity(0.4);
	m_SegmentationSlice = (vtkImageData*)(pipeOrtho->GetSlicer(pipeOrtho->GetSliceDirection())->GetOutput());

	m_Helper.SetSlices(m_VolumeSlice, m_SegmentationSlice);

	m_GuiDialog->Update();
	if (m_CurrentPhase == EDIT_SEGMENTATION)

	UpdateSlice();
	m_View->CameraReset();

	if (m_CurrentPhase == INIT_SEGMENTATION)
		OnThresholdUpate();
	else // EDIT_SEGMENTATION
	{
		CreateRealDrawnImage();
		m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Update();
	}

	m_OldSliceIndex = m_SliceIndex;
	m_OldSlicePlane = m_SlicePlane;
}
//----------------------------------------------------------------------------
void mafOpSegmentation::OnUpdateSlice()
{
	if (m_CurrentPhase == EDIT_SEGMENTATION)
	{
		ApplyRealDrawnImage();
		m_Helper.ApplySliceChangesToVolume(m_OldSlicePlane, m_OldSliceIndex);
	}
	
	UpdateSlice();

	if (m_CurrentPhase == INIT_SEGMENTATION)
	{
		if (m_InitModality == RANGE_INIT)
		{
			//Range update
			if (m_SliceIndex<m_RangesVector[m_CurrentRange].m_StartSlice || m_SliceIndex>m_RangesVector[m_CurrentRange].m_EndSlice)
				SelectRangeByCurrentSlice();
			EnableDisableGuiRange();
		}

		UpdateThresholdRealTimePreview();
	}
	else // EDIT_SEGMENTATION
	{
		CreateRealDrawnImage();
		m_View->CameraUpdate();
	}

	m_OldSliceIndex = m_SliceIndex;
}
//----------------------------------------------------------------------------
void mafOpSegmentation::OnChangeThresholdType()
{
	EnableDisableGuiRange();

	int thesholdIDs[] = { ID_AUTO_INC_MIN_THRESHOLD, ID_AUTO_INC_MIDDLE_THRESHOLD, ID_AUTO_INC_MAX_THRESHOLD, ID_AUTO_DEC_MIN_THRESHOLD, ID_AUTO_DEC_MIDDLE_THRESHOLD, ID_AUTO_DEC_MAX_THRESHOLD };
	for (int i = 0; i < 6; i++)
		m_SegmentationOperationsGui[INIT_SEGMENTATION]->Enable(thesholdIDs[i], m_InitModality != LOAD_INIT);
	m_ThresholdSlider->Enable(m_InitModality != LOAD_INIT);

	m_SegmentationOperationsGui[INIT_SEGMENTATION]->Enable(ID_LOAD, m_InitModality == LOAD_INIT);

	if (m_InitModality == LOAD_INIT)
	{
		//forcing slice update
		double tmp[3] = { VTK_DOUBLE_MAX, VTK_DOUBLE_MAX, VTK_DOUBLE_MAX };
		m_View->SetSlice(tmp);
		UpdateSlice();
	}
	else if (m_InitModality == RANGE_INIT)
		SelectRangeByCurrentSlice();

	UpdateThresholdLabel();
}
//----------------------------------------------------------------------------
void mafOpSegmentation::OnThresholdUpate(int eventID)
{
	m_ThresholdSlider->GetSubRange(m_Threshold);
	//Fine tuning threshold selection, get the event and update relative values
	switch (eventID)
	{
	case ID_AUTO_INC_MIN_THRESHOLD:
		m_Threshold[0]++;
		break;
	case ID_AUTO_INC_MAX_THRESHOLD:
		m_Threshold[1]++;
		break;
	case ID_AUTO_DEC_MAX_THRESHOLD:
		m_Threshold[1]--;
		break;
	case ID_AUTO_DEC_MIN_THRESHOLD:
		m_Threshold[0]--;
		break;
	case ID_AUTO_INC_MIDDLE_THRESHOLD:
		m_Threshold[0]++;
		m_Threshold[1]++;
		break;
	case ID_AUTO_DEC_MIDDLE_THRESHOLD:
		m_Threshold[0]--;
		m_Threshold[1]--;
		break;
	case ID_RANGE_MODIFIED:
	default:
		break;
	}

	m_ThresholdSlider->SetSubRange(m_Threshold);
	if (m_InitModality == RANGE_INIT)
	{
		m_RangesVector[m_CurrentRange].m_Threshold[0] = m_Threshold[0];
		m_RangesVector[m_CurrentRange].m_Threshold[1] = m_Threshold[1];
		m_RangesGuiList->SetString(m_CurrentRange, wxString::Format("[%d,%d] low:%.1f high:%.1f", m_RangesVector[m_CurrentRange].m_StartSlice, m_RangesVector[m_CurrentRange].m_EndSlice, m_Threshold[0], m_Threshold[1]));
	}

	UpdateThresholdLabel();
	UpdateThresholdRealTimePreview();
	m_View->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafOpSegmentation::OnSplitRange()
{
	//Store the parameters
	AutomaticInfoRange range;
	range.m_StartSlice = m_SliceIndex + 1;
	range.m_EndSlice = m_RangesVector[m_CurrentRange].m_EndSlice;
	range.m_Threshold[0] = m_Threshold[0];
	range.m_Threshold[1] = m_Threshold[1];

	//update current range
	m_RangesVector[m_CurrentRange].m_EndSlice = m_SliceIndex;
	m_RangesGuiList->SetString(m_CurrentRange, wxString::Format("[%d,%d] low:%.1f high:%.1f", m_RangesVector[m_CurrentRange].m_StartSlice, m_RangesVector[m_CurrentRange].m_EndSlice, m_Threshold[0], m_Threshold[1]));

	//insert new range
	m_RangesVector.insert(m_RangesVector.begin() + m_CurrentRange + 1, range);
	m_RangesGuiList->Insert(wxString::Format("[%d,%d] low:%.1f high:%.1f", range.m_StartSlice, range.m_EndSlice, m_Threshold[0], m_Threshold[1]), m_CurrentRange + 1);

	m_SegmentationOperationsGui[INIT_SEGMENTATION]->Update();
	EnableDisableGuiRange();
}
//----------------------------------------------------------------------------
void mafOpSegmentation::OnRemoveRange()
{
	if (m_CurrentRange >= 1) //delete current and merge with prev range
	{
		m_RangesVector[m_CurrentRange - 1].m_EndSlice = m_RangesVector[m_CurrentRange].m_EndSlice;
		m_RangesVector.erase(m_RangesVector.begin() + m_CurrentRange);
		m_RangesGuiList->Delete(m_CurrentRange);
		m_CurrentRange--;
		SetRangeListSelection(m_CurrentRange);
	}
	else
	{
		m_RangesVector[m_CurrentRange + 1].m_StartSlice = 1;
		m_RangesVector.erase(m_RangesVector.begin());
		m_RangesGuiList->Delete(m_CurrentRange);
	}

	m_ThresholdSlider->SetSubRange(m_RangesVector[m_CurrentRange].m_Threshold);
	m_RangesGuiList->SetString(m_CurrentRange, wxString::Format("[%d,%d] low:%.1f high:%.1f", m_RangesVector[m_CurrentRange].m_StartSlice, m_RangesVector[m_CurrentRange].m_EndSlice, m_Threshold[0], m_Threshold[1]));
	m_SegmentationOperationsGui[INIT_SEGMENTATION]->Update();
	EnableDisableGuiRange();
	OnThresholdUpate();
}
//----------------------------------------------------------------------------
void mafOpSegmentation::OnLoadSegmentation()
{
	mafString title = mafString("Select a segmentation:");
	mafEvent e(this, VME_CHOOSE);
	e.SetString(&title);
	e.SetPointer(&mafOpSegmentation::SegmentedVolumeAccept);
	mafEventMacro(e);
	mafVME *vme = e.GetVme();
	mafVMEVolumeGray *newVolume = mafVMEVolumeGray::SafeDownCast(vme);

	if (newVolume)
	{
		m_SegmentationOperationsGui[INIT_SEGMENTATION]->Update();
		vtkDataSet * loadVtkData = newVolume->GetOutput()->GetVTKData();
		if (vtkRectilinearGrid::SafeDownCast(loadVtkData))
			m_SegmentationVolume->SetData((vtkRectilinearGrid *)loadVtkData, m_SegmentationVolume->GetTimeStamp());
		else
			m_SegmentationVolume->SetData((vtkImageData*)loadVtkData, m_SegmentationVolume->GetTimeStamp());
		UpdateSlice();
	}
}
//----------------------------------------------------------------------------
bool mafOpSegmentation::SegmentedVolumeAccept(mafVME*node)
{
	/* TODO fix checks*/
	if (node != NULL  \
		&& node->IsMAFType(mafVMEVolumeGray) \
		&& m_CurrentVolumeParametersInitialized \
		&& mafVMEVolumeGray::SafeDownCast(node)->GetOutput() != NULL \
		&& mafVMEVolumeGray::SafeDownCast(node)->GetOutput()->GetVTKData() != NULL \
		)
	{
		mafVMEVolumeGray *volumeToCheck = mafVMEVolumeGray::SafeDownCast(node);
		volumeToCheck->Update();
		/* loaded volume is of the same vtk type of the input volume */
		if (volumeToCheck->GetOutput()->GetVTKData()->GetDataObjectType() != m_InputVtkType)
		{
			return false;
		}
		/* loaded volume must have the same bounds of the input volume */

		double checkVolBounds[6];
		int checkVolDim[3];
		double checkVolSpacing[3];

		volumeToCheck->GetOutput()->GetBounds(checkVolBounds);

		//Vtk save/load system writes outputs to strings so there can be some approximations problem 
		//that can affect saved versus unsaved data comparison 
		//For this reason we need to compare data bounds after a conversion to string 
		if (!vtkImageData::SafeDownCast(volumeToCheck->GetOutput()->GetVTKData()))
		{
			//Rectilinear grid: we compare only the bound of the volume
			for (int i = 0; i < 6; i++)
			{
				double cVolBound, bVolBound;
				std::stringstream cVolBoundStr, bVolBoundStr;
				//We use stringstream to simulate VTK use of filestream
				cVolBoundStr << m_CurrentVolumeBounds[i];
				bVolBoundStr << checkVolBounds[i];
				cVolBoundStr >> cVolBound;
				bVolBoundStr >> bVolBound;

				if ((cVolBound != bVolBound))
				{
					return false;
				}
			}
		}
		else
		{
			//Image data case: We need to simulate VTK bounds calculus.
			//VTK does not saves the bound of the volume to disk
			//it saves origin, spacing, and dimensions (as number of voxels)
			//Then on loading it recalculate the bounds from this data, and obtain 
			//an amplification of the approximation error.
			//In this case we need to simulate this process of error amplification.
			vtkImageData::SafeDownCast(volumeToCheck->GetOutput()->GetVTKData())->GetDimensions(checkVolDim);
			vtkImageData::SafeDownCast(volumeToCheck->GetOutput()->GetVTKData())->GetSpacing(checkVolSpacing);
			for (int i = 0; i < 3; i++)
			{
				double cVolBound, bVolBound;
				std::stringstream cVolBoundStr, bVolBoundStr;
				cVolBoundStr << m_CurrentVolumeBounds[i];
				bVolBoundStr << checkVolBounds[i];
				cVolBoundStr >> cVolBound;
				bVolBoundStr >> bVolBound;

				if ((cVolBound != bVolBound))
				{
					return false;
				}
			}
			for (int i = 0; i < 3; i++)
			{
				double cVolBound, bVolBound, cVolSpacing, bVolSpacing, cVolTot, bVolTot;
				std::stringstream cVolBoundStr, bVolBoundStr, cVolSpacingStr, bVolSpacingStr;
				cVolBoundStr << m_CurrentVolumeBounds[i];
				bVolBoundStr << checkVolBounds[i];
				cVolBoundStr >> cVolBound;
				bVolBoundStr >> bVolBound;

				//converting both origin and spacing to strings
				cVolSpacingStr << m_CurrentVolumeSpacing[i];
				bVolSpacingStr << checkVolSpacing[i];
				cVolSpacingStr >> cVolSpacing;
				bVolSpacingStr >> bVolSpacing;

				cVolTot = cVolBound + cVolSpacing*checkVolDim[i];
				bVolTot = bVolBound + bVolSpacing*m_CurrentVolumeDimensions[i];

				if ((cVolTot != bVolTot || checkVolDim[i] != m_CurrentVolumeDimensions[i]))
				{
					return false;
				}
			}
		}

		/* scalar range should be 0 - 255 */
		double sr[2];
		volumeToCheck->GetOutput()->Update();
		volumeToCheck->GetOutput()->GetVTKData()->Update();
		if (vtkDataSet::SafeDownCast(volumeToCheck->GetOutput()->GetVTKData())->GetPointData() != NULL && vtkDataSet::SafeDownCast(volumeToCheck->GetOutput()->GetVTKData())->GetPointData()->GetScalars() != NULL)
		{
			vtkDataSet::SafeDownCast(volumeToCheck->GetOutput()->GetVTKData())->GetPointData()->GetScalars()->GetRange(sr);
			if (sr[0] < 0 || sr[1] > 255) /* scalar range could be 0-0 empty masc or 255-255 full mask */
			{
				return false;
			}
		}
		return true;
	}

	return false;
}

// EDIT PHASE EVENTS
//------------------------------------------------------------------------
void mafOpSegmentation::OnEditSegmentationEvent(mafEvent *e)
{
	switch (e->GetId())
	{
		case ID_MANUAL_BUCKET_GLOBAL:
		{
			if (m_GlobalFloodFill)
				m_ManualRangeSlider->Enable(true);
			else
				m_ManualRangeSlider->Enable(false);

			m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Update();
		}
		break;

		case ID_MANUAL_TOOLS_BRUSH:
		{
			m_ManualSegmentationTools = 0;

			wxCursor cursor = wxCursor(wxCURSOR_PENCIL);
			m_View->GetWindow()->SetCursor(cursor);

			EnableSizerContent(m_BucketEditingSizer, false);
			EnableSizerContent(m_BrushEditingSizer, true);
			m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Update();
		}
		break;
		case ID_MANUAL_TOOLS_FILL:
		{
			m_ManualSegmentationTools = 1;

			wxCursor cursor = wxCursor(wxCURSOR_SPRAYCAN);
			m_View->GetWindow()->SetCursor(cursor);

			EnableSizerContent(m_BucketEditingSizer, true);
			EnableSizerContent(m_BrushEditingSizer, false);
			m_ManualRangeSlider->Enable(m_GlobalFloodFill == TRUE);
			m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Update();
			ApplyRealDrawnImage();
			OnUpdateSlice();
		}
		break;
		case ID_MANUAL_BRUSH_SHAPE:
		case ID_MANUAL_BRUSH_SIZE:
		{
			m_BrushShape = m_ManualBrushShapeRadioBox->GetSelection();
			ApplyRealDrawnImage();
			m_View->CameraUpdate();
		}
		break;
		case ID_MANUAL_UNDO:
			OnUndoRedo(true);
			break;
		case ID_MANUAL_REDO:
			OnUndoRedo(false);
			break;
		default:
			mafEventMacro(*e);
	}
	m_GuiDialog->SetFocusIgnoringChildren();
}
//----------------------------------------------------------------------------
void mafOpSegmentation::OnUndoRedo(bool undo)
{
	std::vector<UndoRedoState> &from = undo ? m_UndoList : m_RedoList; 
	std::vector<UndoRedoState> &to   = undo ? m_RedoList : m_UndoList; 
	
	int fromListSize = from.size();
	
	//if i changed slice/plane from last edit the redo information
	//are in the plane-slice of last edit (where i saved last undo info).
	if (m_SlicePlane != from[fromListSize - 1].m_Plane)
	{
		m_SlicePlane = from[fromListSize - 1].m_Plane;
		m_SliceIndex = from[fromListSize - 1].m_Slice;
		OnSelectSlicePlane();
	}
	else if (m_SliceIndex != from[fromListSize - 1].m_Slice)
	{
		m_SliceIndex = from[fromListSize - 1].m_Slice;
		OnUpdateSlice();
	}

	to.push_back(CreateUndoRedoState());

	m_RealDrawnImage->DeepCopy(from[fromListSize - 1].m_Scalars);
	ApplyRealDrawnImage();

	from.pop_back();
	m_View->CameraUpdate();

	//Enable-disable buttons
	m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_MANUAL_UNDO, m_UndoList.size() > 0);
	m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_MANUAL_REDO, m_RedoList.size() > 0);
}
//------------------------------------------------------------------------
void mafOpSegmentation::OnRefinementSegmentationEvent(mafEvent *e)
{
	switch (e->GetId())
	{
	case ID_REFINEMENT_ACTION:
	{
		m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_REFINEMENT_ITERATIVE, m_RefinementSegmentationAction == ID_REFINEMENT_ISLANDS_REMOVE);
	}
	break;
	case ID_REFINEMENT_UNDO:
	{
		int numOfChanges = m_RefinementUndoList.size();
		if (numOfChanges)
		{
			vtkDataSet *dataSet = m_SegmentationVolume->GetOutput()->GetVTKData();

			vtkUnsignedCharArray *redoScalars = vtkUnsignedCharArray::New();
			redoScalars->DeepCopy(dataSet->GetPointData()->GetScalars());
			redoScalars->SetName("SCALARS");
			m_RefinementRedoList.push_back(redoScalars);

			vtkDataArray *undoScalars = m_RefinementUndoList[numOfChanges - 1];

			dataSet->GetPointData()->SetScalars(undoScalars);
			dataSet->Update();

			vtkMAFSmartPointer<vtkStructuredPoints> newDataSet;
			newDataSet->DeepCopy(dataSet);

			m_SegmentationVolume->SetData(newDataSet, m_Volume->GetTimeStamp());

			vtkDEL(m_RefinementUndoList[numOfChanges - 1]);
			m_RefinementUndoList.pop_back();

			m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_REFINEMENT_UNDO, m_RefinementUndoList.size() > 0);
			m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_REFINEMENT_REDO, m_RefinementRedoList.size() > 0);

			UpdateSlice();
		}
		break;
	}

	case ID_REFINEMENT_REDO:
	{
		int numOfChanges = m_RefinementRedoList.size();
		if (numOfChanges)
		{
			vtkDataSet *dataSet = m_SegmentationVolume->GetOutput()->GetVTKData();

			vtkUnsignedCharArray *undoScalars = vtkUnsignedCharArray::New();
			undoScalars->DeepCopy(dataSet->GetPointData()->GetScalars());
			undoScalars->SetName("SCALARS");
			m_RefinementUndoList.push_back(undoScalars);

			vtkDataArray *redoScalars = m_RefinementRedoList[numOfChanges - 1];

			dataSet->GetPointData()->SetScalars(redoScalars);
			dataSet->Update();

			vtkMAFSmartPointer<vtkStructuredPoints> newDataSet;
			newDataSet->DeepCopy(dataSet);

			m_SegmentationVolume->SetData(newDataSet, m_Volume->GetTimeStamp());

			vtkDEL(m_RefinementRedoList[numOfChanges - 1]);
			m_RefinementRedoList.pop_back();

			m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_REFINEMENT_UNDO, m_RefinementUndoList.size() > 0);
			m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_REFINEMENT_REDO, m_RefinementRedoList.size() > 0);

			UpdateSlice();
		}
		break;
	}
	case ID_REFINEMENT_APPLY:
	{
		vtkUnsignedCharArray *scalars = vtkUnsignedCharArray::New();
		scalars->DeepCopy(m_SegmentationVolume->GetOutput()->GetVTKData()->GetPointData()->GetScalars());
		scalars->SetName("SCALARS");

		m_RefinementUndoList.push_back(scalars);

		if (!Refinement())
		{
			break;
		}

		m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_REFINEMENT_UNDO, m_RefinementUndoList.size() > 0);
		m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_REFINEMENT_REDO, m_RefinementRedoList.size() > 0);

		UpdateSlice();
		m_View->CameraUpdate();
	}
	break;
	default:
		mafEventMacro(*e);
	}

}
//----------------------------------------------------------------------------
void mafOpSegmentation::OnEditStepExit()
{
	ApplyRealDrawnImage();
	m_Helper.ApplySliceChangesToVolume(m_SlicePlane, m_SliceIndex);
	vtkDEL(m_RealDrawnImage);

	ClearManualUndoList();
	ClearManualRedoList();

  //Gui stuff
  //set default cursor - remove draw actor  
  wxCursor cursor = wxCursor( wxCURSOR_DEFAULT );
  m_View->GetWindow()->SetCursor(cursor);
  //logic stuff
  m_SER->GetAction("pntEditingAction")->UnBindInteractor(m_EditPER);
  m_SER->GetAction("pntEditingAction")->UnBindDevice(m_DialogMouse);
  m_SER->GetAction("pntActionAutomatic")->BindDevice(m_DialogMouse);
  m_SER->GetAction("pntActionAutomatic")->BindInteractor(m_InitPER);
   
  m_GuiDialog->Enable(EDIT_SEGMENTATION,false);

  m_SnippetsLabel->SetLabel( _(""));
}
//----------------------------------------------------------------------------
void mafOpSegmentation::OnRefinementStep()
{
  //gui stuff
  m_Dialog->Update();
  m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_REFINEMENT_UNDO, false);
  m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_REFINEMENT_REDO, false);
  m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_REFINEMENT_APPLY, true);
  m_GuiDialog->Enable(ID_BUTTON_NEXT,false);

	//logic stuff
  UpdateSlice();
}
//----------------------------------------------------------------------------
void mafOpSegmentation::OnEventFloodFill(mafEvent *e)
{
	int id;
	id = e->GetArg();
	int seed[3];
	double seedCoords[3];
	double origin[3];
	vtkDataSet * dataSet = m_SegmentationVolume->GetOutput()->GetVTKData();
	dataSet->GetPoint(id, seedCoords);
	dataSet->GetPoint(0, origin);

	vtkImageData* im = vtkImageData::SafeDownCast(dataSet);
	vtkRectilinearGrid* rg = vtkRectilinearGrid::SafeDownCast(dataSet);
	if (im != NULL)
	{
		double spacing[3];
		im->GetSpacing(spacing);
		seed[0] = (seedCoords[0] - origin[0]) / spacing[0];
		seed[1] = (seedCoords[1] - origin[1]) / spacing[1];
		seed[2] = (seedCoords[2] - origin[2]) / spacing[2];
	}
	else if (rg)
	{
		vtkDoubleArray* xa = (vtkDoubleArray*)rg->GetXCoordinates();
		vtkDoubleArray* ya = (vtkDoubleArray*)rg->GetYCoordinates();
		vtkDoubleArray* za = (vtkDoubleArray*)rg->GetZCoordinates();

		for (int x = 0; x < xa->GetSize(); x++)
		{
			if (xa->GetTuple1(x) == seedCoords[0])
			{
				seed[0] = x;
				break;
			}
		}
		for (int y = 0; y < ya->GetSize(); y++)
		{
			if (ya->GetTuple1(y) == seedCoords[1])
			{
				seed[1] = y;
				break;
			}
		}
		for (int z = 0; z < za->GetSize(); z++)
		{
			if (za->GetTuple1(z) == seedCoords[2])
			{
				seed[2] = z;
				break;
			}
		}
	}
	double dims[3];
	dims[0] = m_VolumeDimensions[0];
	dims[1] = m_VolumeDimensions[1];
	dims[2] = m_VolumeDimensions[2];

	if (m_GlobalFloodFill != TRUE)
	{
		dims[m_SlicePlane] = 1;
		seed[m_SlicePlane] = 0;
	}
	else
	{
		double low, hi;
		m_ManualRangeSlider->GetSubRange(&low, &hi);
		dims[m_SlicePlane] = (hi - low) + 1;
		seed[m_SlicePlane] = seed[m_SlicePlane] - (low - 1);
	}

	// recalculate seed id
	vtkIdType seedID = seed[0] + seed[1] * dims[0] + seed[2] * dims[1] * dims[0];
	FloodFill(seedID);
	CreateRealDrawnImage();
}

// DRAW //////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void mafOpSegmentation::StartDraw(mafEvent *e, bool erase)
{
	if (erase) m_BrushModality = MANUAL_SEGMENTATION_ERASE;
	else m_BrushModality = MANUAL_SEGMENTATION_SELECT;
	
	m_UndoList.push_back(CreateUndoRedoState());

	m_IsDrawing = true;

	//On edit a new branch of redo-list starts, i need to clear the redo stack
	ClearManualRedoList();
	m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_MANUAL_UNDO, true);
	m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_MANUAL_REDO, false);
}
//----------------------------------------------------------------------------
UndoRedoState mafOpSegmentation::CreateUndoRedoState()
{
	//Picking starts here I need to save an undo stack
	ApplyRealDrawnImage();

	//Create State 
	UndoRedoState urs;
	urs.m_Scalars = vtkUnsignedCharArray::New();
	urs.m_Scalars->DeepCopy(m_RealDrawnImage);
	urs.m_Plane = m_SlicePlane;
	urs.m_Slice = m_SliceIndex;

	return urs;
}
//----------------------------------------------------------------------------
void mafOpSegmentation::CreateRealDrawnImage()
{
	if (!m_RealDrawnImage)
		vtkNEW(m_RealDrawnImage);
	
	m_RealDrawnImage->DeepCopy(m_SegmentationSlice->GetPointData()->GetScalars());
}
//----------------------------------------------------------------------------
void mafOpSegmentation::ApplyRealDrawnImage()
{
	if (m_RealDrawnImage)
	{
		vtkDataArray* scalars = m_SegmentationSlice->GetPointData()->GetScalars();
		scalars->DeepCopy(m_RealDrawnImage);
		scalars->Modified();
	}
}
//----------------------------------------------------------------------------
void mafOpSegmentation::FloodFill(vtkIdType seed)
{
	ApplyRealDrawnImage();

	int center = seed;
	if (m_GlobalFloodFill == TRUE)
	{
		UndoRedoState urs;
		urs.m_Scalars = vtkUnsignedCharArray::New();
		urs.m_Scalars->DeepCopy(m_SegmentationVolume->GetOutput()->GetVTKData()->GetPointData()->GetScalars());
		urs.m_Scalars->SetName("SCALARS");
		urs.m_Plane = -1; // indicate that the undo redo data is global and must be performed on manual volume mask (not slice)
		urs.m_Slice = -1; // indicate that the undo redo data is global and must be performed on manual volume mask (not slice)
		m_UndoList.push_back(urs);
		//On edit a new branch of redo-list starts, i need to clear the redo stack
		ClearManualRedoList();

		m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_MANUAL_REDO, false);
		m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_MANUAL_UNDO, m_UndoList.size() > 0);

		wxBusyCursor wait_cursor;
		wxBusyInfo wait(_("Wait! The algorithm could take long time!"));

		vtkMAFSmartPointer <vtkImageData>dummy;
		dummy->SetExtent(0, m_VolumeDimensions[0] - 1, 0, m_VolumeDimensions[1] - 1, 0, m_VolumeDimensions[2] - 1);
		dummy->SetSpacing(m_VolumeSpacing);
		dummy->SetOrigin(0, 0, 0);

		m_SegmentationVolume->GetOutput()->GetVTKData()->GetPointData()->Update();
		dummy->SetScalarTypeToUnsignedChar();
		dummy->GetPointData()->SetScalars(m_SegmentationVolume->GetOutput()->GetVTKData()->GetPointData()->GetScalars());

		int ext[6];
		double low, hi;
		m_ManualRangeSlider->GetSubRange(&low, &hi);
		hi--;
		low--;

		ext[0] = 0;
		ext[1] = m_VolumeDimensions[0] - 1;
		ext[2] = 0;
		ext[3] = m_VolumeDimensions[1] - 1;
		ext[4] = 0;
		ext[5] = m_VolumeDimensions[2] - 1;

		ext[m_SlicePlane * 2] = (int)low;
		ext[m_SlicePlane * 2 + 1] = (int)hi;

		vtkMAFSmartPointer <vtkImageClip> clipper;
		clipper->SetInput(dummy);
		clipper->SetOutputWholeExtent(ext);
		clipper->SetClipData(TRUE);
		clipper->Update();
		vtkImageData *clippedDummy = clipper->GetOutput();
		clippedDummy->Update();

		vtkMAFSmartPointer <vtkImageData> input;
		input->SetExtent(0, (ext[1] - ext[0]), 0, (ext[3] - ext[2]), 0, (ext[5] - ext[4]));
		input->SetSpacing(m_VolumeSpacing);
		input->SetOrigin(0, 0, 0);
		input->GetPointData()->SetScalars(clippedDummy->GetPointData()->GetScalars());
		input->SetScalarTypeToUnsignedChar();
		input->Update();

		vtkMAFSmartPointer <vtkImageData> output;

		int center = ApplyFloodFill(input, output, seed);

		output->Update();

		vtkUnsignedCharArray* outScalars = (vtkUnsignedCharArray*)m_SegmentationVolume->GetOutput()->GetVTKData()->GetPointData()->GetScalars();
		for (int x = ext[0]; x <= ext[1]; x++)
		{
			for (int y = ext[2]; y <= ext[3]; y++)
			{
				for (int z = ext[4]; z <= ext[5]; z++)
				{
					outScalars->SetTuple1(x + y * m_VolumeDimensions[0] + z * m_VolumeDimensions[1] * m_VolumeDimensions[0], output->GetPointData()->GetScalars()->GetTuple1((x - ext[0]) + (y - ext[2]) * (ext[1] - ext[0] + 1) + (z - ext[4]) *  (ext[1] - ext[0] + 1) * (ext[3] - ext[2] + 1)));
				}
			}
		}
		m_SegmentationVolume->GetOutput()->GetVTKData()->GetPointData()->SetScalars(outScalars);
		m_SegmentationVolume->Update();

		UpdateSlice();

		CreateRealDrawnImage();
		OnUpdateSlice();
	}
	else
	{
		UndoRedoState urs;
		urs.m_Scalars = vtkUnsignedCharArray::New();
		urs.m_Scalars->DeepCopy(m_SegmentationVolume->GetOutput()->GetVTKData()->GetPointData()->GetScalars());
		urs.m_Scalars->SetName("SCALARS");
		urs.m_Plane = m_SlicePlane;
		urs.m_Slice = m_SliceIndex;
		m_UndoList.push_back(urs);
		//On edit a new branch of redo-list starts, i need to clear the redo stack
		ClearManualRedoList();

		m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_MANUAL_REDO, false);
		m_SegmentationOperationsGui[EDIT_SEGMENTATION]->Enable(ID_MANUAL_UNDO, m_UndoList.size() > 0);
		vtkStructuredPoints *input = vtkStructuredPoints::New();
		double dimensions[3];


		switch (m_SlicePlane)
		{
		case XY:
		{
			dimensions[0] = m_VolumeDimensions[0];
			dimensions[1] = m_VolumeDimensions[1];
			dimensions[2] = 1;
		} break;
		case XZ:
		{
			dimensions[0] = m_VolumeDimensions[0];
			dimensions[1] = m_VolumeDimensions[2];
			dimensions[2] = 1;
		} break;
		case YZ:
		{
			dimensions[0] = m_VolumeDimensions[1];
			dimensions[1] = m_VolumeDimensions[2];
			dimensions[2] = 1;
		} break;
		}

		input->SetExtent(0, dimensions[0] - 1, 0, dimensions[1] - 1, 0, dimensions[2] - 1);

		input->SetSpacing(m_VolumeSpacing);
		input->SetOrigin(0, 0, 0);

		m_SegmentationVolume->GetOutput()->GetVTKData()->GetPointData()->Update();
		input->SetScalarTypeToUnsignedChar();
		input->GetPointData()->SetScalars(m_SegmentationVolume->GetOutput()->GetVTKData()->GetPointData()->GetScalars());

		vtkStructuredPoints *output = vtkStructuredPoints::New();
		output->CopyStructure(input);
		output->DeepCopy(input);

		int center = ApplyFloodFill(input, output, seed);

		m_SegmentationVolume->GetOutput()->GetVTKData()->GetPointData()->SetScalars(output->GetPointData()->GetScalars());
		m_SegmentationVolume->Update();

		CreateRealDrawnImage();
		OnUpdateSlice();

		input->Delete();
		output->Delete();
	}
}
//----------------------------------------------------------------------------
bool mafOpSegmentation::Refinement()
{
	wxBusyCursor wait_cursor;
	wxBusyInfo wait(_("Wait! The algorithm could take long time!"));

	vtkDataSet *inputDataSet = vtkDataSet::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_SegmentationVolume)->GetOutput()->GetVTKData());

	if (inputDataSet)
	{
		inputDataSet->Update();

		long progress = 0;
		m_ProgressBar->SetValue(progress);
		m_ProgressBar->Show(true);
		m_ProgressBar->Update();

		vtkMAFSmartPointer<vtkUnsignedCharArray> newScalars;
		newScalars->SetName("SCALARS");
		newScalars->SetNumberOfTuples(m_VolumeDimensions[0] * m_VolumeDimensions[1] * m_VolumeDimensions[2]);

		double point[3];
		inputDataSet->GetPoint(m_SliceIndex*m_VolumeDimensions[0] * m_VolumeDimensions[1], point);

		vtkDataArray *inputScalars = inputDataSet->GetPointData()->GetScalars();
		vtkMAFSmartPointer<vtkUnsignedCharArray> scalars;
		scalars->SetName("SCALARS");
		scalars->SetNumberOfTuples(m_VolumeDimensions[0] * m_VolumeDimensions[1]);

		if (m_RefinementEverySlice)
		{
			for (int i = 0; i < m_VolumeDimensions[2]; i++)
			{
				progress = (i * 100 / m_VolumeDimensions[2]);
				m_ProgressBar->SetValue(progress);
				m_ProgressBar->Update();

				for (int k = 0; k < (m_VolumeDimensions[0] * m_VolumeDimensions[1]); k++)
				{
					unsigned char value = inputScalars->GetTuple1(k + i*(m_VolumeDimensions[0] * m_VolumeDimensions[1]));
					scalars->SetTuple1(k, value);
				}

				vtkMAFSmartPointer<vtkStructuredPoints> im;
				im->SetDimensions(m_VolumeDimensions[0], m_VolumeDimensions[1], 1);
				im->SetSpacing(m_VolumeSpacing[0], m_VolumeSpacing[1], 0.0);
				im->GetPointData()->AddArray(scalars);
				im->GetPointData()->SetActiveScalars("SCALARS");
				im->SetScalarTypeToUnsignedChar();
				im->Update();

				vtkMAFSmartPointer<vtkStructuredPoints> filteredImage;
				if (ApplyRefinementFilter2(im, filteredImage) && filteredImage)
				{
					vtkDataArray *binaryScalars = filteredImage->GetPointData()->GetScalars();

					for (int k = 0; k < filteredImage->GetNumberOfPoints(); k++)
					{
						unsigned char value = binaryScalars->GetTuple1(k);
						newScalars->SetTuple1(k + i*(m_VolumeDimensions[0] * m_VolumeDimensions[1]), value);
					}
				}
			}

		}
		else
		{
			for (int i = 0; i < m_VolumeDimensions[2]; i++)
			{
				if (i != m_SliceIndex - 1)
					//if(i != zID)
				{
					for (int k = 0; k < (m_VolumeDimensions[0] * m_VolumeDimensions[1]); k++)
					{
						unsigned char value = inputScalars->GetTuple1(k + i*(m_VolumeDimensions[0] * m_VolumeDimensions[1]));
						newScalars->SetTuple1(k + i*(m_VolumeDimensions[0] * m_VolumeDimensions[1]), value);
					}
				}
				else
				{
					for (int k = 0; k < (m_VolumeDimensions[0] * m_VolumeDimensions[1]); k++)
					{
						unsigned char value = inputScalars->GetTuple1(k + i*(m_VolumeDimensions[0] * m_VolumeDimensions[1]));
						scalars->SetTuple1(k, value);
					}

					vtkMAFSmartPointer<vtkStructuredPoints> im;
					im->SetDimensions(m_VolumeDimensions[0], m_VolumeDimensions[1], 1);
					im->SetSpacing(m_VolumeSpacing[0], m_VolumeSpacing[1], 0.0);
					im->GetPointData()->AddArray(scalars);
					im->GetPointData()->SetActiveScalars("SCALARS");
					im->SetScalarTypeToUnsignedChar();
					im->Update();

					vtkMAFSmartPointer<vtkStructuredPoints> filteredImage;
					if (ApplyRefinementFilter2(im, filteredImage) && filteredImage)
					{
						vtkDataArray *binaryScalars = filteredImage->GetPointData()->GetScalars();

						for (int k = 0; k < filteredImage->GetNumberOfPoints(); k++)
						{
							unsigned char value = binaryScalars->GetTuple1(k);
							newScalars->SetTuple1(k + i*(m_VolumeDimensions[0] * m_VolumeDimensions[1]), value);
						}
					}
				}
			}
		}

		if (inputDataSet->IsA("vtkStructuredPoints"))
		{
			vtkMAFSmartPointer<vtkStructuredPoints> newSP;
			newSP->CopyStructure(vtkStructuredPoints::SafeDownCast(inputDataSet));
			newSP->Update();
			newSP->GetPointData()->AddArray(newScalars);
			newSP->GetPointData()->SetActiveScalars("SCALARS");
			newSP->SetScalarTypeToUnsignedChar();
			newSP->Update();

			m_SegmentationVolume->SetData(newSP, m_Volume->GetTimeStamp());
			vtkStructuredPoints *spVME = vtkStructuredPoints::SafeDownCast(m_SegmentationVolume->GetOutput()->GetVTKData());
			spVME->Update();
		}
		else
		{
			vtkMAFSmartPointer<vtkRectilinearGrid> newRG;
			newRG->CopyStructure(vtkRectilinearGrid::SafeDownCast(inputDataSet));
			newRG->Update();
			newRG->GetPointData()->AddArray(newScalars);
			newRG->GetPointData()->SetActiveScalars("SCALARS");
			newRG->Update();

			m_SegmentationVolume->SetData(newRG, m_Volume->GetTimeStamp());
			vtkRectilinearGrid *rgVME = vtkRectilinearGrid::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_SegmentationVolume)->GetOutput()->GetVTKData());
			rgVME->Update();
		}

		m_SegmentationVolume->Update();

		m_ProgressBar->SetValue(100);
		m_ProgressBar->Show(false);
		m_ProgressBar->Update();

		UpdateSlice();
	}

	return true;
}

// GET SET ///////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
void mafOpSegmentation::GetSliceOrigin(double *origin)
{
  if (m_Volume->GetOutput()->GetVTKData()->IsA("vtkStructuredPoints"))
  {
    vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(m_Volume->GetOutput()->GetVTKData());
    sp->Update();
    double spc[3];
    sp->GetSpacing(spc);
    sp->GetOrigin(origin);

    if (m_SlicePlane == XY)
      origin[2] = (m_SliceIndex-1)*spc[2]+origin[2];
    else if (m_SlicePlane == XZ)
      origin[1] = (m_SliceIndex -1)*spc[1]+origin[1];
    else if (m_SlicePlane == YZ)
      origin[0] = (m_SliceIndex -1)*spc[0]+origin[0];

  }
  else
  {
    vtkRectilinearGrid *rg = vtkRectilinearGrid::SafeDownCast(m_Volume->GetOutput()->GetVTKData());
    rg->Update();
    origin[0] = rg->GetXCoordinates()->GetTuple1(0);
    origin[1] = rg->GetYCoordinates()->GetTuple1(0);
    origin[2] = rg->GetZCoordinates()->GetTuple1(0);

    if (m_SlicePlane == XY)
      origin[2] = rg->GetZCoordinates()->GetTuple1(m_SliceIndex -1);
    else if (m_SlicePlane == XZ)
      origin[1] = rg->GetYCoordinates()->GetTuple1(m_SliceIndex -1);
    if (m_SlicePlane == YZ)
      origin[0] = rg->GetXCoordinates()->GetTuple1(m_SliceIndex -1);
  }
}


//----------------------------------------------------------------------------
void mafOpSegmentation::SelectRangeByCurrentSlice()
{
	m_CurrentRange = 0;
	int rangesLimit = m_RangesVector.size() - 1;
	while (m_SliceIndex > m_RangesVector[m_CurrentRange].m_EndSlice && m_CurrentRange < rangesLimit)
		m_CurrentRange++;
	m_Threshold[0] = m_RangesVector[m_CurrentRange].m_Threshold[0];
	m_Threshold[1] = m_RangesVector[m_CurrentRange].m_Threshold[1];
	m_ThresholdSlider->SetSubRange(m_Threshold);
	SetRangeListSelection(m_CurrentRange);
	EnableDisableGuiRange();
}
//----------------------------------------------------------------------------
void mafOpSegmentation::SetRangeListSelection(int index)
{
	m_IgnoreRangeSelEvent = true;
	m_RangesGuiList->SetSelection(index);
	m_RangesGuiList->Update();
	wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, m_RangesGuiList->GetId());
	event.SetEventObject(m_RangesGuiList);
	event.SetInt(index);
	m_RangesGuiList->GetEventHandler()->ProcessEvent(event);
	m_IgnoreRangeSelEvent = false;
}
//----------------------------------------------------------------------------
void mafOpSegmentation::SetThresholdByRange()
{
	m_Threshold[0] = m_RangesVector[m_CurrentRange].m_Threshold[0];
	m_Threshold[1] = m_RangesVector[m_CurrentRange].m_Threshold[1];
	m_ThresholdSlider->SetSubRange(m_Threshold);
	UpdateSlice();
	UpdateThresholdRealTimePreview();
}

// RESET /////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void mafOpSegmentation::ClearManualRedoList()
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_RedoList.size();i++)
    vtkDEL(m_RedoList[i].m_Scalars);
  m_RedoList.clear();
}
//----------------------------------------------------------------------------
void mafOpSegmentation::ClearManualUndoList()
{
  for (int i=0;i<m_UndoList.size();i++)
    vtkDEL(m_UndoList[i].m_Scalars);
  m_UndoList.clear();
}
//----------------------------------------------------------------------------
void mafOpSegmentation::ResetRefinementRedoList()
{
  for (int i=0;i<m_RefinementRedoList.size();i++)
    vtkDEL(m_RefinementRedoList[i]);
  m_RefinementRedoList.clear();
}
//----------------------------------------------------------------------------
void mafOpSegmentation::ResetRefinementUndoList()
{
  for (int i=0;i<m_RefinementUndoList.size();i++)
    vtkDEL(m_RefinementUndoList[i]);
  m_RefinementUndoList.clear();
}
