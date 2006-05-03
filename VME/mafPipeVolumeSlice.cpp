/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeVolumeSlice.cpp,v $
  Language:  C++
  Date:      $Date: 2006-05-03 10:20:18 $
  Version:   $Revision: 1.27 $
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

#include "mafPipeVolumeSlice.h"
#include "mafDecl.h"
#include "mmgLutPreset.h"
#include "mmgGui.h"
#include "mmgMaterialButton.h"
#include "mafSceneNode.h"
#include "mafVMEVolume.h"
#include "mafVMEOutputVolume.h"
#include "mafTransformBase.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"
#include "vtkMath.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkTexture.h"
#include "vtkPolyDataMapper.h"
#include "vtkLookupTable.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkStructuredPoints.h"
#include "vtkRectilinearGrid.h"
#include "vtkVolumeSlicer.h"
#include "vtkProperty.h"
#include "vtkDataSet.h"
#include "vtkTransform.h"
#include "vtkRenderer.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeVolumeSlice);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeVolumeSlice::mafPipeVolumeSlice()
:mafPipe()
//----------------------------------------------------------------------------
{ 
  for(int i = 0; i<3; i++)
  {
    m_SlicerPolygonal[i]	= NULL;
    m_SlicerImage[i]			= NULL;
    m_Image[i]            = NULL;
    m_Texture[i]		      = NULL;
    m_SlicePolydata[i]		= NULL;
    m_SliceMapper[i]		  = NULL;
    m_SliceActor[i]	      = NULL;
    m_SliceSlider[i]      = NULL;
  }
  m_SliceParametersInitialized  = false;
  m_ShowVolumeBox               = false;
  
  m_AssemblyUsed = NULL;
  m_ColorLUT  = NULL;
  m_CustomColorLUT = NULL;

  m_SliceDirection  = SLICE_Z;
  m_SliceOpacity  = 1.0;
  m_TextureRes    = 256;

  m_XVector[0][0] = 0.0001;	//should be 0 !!! but there is a bug into vtkVolumeSlicer filter
  m_XVector[0][1] = 1;
  m_XVector[0][2] = 0;
  m_YVector[0][0] = 0;
  m_YVector[0][1] = 0;
  m_YVector[0][2] = 1;

  m_XVector[1][0] = 0;
  m_XVector[1][1] = 0;
  m_XVector[1][2] = 1;
  m_YVector[1][0] = 1;
  m_YVector[1][1] = 0;
  m_YVector[1][2] = 0;

  m_XVector[2][0] = 1;
  m_XVector[2][1] = 0;
  m_XVector[2][2] = 0;
  m_YVector[2][0] = 0;
  m_YVector[2][1] = 1;
  m_YVector[2][2] = 0;
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::InitializeSliceParameters(int direction, bool show_vol_bbox)
//----------------------------------------------------------------------------
{
  m_SliceDirection= direction;
  m_ShowVolumeBox = show_vol_bbox;
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::InitializeSliceParameters(int direction, double slice_origin[3], bool show_vol_bbox)
//----------------------------------------------------------------------------
{
  m_SliceParametersInitialized = true;
  m_SliceDirection= direction;
  m_ShowVolumeBox = show_vol_bbox;

  m_Origin[0] = slice_origin[0];
  m_Origin[1] = slice_origin[1];
  m_Origin[2] = slice_origin[2];
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::InitializeSliceParameters(int direction, double slice_origin[3], float slice_xVect[3], float slice_yVect[3], bool show_vol_bbox)
//----------------------------------------------------------------------------
{
  m_SliceParametersInitialized = true;
  
  m_SliceDirection= direction;
  m_ShowVolumeBox = show_vol_bbox;

  m_Origin[0] = slice_origin[0];
  m_Origin[1] = slice_origin[1];
  m_Origin[2] = slice_origin[2];

  if(m_SliceDirection != SLICE_ORTHO)
  {
    m_XVector[m_SliceDirection][0] = slice_xVect[0];
    m_XVector[m_SliceDirection][1] = slice_xVect[1];
    m_XVector[m_SliceDirection][2] = slice_xVect[2];

    m_YVector[m_SliceDirection][0] = slice_yVect[0];
    m_YVector[m_SliceDirection][1] = slice_yVect[1];
    m_YVector[m_SliceDirection][2] = slice_yVect[2];

    vtkMath::Normalize(m_XVector[m_SliceDirection]);
    vtkMath::Normalize(m_YVector[m_SliceDirection]);
    vtkMath::Cross(m_YVector[m_SliceDirection], m_XVector[m_SliceDirection], m_Normal[m_SliceDirection]);
    vtkMath::Normalize(m_Normal[m_SliceDirection]);
    vtkMath::Cross(m_Normal[m_SliceDirection], m_XVector[m_SliceDirection], m_YVector[m_SliceDirection]);
    vtkMath::Normalize(m_YVector[m_SliceDirection]);
  }
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n); // Always call this to initialize m_Vme, m_AssemblyFront, ... vars

  m_AssemblyUsed = m_AssemblyBack ? m_AssemblyBack : m_AssemblyFront;

  assert(m_Vme->IsMAFType(mafVMEVolume));
  double b[6];
  m_Vme->GetOutput()->Update();
  m_Vme->GetOutput()->GetVTKData()->Update();
  m_Vme->GetOutput()->GetVMELocalBounds(b);

  double srange[2];
  m_Vme->GetOutput()->GetVTKData()->GetScalarRange(srange);
  
  vtkNEW(m_ColorLUT);
  m_ColorLUT->SetRange(srange);
  m_ColorLUT->Build();
  lutPreset(4,m_ColorLUT);

	if(m_SliceDirection == SLICE_ARB)
		m_SliceDirection = SLICE_X;

	if (!m_SliceParametersInitialized)
	{
    m_Origin[0] = (b[0] + b[1])*.5;
    m_Origin[1] = (b[2] + b[3])*.5;
    m_Origin[2] = (b[4] + b[5])*.5;
	}

	if(m_SliceDirection == SLICE_ORTHO)
	{
    // overwrite the plane vector, because the slices have to be orthogonal
    m_XVector[0][0] = 0.0001;	//modified by Paolo 29-10-2003 should be 0 !!! check into Sasha's filter
    m_XVector[0][1] = 1;
    m_XVector[0][2] = 0;
    m_YVector[0][0] = 0;
    m_YVector[0][1] = 0;
    m_YVector[0][2] = 1;

    m_XVector[1][0] = 0;
    m_XVector[1][1] = 0;
    m_XVector[1][2] = 1;
    m_YVector[1][0] = 1;
    m_YVector[1][1] = 0;
    m_YVector[1][2] = 0;

    m_XVector[2][0] = 1;
    m_XVector[2][1] = 0;
    m_XVector[2][2] = 0;
    m_YVector[2][0] = 0;
    m_YVector[2][1] = 1;
    m_YVector[2][2] = 0;

    int i;
    for(i = 0; i<3; i++)
		{
			vtkMath::Normalize(m_XVector[i]);
			vtkMath::Normalize(m_YVector[i]);
			vtkMath::Cross(m_YVector[i], m_XVector[i], m_Normal[i]);
			vtkMath::Normalize(m_Normal[i]);
			vtkMath::Cross(m_Normal[i], m_XVector[i], m_YVector[i]);
			vtkMath::Normalize(m_YVector[i]);
			CreateSlice(i);
		}
	}
	else
	{
		vtkMath::Normalize(m_XVector[m_SliceDirection]);
		vtkMath::Normalize(m_YVector[m_SliceDirection]);
		vtkMath::Cross(m_YVector[m_SliceDirection], m_XVector[m_SliceDirection], m_Normal[m_SliceDirection]);
		vtkMath::Normalize(m_Normal[m_SliceDirection]);
		vtkMath::Cross(m_Normal[m_SliceDirection], m_XVector[m_SliceDirection], m_YVector[m_SliceDirection]);
		vtkMath::Normalize(m_YVector[m_SliceDirection]);
		CreateSlice(m_SliceDirection);
	}

	if(m_ShowVolumeBox)
	{
		vtkNEW(m_VolumeBox);
		m_VolumeBox->SetInput(m_Vme->GetOutput()->GetVTKData());

		vtkNEW(m_VolumeBoxMapper);
		m_VolumeBoxMapper->SetInput(m_VolumeBox->GetOutput());

		vtkNEW(m_VolumeBoxActor);
		m_VolumeBoxActor->SetMapper(m_VolumeBoxMapper);

		m_AssemblyFront->AddPart(m_VolumeBoxActor);
	}
	else
	{
		m_VolumeBox				= NULL;
		m_VolumeBoxMapper = NULL;
		m_VolumeBoxActor	= NULL;
	}

	// if the actor is in the background renderer
	// create something invisible in the front renderer so that ResetCamera will work
  m_GhostActor = NULL;
  if(m_AssemblyBack != NULL)
	{
		vtkNEW(m_GhostActor);
		m_GhostActor->SetMapper(m_SliceMapper[m_SliceDirection]);
		m_GhostActor->PickableOff();
		m_GhostActor->GetProperty()->SetOpacity(0);
		m_GhostActor->GetProperty()->SetRepresentationToPoints();
		m_GhostActor->GetProperty()->SetInterpolationToFlat();
		m_AssemblyFront->AddPart(m_GhostActor);
  }
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::CreateSlice(int direction)
//----------------------------------------------------------------------------
{
	double srange[2],w,l, xspc = 0.33, yspc = 0.33;

  vtkDataSet *vtk_data = m_Vme->GetOutput()->GetVTKData();
  vtk_data->Update();
  vtk_data->GetScalarRange(srange);

	w = srange[1] - srange[0];
	l = (srange[1] + srange[0]) * 0.5;

	vtkNEW(m_SlicerPolygonal[direction]);
	vtkNEW(m_SlicerImage[direction]);
	m_SlicerImage[direction]->SetPlaneOrigin(m_Origin[0], m_Origin[1], m_Origin[2]);
	m_SlicerPolygonal[direction]->SetPlaneOrigin(m_SlicerImage[direction]->GetPlaneOrigin());
	m_SlicerImage[direction]->SetPlaneAxisX(m_XVector[direction]);
	m_SlicerImage[direction]->SetPlaneAxisY(m_YVector[direction]);
	m_SlicerPolygonal[direction]->SetPlaneAxisX(m_XVector[direction]);
	m_SlicerPolygonal[direction]->SetPlaneAxisY(m_YVector[direction]);
	m_SlicerImage[direction]->SetInput(vtk_data);
	m_SlicerPolygonal[direction]->SetInput(vtk_data);
//  m_SlicerImage[direction]->SetSliceTransform(m_Vme->GetOutput()->GetAbsTransform()->GetVTKTransform()->GetLinearInverse());
//  m_SlicerPolygonal[direction]->SetSliceTransform(m_Vme->GetOutput()->GetAbsTransform()->GetVTKTransform()->GetLinearInverse());
  
	vtkNEW(m_Image[direction]);
  //m_Image[direction]->SetScalarType(vtk_data->GetPointData()->GetScalars()->GetDataType());
  m_Image[direction]->SetScalarTypeToUnsignedChar();
	//m_Image[direction]->SetNumberOfScalarComponents(vtk_data->GetPointData()->GetScalars()->GetNumberOfComponents());
  m_Image[direction]->SetNumberOfScalarComponents(3);
	m_Image[direction]->SetExtent(0, m_TextureRes - 1, 0, m_TextureRes - 1, 0, 0);
	m_Image[direction]->SetSpacing(xspc, yspc, 1.f);

	m_SlicerImage[direction]->SetOutput(m_Image[direction]);
  m_SlicerImage[direction]->Update();
  m_SlicerImage[direction]->SetWindow(w);
  m_SlicerImage[direction]->SetLevel(l);

	vtkNEW(m_Texture[direction]);
	m_Texture[direction]->RepeatOff();
	m_Texture[direction]->InterpolateOn();
	m_Texture[direction]->SetQualityTo32Bit();
	m_Texture[direction]->SetInput(m_Image[direction]);
  m_Texture[direction]->SetLookupTable(m_ColorLUT);
  //m_Texture[direction]->MapColorScalarsThroughLookupTableOn();

  vtkNEW(m_SlicePolydata[direction]);
	m_SlicerPolygonal[direction]->SetOutput(m_SlicePolydata[direction]);
	m_SlicerPolygonal[direction]->SetTexture(m_Image[direction]);
	m_SlicerPolygonal[direction]->Update();

	vtkNEW(m_SliceMapper[direction]);
	m_SliceMapper[direction]->SetInput(m_SlicePolydata[direction]);
	m_SliceMapper[direction]->ScalarVisibilityOff();

	vtkNEW(m_SliceActor[direction]);
	m_SliceActor[direction]->SetMapper(m_SliceMapper[direction]);
	m_SliceActor[direction]->SetTexture(m_Texture[direction]);
	m_SliceActor[direction]->GetProperty()->SetAmbient(1.f);
	m_SliceActor[direction]->GetProperty()->SetDiffuse(0.f);
  m_SliceActor[direction]->GetProperty()->SetOpacity(m_SliceOpacity);

	m_AssemblyUsed->AddPart(m_SliceActor[direction]);
}
//----------------------------------------------------------------------------
mafPipeVolumeSlice::~mafPipeVolumeSlice()
//----------------------------------------------------------------------------
{
	if(m_VolumeBoxActor)
    m_AssemblyFront->RemovePart(m_VolumeBoxActor);

	for(int i = 0; i<3; i++)
	{
		if(m_SliceActor[i])
      m_AssemblyUsed->RemovePart(m_SliceActor[i]);
		if (m_SlicerImage[i])
		{
      m_SlicerImage[i]->SetSliceTransform(NULL);
		}
    if (m_SlicerPolygonal[i])
    {
      m_SlicerPolygonal[i]->SetSliceTransform(NULL);
    }
    vtkDEL(m_SlicerImage[i]);
		vtkDEL(m_SlicerPolygonal[i]);
		vtkDEL(m_Image[i]);
		vtkDEL(m_Texture[i]);
		vtkDEL(m_SliceMapper[i]);
		vtkDEL(m_SlicePolydata[i]);
		vtkDEL(m_SliceActor[i]);
	}
  vtkDEL(m_ColorLUT);
	vtkDEL(m_VolumeBox);
	vtkDEL(m_VolumeBoxMapper);
	vtkDEL(m_VolumeBoxActor);

  if(m_GhostActor) 
    m_AssemblyFront->RemovePart(m_GhostActor);
  vtkDEL(m_GhostActor);
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::SetLutRange(double low, double high)
//----------------------------------------------------------------------------
{
	for(int i=0;i<3;i++)
	{
		if(m_SlicerImage[i])
		{
      m_SlicerImage[i]->SetWindow(high-low);
      m_SlicerImage[i]->SetLevel((low+high)*.5);
      m_SlicerImage[i]->Update();
      //m_Texture[i]->GetLookupTable()->SetRange(low,high);
      //m_Texture[i]->GetLookupTable()->Build();
		}
	}
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::GetLutRange(double range[2])
//----------------------------------------------------------------------------
{
	if(m_SliceDirection != SLICE_ORTHO)
    m_Texture[m_SliceDirection]->GetLookupTable()->GetRange();
	else
    m_Texture[0]->GetLookupTable()->GetRange();
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::SetSlice(double origin[3], float xVect[3], float yVect[3])
//----------------------------------------------------------------------------
{
	m_XVector[m_SliceDirection][0] = xVect[0];
	m_XVector[m_SliceDirection][1] = xVect[1];
	m_XVector[m_SliceDirection][2] = xVect[2];

	m_YVector[m_SliceDirection][0] = yVect[0];
	m_YVector[m_SliceDirection][1] = yVect[1];
	m_YVector[m_SliceDirection][2] = yVect[2];

	vtkMath::Normalize(m_XVector[m_SliceDirection]);
	vtkMath::Normalize(m_YVector[m_SliceDirection]);
	vtkMath::Cross(m_YVector[m_SliceDirection], m_XVector[m_SliceDirection], m_Normal[m_SliceDirection]);
	vtkMath::Normalize(m_Normal[m_SliceDirection]);
	vtkMath::Cross(m_Normal[m_SliceDirection], m_XVector[m_SliceDirection], m_YVector[m_SliceDirection]);
	vtkMath::Normalize(m_YVector[m_SliceDirection]);

  SetSlice(origin);
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::SetSlice(double origin[3])
//----------------------------------------------------------------------------
{
	m_Origin[0] = origin[0];
	m_Origin[1] = origin[1];
	m_Origin[2] = origin[2];

	for(int i=0;i<3;i++)
	{
		if(m_SlicerImage[i])
		{
			m_SlicerImage[i]->SetPlaneOrigin(m_Origin[0], m_Origin[1], m_Origin[2]);
			m_SlicerPolygonal[i]->SetPlaneOrigin(m_SlicerImage[i]->GetPlaneOrigin());
			m_SlicerImage[i]->SetPlaneAxisX(m_XVector[i]);
			m_SlicerImage[i]->SetPlaneAxisY(m_YVector[i]);
			m_SlicerPolygonal[i]->SetPlaneAxisX(m_XVector[i]);
			m_SlicerPolygonal[i]->SetPlaneAxisY(m_YVector[i]);

			m_SlicerImage[i]->Update();
			m_SlicerPolygonal[i]->Update();
    }
	}
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::GetSliceOrigin(double origin[3])
//----------------------------------------------------------------------------
{
  origin[0] = m_Origin[0];
  origin[1] = m_Origin[1];
  origin[2] = m_Origin[2];
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::GetSliceNormal(double normal[3])
//----------------------------------------------------------------------------
{
	normal[0] = m_Normal[m_SliceDirection][0];
	normal[1] = m_Normal[m_SliceDirection][1];
	normal[2] = m_Normal[m_SliceDirection][2];
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::SetSliceOpacity(float opacity)
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
float mafPipeVolumeSlice::GetSliceOpacity()
//----------------------------------------------------------------------------
{
  return m_SliceOpacity;
}
//----------------------------------------------------------------------------
mmgGui *mafPipeVolumeSlice::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  double b[6] = {-1,1,-1,1,-1,1};
  m_Gui = new mmgGui(this);
  m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);
  m_Vme->GetOutput()->GetVMELocalBounds(b);
  if (m_SliceDirection == SLICE_X || m_SliceDirection == SLICE_ORTHO)
  {
    m_SliceSlider[0] = m_Gui->FloatSlider(ID_SLICE_SLIDER_X,"x",&m_Origin[0],b[0],b[1]);
  }
  if (m_SliceDirection == SLICE_Y || m_SliceDirection == SLICE_ORTHO)
  {
    m_SliceSlider[1] = m_Gui->FloatSlider(ID_SLICE_SLIDER_Y,"y",&m_Origin[1],b[2],b[3]);
  }
  if (m_SliceDirection == SLICE_Z || m_SliceDirection == SLICE_ORTHO)
  {
    m_SliceSlider[2] = m_Gui->FloatSlider(ID_SLICE_SLIDER_Z,"z",&m_Origin[2],b[4],b[5]);
  }

  return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
      case ID_LUT_CHOOSER:
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
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
  else
  {
    mafEventMacro(*maf_event);
  }
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::SetColorLookupTable(vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
  int i;
  if (lut == NULL)
  {
    for (i=0;i<3;i++)
    {
      if(m_Texture[i])
        m_Texture[i]->SetLookupTable(m_ColorLUT);
    }
    return;
  }
  m_CustomColorLUT = lut;
  for (i=0;i<3;i++)
  {
    if(m_Texture[i])
      m_Texture[i]->SetLookupTable(m_CustomColorLUT);
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
