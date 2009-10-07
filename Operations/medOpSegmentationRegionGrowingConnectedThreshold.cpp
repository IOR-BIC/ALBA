/*=========================================================================
Program:   @neufuse
Module:    $RCSfile: medOpSegmentationRegionGrowingConnectedThreshold.cpp,v $
Language:  C++
Date:      $Date: 2009-10-07 14:09:48 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni, Alessandro Chiarini
==========================================================================
Copyright (c) 2008
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
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
#include "mmiPicker.h"
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

#define ITK_IMAGE_DIMENSION 3
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

  m_SeedScalarValue = "";
}
//----------------------------------------------------------------------------
medOpSegmentationRegionGrowingConnectedThreshold::~medOpSegmentationRegionGrowingConnectedThreshold( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_Picker);
  vtkDEL(m_SphereVTK);
}
//----------------------------------------------------------------------------
bool medOpSegmentationRegionGrowingConnectedThreshold::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  // accepts only structuredpoints data!
  return  (node->IsA("mafVMEVolumeGray")) && ((mafVME*)node)->GetOutput()->GetVTKData() &&(((mafVME*)node)->GetOutput()->GetVTKData()->IsA("vtkStructuredPoints"));
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


	m_OldBehavior = mafVME::SafeDownCast(m_Input)->GetBehavior();

	m_Picker = mmiPicker::New();
	m_Picker->SetListener(this);
	mafVME::SafeDownCast(m_Input)->SetBehavior(m_Picker);

	mafNEW(m_Sphere);
	m_Sphere->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
	m_Sphere->SetVisibleToTraverse(false);
	m_Sphere->ReparentTo(m_Input->GetParent());

	vtkNEW(m_SphereVTK);
  double bounds[6];mafVME::SafeDownCast(m_Input)->GetOutput()->GetBounds(bounds);
	m_SphereVTK->SetRadius((bounds[5]-bounds[4])/256);
  mafLogMessage("Sphere radius = %f",(bounds[5]-bounds[4])/256);
	m_SphereVTK->Update();

	m_Sphere->SetData(m_SphereVTK->GetOutput(),0.0);
	m_Sphere->Update();
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingConnectedThreshold::CreateGui()   
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);

  m_Gui->Label(_("Parameters"));
  m_Gui->Integer(ID_LOWER,_("lower"),&m_Lower,0,MAXLONG,_("minimum intensity value to be included in segmentation"));
  m_Gui->Integer(ID_UPPER,_("upper"),&m_Upper,0,MAXLONG,_("maximum intensity value to be included in segmentation"));
  m_Gui->Integer(ID_REPLACE,_("replace"),&m_Replace,0,MAXINT,_("segmented region value"));
  m_Gui->Label(_("Voxel val:"),&m_SeedScalarValue);
  m_Gui->VectorN(ID_SEED,_("seed"),m_Seed,m_ImageDim,MININT,MAXINT,"seed point to start growing, in image coordinates");

  m_Gui->Label(_("Anisotropic curvature diffusion parameters"));
  m_Gui->Integer(ID_ITERATIONS,_("iterations"),&m_NumIter,0,MAXINT,_("number of iterations. Default [5]"));
  m_Gui->Float(ID_TIME_STEP,_("time step"),&m_TimeStep,0,MAXFLOAT,0,-1,_("time step. Default [0.0125]"));
  m_Gui->Float(ID_CONDUCTANCE,_("conductance"),&m_Conductance,MINFLOAT,MAXFLOAT,0,-1,_("conductance. Default [1.0]"));
  m_Gui->Bool(ID_USE_SPACING,_("use spacing"),&m_UseSpacing,1,_("do NOT use image spacing when computing filter"));

  m_Gui->OkCancel();

  ShowGui();
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingConnectedThreshold::OpStop(int result)   
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,VME_SHOW,m_Sphere,false));
  m_Sphere->ReparentTo(NULL);

  mafVME::SafeDownCast(m_Input)->SetBehavior(m_OldBehavior);
  mafVME::SafeDownCast(m_Input)->Update();

  HideGui();
  mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingConnectedThreshold::Algorithm()   
//----------------------------------------------------------------------------
{

  wxBusyCursor wait;
  wxBusyInfo wait_info("Please wait");

 typedef itk::ConnectedThresholdImageFilter<RealImage, RealImage> ITKConnectedThresholdFilter;
  ITKConnectedThresholdFilter::Pointer connectedThreshold = ITKConnectedThresholdFilter::New();

  vtkImageData *im = vtkImageData::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_Input)->GetOutput()->GetVTKData());
  
  vtkImageCast * vtkImageToFloat = vtkImageCast::New();
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

  mafVMEVolumeGray *volumeOut;
  mafNEW(volumeOut);
  volumeOut->SetName("Connected Threshold");

  vtkImageData *image = ((vtkImageData*)itkTOvtk->GetOutput());
  image->Update();


  vtkMAFSmartPointer<vtkImageToStructuredPoints> image_to_sp;
  image_to_sp->SetInput(image);
  image_to_sp->Update();
  volumeOut->SetData(image_to_sp->GetOutput(),mafVME::SafeDownCast(m_Input)->GetTimeStamp());

  mafTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("SYNTHETIC");

  volumeOut->GetTagArray()->SetTag(tag_Nature);

  volumeOut->Update();

  m_Output = volumeOut;

  vtkImageToFloat->Delete();

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
        mafVMEVolumeGray::SafeDownCast(m_Input)->GetOutput()->GetBounds(b);

        vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_Input)->GetOutput()->GetVTKData());
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

          vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_Input)->GetOutput()->GetVTKData());
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
