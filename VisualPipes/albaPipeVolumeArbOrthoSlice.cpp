/*=========================================================================

Program: ALBA (Agile Library for Biomedical Applications)
Module: albaPipeVolumeArbSlice
Authors: Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or


This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaPipeVolumeArbOrthoSlice.h"
#include "albaPipeVolumeArbSlice.h"
#include "albaDecl.h"
#include "albaGUI.h"
#include "albaSceneNode.h"
#include "albaTransform.h"
#include "mmaVolumeMaterial.h"

#include "albaVME.h"
#include "albaVMEOutputVolume.h"

#include "albaGUIFloatSlider.h"

#include "albaLODActor.h"
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
#include "vtkProperty.h"
#include "vtkDataSet.h"
#include "vtkRenderer.h"
#include "vtkOutlineSource.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeVolumeArbOrthoSlice);
//----------------------------------------------------------------------------

#include "albaMemDbg.h"
#include "vtkALBAAssembly.h"

//----------------------------------------------------------------------------
albaPipeVolumeArbOrthoSlice::albaPipeVolumeArbOrthoSlice():albaPipe()
{ 
  for(int i = 0; i<3; i++)
  {
		m_SlicingPipes[i] = NULL;
    m_SliceSlider[i]      = NULL;
  }
  m_ShowVolumeBox               = false;
	m_ShowBounds									= false;
  
 
	m_Box = NULL;
	m_Mapper = NULL;
	m_Actor = NULL;

	m_TickActor = NULL;

  m_SliceOpacity  = 1.0;
 
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

	m_ShowSlider = true;
	m_ShowTICKs	 = false;
}

//----------------------------------------------------------------------------
void albaPipeVolumeArbOrthoSlice::Create(albaSceneNode *n)
{
  Superclass::Create(n); // Always call this to initialize m_Vme, m_AssemblyFront, ... vars

  m_Vme->AddObserver(this);

  m_AssemblyUsed = m_AssemblyBack ? m_AssemblyBack : m_AssemblyFront;

  m_VolumeOutput = albaVMEOutputVolume::SafeDownCast(m_Vme->GetOutput());
  assert(m_VolumeOutput != NULL);

  vtkDataSet *data = m_Vme->GetOutput()->GetVTKData();
  double b[6];
  m_Vme->GetOutput()->Update();
  m_Vme->GetOutput()->GetVMELocalBounds(b);

  mmaVolumeMaterial *material = m_VolumeOutput->GetMaterial();
  if (material->GetTableRange()[1] < material->GetTableRange()[0]) 
  {
		material->SetTableRange(data->GetScalarRange());
    material->UpdateProp();
  }
  

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

	m_Origin[0] = (b[0] + b[1])*.5;
	m_Origin[1] = (b[2] + b[3])*.5;
	m_Origin[2] = (b[4] + b[5])*.5;

  for(int i = 0; i < 3; i++)
	{
		vtkMath::Normalize(m_XVector[i]);
		vtkMath::Normalize(m_YVector[i]);
		vtkMath::Cross(m_YVector[i], m_XVector[i], m_NormalVector[i]);
		vtkMath::Normalize(m_NormalVector[i]);
		vtkMath::Cross(m_NormalVector[i], m_XVector[i], m_YVector[i]);
		vtkMath::Normalize(m_YVector[i]);
		CreateSlice(i,n);
	}

	CreateTICKs();

  vtkALBASmartPointer<vtkOutlineCornerFilter> corner;
	corner->SetInputData(data);

	vtkALBASmartPointer<vtkPolyDataMapper> corner_mapper;
	corner_mapper->SetInputConnection(corner->GetOutputPort());

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
		m_Mapper->SetInputConnection(m_Box->GetOutputPort());
		vtkNEW(m_Actor);
		m_Actor->SetMapper(m_Mapper);
		m_AssemblyUsed->AddPart(m_Actor);
	}

	n->SetPipe(this);
}

void albaPipeVolumeArbOrthoSlice::CreateSlice(int direction, albaSceneNode *n)
//----------------------------------------------------------------------------
{
	m_SlicingPipes[direction] = albaPipeVolumeArbSlice::New();
	m_SlicingPipes[direction]->Create(n);
	m_SlicingPipes[direction]->InitializeSliceParameters(m_Origin, m_XVector[direction], m_YVector[direction], false, false);
}

//----------------------------------------------------------------------------
void albaPipeVolumeArbOrthoSlice::CreateTICKs()
{
	//---- TICKs creation --------------------------
	vtkPolyData  *CTLinesPD      = vtkPolyData::New();	
	vtkPoints    *CTLinesPoints  = vtkPoints::New();	
	vtkCellArray *CTCells        = vtkCellArray::New();
	vtkIdType points_id[2];    
	int	counter = 0;

	vtkDataSet *vtk_data = m_Vme->GetOutput()->GetVTKData();

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
	TickMapper->SetInputData(CTLinesPD);

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
albaPipeVolumeArbOrthoSlice::~albaPipeVolumeArbOrthoSlice()
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
		albaDEL(m_SlicingPipes[i]);
	}
	vtkDEL(m_VolumeBoxActor);
	vtkDEL(m_Actor);
	vtkDEL(m_TickActor);
}
//----------------------------------------------------------------------------
void albaPipeVolumeArbOrthoSlice::SetLutRange(double low, double high)
{
	for (int i = 0; i < 3; i++)
		m_SlicingPipes[i]->SetLutRange(low, high);
}
//----------------------------------------------------------------------------
void albaPipeVolumeArbOrthoSlice::GetLutRange(double range[2])
{
	m_SlicingPipes[0]->GetLutRange(range);
}

//----------------------------------------------------------------------------
void albaPipeVolumeArbOrthoSlice::SetSlice(int direction, double origin[3], float xVect[3], float yVect[3])
{
	m_SlicingPipes[direction]->SetSlice(origin, xVect, yVect);

}

//----------------------------------------------------------------------------
//Set the origin and normal of the slice
void albaPipeVolumeArbOrthoSlice::SetSlice(int direction, double* Origin, double* Normal)
{
	m_SlicingPipes[direction]->SetSlice(Origin, Normal);
}

//----------------------------------------------------------------------------
//Get the slice origin coordinates and normal.
//Both, Origin and Normal may be NULL, if the value is not to be retrieved.
/*virtual*/ void albaPipeVolumeArbOrthoSlice::GetSlice(int direction, double *Origin, double *Normal)
{
	m_SlicingPipes[direction]->GetSlice(Origin, Normal);
}

