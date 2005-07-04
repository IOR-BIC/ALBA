/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeVolumeSlice.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-04 16:08:30 $
  Version:   $Revision: 1.5 $
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
#include "mmgGui.h"
#include "mmgMaterialButton.h"
#include "mafSceneNode.h"

#include "mafVMEVolume.h"
#include "mafVMEOutputVolume.h"

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
    m_ColourLUT[i]        = NULL;
    m_SlicePolydata[i]		= NULL;
    m_SliceMapper[i]		  = NULL;
    m_SliceActor[i]	      = NULL;
  }
  m_ParametersInitialized = false;
  m_ShowVolumeBox         = false;

  m_SliceMode = SLICE_Z;

  m_ColorLUTEnabled       = 0;
  m_SliceOpacity  = 1.0;
  m_TextureRes    = 512;

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
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::InitializeSliceParameters(int mode, double slice_origin[3], float slice_xVect[3], float slice_yVect[3], bool show_vol_bbox)
//----------------------------------------------------------------------------
{
  m_SliceMode     = mode;
  m_ShowVolumeBox = show_vol_bbox;

  m_Origin[0] = slice_origin[0];
  m_Origin[1] = slice_origin[1];
  m_Origin[2] = slice_origin[2];

  if(m_SliceMode != SLICE_ORTHO)
  {
    m_XVector[m_SliceMode][0] = slice_xVect[0];
    m_XVector[m_SliceMode][1] = slice_xVect[1];
    m_XVector[m_SliceMode][2] = slice_xVect[2];

    m_YVector[m_SliceMode][0] = slice_yVect[0];
    m_YVector[m_SliceMode][1] = slice_yVect[1];
    m_YVector[m_SliceMode][2] = slice_yVect[2];

    vtkMath::Normalize(m_XVector[m_SliceMode]);
    vtkMath::Normalize(m_YVector[m_SliceMode]);
    vtkMath::Cross(m_YVector[m_SliceMode], m_XVector[m_SliceMode], m_Normal[m_SliceMode]);
    vtkMath::Normalize(m_Normal[m_SliceMode]);
    vtkMath::Cross(m_Normal[m_SliceMode], m_XVector[m_SliceMode], m_YVector[m_SliceMode]);
    vtkMath::Normalize(m_YVector[m_SliceMode]);
  }
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n); // Always call this to initialize m_Vme, m_AssemblyFront, ... vars

  assert(m_Vme->IsMAFType(mafVMEVolume));
  //mafVMEVolume *vme = ((mafVMEVolume *) m_Vme);
  m_Vme->GetOutput()->Update();
  m_Vme->GetOutput()->GetVTKData()->Update();

	if(m_SliceMode == SLICE_ARB)
		m_SliceMode = SLICE_X;

	if (!m_ParametersInitialized)
	{
    double b[6];
    m_Vme->GetOutput()->GetBounds(b);
    m_Origin[0] = (b[0] + b[1])*.5;
    m_Origin[1] = (b[2] + b[3])*.5;
    m_Origin[2] = (b[4] + b[5])*.5;
	}

	if(m_SliceMode == SLICE_ORTHO)
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
		vtkMath::Normalize(m_XVector[m_SliceMode]);
		vtkMath::Normalize(m_YVector[m_SliceMode]);
		vtkMath::Cross(m_YVector[m_SliceMode], m_XVector[m_SliceMode], m_Normal[m_SliceMode]);
		vtkMath::Normalize(m_Normal[m_SliceMode]);
		vtkMath::Cross(m_Normal[m_SliceMode], m_XVector[m_SliceMode], m_YVector[m_SliceMode]);
		vtkMath::Normalize(m_YVector[m_SliceMode]);
		CreateSlice(m_SliceMode);
	}

	if(m_ShowVolumeBox)
	{
		m_VolumeBox = vtkOutlineCornerFilter::New();
		m_VolumeBox->SetInput(m_Vme->GetOutput()->GetVTKData());

		m_VolumeBoxMapper = vtkPolyDataMapper::New();
		m_VolumeBoxMapper->SetInput(m_VolumeBox->GetOutput());

		m_VolumeBoxActor = vtkActor::New();
		//m_VolumeBoxActor->SetProperty(((mafVmeData*)m_vme->GetClientData())->m_mat_gui->GetMaterial()->m_prop);
		m_VolumeBoxActor->SetMapper(m_VolumeBoxMapper);

		m_AssemblyFront->AddPart(m_VolumeBoxActor);
	}
	else
	{
		m_VolumeBox				= NULL;
		m_VolumeBoxMapper = NULL;
		m_VolumeBoxActor	= NULL;
	}

	//ghost - SIL. 26-5-2003 
	// if the actor is in the background renderer
	// create something invisible in the front renderer so that ResetCamera will work
  m_GhostActor = NULL;
  if(m_AssemblyBack != NULL)
	{
		m_GhostActor = vtkActor::New();
		m_GhostActor->SetMapper(m_SliceMapper[m_SliceMode]);
		m_GhostActor->PickableOff();
		m_GhostActor->GetProperty()->SetOpacity(0);
		m_GhostActor->GetProperty()->SetRepresentationToPoints();
		m_GhostActor->GetProperty()->SetInterpolationToFlat();
		m_AssemblyFront->AddPart(m_GhostActor);
  }
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::CreateSlice(int mode)
//----------------------------------------------------------------------------
{
	double srange[2],w,l, xspc = 0.33, yspc = 0.33;

	//mafVMEVolume *vme = ((mafVMEVolume *) m_Vme);
  vtkDataSet *vtk_data = m_Vme->GetOutput()->GetVTKData();
  if (vtk_data)
    vtk_data->GetScalarRange(srange);
  else
  {
    srange[0] = 0.0;
    srange[1] = 1.0;
  }

	w = srange[1] - srange[0];
	l = (srange[1] + srange[0]) * 0.5;

	m_SlicerPolygonal[mode]= vtkVolumeSlicer::New();
	m_SlicerImage[mode]	= vtkVolumeSlicer::New();
	m_SlicerImage[mode]->SetPlaneOrigin(m_Origin[0], m_Origin[1], m_Origin[2]);
	m_SlicerPolygonal[mode]->SetPlaneOrigin(m_SlicerImage[mode]->GetPlaneOrigin());
	m_SlicerImage[mode]->SetPlaneAxisX(m_XVector[mode]);
	m_SlicerImage[mode]->SetPlaneAxisY(m_YVector[mode]);
	m_SlicerPolygonal[mode]->SetPlaneAxisX(m_XVector[mode]);
	m_SlicerPolygonal[mode]->SetPlaneAxisY(m_YVector[mode]);
	m_SlicerImage[mode]->SetInput(vtk_data);
	m_SlicerPolygonal[mode]->SetInput(vtk_data);

	m_Image[mode] = vtkImageData::New();
  m_Image[mode]->SetScalarTypeToUnsignedChar();
	m_Image[mode]->SetNumberOfScalarComponents(3);
	m_Image[mode]->SetExtent(0, m_TextureRes - 1, 0, m_TextureRes - 1, 0, 0);
	m_Image[mode]->SetSpacing(xspc, yspc, 1.f);

	m_SlicerImage[mode]->SetOutput(m_Image[mode]);
	m_SlicerImage[mode]->Update();
	m_SlicerImage[mode]->SetWindow(w);
	m_SlicerImage[mode]->SetLevel(l);

  m_ColourLUT[mode] = vtkLookupTable::New();
  
	m_Texture[mode] = vtkTexture::New();
	m_Texture[mode]->RepeatOff();
	m_Texture[mode]->InterpolateOn();
	m_Texture[mode]->SetQualityTo32Bit();
	m_Texture[mode]->SetInput(m_Image[mode]);
  m_Texture[mode]->SetLookupTable(m_ColourLUT[mode]);

  m_SlicePolydata[mode]	= vtkPolyData::New();
	m_SlicerPolygonal[mode]->SetOutput(m_SlicePolydata[mode]);
	m_SlicerPolygonal[mode]->SetTexture(m_Image[mode]);
	m_SlicerPolygonal[mode]->Update();

  double sr[2];
  m_Image[mode]->GetScalarRange(sr);
  m_ColourLUT[mode]->SetRange(sr);
  m_ColourLUT[mode]->Build();

	m_SliceMapper[mode]	= vtkPolyDataMapper::New();
	m_SliceMapper[mode]->SetInput(m_SlicePolydata[mode]);
	m_SliceMapper[mode]->ScalarVisibilityOff();

	m_SliceActor[mode] = vtkActor::New();
	m_SliceActor[mode]->SetMapper(m_SliceMapper[mode]);
	m_SliceActor[mode]->SetTexture(m_Texture[mode]);
	m_SliceActor[mode]->GetProperty()->SetAmbient(1.f);
	m_SliceActor[mode]->GetProperty()->SetDiffuse(0.f);
  m_SliceActor[mode]->GetProperty()->SetOpacity(m_SliceOpacity);

	m_AssemblyFront->AddPart(m_SliceActor[mode]);
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
      m_AssemblyFront->RemovePart(m_SliceActor[i]);
		vtkDEL(m_SlicerImage[i]);
		vtkDEL(m_SlicerPolygonal[i]);
		vtkDEL(m_Image[i]);
		vtkDEL(m_Texture[i]);
    vtkDEL(m_ColourLUT[i]);
		vtkDEL(m_SliceMapper[i]);
		vtkDEL(m_SlicePolydata[i]);
		vtkDEL(m_SliceActor[i]);
	}
	vtkDEL(m_VolumeBox);
	vtkDEL(m_VolumeBoxMapper);
	vtkDEL(m_VolumeBoxActor);

  if(m_GhostActor) 
    m_AssemblyFront->RemovePart(m_GhostActor);
  vtkDEL(m_GhostActor);
}
/*
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::Show(bool show)
//----------------------------------------------------------------------------
{
	if(m_GhostActor) m_GhostActor->SetVisibility(show);

	for(int i=0;i<3;i++)
		if(m_SliceActor[i]) m_SliceActor[i]->SetVisibility(show);
}
*/
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::Select(bool sel)
//----------------------------------------------------------------------------
{
  //slices doesn't hilight
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::SetLutRange(double low, double hi)
//----------------------------------------------------------------------------
{
	for(int i=0;i<3;i++)
	{
		if(m_SlicerImage[i])
		{
      m_SlicerImage[i]->SetWindow(hi - low);
      m_SlicerImage[i]->SetLevel((low + hi)*0.5);
      m_SlicerImage[i]->Update();
		}
	}
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::GetLutRange(double range[2])
//----------------------------------------------------------------------------
{
	if(m_SliceMode != SLICE_ORTHO)
		m_SlicerImage[m_SliceMode]->GetOutput()->GetScalarRange(range);
	else
		m_SlicerImage[0]->GetOutput()->GetScalarRange(range);
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::SetSlice(double origin[3], float xVect[3], float yVect[3])
//----------------------------------------------------------------------------
{
	m_Origin[0] = origin[0];
	m_Origin[1] = origin[1];
	m_Origin[2] = origin[2];

	m_XVector[m_SliceMode][0] = xVect[0];
	m_XVector[m_SliceMode][1] = xVect[1];
	m_XVector[m_SliceMode][2] = xVect[2];

	m_YVector[m_SliceMode][0] = yVect[0];
	m_YVector[m_SliceMode][1] = yVect[1];
	m_YVector[m_SliceMode][2] = yVect[2];

	vtkMath::Normalize(m_XVector[m_SliceMode]);
	vtkMath::Normalize(m_YVector[m_SliceMode]);
	vtkMath::Cross(m_YVector[m_SliceMode], m_XVector[m_SliceMode], m_Normal[m_SliceMode]);
	vtkMath::Normalize(m_Normal[m_SliceMode]);
	vtkMath::Cross(m_Normal[m_SliceMode], m_XVector[m_SliceMode], m_YVector[m_SliceMode]);
	vtkMath::Normalize(m_YVector[m_SliceMode]);

	m_SlicerImage[m_SliceMode]->SetPlaneOrigin(m_Origin[0], m_Origin[1], m_Origin[2]);
	m_SlicerPolygonal[m_SliceMode]->SetPlaneOrigin(m_SlicerImage[m_SliceMode]->GetPlaneOrigin());
	m_SlicerImage[m_SliceMode]->SetPlaneAxisX(m_XVector[m_SliceMode]);
	m_SlicerImage[m_SliceMode]->SetPlaneAxisY(m_YVector[m_SliceMode]);
	m_SlicerPolygonal[m_SliceMode]->SetPlaneAxisX(m_XVector[m_SliceMode]);
	m_SlicerPolygonal[m_SliceMode]->SetPlaneAxisY(m_YVector[m_SliceMode]);

	m_SlicerImage[m_SliceMode]->Update();
	m_SlicerPolygonal[m_SliceMode]->Update();
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
	normal[0] = m_Normal[m_SliceMode][0];
	normal[1] = m_Normal[m_SliceMode][1];
	normal[2] = m_Normal[m_SliceMode][2];
}
//----------------------------------------------------------------------------
void mafPipeVolumeSlice::ColorLookupTable(bool enable)
//----------------------------------------------------------------------------
{
  for (int i=0;i<3;i++)
  {
    if(m_Texture[i])
    {
      if(enable)
        m_Texture[i]->MapColorScalarsThroughLookupTableOn();
      else
        m_Texture[i]->MapColorScalarsThroughLookupTableOff();
    }
  }
  m_ColorLUTEnabled = enable ? 1 : 0;
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
bool mafPipeVolumeSlice::IsColorLookupTable()
//----------------------------------------------------------------------------
{
  return m_ColorLUTEnabled != 0;
}

//----------------------------------------------------------------------------
mmgGui *mafPipeVolumeSlice::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_Gui->Bool(ID_RGB_LUT,"rgb lut", &m_ColorLUTEnabled,0,"turn on/off RGB LUT");

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
      case ID_RGB_LUT:
      {
        ColorLookupTable(m_ColorLUTEnabled != 0);
        m_Vme->ForwardUpEvent(&mafEvent(this,CAMERA_UPDATE));
      }
      break;
      default:
      break;
    }
  }
}
