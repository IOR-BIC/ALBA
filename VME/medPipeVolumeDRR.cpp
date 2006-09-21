/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeVolumeDRR.cpp,v $
  Language:  C++
  Date:      $Date: 2006-09-21 07:55:34 $
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

#include "medPipeVolumeDRR.h"

#include "mafDecl.h"
#include "mmgGui.h"
#include "mmgLutPreset.h"

#include "mmaVolumeMaterial.h"
#include "mafVME.h"
#include "mafVMEVolume.h"
#include "mafSceneGraph.h"

#include "vtkMAFAssembly.h"
#include "vtkMAFSmartPointer.h"
#include "vtkImageData.h"
#include "vtkImageCast.h"
#include "vtkPiecewiseFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkXRayVolumeMapper.h"
#include "vtkPlaneSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkVolume.h" 
#include "vtkProperty.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkLookupTable.h"
#include "vtkVolumeResample.h"
#include "mafTransform.h"
#include "mafTransformFrame.h"
#include "mafVMEVolumeGray.h"
#include "mafTagArray.h"
#include "mafDataVector.h"
#include "mafVMEItemVTK.h"
#include "vtkPointData.h"
#include "vtkRectilinearGrid.h"
#include "vtkStructuredPoints.h"
#include "vtkRenderer.h"
//----------------------------------------------------------------------------
mafCxxTypeMacro(medPipeVolumeDRR);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medPipeVolumeDRR::medPipeVolumeDRR()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_Caster            = NULL;
  m_OpacityTransferFunction = NULL;
  m_VolumeProperty    = NULL;
  m_Volume            = NULL;
  //m_MIPFunction       = NULL;
  m_VolumeMapper      = NULL;
  //m_VolumeMapperLow   = NULL;