//----------------------------------------------------------------------------
void albaPipeVolumeArbOrthoSlice::SetSliceOpacity(double opacity)
{
	m_SliceOpacity = opacity;
	for (int i = 0; i < 3; i++)
		m_SlicingPipes[i]->SetSliceOpacity(opacity);
}
//----------------------------------------------------------------------------
float albaPipeVolumeArbOrthoSlice::GetSliceOpacity()
{
	return m_SliceOpacity;
}
//----------------------------------------------------------------------------
albaGUI *albaPipeVolumeArbOrthoSlice::CreateGui()
{
  assert(m_Gui == NULL);
  double b[6] = {-1,1,-1,1,-1,1};
  m_Gui = new albaGUI(this);
  m_Gui->Lut(ID_LUT_CHOOSER,"Lut",m_ColorLUT);
	m_Gui->FloatSlider(ID_OPACITY_SLIDER,"opacity",&m_SliceOpacity,0.1,1.0);
  m_Vme->GetOutput()->GetVMELocalBounds(b);
	if(m_ShowSlider)
	{
			m_SliceSlider[0] = m_Gui->FloatSlider(ID_SLICE_SLIDER_X,"x",&m_Origin[0],b[0],b[1]);
			m_SliceSlider[1] = m_Gui->FloatSlider(ID_SLICE_SLIDER_Y,"y",&m_Origin[1],b[2],b[3]);
			m_SliceSlider[2] = m_Gui->FloatSlider(ID_SLICE_SLIDER_Z,"z",&m_Origin[2],b[4],b[5]);
	}
	m_Gui->Divider();

	m_Gui->Bool(ID_ENABLE_TRILINEAR_INTERPOLATION,"Interpolation",&m_TrilinearInterpolationOn,1,"Enable/Disable tri-linear interpolation on slices");
  return m_Gui;
}
//----------------------------------------------------------------------------
void albaPipeVolumeArbOrthoSlice::OnEvent(albaEventBase *alba_event)
 {
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
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
				SetSlice(0,m_Origin,NULL);
				break;
      case ID_SLICE_SLIDER_Y:
				SetSlice(1, m_Origin, NULL);
				break;
      case ID_SLICE_SLIDER_Z:
				SetSlice(2, m_Origin, NULL);
				break;
			case ID_OPACITY_SLIDER:
				SetSliceOpacity(m_SliceOpacity);
				GetLogicManager()->CameraUpdate();
        break;
			break;
      case ID_ENABLE_GPU:
				SetEnableGPU(m_EnableGPU);
				GetLogicManager()->CameraUpdate();
        break;
      case ID_ENABLE_TRILINEAR_INTERPOLATION:
        SetTrilinearInterpolation(m_TrilinearInterpolationOn);
				GetLogicManager()->CameraUpdate();
        break;
      default:
      break;
    }
  }
  else
  {
    albaEventMacro(*alba_event);
  }
}

//----------------------------------------------------------------------------
void albaPipeVolumeArbOrthoSlice::Select(bool sel)
{
	m_Selected = sel;
	m_ShowVolumeBox = sel;
	m_VolumeBoxActor->SetVisibility(sel);
}
//----------------------------------------------------------------------------
void albaPipeVolumeArbOrthoSlice::HideSlider()
{
	m_ShowSlider=false;
}
//----------------------------------------------------------------------------
void albaPipeVolumeArbOrthoSlice::ShowSlider()
{
	m_ShowSlider=true;
}
//----------------------------------------------------------------------------
void albaPipeVolumeArbOrthoSlice::ShowTICKsOn()
{
	m_ShowTICKs=true;
	if(m_TickActor)
		m_TickActor->SetVisibility(m_ShowTICKs);
}
//----------------------------------------------------------------------------
void albaPipeVolumeArbOrthoSlice::ShowTICKsOff()
{
	m_ShowTICKs=false;
	if(m_TickActor)
		m_TickActor->SetVisibility(m_ShowTICKs);
}


//------------------------------------------------------------------------
void albaPipeVolumeArbOrthoSlice::SetEnableGPU(int enable)
{
  m_EnableGPU = enable;
	for (int i = 0; i < 3; i++)
		m_SlicingPipes[i]->SetEnableGPU(enable);
};

//------------------------------------------------------------------------
int albaPipeVolumeArbOrthoSlice::GetEnableGPU()
{
  return m_EnableGPU;
};

//----------------------------------------------------------------------------
void albaPipeVolumeArbOrthoSlice::SetTrilinearInterpolation(int on)
{
	m_TrilinearInterpolationOn = on; 
	for (int i = 0; i < 3; i++)
		m_SlicingPipes[i]->SetTrilinearInterpolation(on);
}
