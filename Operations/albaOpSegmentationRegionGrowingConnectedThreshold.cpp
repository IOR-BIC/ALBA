/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSegmentationRegionGrowingConnectedThreshold
 Authors: Matteo Giacomoni, Alessandro Chiarini, Grazia Di Cosmo, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaOpSegmentationRegionGrowingConnectedThreshold.h"
#include "wx/busyinfo.h"

#include "albaGUI.h"
#include "albaVME.h"
#include "albaVME.h"
#include "albaVMEVolumeGray.h"
#include "albaVMESurface.h"
#include "albaInteractorPicker.h"
#include "albaTagArray.h"

#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkImageCast.h"
#include "vtkPoints.h"
#include "vtkSphereSource.h"
#include "itkVTKImageToImageFilter.h"
#include "itkImage.h"
#include "itkImageToVTKImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"
#include "vtkALBASmartPointer.h"
#include "vtkImageToStructuredPoints.h"
#include "albaOpVolumeResample.h"
#include "vtkRectilinearGrid.h"
#include "vtkALBAVolumeToClosedSmoothSurface.h"


#define ITK_IMAGE_DIMENSION 3
#define SPACING_PERCENTAGE_BOUNDS 0.1
typedef  itk::Image< float, ITK_IMAGE_DIMENSION> RealImage;
using namespace std;

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpSegmentationRegionGrowingConnectedThreshold);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpSegmentationRegionGrowingConnectedThreshold::albaOpSegmentationRegionGrowingConnectedThreshold(wxString label) : albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;
  m_InputPreserving = true;

  m_Lower = 0;
  m_Upper = 0;
  m_Replace = 255;

  m_ImageDim = ITK_IMAGE_DIMENSION;

  m_Seed = new int[m_ImageDim];
  for (int i=0;i<m_ImageDim;i++)
  {
    m_Seed[i]=0;
  }
  m_NumIter = 5;// this is used in smoothing
  m_TimeStep = 0.0125;// this is used in smoothing
  m_Conductance = 1.0; // this is used in smoothing
  m_UseSpacing = true;// this is used in smoothing

  m_Picker = NULL;
  m_OldBehavior = NULL;

  m_Sphere = NULL;
  m_SphereVTK = NULL;
  m_VolumeOut = NULL;
  m_SurfaceOut = NULL;
  m_Resample=NULL;
  m_ResampleInput= NULL;

  m_SeedScalarValue = "";

  m_VolumeBounds[0] = m_VolumeBounds[1] = m_VolumeBounds[2] = \
  m_VolumeBounds[3] = m_VolumeBounds[4] = m_VolumeBounds[5] = 0;

  m_VolumeSpacing[0] = m_VolumeSpacing[1] = m_VolumeSpacing[2] = 1;
}
//----------------------------------------------------------------------------
albaOpSegmentationRegionGrowingConnectedThreshold::~albaOpSegmentationRegionGrowingConnectedThreshold( ) 
//----------------------------------------------------------------------------
{
  delete []m_Seed;
  albaDEL(m_VolumeOut);
  albaDEL(m_SurfaceOut);
  albaDEL(m_Picker);
  vtkDEL(m_SphereVTK);
}
//----------------------------------------------------------------------------
bool albaOpSegmentationRegionGrowingConnectedThreshold::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  // accepts albaVMEVolumeGray data
  return  (node->IsA("albaVMEVolumeGray")) && node->GetOutput()->GetVTKData();
}
//----------------------------------------------------------------------------
albaOp* albaOpSegmentationRegionGrowingConnectedThreshold::Copy()   
//----------------------------------------------------------------------------
{
  return new albaOpSegmentationRegionGrowingConnectedThreshold(m_Label);
}
//----------------------------------------------------------------------------
void albaOpSegmentationRegionGrowingConnectedThreshold::OpRun()   
//----------------------------------------------------------------------------
{
  CreateGui();
  if (CreateResample())
  {
    
	  m_OldBehavior = m_ResampleInput->GetBehavior();

	  m_Picker = albaInteractorPicker::New();
	  m_Picker->SetListener(this);
	  m_ResampleInput->SetBehavior(m_Picker);

	  albaNEW(m_Sphere);
	  m_Sphere->GetTagArray()->SetTag(albaTagItem("VISIBLE_IN_THE_TREE", 0.0));
	  m_Sphere->SetVisibleToTraverse(false);
	  m_Sphere->ReparentTo(m_ResampleInput->GetParent());

	  vtkNEW(m_SphereVTK);
    double bounds[6];m_ResampleInput->GetOutput()->GetBounds(bounds);
	  m_SphereVTK->SetRadius((bounds[5]-bounds[4])/256);
    albaLogMessage("Sphere radius = %f",(bounds[5]-bounds[4])/256);
	  m_SphereVTK->Update();

	  m_Sphere->SetData(m_SphereVTK->GetOutput(),0.0);
	  m_Sphere->Update(); 
  }
  else
    OpStop(OP_RUN_CANCEL);
}
//----------------------------------------------------------------------------
void albaOpSegmentationRegionGrowingConnectedThreshold::OpDo()
//----------------------------------------------------------------------------
{
  if(m_Output)
  {
    m_Output->ReparentTo(m_ResampleInput);
    GetLogicManager()->VmeShow(m_Output, true);
		GetLogicManager()->CameraUpdate();
  }
}
//----------------------------------------------------------------------------
void albaOpSegmentationRegionGrowingConnectedThreshold::CreateGui()   
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUI(this);

  m_Gui->Label(_("Parameters"), true);
  m_Gui->Integer(ID_LOWER,_("Lower"),&m_Lower,MINLONG,MAXLONG,_("Minimum intensity value to be included in segmentation"));
  m_Gui->Integer(ID_UPPER,_("Upper"),&m_Upper,MINLONG,MAXLONG,_("Maximum intensity value to be included in segmentation"));
  m_Gui->Integer(ID_REPLACE,_("Replace"),&m_Replace,0,MAXINT,_("Segmented region value"));
  m_Gui->Label(_("Voxel value:"),&m_SeedScalarValue, true, false, false, 0.45);
  m_Gui->VectorN(ID_SEED,_("Seed"),m_Seed,m_ImageDim,MININT,MAXINT,"Seed point to start growing, in image coordinates");

