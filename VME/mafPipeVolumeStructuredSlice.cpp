/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeVolumeStructuredSlice.cpp,v $
  Language:  C++
  Date:      $Date: 2005-11-09 11:24:36 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafPipeVolumeStructuredSlice.h"
#include "mafDecl.h"
#include "mmgGui.h"
#include "mmgFloatSlider.h"
#include "mafSceneNode.h"
#include "mafPipeVolumeSlice.h"

#include "mafVMEVolume.h"
#include "mafVMEOutputVolume.h"

#include "vtkMAFAssembly.h"
#include "vtkMath.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkTexture.h"
#include "vtkPolyDataMapper.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkLookupTable.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkOutlineFilter.h"
#include "vtkStructuredPoints.h"
#include "vtkRectilinearGrid.h"
#include "vtkExtractVOI.h"
#include "vtkProperty.h"
#include "vtkDataSet.h"
#include "vtkPlaneSource.h"
#include "vtkVolumeResample.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeVolumeStructuredSlice);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeVolumeStructuredSlice::mafPipeVolumeStructuredSlice()
:mafPipe()
//----------------------------------------------------------------------------
{ 
  for(int i = 0; i<3; i++)
  {
    m_SlicerImage[i]	= NULL;
    m_Texture[i]		  = NULL;
    m_GrayLUT[i]      = NULL;
    m_ColorLUT[i]     = NULL;
    m_SlicePolygon[i] = NULL;
    m_SliceMapper[i]	= NULL;
    m_SliceActor[i]	  = NULL;
    m_SliceSlider[i]  = NULL;
  }
  m_VolumeData      = NULL;
  m_VolumeResampler = NULL;
  m_SliceParametersInitialized = false;
  m_ShowVolumeBox         = 0;

  m_SliceMode = SLICE_Z;

  m_ColorLUTEnabled = 0;
  m_SliceOpacity    = 1.0;

  m_SliceNumber[0] = 0;
  m_SliceNumber[1] = 0;
  m_SliceNumber[2] = 0;
}
//----------------------------------------------------------------------------
void mafPipeVolumeStructuredSlice::InitializeSliceParameters(int mode, bool show_vol_bbox)
//----------------------------------------------------------------------------
{
  m_SliceMode     = mode;
  m_ShowVolumeBox = show_vol_bbox;
}
//----------------------------------------------------------------------------
void mafPipeVolumeStructuredSlice::InitializeSliceParameters(int mode, double slice_origin[3], bool show_vol_bbox)
//----------------------------------------------------------------------------
{
  m_SliceParametersInitialized = true;
  m_SliceMode     = mode;
  m_ShowVolumeBox = show_vol_bbox;

  m_Origin[0] = slice_origin[0];
  m_Origin[1] = slice_origin[1];
  m_Origin[2] = slice_origin[2];
}
//----------------------------------------------------------------------------
void mafPipeVolumeStructuredSlice::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n); // Always call this to initialize m_Vme, m_AssemblyFront, ... vars

  assert(m_Vme->IsMAFType(mafVMEVolume));
  m_Vme->GetOutput()->Update();
  vtkDataSet *vtk_data = m_Vme->GetOutput()->GetVTKData();
  vtk_data->Update();

  m_VolumeData = vtkImageData::SafeDownCast(vtk_data);
  if (m_VolumeData == NULL)
  {
    vtkNEW(m_VolumeResampler);
    m_VolumeResampler->SetInput(vtk_data);
    m_VolumeResampler->Update();
    m_VolumeData = vtkImageData::SafeDownCast(m_VolumeResampler->GetOutput());
  }

  vtkNEW(m_VolumeBox);
  m_VolumeBox->SetInput(m_Vme->GetOutput()->GetVTKData());

  vtkNEW(m_VolumeBoxMapper);
  m_VolumeBoxMapper->SetInput(m_VolumeBox->GetOutput());

  vtkNEW(m_VolumeBoxActor);
  m_VolumeBoxActor->SetMapper(m_VolumeBoxMapper);
  m_VolumeBoxActor->SetVisibility(m_ShowVolumeBox);

  m_AssemblyFront->AddPart(m_VolumeBoxActor);

	if(m_SliceMode == SLICE_ORTHO)
	{
    for(int i = 0; i<3; i++)
		{
			CreateSlice(i);
		}
	}
	else
	{
		CreateSlice(m_SliceMode);
	}

  // if the actor is in the background renderer
	// create something invisible in the front renderer so that ResetCamera will work
  m_GhostActor = NULL;
  if(m_AssemblyBack != NULL)
	{
		vtkNEW(m_GhostActor);
		m_GhostActor->SetMapper(m_SliceMapper[m_SliceMode]);
		m_GhostActor->PickableOff();
		m_GhostActor->GetProperty()->SetOpacity(0);
		m_GhostActor->GetProperty()->SetRepresentationToPoints();
		m_GhostActor->GetProperty()->SetInterpolationToFlat();
		m_AssemblyFront->AddPart(m_GhostActor);
  }
}
//----------------------------------------------------------------------------
void mafPipeVolumeStructuredSlice::CreateSlice(int mode)
//----------------------------------------------------------------------------
{
	double srange[2],w,l,slice_bounds[6];
  int slice_extent[6];

  m_VolumeData->GetScalarRange(srange);
  m_VolumeData->GetExtent(m_VolumeExtent);
  m_VolumeData->GetSpacing(m_VolumeSpacing);
  m_VolumeData->GetBounds(m_VolumeBounds);

	w = srange[1] - srange[0];
	l = (srange[1] + srange[0]) * 0.5;

  if (!m_SliceParametersInitialized)
  {
    m_Origin[mode] = (m_VolumeBounds[2*mode]+m_VolumeBounds[2*mode+1])*.5;
  }

  memcpy(slice_extent,m_VolumeExtent,6*sizeof(int));
	m_SliceNumber[mode] = (int)((m_Origin[mode]-m_VolumeBounds[2*mode])/m_VolumeSpacing[mode]);
  slice_extent[2*mode]      = m_SliceNumber[mode];
  slice_extent[2*mode + 1]  = m_SliceNumber[mode];

  vtkNEW(m_SlicerImage[mode]);
	m_SlicerImage[mode]->SetInput(m_VolumeData);
  m_SlicerImage[mode]->SetVOI(slice_extent);
  m_SlicerImage[mode]->Update();

  vtkNEW(m_ColorLUT[mode]);
  m_ColorLUT[mode]->SetRange(srange);
  m_ColorLUT[mode]->Build();

  vtkNEW(m_GrayLUT[mode]);
  m_GrayLUT[mode]->SetWindow(w);
  m_GrayLUT[mode]->SetLevel(l);
  m_GrayLUT[mode]->Build();

	vtkNEW(m_Texture[mode]);
	m_Texture[mode]->RepeatOff();
	m_Texture[mode]->InterpolateOn();
	m_Texture[mode]->SetQualityTo32Bit();
	m_Texture[mode]->SetInput(m_SlicerImage[mode]->GetOutput());
  m_Texture[mode]->SetLookupTable(m_GrayLUT[mode]);
  m_Texture[mode]->MapColorScalarsThroughLookupTableOn();

  memcpy(slice_bounds,m_VolumeBounds,6*sizeof(double));
  m_Origin[mode] = m_VolumeBounds[2*mode] + m_SliceNumber[mode]*m_VolumeSpacing[mode];
  slice_bounds[2*mode]      = m_Origin[mode];
  slice_bounds[2*mode + 1]  = m_Origin[mode];

  vtkNEW(m_SlicePolygon[mode]);
  switch(mode) 
  {
    case SLICE_X:
      m_SlicePolygon[mode]->SetOrigin(slice_bounds[0],slice_bounds[2],slice_bounds[4]);
      m_SlicePolygon[mode]->SetPoint1(slice_bounds[0],slice_bounds[3],slice_bounds[4]);
      m_SlicePolygon[mode]->SetPoint2(slice_bounds[0],slice_bounds[2],slice_bounds[5]);
  	break;
    case SLICE_Y:
      m_SlicePolygon[mode]->SetOrigin(slice_bounds[0],slice_bounds[2],slice_bounds[4]);
      m_SlicePolygon[mode]->SetPoint1(slice_bounds[1],slice_bounds[2],slice_bounds[4]);
      m_SlicePolygon[mode]->SetPoint2(slice_bounds[0],slice_bounds[2],slice_bounds[5]);
    break;
    default:
      m_SlicePolygon[mode]->SetOrigin(slice_bounds[0],slice_bounds[2],slice_bounds[4]);
      m_SlicePolygon[mode]->SetPoint1(slice_bounds[1],slice_bounds[2],slice_bounds[4]);
      m_SlicePolygon[mode]->SetPoint2(slice_bounds[0],slice_bounds[3],slice_bounds[4]);
    break;
  }
  m_SlicePolygon[mode]->Update();

	vtkNEW(m_SliceMapper[mode]);
	m_SliceMapper[mode]->SetInput(m_SlicePolygon[mode]->GetOutput());
	m_SliceMapper[mode]->ScalarVisibilityOff();

	vtkNEW(m_SliceActor[mode]);
	m_SliceActor[mode]->SetMapper(m_SliceMapper[mode]);
	m_SliceActor[mode]->SetTexture(m_Texture[mode]);
	m_SliceActor[mode]->GetProperty()->SetAmbient(1.f);
	m_SliceActor[mode]->GetProperty()->SetDiffuse(0.f);
  m_SliceActor[mode]->GetProperty()->SetOpacity(m_SliceOpacity);

	m_AssemblyFront->AddPart(m_SliceActor[mode]);
}
//----------------------------------------------------------------------------
mafPipeVolumeStructuredSlice::~mafPipeVolumeStructuredSlice()
//----------------------------------------------------------------------------
{
	if(m_VolumeBoxActor)
    m_AssemblyFront->RemovePart(m_VolumeBoxActor);

	for(int i = 0; i<3; i++)
	{
		if(m_SliceActor[i])
      m_AssemblyFront->RemovePart(m_SliceActor[i]);
		vtkDEL(m_SlicerImage[i]);
		vtkDEL(m_Texture[i]);
    vtkDEL(m_GrayLUT[i]);
    vtkDEL(m_ColorLUT[i]);
		vtkDEL(m_SliceMapper[i]);
		vtkDEL(m_SlicePolygon[i]);
		vtkDEL(m_SliceActor[i]);
	}
	vtkDEL(m_VolumeBox);
	vtkDEL(m_VolumeBoxMapper);
	vtkDEL(m_VolumeBoxActor);

  if(m_GhostActor) 
    m_AssemblyFront->RemovePart(m_GhostActor);
  vtkDEL(m_GhostActor);
}
//----------------------------------------------------------------------------
void mafPipeVolumeStructuredSlice::Select(bool sel)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipeVolumeStructuredSlice::SetLutRange(double low, double hi)
//----------------------------------------------------------------------------
{
	for(int i=0;i<3;i++)
	{
		if(m_GrayLUT[i])
		{
      m_GrayLUT[i]->SetWindow(hi - low);
      m_GrayLUT[i]->SetLevel((low + hi)*0.5);
      m_GrayLUT[i]->Build();
		}
	}
}
//----------------------------------------------------------------------------
void mafPipeVolumeStructuredSlice::GetLutRange(double range[2])
//----------------------------------------------------------------------------
{
	if(m_SliceMode != SLICE_ORTHO)
    m_SlicerImage[m_SliceMode]->GetOutput()->GetScalarRange(range);
	else
    m_SlicerImage[0]->GetOutput()->GetScalarRange(range);
}
//----------------------------------------------------------------------------
void mafPipeVolumeStructuredSlice::SetSlice(double origin[3])
//----------------------------------------------------------------------------
{
  double slice_bounds[6],sr[2];
  int slice_extent[6];
  memcpy(m_Origin,origin,3*sizeof(double));

	for(int i=0;i<3;i++)
	{
		if(m_SlicerImage[i])
		{
      m_SliceNumber[i] = (int)((m_Origin[i]-m_VolumeBounds[2*i])/m_VolumeSpacing[i]);
      memcpy(slice_extent,m_VolumeExtent,6*sizeof(int));
      slice_extent[2*i]      = m_SliceNumber[i];
      slice_extent[2*i + 1]  = m_SliceNumber[i];
      m_SlicerImage[i]->SetVOI(slice_extent);
      m_SlicerImage[i]->UpdateWholeExtent();
      m_SlicerImage[i]->Update();
      m_SlicerImage[i]->GetOutput()->GetScalarRange(sr);

      memcpy(slice_bounds,m_VolumeBounds,6*sizeof(double));
      //m_Origin[i] = m_VolumeBounds[2*i] + m_SliceNumber[i]*m_VolumeSpacing[i];
      slice_bounds[2*i]      = m_Origin[i];
      slice_bounds[2*i + 1]  = m_Origin[i];
      switch(i) 
      {
        case SLICE_X:
          m_SlicePolygon[0]->SetOrigin(slice_bounds[0],slice_bounds[2],slice_bounds[4]);
          m_SlicePolygon[0]->SetPoint1(slice_bounds[0],slice_bounds[3],slice_bounds[4]);
          m_SlicePolygon[0]->SetPoint2(slice_bounds[0],slice_bounds[2],slice_bounds[5]);
        break;
        case SLICE_Y:
          m_SlicePolygon[1]->SetOrigin(slice_bounds[0],slice_bounds[2],slice_bounds[4]);
          m_SlicePolygon[1]->SetPoint1(slice_bounds[1],slice_bounds[2],slice_bounds[4]);
          m_SlicePolygon[1]->SetPoint2(slice_bounds[0],slice_bounds[2],slice_bounds[5]);
        break;
        default:
          m_SlicePolygon[2]->SetOrigin(slice_bounds[0],slice_bounds[2],slice_bounds[4]);
          m_SlicePolygon[2]->SetPoint1(slice_bounds[1],slice_bounds[2],slice_bounds[4]);
          m_SlicePolygon[2]->SetPoint2(slice_bounds[0],slice_bounds[3],slice_bounds[4]);
        break;
      }
      m_SlicePolygon[i]->Update();
    }
	}
}
//----------------------------------------------------------------------------
void mafPipeVolumeStructuredSlice::ColorLookupTable(bool enable)
//----------------------------------------------------------------------------
{
  for (int i=0;i<3;i++)
  {
    if(m_Texture[i])
    {
      if(enable)
      {
        m_Texture[i]->SetLookupTable(m_ColorLUT[i]);
      }
      else
      {
        m_Texture[i]->SetLookupTable(m_GrayLUT[i]);
      }
    }
  }
  m_ColorLUTEnabled = enable ? 1 : 0;
}
//----------------------------------------------------------------------------
void mafPipeVolumeStructuredSlice::SetSliceOpacity(float opacity)
//----------------------------------------------------------------------------
{
  m_SliceOpacity = opacity;
  
  for (int i=0;i<3;i++)
  {
    if(m_SliceActor[i])
      m_SliceActor[i]->GetProperty()->SetOpacity(m_SliceOpacity);
  }
}
//----------------------------------------------------------------------------
float mafPipeVolumeStructuredSlice::GetSliceOpacity()
//----------------------------------------------------------------------------
{
  return m_SliceOpacity;
}
//----------------------------------------------------------------------------
bool mafPipeVolumeStructuredSlice::IsColorLookupTable()
//----------------------------------------------------------------------------
{
  return m_ColorLUTEnabled != 0;
}

