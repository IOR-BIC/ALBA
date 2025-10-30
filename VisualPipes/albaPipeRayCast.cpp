/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeRayCast
 Authors: Gianluigi Crimi  -  Grazia Di Cosmo
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#if defined(WIN32)
#pragma warning (disable : 4018)
#endif

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaPipeRayCast.h"
#include "albaEvent.h"
#include "albaSceneNode.h"
#include "albaGUIFloatSlider.h"
#include "albaGUI.h"

#include "albaVME.h"
#include "albaVMEVolumeGray.h"

#include "albaGUIBusyInfo.h"

#include "vtkALBAAssembly.h"
#include "vtkALBASmartPointer.h"
#include "vtkActor.h"
#include "vtkVolume.h"
#include "vtkDataSet.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkGPUVolumeRayCastMapper.h"
#include "vtkVolumeProperty.h"
#include "vtkImageCast.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkALBARayCastCleaner.h"
#include "vtkALBAVolumeResample.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"

#include "vtkProperty.h"
#include "vtkPolyDataMapper.h"
#include "vtkOutlineCornerFilter.h"
#include "albaTagArray.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeRayCast);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeRayCast::albaPipeRayCast(double skinOpacity,double fatMassOpacity,double muscleOpacity,double bloodOpacity,double boneOpacity) 
: albaPipe()
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

  m_RayCastCleaner  = NULL;

  //Setting Opacity 
  //Default values 0.15 - 0.8 - 0.2  
  m_SkinOpacity=skinOpacity;
  m_FatMassOpacity=fatMassOpacity;
  m_MuscleOpacity=muscleOpacity;
  m_BloodOpacity=bloodOpacity;
  m_BoneOpacity=boneOpacity;
  m_BloodFocus=0.5;
  
  m_OnLoading=false;

  m_BoundingBoxVisibility = true;

  //Set Layers
  m_Preset=0;

  //Set TestMode
  m_TestMode=false;
}
//----------------------------------------------------------------------------
void albaPipeRayCast::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{
	Superclass::Create(n);

	assert(m_Vme->GetOutput()->IsA("albaVMEOutputVolume"));

  m_Vme->AddObserver(this);

  vtkDataSet *dataset = m_Vme->GetOutput()->GetVTKData();
  
  //If the volume is not loaded this dataset->update() calls update-event
  //in this case we disable temporary the call to UpdateFromData() in order
  //to have a single call to that function
  //if the volume is already loaded m_Onloading changes simple do nothing
  m_OnLoading=true;
  m_OnLoading=false;


  //Range is used for piecewise function shifts 
  dataset->GetScalarRange(m_ScalarRange);
  
  DetectModality();

  SetThresholding();

  UpdateFromData();
    
  // selection box
  vtkNEW(m_OutlineBox);
  m_OutlineBox->SetInputData(dataset);

  vtkNEW(m_OutlineMapper);
  m_OutlineMapper->SetInputConnection(m_OutlineBox->GetOutputPort());

  vtkNEW(m_OutlineActor);
  m_OutlineActor->SetMapper(m_OutlineMapper);
  m_OutlineActor->VisibilityOn();
  m_OutlineActor->PickableOff();

  //box property
  vtkALBASmartPointer<vtkProperty> property;
  property->SetColor(1,1,1);
  property->SetAmbient(1);
  property->SetRepresentationToWireframe();
  property->SetInterpolationToFlat();
  m_OutlineActor->SetProperty(property);

  if(m_BoundingBoxVisibility)
    m_AssemblyFront->AddPart(m_OutlineActor);
}
//----------------------------------------------------------------------------
albaPipeRayCast::~albaPipeRayCast()
//----------------------------------------------------------------------------
{

  //Removing pipe binding
  m_Vme->RemoveObserver(this);

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
  vtkDEL(m_RayCastCleaner);

}

