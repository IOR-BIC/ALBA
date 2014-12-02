/*=========================================================================

 Program: MAF2
 Module: mafPipeVolumeSliceBlend
 Authors: Paolo Quadrani
 
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

#include "mafPipeVolumeSliceBlend.h"
#include "medDecl.h"
#include "mafGUI.h"
#include "mafSceneNode.h"
#include "mmaVolumeMaterial.h"
#include "mafPipeVolumeSlice.h"
#include "mafTransformBase.h"
#include "mafVMEVolume.h"
#include "mafVMEOutputVolume.h"
#include "mafGUIFloatSlider.h"
#include "mafLODActor.h"
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
#include "vtkMAFVolumeSlicer.h"
#include "vtkProperty.h"
#include "vtkDataSet.h"
#include "vtkTransform.h"
#include "vtkRenderer.h"
#include "vtkOutlineSource.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeVolumeSliceBlend);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeVolumeSliceBlend::mafPipeVolumeSliceBlend()
:mafPipe()
//----------------------------------------------------------------------------
{
  //Initialize values
  for(int j=0;j<2;j++)
    for(int i = 0; i<3; i++)
    {
      m_SlicerPolygonal[j][i]		= NULL;
      m_SlicerImage[j][i]				= NULL;
      m_Image[j][i]							= NULL;
      m_Texture[j][i]						= NULL;
      m_SlicePolydata[j][i]			= NULL;
      m_SliceMapper[j][i]				= NULL;
      m_SliceActor[j][i]				= NULL;
    }
    m_SliceParametersInitialized  = false;
    m_ShowVolumeBox               = false;
    m_ShowBounds									= false;
    m_AssemblyUsed = NULL;
    m_ColorLUT  = NULL;
    m_CustomColorLUT = NULL;
    m_Box = NULL;
    m_Mapper = NULL;
    m_Actor = NULL;
    m_TickActor = NULL;
    m_SliceDirection  = SLICE_Z;
    m_SliceOpacity  = 1.0;
    m_TextureRes    = 512;
    ///Default value of X Y vectors
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
void mafPipeVolumeSliceBlend::InitializeSliceParameters(int direction, bool show_vol_bbox, bool show_bounds)
//----------------------------------------------------------------------------
{
  m_SliceDirection= direction;
  m_ShowVolumeBox = show_vol_bbox;
  m_ShowBounds = show_bounds;
}
//----------------------------------------------------------------------------
void mafPipeVolumeSliceBlend::InitializeSliceParameters(int direction, double slice_origin0[3],double slice_origin1[3], bool show_vol_bbox,bool show_bounds)
//----------------------------------------------------------------------------
{
  m_SliceParametersInitialized = true;
  m_SliceDirection= direction;
  m_ShowVolumeBox = show_vol_bbox;
  m_ShowBounds = show_bounds;
  m_Origin[0][0] = slice_origin0[0];
  m_Origin[0][1] = slice_origin0[1];
  m_Origin[0][2] = slice_origin0[2];
  m_Origin[1][0] = slice_origin1[0];
  m_Origin[1][1] = slice_origin1[1];
  m_Origin[1][2] = slice_origin1[2];
}
//----------------------------------------------------------------------------
void mafPipeVolumeSliceBlend::InitializeSliceParameters(int direction, double slice_origin0[3],double slice_origin1[3], float slice_xVect[3], float slice_yVect[3], bool show_vol_bbox,bool show_bounds)
//----------------------------------------------------------------------------
{
  m_SliceParametersInitialized = true;
  m_ShowBounds = show_bounds;

  m_SliceDirection= direction;
  if(m_SliceDirection == SLICE_ARB)
    m_SliceDirection = SLICE_Z;
  m_ShowVolumeBox = show_vol_bbox;

  m_Origin[0][0] = slice_origin0[0];
  m_Origin[0][1] = slice_origin0[1];
  m_Origin[0][2] = slice_origin0[2];

  m_Origin[1][0] = slice_origin1[0];
  m_Origin[1][1] = slice_origin1[1];
  m_Origin[1][2] = slice_origin1[2];

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
void mafPipeVolumeSliceBlend::Create(mafSceneNode *n)
  //----------------------------------------------------------------------------
{
  Superclass::Create(n); // Always call this to initialize m_Vme, m_AssemblyFront, ... vars

  m_AssemblyUsed = m_AssemblyBack ? m_AssemblyBack : m_AssemblyFront;

  assert(m_Vme->IsMAFType(mafVMEVolume));
  double b[6];
  //Update input data
  m_Vme->GetOutput()->Update();
  m_Vme->GetOutput()->GetVTKData()->Update();
  m_Vme->GetOutput()->GetVMELocalBounds(b);

  mmaVolumeMaterial *material = ((mafVMEVolume *)m_Vme)->GetMaterial();
  //If material has a valid table range use it
  if (material->m_TableRange[1] < material->m_TableRange[0]) 
  {
    m_Vme->GetOutput()->GetVTKData()->GetScalarRange(material->m_TableRange);
  }

  //Update material
  m_ColorLUT = material->m_ColorLut;
  material->UpdateProp();

  double sr[2];
  m_Vme->GetOutput()->GetVTKData()->GetScalarRange(sr);
  m_ColorLUT->SetTableRange(sr[0], sr[1]);
  material->UpdateFromTables();
  //mafEventMacro(mafEvent(this, CAMERA_UPDATE));

  if(m_SliceDirection == SLICE_ARB)
    m_SliceDirection = SLICE_Z;

  if (!m_SliceParametersInitialized)
  {
    //If slices aren't initialized
    m_Origin[0][0] = (b[0] + b[1])*.5;
    m_Origin[0][1] = (b[2] + b[3])*.5;
    m_Origin[0][2] = (b[4] + b[5])*.5;

    m_Origin[1][0] = (b[0] + b[1])*.5;
    m_Origin[1][1] = (b[2] + b[3])*.5;
    m_Origin[1][2] = (b[4] + b[5])*.5;
  }
  //Define X and Y vector
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

  //Create selection actor
  vtkNEW(m_VolumeBox);
  m_VolumeBox->SetInput(m_Vme->GetOutput()->GetVTKData());

  vtkNEW(m_VolumeBoxMapper);
  m_VolumeBoxMapper->SetInput(m_VolumeBox->GetOutput());

  vtkNEW(m_VolumeBoxActor);
  m_VolumeBoxActor->SetMapper(m_VolumeBoxMapper);
  m_VolumeBoxActor->PickableOff();
  m_VolumeBoxActor->SetVisibility(m_ShowVolumeBox);
  if(m_AssemblyUsed==m_AssemblyFront)
    m_VolumeBoxActor->SetScale(1.01);
  else
    m_VolumeBoxActor->SetScale(1);
  m_AssemblyFront->AddPart(m_VolumeBoxActor);

  if(m_ShowBounds)
  {
    double bounds[6];
    m_Vme->GetOutput()->Update();
    m_Vme->GetOutput()->GetVMELocalBounds(bounds);
    vtkNEW(m_Box);
    m_Box->SetBounds(bounds);
    vtkNEW(m_Mapper);
    m_Mapper->SetInput(m_Box->GetOutput());
    vtkNEW(m_Actor);
    m_Actor->SetMapper(m_Mapper);
    m_AssemblyUsed->AddPart(m_Actor);
  }

  // if the actor is in the background renderer
  // create something invisible in the front renderer so that ResetCamera will work
  for(int i=0;i<2;i++)
  {
    if(m_AssemblyBack != NULL)
    {
      vtkNEW(m_GhostActor[i]);
      m_GhostActor[i]->SetMapper(m_SliceMapper[i][m_SliceDirection]);
      m_GhostActor[i]->PickableOff();
      m_GhostActor[i]->GetProperty()->SetOpacity(0);
      m_GhostActor[i]->GetProperty()->SetRepresentationToPoints();
      m_GhostActor[i]->GetProperty()->SetInterpolationToFlat();
      m_AssemblyFront->AddPart(m_GhostActor[i]);
    }
  }
}
//----------------------------------------------------------------------------
void mafPipeVolumeSliceBlend::CreateSlice(int direction)
//----------------------------------------------------------------------------
{
  //Generate slices scalars using scalars
  double xspc = 0.33, yspc = 0.33, zspc = 1.0;

  vtkDataSet *vtk_data = m_Vme->GetOutput()->GetVTKData();
  vtk_data->Update();
  if(vtk_data->IsA("vtkImageData") || vtk_data->IsA("vtkStructuredPoints"))
  {
    ((vtkImageData *)vtk_data)->GetSpacing(xspc,yspc,zspc);
  }
  for(int i=0;i<2;i++)
  {
    vtkNEW(m_SlicerPolygonal[i][direction]);
    vtkNEW(m_SlicerImage[i][direction]);
    m_SlicerImage[i][direction]->SetPlaneOrigin(m_Origin[i][0], m_Origin[i][1], m_Origin[i][2]);
    m_SlicerPolygonal[i][direction]->SetPlaneOrigin(m_SlicerImage[i][direction]->GetPlaneOrigin());
    m_SlicerImage[i][direction]->SetPlaneAxisX(m_XVector[direction]);
    m_SlicerImage[i][direction]->SetPlaneAxisY(m_YVector[direction]);
    m_SlicerPolygonal[i][direction]->SetPlaneAxisX(m_XVector[direction]);
    m_SlicerPolygonal[i][direction]->SetPlaneAxisY(m_YVector[direction]);
    m_SlicerImage[i][direction]->SetInput(vtk_data);
    m_SlicerPolygonal[i][direction]->SetInput(vtk_data);

    vtkNEW(m_Image[i][direction]);
    m_Image[i][direction]->SetScalarType(vtk_data->GetPointData()->GetScalars()->GetDataType());
    m_Image[i][direction]->SetNumberOfScalarComponents(vtk_data->GetPointData()->GetScalars()->GetNumberOfComponents());
    m_Image[i][direction]->SetExtent(0, m_TextureRes - 1, 0, m_TextureRes - 1, 0, 0);
    m_Image[i][direction]->SetSpacing(xspc, yspc, zspc);

    m_SlicerImage[i][direction]->SetOutput(m_Image[i][direction]);
    m_SlicerImage[i][direction]->Update();

    vtkNEW(m_Texture[i][direction]);
    m_Texture[i][direction]->RepeatOff();
    m_Texture[i][direction]->InterpolateOn();
    m_Texture[i][direction]->SetQualityTo32Bit();
    m_Texture[i][direction]->SetInput(m_Image[i][direction]);
    m_Texture[i][direction]->SetLookupTable(m_ColorLUT);
    m_Texture[i][direction]->MapColorScalarsThroughLookupTableOn();

    vtkNEW(m_SlicePolydata[i][direction]);
    m_SlicerPolygonal[i][direction]->SetOutput(m_SlicePolydata[i][direction]);
    m_SlicerPolygonal[i][direction]->SetTexture(m_Image[i][direction]);
    m_SlicerPolygonal[i][direction]->Update();

    vtkNEW(m_SliceMapper[i][direction]);
    m_SliceMapper[i][direction]->SetInput(m_SlicePolydata[i][direction]);
    m_SliceMapper[i][direction]->ScalarVisibilityOff();

    vtkNEW(m_SliceActor[i][direction]);
    m_SliceActor[i][direction]->SetMapper(m_SliceMapper[i][direction]);
    m_SliceActor[i][direction]->SetTexture(m_Texture[i][direction]);
    m_SliceActor[i][direction]->GetProperty()->SetAmbient(1.f);
    m_SliceActor[i][direction]->GetProperty()->SetDiffuse(0.f);
    m_SliceActor[i][direction]->GetProperty()->SetOpacity(m_SliceOpacity);

    SetSliceOpacity(m_SliceOpacity);

    m_AssemblyUsed->AddPart(m_SliceActor[i][direction]);
  }
}
//----------------------------------------------------------------------------
mafPipeVolumeSliceBlend::~mafPipeVolumeSliceBlend()
//----------------------------------------------------------------------------
{
  //remove all actors from renderer
  if(m_VolumeBoxActor)
    m_AssemblyFront->RemovePart(m_VolumeBoxActor);
  if(m_Actor)
    m_AssemblyUsed->RemovePart(m_Actor);
  if(m_TickActor)
    m_AssemblyUsed->RemovePart(m_TickActor);
  //Delete all pipe stuff
  for(int j=0;j<2;j++)
  {
    for(int i = 0; i<3; i++)
    {
      if(m_SliceActor[j][i])
        m_AssemblyUsed->RemovePart(m_SliceActor[j][i]);
      if (m_SlicerImage[j][i])
      {
        m_SlicerImage[j][i]->SetSliceTransform(NULL);
      }
      if (m_SlicerPolygonal[j][i])
      {
        m_SlicerPolygonal[j][i]->SetSliceTransform(NULL);
      }
      vtkDEL(m_SlicerImage[j][i]);
      vtkDEL(m_SlicerPolygonal[j][i]);
      vtkDEL(m_Image[j][i]);
      vtkDEL(m_Texture[j][i]);
      vtkDEL(m_SliceMapper[j][i]);
      vtkDEL(m_SlicePolydata[j][i]);
      vtkDEL(m_SliceActor[j][i]);
    }
  }
  vtkDEL(m_VolumeBox);
  vtkDEL(m_VolumeBoxMapper);
  vtkDEL(m_VolumeBoxActor);
  vtkDEL(m_Actor);
  vtkDEL(m_TickActor);

  for(int i=0;i<2;i++)
  {
    if(m_GhostActor[i]) 
      m_AssemblyFront->RemovePart(m_GhostActor[i]);
    vtkDEL(m_GhostActor[i]);
  }
}
//----------------------------------------------------------------------------
void mafPipeVolumeSliceBlend::SetLutRange(double low, double high)
//----------------------------------------------------------------------------
{
  //Update material with new LUT range
  mmaVolumeMaterial *material = ((mafVMEVolume *)m_Vme)->GetMaterial();
  material->m_Window_LUT = high-low;
  material->m_Level_LUT  = (low+high)*.5;
  material->m_TableRange[0] = low;
  material->m_TableRange[1] = high;
  material->UpdateProp();
}
//----------------------------------------------------------------------------
void mafPipeVolumeSliceBlend::GetLutRange(double range[2])
//----------------------------------------------------------------------------
{
  mmaVolumeMaterial *material = ((mafVMEVolume *)m_Vme)->GetMaterial();
  material->m_ColorLut->GetTableRange(range);
}
//----------------------------------------------------------------------------
void mafPipeVolumeSliceBlend::SetSlice(int nSlice,double origin[3], float xVect[3], float yVect[3])
//----------------------------------------------------------------------------
{
  //set new slice position
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
  SetSlice(nSlice,origin);
}
//----------------------------------------------------------------------------
void mafPipeVolumeSliceBlend::SetSlice(int nSlice,double origin[3])
//----------------------------------------------------------------------------
{
  //set new slice position
  m_Origin[nSlice][0] = origin[0];
  m_Origin[nSlice][1] = origin[1];
  m_Origin[nSlice][2] = origin[2];
  for(int j=0;j<2;j++)
  {
    for(int i=0;i<3;i++)
    {
      if(m_SlicerImage[j][i])
      {
        m_SlicerImage[j][i]->SetPlaneOrigin(m_Origin[j][0], m_Origin[j][1], m_Origin[j][2]);
        m_SlicerPolygonal[j][i]->SetPlaneOrigin(m_SlicerImage[j][i]->GetPlaneOrigin());
        m_SlicerImage[j][i]->SetPlaneAxisX(m_XVector[i]);
        m_SlicerImage[j][i]->SetPlaneAxisY(m_YVector[i]);
        m_SlicerPolygonal[j][i]->SetPlaneAxisX(m_XVector[i]);
        m_SlicerPolygonal[j][i]->SetPlaneAxisY(m_YVector[i]);
        m_SlicerImage[j][i]->Update();
        m_SlicerPolygonal[j][i]->Update();
      }
    }
  }
  SetSliceOpacity(m_SliceOpacity);
}
//----------------------------------------------------------------------------
void mafPipeVolumeSliceBlend::GetSliceOrigin(int nSlice,double origin[3])
//----------------------------------------------------------------------------
{
  origin[0] = m_Origin[nSlice][0];
  origin[1] = m_Origin[nSlice][1];
  origin[2] = m_Origin[nSlice][2];
}
//----------------------------------------------------------------------------
void mafPipeVolumeSliceBlend::GetSliceNormal(double normal[3])
//----------------------------------------------------------------------------
{
  normal[0] = m_Normal[m_SliceDirection][0];
  normal[1] = m_Normal[m_SliceDirection][1];
  normal[2] = m_Normal[m_SliceDirection][2];
}
//----------------------------------------------------------------------------
void mafPipeVolumeSliceBlend::SetSliceOpacity(double opacity)
  //----------------------------------------------------------------------------
{
  m_SliceOpacity = opacity;

  for (int i=0;i<3;i++)
  {
    //Implemented only for z-axis
    if(m_SliceDirection==SLICE_Z)
    {
      //Check witch slice are at the TOP
      if(m_Origin[0][2]>m_Origin[1][2])
      {
        if(m_SliceActor[0][i]&&m_SliceActor[1][i])
        {
          m_SliceActor[0][i]->GetProperty()->SetOpacity(m_SliceOpacity);
          //Set opacity 1 for slice at bottom
          m_SliceActor[1][i]->GetProperty()->SetOpacity(1.0);
        }
      }
      else if(m_Origin[0][2]<m_Origin[1][2])
      {
        if(m_SliceActor[1][i]&&m_SliceActor[1][i])
        {
          m_SliceActor[1][i]->GetProperty()->SetOpacity(m_SliceOpacity);
          //Set opacity 1 for slice at bottom
          m_SliceActor[0][i]->GetProperty()->SetOpacity(1.0);
        }
      }
    }
  }
}
//----------------------------------------------------------------------------
float mafPipeVolumeSliceBlend::GetSliceOpacity()
  //----------------------------------------------------------------------------
{
  return m_SliceOpacity;
}
//----------------------------------------------------------------------------
mafGUI *mafPipeVolumeSliceBlend::CreateGui()
  //----------------------------------------------------------------------------
{
  //Create pipe gui
  assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);
  m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);
  m_Gui->FloatSlider(ID_OPACITY_SLIDER,"opacity",&m_SliceOpacity,0.0,1.0);
  m_Gui->Divider();
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeVolumeSliceBlend::OnEvent(mafEventBase *maf_event)
  //----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
    case ID_LUT_CHOOSER:
      {
        mmaVolumeMaterial *material = ((mafVMEVolume *)m_Vme)->GetMaterial();
        material->UpdateFromTables();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    case ID_OPACITY_SLIDER:
      {
        //Event to modify the opacity of the slice at top
        SetSliceOpacity(m_SliceOpacity);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    default:
      //Other events
      mafEventMacro(*maf_event);
      break;
    }
  }
  else
  {
    mafEventMacro(*maf_event);
  }
}
//----------------------------------------------------------------------------
void mafPipeVolumeSliceBlend::SetColorLookupTable(vtkLookupTable *lut)
  //----------------------------------------------------------------------------
{
  //Set for every slice the new LUT
  int i;
  if (lut == NULL)
  {
    for(int j=0;j<2;j++)
    {
      for (i=0;i<3;i++)
      {
        if(m_Texture[j][i])
          m_Texture[j][i]->SetLookupTable(m_ColorLUT);
      }
    }
    return;
  }
  m_CustomColorLUT = lut;
  for(int j=0;j<2;j++)
  {
    for (i=0;i<3;i++)
    {
      if(m_Texture[j][i])
        m_Texture[j][i]->SetLookupTable(m_CustomColorLUT);
    }
  }
  //mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafPipeVolumeSliceBlend::Select(bool sel)
//----------------------------------------------------------------------------
{
  //Update actor selection
  m_Selected = sel;
  m_ShowVolumeBox = sel;
  m_VolumeBoxActor->SetVisibility(sel);
}
