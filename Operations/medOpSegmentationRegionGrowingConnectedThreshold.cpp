/*=========================================================================

 Program: MAF2Medical
 Module: medOpSegmentationRegionGrowingConnectedThreshold
 Authors: Matteo Giacomoni, Alessandro Chiarini, Grazia Di Cosmo, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpSegmentationRegionGrowingConnectedThreshold.h"
#include "wx/busyinfo.h"

#include "mafGUI.h"
#include "mafNode.h"
#include "mafVME.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurface.h"
#include "mafInteractorPicker.h"
#include "mafTagArray.h"

#include "vtkDataSet.h"
#include "vtkStructuredPoints.h"
#include "vtkImageCast.h"
#include "vtkPoints.h"
#include "vtkSphereSource.h"
#include "itkVTKImageToImageFilter.h"
#include "itkImage.h"
#include "itkImageToVTKImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"
#include "vtkMAFSmartPointer.h"
#include "vtkImageToStructuredPoints.h"
#include "medOpVolumeResample.h"
#include "vtkRectilinearGrid.h"
#include "vtkMEDVolumeToClosedSmoothSurface.h"


#define ITK_IMAGE_DIMENSION 3
#define SPACING_PERCENTAGE_BOUNDS 0.1
typedef  itk::Image< float, ITK_IMAGE_DIMENSION> RealImage;
using namespace std;

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpSegmentationRegionGrowingConnectedThreshold);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpSegmentationRegionGrowingConnectedThreshold::medOpSegmentationRegionGrowingConnectedThreshold(wxString label) : mafOp(label)
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
  m_UseSpacing = TRUE;// this is used in smoothing

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
medOpSegmentationRegionGrowingConnectedThreshold::~medOpSegmentationRegionGrowingConnectedThreshold( ) 
//----------------------------------------------------------------------------
{
  delete []m_Seed;
  mafDEL(m_VolumeOut);
  mafDEL(m_SurfaceOut);
  mafDEL(m_Picker);
  vtkDEL(m_SphereVTK);
}
//----------------------------------------------------------------------------
bool medOpSegmentationRegionGrowingConnectedThreshold::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  // accepts mafVMEVolumeGray data
  return  (node->IsA("mafVMEVolumeGray")) && ((mafVME*)node)->GetOutput()->GetVTKData();
}
//----------------------------------------------------------------------------
mafOp* medOpSegmentationRegionGrowingConnectedThreshold::Copy()   
//----------------------------------------------------------------------------
{
  return new medOpSegmentationRegionGrowingConnectedThreshold(m_Label);
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingConnectedThreshold::OpRun()   
//----------------------------------------------------------------------------
{
  CreateGui();
  if (CreateResample())
  {
    
	  m_OldBehavior = mafVME::SafeDownCast(m_ResampleInput)->GetBehavior();

	  m_Picker = mafInteractorPicker::New();
	  m_Picker->SetListener(this);
	  mafVME::SafeDownCast(m_ResampleInput)->SetBehavior(m_Picker);

	  mafNEW(m_Sphere);
	  m_Sphere->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
	  m_Sphere->SetVisibleToTraverse(false);
	  m_Sphere->ReparentTo(m_ResampleInput->GetParent());

	  vtkNEW(m_SphereVTK);
    double bounds[6];mafVME::SafeDownCast(m_ResampleInput)->GetOutput()->GetBounds(bounds);
	  m_SphereVTK->SetRadius((bounds[5]-bounds[4])/256);
    mafLogMessage("Sphere radius = %f",(bounds[5]-bounds[4])/256);
	  m_SphereVTK->Update();

	  m_Sphere->SetData(m_SphereVTK->GetOutput(),0.0);
	  m_Sphere->Update(); 
  }
  else
    OpStop(OP_RUN_CANCEL);
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingConnectedThreshold::OpDo()
//----------------------------------------------------------------------------
{
  if(m_Output)
  {
    m_Output->ReparentTo(m_ResampleInput);
    mafEventMacro(mafEvent(this,VME_SHOW,m_Output,true));
    mafEventMacro(mafEvent(this, CAMERA_UPDATE));
  }
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingConnectedThreshold::CreateGui()   
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);

  m_Gui->Label(_("Parameters"));
  m_Gui->Integer(ID_LOWER,_("lower"),&m_Lower,MINLONG,MAXLONG,_("minimum intensity value to be included in segmentation"));
  m_Gui->Integer(ID_UPPER,_("upper"),&m_Upper,MINLONG,MAXLONG,_("maximum intensity value to be included in segmentation"));
  m_Gui->Integer(ID_REPLACE,_("replace"),&m_Replace,0,MAXINT,_("segmented region value"));
  m_Gui->Label(_("Voxel val:"),&m_SeedScalarValue);
  m_Gui->VectorN(ID_SEED,_("seed"),m_Seed,m_ImageDim,MININT,MAXINT,"seed point to start growing, in image coordinates");

//   m_Gui->Label(_("Anisotropic curvature diffusion parameters"));
//   m_Gui->Integer(ID_ITERATIONS,_("iterations"),&m_NumIter,0,MAXINT,_("number of iterations. Default [5]"));
//   m_Gui->Float(ID_TIME_STEP,_("time step"),&m_TimeStep,0,MAXFLOAT,0,-1,_("time step. Default [0.0125]"));
//   m_Gui->Float(ID_CONDUCTANCE,_("conductance"),&m_Conductance,MINFLOAT,MAXFLOAT,0,-1,_("conductance. Default [1.0]"));
//   m_Gui->Bool(ID_USE_SPACING,_("use spacing"),&m_UseSpacing,1,_("do NOT use image spacing when computing filter"));

  m_Gui->OkCancel();

  ShowGui();
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingConnectedThreshold::OpStop(int result)   
//----------------------------------------------------------------------------
{  
  if (result==OP_RUN_OK)
  {
    mafEventMacro(mafEvent(this,VME_SHOW,m_Sphere,false));
    m_Sphere->ReparentTo(NULL);

    mafVME::SafeDownCast(m_ResampleInput)->SetBehavior(m_OldBehavior);
    mafVME::SafeDownCast(m_ResampleInput)->Update();

    HideGui();
    mafEventMacro(mafEvent(this,result));
  } 
  else if (result==OP_RUN_CANCEL)
  { 
    if(m_Sphere!=NULL )
    {
      mafEventMacro(mafEvent(this,VME_SHOW,m_Sphere,false));
      m_Sphere->ReparentTo(NULL);
  
      mafVME::SafeDownCast(m_ResampleInput)->SetBehavior(m_OldBehavior);
      mafVME::SafeDownCast(m_ResampleInput)->Update();
    }
    
    if(m_ResampleInput!=NULL && m_ResampleInput!=m_Input)
    {
      mafEventMacro(mafEvent(this,VME_SHOW,m_ResampleInput,false)); 
      mafEventMacro(mafEvent(this,VME_SELECT,m_ResampleInput,false)); 
      mafEventMacro(mafEvent(this, VME_SELECT, m_Input, true));
      mafEventMacro(mafEvent(this,VME_SHOW,m_Input,true)); 
      m_ResampleInput->ReparentTo(NULL);
      m_ResampleInput->Update();
    }
    mafDEL(m_Resample); 
    HideGui();
    mafEventMacro(mafEvent(this,result));
  }
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingConnectedThreshold::Algorithm()   
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

  vtkImageData *im = vtkImageData::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_ResampleInput)->GetOutput()->GetVTKData());
  
  vtkMAFSmartPointer<vtkImageCast> vtkImageToFloat;
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

  if (m_UseSpacing == TRUE)
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

  mafNEW(m_VolumeOut);
  m_VolumeOut->SetName("Connected Threshold");

  vtkImageData *image = ((vtkImageData*)itkTOvtk->GetOutput());
  image->Update();


  vtkMAFSmartPointer<vtkImageToStructuredPoints> image_to_sp;
  image_to_sp->SetInput(image);
  image_to_sp->Update();
  m_VolumeOut->SetData(image_to_sp->GetOutput(),mafVME::SafeDownCast(m_ResampleInput)->GetTimeStamp());

  mafTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("SYNTHETIC");

  m_VolumeOut->GetTagArray()->SetTag(tag_Nature);

  m_VolumeOut->Update();

  m_VolumeOut->GetTagArray()->SetTag(mafTagItem("VOLUME_TYPE","BINARY"));

  
  vtkMAFSmartPointer<vtkMEDVolumeToClosedSmoothSurface> volToSurface;
  volToSurface->SetInput(m_VolumeOut->GetOutput()->GetVTKData());
  volToSurface->SetContourValue(127.5);
  volToSurface->Update();
  vtkPolyData *surface=volToSurface->GetOutput();

  //Generating Surface VME
  mafNEW(m_SurfaceOut);
  m_SurfaceOut->SetName("Connected Threshold Surface");
  m_SurfaceOut->SetData(surface,mafVMEVolumeGray::SafeDownCast(m_ResampleInput)->GetTimeStamp());
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
void medOpSegmentationRegionGrowingConnectedThreshold::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_SEED:
      {
        double b[6],spacing[3],origin[3];
        mafVMEVolumeGray::SafeDownCast(m_ResampleInput)->GetOutput()->GetBounds(b);

        vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_ResampleInput)->GetOutput()->GetVTKData());
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

        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
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

          mafEventMacro(mafEvent(this,VME_SHOW,m_Sphere,true));
          mafEventMacro(mafEvent(this,CAMERA_UPDATE));

          vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_ResampleInput)->GetOutput()->GetVTKData());
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
      mafEventMacro(*e);
      break;
    }
  }
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingConnectedThreshold::SetSeed(int *seed)
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_ImageDim;i++)
  {
    m_Seed[i]=seed[i];
  }
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingConnectedThreshold::GetSeed(int *seed)
//----------------------------------------------------------------------------
{
  for (int i=0;i<m_ImageDim;i++)
  {
    seed[i]=m_Seed[i];
  }
}
//----------------------------------------------------------------------------
int medOpSegmentationRegionGrowingConnectedThreshold::CreateResample()
//----------------------------------------------------------------------------
{
 
  // if the volume is a rectilinear grid we resample it 
  if(((mafVME*)m_Input)->GetOutput()->GetVTKData()->IsA("vtkRectilinearGrid"))
  { 
    wxBusyInfo *info;
    wxBusyCursor *wait;

    m_Resample=new medOpVolumeResample();
    m_Resample->SetInput(m_Input);
    m_Resample->TestModeOn();
    m_Resample->AutoSpacing();
    m_Resample->GetSpacing(m_VolumeSpacing);
      
    ((mafVME*)m_Input)->GetOutput()->GetVMELocalBounds(m_VolumeBounds);
    m_Resample->SetBounds(m_VolumeBounds,medOpVolumeResample::CUSTOMBOUNDS);
          
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
     
    mafVME *Output = mafVME::SafeDownCast(m_Resample->GetOutput());
    Output->GetOutput()->GetVTKData()->Update();
    m_ResampleInput=mafVMEVolumeGray::SafeDownCast(Output);
    m_ResampleInput->Update();

    // show volume resampled
    mafEventMacro(mafEvent(this, VME_SHOW, m_ResampleInput, true));
    mafEventMacro(mafEvent(this, VME_SELECT, m_ResampleInput, true));
    mafEventMacro(mafEvent(this, CAMERA_UPDATE));
     
    if(!m_TestMode)
    {
      delete wait;
      delete info;
    }
  }
  else 
  {
    m_ResampleInput=mafVMEVolumeGray::SafeDownCast(m_Input);
  }
  return true;

}
//----------------------------------------------------------------------------
bool medOpSegmentationRegionGrowingConnectedThreshold::CheckSpacing()
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

