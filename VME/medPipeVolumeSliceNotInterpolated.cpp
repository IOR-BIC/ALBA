/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeVolumeSliceNotInterpolated.cpp,v $
  Language:  C++
  Date:      $Date: 2012-04-30 07:57:24 $
  Version:   $Revision: 1.1.2.6 $
  Authors:   Alberto Losi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medPipeVolumeSliceNotInterpolated.h"
#include "vtkMEDVolumeSlicerNotInterpolated.h"
#include "mafVME.h"
#include "mafEventSource.h"
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

mafCxxTypeMacro(medPipeVolumeSliceNotInterpolated);

#define max(a,b)(((a) > (b)) ? (a) : (b))
#define min(a,b)(((a) < (b)) ? (a) : (b))
#define round(x) (x<0?ceil((x)-0.5):floor((x)+0.5))

//----------------------------------------------------------------------------
medPipeVolumeSliceNotInterpolated::medPipeVolumeSliceNotInterpolated()
:mafPipeSlice()
//----------------------------------------------------------------------------
{
  // Initialize attributes
  m_SliceAxis = vtkMEDVolumeSlicerNotInterpolated::SLICE_Z;
  m_Bounds[0] = m_Bounds[1] = m_Bounds[2] = m_Bounds[3] = m_Bounds[4] = m_Bounds[5] = 0;
  m_CurrentSlice = 0.;
  m_SliceSlider = NULL;
  m_ColorLUT = NULL;
  m_VolumeLUT = NULL;
  m_Slicer = NULL;
  m_SlicerOutputImageData = NULL;
  m_SlicerImageDataToRender = NULL;
  m_ImageMapToColors = NULL;
  m_ImageShiftScale = NULL;
  m_ImageActor = NULL;
  m_ShowGui = false;
  m_ScalarRange[0] = m_ScalarRange[1] = 0.0;
  m_SlicerOutputRectilinearGrid = NULL;
  m_RectilinearGridMapper = NULL;
  m_RectilinearGridActor = NULL;
  m_DataType = VTK_IMAGE_DATA;
}

//----------------------------------------------------------------------------
medPipeVolumeSliceNotInterpolated::~medPipeVolumeSliceNotInterpolated()
//----------------------------------------------------------------------------
{
  // Destroy allocated objects
  m_Vme->GetEventSource()->RemoveObserver(this);

  if (m_DataType == VTK_IMAGE_DATA)
  {
    DeleteImageActor();
    DeleteMapToColorsFilter();
    DeleteShiftScaleFilter();
    m_SlicerImageDataToRender->Delete();
    m_SlicerOutputImageData->Delete();

  }
  else if(m_DataType == VTK_RECTILINEAR_GRID)
  {
    DeleteRectilinearGridActor();
    DeleteRectilinearGridMapper();
  }

  m_Slicer->Delete();
  m_ColorLUT->Delete();
}

//----------------------------------------------------------------------------
void medPipeVolumeSliceNotInterpolated::OnEvent(mafEventBase * event)
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
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      } break;
    case ID_AXIS:
      {
        // Update "z" bounds and gui
        m_CurrentSlice = m_Bounds[m_SliceAxis * 2];
        m_SliceSlider->SetRange(m_Bounds[m_SliceAxis * 2], m_Bounds[(m_SliceAxis * 2) + 1]);
        m_Gui->Update();
        // set the origin and update slicer pipeline
        SetSlice();
        mafEventMacro(mafEvent(this,CAMERA_RESET));
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      } break;
    case ID_SLICE:
      {
        // set the origin and update slicer pipeline
        SetSlice();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      } break;
    default:
      {

      }
    }
  }
}

//----------------------------------------------------------------------------
void medPipeVolumeSliceNotInterpolated::Create(mafSceneNode * node)
//----------------------------------------------------------------------------
{
  Superclass::Create(node);
  m_Vme->GetEventSource()->AddObserver(this);
  CreateSlice();
}

//----------------------------------------------------------------------------
void medPipeVolumeSliceNotInterpolated::SetSlice(double origin[3], int sliceAxis)
//----------------------------------------------------------------------------
{
  m_Origin[0] = origin[0];
  m_Origin[1] = origin[1];
  m_Origin[2] = origin[2];

  m_SliceAxis = sliceAxis;

  UpdateSlice();
}

//----------------------------------------------------------------------------
void medPipeVolumeSliceNotInterpolated::SetSlice(double currentSlice, int sliceAxis)
//----------------------------------------------------------------------------
{
  m_SliceAxis = sliceAxis;
  m_CurrentSlice = currentSlice;
  
  SetOrigin();
  UpdateSlice();
}

