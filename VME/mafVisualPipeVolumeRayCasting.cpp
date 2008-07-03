/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVisualPipeVolumeRayCasting.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-03 11:29:30 $
  Version:   $Revision: 1.3 $
  Authors:   Alexander Savenko - Paolo Quadrani (porting MAF2.2)
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

#include "mafVisualPipeVolumeRayCasting.h"

#include "mmaVolumeMaterial.h"
#include "mafSceneNode.h"
#include "mmgGui.h"

#include "mafVME.h"
#include "mafVMEOutputVolume.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"
#include "vtkDataSet.h"
#include "vtkActor.h"
#include "vtkVolume.h"
#include "vtkMAFTransferFunction2D.h"
#include "vtkMAFAdaptiveVolumeMapper.h"
#include "vtkPolyDataMapper.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkImageCast.h"
#include "vtkPointData.h"
#include "vtkProperty.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafVisualPipeVolumeRayCasting);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafVisualPipeVolumeRayCasting::mafVisualPipeVolumeRayCasting() 
: mafPipe()
//----------------------------------------------------------------------------
{
  m_Mapper = NULL;
  m_Volume = NULL;
  m_Box    = NULL;
}

//----------------------------------------------------------------------------
mafVisualPipeVolumeRayCasting::~mafVisualPipeVolumeRayCasting() 
//----------------------------------------------------------------------------
{
  this->m_AssemblyFront->RemovePart(this->m_Volume);
  this->m_AssemblyFront->RemovePart(this->m_Box);

  vtkDEL(m_Mapper);
  vtkDEL(m_Volume);
  vtkDEL(m_Box);
}

//----------------------------------------------------------------------------
void mafVisualPipeVolumeRayCasting::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  m_Selected = false;

  // rendering pipeline
  mafVMEOutputVolume *out_data = mafVMEOutputVolume::SafeDownCast(m_Vme->GetOutput());
  vtkDataSet *data = this->m_Vme->GetOutput()->GetVTKData();
  data->Update();
  this->m_Mapper = vtkMAFAdaptiveVolumeMapper::New();

  vtkMAFSmartPointer<vtkImageCast> chardata;
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
    imageData->SetNumberOfScalarComponents(gridData->GetPointData()->GetNumberOfComponents());
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
      imageData->SetScalarType(VTK_UNSIGNED_SHORT);
    else if (data->IsA("vtkShortArray"))
      imageData->SetScalarType(VTK_SHORT);
    else if (data->IsA("vtkCharArray"))
      imageData->SetScalarType(VTK_CHAR);
    else if (data->IsA("vtkUnsignedCharArray"))
      imageData->SetScalarType(VTK_UNSIGNED_CHAR);
    else if (data->IsA("vtkFloatArray"))
      imageData->SetScalarType(VTK_FLOAT);
    else if (data->IsA("vtkDoubleArray"))
      imageData->SetScalarType(VTK_DOUBLE);
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
    chardata->SetInput(imageData);
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
  
  vtkMAFTransferFunction2D *tf = vp->GetTransferFunction2D();
  if (tf == NULL) 
  {
    tf = vtkMAFTransferFunction2D::New();
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
  vtkMAFSmartPointer<vtkOutlineCornerFilter> outlineFilter;
  outlineFilter->SetInput(data);

  vtkMAFSmartPointer<vtkPolyDataMapper> outlineMapper;
  outlineMapper->SetInput(outlineFilter->GetOutput());

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
void mafVisualPipeVolumeRayCasting::Select(bool sel) 
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
mmgGui *mafVisualPipeVolumeRayCasting::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);

  return m_Gui;
}
