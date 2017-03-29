/*=========================================================================

 Program: MAF2
 Module: mafPipeVolumeSliceNotInterpolated
 Authors: Alberto Losi
 
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

#include "mafPipeVolumeSliceNotInterpolated.h"
#include "vtkMAFVolumeSlicerNotInterpolated.h"
#include "mafVME.h"
#include "mafGUI.h"
#include "vtkLookupTable.h"
#include "vtkRenderer.h"
#include "vtkPointData.h"
#include "mafVMEVolumeGray.h"
#include "mmaVolumeMaterial.h"
#include "vtkImageData.h"
#include "vtkImageActor.h"
#include "vtkImageMapToColors.h"
#include "vtkStructuredPointsWriter.h"
#include "vtkImageShiftScale.h"
#include "vtkRectilinearGrid.h"
#include "vtkDataSetMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "mafGUIFloatSlider.h"
#include "vtkMAFAssembly.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkDataSetWriter.h"

mafCxxTypeMacro(mafPipeVolumeSliceNotInterpolated);


//----------------------------------------------------------------------------
mafPipeVolumeSliceNotInterpolated::mafPipeVolumeSliceNotInterpolated()
:mafPipeSlice()
//----------------------------------------------------------------------------
{
  // Initialize attributes
  m_SliceAxis = vtkMAFVolumeSlicerNotInterpolated::SLICE_Z;
  m_Bounds[0] = m_Bounds[1] = m_Bounds[2] = m_Bounds[3] = m_Bounds[4] = m_Bounds[5] = 0;
  m_CurrentSlice = 0.;
  m_SliceSlider = NULL;
  m_ColorLUT = NULL;
  m_VolumeLUT = NULL;
  m_Slicer = NULL;
  m_ShowGui = false;
  m_ScalarRange[0] = m_ScalarRange[1] = 0.0;
  m_SlicerOutputRectilinearGrid = NULL;
  m_RectilinearGridMapper = NULL;
  m_RectilinearGridActor = NULL;
  m_DataType = VTK_IMAGE_DATA;

  // Rectilinear grid visualization: iterate over number of pieces and initialize to NULLs
  for (int i = 0; i < MAX_NUMBER_OF_PIECES; i++)
  {
    m_ImageDummyData.push_back(NULL);
    m_ImageDummyMapper.push_back(NULL);
    m_ImageDummyActor.push_back(NULL);
    m_SlicerOutputImageData.push_back(NULL);
    m_SlicerImageDataToRender.push_back(NULL);
    m_ImageMapToColors.push_back(NULL);
    m_ImageShiftScale.push_back(NULL);
    m_ImageActor.push_back(NULL);
  }
  m_CurrentImageIndex = 0;
}

//----------------------------------------------------------------------------
mafPipeVolumeSliceNotInterpolated::~mafPipeVolumeSliceNotInterpolated()
//----------------------------------------------------------------------------
{
  // Destroy allocated objects
  m_Vme->RemoveObserver(this);

  if (m_DataType == VTK_IMAGE_DATA)
  {
    for(m_CurrentImageIndex = 0; m_CurrentImageIndex < m_Slicer->GetNumberOfOutputs(); m_CurrentImageIndex++)
    {
      DeleteImageDummyActor();
      DeleteImageDummyMapper();
      DeleteImageDummyData();
      DeleteImageActor();
      DeleteMapToColorsFilter();
      DeleteShiftScaleFilter();
      m_SlicerImageDataToRender.at(m_CurrentImageIndex)->Delete();
      m_SlicerImageDataToRender.at(m_CurrentImageIndex) = NULL;
//       m_SlicerOutputImageData.at(m_CurrentImageIndex)->Delete();
//       m_SlicerOutputImageData.at(m_CurrentImageIndex) = NULL;
    }
  }
  else if(m_DataType == VTK_RECTILINEAR_GRID)
  {
    DeleteRectilinearGridActor();
    DeleteRectilinearGridMapper();
  }

  m_ImageDummyData.clear();
  m_ImageDummyMapper.clear();
  m_ImageDummyActor.clear();
  m_SlicerOutputImageData.clear();
  m_SlicerImageDataToRender.clear();
  m_ImageMapToColors.clear();
  m_ImageShiftScale.clear();
  m_ImageActor.clear();
  m_ColorLUT->Delete();
  m_Slicer->Delete();
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::OnEvent(mafEventBase * event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(event))
  {
    // Trap gui events
    switch(e->GetId()) 
    {
    case ID_LUT:
      {
        SetLut(m_VolumeLUT);
				GetLogicManager()->CameraUpdate();
      } break;
    case ID_AXIS:
      {
        // Update "z" bounds and gui
        m_CurrentSlice = m_Bounds[m_SliceAxis * 2];
        m_SliceSlider->SetRange(m_Bounds[m_SliceAxis * 2], m_Bounds[(m_SliceAxis * 2) + 1]);
        m_Gui->Update();
        // set the origin and update slicer pipeline
        SetSlice();
        GetLogicManager()->CameraReset();
				GetLogicManager()->CameraUpdate();
      } break;
    case ID_SLICE:
      {
        // set the origin and update slicer pipeline
        SetSlice();
				GetLogicManager()->CameraUpdate();
      } break;
    default:
      {

      }
    }
  }
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::Create(mafSceneNode * node)
//----------------------------------------------------------------------------
{
  Superclass::Create(node);
  m_Vme->AddObserver(this);
  CreateSlice();
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::SetSlice(double origin[3], int sliceAxis)
//----------------------------------------------------------------------------
{
// Set slice origin and slice axis for the pipe
  m_Origin[0] = origin[0];
  m_Origin[1] = origin[1];
  m_Origin[2] = origin[2];

  m_SliceAxis = sliceAxis;

  UpdateSlice();
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::SetSlice(double currentSlice, int sliceAxis)
//----------------------------------------------------------------------------
{
  // Set origin[slice axis]
  m_SliceAxis = sliceAxis;
  m_CurrentSlice = currentSlice;
  
  SetOrigin();
  UpdateSlice();
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::SetSlice()
//----------------------------------------------------------------------------
{
  SetOrigin();
  UpdateSlice();
}
//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::UpdateSlice()
//----------------------------------------------------------------------------
{
  if(m_DataType == VTK_IMAGE_DATA)
  {
    // Image data (and rectilinear grid with a number of pieces less than MAX_NUMBER_OF_PIECES)
    for(m_CurrentImageIndex = 0; m_CurrentImageIndex < m_Slicer->GetNumberOfOutputs(); m_CurrentImageIndex++)
    {
      DeleteImageDummyActor();
      DeleteImageDummyMapper();
      DeleteImageDummyData();
      DeleteImageActor();
      DeleteMapToColorsFilter();
      DeleteShiftScaleFilter();
    }
  }
  else if (m_DataType == VTK_RECTILINEAR_GRID)
  {
    // Old rectilinear grid visualizations
    DeleteRectilinearGridActor();
    DeleteRectilinearGridMapper();
  }

  // Update slicer pipeline
  // Ensure that data is loaded in memory
  m_Vme->Update();
  vtkDataSet * data = m_Vme->GetOutput()->GetVTKData();

  // Get bound and scalar range
  data->GetBounds(m_Bounds);
  data->GetScalarRange(m_ScalarRange);
  m_Slicer->SetInput(data);
  m_Slicer->SetOrigin(m_Origin);
  m_Slicer->SetSliceAxis(m_SliceAxis);
  m_Slicer->Modified();
  m_Slicer->Update();

  m_DataType = m_Slicer->GetOutputDataType();

  if (m_DataType == VTK_IMAGE_DATA)
  {
    // Image data (and rectilinear grid with a number of pieces less than MAX_NUMBER_OF_PIECES)
    assert(m_Slicer->GetNumberOfOutputs() < MAX_NUMBER_OF_PIECES);
    for(m_CurrentImageIndex = 0; m_CurrentImageIndex < m_Slicer->GetNumberOfOutputs(); m_CurrentImageIndex++)
    {
      m_SlicerOutputImageData.at(m_CurrentImageIndex) = m_Slicer->GetOutput(m_CurrentImageIndex);

      // Update output image
      UpdateImageToRender();

      // Update shift scale filter
      CreateShiftScaleFilter();

      // Update color map filter
      CreateMapToColorsFilter();

      // Update image actor
      CreateImageActor();

      CreateImageDummyData();

      CreateImageDummyMapper();

      CreateImageDummyActor();
    }
  }
  else if (m_DataType == VTK_RECTILINEAR_GRID)
  {
    // This representation is left to render rectilinear grid that cannot be represented by a number of image data less then MAX_NUMBER_OF_PIECES
    m_SlicerOutputRectilinearGrid = m_Slicer->GetOutputRectilinearGrid();

    // Update mapper
    CreateRectilinearGridMapper();

    // Update actor
    CreateRectilinearGridActor();
  }
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::CreateRectilinearGridMapper()
//----------------------------------------------------------------------------
{
  // data set mapper for rg
  assert(m_SlicerOutputRectilinearGrid);
  m_RectilinearGridMapper = vtkDataSetMapper::New();
  m_RectilinearGridMapper->SetInput(m_SlicerOutputRectilinearGrid);
  m_RectilinearGridMapper->SetLookupTable(m_VolumeLUT);
  m_RectilinearGridMapper->ScalarVisibilityOn();
  m_RectilinearGridMapper->SetScalarModeToUseCellData();
  m_RectilinearGridMapper->SetUseLookupTableScalarRange(TRUE);
  m_RectilinearGridMapper->SetColorModeToMapScalars();
  m_RectilinearGridMapper->Update();
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::DeleteRectilinearGridMapper()
//----------------------------------------------------------------------------
{
  if(m_RectilinearGridMapper)
  {
    m_RectilinearGridMapper->Delete();
    m_RectilinearGridMapper = NULL;
  }
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::CreateRectilinearGridActor()
//----------------------------------------------------------------------------
{
  // 3D actor for rgs
  assert(m_RectilinearGridMapper);
  m_RectilinearGridActor = vtkActor::New();
  m_RectilinearGridActor->SetMapper(m_RectilinearGridMapper);
  m_RectilinearGridActor->GetProperty()->SetInterpolationToFlat();
  //m_RectilinearGridActor->SetOrigin(m_Origin);
  m_RectilinearGridActor->Modified();
  m_AssemblyFront->AddPart(m_RectilinearGridActor);
  m_AssemblyFront->Modified();
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::DeleteRectilinearGridActor()
//----------------------------------------------------------------------------
{
  if(m_RectilinearGridActor)
  {
    m_AssemblyFront->RemovePart(m_RectilinearGridActor);
    m_AssemblyFront->Modified();
    m_RectilinearGridActor->Delete();
    m_RectilinearGridActor = NULL;
  }
}
//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::CreateImageActor()
//----------------------------------------------------------------------------
{
  // Update image actor
  if(!m_ImageMapToColors.at(m_CurrentImageIndex) || !m_ImageMapToColors.at(m_CurrentImageIndex)->GetOutput())
  {
    m_ImageActor.at(m_CurrentImageIndex) = NULL;
    return;
  }
  m_ImageActor.at(m_CurrentImageIndex) = vtkImageActor::New();
  m_ImageActor.at(m_CurrentImageIndex)->SetInput(m_ImageMapToColors.at(m_CurrentImageIndex)->GetOutput());
  m_ImageActor.at(m_CurrentImageIndex)->InterpolateOff();
  int extent[6];
  m_ImageMapToColors.at(m_CurrentImageIndex)->GetOutput()->GetExtent(extent);
  m_ImageActor.at(m_CurrentImageIndex)->SetDisplayExtent(extent);
  m_ImageActor.at(m_CurrentImageIndex)->Modified();
  m_RenFront->AddProp(m_ImageActor.at(m_CurrentImageIndex));
  m_RenFront->Modified();
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::CreateImageDummyData()
//----------------------------------------------------------------------------
{
  // Part of the "dummy" pipeline: necessary to allow pick on image actor
  // Create a polydata that represents the bounds of the image actors
  if(!m_ImageMapToColors.at(m_CurrentImageIndex) || !m_ImageMapToColors.at(m_CurrentImageIndex)->GetOutput())
  {
    m_ImageDummyData.at(m_CurrentImageIndex) = NULL;
    return;
  }
  double bounds[6];

  m_ImageMapToColors.at(m_CurrentImageIndex)->GetOutput()->GetBounds(bounds);
  m_ImageDummyData.at(m_CurrentImageIndex) = vtkPolyData::New();

  vtkPoints *pts = vtkPoints::New();
  vtkCellArray *polys = vtkCellArray::New();

  switch(m_SliceAxis)
  {
  case vtkMAFVolumeSlicerNotInterpolated::SLICE_Z:
    {
      pts->InsertNextPoint(bounds[0],bounds[2],bounds[4] + 0.01);
      pts->InsertNextPoint(bounds[1],bounds[2],bounds[4] + 0.01);
      pts->InsertNextPoint(bounds[1],bounds[3],bounds[4] + 0.01);
      pts->InsertNextPoint(bounds[0],bounds[3],bounds[4] + 0.01);
    }break;
  case vtkMAFVolumeSlicerNotInterpolated::SLICE_Y:
    {
      pts->InsertNextPoint(bounds[0],bounds[2] + 0.01,bounds[4]);
      pts->InsertNextPoint(bounds[1],bounds[2] + 0.01,bounds[4]);
      pts->InsertNextPoint(bounds[1],bounds[2] + 0.01,bounds[5]);
      pts->InsertNextPoint(bounds[0],bounds[2] + 0.01,bounds[5]);
    }break;
  case vtkMAFVolumeSlicerNotInterpolated::SLICE_X:
    {
      pts->InsertNextPoint(bounds[0] - 0.01,bounds[2],bounds[4]);
      pts->InsertNextPoint(bounds[0] - 0.01,bounds[3],bounds[4]);
      pts->InsertNextPoint(bounds[0] - 0.01,bounds[3],bounds[5]);
      pts->InsertNextPoint(bounds[0] - 0.01,bounds[2],bounds[5]);
    }break;
  }

  polys->InsertNextCell(4);
  for(int i = 0; i < 4; i++)
  {
    polys->InsertCellPoint(i);
  }

  m_ImageDummyData.at(m_CurrentImageIndex)->SetPoints(pts);
  m_ImageDummyData.at(m_CurrentImageIndex)->SetPolys(polys);
  m_ImageDummyData.at(m_CurrentImageIndex)->Modified();
  m_ImageDummyData.at(m_CurrentImageIndex)->Update();

  pts->Delete();
  polys->Delete();
}
//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::DeleteImageDummyData()
//----------------------------------------------------------------------------
{
  if(m_ImageDummyData.at(m_CurrentImageIndex))
  {
    m_ImageDummyData.at(m_CurrentImageIndex)->Delete();
    m_ImageDummyData.at(m_CurrentImageIndex) = NULL;
  }
}
//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::CreateImageDummyMapper()
//----------------------------------------------------------------------------
{
  // Part of the "dummy" pipeline: necessary to allow pick on image actor
  if(!m_ImageDummyData.at(m_CurrentImageIndex))
  {
    m_ImageDummyMapper.at(m_CurrentImageIndex) = NULL;
    return;
  }
  m_ImageDummyMapper.at(m_CurrentImageIndex) = vtkPolyDataMapper::New();
  m_ImageDummyMapper.at(m_CurrentImageIndex)->SetInput(m_ImageDummyData.at(m_CurrentImageIndex));
  m_ImageDummyMapper.at(m_CurrentImageIndex)->Update();
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::DeleteImageDummyMapper()
//----------------------------------------------------------------------------
{
  if(m_ImageDummyMapper.at(m_CurrentImageIndex))
  {
    m_ImageDummyMapper.at(m_CurrentImageIndex)->Delete();
    m_ImageDummyMapper.at(m_CurrentImageIndex) = NULL;
  }
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::CreateImageDummyActor()
//----------------------------------------------------------------------------
{
  // Part of the "dummy" pipeline: necessary to allow pick on image actor
  if(!m_ImageDummyMapper.at(m_CurrentImageIndex))
  {
    m_ImageDummyActor.at(m_CurrentImageIndex) = NULL;
    return;
  }
  m_ImageDummyActor.at(m_CurrentImageIndex) = vtkActor::New();
  m_ImageDummyActor.at(m_CurrentImageIndex)->SetMapper(m_ImageDummyMapper.at(m_CurrentImageIndex));
  m_ImageDummyActor.at(m_CurrentImageIndex)->GetProperty()->SetOpacity(.1);
  m_ImageDummyActor.at(m_CurrentImageIndex)->Modified();
  m_AssemblyFront->AddPart(m_ImageDummyActor.at(m_CurrentImageIndex));
  m_AssemblyFront->Modified();
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::DeleteImageDummyActor()
//----------------------------------------------------------------------------
{
  if(m_ImageDummyActor.at(m_CurrentImageIndex))
  {
    m_AssemblyFront->RemovePart(m_ImageDummyActor.at(m_CurrentImageIndex));
    m_AssemblyFront->Modified();
    m_ImageDummyActor.at(m_CurrentImageIndex)->Delete();
    m_ImageDummyActor.at(m_CurrentImageIndex) = NULL;
  }
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::DeleteImageActor()
//----------------------------------------------------------------------------
{
  // Is necessary to remove and add the actor to the renderer otherwise the visualization may be incorrect
  if(m_ImageActor.at(m_CurrentImageIndex))
  {
    m_RenFront->RemoveProp(m_ImageActor.at(m_CurrentImageIndex));
    m_ImageActor.at(m_CurrentImageIndex)->Delete();
    m_ImageActor.at(m_CurrentImageIndex) = NULL;
  }
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::UpdateImageToRender()
//----------------------------------------------------------------------------
{
  // Update intermediate image to be rendered (part of the slicer pipeline)
  if(!m_SlicerOutputImageData.at(m_CurrentImageIndex))
  {
    m_SlicerImageDataToRender.at(m_CurrentImageIndex) = NULL;
    return;
  }

  if(!m_SlicerImageDataToRender.at(m_CurrentImageIndex))
  {
    m_SlicerImageDataToRender.at(m_CurrentImageIndex) = vtkImageData::New();
  }

  m_SlicerImageDataToRender.at(m_CurrentImageIndex)->CopyStructure(m_SlicerOutputImageData.at(m_CurrentImageIndex));
  m_SlicerImageDataToRender.at(m_CurrentImageIndex)->Update();
  m_SlicerImageDataToRender.at(m_CurrentImageIndex)->GetPointData()->RemoveArray("SCALARS");
  m_SlicerImageDataToRender.at(m_CurrentImageIndex)->GetPointData()->AddArray(m_SlicerOutputImageData.at(m_CurrentImageIndex)->GetPointData()->GetScalars());
  m_SlicerImageDataToRender.at(m_CurrentImageIndex)->GetPointData()->SetActiveScalars("SCALARS");
  m_SlicerImageDataToRender.at(m_CurrentImageIndex)->Modified();
  m_SlicerImageDataToRender.at(m_CurrentImageIndex)->Update();
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::CreateShiftScaleFilter()
//----------------------------------------------------------------------------
{
  if(!m_SlicerImageDataToRender.at(m_CurrentImageIndex))
  {
    m_ImageShiftScale.at(m_CurrentImageIndex) = NULL;
    return;
  }
  // Shift scale of the image
  // Image actor can render only unsigned char images with scalar range 0 255
  m_ImageShiftScale.at(m_CurrentImageIndex)  = vtkImageShiftScale::New();
  
  // THe new scale must be from 0 to 255
  if(m_ScalarRange[0] !=  m_ScalarRange[1] && m_ScalarRange[1] != 255)
  {
    m_ImageShiftScale.at(m_CurrentImageIndex)->SetScale(255./(m_ScalarRange[1]-m_ScalarRange[0]));
    m_ImageShiftScale.at(m_CurrentImageIndex)->SetShift(-m_ScalarRange[0]);
  }
  m_ImageShiftScale.at(m_CurrentImageIndex)->SetOutputScalarTypeToUnsignedChar();
  m_ImageShiftScale.at(m_CurrentImageIndex)->SetInput(m_SlicerImageDataToRender.at(m_CurrentImageIndex));
  m_ImageShiftScale.at(m_CurrentImageIndex)->Update();
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::DeleteShiftScaleFilter()
//----------------------------------------------------------------------------
{
  if(m_ImageShiftScale.at(m_CurrentImageIndex))
  {
    m_ImageShiftScale.at(m_CurrentImageIndex)->Delete();
    m_ImageShiftScale.at(m_CurrentImageIndex) = NULL;
  }
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::CreateMapToColorsFilter()
//----------------------------------------------------------------------------
{
  // Map rescaled lut on image data
  if(!m_ImageShiftScale.at(m_CurrentImageIndex) || !m_ImageShiftScale.at(m_CurrentImageIndex)->GetOutput())
  {
    m_ImageMapToColors.at(m_CurrentImageIndex) = NULL;
    return;
  }
  m_ImageMapToColors.at(m_CurrentImageIndex) = vtkImageMapToColors::New();
  m_ImageMapToColors.at(m_CurrentImageIndex)->SetLookupTable(m_ColorLUT);
  m_ImageMapToColors.at(m_CurrentImageIndex)->PassAlphaToOutputOn();
  m_ImageMapToColors.at(m_CurrentImageIndex)->SetInput(m_ImageShiftScale.at(m_CurrentImageIndex)->GetOutput());
  m_ImageMapToColors.at(m_CurrentImageIndex)->Update();
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::DeleteMapToColorsFilter()
//----------------------------------------------------------------------------
{
  if(m_ImageMapToColors.at(m_CurrentImageIndex))
  {
    m_ImageMapToColors.at(m_CurrentImageIndex)->Delete();
    m_ImageMapToColors.at(m_CurrentImageIndex) = NULL;
  }
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::CreateSlice()
//----------------------------------------------------------------------------
{
  // Get the volume lut and rescale to 0 255 scalar range
  m_ColorLUT = vtkLookupTable::New();
  m_VolumeLUT = mafVMEVolumeGray::SafeDownCast(m_Vme)->GetMaterial()->m_ColorLut;
  RescaleLUT(m_VolumeLUT, m_ColorLUT);
  
  // Evaluate the origin from bounds and current slice
  m_CurrentSlice = m_Bounds[m_SliceAxis * 2];
  m_Origin[0] = m_Bounds[0];
  m_Origin[1] = m_Bounds[2];
  m_Origin[2] = m_Bounds[4];
  m_Origin[m_SliceAxis] = m_CurrentSlice;

  // Create the slicer and set its attributes
  m_Slicer = vtkMAFVolumeSlicerNotInterpolated::New();
  UpdateSlice();
}

//----------------------------------------------------------------------------
mafGUI * mafPipeVolumeSliceNotInterpolated::CreateGui()
//----------------------------------------------------------------------------
{
  // create the pipe guis
  m_Gui = new mafGUI(this);
  if(m_ShowGui)
  {
    m_Gui->Lut(ID_LUT,"LUT",m_VolumeLUT); // Lut widget
    wxString choices[3] = {"X","Y","Z"};
    m_Gui->Combo(ID_AXIS,"Axis",&m_SliceAxis,3,choices); // Slice Axis
    m_SliceSlider = m_Gui->FloatSlider(ID_SLICE,"Slice",&m_CurrentSlice, m_Bounds[m_SliceAxis * 2], m_Bounds[(m_SliceAxis * 2) + 1]); // Current slice coordinate
    m_Gui->Divider();
  }
  return m_Gui;
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::SetLut(vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
  m_VolumeLUT = lut;
  SetLut();
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::SetLut()
//----------------------------------------------------------------------------
{
  // Rescale the volume lut with scalar range 0 255
  RescaleLUT(m_VolumeLUT, m_ColorLUT);
  mafVMEVolumeGray::SafeDownCast(m_Vme)->GetMaterial()->UpdateFromTables();
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::RescaleLUT(vtkLookupTable *inputLUT,vtkLookupTable *outputLUT)
//----------------------------------------------------------------------------
{
  // Copy volume lut
  outputLUT->DeepCopy(inputLUT);

  if(m_ScalarRange[0] == 0 && m_ScalarRange[1] == 255 || m_ScalarRange[0] == 255 && m_ScalarRange[1] == 255)
  {
    return;
  }

  double tableRange[2];
  inputLUT->GetTableRange(tableRange);

  double maxRange = 255 - (m_ScalarRange[1] - tableRange[1]) / (m_ScalarRange[1] - m_ScalarRange[0]) * 255;
  double minRange = ((tableRange[0] - m_ScalarRange[0]) / (m_ScalarRange[1] - m_ScalarRange[0])) * 255;

  // Set table scalar range
  outputLUT->SetTableRange(minRange,maxRange);
  
  // Copy table entries
  for(int c = 0; c < inputLUT->GetNumberOfTableValues(); c++)
  {
    outputLUT->SetTableValue(c,inputLUT->GetTableValue(c));
  }
  outputLUT->Modified();
}

//----------------------------------------------------------------------------
void mafPipeVolumeSliceNotInterpolated::SetOrigin()
//----------------------------------------------------------------------------
{
  // Set the origin with the current parameters
  m_Origin[0] = m_Bounds[0];
  m_Origin[1] = m_Bounds[2];
  m_Origin[2] = m_Bounds[4];
  m_Origin[m_SliceAxis] = m_CurrentSlice;
}

/*
//----------------------------------------------------------------------------
mafGUI *mafPipeVolumeSliceNotInterpolated::GetGui()
//----------------------------------------------------------------------------
{
  if(!m_Gui)
    CreateGui();
  return m_Gui;
}
*/