//  m_VolumeLOD         = NULL;
  m_SelectionActor    = NULL;
  m_ColorLUT          = NULL;
  m_VolumeBounds[0] = m_VolumeBounds[1] = m_VolumeBounds[2] \
    = m_VolumeBounds[3] = m_VolumeBounds[4] = m_VolumeBounds[5] = 0;

  m_VolumeOrientation[0] = m_VolumeOrientation[1] = m_VolumeOrientation[2] = 0;
}
//----------------------------------------------------------------------------
void medPipeVolumeDRR::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  m_Selected = false;

  wxBusyCursor wait;

  // image pipeline
  double sr[2];
  vtkImageData *image_data = vtkImageData::New();
  m_Vme->GetOutput()->Update();

  mafString vmeControl = m_Vme->GetOutput()->GetVTKData()->GetClassName();
  if(vmeControl == "vtkStructuredPoints")
  {
   image_data = vtkImageData::SafeDownCast(m_Vme->GetOutput()->GetVTKData());
  }
  else if (vmeControl == "vtkRectilinearGrid")
  {
  wxMessageBox("Resample the RectilinearGrid before Visualizing in DRR View");
  return;

  double volumeSpacing[3];
  volumeSpacing[0] = VTK_DOUBLE_MAX;
  volumeSpacing[1] = VTK_DOUBLE_MAX;
  volumeSpacing[2] = VTK_DOUBLE_MAX;
  
  vtkDataSet *vme_data = ((mafVME *)m_Vme)->GetOutput()->GetVTKData();
  vtkRectilinearGrid *rgrid = vtkRectilinearGrid::SafeDownCast(vme_data);
  

  for (int xi = 1; xi < rgrid->GetXCoordinates()->GetNumberOfTuples (); xi++)
    {
      double spcx = rgrid->GetXCoordinates()->GetTuple1(xi)-rgrid->GetXCoordinates()->GetTuple1(xi-1);
      if (volumeSpacing[0] > spcx)
        volumeSpacing[0] = spcx;
    }
    
    for (int yi = 1; yi < rgrid->GetYCoordinates()->GetNumberOfTuples (); yi++)
    {
      double spcy = rgrid->GetYCoordinates()->GetTuple1(yi)-rgrid->GetYCoordinates()->GetTuple1(yi-1);
      if (volumeSpacing[1] > spcy)
        volumeSpacing[1] = spcy;
    }

    for (int zi = 1; zi < rgrid->GetZCoordinates()->GetNumberOfTuples (); zi++)
    {
      double spcz = rgrid->GetZCoordinates()->GetTuple1(zi)-rgrid->GetZCoordinates()->GetTuple1(zi-1);
      if (volumeSpacing[2] > spcz)
        volumeSpacing[2] = spcz;
    }

  for(int i = 0; i < 3; i++)
  {
   volumeSpacing[i] = fabs(volumeSpacing[i]);
  }

  m_Vme->GetOutput()->GetBounds(m_VolumeBounds);
  m_Vme->GetOutput()->GetAbsPose(m_VolumePosition,m_VolumeOrientation);

  mafSmartPointer<mafTransform> box_pose;
  box_pose->SetOrientation(m_VolumeOrientation);
  box_pose->SetPosition(m_VolumePosition);

  mafSmartPointer<mafTransformFrame> local_pose;
  local_pose->SetInput(box_pose);
  
  mafSmartPointer<mafTransformFrame> output_to_input;
  
  // In a future version if not a "Natural" data the filter should operate in place.
	mafString new_vme_name = "resampled_";
	new_vme_name += m_Vme->GetName();

  double w,l,sr[2];
  for (int i = 0; i < ((mafVMEGenericAbstract *)m_Vme)->GetDataVector()->GetNumberOfItems(); i++)
  {
    if (mafVMEItemVTK *input_item = mafVMEItemVTK::SafeDownCast(((mafVMEGenericAbstract *)m_Vme)->GetDataVector()->GetItemByIndex(i)))
    {
      if (vtkDataSet *input_data = input_item->GetData())
      {
        // the resample filter
        // the resample filter
        vtkMAFSmartPointer<vtkVolumeResample> resampler;
        resampler->SetZeroValue(0);

        // Set the target be vme's parent frame. And Input frame to the root. I've to 
        // set at each iteration since I'm using the SetMatrix, which doesn't support
        // transform pipelines.
        mafSmartPointer<mafMatrix> output_parent_abs_pose;
        m_Vme->GetParent()->GetOutput()->GetAbsMatrix(*output_parent_abs_pose.GetPointer(),input_item->GetTimeStamp());
        local_pose->SetInputFrame(output_parent_abs_pose);

        mafSmartPointer<mafMatrix> input_parent_abs_pose;
        ((mafVME *)m_Vme->GetParent())->GetOutput()->GetAbsMatrix(*input_parent_abs_pose.GetPointer(),input_item->GetTimeStamp());
        local_pose->SetTargetFrame(input_parent_abs_pose);
        local_pose->Update();

        mafSmartPointer<mafMatrix> output_abs_pose;
        m_Vme->GetOutput()->GetAbsMatrix(*output_abs_pose.GetPointer(),input_item->GetTimeStamp());
        output_to_input->SetInputFrame(output_abs_pose);

        mafSmartPointer<mafMatrix> input_abs_pose;
        ((mafVME *)m_Vme)->GetOutput()->GetAbsMatrix(*input_abs_pose.GetPointer(),input_item->GetTimeStamp());
        output_to_input->SetTargetFrame(input_abs_pose);
        output_to_input->Update();

        double orient_input[3],orient_target[3];
        mafTransform::GetOrientation(*output_abs_pose.GetPointer(),orient_target);
        mafTransform::GetOrientation(*input_abs_pose.GetPointer(),orient_input);

        double origin[3];
        origin[0] = m_VolumeBounds[0];
        origin[1] = m_VolumeBounds[2];
        origin[2] = m_VolumeBounds[4];

        output_to_input->TransformPoint(origin,origin);

        resampler->SetVolumeOrigin(origin[0],origin[1],origin[2]);

        vtkMatrix4x4 *mat = output_to_input->GetMatrix().GetVTKMatrix();

        double local_orient[3],local_position[3];
        mafTransform::GetOrientation(output_to_input->GetMatrix(),local_orient);
        mafTransform::GetPosition(output_to_input->GetMatrix(),local_position);

        // extract versors
        double x_axis[3],y_axis[3];

        mafMatrix::GetVersor(0,mat,x_axis);
        mafMatrix::GetVersor(1,mat,y_axis);
        
        resampler->SetVolumeAxisX(x_axis);
        resampler->SetVolumeAxisY(y_axis);
        
        vtkMAFSmartPointer<vtkStructuredPoints> output_data;
        output_data->SetSpacing(volumeSpacing);
        // TODO: here I probably should allow a data type casting... i.e. a GUI widget
        output_data->SetScalarType(input_data->GetPointData()->GetScalars()->GetDataType());
        
        
        input_data->GetScalarRange(sr);

        w = sr[1] - sr[0];
        l = (sr[1] + sr[0]) * 0.5;

        resampler->SetWindow(w);
        resampler->SetLevel(l);
        resampler->SetInput(input_data);
        resampler->SetOutput(output_data);
        resampler->AutoSpacingOff();
        resampler->Update();
        
        output_data->SetSource(NULL);
        output_data->SetOrigin(m_VolumeBounds[0],m_VolumeBounds[2],m_VolumeBounds[4]);

      }
    }
  }
	
  }



  assert(image_data);
  image_data->Update();
  image_data->GetScalarRange(sr);
  
  vtkNEW(m_Caster);
  m_Caster->SetInput(image_data);
  m_Caster->SetNumberOfThreads(1);
  m_Caster->SetOutputScalarType(image_data->GetScalarType());
  m_Caster->BypassOff();
  m_Caster->ClampOverflowOff();

  vtkNEW(m_ColorLUT);
  m_ColorLUT->SetTableRange(sr);

  //vtkNEW(m_OpacityTransferFunction);
  mmaVolumeMaterial *material = ((mafVMEVolume *)m_Vme)->GetMaterial();
  m_OpacityTransferFunction = material->m_OpacityTransferFunction;

  vtkNEW(m_VolumeProperty);
  m_VolumeProperty->SetScalarOpacity(m_OpacityTransferFunction);
  m_VolumeProperty->SetInterpolationTypeToLinear();

  /*vtkNEW(m_MIPFunction);
  m_MIPFunction->SetMaximizeMethodToOpacity();*/

  vtkNEW(m_VolumeMapper);
  m_VolumeMapper->SetInput(m_Caster->GetOutput());
  //m_VolumeMapper->SetVolumeRayCastFunction(m_MIPFunction);
  m_VolumeMapper->SetCroppingRegionPlanes(0, 1, 0, 1, 0, 1);