//----------------------------------------------------------------------------
mmgGui *mafPipeVolumeStructuredSlice::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  double b[6] = {-1,1,-1,1,-1,1};
  m_Gui = new mmgGui(this);
  m_Gui->Bool(ID_RGB_LUT,"rgb lut", &m_ColorLUTEnabled,0,"turn on/off RGB LUT");
  if (m_VolumeData != NULL)
  {
    m_VolumeData->Update();
    m_VolumeData->GetBounds(b);
  }
  if (m_SliceMode == SLICE_X || m_SliceMode == SLICE_ORTHO)
  {
    m_SliceSlider[0] = m_Gui->FloatSlider(ID_SLICE_SLIDER_X,"x",&m_Origin[0],b[0],b[1]);
  }
  if (m_SliceMode == SLICE_Y || m_SliceMode == SLICE_ORTHO)
  {
    m_SliceSlider[1] = m_Gui->FloatSlider(ID_SLICE_SLIDER_Y,"y",&m_Origin[1],b[2],b[3]);
  }
  if (m_SliceMode == SLICE_Z || m_SliceMode == SLICE_ORTHO)
  {
    m_SliceSlider[2] = m_Gui->FloatSlider(ID_SLICE_SLIDER_Z,"z",&m_Origin[2],b[4],b[5]);
  }

  return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeVolumeStructuredSlice::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
      case ID_RGB_LUT:
      {
        ColorLookupTable(m_ColorLUTEnabled != 0);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
      case ID_SLICE_SLIDER_X:
      case ID_SLICE_SLIDER_Y:
      case ID_SLICE_SLIDER_Z:
        SetSlice(m_Origin);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
      default:
      break;
    }
  }
}