//----------------------------------------------------------------------------
void medPipeVolumeSliceNotInterpolated::SetSlice()
//----------------------------------------------------------------------------
{
  SetOrigin();
  UpdateSlice();
}
//----------------------------------------------------------------------------
void medPipeVolumeSliceNotInterpolated::UpdateSlice()
//----------------------------------------------------------------------------
{

  if(m_DataType == VTK_IMAGE_DATA)
  {
    DeleteImageActor();
    DeleteMapToColorsFilter();
    DeleteShiftScaleFilter();
  }
  else if (m_DataType == VTK_RECTILINEAR_GRID)
  {
    DeleteRectilinearGridActor();
    DeleteRectilinearGridMapper();
  }

  // Update slicer pipeline
  m_Slicer->SetOrigin(m_Origin);
  m_Slicer->SetSliceAxis(m_SliceAxis);
  m_Slicer->Modified();
  m_Slicer->Update();

  m_DataType = m_Slicer->GetOutputDataType();

  if (m_DataType == VTK_IMAGE_DATA)
  {
    // Update output image
    UpdateImageToRender();

    // Update shift scale filter
    CreateShiftScaleFilter();

    // Update color map filter
    CreateMapToColorsFilter();

    // Update image actor
    CreateImageActor();
  }
  else if (m_DataType == VTK_RECTILINEAR_GRID)
  {
    m_SlicerOutputRectilinearGrid = m_Slicer->GetOutputRectilinearGrid();

    // Update mapper
    CreateRectilinearGridMapper();

    // Update actor
    CreateRectilinearGridActor();
  }
}

//----------------------------------------------------------------------------
void medPipeVolumeSliceNotInterpolated::CreateRectilinearGridMapper()
//----------------------------------------------------------------------------
{
  assert(m_SlicerOutputRectilinearGrid);
  m_RectilinearGridMapper = vtkDataSetMapper::New();
  m_RectilinearGridMapper->SetInput(m_SlicerOutputRectilinearGrid);
  m_RectilinearGridMapper->SetLookupTable(m_VolumeLUT);
  m_RectilinearGridMapper->ScalarVisibilityOn();
  m_RectilinearGridMapper->SetScalarModeToUseCellData();
  m_RectilinearGridMapper->SetUseLookupTableScalarRange(TRUE);
  m_RectilinearGridMapper->Update();
}

//----------------------------------------------------------------------------
void medPipeVolumeSliceNotInterpolated::DeleteRectilinearGridMapper()
//----------------------------------------------------------------------------
{
  if(m_RectilinearGridMapper)
  {
    m_RectilinearGridMapper->Delete();
    m_RectilinearGridMapper = NULL;
  }
}

//----------------------------------------------------------------------------
void medPipeVolumeSliceNotInterpolated::CreateRectilinearGridActor()
//----------------------------------------------------------------------------
{
  assert(m_RectilinearGridMapper);
  m_RectilinearGridActor = vtkActor::New();
  m_RectilinearGridActor->SetMapper(m_RectilinearGridMapper);
  m_RectilinearGridActor->GetProperty()->SetInterpolationToFlat();
  m_RectilinearGridActor->Modified();
  m_AssemblyFront->AddPart(m_RectilinearGridActor);
  m_AssemblyFront->Modified();
}

//----------------------------------------------------------------------------
void medPipeVolumeSliceNotInterpolated::DeleteRectilinearGridActor()
//----------------------------------------------------------------------------
{
  if(m_RectilinearGridActor)
  {
    m_RenFront->RemoveActor(m_RectilinearGridActor);
    m_RectilinearGridActor->Delete();
    m_RectilinearGridActor = NULL;
  }
}
//----------------------------------------------------------------------------
void medPipeVolumeSliceNotInterpolated::CreateImageActor()
//----------------------------------------------------------------------------
{
  // Update image actor
  if(!m_ImageMapToColors->GetOutput())
  {
    return;
  }
  m_ImageActor = vtkImageActor::New();
  m_ImageActor->SetInput(m_ImageMapToColors->GetOutput());
  m_ImageActor->InterpolateOff();
  m_ImageActor->Modified();
  m_RenFront->AddProp(m_ImageActor);
  m_RenFront->Modified();
}

//----------------------------------------------------------------------------
void medPipeVolumeSliceNotInterpolated::DeleteImageActor()
//----------------------------------------------------------------------------
{
  // Is necessary to remove and add the actor to the renderer otherwise the visualization may be incorrect
  if(m_ImageActor)
  {
    m_RenFront->RemoveProp(m_ImageActor);
    m_ImageActor->Delete();
    m_ImageActor = NULL;
  }
}

//----------------------------------------------------------------------------
void medPipeVolumeSliceNotInterpolated::UpdateImageToRender()
//----------------------------------------------------------------------------
{
  // Update intermediate image to be rendered (part of the slicer pipeline)
  if(!m_SlicerOutputImageData)
  {
    return;
  }
  if(!m_SlicerImageDataToRender)
  {
    m_SlicerImageDataToRender = vtkImageData::New();
  }
  m_SlicerImageDataToRender->CopyStructure(m_SlicerOutputImageData);
  m_SlicerImageDataToRender->Update();
  m_SlicerImageDataToRender->GetPointData()->RemoveArray("SCALARS");
  m_SlicerImageDataToRender->GetPointData()->AddArray(m_SlicerOutputImageData->GetPointData()->GetScalars());
  m_SlicerImageDataToRender->GetPointData()->SetActiveScalars("SCALARS");
  m_SlicerImageDataToRender->Modified();
  m_SlicerImageDataToRender->Update();
}

