/*=========================================================================

 Program: MAF2
 Module: mafPipeVolumeVR
 Authors: Daniele Giunchi
 
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

#include "mafPipeVolumeVR.h"

#include "mafDecl.h"
#include "mafGUI.h"
#include "mafGUILutPreset.h"

#include "vtkMAFAssembly.h"
#include "mmaVolumeMaterial.h"
#include "mafVME.h"
#include "mafVMEVolume.h"

//#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"

#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkMAFVolumeTextureMapper2D.h"
#include "vtkVolume.h"
#include "vtkLODProp3D.h"
#include "vtkImageShiftScale.h"
#include "vtkProbeFilter.h" 
#include "vtkRectilinearGrid.h" 
#include "vtkStructuredPoints.h"
#include "vtkLookupTable.h"
#include "vtkImageResample.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeVolumeVR);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeVolumeVR::mafPipeVolumeVR()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_PiecewiseFunction			= NULL;
  m_ColorTransferFunction	= NULL;

  m_VolumePropertyHigh				= NULL;
  m_VolumePropertyLow					= NULL; 
  m_VolumeTextureMapperHigh		= NULL;
  m_VolumeTextureMapperLow		= NULL;  

  m_PropertyLOD	= NULL;
  m_ActorLOD		= NULL;

  m_ImageShift				= NULL;
  m_GradientFunction	= NULL;
  m_Range[0]		= 0;
  m_Range[1]		= 0;
  m_UnsignRange[0]= 0;
  m_UnsignRange[1]= 0;

  m_Rescale				= false;
  m_IsStructured	= true;

  m_StructuredImage = NULL;
  m_Probe						= NULL;
  m_ResampleFilter	= NULL;
  m_ResampleFactor	= 1.0;
}
//----------------------------------------------------------------------------
void mafPipeVolumeVR::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  m_NumberOfSlices = 64; //<< was parameter

  if(m_NumberOfSlices != 64) m_SliceSet = true;
  else m_SliceSet = false;


  //assert(mafGetBaseType(m_Vme) == VME_GRAY_VOLUME);

  m_Created = true;

  m_Vme->Update();

  mafString vmeControl = m_Vme->GetOutput()->GetVTKData()->GetClassName();
  if(vmeControl == "vtkRectilinearGrid")
  {
    m_IsStructured = false;

    int dim[3];
    double bounds[6];
    ((vtkRectilinearGrid *)m_Vme->GetOutput()->GetVTKData())->GetDimensions(dim);

    //Get the bounds for m_Vme as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax)
    m_Vme->GetOutput()->GetVTKData()->GetBounds(bounds);

    m_StructuredImage = vtkStructuredPoints::New();

    m_Probe = vtkProbeFilter::New();

    //m_NumberOfSlices is 128 as default
    double spaceInt_x = (bounds[1]-bounds[0])/(dim[0]-1);
    double spaceInt_y = (bounds[3]-bounds[2])/(dim[1]-1);;
    double spaceInt_z = (bounds[5]-bounds[4])/((double)m_NumberOfSlices);


    m_StructuredImage->SetOrigin(bounds[0],bounds[2],bounds[4]);

    int dim_z = (bounds[5]-bounds[4])/spaceInt_z + 1;

    m_StructuredImage->SetDimensions(dim[0],dim[1],dim_z);

    m_StructuredImage->SetSpacing(spaceInt_x,spaceInt_y,spaceInt_z);

    m_StructuredImage->Update();

    m_Probe->SetInput(m_StructuredImage);
    m_Probe->SetSource(m_Vme->GetOutput()->GetVTKData());

    m_Probe->Update();


  }//end if(m_Vme->GetCurrentData()->IsA(L"vtkRectilinearGrid")) 

  m_PiecewiseFunction	= vtkPiecewiseFunction::New();
  m_ColorTransferFunction = vtkColorTransferFunction::New();
  m_GradientFunction  = vtkPiecewiseFunction::New();

  m_VolumePropertyHigh = vtkVolumeProperty::New(); 
  m_VolumePropertyHigh->SetColor(m_ColorTransferFunction);
  m_VolumePropertyHigh->SetScalarOpacity(m_PiecewiseFunction);
  m_VolumePropertyHigh->SetGradientOpacity(m_GradientFunction);	
  //m_VolumePropertyHigh->DisableGradientOpacityOff();
  m_VolumePropertyHigh->SetInterpolationTypeToLinear();
  m_VolumePropertyHigh->ShadeOn();	
  m_VolumePropertyHigh->Modified();

  m_VolumePropertyLow = vtkVolumeProperty::New(); 
  m_VolumePropertyLow->SetColor(m_ColorTransferFunction);
  m_VolumePropertyLow->SetScalarOpacity(m_PiecewiseFunction);
  m_VolumePropertyLow->SetGradientOpacity(m_GradientFunction);	
  //m_VolumePropertyLow->DisableGradientOpacityOff();
  m_VolumePropertyLow->ShadeOff();

  m_PropertyLOD = vtkProperty::New();
  m_PropertyLOD->SetAmbient(0.1);
  m_PropertyLOD->SetDiffuse(0.9);
  m_PropertyLOD->SetSpecular(0.2);
  m_PropertyLOD->SetSpecularPower(10.0);

  m_VolumeTextureMapperHigh = vtkMAFVolumeTextureMapper2D::New();
  m_VolumeTextureMapperLow = vtkMAFVolumeTextureMapper2D::New(); 

  m_Vme->GetOutput()->GetVTKData()->GetScalarRange(m_Range);

  m_Minimum = m_Range[0];
  m_Maximum = m_Range[1];

  if(m_Minimum < 0.0)
  {
    m_Rescale = true;

    m_ImageShift = vtkImageShiftScale::New();

    if (!m_IsStructured){
      m_ImageShift->SetInput((vtkImageData *)m_Probe->GetOutput());
    }
    else 
    {
      vtkDataSet* data = m_Vme->GetOutput()->GetVTKData();
      vtkNEW(m_ResampleFilter);
      m_ResampleFilter->SetInput((vtkImageData*)data);
      for(int i=0;i<3;i++)
        m_ResampleFilter->SetAxisMagnificationFactor(i,m_ResampleFactor);
      m_ResampleFilter->Update();
      m_ImageShift->SetInput((vtkImageData *)m_ResampleFilter->GetOutput());

    }

    m_ImageShift->SetShift(- m_Minimum);

    m_ImageShift->SetOutputScalarTypeToUnsignedShort();

    m_ImageShift->Update();

    m_VolumeTextureMapperHigh->SetInput((vtkImageData *)m_ImageShift->GetOutput());
    m_VolumeTextureMapperLow->SetInput((vtkImageData *)m_ImageShift->GetOutput());

    ((vtkImageData *)m_ImageShift->GetOutput())->GetScalarRange(m_UnsignRange);
  }
  else //if (m_Minimum < 0.0)
  {
    if (!m_IsStructured)
    {
      m_VolumeTextureMapperHigh->SetInput((vtkImageData *)m_Probe->GetOutput());
      m_VolumeTextureMapperLow->SetInput((vtkImageData *)m_Probe->GetOutput());

      ((vtkImageData *)m_Probe->GetOutput())->GetScalarRange(m_UnsignRange);
    }

    else 
    {
      vtkDataSet* data = m_Vme->GetOutput()->GetVTKData();
      data->Update();
      vtkNEW(m_ResampleFilter);
      m_ResampleFilter->SetInput((vtkImageData*)data);
      for(int i=0;i<3;i++)
        m_ResampleFilter->SetAxisMagnificationFactor(i,m_ResampleFactor);
      m_ResampleFilter->Update();

      m_VolumeTextureMapperHigh->SetInput((vtkImageData *)m_ResampleFilter->GetOutput());
      m_VolumeTextureMapperLow->SetInput((vtkImageData *)m_ResampleFilter->GetOutput());

      ((vtkImageData *)m_ResampleFilter->GetOutput())->GetScalarRange(m_UnsignRange);
    }    
  }

  //BES: 12.8.2008 - Usually we deal with 8, 12 or 16 bits volume data sets and they
  //can be either signed or unsigned => transfer functions will be derived for it
  //Anyway, there should be an option for the user to change the function in GUI
  double MaxR;
  if (m_UnsignRange[1] < 256.0)
  {    
    m_PiecewiseFunction->AddPoint(  0, 0.0);
    m_PiecewiseFunction->AddPoint(255,1.0);

    MaxR = 255.0;
  }
  else if (m_UnsignRange[1] < 4096.0)
  {
    m_PiecewiseFunction->AddPoint(   0,0.0);
    m_PiecewiseFunction->AddPoint(128,0.0);    
    m_PiecewiseFunction->AddPoint(4095,1.0);

    MaxR = 4095.0;
  }
  else
  {
    m_PiecewiseFunction->AddPoint(    0,0.0);
    m_PiecewiseFunction->AddPoint(22737,0.0);
    m_PiecewiseFunction->AddPoint(44327,1.0);
    m_PiecewiseFunction->AddPoint(65535,1.0);

    MaxR = 65535.0;  
  }

  m_ColorTransferFunction->AddRGBPoint(    0, 0.00, 0.00, 0.00);
  m_ColorTransferFunction->AddRGBPoint((11655 / 65535.0)*MaxR, 0.74, 0.19, 0.14);
  m_ColorTransferFunction->AddRGBPoint((31908 / 65535.0)*MaxR, 0.96, 0.64, 0.42);
  m_ColorTransferFunction->AddRGBPoint((33818 / 65535.0)*MaxR, 0.76, 0.78, 0.25);
  m_ColorTransferFunction->AddRGBPoint((41843 / 65535.0)*MaxR, 1.00, 1.00, 1.00);
  m_ColorTransferFunction->AddRGBPoint((65535 / 65535.0)*MaxR, 1.00, 1.00, 1.00); 

  m_VolumeTextureMapperHigh->SetMaximumNumberOfPlanes(1024);
  m_VolumeTextureMapperHigh->SetTargetTextureSize(512,512);
  m_VolumeTextureMapperHigh->SetMaximumStorageSize(64*1024*1024);  //BES 2.6.2008 - enable texture saving using up to 64 MB

  m_VolumeTextureMapperLow->SetMaximumNumberOfPlanes(128);
  m_VolumeTextureMapperLow->SetTargetTextureSize(32, 32);
  m_VolumeTextureMapperLow->SetMaximumStorageSize(8*1024*1024);   //BES 2.6.2008 - enable texture saving using up to 8 MB


  //BES 25.4.2008 - with texture saving, we are enable to render High and Low in zero time
  //=> as VTK selects LOD from first index, high must go first
  m_ActorLOD = vtkLODProp3D ::New();
  int nID1 = m_ActorLOD->AddLOD(m_VolumeTextureMapperHigh, m_VolumePropertyHigh, 0);
  int nID2 = m_ActorLOD->AddLOD(m_VolumeTextureMapperLow, m_VolumePropertyLow, 0);
  m_ActorLOD->SetLODLevel(nID1, 0);
  m_ActorLOD->SetLODLevel(nID2, 1);

  m_ActorLOD->PickableOff();
  m_ActorLOD->SetLODProperty(nID1, m_PropertyLOD);
  m_ActorLOD->SetLODProperty(nID2, m_PropertyLOD);
  m_ActorLOD->Modified();

  m_AssemblyFront->AddPart(m_ActorLOD);
  m_AssemblyFront->PickableOff();

  //SIL. 20-6-2003 modified code - global visibility handling is temporary disable
  /*
  vtkTagItem *t = m_Vme->GetTagArray()->GetTag(L"visible");
  if(t) Show(t->GetValueAsDouble()!= 0);
  */
}
//----------------------------------------------------------------------------
mafPipeVolumeVR::~mafPipeVolumeVR()
//----------------------------------------------------------------------------
{
  if(!m_Created) return;

  m_AssemblyFront->RemovePart(m_ActorLOD);

  vtkDEL(m_ActorLOD);
  vtkDEL(m_PropertyLOD);          //BES: bug fix
  vtkDEL(m_PiecewiseFunction);
  vtkDEL(m_ColorTransferFunction);
  vtkDEL(m_VolumePropertyLow);
  vtkDEL(m_VolumePropertyHigh);
  vtkDEL(m_VolumeTextureMapperLow);
  vtkDEL(m_VolumeTextureMapperHigh);
  vtkDEL(m_ImageShift);
  vtkDEL(m_GradientFunction);
  vtkDEL(m_StructuredImage);
  vtkDEL(m_Probe);
  vtkDEL(m_ResampleFilter);
}
//----------------------------------------------------------------------------
void mafPipeVolumeVR::Show(bool show)
//----------------------------------------------------------------------------
{
  if(!m_Created) return;
  m_ActorLOD->SetVisibility(show);
  //if(m_Selected) m_sel_a->SetVisibility(show);
}
//----------------------------------------------------------------------------
void mafPipeVolumeVR::Select(bool sel)
//----------------------------------------------------------------------------
{
  if(!m_Created || m_ActorLOD == NULL) return;

  m_Selected = sel;

  //if(m_act_a->GetVisibility()) 
  //	m_sel_a->SetVisibility(sel);
}
//----------------------------------------------------------------------------
mafGUI *mafPipeVolumeVR::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);

  m_Gui->Double(ID_RESAMPLE_FACTOR,_("Resample"),&m_ResampleFactor,0.00001,1);

  return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeVolumeVR::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
    case ID_LUT_CHOOSER:

      break;
    case ID_RESAMPLE_FACTOR:
      {
        for(int i=0;i<3;i++)
          m_ResampleFilter->SetAxisMagnificationFactor(i,m_ResampleFactor);

        m_ResampleFilter->Update();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    }
  }
}
//----------------------------------------------------------------------------
void mafPipeVolumeVR::AddPoint(int scalarPoint,double opacity)
//----------------------------------------------------------------------------
{
  m_PiecewiseFunction->AddPoint((double)scalarPoint,opacity);		
}
//----------------------------------------------------------------------------
void mafPipeVolumeVR::AddPoint(int scalarPoint,double red,double green,double blue)
//----------------------------------------------------------------------------
{
  m_ColorTransferFunction->AddRGBPoint((double)scalarPoint,red,green,blue);
}
//----------------------------------------------------------------------------
void mafPipeVolumeVR::AddGradPoint(int scalarPoint,double gradient)
//----------------------------------------------------------------------------
{
  m_GradientFunction->AddPoint((double)scalarPoint,gradient);	
}
//----------------------------------------------------------------------------
void mafPipeVolumeVR::RemoveOpacityPoint(int scalarPoint)
//----------------------------------------------------------------------------
{
  m_PiecewiseFunction->RemovePoint((double)scalarPoint);
}
//----------------------------------------------------------------------------
void mafPipeVolumeVR::RemoveColorPoint(int scalarPoint)
//----------------------------------------------------------------------------
{
  m_ColorTransferFunction->RemovePoint((double)scalarPoint);	
}
//----------------------------------------------------------------------------
void mafPipeVolumeVR::RemoveOpacityGradPoint(int scalarPoint)
//----------------------------------------------------------------------------
{
  m_GradientFunction->RemovePoint((double)scalarPoint);	
}
//----------------------------------------------------------------------------
void mafPipeVolumeVR::SetNumberPoints(int n)
//----------------------------------------------------------------------------
{

  m_NumberOfSlices = n;
  if (m_IsStructured) return;

  int dim[3];
  double bounds[6];

  ((vtkStructuredPoints *)m_Vme->GetOutput()->GetVTKData())->GetDimensions(dim);

  //Get the bounds for m_Vme as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax)
  m_Vme->GetOutput()->GetVTKData()->GetBounds(bounds);

  double spaceInt_x = (bounds[1]-bounds[0])/(dim[0]-1);
  double spaceInt_y = (bounds[3]-bounds[2])/(dim[1]-1);;
  double spaceInt_z = (bounds[5]-bounds[4])/((double)m_NumberOfSlices);

  if(m_StructuredImage) m_StructuredImage->Delete();

  m_StructuredImage = vtkStructuredPoints::New();

  m_StructuredImage->SetOrigin(bounds[0],bounds[2],bounds[4]);
  int dim_z = (bounds[5]-bounds[4])/spaceInt_z + 1;

  m_StructuredImage->SetDimensions(dim[0],dim[1],dim_z);
  m_StructuredImage->SetSpacing(spaceInt_x,spaceInt_y,spaceInt_z);

  //m_StructuredImage->Modified();
  m_StructuredImage->Update();

  if(m_Probe) m_Probe->Delete();

  m_Probe = vtkProbeFilter::New();

  m_Probe->SetInput(m_StructuredImage);
  m_Probe->SetSource(m_Vme->GetOutput()->GetVTKData());

  m_Probe->Update();	
}
//----------------------------------------------------------------------------
double mafPipeVolumeVR::GetResampleFactor()
//----------------------------------------------------------------------------
{
  return m_ResampleFactor;
}
//----------------------------------------------------------------------------
void mafPipeVolumeVR::SetResampleFactor(double value)
//----------------------------------------------------------------------------
{
  m_ResampleFactor = value;


  if(m_ResampleFilter)
  {
    m_AssemblyFront->RemovePart(m_ActorLOD);
    vtkDEL(m_ActorLOD);
    vtkDEL(m_VolumeTextureMapperHigh);
    vtkDEL(m_VolumeTextureMapperLow);

    m_ResampleFilter->Update();

    for(int i=0;i<3;i++)
      m_ResampleFilter->SetAxisMagnificationFactor(i,m_ResampleFactor);

    m_ResampleFilter->Update();

    vtkNEW(m_VolumeTextureMapperHigh);
    vtkNEW(m_VolumeTextureMapperLow);

    m_VolumeTextureMapperHigh->SetInput(m_ResampleFilter->GetOutput());
    m_VolumeTextureMapperLow->SetInput(m_ResampleFilter->GetOutput());

    m_VolumeTextureMapperHigh->SetMaximumNumberOfPlanes(1024);
    m_VolumeTextureMapperHigh->SetTargetTextureSize(512,512);

    m_VolumeTextureMapperLow->SetMaximumNumberOfPlanes(128);
    m_VolumeTextureMapperLow->SetTargetTextureSize(32, 32);

    vtkNEW(m_ActorLOD);

    m_ActorLOD->AddLOD(m_VolumeTextureMapperLow, m_VolumePropertyLow, 0);
    m_ActorLOD->AddLOD(m_VolumeTextureMapperHigh, m_VolumePropertyHigh, 0);
    m_ActorLOD->PickableOff();
    m_ActorLOD->SetLODProperty(1, m_PropertyLOD);
    m_ActorLOD->SetLODProperty(2, m_PropertyLOD);
    m_ActorLOD->Modified();

    m_AssemblyFront->AddPart(m_ActorLOD);
  }

  if(m_Gui)
    m_Gui->Update();
}
//----------------------------------------------------------------------------
void mafPipeVolumeVR::VolumePropertyHighShadeOn()
//----------------------------------------------------------------------------
{
  m_VolumePropertyHigh->ShadeOn();
  m_VolumePropertyHigh->Modified();

}
//----------------------------------------------------------------------------
void mafPipeVolumeVR::VolumePropertyHighShadeOff()
//----------------------------------------------------------------------------
{
  m_VolumePropertyHigh->ShadeOff();
  m_VolumePropertyHigh->Modified();
}