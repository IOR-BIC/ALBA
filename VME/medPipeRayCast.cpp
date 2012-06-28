/*=========================================================================
Program:   Multimod Application Framework
Language:  C++
Authors:   Gianluigi Crimi  -  Grazia Di Cosmo
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#if defined(WIN32)
#pragma warning (disable : 4018)
#endif

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medPipeRayCast.h"
#include "mafEvent.h"
#include "mafSceneNode.h"
#include "mafGUIFloatSlider.h"
#include "mafGUI.h"

#include "mafVME.h"
#include "mafVMEVolumeGray.h"
#include "mafEventSource.h"

#include "wx/busyinfo.h"

#include "vtkMAFAssembly.h"
#include "vtkMAFSmartPointer.h"
#include "vtkActor.h"
#include "vtkVolume.h"
#include "vtkDataSet.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeRayCastMapper.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkImageCast.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkMEDRayCastCleaner.h"
#include "vtkMAFVolumeResample.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"

#include "vtkProperty.h"
#include "vtkPolyDataMapper.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkStructuredPoints.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medPipeRayCast);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medPipeRayCast::medPipeRayCast(double muscleOpacity,double bloodOpacity,double boneOpacity) 
: mafPipe()
//----------------------------------------------------------------------------
{

  //Setting initial Values to Null
	m_Volume          = NULL;
	
  m_ColorFunction   = NULL;
  m_OpacityFunction = NULL;
  m_RayCastMapper   = NULL; 
  
  m_OutlineActor    = NULL;
  m_OutlineBox      = NULL;
	m_OutlineMapper   = NULL;
  m_VolumeCaster    = NULL;
  m_RayCastCleaner  = NULL;
  m_ResampleFilter  = NULL;

  //Setting Opacity 
  //Default values 0.15 - 0.8 - 0.2  
  m_MuscleOpacity=muscleOpacity;
  m_BloodOpacity=bloodOpacity;
  m_BoneOpacity=boneOpacity;


  //Thresholding values
  //Muscles
  m_MuscleLowerThreshold=10.0;
  m_MuscleUpperThreshold=80.0;
  //Blood
  m_BloodLowerThreshold=200.0;
  m_BloodUpperThreshold=340.0;
  //Bones
  m_BoneLowerThreshold=350.0;

  m_OnLoading=false;

  m_BoundingBoxVisibility = true;
}
//----------------------------------------------------------------------------
void medPipeRayCast::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
	Superclass::Create(n);

	assert(m_Vme->GetOutput()->IsA("mafVMEOutputVolume"));

  m_Vme->GetEventSource()->AddObserver(this);

  vtkDataSet *dataset = m_Vme->GetOutput()->GetVTKData();
  
  //If the volume is not loaded this dataset->update() calls update-event
  //in this case we disable temporary the call to UpdateFromData() in order
  //to have a single call to that function
  //if the volume is already loaded m_Onloading changes simple do nothing
  m_OnLoading=true;
  dataset->Update();
  m_OnLoading=false;

  UpdateFromData();
  
  // selection box
  vtkNEW(m_OutlineBox);
  m_OutlineBox->SetInput(dataset);

  vtkNEW(m_OutlineMapper);
  m_OutlineMapper->SetInput(m_OutlineBox->GetOutput());

  vtkNEW(m_OutlineActor);
  m_OutlineActor->SetMapper(m_OutlineMapper);
  m_OutlineActor->VisibilityOn();
  m_OutlineActor->PickableOff();

  //box property
  vtkMAFSmartPointer<vtkProperty> property;
  property->SetColor(1,1,1);
  property->SetAmbient(1);
  property->SetRepresentationToWireframe();
  property->SetInterpolationToFlat();
  m_OutlineActor->SetProperty(property);

  if(m_BoundingBoxVisibility)
    m_AssemblyFront->AddPart(m_OutlineActor);
}
//----------------------------------------------------------------------------
medPipeRayCast::~medPipeRayCast()
//----------------------------------------------------------------------------
{

  //Removing pipe binding
  m_Vme->GetEventSource()->RemoveObserver(this);

	m_AssemblyFront->RemovePart(m_Volume);
	
  if(m_BoundingBoxVisibility)
    m_AssemblyFront->RemovePart(m_OutlineActor);


  //Free memory
	vtkDEL(m_Volume);

  vtkDEL(m_RayCastMapper);
  vtkDEL(m_ColorFunction);
  vtkDEL(m_OpacityFunction);
  
  vtkDEL(m_OutlineActor);
  vtkDEL(m_OutlineBox);
	vtkDEL(m_OutlineMapper);
  vtkDEL(m_VolumeCaster);

}

//----------------------------------------------------------------------------
void medPipeRayCast::Select(bool sel) 
//----------------------------------------------------------------------------
{
  //show/hide outline box on selection operations
	m_Selected = sel;
	if (m_Volume->GetVisibility())
	{
		m_OutlineActor->SetVisibility(sel);
	}
}


//----------------------------------------------------------------------------
mafGUI *medPipeRayCast::CreateGui()
//----------------------------------------------------------------------------
{
	double range[2] = {0, 0};
	m_Vme->GetOutput()->GetVTKData()->GetScalarRange(range);

	assert(m_Gui == NULL);
	m_Gui = new mafGUI(this);
	
  //Creating Gui

  m_Gui->Label("");
  m_Gui->Label("  RayCast Pipe  ","",true);
  m_Gui->Label("");
  m_Gui->Label("");
  m_Gui->FloatSlider(ID_OPACITY_SLIDERS,"Muscle",&m_MuscleOpacity,0.0,1.0);
  m_Gui->FloatSlider(ID_OPACITY_SLIDERS,"Blood",&m_BloodOpacity,0.0,1.0);
  m_Gui->FloatSlider(ID_OPACITY_SLIDERS,"Bone",&m_BoneOpacity,0.0,1.0);
  
	return m_Gui;
}
//----------------------------------------------------------------------------
void medPipeRayCast::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId()) 
		{
    case ID_OPACITY_SLIDERS:
			{
        //Update functions
				SetRayCastFunctions();
        //generate property
        vtkMAFSmartPointer<vtkVolumeProperty> volumeProperty;
        volumeProperty->SetColor(m_ColorFunction);
        volumeProperty->SetScalarOpacity(m_OpacityFunction);
        m_Volume->SetProperty(volumeProperty);
        m_Vme->ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
				m_Gui->Update();
			}
			break;
  	  default:
			break;
		}
	}
  //Call UpdadeFromData only on non-loadings volume updates
  if(maf_event->GetId() == VME_OUTPUT_DATA_UPDATE && !m_OnLoading)
  {
      UpdateFromData();
  }
}
//----------------------------------------------------------------------------
void medPipeRayCast::UpdateFromData()
//----------------------------------------------------------------------------
{
  vtkDataSet *dataset = m_Vme->GetOutput()->GetVTKData();
  dataset->Update();

  int resampled=false;

  vtkImageData *volume;

  //volume spacing 
  double volSpacing[3];

  //If input is a Rectilinear grid this pipe need a Resample
  if (vtkRectilinearGrid *rgrid = vtkRectilinearGrid::SafeDownCast(dataset))
  {
    wxBusyCursor wait;
    wxBusyInfo wait_info(_("Resampling..."));

    resampled=true;



    vtkNEW(volume);
    vtkNEW(m_ResampleFilter);

    // the resample filter
    m_ResampleFilter->SetZeroValue(0);
    double bounds[6];
    rgrid->GetBounds(bounds);

    //Compute spacing the output spacing is the mean value of each side
    volSpacing[0] = (bounds[1]-bounds[0]) / (double) rgrid->GetXCoordinates()->GetNumberOfTuples();
    volSpacing[1] = (bounds[3]-bounds[2]) / (double) rgrid->GetYCoordinates()->GetNumberOfTuples();
    volSpacing[2] = (bounds[5]-bounds[4]) / (double) rgrid->GetZCoordinates()->GetNumberOfTuples();


    //compute Output extent 
    int output_extent[6];
    output_extent[0] = 0;
    output_extent[1] = (bounds[1] - bounds[0]) / volSpacing[0];
    output_extent[2] = 0;
    output_extent[3] = (bounds[3] - bounds[2]) / volSpacing[1];
    output_extent[4] = 0;
    output_extent[5] = (bounds[5] - bounds[4]) / volSpacing[2];

    //Setting the origin to the filter using volume bounds
    m_ResampleFilter->SetVolumeOrigin(bounds[0],bounds[2], bounds[4]);

    volume->SetSpacing(volSpacing);
    //output scalars are of the same type of input
    volume->SetScalarType(rgrid->GetPointData()->GetScalars()->GetDataType());
    volume->SetExtent(output_extent);
    volume->SetUpdateExtent(output_extent);
    volume->SetOrigin(bounds[0],bounds[2],bounds[4]);

    double sr[2];
    rgrid->GetScalarRange(sr);

    double w = sr[1] - sr[0];
    double l = (sr[1] + sr[0]) * 0.5;

    //Setting Filter parameters 
    m_ResampleFilter->SetWindow(w);
    m_ResampleFilter->SetLevel(l);
    m_ResampleFilter->SetInput(rgrid);
    m_ResampleFilter->SetOutput(volume);
    m_ResampleFilter->AutoSpacingOff();
    m_ResampleFilter->Update();
  }
  //Else if input is an Structured Point we use it directly
  else 
    volume=vtkImageData::SafeDownCast(dataset);

  wxBusyCursor wait;
  wxBusyInfo wait_info(_("Volume filtering..."));

  //RayCast Cleaner removes border effects from bones
  //(bone sanding)
  vtkNEW(m_RayCastCleaner);
  m_RayCastCleaner->SetInput(volume);
  m_RayCastCleaner->SetBloodLowerThreshold(m_BloodLowerThreshold);
  m_RayCastCleaner->SetBloodUpperThreshold(m_BloodUpperThreshold);
  m_RayCastCleaner->SetBoneLowerThreshold(m_BoneLowerThreshold);
  m_RayCastCleaner->Update();


  // Raycast mapper need unsigned short input
  // So we use a caster
  if (m_VolumeCaster==NULL)
    vtkNEW(m_VolumeCaster);
  m_VolumeCaster->SetInput(vtkImageData::SafeDownCast(m_RayCastCleaner->GetOutput()));
  m_VolumeCaster->SetClampOverflow(1);
  m_VolumeCaster->SetOutputScalarTypeToUnsignedShort();
  m_VolumeCaster->Update();

  //Deleting unnescessary stuff
  if (resampled)
  {
    vtkDEL(volume);
    vtkDEL(m_ResampleFilter);
  }
  vtkDEL(m_RayCastCleaner);


  //Create Raycast Mapper and relative functions  

  if (m_RayCastMapper==NULL)
    vtkNEW(m_RayCastMapper);
  if (m_ColorFunction==NULL)
    vtkNEW(m_ColorFunction);
  if (m_OpacityFunction==NULL)
    vtkNEW(m_OpacityFunction);

  //The ray cast function know how to render the data
  vtkMAFSmartPointer<vtkVolumeRayCastCompositeFunction> compositeFunction;
  compositeFunction->SetCompositeMethodToClassifyFirst();
  m_RayCastMapper->SetVolumeRayCastFunction(compositeFunction);
  m_RayCastMapper->SetInput(m_VolumeCaster->GetOutput());
  SetRayCastFunctions();

  // The property describes how the data will look
  vtkMAFSmartPointer<vtkVolumeProperty> volumeProperty;
  volumeProperty->SetColor(m_ColorFunction);
  volumeProperty->SetScalarOpacity(m_OpacityFunction);

  //Create a empty volume to manage the mapper
  if (m_Volume==NULL)
    vtkNEW(m_Volume);
  m_Volume->SetMapper(m_RayCastMapper);
  m_Volume->SetProperty(volumeProperty);
  m_Volume->PickableOff();
  m_AssemblyFront->AddPart(m_Volume);

}

//----------------------------------------------------------------------------
void medPipeRayCast::EnableBoundingBoxVisibility(bool enable)
//----------------------------------------------------------------------------
{
	m_BoundingBoxVisibility = enable;
}

//----------------------------------------------------------------------------
void medPipeRayCast::SetActorVisibility(int visibility)
//----------------------------------------------------------------------------
{
  //Actor visibility update
  m_Volume->SetVisibility(visibility);
  m_Volume->Modified();
}


//----------------------------------------------------------------------------
void medPipeRayCast::SetRayCastFunctions()
//----------------------------------------------------------------------------
{

  
  double bloodA,bloodB,bloodDiff,boneA,boneB,muscle,muscleA,muscleB,muscleDiff;

  //Muscle point intervals
  muscleDiff=m_MuscleUpperThreshold-m_MuscleLowerThreshold;
  muscle=m_MuscleLowerThreshold+0.5*muscleDiff;
  muscleA=m_MuscleLowerThreshold+0.4*muscleDiff;
  muscleB=m_MuscleLowerThreshold+0.7*muscleDiff;

  //Blood point intervals
  bloodDiff=m_BloodUpperThreshold-m_BloodLowerThreshold;
  bloodA=m_BloodLowerThreshold+0.333*bloodDiff;
  bloodB=m_BloodLowerThreshold+0.666*bloodDiff;

  //Bone point intervals
  boneA=1.06*m_BoneLowerThreshold;
  boneB=3.23*m_BoneLowerThreshold;

  
  //Clear previous opacity settings
  m_OpacityFunction->Initialize();

  //muscle opacity
  m_OpacityFunction->AddPoint(0, 0.0);
  m_OpacityFunction->AddPoint(m_MuscleLowerThreshold-1.0, 0.0);
  m_OpacityFunction->AddPoint(m_MuscleLowerThreshold, 0.17*m_MuscleOpacity);
  m_OpacityFunction->AddPoint(muscle, 0.2*m_MuscleOpacity);
  m_OpacityFunction->AddPoint(m_MuscleUpperThreshold, 0.17*m_MuscleOpacity);
  m_OpacityFunction->AddPoint(81, 0.0);
  //blood opacity
  m_OpacityFunction->AddPoint(m_BloodLowerThreshold, 0.0);
  //m_OpacityFunction->AddPoint(bloodA, 0.8*m_BloodOpacity);
  m_OpacityFunction->AddPoint(bloodB, 0.8*m_BloodOpacity);
  m_OpacityFunction->AddPoint(m_BloodUpperThreshold, 0.0);
  //bone opacity
  m_OpacityFunction->AddPoint(m_BoneLowerThreshold, 0.0);
  m_OpacityFunction->AddPoint(boneA, m_BoneOpacity);


  m_ColorFunction->RemoveAllPoints();

  //muscle color
  m_ColorFunction->AddRGBPoint(m_MuscleLowerThreshold, 0.7, 0.4, 0.42);
  m_ColorFunction->AddRGBPoint(muscleA, 0.57, 0.33, 0.36);
  m_ColorFunction->AddRGBPoint(muscleB, 0.3, 0.06, 0.1);
  m_ColorFunction->AddRGBPoint(m_MuscleUpperThreshold, 0.2, 0.04, 0.04);
  //blood color
  m_ColorFunction->AddRGBPoint(m_BloodLowerThreshold-1.0, 0.35, 0.07, 0.12);
  m_ColorFunction->AddRGBPoint(bloodA, 0.65, 0.07, 0.12);
  m_ColorFunction->AddRGBPoint(m_BloodUpperThreshold,0.6, 0.05, 0.32);
  //bone color
  m_ColorFunction->AddRGBPoint(m_BoneLowerThreshold-1, 0.90,0.87, 0.68);
  m_ColorFunction->AddRGBPoint(boneB, 1, 0.98, 0.95);

}