//----------------------------------------------------------------------------
void albaPipeRayCast::Select(bool sel) 
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
albaGUI *albaPipeRayCast::CreateGui()
//----------------------------------------------------------------------------
{
	
	assert(m_Gui == NULL);
	m_Gui = new albaGUI(this);
	
  m_CTSliders = new albaGUI(this);
  m_MRSliders = new albaGUI(this);

  //Creating Gui
  m_Gui->Label("");
  m_Gui->Label(" Layers ", "", true );
  m_Gui->Divider(2);
  //Set Layer type to visualize
  wxString layersType[4] = {"Default", "Muscular System", "Circulatory System", "Skeleton"};
  m_Gui->Combo(ID_CHANGE_OPACITY, "", &m_Preset, 4, layersType);

  //Advanced 
  m_Gui->Label("");
  m_Gui->Label(" Advanced ", "", true );
  m_Gui->Divider(2);
  
  wxString modalities[2] = {"CT view", "MR view"};
  m_Gui->Combo(ID_CHANGE_MODALITY, "", &m_Modality, 2, modalities);
	
	m_Gui->Label("");

  m_CTSliders->FloatSlider(ID_OPACITY_SLIDERS,"Skin",&m_SkinOpacity,0.0,1.0);
  m_CTSliders->FloatSlider(ID_OPACITY_SLIDERS,"Fat Mass",&m_FatMassOpacity,0.0,1.0);
  m_Gui->Add(m_CTSliders);
  m_CTSliders->Update();
  m_Gui->FloatSlider(ID_OPACITY_SLIDERS,"Muscle",&m_MuscleOpacity,0.0,1.0);
  m_Gui->FloatSlider(ID_OPACITY_SLIDERS,"Bone",&m_BoneOpacity,0.0,1.0);
  m_Gui->FloatSlider(ID_OPACITY_SLIDERS,"Blood",&m_BloodOpacity,0.0,1.0);
  m_Gui->Divider(1);
  m_MRSliders->FloatSlider(ID_OPACITY_SLIDERS,"Blood focus",&m_BloodFocus,0.0,1.0);
  m_Gui->Add(m_MRSliders);
  m_MRSliders->Update();
  m_Gui->Label("");
  m_Gui->Label("");
  m_Gui->Label("");

  ShowHideSliders();

	return m_Gui;
}
//----------------------------------------------------------------------------
void albaPipeRayCast::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch(e->GetId()) 
		{
    case ID_OPACITY_SLIDERS:
			{
        //Update functions
				SetRayCastFunctions();
        m_Vme->ForwardUpEvent(&albaEvent(this,CAMERA_UPDATE));
				m_Gui->Update();
			}
		break;
    case ID_CHANGE_OPACITY:
      {
        OnPreset();

        m_Gui->Update();

        //Update functions
        SetRayCastFunctions();
        m_Vme->ForwardUpEvent(&albaEvent(this,CAMERA_UPDATE));
      }
    break;
    case ID_CHANGE_MODALITY:
      {
        OnChangeModality();
        m_Vme->ForwardUpEvent(&albaEvent(this,CAMERA_UPDATE));
      }
    break;
  	default:
		break;
		}
	}
  //Call UpdadeFromData only on non-loadings volume updates
  if(alba_event->GetId() == VME_OUTPUT_DATA_UPDATE && !m_OnLoading)
  {
      UpdateFromData();
  }
}

