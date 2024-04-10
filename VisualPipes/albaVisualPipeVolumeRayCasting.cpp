/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVisualPipeVolumeRayCasting
 Authors: Alexander Savenko - Paolo Quadrani (porting ALBA.2)
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaVisualPipeVolumeRayCasting.h"

#include "mmaVolumeMaterial.h"
#include "albaSceneNode.h"
#include "albaGUI.h"

#include "albaVME.h"
#include "albaVMEOutputVolume.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBAAssembly.h"
#include "vtkDataSet.h"
#include "vtkActor.h"
#include "vtkVolume.h"
#include "vtkALBATransferFunction2D.h"
#include "vtkALBAAdaptiveVolumeMapper.h"
#include "vtkPolyDataMapper.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkImageCast.h"
#include "vtkPointData.h"
#include "vtkProperty.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaVisualPipeVolumeRayCasting);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaVisualPipeVolumeRayCasting::albaVisualPipeVolumeRayCasting() 
: albaPipe()
//----------------------------------------------------------------------------
{
  m_Mapper = NULL;
  m_Volume = NULL;
  m_Box    = NULL;
}

//----------------------------------------------------------------------------
albaVisualPipeVolumeRayCasting::~albaVisualPipeVolumeRayCasting() 
//----------------------------------------------------------------------------
{
  this->m_AssemblyFront->RemovePart(this->m_Volume);
  this->m_AssemblyFront->RemovePart(this->m_Box);

  vtkDEL(m_Mapper);
  vtkDEL(m_Volume);
  vtkDEL(m_Box);
}

//----------------------------------------------------------------------------
void albaVisualPipeVolumeRayCasting::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  m_Selected = false;

  // rendering pipeline
  albaVMEOutputVolume *out_data = albaVMEOutputVolume::SafeDownCast(m_Vme->GetOutput());
  vtkDataSet *data = this->m_Vme->GetOutput()->GetVTKData();
  this->m_Mapper = vtkALBAAdaptiveVolumeMapper::New();

  vtkALBASmartPointer<vtkImageCast> chardata;
  chardata->SetOutputScalarTypeToShort();

  // convert rect. data to image data
  vtkImageData *imageData = vtkImageData::SafeDownCast(data);
  vtkRectilinearGrid *gridData = vtkRectilinearGrid::SafeDownCast(data);

  // convert grid data to image data
  if (gridData && gridData->GetPointData() && gridData->GetPointData()->GetArray(0) &&
    gridData->GetXCoordinates()->GetNumberOfTuples() > 1 &&
    gridData->GetYCoordinates()->GetNumberOfTuples() > 1 &&
    gridData->GetZCoordinates()->GetNumberOfTuples() > 1) 
  {
    imageData = vtkImageData::New();
    imageData->ShallowCopy(gridData);
    imageData->SetDimensions(gridData->GetDimensions());
    double offset[3], spacing[3];
    offset[0] = gridData->GetXCoordinates()->GetTuple(0)[0];
    offset[1] = gridData->GetYCoordinates()->GetTuple(0)[0];
    offset[2] = gridData->GetZCoordinates()->GetTuple(0)[0];
    spacing[0] = gridData->GetXCoordinates()->GetTuple(1)[0] - offset[0];
    spacing[1] = gridData->GetYCoordinates()->GetTuple(1)[0] - offset[1];;
    spacing[2] = gridData->GetZCoordinates()->GetTuple(1)[0] - offset[2];;
    imageData->SetOrigin(offset);
    imageData->SetSpacing(spacing);
    // set type
    vtkDataArray *data = gridData->GetPointData()->GetArray(0);
    if (data->IsA("vtkUnsignedShortArray"))
      imageData->AllocateScalars(VTK_UNSIGNED_SHORT,gridData->GetPointData()->GetNumberOfComponents());
    else if (data->IsA("vtkShortArray"))
      imageData->AllocateScalars(VTK_SHORT,gridData->GetPointData()->GetNumberOfComponents());
    else if (data->IsA("vtkCharArray"))
      imageData->AllocateScalars(VTK_CHAR,gridData->GetPointData()->GetNumberOfComponents());
    else if (data->IsA("vtkUnsignedCharArray"))
      imageData->AllocateScalars(VTK_UNSIGNED_CHAR,gridData->GetPointData()->GetNumberOfComponents());
    else if (data->IsA("vtkFloatArray"))
      imageData->AllocateScalars(VTK_FLOAT,gridData->GetPointData()->GetNumberOfComponents());
    else if (data->IsA("vtkDoubleArray"))
      imageData->AllocateScalars(VTK_DOUBLE,gridData->GetPointData()->GetNumberOfComponents());
    else 
    {
      imageData->Delete();
      imageData = NULL;
    }
  }

  vtkDataArray *scalars = data->GetPointData()->GetScalars();

  if(scalars->GetDataType() != VTK_UNSIGNED_SHORT || 
     scalars->GetDataType() != VTK_SHORT ||
     scalars->GetDataType() != VTK_CHAR ||
     scalars->GetDataType() != VTK_UNSIGNED_CHAR)
  {
    chardata->SetInputData(imageData);
    chardata->Update();
    this->m_Mapper->SetInput((vtkDataSet*)(chardata->GetOutput()));
  }
  else
  {
    this->m_Mapper->SetInput(data);
  }
  this->m_Volume = vtkVolume::New();
  this->m_Volume->SetMapper(this->m_Mapper);
  this->m_AssemblyFront->AddPart(this->m_Volume);
  
  // property
  vtkVolumeProperty2 *vp = out_data->GetMaterial()->m_VolumeProperty2;
  if (vp == NULL)
    vp = vtkVolumeProperty2::New();
  
  vtkALBATransferFunction2D *tf = vp->GetTransferFunction2D();
  if (tf == NULL) 
  {
    tf = vtkALBATransferFunction2D::New();
    vp->SetTransferFunction2D(tf);
    tf->UnRegister(NULL);
    
    // default TF
    tfWidget widget;
    double range[2];
    data->GetScalarRange(range);
    double rsize = range[1] - range[0];
    
    widget.Opacity = 1.0;
    widget.SetValueRange(range[0] + rsize * 0.1, range[1], range[0] + rsize * 0.5);
    widget.SetGradientRange(50, 10000, 100);
    widget.Diffuse = 0.8;
    tf->AddWidget(widget);
  }

  this->m_Volume->SetProperty(vp);

  // selection box
  vtkALBASmartPointer<vtkOutlineCornerFilter> outlineFilter;
  outlineFilter->SetInputData(data);

  vtkALBASmartPointer<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInputConnection(outlineFilter->GetOutputPort());

  this->m_Box = vtkActor::New();
  this->m_Box->SetMapper(outlineMapper);
  this->m_Box->VisibilityOff();
  this->m_Box->PickableOff();

  vtkProperty *property = vtkProperty::New();
	property->SetColor(1,1,1);
	property->SetAmbient(1);
	property->SetRepresentationToWireframe();
	property->SetInterpolationToFlat();
  this->m_Box->SetProperty(property);
  this->m_AssemblyFront->AddPart(this->m_Box);
  property->Delete();
}

//----------------------------------------------------------------------------
void albaVisualPipeVolumeRayCasting::Select(bool sel) 
//----------------------------------------------------------------------------
{
  if (this->m_Volume) 
  {
  	this->m_Selected = sel;
    if(this->m_Box) 
		  this->m_Box->SetVisibility(sel);
  }
}
//----------------------------------------------------------------------------
albaGUI *albaVisualPipeVolumeRayCasting::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new albaGUI(this);

  return m_Gui;
}
