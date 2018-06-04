/*=========================================================================

 Program: MAF2
 Module: mafPipeVolumeOrthoSlice
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

#include "mafDecl.h"
#include "mafPipeVolumeOrthoSlice.h"
#include "mafGUI.h"
#include "mafSceneNode.h"
#include "mafTransform.h"
#include "mmaVolumeMaterial.h"
#include "mafVME.h"
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
#include "vtkMAFVolumeOrthoSlicer.h"
#include "vtkProperty.h"
#include "vtkDataSet.h"
#include "vtkRenderer.h"
#include "vtkOutlineSource.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkPlaneSource.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeVolumeOrthoSlice);

#include "mafMemDbg.h"
#include "vtkDataSetToDataSetFilter.h"

//----------------------------------------------------------------------------
mafPipeVolumeOrthoSlice::mafPipeVolumeOrthoSlice():mafPipeSlice()
{ 
  for(int i = 0; i<3; i++)
  {
    m_Texture[i]		      = NULL;
		m_Slicer[i]						= NULL;
    m_SlicePlane[i]		= NULL;
    m_SliceMapper[i]		  = NULL;
    m_SliceActor[i]	      = NULL;
    m_SliceSlider[i]      = NULL;
  }
  m_SliceParametersInitialized  = false;
  m_ShowVolumeBox               = false;
	m_ShowBounds									= false;
  
  m_AssemblyUsed = NULL;
  m_ColorLUT  = NULL;
  m_CustomColorLUT = NULL;

  m_VolumeOutput = NULL;

	m_Box = NULL;
	m_Mapper = NULL;
	m_Actor = NULL;

	m_TickActor = NULL;

  m_SliceDirection  = SLICE_Z;
  m_SliceOpacity  = 1.0;
  m_TextureRes    = 512;

	m_ShowSlider = true;
	m_ShowTICKs	 = false;
  m_Interpolate = true;
}
//----------------------------------------------------------------------------
void mafPipeVolumeOrthoSlice::InitializeSliceParameters(int direction, bool show_vol_bbox, bool show_bounds/* =false */, bool interpolate/* =true */)
{
  m_SliceDirection= direction;
  m_ShowVolumeBox = show_vol_bbox;
	m_ShowBounds = show_bounds;
  m_Interpolate = interpolate;
}
//----------------------------------------------------------------------------
void mafPipeVolumeOrthoSlice::InitializeSliceParameters(int direction, double slice_origin[3], bool show_vol_bbox,bool show_bounds/* =false */, bool interpolate/* =true */)
{
  m_SliceParametersInitialized = true;
  m_SliceDirection= direction;
  m_ShowVolumeBox = show_vol_bbox;
	m_ShowBounds = show_bounds;
  m_Interpolate = interpolate;

  m_Origin[0] = slice_origin[0];
  m_Origin[1] = slice_origin[1];
  m_Origin[2] = slice_origin[2];
}
//----------------------------------------------------------------------------
void mafPipeVolumeOrthoSlice::Create(mafSceneNode *n)
{
  Superclass::Create(n); // Always call this to initialize m_Vme, m_AssemblyFront, ... vars

  m_Vme->AddObserver(this);

  m_AssemblyUsed = m_AssemblyBack ? m_AssemblyBack : m_AssemblyFront;

  m_VolumeOutput = mafVMEOutputVolume::SafeDownCast(m_Vme->GetOutput());
  assert(m_VolumeOutput != NULL);

  vtkDataSet *data = m_Vme->GetOutput()->GetVTKData();
  double b[6];
  m_Vme->GetOutput()->Update();
  data->Update();
  m_Vme->GetOutput()->GetVMELocalBounds(b);

  mmaVolumeMaterial *material = m_VolumeOutput->GetMaterial();
  if (material->m_TableRange[1] < material->m_TableRange[0]) 
  {
    data->GetScalarRange(material->m_TableRange);
    material->UpdateProp();
  }
  
  m_ColorLUT = material->m_ColorLut;

	if (!m_SliceParametersInitialized)
	{
    m_Origin[0] = (b[0] + b[1])*.5;
    m_Origin[1] = (b[2] + b[3])*.5;
    m_Origin[2] = (b[4] + b[5])*.5;
	}

	if(m_SliceDirection == SLICE_ORTHO)
	{
    for(int i = 0; i < 3; i++)
			CreateSlice(i);
	}
	else
	{
		CreateSlice(m_SliceDirection);
	}

	CreateTICKs();

  vtkMAFSmartPointer<vtkOutlineCornerFilter> corner;
	corner->SetInput(data);

  vtkMAFSmartPointer<vtkPolyDataMapper> corner_mapper;
	corner_mapper->SetInput(corner->GetOutput());

	vtkNEW(m_VolumeBoxActor);
	m_VolumeBoxActor->SetMapper(corner_mapper);
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
void mafPipeVolumeOrthoSlice::CreateTICKs()
//----------------------------------------------------------------------------
{
	//---- TICKs creation --------------------------
	vtkPolyData  *CTLinesPD      = vtkPolyData::New();	
	vtkPoints    *CTLinesPoints  = vtkPoints::New();	
	vtkCellArray *CTCells        = vtkCellArray::New();
	int points_id[2];    
	int	counter = 0;

	vtkDataSet *vtk_data = m_Vme->GetOutput()->GetVTKData();
	vtk_data->Update();

	double bounds[6];
	vtk_data->GetBounds(bounds);

	double xmin, xmax, ymin, ymax, zmin, zmax;
	xmin = bounds[0];
	xmax = bounds[1];
	ymin = bounds[2];
	ymax = bounds[3];
	zmin = bounds[4];
	zmax = bounds[5];

	vtkRectilinearGrid *rg_data = vtkRectilinearGrid::SafeDownCast(vtk_data);
	if (rg_data)
	{
		vtkDoubleArray* z_fa = vtkDoubleArray::SafeDownCast(rg_data->GetZCoordinates());
		if(z_fa)
		{
			for (int i = 0; i < z_fa->GetNumberOfTuples(); i++)
			{
				CTLinesPoints->InsertNextPoint(xmax, ymax, z_fa->GetValue(i));
				CTLinesPoints->InsertNextPoint(xmax+(xmax-xmin)/30, ymax+(ymax-ymin)/30 ,z_fa->GetValue(i));
				points_id[0] = counter;
				points_id[1] = counter+1;
				counter+=2;
				CTCells->InsertNextCell(2 , points_id);
			}
		}
		else
		{
			vtkFloatArray* z_fa_f = vtkFloatArray::SafeDownCast(rg_data->GetZCoordinates());
			for (int i = 0; i < z_fa_f->GetNumberOfTuples(); i++)
			{
				CTLinesPoints->InsertNextPoint(xmax, ymax, z_fa_f->GetValue(i));
				CTLinesPoints->InsertNextPoint(xmax+(xmax-xmin)/30, ymax+(ymax-ymin)/30 ,z_fa_f->GetValue(i));
				points_id[0] = counter;
				points_id[1] = counter+1;
				counter+=2;
				CTCells->InsertNextCell(2 , points_id);
			}
		}
	}
	vtkStructuredPoints *sp_data = vtkStructuredPoints::SafeDownCast(vtk_data);
	if (sp_data)
	{
		int dim[3];
		double origin[3];
		double spacing[3];
		sp_data->GetDimensions(dim);
		sp_data->GetOrigin(origin);
		sp_data->GetSpacing(spacing);

		for (int i=0; i < dim[2]; i++)
		{
			float z_i = origin[2] + i*spacing[2];	//?
			CTLinesPoints->InsertNextPoint(xmax, ymax, z_i);
			CTLinesPoints->InsertNextPoint(xmax+(xmax-xmin)/30,ymax+(ymax-ymin)/30,z_i);

			points_id[0] = counter;
			points_id[1] = counter+1;
			counter+=2;
			CTCells->InsertNextCell(2 , points_id);
		}	
	}
	CTLinesPD->SetPoints(CTLinesPoints);
	CTLinesPD->SetLines(CTCells); 
	CTLinesPD->Modified();	  

	//Add tick to scene
	vtkPolyDataMapper *TickMapper = vtkPolyDataMapper::New();
	TickMapper->SetInput(CTLinesPD);

	vtkProperty	*TickProperty = vtkProperty::New();
	TickProperty->SetColor(1,0,0);
	TickProperty->SetAmbient(1);
	TickProperty->SetRepresentationToWireframe();
	TickProperty->SetInterpolationToFlat();

	m_TickActor = vtkActor::New();
	m_TickActor->SetMapper(TickMapper);
	m_TickActor->SetVisibility(m_ShowTICKs);
	m_TickActor->PickableOff();
	m_TickActor->SetProperty(TickProperty);

	m_AssemblyUsed->AddPart(m_TickActor);

	vtkDEL(CTLinesPoints);
	vtkDEL(CTCells);
	vtkDEL(CTLinesPD);

	vtkDEL(TickMapper);
	vtkDEL(TickProperty);

}
//----------------------------------------------------------------------------
void mafPipeVolumeOrthoSlice::CreateSlice(int direction)
//----------------------------------------------------------------------------
{
	double bounds[6];
	vtkDataSet *vtk_data = m_Vme->GetOutput()->GetVTKData();
  vtk_data->Update();
 
	vtkNEW(m_Slicer[direction]);
	m_Slicer[direction]->SetSclicingMode(direction);
	m_Slicer[direction]->SetPlaneOrigin(m_Origin);
	m_Slicer[direction]->SetInput(vtk_data);
  

	vtkNEW(m_Texture[direction]);
	m_Texture[direction]->RepeatOff();
	m_Texture[direction]->SetInterpolate(m_Interpolate);
	m_Texture[direction]->SetQualityTo32Bit();
	m_Texture[direction]->SetInput((vtkImageData*)m_Slicer[direction]->GetOutput());
  m_Texture[direction]->SetLookupTable(m_ColorLUT);
  m_Texture[direction]->MapColorScalarsThroughLookupTableOn();

	vtkNEW(m_SlicePlane[direction]);
	vtk_data->GetBounds(bounds);
	
	double xmin, xmax, ymin, ymax, zmin, zmax;
	xmin = bounds[0];
	xmax = bounds[1];
	ymin = bounds[2];
	ymax = bounds[3];
	zmin = bounds[4];
	zmax = bounds[5];

	//---- pipeline for the Plane --------------------------
	switch (direction)
	{
		case SLICE_X:
			m_SlicePlane[direction]->SetOrigin(m_Origin[0], ymin, zmin);
			m_SlicePlane[direction]->SetPoint1(m_Origin[0], ymax, zmin);
			m_SlicePlane[direction]->SetPoint2(m_Origin[0], ymin, zmax);
			break;
		case SLICE_Y:
			m_SlicePlane[direction]->SetOrigin(xmin, m_Origin[1], zmin);
			m_SlicePlane[direction]->SetPoint1(xmax, m_Origin[1], zmin);
			m_SlicePlane[direction]->SetPoint2(xmin, m_Origin[1], zmax);
			break;
		case SLICE_Z:
			m_SlicePlane[direction]->SetOrigin(xmin, ymin, m_Origin[2]);
			m_SlicePlane[direction]->SetPoint1(xmax, ymin, m_Origin[2]);
			m_SlicePlane[direction]->SetPoint2(xmin, ymax, m_Origin[2]);
			break;
	}

	vtkNEW(m_SliceMapper[direction]);
	m_SliceMapper[direction]->SetInput(m_SlicePlane[direction]->GetOutput());
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
void mafPipeVolumeOrthoSlice::DeleteSlice(int direction)
{
	if (m_SliceActor[direction])
		m_AssemblyUsed->RemovePart(m_SliceActor[direction]);
	vtkDEL(m_Slicer[direction]);
	vtkDEL(m_Texture[direction]);
	vtkDEL(m_SliceMapper[direction]);
	vtkDEL(m_SlicePlane[direction]);
	vtkDEL(m_SliceActor[direction]);
}

//----------------------------------------------------------------------------
void mafPipeVolumeOrthoSlice::UpdatePlaneOrigin(int direction)
{
	double p[3];

	m_SlicePlane[direction]->GetOrigin(p);
	p[direction] = m_Origin[direction];
	m_SlicePlane[direction]->SetOrigin(p);

	m_SlicePlane[direction]->GetPoint1(p);
	p[direction] = m_Origin[direction];
	m_SlicePlane[direction]->SetPoint1(p);

	m_SlicePlane[direction]->GetPoint2(p);
	p[direction] = m_Origin[direction];
	m_SlicePlane[direction]->SetPoint2(p);
}

//----------------------------------------------------------------------------
mafPipeVolumeOrthoSlice::~mafPipeVolumeOrthoSlice()
//----------------------------------------------------------------------------
{
  m_Vme->RemoveObserver(this);
	
  if(m_VolumeBoxActor)
    m_AssemblyFront->RemovePart(m_VolumeBoxActor);
	if(m_Actor)
		m_AssemblyUsed->RemovePart(m_Actor);
	if(m_TickActor)
		m_AssemblyUsed->RemovePart(m_TickActor);

	for(int i = 0; i<3; i++)
	{
		DeleteSlice(i);
	}
	vtkDEL(m_VolumeBoxActor);
	vtkDEL(m_Actor);
	vtkDEL(m_TickActor);

  if(m_GhostActor) 
    m_AssemblyFront->RemovePart(m_GhostActor);
  vtkDEL(m_GhostActor);
}
//----------------------------------------------------------------------------
void mafPipeVolumeOrthoSlice::SetLutRange(double low, double high)
//----------------------------------------------------------------------------
{
  mmaVolumeMaterial *material = m_VolumeOutput->GetMaterial();
  material->m_Window_LUT = high-low;
  material->m_Level_LUT  = (low+high)*.5;
  material->m_TableRange[0] = low;
  material->m_TableRange[1] = high;
  material->UpdateProp();
}
//----------------------------------------------------------------------------
void mafPipeVolumeOrthoSlice::GetLutRange(double range[2])
//----------------------------------------------------------------------------
{
  mmaVolumeMaterial *material = m_VolumeOutput->GetMaterial();
  material->m_ColorLut->GetTableRange(range);
}


//----------------------------------------------------------------------------
//Set the origin and normal of the slice
void mafPipeVolumeOrthoSlice::SetSlice(double* origin, double* normal)
{

	double oldOrigin[3];

	if(origin)
		mafLogMessage("origin: %f %f %f", origin[0], origin[1], origin[2]);


	if (origin != NULL)
	{
		oldOrigin[0] = m_Origin[0];
		oldOrigin[1] = m_Origin[1];
		oldOrigin[2] = m_Origin[2];
		m_Origin[0] = origin[0];
		m_Origin[1] = origin[1];
		m_Origin[2] = origin[2];
	}
	
	if (normal != NULL)
	{
		int newDirection;
		if (normal[0] == 1 && normal[1] == 0 && normal[2] == 0)
			newDirection = SLICE_X;
		else if (normal[0] == 0 && normal[1] == 1 && normal[2] == 0)
			newDirection = SLICE_Y;
		else
			newDirection = SLICE_Z;

		if (newDirection != m_SliceDirection)
		{
			DeleteSlice(m_SliceDirection);
			m_SliceDirection = newDirection;
			CreateSlice(m_SliceDirection);
		}
	}

	for(int i=0;i<3;i++)
	{
		if(origin && m_Origin[i]==oldOrigin[i])
			continue;

		if (m_Slicer[i])
		{
			m_Slicer[i]->SetPlaneOrigin(m_Origin);
			UpdatePlaneOrigin(i);
		}
	}

  UpdateSlice();
}

//----------------------------------------------------------------------------
//Get the slice origin coordinates and normal.
//Both, Origin and Normal may be NULL, if the value is not to be retrieved.
/*virtual*/ void mafPipeVolumeOrthoSlice::GetSlice(double *origin, double *normal)
//----------------------------------------------------------------------------
{
  if (origin != NULL)
  {
    origin[0] = m_Origin[0];
    origin[1] = m_Origin[1];
    origin[2] = m_Origin[2];
  }

  if (normal != NULL)
  {
		normal[0] = m_SliceDirection == SLICE_X;
    normal[1] = m_SliceDirection == SLICE_Y;
    normal[2] = m_SliceDirection == SLICE_Z;
  }  
}

//----------------------------------------------------------------------------
void mafPipeVolumeOrthoSlice::SetSliceOpacity(double opacity)
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
float mafPipeVolumeOrthoSlice::GetSliceOpacity()
//----------------------------------------------------------------------------
{
  return m_SliceOpacity;
}
//----------------------------------------------------------------------------
mafGUI *mafPipeVolumeOrthoSlice::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  double b[6] = {-1,1,-1,1,-1,1};
  m_Gui = new mafGUI(this);
  m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);
	m_Gui->FloatSlider(ID_OPACITY_SLIDER,"opacity",&m_SliceOpacity,0.1,1.0);
  m_Vme->GetOutput()->GetVMELocalBounds(b);
	if(m_ShowSlider)
	{
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
	}
	m_Gui->Divider();
//   m_Gui->Bool(ID_ENABLE_GPU, "Enable GPU", &m_EnableGPU, 1, 
//     _("Enables / disables GPU support for slicing. GPU slicing is typically faster but may produce wrong results on some hardware."));
  m_Gui->Bool(ID_ENABLE_TRILINEAR_INTERPOLATION,"Interpolation",&m_Interpolate,1,"Enable/Disable tri-linear interpolation on slices");
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeVolumeOrthoSlice::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
 {
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
      case ID_LUT_CHOOSER:
      {
        mmaVolumeMaterial *material = m_VolumeOutput->GetMaterial();
        material->UpdateFromTables();
				GetLogicManager()->CameraUpdate();
      }
      break;
      case ID_SLICE_SLIDER_X:
      case ID_SLICE_SLIDER_Y:
      case ID_SLICE_SLIDER_Z:
        SetOrigin(m_Origin);
				GetLogicManager()->CameraUpdate();
      break;
			case ID_OPACITY_SLIDER:
				SetSliceOpacity(m_SliceOpacity);
				GetLogicManager()->CameraUpdate();
        break;
			break;
      case ID_ENABLE_GPU:
        UpdateSlice();
				GetLogicManager()->CameraUpdate();
        break;
      case ID_ENABLE_TRILINEAR_INTERPOLATION:
				SetInterpolation(m_Interpolate);
        break;
      default:
      break;
    }
  }
  else if (maf_event->GetSender() == m_Vme)
  {
    if(maf_event->GetId() == VME_OUTPUT_DATA_UPDATE)
    {
      UpdateSlice();
    }
  }
  else
  {
    mafEventMacro(*maf_event);
  }
}
//----------------------------------------------------------------------------
void mafPipeVolumeOrthoSlice::SetColorLookupTable(vtkLookupTable *lut)
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
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafPipeVolumeOrthoSlice::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	m_ShowVolumeBox = sel;
	m_VolumeBoxActor->SetVisibility(sel);
}
//----------------------------------------------------------------------------
void mafPipeVolumeOrthoSlice::HideSlider()
//----------------------------------------------------------------------------
{
	m_ShowSlider=false;
}
//----------------------------------------------------------------------------
void mafPipeVolumeOrthoSlice::ShowSlider()
//----------------------------------------------------------------------------
{
	m_ShowSlider=true;
}

//----------------------------------------------------------------------------
void mafPipeVolumeOrthoSlice::SetInterpolation(int on)
{
	m_Interpolate = on;
	for (int i = 0; i < 3; i++)
		if (m_Texture[i])
			m_Texture[i]->SetInterpolate(m_Interpolate);
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void mafPipeVolumeOrthoSlice::ShowTICKsOn()
//----------------------------------------------------------------------------
{
	m_ShowTICKs=true;
	if(m_TickActor)
		m_TickActor->SetVisibility(m_ShowTICKs);
}
//----------------------------------------------------------------------------
void mafPipeVolumeOrthoSlice::ShowTICKsOff()
//----------------------------------------------------------------------------
{
	m_ShowTICKs=false;
	if(m_TickActor)
		m_TickActor->SetVisibility(m_ShowTICKs);
}

//------------------------------------------------------------------------
//Updates VTK slicers. It also sets GPUEnabled flag. 
void mafPipeVolumeOrthoSlice::UpdateSlice()
//------------------------------------------------------------------------
{
  for (int i = 0; i < 3; i++)
  {
    if (m_Slicer[i] != NULL)
      m_Slicer[i]->Update();
  }    
}