//----------------------------------------------------------------------------
void albaPipeRayCast::SetThresholding()
//----------------------------------------------------------------------------
{
 
  if (m_Modality == CT_MODALITY)
  {  
    //Thresholding values
    //Skin
    m_SkinLowerThreshold=-800.0;
    m_SkinUpperThreshold=-180.0;  
    //Fat Mass
    m_FatMassLowerThreshold=-150;
    m_FatMassUpperThreshold=10;
    //Muscles
    m_MuscleLowerThreshold=10.0;
    m_MuscleUpperThreshold=80.0;
    //Blood
    m_BloodLowerThreshold=200.0;
    m_BloodUpperThreshold=340.0;
    //Bones
    m_BoneLowerThreshold=350.0;
  }
  else 
  {
    //Muscles
    m_MuscleLowerThreshold=40.0;
    m_MuscleUpperThreshold=150.0;
    //Blood
    m_BloodLowerThreshold=155.0;
    m_BloodUpperThreshold=520.0;
    //Bones
    m_BoneLowerThreshold=16.0;
    m_BoneUpperThreshold=22.0;
  }
}
//----------------------------------------------------------------------------
void albaPipeRayCast::UpdateFromData()
//----------------------------------------------------------------------------
{
  vtkALBAVolumeResample		 *resampleFilter;	

  vtkDataSet *dataset = m_Vme->GetOutput()->GetVTKData();

  int resampled=false;

  //volume spacing 
  double volSpacing[3];

  //If input is a Rectilinear grid this pipe need a Resample
  if (vtkRectilinearGrid *rgrid = vtkRectilinearGrid::SafeDownCast(dataset))
  {
		albaGUIBusyInfo busy(_("Resampling..."));
    wxBusyCursor *wait=NULL;
		if (!m_TestMode)
      wait = new wxBusyCursor;
      
    
    resampled=true;
    vtkNEW(resampleFilter);

    // the resample filter
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

    double sr[2];
    rgrid->GetScalarRange(sr);

    double w = sr[1] - sr[0];
    double l = (sr[1] + sr[0]) * 0.5;

    //Setting Filter parameters 
		resampleFilter->SetZeroValue(0);
    resampleFilter->SetWindow(w);
    resampleFilter->SetLevel(l);
		resampleFilter->SetVolumeOrigin(bounds[0], bounds[2], bounds[4]);
		resampleFilter->SetOutputSpacing(volSpacing);
    resampleFilter->SetInputData(rgrid);
		resampleFilter->SetOutputExtent(output_extent);
    resampleFilter->AutoSpacingOff();
    resampleFilter->Update();

		cppDEL(wait);
  }
 

  wxBusyCursor *wait=NULL;

  if (!m_TestMode)
    wait = new wxBusyCursor;
  
  //RayCast Cleaner removes border effects from bones
  //(bone sanding) and produces in output a volume whit unsigned short 
  //scalars shifted by - lower range 
  if (m_RayCastCleaner==NULL)
    vtkNEW(m_RayCastCleaner);

	if (resampled)
		m_RayCastCleaner->SetInputConnection(resampleFilter->GetOutputPort());
	else
		m_RayCastCleaner->SetInputData(dataset);
  m_RayCastCleaner->SetBloodLowerThreshold(m_BloodLowerThreshold);
  m_RayCastCleaner->SetBloodUpperThreshold(m_BloodUpperThreshold);
  m_RayCastCleaner->SetBoneLowerThreshold(m_BoneLowerThreshold);
  if (m_Modality==CT_MODALITY)
    m_RayCastCleaner->SetModalityToCT();
  else 
    m_RayCastCleaner->SetModalityToMR();
  m_RayCastCleaner->Update();

  //Deleting unnecessary stuff
  if (resampled)
    vtkDEL(resampleFilter);

  //Create Raycast Mapper and relative functions
  if (m_RayCastMapper == NULL)
    m_RayCastMapper = vtkGPUVolumeRayCastMapper::New();
  if (m_ColorFunction==NULL)
    vtkNEW(m_ColorFunction);
  if (m_OpacityFunction==NULL)
    vtkNEW(m_OpacityFunction);

  m_RayCastMapper->SetInputConnection(m_RayCastCleaner->GetOutputPort());
  
  //Create a empty volume to manage the mapper
  if (m_Volume==NULL)
    vtkNEW(m_Volume);
  m_Volume->SetMapper(m_RayCastMapper);
  SetRayCastFunctions();
  m_Volume->PickableOff();
  m_AssemblyFront->AddPart(m_Volume);

  cppDEL(wait);
}

//----------------------------------------------------------------------------
void albaPipeRayCast::EnableBoundingBoxVisibility(bool enable)
//----------------------------------------------------------------------------
{
	m_BoundingBoxVisibility = enable;
}

//----------------------------------------------------------------------------
void albaPipeRayCast::SetActorVisibility(int visibility)
//----------------------------------------------------------------------------
{
  //Actor visibility update
  m_Volume->SetVisibility(visibility);
  m_Volume->Modified();
}


//Shift the scalar value from range [x,y] to [0,y-x]
#define scalarShift(X) (X-m_ScalarRange[0])