//  m_VolumeMapper->SetImageSampleDistance(1);
//  m_VolumeMapper->SetMaximumImageSampleDistance(10);
//  m_VolumeMapper->SetMinimumImageSampleDistance(1);
//  m_VolumeMapper->SetNumberOfThreads(1);
//  m_VolumeMapper->SetSampleDistance(1);

  /*vtkNEW(m_VolumeMapperLow);
  m_VolumeMapperLow->SetInput(m_Caster->GetOutput());
  m_VolumeMapperLow->SetVolumeRayCastFunction(m_MIPFunction);
  m_VolumeMapperLow->SetCroppingRegionPlanes(0, 1, 0, 1, 0, 1);
  m_VolumeMapperLow->SetImageSampleDistance(5);
  m_VolumeMapperLow->SetMaximumImageSampleDistance(10);
  m_VolumeMapperLow->SetMinimumImageSampleDistance(5);
  m_VolumeMapperLow->SetNumberOfThreads(1);
  m_VolumeMapperLow->SetSampleDistance(5);

  vtkNEW(m_VolumeLOD);
  m_VolumeLOD->AddLOD(m_VolumeMapperLow, m_VolumeProperty,0);
  m_VolumeLOD->AddLOD(m_VolumeMapper, m_VolumeProperty,0);
  m_VolumeLOD->PickableOff();

  m_AssemblyFront->AddPart(m_VolumeLOD);
  */
  vtkNEW(m_Volume);
  m_Volume->SetMapper(m_VolumeMapper);
  m_Volume->PickableOff();
  
  
  m_AssemblyFront->AddPart(m_Volume);
  
  
  vtkMAFSmartPointer<vtkOutlineCornerFilter> selection_filter;
  selection_filter->SetInput(image_data);  

  vtkMAFSmartPointer<vtkPolyDataMapper> selection_papper;
  selection_papper->SetInput(selection_filter->GetOutput());

  vtkMAFSmartPointer<vtkProperty> selection_property;
  selection_property->SetColor(1,1,1);
  selection_property->SetAmbient(1);
  selection_property->SetRepresentationToWireframe();
  selection_property->SetInterpolationToFlat();

  vtkNEW(m_SelectionActor);
  m_SelectionActor->SetMapper(selection_papper);
  m_SelectionActor->VisibilityOff();
  m_SelectionActor->PickableOff();
  m_SelectionActor->SetProperty(selection_property);
  m_SelectionActor->SetScale(1.01,1.01,1.01);