//   m_Gui->Label(_("Anisotropic curvature diffusion parameters"));
//   m_Gui->Integer(ID_ITERATIONS,_("iterations"),&m_NumIter,0,MAXINT,_("number of iterations. Default [5]"));
//   m_Gui->Float(ID_TIME_STEP,_("time step"),&m_TimeStep,0,MAXFLOAT,0,-1,_("time step. Default [0.0125]"));
//   m_Gui->Float(ID_CONDUCTANCE,_("conductance"),&m_Conductance,MINFLOAT,MAXFLOAT,0,-1,_("conductance. Default [1.0]"));
//   m_Gui->Bool(ID_USE_SPACING,_("use spacing"),&m_UseSpacing,1,_("do NOT use image spacing when computing filter"));

//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	ShowGui();

  ShowGui();
}
//----------------------------------------------------------------------------
void albaOpSegmentationRegionGrowingConnectedThreshold::OpStop(int result)   
//----------------------------------------------------------------------------
{  
  if (result==OP_RUN_OK)
  {
    GetLogicManager()->VmeShow(m_Sphere, false);
    m_Sphere->ReparentTo(NULL);

		m_ResampleInput->SetBehavior(m_OldBehavior);
    m_ResampleInput->Update();

    HideGui();
    albaEventMacro(albaEvent(this,result));
  } 
  else if (result==OP_RUN_CANCEL)
  { 
    if(m_Sphere!=NULL )
    {
      GetLogicManager()->VmeShow(m_Sphere, false);
      m_Sphere->ReparentTo(NULL);
  
			m_ResampleInput->SetBehavior(m_OldBehavior);
      m_ResampleInput->Update();
    }
    
    if(m_ResampleInput!=NULL && m_ResampleInput!=m_Input)
    {
      GetLogicManager()->VmeShow(m_ResampleInput, false);
      albaEventMacro(albaEvent(this,VME_SELECT,m_ResampleInput,false)); 
      albaEventMacro(albaEvent(this, VME_SELECT, m_Input, true));
      GetLogicManager()->VmeShow(m_Input, true);
      m_ResampleInput->ReparentTo(NULL);
      m_ResampleInput->Update();
    }
    albaDEL(m_Resample); 
    HideGui();
    albaEventMacro(albaEvent(this,result));
  }
}
//----------------------------------------------------------------------------
void albaOpSegmentationRegionGrowingConnectedThreshold::Algorithm()   
//----------------------------------------------------------------------------
{
  wxBusyInfo *info;
  wxBusyCursor *wait;

  if (!m_TestMode)
  {
    wait = new wxBusyCursor;
    info = new wxBusyInfo("Please wait");
  }

  //in test mode resample is not created before algorithm call
  if (m_TestMode) CreateResample();

 typedef itk::ConnectedThresholdImageFilter<RealImage, RealImage> ITKConnectedThresholdFilter;
  ITKConnectedThresholdFilter::Pointer connectedThreshold = ITKConnectedThresholdFilter::New();

  vtkImageData *im = vtkImageData::SafeDownCast(albaVMEVolumeGray::SafeDownCast(m_ResampleInput)->GetOutput()->GetVTKData());
  
  vtkALBASmartPointer<vtkImageCast> vtkImageToFloat;
  vtkImageToFloat->SetOutputScalarTypeToFloat ();
  vtkImageToFloat->SetInput(im);
  vtkImageToFloat->Modified();
  vtkImageToFloat->Update();

  typedef itk::VTKImageToImageFilter< RealImage > ConvertervtkTOitk;
  ConvertervtkTOitk::Pointer vtkTOitk = ConvertervtkTOitk::New();
	vtkTOitk->SetInput( vtkImageToFloat->GetOutput() );
	vtkTOitk->Update();

  connectedThreshold->SetLower(m_Lower);
  connectedThreshold->SetUpper(m_Upper);
  connectedThreshold->SetReplaceValue(m_Replace);

  RealImage::IndexType seed;
  for (int i=0;i<m_ImageDim;i++)
  {
    seed[i]=m_Seed[i];
  }
  connectedThreshold->AddSeed(seed);
  //ConnectedThresholdParams->seed = seedGeo;

  //ConnectedThresholdParams->numberOfIterations = m_NumIter;
  //ConnectedThresholdParams->timeStep = m_TimeStep;
  //ConnectedThresholdParams->conductance = m_Conductance;

  if (m_UseSpacing == true)
  {
    //ConnectedThresholdParams->useImageSpacing = true;
  } 
  else
  {
    //ConnectedThresholdParams->useImageSpacing = false;
  }

  //connectedThreshold->SetParameters( static_cast<evoConnectedThresholdParameters::ConstPointer> (ConnectedThresholdParams) );

  connectedThreshold->SetInput( ((RealImage*)vtkTOitk->GetOutput()) );

  try
  {
    connectedThreshold->Update();
  }
  catch ( itk::ExceptionObject &err )
  {
    std::cout << "ExceptionObject caught !" << std::endl; 
    std::cout << err << std::endl; 
  }

  typedef itk::ImageToVTKImageFilter< RealImage > ConverteritkTOvtk;
  ConverteritkTOvtk::Pointer itkTOvtk = ConverteritkTOvtk::New();
  itkTOvtk->SetInput( connectedThreshold->GetOutput() );
  itkTOvtk->Update();

  albaNEW(m_VolumeOut);
  m_VolumeOut->SetName("Connected Threshold");

  vtkImageData *image = ((vtkImageData*)itkTOvtk->GetOutput());
  image->Update();


  vtkALBASmartPointer<vtkImageToStructuredPoints> image_to_sp;
  image_to_sp->SetInput(image);
  image_to_sp->Update();
  m_VolumeOut->SetData((vtkImageData*)image_to_sp->GetOutput(),m_ResampleInput->GetTimeStamp());

  albaTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("SYNTHETIC");

  m_VolumeOut->GetTagArray()->SetTag(tag_Nature);

  m_VolumeOut->Update();

  m_VolumeOut->GetTagArray()->SetTag(albaTagItem("VOLUME_TYPE","BINARY"));

  
  vtkALBASmartPointer<vtkALBAVolumeToClosedSmoothSurface> volToSurface;
  volToSurface->SetInput(m_VolumeOut->GetOutput()->GetVTKData());
  volToSurface->SetContourValue(127.5);
  volToSurface->Update();
  vtkPolyData *surface=volToSurface->GetOutput();

  //Generating Surface VME
  albaNEW(m_SurfaceOut);
  m_SurfaceOut->SetName("Connected Threshold Surface");
  m_SurfaceOut->SetData(surface,albaVMEVolumeGray::SafeDownCast(m_ResampleInput)->GetTimeStamp());
  m_SurfaceOut->ReparentTo(m_ResampleInput);
  m_SurfaceOut->Modified();
  m_SurfaceOut->Update();

  vtkDEL(surface);
  
  //Volume output is a child of surface out
  //The result tree is Input
  //                     |-Surface
  //                          |-Binary volume
  m_VolumeOut->ReparentTo(m_SurfaceOut);

  m_Output=m_SurfaceOut;

  if(!m_TestMode)
  {
    delete wait;
    delete info;
  }

 
}
//----------------------------------------------------------------------------
void albaOpSegmentationRegionGrowingConnectedThreshold::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
    case ID_SEED:
      {
        double b[6],spacing[3],origin[3];
        albaVMEVolumeGray::SafeDownCast(m_ResampleInput)->GetOutput()->GetBounds(b);

        vtkImageData *sp = vtkImageData::SafeDownCast(albaVMEVolumeGray::SafeDownCast(m_ResampleInput)->GetOutput()->GetVTKData());
        sp->Update();

        sp->GetSpacing(spacing);
        sp->GetOrigin(origin);
        
        double pos[3];
        for (int i=0;i<3;i++)
        {
        	pos[i] = (m_Seed[i]*spacing[i])+origin[i]+b[i*2];
        }

        m_SphereVTK->SetCenter(pos);
        m_SphereVTK->Update();

        m_Sphere->SetData(m_SphereVTK->GetOutput(),0.0);
        m_Sphere->Update();

				GetLogicManager()->CameraUpdate();
      }
      break;
    case VME_PICKED:
      {
      	vtkPoints *pts = vtkPoints::SafeDownCast(e->GetVtkObj());
      	if (pts)
      	{
          double position[3];
      	  pts->GetPoint(0,position);

          m_SphereVTK->SetCenter(position);
          m_SphereVTK->Update();

          m_Sphere->SetData(m_SphereVTK->GetOutput(),0.0);
          m_Sphere->Update();

          GetLogicManager()->VmeShow(m_Sphere, true);
					GetLogicManager()->CameraUpdate();

          vtkImageData *sp = vtkImageData::SafeDownCast(albaVMEVolumeGray::SafeDownCast(m_ResampleInput)->GetOutput()->GetVTKData());
          sp->Update();

          int id;
          id = e->GetArg();
          double point[3],spacing[3],origin[3];
          sp->GetOrigin(origin);
          sp->GetPoint(id,point);
          sp->GetSpacing(spacing);
          int ix = (point[0]-origin[0])/spacing[0];
          int iy = (point[1]-origin[1])/spacing[1];
          int iz = (point[2]-origin[2])/spacing[2];

          m_Seed[0] = ix;
          m_Seed[1] = iy;
          m_Seed[2] = iz;

          double scalarValue = e->GetDouble();
          m_SeedScalarValue = scalarValue;
          m_Lower = scalarValue - 64;
          m_Upper = scalarValue + 64;

      	  m_Gui->Update();
      	}
      }

      break;
    case wxOK:
      {
        Algorithm();
        OpStop(OP_RUN_OK);
      }
      break;
    case wxCANCEL:
      {
        OpStop(OP_RUN_CANCEL);
      }
      break;
    default:
      albaEventMacro(*e);
      break;
    }
  }
}
//----------------------------------------------------------------------------
void albaOpSegmentationRegionGrowingConnectedThreshold::SetSeed(int *seed)
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_ImageDim;i++)
  {
    m_Seed[i]=seed[i];
  }
}
//----------------------------------------------------------------------------
void albaOpSegmentationRegionGrowingConnectedThreshold::GetSeed(int *seed)
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_ImageDim;i++)
  {
    seed[i]=m_Seed[i];
  }
}
//----------------------------------------------------------------------------
int albaOpSegmentationRegionGrowingConnectedThreshold::CreateResample()
//----------------------------------------------------------------------------
{
 
  // if the volume is a rectilinear grid we resample it 
  if(m_Input->GetOutput()->GetVTKData()->IsA("vtkRectilinearGrid"))
  { 
    wxBusyInfo *info;
    wxBusyCursor *wait;

    m_Resample=new albaOpVolumeResample();
    m_Resample->SetInput(m_Input);
    m_Resample->TestModeOn();
    m_Resample->AutoSpacing();
    m_Resample->GetSpacing(m_VolumeSpacing);
      
    m_Input->GetOutput()->GetVMELocalBounds(m_VolumeBounds);
    m_Resample->SetBounds(m_VolumeBounds,albaOpVolumeResample::CUSTOMBOUNDS);
          
    if (!CheckSpacing())
    {
      int answer = wxMessageBox( "Spacing values are too little and could generate memory problems - Continue?", "Warning", wxYES_NO, NULL);
      if (answer == wxNO)
      {
        return false;
      }
    }
    if(!m_TestMode)
    {
      wait = new wxBusyCursor;
      info = new wxBusyInfo("Waiting for volume to be resampled");
    }
    
    m_Resample->Resample();
     
    albaVME *Output = m_Resample->GetOutput();
    Output->GetOutput()->GetVTKData()->Update();
    m_ResampleInput=albaVMEVolumeGray::SafeDownCast(Output);
    m_ResampleInput->Update();

    // show volume resampled
    GetLogicManager()->VmeShow(m_ResampleInput, true);
    albaEventMacro(albaEvent(this, VME_SELECT, m_ResampleInput, true));
		GetLogicManager()->CameraUpdate();
     
    if(!m_TestMode)
    {
      delete wait;
      delete info;
    }
  }
  else 
  {
    m_ResampleInput=albaVMEVolumeGray::SafeDownCast(m_Input);
  }
  return true;

}
//----------------------------------------------------------------------------
bool albaOpSegmentationRegionGrowingConnectedThreshold::CheckSpacing()
//----------------------------------------------------------------------------
{
  if ((m_VolumeSpacing[0]/(m_VolumeBounds[1] - m_VolumeBounds[0]))*100 < SPACING_PERCENTAGE_BOUNDS)
  {
    return false;
  }
  if ((m_VolumeSpacing[1]/(m_VolumeBounds[3] - m_VolumeBounds[2]))*100 < SPACING_PERCENTAGE_BOUNDS)
  {
    return false;
  }
  if ((m_VolumeSpacing[2]/(m_VolumeBounds[5] - m_VolumeBounds[4]))*100 < SPACING_PERCENTAGE_BOUNDS)
  {
    return false;
  }

  return true;
}