//----------------------------------------------------------------------------
void albaPipeRayCast::SetRayCastFunctions()
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
  m_ColorFunction->RemoveAllPoints();

  if (m_Modality ==  CT_MODALITY )
  {  
    /////////////////CT OPACITTY/COLOR VALUES////////////////
    //skin opacity
    m_OpacityFunction->AddPoint(scalarShift(m_SkinLowerThreshold-1.0)   , 0.0);
    m_OpacityFunction->AddPoint(scalarShift(m_SkinLowerThreshold)       , 0.2*m_SkinOpacity);
    m_OpacityFunction->AddPoint(scalarShift(m_SkinUpperThreshold)       , 0.2*m_SkinOpacity);
    m_OpacityFunction->AddPoint(scalarShift(m_SkinUpperThreshold+1)     , 0.0);
    //fat mass opacity
    m_OpacityFunction->AddPoint(scalarShift(m_FatMassLowerThreshold-1.0)   , 0.0);
    m_OpacityFunction->AddPoint(scalarShift(m_FatMassLowerThreshold)       , 0.1*m_FatMassOpacity);
    m_OpacityFunction->AddPoint(scalarShift(m_FatMassUpperThreshold)       , 0.1*m_FatMassOpacity);
    m_OpacityFunction->AddPoint(scalarShift(m_FatMassUpperThreshold+1)     , 0.0);
    //muscle opacity
    m_OpacityFunction->AddPoint(scalarShift(0)                          , 0.0);
    m_OpacityFunction->AddPoint(scalarShift(m_MuscleLowerThreshold-1.0) , 0.0);
    m_OpacityFunction->AddPoint(scalarShift(m_MuscleLowerThreshold)     , 0.13*m_MuscleOpacity);
    m_OpacityFunction->AddPoint(scalarShift(muscle)                     , 0.15*m_MuscleOpacity);
    m_OpacityFunction->AddPoint(scalarShift(m_MuscleUpperThreshold)     , 0.13*m_MuscleOpacity);
    m_OpacityFunction->AddPoint(scalarShift(m_MuscleUpperThreshold+1)   , 0.0);
    //blood opacity
    m_OpacityFunction->AddPoint(scalarShift(m_BloodLowerThreshold)      , 0.0);
    m_OpacityFunction->AddPoint(scalarShift(bloodB)                     , 0.8*m_BloodOpacity);
    m_OpacityFunction->AddPoint(scalarShift(m_BloodUpperThreshold)      , 0.0);
    //bone opacity
    m_OpacityFunction->AddPoint(scalarShift(m_BoneLowerThreshold)       , 0.0);
    m_OpacityFunction->AddPoint(scalarShift(boneA)                      , m_BoneOpacity);

    //skin color
    m_ColorFunction->AddRGBPoint(scalarShift(m_SkinLowerThreshold)      , 0.80, 0.52, 0.26);
    m_ColorFunction->AddRGBPoint(scalarShift(m_SkinUpperThreshold)      , 0.80, 0.52, 0.26);
    //fat mass color
    m_ColorFunction->AddRGBPoint(scalarShift(m_FatMassLowerThreshold)   , 1.00, 0.98, 0.87);
    m_ColorFunction->AddRGBPoint(scalarShift(m_FatMassUpperThreshold)   , 1.00, 0.98, 0.87);
    //muscle color
    m_ColorFunction->AddRGBPoint(scalarShift(m_MuscleLowerThreshold)    , 0.70, 0.40, 0.42);
    m_ColorFunction->AddRGBPoint(scalarShift(muscleA)                   , 0.57, 0.33, 0.36);
    m_ColorFunction->AddRGBPoint(scalarShift(muscleB)                   , 0.30, 0.06, 0.1);
    m_ColorFunction->AddRGBPoint(scalarShift(m_MuscleUpperThreshold)    , 0.20, 0.04, 0.04);
    //blood color
    m_ColorFunction->AddRGBPoint(scalarShift(m_BloodLowerThreshold-1.0) , 0.35, 0.07, 0.12);
    m_ColorFunction->AddRGBPoint(scalarShift(bloodA)                    , 0.65, 0.07, 0.12);
    m_ColorFunction->AddRGBPoint(scalarShift(m_BloodUpperThreshold)     , 0.60, 0.05, 0.32);
    //bone color
    m_ColorFunction->AddRGBPoint(scalarShift(m_BoneLowerThreshold-1)    , 0.90, 0.87, 0.68);
    m_ColorFunction->AddRGBPoint(scalarShift(boneB)                     , 1.00, 0.98, 0.95);
  }
  else
  {
    /////////////////MR OPACITTY/COLOR VALUES////////////////
    //bone opacity
    m_OpacityFunction->AddPoint(scalarShift(0)       , 0.0);
    m_OpacityFunction->AddPoint(scalarShift(m_BoneLowerThreshold-1)     , 0.0);
    m_OpacityFunction->AddPoint(scalarShift(m_BoneLowerThreshold)       , 0.07*m_BoneOpacity);
    m_OpacityFunction->AddPoint(scalarShift(m_BoneUpperThreshold)       , 0.1*m_BoneOpacity);
    m_OpacityFunction->AddPoint(scalarShift(m_BoneLowerThreshold+1)     , 0);
    //muscle opacity
    m_OpacityFunction->AddPoint(scalarShift(0)                          , 0.0);
    m_OpacityFunction->AddPoint(scalarShift(m_MuscleLowerThreshold-1.0) , 0.0);
    m_OpacityFunction->AddPoint(scalarShift(m_MuscleLowerThreshold)     , 0.04*m_MuscleOpacity);
    m_OpacityFunction->AddPoint(scalarShift(muscle)                     , 0.06*m_MuscleOpacity);
    m_OpacityFunction->AddPoint(scalarShift(m_MuscleUpperThreshold)     , 0.04*m_MuscleOpacity);
    m_OpacityFunction->AddPoint(scalarShift(m_MuscleUpperThreshold+1)   , 0.0);
    //blood opacity
    m_OpacityFunction->AddPoint(scalarShift(m_BloodLowerThreshold)      , 0.0);
    m_OpacityFunction->AddPoint(scalarShift(m_BloodLowerThreshold+0.1*m_BloodLowerThreshold)  , 0.20*m_BloodOpacity*(1.0-m_BloodFocus));
    m_OpacityFunction->AddPoint(scalarShift(bloodA)  , 0.60*m_BloodOpacity*(1.0-m_BloodFocus*0.7));
    m_OpacityFunction->AddPoint(scalarShift(bloodB)  , m_BloodOpacity*m_BloodFocus);
    m_OpacityFunction->AddPoint(scalarShift(0.9*m_BloodUpperThreshold)  , m_BloodOpacity*m_BloodFocus);
    m_OpacityFunction->AddPoint(scalarShift(m_BloodUpperThreshold)      , 0.17*m_BloodOpacity*m_BloodFocus);
    m_OpacityFunction->AddPoint(scalarShift(m_BloodUpperThreshold+1)   , 0.0);
    
    
    //bone color
    m_ColorFunction->AddRGBPoint(scalarShift(m_BoneLowerThreshold-1)    , 0.90, 0.87, 0.68);
    m_ColorFunction->AddRGBPoint(scalarShift(m_BoneUpperThreshold)      , 1.00, 0.98, 0.95);
    //muscle color
    muscleA=m_MuscleLowerThreshold+0.3*muscleDiff;
    muscleB=m_MuscleLowerThreshold+0.5*muscleDiff;
    m_ColorFunction->AddRGBPoint(scalarShift(m_MuscleLowerThreshold)    , 0.70, 0.40, 0.42);
    m_ColorFunction->AddRGBPoint(scalarShift(muscleA)                   , 0.57, 0.33, 0.36);
    m_ColorFunction->AddRGBPoint(scalarShift(muscleB)                   , 0.30, 0.06, 0.1);
    m_ColorFunction->AddRGBPoint(scalarShift(m_MuscleUpperThreshold)    , 0.20, 0.04, 0.04);
    //blood color
    m_ColorFunction->AddRGBPoint(scalarShift(m_BloodLowerThreshold-1.0) ,  1.00, 0.88, 0.85);
    m_ColorFunction->AddRGBPoint(scalarShift(m_BloodLowerThreshold+0.1*m_BloodLowerThreshold),0.90, 0.84, 0.64); 
    m_ColorFunction->AddRGBPoint(scalarShift(bloodA)                    , 0.65, 0.07, 0.12);
    m_ColorFunction->AddRGBPoint(scalarShift(bloodB)                    , 0.35, 0.07, 0.12);
    m_ColorFunction->AddRGBPoint(scalarShift(m_BloodUpperThreshold+1.0) ,  0.90, 0.87, 0.68);
  }
    
  // The property describes how the data will look
  vtkALBASmartPointer<vtkVolumeProperty> volumeProperty;
  volumeProperty->SetColor(m_ColorFunction);
  volumeProperty->SetScalarOpacity(m_OpacityFunction);
  //Set property  
  m_Volume->SetProperty(volumeProperty);

}