//----------------------------------------------------------------------------
void medPipeVolumeSliceNotInterpolated::CreateShiftScaleFilter()
//----------------------------------------------------------------------------
{
  if(!m_SlicerImageDataToRender)
  {
    return;
  }
  // Shift scale of the image
  // Image actor can render only unsigned char images with scalar range 0 255
  m_ImageShiftScale = vtkImageShiftScale::New();
  m_ImageShiftScale->SetScale(255./(m_ScalarRange[1]-m_ScalarRange[0]));
  m_ImageShiftScale->SetShift(-m_ScalarRange[0]);
  m_ImageShiftScale->SetOutputScalarTypeToUnsignedChar();
  m_ImageShiftScale->SetInput(m_SlicerImageDataToRender);
  m_ImageShiftScale->Update();
}

//----------------------------------------------------------------------------
void medPipeVolumeSliceNotInterpolated::DeleteShiftScaleFilter()
//----------------------------------------------------------------------------
{
  if(m_ImageShiftScale)
  {
    m_ImageShiftScale->Delete();
    m_ImageShiftScale = NULL;
  }
}

//----------------------------------------------------------------------------
void medPipeVolumeSliceNotInterpolated::CreateMapToColorsFilter()
//----------------------------------------------------------------------------
{
  if(!m_ImageShiftScale->GetOutput())
  {
    return;
  }
  m_ImageMapToColors = vtkImageMapToColors::New();
  m_ImageMapToColors->SetLookupTable(m_ColorLUT);
  m_ImageMapToColors->PassAlphaToOutputOn();
  m_ImageMapToColors->SetInput(m_ImageShiftScale->GetOutput());
  m_ImageMapToColors->Update();
}

//----------------------------------------------------------------------------
void medPipeVolumeSliceNotInterpolated::DeleteMapToColorsFilter()
//----------------------------------------------------------------------------
{
  if(m_ImageMapToColors)
  {
    m_ImageMapToColors->Delete();
    m_ImageMapToColors = NULL;
  }
}

//----------------------------------------------------------------------------
void medPipeVolumeSliceNotInterpolated::CreateSlice()
//----------------------------------------------------------------------------
{
  // Ensure that data is loaded in memory
  m_Vme->Update();
  vtkDataSet * data = m_Vme->GetOutput()->GetVTKData();

  // Get the volume lut and rescale to 0 255 scalar range
  m_ColorLUT = vtkLookupTable::New();
  m_VolumeLUT = mafVMEVolumeGray::SafeDownCast(m_Vme)->GetMaterial()->m_ColorLut;
  RescaleLUT(m_VolumeLUT, m_ColorLUT);
  
  // Get bound and scalar range
  data->GetBounds(m_Bounds);
  data->GetScalarRange(m_ScalarRange);
  
  // Evaluate the origin from bounds and current slice
  m_CurrentSlice = m_Bounds[m_SliceAxis * 2];
  m_Origin[0] = m_Bounds[0];
  m_Origin[1] = m_Bounds[2];
  m_Origin[2] = m_Bounds[4];
  m_Origin[m_SliceAxis] = m_CurrentSlice;

  // Create the slicer and set its attributes
  m_Slicer = vtkMEDVolumeSlicerNotInterpolated::New();
  m_Slicer->SetInput(data);
  m_SlicerOutputImageData = vtkImageData::New();
  m_Slicer->SetOutput(m_SlicerOutputImageData);
  
  UpdateSlice();
}

//----------------------------------------------------------------------------
mafGUI * medPipeVolumeSliceNotInterpolated::CreateGui()
//----------------------------------------------------------------------------
{
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
void medPipeVolumeSliceNotInterpolated::SetLut(vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
  m_VolumeLUT = lut;
  SetLut();
}

//----------------------------------------------------------------------------
void medPipeVolumeSliceNotInterpolated::SetLut()
//----------------------------------------------------------------------------
{
  // Rescale the volume lut with scalar range 0 255
  RescaleLUT(m_VolumeLUT, m_ColorLUT);
  mafVMEVolumeGray::SafeDownCast(m_Vme)->GetMaterial()->UpdateFromTables();
}

//----------------------------------------------------------------------------
void medPipeVolumeSliceNotInterpolated::RescaleLUT(vtkLookupTable *inputLUT,vtkLookupTable *outputLUT)
//----------------------------------------------------------------------------
{
  // Copy volume lut
  outputLUT->DeepCopy(inputLUT);

  double tableRange[2];
  inputLUT->GetTableRange(tableRange);

  double maxRange = 255 - (m_ScalarRange[1] - tableRange[1]) / (m_ScalarRange[1] - m_ScalarRange[0]) * 255;
  double minRange = ((tableRange[0] - m_ScalarRange[0]) / (m_ScalarRange[1] - m_ScalarRange[0])) * 255;

  mafLogMessage("> Rescaled lut range %f %f",minRange,maxRange);
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
void medPipeVolumeSliceNotInterpolated::SetOrigin()
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
mafGUI *medPipeVolumeSliceNotInterpolated::GetGui()
//----------------------------------------------------------------------------
{
  if(!m_Gui)
    CreateGui();
  return m_Gui;
}
*/