//  m_AssemblyFront->AddPart(m_SelectionActor); // commented to avoid problems on ray cast volume rendering
}
//----------------------------------------------------------------------------
medPipeVolumeDRR::~medPipeVolumeDRR()
//----------------------------------------------------------------------------
{
  m_AssemblyFront->RemovePart(m_Volume);
  //m_AssemblyFront->RemovePart(m_SelectionActor);

  vtkDEL(m_ColorLUT);
  vtkDEL(m_Caster);
//  vtkDEL(m_OpacityTransferFunction);
  vtkDEL(m_VolumeProperty);
  //vtkDEL(m_MIPFunction);
  vtkDEL(m_VolumeMapper);
  //vtkDEL(m_VolumeMapperLow);
  vtkDEL(m_Volume);
  vtkDEL(m_SelectionActor);
}
//----------------------------------------------------------------------------
void medPipeVolumeDRR::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
  if(m_Volume == NULL) return;
	if(m_Volume->GetVisibility())
			m_SelectionActor->SetVisibility(sel);
}
//----------------------------------------------------------------------------
mmgGui *medPipeVolumeDRR::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  //lutPreset(15,m_ColorLUT);
  //m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);
  //UpdateMIPFromLUT();

	//Gui

	this->m_Gui->SetListener(this);

	this->m_Gui->Label("DRR settings:", true);
	m_Gui->Color(ID_VOLUME_COLOR, "Color", &this->m_VolumeColor);
	vtkXRayVolumeMapper::GetExposureCorrection(this->m_ExposureCorrection);
	m_Gui->FloatSlider(ID_EXPOSURE_CORRECTION_L,	"Min", &this->m_ExposureCorrection[0], -1.f, 1.f);
	m_Gui->FloatSlider(ID_EXPOSURE_CORRECTION_H,	"Max", &this->m_ExposureCorrection[1], -1.f, 1.f);
	m_Gui->FloatSlider(ID_GAMMA,	"Gamma", &this->m_Gamma, 0.1f, 3.f);