//----------------------------------------------------------------------------
void albaPipeRayCast::DetectModality()
//----------------------------------------------------------------------------
{
  
  albaTagItem *item=NULL;
  wxString tagModality;

  //getting modality tags
  item = m_Vme->GetTagArray()->GetTag("Modality");
  if (item != NULL )
    tagModality = item->GetValue();

  //if modality tag is set to ct we use CT_MODALITY
  //if modality tag is set to mr we use MR_MODALITY
  //if both tags are not set we use an heuristic, 
  if (strcmp(tagModality,"CT")==0 || (strcmp(tagModality,"MR")!=0 && m_ScalarRange[0]<0) )
    m_Modality=CT_MODALITY;
  else
    m_Modality=MR_MODALITY;
}

//----------------------------------------------------------------------------
void albaPipeRayCast::OnPreset()
//----------------------------------------------------------------------------
{
  if (m_Modality==CT_MODALITY)
  {
    //Setting CT presets opacity values
    switch(m_Preset) 
    {
    case DEFAULT_PRESET:
        m_SkinOpacity = 0.2;
        m_FatMassOpacity = 0.2;
        m_MuscleOpacity=0.2;
        m_BloodOpacity=0.8;
        m_BoneOpacity=0.2;
      break;  
      case MUSCULAR_PRESET:
        m_SkinOpacity = 0.0;
        m_FatMassOpacity = 0.2;
        m_MuscleOpacity=0.8;
        m_BloodOpacity=1.0;
        m_BoneOpacity=0.5;
      break;
      case CIRCULATORY_PRESET:
        m_SkinOpacity = m_FatMassOpacity = m_MuscleOpacity = 0.0;
        m_BloodOpacity=1.0;
        m_BoneOpacity=0.5;
      break;
      case SKELETON_PRESET:
        m_SkinOpacity = m_FatMassOpacity = m_MuscleOpacity = m_BloodOpacity = 0.0;
        m_BoneOpacity=0.5;
      break;
    }
  }
  else
  {
    //Setting MR presets opacity values
    switch(m_Preset) 
    {
      case DEFAULT_PRESET:
        m_MuscleOpacity=0.25;
        m_BloodOpacity=0.4;
        m_BoneOpacity=0.1;
      break;  
      case MUSCULAR_PRESET:
        m_MuscleOpacity=0.8;
        m_BloodOpacity=0.1;
        m_BoneOpacity=0.05;
      break;
      case CIRCULATORY_PRESET:
        m_MuscleOpacity = 0.15;
        m_BloodOpacity=1.0;
        m_BoneOpacity=0.1;
      break;
      case SKELETON_PRESET:
        m_MuscleOpacity = m_BloodOpacity = 0.0;
        m_BoneOpacity=0.5;
      break;
    }
  }
}

//----------------------------------------------------------------------------
void albaPipeRayCast::ShowHideSliders()
//----------------------------------------------------------------------------
{
  m_CTSliders->Show(m_Modality==CT_MODALITY);
  m_CTSliders->FitGui();
  m_MRSliders->Show(m_Modality==MR_MODALITY);
  m_MRSliders->FitGui();
  m_Gui->FitGui();
}

//----------------------------------------------------------------------------
void albaPipeRayCast::OnChangeModality()
//----------------------------------------------------------------------------
{
  SetThresholding();
  UpdateFromData();
  OnPreset();
  ShowHideSliders();
}