/*
	this->m_Gui->Divider(0);
	this->m_Gui->Label("Image settings:", true);
	m_Gui->Color(ID_IMAGE_COLOR, "Color", &this->m_ImageColor);
	m_Gui->FloatSlider(ID_IMAGE_ANGLE, "View angle", &m_ImageAngle, 0.5, 45.0);
	m_Gui->FloatSlider(ID_IMAGE_OFFSET_X, "Offset X", &(m_Offset[0]), -1.0, 1.0);
	m_Gui->FloatSlider(ID_IMAGE_OFFSET_Y, "Offset Y", &(m_Offset[1]), -1.0, 1.0);
*/
	this->m_Gui->Divider(0);
	this->m_Gui->Label("Camera settings:", true);
	vtkCamera *camera = this->m_Sg->m_RenFront->GetActiveCamera();
	this->m_CameraAngle = camera->GetViewAngle();
	m_Gui->FloatSlider(ID_CAMERA_ANGLE, "View angle", &m_CameraAngle, 0.5, 45.0);
	camera->GetPosition(this->m_CameraPosition);
	m_Gui->Vector(ID_CAMERA_POSITION, "Position",	m_CameraPosition);
	camera->GetFocalPoint(this->m_CameraFocus);
	m_Gui->Vector(ID_CAMERA_FOCUS, "Focal point",	m_CameraFocus);
	this->m_CameraRoll = camera->GetRoll();
	m_Gui->FloatSlider(ID_CAMERA_ROLL, "Roll angle", &m_CameraRoll, -180., 180.0);

	this->m_Gui->Divider(0);
	//this->m_Gui->Button(ID_EXPORT, "Export Registration Settings");

	this->m_Gui->Update();

	//end Gui

  return m_Gui;
}
//----------------------------------------------------------------------------
void medPipeVolumeDRR::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
		mafSceneNode *node = NULL;
		vtkCamera *camera = this->m_Sg->m_RenFront->GetActiveCamera();

    switch(e->GetId()) 
    {
      case ID_LUT_CHOOSER:
        UpdateMIPFromLUT();
      break;

			/////
			/*case ID_EXPORT:
				this->ExportData();
				return;
				break;*/

			case ID_VOLUME_COLOR:
				vtkXRayVolumeMapper::SetColor(this->m_VolumeColor.Red() / 255.f, this->m_VolumeColor.Green() / 255.f, this->m_VolumeColor.Blue() / 255.f);
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
				mafEventMacro(mafEvent(this,MOUSE_MOVE));
				break;
			case ID_EXPOSURE_CORRECTION_L:
			case ID_EXPOSURE_CORRECTION_H:
				if (!vtkXRayVolumeMapper::SetExposureCorrection(this->m_ExposureCorrection))
					vtkXRayVolumeMapper::GetExposureCorrection(this->m_ExposureCorrection);
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
				break;

			case ID_GAMMA:
				if (!vtkXRayVolumeMapper::SetGamma(this->m_Gamma))
					this->m_Gamma = vtkXRayVolumeMapper::GetGamma();
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
				mafEventMacro(mafEvent(this,CAMERA_UPDATE));
				break;

			case ID_IMAGE_COLOR:
			case ID_IMAGE_ANGLE:
			case ID_IMAGE_OFFSET_X:
			/*case ID_IMAGE_OFFSET_Y:
				// update pipes
				for (node = this->m_Sg->m_list; node != NULL; node = node->m_next) {
					mafVME *vme = node->m_vme;
					if (vme->GetClassName() != "mafVMEImage" || node->m_Pipe == NULL)
						continue;
					//mafPipeImageBackground *pipe = (mafPipeImageBackground *)node->m_Pipe;
					if (e.GetId() == ID_IMAGE_ANGLE)
						pipe->SetAngle(this->m_ImageAngle);
					else if (e.GetId() == ID_IMAGE_COLOR)
						pipe->SetColor(this->m_ImageColor);
					else
						pipe->SetOffset(this->m_offset[0], this->m_offset[1]);
				}
				this->CameraUpdate();
				break;
      */
			case CAMERA_UPDATE:
			case CAMERA_RESET:
			case CAMERA_FIT:
			case MOUSE_MOVE:
				this->m_CameraAngle = camera->GetViewAngle();
				camera->GetPosition(this->m_CameraPosition);
				camera->GetFocalPoint(this->m_CameraFocus);
				this->m_CameraRoll = camera->GetRoll();
				break;
			case ID_CAMERA_ANGLE:
				camera->SetViewAngle(this->m_CameraAngle);
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
				break;
			case ID_CAMERA_POSITION:
				camera->SetPosition(this->m_CameraPosition);
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
				break;
			case ID_CAMERA_FOCUS:
				camera->SetFocalPoint(this->m_CameraFocus);
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
				break;
			case ID_CAMERA_ROLL:
				camera->SetRoll(this->m_CameraRoll);
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
				break;
			default:
				mafEventMacro(*e);
				return;
			/////
    }

		this->m_Gui->Update();
  }
}
//----------------------------------------------------------------------------
void medPipeVolumeDRR::UpdateMIPFromLUT()
//----------------------------------------------------------------------------
{
  m_OpacityTransferFunction->RemoveAllPoints();
  int tv = m_ColorLUT->GetNumberOfTableValues();
  double rgba[4], sr[2],w,p;
  m_Caster->GetOutput()->GetScalarRange(sr);
  w = sr[1] - sr[0];
  for (int v=0;v<tv;v++)
  {
    m_ColorLUT->GetTableValue(v,rgba);
    p = v*w/tv+sr[0];
    m_OpacityTransferFunction->AddPoint(p,rgba[3]);
  }
  m_OpacityTransferFunction->Update();
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
