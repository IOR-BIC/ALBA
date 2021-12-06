/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpVolumeResample
 Authors: Marco Petrone
 
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


#include "albaOpVolumeResample.h"

#include "albaEvent.h"
#include "albaGUI.h"
#include "mmaMaterial.h"

#include "albaVME.h"
#include "albaVMESurface.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEGizmo.h"
#include "albaGizmoHandle.h"
#include "albaGizmoTranslate.h"
#include "albaGizmoRotate.h"
#include "albaGizmoHandle.h"
#include "albaGizmoROI.h"
#include "albaVMEItemVTK.h"
#include "albaTagArray.h"
#include "albaDataVector.h"

#include "albaTransform.h"
#include "albaTransformFrame.h"
#include "albaInteractorGenericMouse.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBAVolumeResample.h"
#include "vtkOutlineSource.h"
#include "vtkProperty.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkPointData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkCubeSource.h"
#include "vtkDoubleArray.h"


#include <vector>
#include <algorithm>


#define SPACING_PERCENTAGE_BOUNDS 0.1

using namespace std;

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpVolumeResample);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpVolumeResample::albaOpVolumeResample(const wxString &label /* =  */,bool showShadingPlane /* = false */) : albaOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
  m_Canundo	= true;

  m_ShowShadingPlane = showShadingPlane;

  m_ResampledVme = NULL;

  m_InputPreserving = true;
	
  // initialize Crop OBB parameters
  m_ROIPosition[0]    = m_ROIPosition[1]    = m_ROIPosition[2]    = 0;

	m_ROIPosition[0] = m_ROIPosition[1] = m_ROIPosition[2] = 0.0;
  m_ROIOrientation[0] = m_ROIOrientation[1] = m_ROIOrientation[2] = 0.0;

	m_VolumeCenterPosition[0] = m_VolumeCenterPosition[1] = m_VolumeCenterPosition[2] = 0;
	m_NewVolumePosition[0] = m_NewVolumePosition[1] = m_NewVolumePosition[2] = 0;
	m_PrecedentPosition[0] = m_PrecedentPosition[1] = m_PrecedentPosition[2] = 0;

  m_VolumeBounds[0] = m_VolumeBounds[1] = m_VolumeBounds[2] = \
  m_VolumeBounds[3] = m_VolumeBounds[4] = m_VolumeBounds[5] = 0;

  m_VolumeSpacing[0] = m_VolumeSpacing[1] = m_VolumeSpacing[2] = 1;

  m_ZeroPadValue = 0;
	m_GizmoChoose	 = 0;

	m_GizmoTranslate	= NULL;
	m_GizmoRotate			= NULL;
	m_GizmoROI				= NULL;
	m_VMEDummy				= NULL;

	m_ShowHandle					= 1;
	m_ShowGizmoTransform	= 1;

	m_MaxBoundX = 0.0;
	m_MaxBoundY = 0.0;
	m_MaxBoundZ = 0.0;

  m_CenterVolumeRefSysMatrix = NULL;
}
//----------------------------------------------------------------------------
albaOpVolumeResample::~albaOpVolumeResample()
//----------------------------------------------------------------------------
{
  GizmoDelete();

  if (m_VMEDummy)
  {
  	m_VMEDummy->ReparentTo(NULL);
  }
  albaDEL(m_VMEDummy);
  albaDEL(m_CenterVolumeRefSysMatrix);
	albaDEL(m_ResampledVme);
}
//----------------------------------------------------------------------------
bool albaOpVolumeResample::InternalAccept(albaVME* vme) 
//----------------------------------------------------------------------------
{
	albaEvent e(this,VIEW_SELECTED);
	albaEventMacro(e);
  return (vme && vme->IsALBAType(albaVMEVolumeGray));
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::InternalUpdateBounds(double bounds[6], bool center)
//----------------------------------------------------------------------------
{
  if (center)
  {
    m_VolumeCenterPosition[0] = (bounds[1] + bounds[0]) / 2.0;
    m_VolumeCenterPosition[1] = (bounds[3] + bounds[2]) / 2.0;
    m_VolumeCenterPosition[2] = (bounds[5] + bounds[4]) / 2.0;

    double dims[3];
    dims[0] = bounds[1] - bounds[0];
    dims[1] = bounds[3] - bounds[2];
    dims[2] = bounds[5] - bounds[4];

    m_VolumeBounds[0] = -dims[0] / 2.0;
    m_VolumeBounds[1] =  dims[0] / 2.0;
    m_VolumeBounds[2] = -dims[1] / 2.0;
    m_VolumeBounds[3] =  dims[1] / 2.0;
    m_VolumeBounds[4] = -dims[2] / 2.0;
    m_VolumeBounds[5] =  dims[2] / 2.0;
  }
  else
  {
    m_VolumeCenterPosition[0] = 0;
    m_VolumeCenterPosition[1] = 0;
    m_VolumeCenterPosition[2] = 0;

    for(int i = 0; i < 6; i++)
    {
      m_VolumeBounds[i] = bounds[i];
    }
  }
	m_MaxBoundX = m_VolumeBounds[1] - m_VolumeBounds[0];
	m_MaxBoundY = m_VolumeBounds[3] - m_VolumeBounds[2];
	m_MaxBoundZ = m_VolumeBounds[5] - m_VolumeBounds[4];
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::CreateGizmos()
//----------------------------------------------------------------------------
{
  albaEvent e(this,VIEW_SELECTED);
  albaEventMacro(e);
  m_ViewSelectedMessage = e.GetBool();

	m_GizmoROI = new albaGizmoROI(m_Input, this, albaGizmoHandle::FREE,m_VMEDummy,m_ShowShadingPlane);
  m_GizmoROI->ShowShadingPlane(true);
	m_GizmoROI->Show(true && m_ViewSelectedMessage);
	
  if(m_ViewSelectedMessage)
    m_GizmoROI->GetBounds(m_VolumeBounds);
  else
    m_Input->GetOutput()->GetVMELocalBounds(m_VolumeBounds);


	SetBoundsToVMELocalBounds();

	albaTransform::GetOrientation(*(m_Input->GetOutput()->GetAbsMatrix()),m_VolumeOrientation);

	albaVMEVolumeGray *inputVolume = albaVMEVolumeGray::SafeDownCast(m_Input);
	inputVolume->GetOutput()->GetVTKData()->GetCenter(m_VolumeCenterPosition);

	//Compute the center of Volume in absolute coordinate to center gizmo
	vtkALBASmartPointer<vtkPoints> point;
	point->InsertNextPoint(m_VolumeCenterPosition);
	vtkALBASmartPointer<vtkPolyData> polydata;
	polydata->SetPoints(point);
	vtkALBASmartPointer<vtkTransform> transform;
	transform->Identity();
	transform->SetMatrix(inputVolume->GetOutput()->GetMatrix()->GetVTKMatrix());
	transform->Update();
	vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFilter;
	transformFilter->SetInput(polydata);
	transformFilter->SetTransform(transform);
	transformFilter->Update();
	transformFilter->GetOutput()->GetCenter(m_VolumeCenterPosition);

	vtkALBASmartPointer<vtkTransform> startTransform;
	startTransform->Identity();
	startTransform->Translate(m_VolumeCenterPosition);
	startTransform->RotateX(m_VolumeOrientation[0]);
	startTransform->RotateY(m_VolumeOrientation[1]);
	startTransform->RotateZ(m_VolumeOrientation[2]);
	startTransform->Update();

	//Save ref sys of center volume
	albaNEW(m_CenterVolumeRefSysMatrix);
	m_CenterVolumeRefSysMatrix->Identity();
	m_CenterVolumeRefSysMatrix->SetVTKMatrix(startTransform->GetMatrix());

	if(!m_TestMode)
	{
		m_GizmoTranslate = new albaGizmoTranslate(m_Input, this);
		m_GizmoTranslate->SetRefSys(m_Input);
		m_GizmoTranslate->SetAbsPose(m_CenterVolumeRefSysMatrix);
		m_GizmoTranslate->Show(true && e.GetBool());
		m_GizmoRotate = new albaGizmoRotate(m_Input, this);
		m_GizmoRotate->SetRefSys(m_Input);
		m_GizmoRotate->SetAbsPose(m_CenterVolumeRefSysMatrix);
		m_GizmoRotate->Show(false);
		GetLogicManager()->CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::AutoSpacing()
//----------------------------------------------------------------------------
{
  vtkDataSet *vme_data = m_Input->GetOutput()->GetVTKData();

  m_VolumeSpacing[0] = VTK_DOUBLE_MAX;
  m_VolumeSpacing[1] = VTK_DOUBLE_MAX;
  m_VolumeSpacing[2] = VTK_DOUBLE_MAX;

  if (vtkImageData *image = vtkImageData::SafeDownCast(vme_data))
  {
    image->GetSpacing(m_VolumeSpacing);
  }
  else if (vtkRectilinearGrid *rgrid = vtkRectilinearGrid::SafeDownCast(vme_data))
  {
    for (int xi = 1; xi < rgrid->GetXCoordinates()->GetNumberOfTuples (); xi++)
    {
      double spcx = rgrid->GetXCoordinates()->GetTuple1(xi)-rgrid->GetXCoordinates()->GetTuple1(xi-1);
      if (m_VolumeSpacing[0] > spcx && spcx != 0.0)
        m_VolumeSpacing[0] = spcx;
    }
    
    for (int yi = 1; yi < rgrid->GetYCoordinates()->GetNumberOfTuples (); yi++)
    {
      double spcy = rgrid->GetYCoordinates()->GetTuple1(yi)-rgrid->GetYCoordinates()->GetTuple1(yi-1);
      if (m_VolumeSpacing[1] > spcy && spcy != 0.0)
        m_VolumeSpacing[1] = spcy;
    }

    for (int zi = 1; zi < rgrid->GetZCoordinates()->GetNumberOfTuples (); zi++)
    {
      double spcz = rgrid->GetZCoordinates()->GetTuple1(zi)-rgrid->GetZCoordinates()->GetTuple1(zi-1);
      if (m_VolumeSpacing[2] > spcz && spcz != 0.0)
        m_VolumeSpacing[2] = spcz;
    }
  }
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::UpdateGui()
//----------------------------------------------------------------------------
{
	m_MaxBoundX = m_VolumeBounds[1] - m_VolumeBounds[0];
	m_MaxBoundY = m_VolumeBounds[3] - m_VolumeBounds[2];
	m_MaxBoundZ = m_VolumeBounds[5] - m_VolumeBounds[4];

  if(m_ViewSelectedMessage)
    m_GizmoROI->GetBounds(m_VolumeBounds);
  else
    m_Input->GetOutput()->GetVMELocalBounds(m_VolumeBounds);
	if(m_Gui)
		m_Gui->Update();
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::SetBoundsToVMEBounds()
//----------------------------------------------------------------------------
{
  double bounds[6];
  m_Input->GetOutput()->GetVMEBounds(bounds);

  InternalUpdateBounds(bounds,true);
  m_VolumeOrientation[0] = m_VolumeOrientation[1] = m_VolumeOrientation[2] = 0;
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::SetBoundsToVME4DBounds()
//----------------------------------------------------------------------------
{
  double bounds[6];
  m_Input->GetOutput()->GetVME4DBounds(bounds);

  InternalUpdateBounds(bounds,true);
  m_VolumeOrientation[0] = m_VolumeOrientation[1] = m_VolumeOrientation[2] = 0;
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::SetBoundsToVMELocalBounds()
//----------------------------------------------------------------------------
{
  double bounds[6];
  m_Input->GetOutput()->GetVMELocalBounds(bounds);

  InternalUpdateBounds(bounds,false);
  m_Input->GetOutput()->GetAbsPose(m_VolumePosition,m_VolumeOrientation);
	
	m_ROIOrientation[0] = m_ROIOrientation[1] = m_ROIOrientation[2] = 0;
	m_ROIPosition[0] = m_ROIPosition[1] = m_ROIPosition[2] = 0;
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::GizmoDelete()
//----------------------------------------------------------------------------
{	
  if (m_GizmoTranslate)
  {
    m_GizmoTranslate->Show(false);
    cppDEL(m_GizmoTranslate);
  }
  
  if (m_GizmoRotate)
  {
    m_GizmoRotate->Show(false);
    cppDEL(m_GizmoRotate);
  }
  
  if (m_GizmoROI)
  {
    m_GizmoROI->Show(false);
    cppDEL(m_GizmoROI);
  }	

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
albaOp *albaOpVolumeResample::Copy()
//----------------------------------------------------------------------------
{
	return new albaOpVolumeResample(m_Label,m_ShowShadingPlane);
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::InizializeVMEDummy()   
//----------------------------------------------------------------------------
{
	albaNEW(m_VMEDummy);
	vtkALBASmartPointer<vtkCubeSource> cube;
	m_VMEDummy->SetData(vtkPolyData::SafeDownCast(cube->GetOutput()),0.0);
	m_VMEDummy->SetVisibleToTraverse(false);
	m_VMEDummy->GetTagArray()->SetTag(albaTagItem("VISIBLE_IN_THE_TREE", 0.0));
	m_VMEDummy->ReparentTo(m_Input->GetRoot());
	m_VMEDummy->SetAbsMatrix(*m_Input->GetOutput()->GetAbsMatrix());
  m_VMEDummy->SetName("Dummy");
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::OpRun()   
//----------------------------------------------------------------------------
{
	InizializeVMEDummy();
	CreateGizmos();
	if(!this->m_TestMode)
		CreateGui();
	UpdateGui();
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::OpDo()
//----------------------------------------------------------------------------
{
	m_ResampledVme->ReparentTo(m_Input->GetParent());
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::Resample()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaTransform> resamplingBoxPose;
  resamplingBoxPose->SetOrientation(m_ROIOrientation);
  resamplingBoxPose->SetPosition(m_NewVolumePosition);

  albaSmartPointer<albaTransformFrame> localPoseTransformFrame;
  localPoseTransformFrame->SetInput(resamplingBoxPose);
  
  albaSmartPointer<albaTransformFrame> outputToInputTransformFrame;
  
  albaString outputVmeName = "resampled_";
	outputVmeName += m_Input->GetName();

  m_ResampledVme = (albaVMEVolumeGray *)m_Input->NewInstance();
  m_ResampledVme->Register(m_ResampledVme);
  m_ResampledVme->GetTagArray()->DeepCopy(m_Input->GetTagArray());
  
  albaTagItem *ti = NULL;
  ti = m_ResampledVme->GetTagArray()->GetTag("VME_NATURE");
  if(ti)
  {
    ti->SetValue("SYNTHETIC");
  }
  else
  {
    albaTagItem tag_Nature;
    tag_Nature.SetName("VME_NATURE");
    tag_Nature.SetValue("SYNTHETIC");

    m_ResampledVme->GetTagArray()->SetTag(tag_Nature);
  }

  m_ResampledVme->SetName(outputVmeName);
  m_ResampledVme->ReparentTo(m_Input->GetParent());
  m_ResampledVme->SetMatrix(resamplingBoxPose->GetMatrix());

  int outputSPExtent[6];
  outputSPExtent[0] = 0;
  outputSPExtent[1] = round((m_VolumeBounds[1] - m_VolumeBounds[0]) / m_VolumeSpacing[0]);
  outputSPExtent[2] = 0;
  outputSPExtent[3] = round((m_VolumeBounds[3] - m_VolumeBounds[2]) / m_VolumeSpacing[1]);
  outputSPExtent[4] = 0;
  outputSPExtent[5] = round((m_VolumeBounds[5] - m_VolumeBounds[4]) / m_VolumeSpacing[2]);

  double w,l,sr[2];
  for (int i = 0; i < ((albaVMEGenericAbstract *)m_Input)->GetDataVector()->GetNumberOfItems(); i++)
  {
    if (albaVMEItemVTK *input_item = albaVMEItemVTK::SafeDownCast(((albaVMEGenericAbstract *)m_Input)->GetDataVector()->GetItemByIndex(i)))
    {
      if (vtkDataSet *inputData = input_item->GetData())
      {
        // the resample filter
        vtkALBASmartPointer<vtkALBAVolumeResample> volumeResampleFilter;
        volumeResampleFilter->SetZeroValue(m_ZeroPadValue);

        // Set the target be vme's parent frame. And Input frame to the root. I've to 
        // set at each iteration since I'm using the SetMatrix, which doesn't support
        // transform pipelines.
        albaSmartPointer<albaMatrix> outputParentAbsPose;
        m_ResampledVme->GetParent()->GetOutput()->GetAbsMatrix(*outputParentAbsPose.GetPointer(),input_item->GetTimeStamp());
        localPoseTransformFrame->SetInputFrame(outputParentAbsPose);

        albaSmartPointer<albaMatrix> inputParentAbsPose;
        ((albaVME *)m_Input->GetParent())->GetOutput()->GetAbsMatrix(*inputParentAbsPose.GetPointer(),input_item->GetTimeStamp());
        localPoseTransformFrame->SetTargetFrame(inputParentAbsPose);
        localPoseTransformFrame->Update();

        albaSmartPointer<albaMatrix> outputAbsPose;
        m_ResampledVme->GetOutput()->GetAbsMatrix(*outputAbsPose.GetPointer(),input_item->GetTimeStamp());
        outputToInputTransformFrame->SetInputFrame(resamplingBoxPose->GetMatrixPointer());

        albaSmartPointer<albaMatrix> inputAbsPose;
				inputAbsPose->Identity();
        outputToInputTransformFrame->SetTargetFrame(inputAbsPose);
        outputToInputTransformFrame->Update();

        double origin[3];
        origin[0] = m_VolumeBounds[0];
        origin[1] = m_VolumeBounds[2];
        origin[2] = m_VolumeBounds[4];

        outputToInputTransformFrame->TransformPoint(origin,origin);

        volumeResampleFilter->SetVolumeOrigin(origin[0],origin[1],origin[2]);
        
        vtkMatrix4x4 *outputToInputTransformFrameMatrix = outputToInputTransformFrame->GetMatrix().GetVTKMatrix();
     
        double xAxis[3],yAxis[3];

        albaMatrix::GetVersor(0,outputToInputTransformFrameMatrix,xAxis);
        albaMatrix::GetVersor(1,outputToInputTransformFrameMatrix,yAxis);
        
        volumeResampleFilter->SetVolumeAxisX(xAxis);
        volumeResampleFilter->SetVolumeAxisY(yAxis);
        
        vtkALBASmartPointer<vtkImageData> outputSPVtkData;
        outputSPVtkData->SetSpacing(m_VolumeSpacing);
        outputSPVtkData->SetScalarType(inputData->GetPointData()->GetScalars()->GetDataType());
        outputSPVtkData->SetExtent(outputSPExtent);
        outputSPVtkData->SetUpdateExtent(outputSPExtent);

        vtkDoubleArray *scalar = vtkDoubleArray::SafeDownCast(outputSPVtkData->GetPointData()->GetScalars());
        
        inputData->GetScalarRange(sr);

        w = sr[1] - sr[0];
        l = (sr[1] + sr[0]) * 0.5;

        volumeResampleFilter->SetWindow(w);
        volumeResampleFilter->SetLevel(l);
        volumeResampleFilter->SetInput(inputData);
        volumeResampleFilter->SetOutput(outputSPVtkData);
        volumeResampleFilter->AutoSpacingOff();
        volumeResampleFilter->Update();
        
        std::ostringstream stringStream;
        volumeResampleFilter->PrintSelf(stringStream,NULL);
        albaLogMessage(stringStream.str().c_str());

        stringStream.str("");
        this->PrintSelf(stringStream);
        albaLogMessage(stringStream.str().c_str());

        outputSPVtkData->SetSource(NULL);
        outputSPVtkData->SetOrigin(m_VolumeBounds[0],m_VolumeBounds[2],m_VolumeBounds[4]);

        m_ResampledVme->SetDataByDetaching(outputSPVtkData, input_item->GetTimeStamp());
        m_ResampledVme->Update();
      }
    }
  }
	m_ResampledVme->SetAbsMatrix(*m_Input->GetOutput()->GetAbsMatrix());
	m_Output = m_ResampledVme;

  std::ostringstream stringStream;
  PrintVolume(stringStream, m_Output,"Output Volume");
  albaLogMessage(stringStream.str().c_str(), "Output Volume");
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::OpUndo()
//----------------------------------------------------------------------------
{   
	assert(m_ResampledVme);
	GetLogicManager()->VmeRemove(m_ResampledVme);
}
//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
enum VOLUME_RESAMPLE_WIDGET_ID
{
	ID_FIRST = MINID,	
	ID_VOLUME_DIR_X,
	ID_VOLUME_DIR_Y,
	ID_VOLUME_DIR_Z,
  ID_VOLUME_ORIGIN,
  ID_VOLUME_ORIENTATION,
  ID_VOLUME_4DBOUNDS,
  ID_VOLUME_VMEBOUNDS,
  ID_VOLUME_VMELOCALBOUNDS,
  ID_VOLUME_SPACING,
  ID_VOLUME_CURRENT_SLICE,
  ID_VOLUME_AUTOSPACING,
  ID_VOLUME_ZERO_VALUE,
	ID_CHOOSE_GIZMO,
	ID_SHOW_HANDLE,
	ID_SHOW_GIZMO_TRANSFORM,
};

enum GIZMOS
{
	ID_GIZMO_TRANSLATE = 0,
	ID_GIZMO_ROTATE,
};
//----------------------------------------------------------------------------
void albaOpVolumeResample::UpdateGizmoData(albaEvent *e)
//----------------------------------------------------------------------------
{
	switch(e->GetId())
	{
	case ID_VOLUME_ORIENTATION:
		{
			albaSmartPointer<albaMatrix> LocMatr;
			albaSmartPointer<albaMatrix> NewAbsMatr;
			albaTransform::SetOrientation(*(LocMatr.GetPointer()),m_ROIOrientation[0],m_ROIOrientation[1],m_ROIOrientation[2]);
			albaSmartPointer<albaTransformFrame> mflTr;
			mflTr->SetInput(LocMatr);
			mflTr->SetInputFrame(m_CenterVolumeRefSysMatrix);
			mflTr->Update();
			NewAbsMatr->DeepCopy(&(mflTr->GetMatrix()));

			// build objects
			albaSmartPointer<albaMatrix> M;
			albaMatrix invOldAbsPose;
			albaSmartPointer<albaMatrix> newAbsPose;

			// incoming matrix is a translation matrix
			newAbsPose->DeepCopy(m_GizmoRotate->GetAbsPose());
			// copy rotation part from OldAbsPose into NewAbsPose
			albaTransform::CopyRotation(*NewAbsMatr,*newAbsPose);
			invOldAbsPose.DeepCopy(m_GizmoRotate->GetAbsPose());
			invOldAbsPose.Invert();
			albaMatrix::Multiply4x4(*newAbsPose.GetPointer(), invOldAbsPose, *M.GetPointer());
			// update gizmo abs pose
			m_GizmoRotate->SetAbsPose(newAbsPose, 0.0);

			vtkTransform *tranVMEDummy = vtkTransform::New();
			tranVMEDummy->PostMultiply();
			tranVMEDummy->SetMatrix(m_VMEDummy->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
			tranVMEDummy->Concatenate(M->GetVTKMatrix());
			tranVMEDummy->Update();

			albaSmartPointer<albaMatrix> newAbsMatrVMEDummy;
			newAbsMatrVMEDummy->DeepCopy(tranVMEDummy->GetMatrix());
			newAbsMatrVMEDummy->SetTimeStamp(0.0);

			m_VMEDummy->SetAbsMatrix(*newAbsMatrVMEDummy);
		}
		break;
	case ID_VOLUME_ORIGIN:
		{
			albaSmartPointer<albaMatrix> LocMatr;
			albaSmartPointer<albaMatrix> NewAbsMatr;
			albaTransform::SetPosition(*(LocMatr.GetPointer()),m_ROIPosition[0],m_ROIPosition[1],m_ROIPosition[2]);
			albaSmartPointer<albaTransformFrame> mflTr;
			mflTr->SetInput(LocMatr);
			mflTr->SetInputFrame(m_CenterVolumeRefSysMatrix);
			mflTr->Update();
			NewAbsMatr->DeepCopy(&(mflTr->GetMatrix()));

			// build objects
			albaSmartPointer<albaMatrix> M;
			albaMatrix invOldAbsPose;
			albaSmartPointer<albaMatrix> newAbsPose;

			// incoming matrix is a translation matrix
			newAbsPose->DeepCopy(NewAbsMatr);
			// copy rotation part from OldAbsPose into NewAbsPose
			albaTransform::CopyRotation(*(m_GizmoTranslate->GetAbsPose()),*newAbsPose.GetPointer());
			invOldAbsPose.DeepCopy(m_GizmoTranslate->GetAbsPose());
			invOldAbsPose.Invert();
			albaMatrix::Multiply4x4(*newAbsPose.GetPointer(), invOldAbsPose, *M.GetPointer());
			// update gizmo abs pose
			m_GizmoTranslate->SetAbsPose(newAbsPose, 0.0);

			vtkTransform *tranVMEDummy = vtkTransform::New();
			tranVMEDummy->PostMultiply();
			tranVMEDummy->SetMatrix(m_VMEDummy->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
			tranVMEDummy->Concatenate(M->GetVTKMatrix());
			tranVMEDummy->Update();

			albaSmartPointer<albaMatrix> newAbsMatrVMEDummy;
			newAbsMatrVMEDummy->DeepCopy(tranVMEDummy->GetMatrix());
			newAbsMatrVMEDummy->SetTimeStamp(0.0);

			m_VMEDummy->SetAbsMatrix(*newAbsMatrVMEDummy);
		}
		break;
	}
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::CreateGui() 
//----------------------------------------------------------------------------
{
	m_Gui = new albaGUI(this);

  //m_Gui->Button(ID_VOLUME_VMELOCALBOUNDS,"VME Local Bounds","","set the crop bounding box to the oriented VME bounds (default option)");
  //m_Gui->Button(ID_VOLUME_VMEBOUNDS,"VME Global Bounds","","set the crop bounding box to the VME global bounds");
  //m_Gui->Button(ID_VOLUME_4DBOUNDS,"VME 4D Bounds","","set the crop bounding box to the current VME 4D bounds");
  //m_Gui->Label("");

	m_Gui->Bool(ID_SHOW_HANDLE,_("Show Handle"),&m_ShowHandle,1);
	m_Gui->Bool(ID_SHOW_GIZMO_TRANSFORM,_("Show Gizmo Transform"),&m_ShowGizmoTransform,1);

	m_Gui->Label("ROI Selection",true);
	m_Gui->Label("Resample Bounding Box Extent");
	/*m_Gui->VectorN(ID_VOLUME_DIR_X, "X", &m_VolumeBounds[0], 2);
	m_Gui->VectorN(ID_VOLUME_DIR_Y, "Y", &m_VolumeBounds[2], 2);
	m_Gui->VectorN(ID_VOLUME_DIR_Z, "Z", &m_VolumeBounds[4], 2);*/
	m_Gui->Double(ID_VOLUME_DIR_X, "X", &m_MaxBoundX,0.0001);
	m_Gui->Double(ID_VOLUME_DIR_Y, "Y", &m_MaxBoundY,0.0001);
	m_Gui->Double(ID_VOLUME_DIR_Z, "Z", &m_MaxBoundZ,0.0001);
	m_Gui->Label("");

  m_Gui->Label("ROI Orientation",true);
	wxString chooses_gizmo[3];
	chooses_gizmo[0]="Translate Origin";
	chooses_gizmo[1]="Rotate Volume";
	m_Gui->Combo(ID_CHOOSE_GIZMO,"",&m_GizmoChoose,2,chooses_gizmo);
  
	m_Gui->Label("Bounding Box Origin");

  m_Gui->Vector(ID_VOLUME_ORIGIN, "", m_ROIPosition,MINFLOAT,MAXFLOAT,2,"output volume origin");
	m_Gui->Enable(ID_VOLUME_ORIGIN,m_GizmoChoose==ID_GIZMO_TRANSLATE);
  if(m_GizmoTranslate)
		m_GizmoTranslate->Show(m_GizmoChoose==ID_GIZMO_TRANSLATE);

  m_Gui->Label("Bounding Box Orientation");
  m_Gui->Vector(ID_VOLUME_ORIENTATION, "", m_ROIOrientation,MINFLOAT,MAXFLOAT,2,"output volume orientation");
	m_Gui->Enable(ID_VOLUME_ORIENTATION,m_GizmoChoose==ID_GIZMO_ROTATE);
	if(m_GizmoRotate)
		m_GizmoRotate->Show(m_GizmoChoose==ID_GIZMO_ROTATE);
  
	m_Gui->Label("Volume Spacing",false);

  m_Gui->Vector(ID_VOLUME_SPACING, "", this->m_VolumeSpacing,MINFLOAT,MAXFLOAT,4,"output volume spacing");
  m_Gui->Button(ID_VOLUME_AUTOSPACING,"AutoSpacing","","compute auto spacing by rotating original spacing");

  m_Gui->Label("");
  
  /*double range[2];
  wxString str_range;
  m_Input->GetOutput()->GetVTKData()->GetScalarRange(range);
  str_range.Printf("[ %.3f , %.3f ]",range[0],range[1]);
  
  m_Gui->Label("Scalar Range:");
  m_Gui->Label(str_range);*/

  m_Gui->Label("");
  m_Gui->Label("Padding Value");
  m_Gui->Double(ID_VOLUME_ZERO_VALUE,"",&m_ZeroPadValue);

	m_Gui->Label("");

	m_Gui->OkCancel();

	m_Gui->Divider();

	ShowGui();

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (alba_event->GetSender() == this->m_Gui)
	{
		OnEventThis(alba_event); 
	}
	else if(alba_event->GetSender() == this->m_GizmoROI)
	{
		OnEventGizmoROI(alba_event);
	}
	else if(alba_event->GetSender() == this->m_GizmoTranslate)
	{
		OnEventGizmoTranslate(alba_event);
	}
	else if(alba_event->GetSender() == this->m_GizmoRotate)
	{
		OnEventGizmoRotate(alba_event);
	}
	else
	{
		if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
		{
			switch(e->GetId())
			{
			case ID_TRANSFORM:
        if(m_ViewSelectedMessage)
          m_GizmoROI->GetBounds(m_VolumeBounds);
        else
          m_Input->GetOutput()->GetVMELocalBounds(m_VolumeBounds);
					m_Gui->Update();
					albaEventMacro(*e);
					break;
				default:
					albaEventMacro(*e);
					break;
			}
		}
		else
			albaEventMacro(*alba_event);
	}
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::OnEventGizmoROI(albaEventBase *alba_event) 
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		UpdateGui();
		switch(e->GetId())
		{
		case ID_TRANSFORM:
			{
			}
			break;
		default:
			albaEventMacro(*e);
			break;
		}
	}
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::OnEventThis(albaEventBase *alba_event) 
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
		case ID_SHOW_HANDLE:
			{
				m_GizmoROI->ShowHandles(m_ShowHandle != 0);
				GetLogicManager()->CameraUpdate();
			}
			break;
		case ID_SHOW_GIZMO_TRANSFORM:
			{
				m_GizmoRotate->Show(m_ShowGizmoTransform&&(m_GizmoChoose==ID_GIZMO_ROTATE));
				m_GizmoTranslate->Show(m_ShowGizmoTransform&&(m_GizmoChoose==ID_GIZMO_TRANSLATE));
				GetLogicManager()->CameraUpdate();
			}
			break;
			case ID_VOLUME_ORIENTATION:
				{
					ShiftCenterResampled();

					UpdateGizmoData(e);

					GetLogicManager()->CameraUpdate();
				}
				break;
			case ID_VOLUME_ORIGIN:
				{
					if(m_PrecedentPosition[0] != m_ROIPosition[0])
						m_NewVolumePosition[0] = m_NewVolumePosition[0] - m_PrecedentPosition[0] + m_ROIPosition[0];
					if(m_PrecedentPosition[1] != m_ROIPosition[1])
						m_NewVolumePosition[1] = m_NewVolumePosition[1] - m_PrecedentPosition[1] + m_ROIPosition[1];
					if(m_PrecedentPosition[2] != m_ROIPosition[2])
						m_NewVolumePosition[2] = m_NewVolumePosition[2] - m_PrecedentPosition[2] + m_ROIPosition[2];

					m_PrecedentPosition[0] = m_ROIPosition[0];
					m_PrecedentPosition[1] = m_ROIPosition[1];
					m_PrecedentPosition[2] = m_ROIPosition[2];

					UpdateGizmoData(e);

					GetLogicManager()->CameraUpdate();
				}
				break;
      case ID_VOLUME_DIR_X:		
      case ID_VOLUME_DIR_Y:
      case ID_VOLUME_DIR_Z:
				{
					double inputVolumeBBCentre[3];
					albaVMEVolumeGray *volume = albaVMEVolumeGray::SafeDownCast(m_Input);
					volume->GetOutput()->GetVTKData()->GetCenter(inputVolumeBBCentre);

					m_VolumeBounds[0] = inputVolumeBBCentre[0] - (m_MaxBoundX/2);
					m_VolumeBounds[1] = inputVolumeBBCentre[0] + (m_MaxBoundX/2);
					m_VolumeBounds[2] = inputVolumeBBCentre[1] - (m_MaxBoundY/2);
					m_VolumeBounds[3] = inputVolumeBBCentre[1] + (m_MaxBoundY/2);
					m_VolumeBounds[4] = inputVolumeBBCentre[2] - (m_MaxBoundZ/2);
					m_VolumeBounds[5] = inputVolumeBBCentre[2] + (m_MaxBoundZ/2);
					m_GizmoROI->SetBounds(m_VolumeBounds);

					GetLogicManager()->CameraUpdate();
				}
				break;
      case ID_VOLUME_SPACING:
      break;
      case ID_VOLUME_VMEBOUNDS:
        SetBoundsToVMEBounds();
        //UpdateGizmoData();
        UpdateGui();
				GetLogicManager()->CameraUpdate();
      break;
      case ID_VOLUME_4DBOUNDS:
        SetBoundsToVME4DBounds();
        //UpdateGizmoData();
        UpdateGui();
				GetLogicManager()->CameraUpdate();
      break;
      case ID_VOLUME_VMELOCALBOUNDS:
        SetBoundsToVMELocalBounds();
        //UpdateGizmoData();
        UpdateGui();
				GetLogicManager()->CameraUpdate();
      break;
      case ID_VOLUME_AUTOSPACING:
        AutoSpacing();
        UpdateGui();
      break;
      case wxOK:
        if (!CheckSpacing())
        {
          int answer = wxMessageBox( "Spacing values are too little and could generate memory problems - Continue?", "Warning", wxYES_NO, NULL);
          if (answer == wxNO)
          {
            break;
          }
        }
				Resample();
        GizmoDelete();
				OpStop(OP_RUN_OK);
      break;
      case wxCANCEL:
        GizmoDelete();
        OpStop(OP_RUN_CANCEL);		
      break;
      /*		case MOUSE_MOVE:
      {
      long handle_id = e.GetArg();
      float pos[3];
      vtkPoints *p = (vtkPoints *)e.GetVtkObj();
      p->GetPoint(0,pos);
      UpdateGizmo(handle_id, pos);
      UpdateGui();
      GetLogicManager()->CameraUpdate();

      }
      break;

      case MOUSE_UP:
      {
      UpdateHandlesDim();
      }
      */
			case ID_CHOOSE_GIZMO:
				{
					m_Gui->Enable(ID_VOLUME_ORIGIN,m_GizmoChoose==ID_GIZMO_TRANSLATE);
					if(m_GizmoTranslate)
					{
						if(m_GizmoChoose==ID_GIZMO_TRANSLATE)
							m_GizmoTranslate->SetAbsPose(m_GizmoRotate->GetAbsPose());
						m_GizmoTranslate->Show(m_ShowGizmoTransform&&(m_GizmoChoose==ID_GIZMO_TRANSLATE));
					}
					m_Gui->Enable(ID_VOLUME_ORIENTATION,m_GizmoChoose==ID_GIZMO_ROTATE);
					if(m_GizmoRotate)
					{
						//change the position of rotation gizmos
						if(m_GizmoChoose==ID_GIZMO_ROTATE)
							m_GizmoRotate->SetAbsPose(m_GizmoTranslate->GetAbsPose());
						m_GizmoRotate->Show(m_ShowGizmoTransform&&(m_GizmoChoose==ID_GIZMO_ROTATE));
					}

					GetLogicManager()->CameraUpdate();
				}
				break;
      default:
        albaEventMacro(*e);
      break;
    }	
  }
}
//----------------------------------------------------------------------------
bool albaOpVolumeResample::CheckSpacing()
//----------------------------------------------------------------------------
{
  if ((m_VolumeSpacing[0]/(m_VolumeBounds[1] - m_VolumeBounds[0]))*100 < SPACING_PERCENTAGE_BOUNDS)
  {
    return false;
  }
  if ((m_VolumeSpacing[1]/(m_VolumeBounds[3] - m_VolumeBounds[2]))*100 < SPACING_PERCENTAGE_BOUNDS)
  {
    return false;
  }
  if ((m_VolumeSpacing[2]/(m_VolumeBounds[5] - m_VolumeBounds[4]))*100 < SPACING_PERCENTAGE_BOUNDS)
  {
    return false;
  }
  
  return true;
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::OnEventGizmoTranslate(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	switch(alba_event->GetId())
	{
	case ID_TRANSFORM:
		{    
			// post multiplying matrixes coming from the gizmo to the vme
			// gizmo does not set vme pose  since they cannot scale
			PostMultiplyEventMatrix(alba_event);
		}
		break;

	default:
		{
			albaEventMacro(*alba_event);
		}
	}
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::OnEventGizmoRotate(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	switch(alba_event->GetId())
	{
	case ID_TRANSFORM:
		{    
			// post multiplying matrixes coming from the gizmo to the vme
			// gizmo does not set vme pose  since they cannot scale
			PostMultiplyEventMatrix(alba_event);
		}
		break;

	default:
		{
			albaEventMacro(*alba_event);
		}
	}
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::PostMultiplyEventMatrix(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		// handle incoming transform events
		albaSmartPointer<albaMatrix> newAbsMatr;
		if(e->GetSender()==m_GizmoTranslate)
			newAbsMatr->DeepCopy(m_GizmoTranslate->GetAbsPose()->GetVTKMatrix());
		else if(e->GetSender()==m_GizmoRotate)
			newAbsMatr->DeepCopy(m_GizmoRotate->GetAbsPose()->GetVTKMatrix());

		newAbsMatr->SetTimeStamp(0.0);

		if(e->GetSender()==m_GizmoTranslate)//translate
		{

			albaSmartPointer<albaTransformFrame> mflTr;
			mflTr->SetInput(newAbsMatr);
			mflTr->SetTargetFrame(m_CenterVolumeRefSysMatrix);
			mflTr->Update();

			albaTransform::GetPosition(mflTr->GetMatrix(),m_ROIPosition);

			if(m_PrecedentPosition[0] != m_ROIPosition[0])
				m_NewVolumePosition[0] = m_NewVolumePosition[0] - m_PrecedentPosition[0] + m_ROIPosition[0];
			if(m_PrecedentPosition[1] != m_ROIPosition[1])
				m_NewVolumePosition[1] = m_NewVolumePosition[1] - m_PrecedentPosition[1] + m_ROIPosition[1];
			if(m_PrecedentPosition[2] != m_ROIPosition[2])
				m_NewVolumePosition[2] = m_NewVolumePosition[2] - m_PrecedentPosition[2] + m_ROIPosition[2];

			m_PrecedentPosition[0] = m_ROIPosition[0];
			m_PrecedentPosition[1] = m_ROIPosition[1];
			m_PrecedentPosition[2] = m_ROIPosition[2];
		}
		else if(e->GetSender()==m_GizmoRotate)//rotate
		{
			albaSmartPointer<albaTransformFrame> mflTr;
			mflTr->SetInput(newAbsMatr);
			mflTr->SetTargetFrame(m_CenterVolumeRefSysMatrix);
			mflTr->Update();
			albaTransform::GetOrientation(mflTr->GetMatrix(),m_ROIOrientation);
			ShiftCenterResampled();
		}
		m_Gui->Update();

		vtkALBASmartPointer<vtkTransform> tran_bound_box;
		tran_bound_box->PostMultiply();
		tran_bound_box->SetMatrix(m_VMEDummy->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
		tran_bound_box->Concatenate(e->GetMatrix()->GetVTKMatrix());
		tran_bound_box->Update();

		albaSmartPointer<albaMatrix> newAbsMatrBox;
		newAbsMatrBox->DeepCopy(tran_bound_box->GetMatrix());
		newAbsMatrBox->SetTimeStamp(0.0);

		m_VMEDummy->SetAbsMatrix(*newAbsMatrBox);

		GetLogicManager()->CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::SetSpacing(double spacing[3]) 
//----------------------------------------------------------------------------
{
	m_VolumeSpacing[0] = spacing[0];
	m_VolumeSpacing[1] = spacing[1];
	m_VolumeSpacing[2] = spacing[2];
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::GetSpacing( double spacing[3] )
//----------------------------------------------------------------------------
{
  spacing[0] = m_VolumeSpacing[0];
  spacing[1] = m_VolumeSpacing[1];
  spacing[2] = m_VolumeSpacing[2];
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::SetBounds(double bounds[6],int type) 
//----------------------------------------------------------------------------
{
  switch (type)
  {
  case VME4DBOUNDS:
    SetBoundsToVME4DBounds();
    break;
  case VMELOCALBOUNDS:
    SetBoundsToVMELocalBounds();
    break;
  case VMEBOUNDS:
    this->SetBoundsToVMEBounds();
    break;
  case CUSTOMBOUNDS:
    m_VolumeBounds[0] = bounds[0];
    m_VolumeBounds[1] = bounds[1];
    m_VolumeBounds[2] = bounds[2];
    m_VolumeBounds[3] = bounds[3];
    m_VolumeBounds[4] = bounds[4];
    m_VolumeBounds[5] = bounds[5];
    break;
  }
}

void albaOpVolumeResample::SetROIOrientation(double ROIOrientation[3]) 
{
  m_ROIOrientation[0] = ROIOrientation[0];
  m_ROIOrientation[1] = ROIOrientation[1];
  m_ROIOrientation[2] = ROIOrientation[2];
}

void albaOpVolumeResample::SetVolumePosition(double  volumePosition[3]) 
{
  m_VolumePosition[0] = volumePosition[0];
  m_VolumePosition[1] = volumePosition[1];
  m_VolumePosition[2] = volumePosition[2];
}

//----------------------------------------------------------------------------
void albaOpVolumeResample::ShiftCenterResampled() 
//----------------------------------------------------------------------------
{

	double inputVolumeLocalVTKBBCenter[3];

	m_Input->GetOutput()->GetVTKData()->GetCenter(inputVolumeLocalVTKBBCenter);

	vtkALBASmartPointer<vtkPoints> points;
	points->InsertNextPoint(inputVolumeLocalVTKBBCenter);

	vtkALBASmartPointer<vtkPolyData> poly;
	poly->SetPoints(points);
	poly->Update();

	vtkALBASmartPointer<vtkTransform> t;
	t->RotateX(m_ROIOrientation[0]);
	t->RotateY(m_ROIOrientation[1]);
	t->RotateZ(m_ROIOrientation[2]);
	t->Update();

	vtkALBASmartPointer<vtkTransformPolyDataFilter> ptf;
	ptf->SetTransform(t);
	ptf->SetInput(poly);
	ptf->Update();

	double pt[3];
	ptf->GetOutput()->GetPoint(0,pt);

	double difference[3];
	difference[0] = inputVolumeLocalVTKBBCenter[0] - pt[0];
	difference[1] = inputVolumeLocalVTKBBCenter[1] - pt[1];
	difference[2] = inputVolumeLocalVTKBBCenter[2] - pt[2];

	m_NewVolumePosition[0] = difference[0] + m_VolumePosition[0];
	m_NewVolumePosition[1] = difference[1] + m_VolumePosition[1];
	m_NewVolumePosition[2] = difference[2] + m_VolumePosition[2];
}
//----------------------------------------------------------------------------
void albaOpVolumeResample::OpStop(int result)
//----------------------------------------------------------------------------
{
	if (!m_TestMode)
	{
		HideGui();
	}
	albaEventMacro(albaEvent(this,result));
}

//----------------------------------------------------------------------------
void albaOpVolumeResample::PrintSelf(ostream& os)
//----------------------------------------------------------------------------
{
  os << "-------------------------------------------------------" << std::endl;
  os << "albaOpVolumeResample PrintSelf:" << std::endl;

  PrintVolume(os, m_Input, "Input Volume");
  
  albaString parameter;
  
  parameter.Append("m_MaxBoundX = ");
  parameter.Append(wxString::Format("%f", m_MaxBoundX));
  parameter.Append("\n");
  parameter.Append("m_MaxBoundY = ");
  parameter.Append(wxString::Format("%f", m_MaxBoundY));
  parameter.Append("\n");
  parameter.Append("m_MaxBoundZ = ");
  parameter.Append(wxString::Format("%f", m_MaxBoundZ));
  parameter.Append("\n");
  parameter.Append("m_ROIPosition[0] = ");
  parameter.Append(wxString::Format("%f", m_ROIPosition[0]));
  parameter.Append("\n");
  parameter.Append("m_ROIPosition[1] = ");
  parameter.Append(wxString::Format("%f", m_ROIPosition[1]));
  parameter.Append("\n");
  parameter.Append("m_ROIPosition[2] = ");
  parameter.Append(wxString::Format("%f", m_ROIPosition[2]));
  parameter.Append("\n");
  parameter.Append("m_ROIOrientation[0] = ");
  parameter.Append(wxString::Format("%f", m_ROIOrientation[0]));
  parameter.Append("\n");
  parameter.Append("m_ROIOrientation[1] = ");
  parameter.Append(wxString::Format("%f", m_ROIOrientation[1]));
  parameter.Append("\n");
  parameter.Append("m_ROIOrientation[2] = ");
  parameter.Append(wxString::Format("%f", m_ROIOrientation[2]));
  parameter.Append("\n");
  parameter.Append("m_NewVolumePosition[0] = ");
  parameter.Append(wxString::Format("%f", m_NewVolumePosition[0]));
  parameter.Append("\n");
  parameter.Append("m_NewVolumePosition[1] = ");
  parameter.Append(wxString::Format("%f", m_NewVolumePosition[1]));
  parameter.Append("\n");
  parameter.Append("m_NewVolumePosition[2] = ");
  parameter.Append(wxString::Format("%f", m_NewVolumePosition[2]));
  parameter.Append("\n");
  parameter.Append("m_VolumeBounds[0] = ");
  parameter.Append(wxString::Format("%f", m_VolumeBounds[0]));
  parameter.Append("\n");
  parameter.Append("m_VolumeBounds[1] = ");
  parameter.Append(wxString::Format("%f", m_VolumeBounds[1]));
  parameter.Append("\n");
  parameter.Append("m_VolumeBounds[2] = ");
  parameter.Append(wxString::Format("%f", m_VolumeBounds[2]));
  parameter.Append("\n");
  parameter.Append("m_VolumeBounds[3] = ");
  parameter.Append(wxString::Format("%f", m_VolumeBounds[3]));
  parameter.Append("\n");
  parameter.Append("m_VolumeBounds[4] = ");
  parameter.Append(wxString::Format("%f", m_VolumeBounds[4]));
  parameter.Append("\n");
  parameter.Append("m_VolumeBounds[5] = ");
  parameter.Append("\n");
  parameter.Append(wxString::Format("%f", m_VolumeBounds[5]));
  parameter.Append("\n");
  parameter.Append("m_VolumeSpacing[0] = ");
  parameter.Append(wxString::Format("%f", m_VolumeSpacing[0]));
  parameter.Append("\n");
  parameter.Append("m_VolumeSpacing[1] = ");
  parameter.Append(wxString::Format("%f", m_VolumeSpacing[1]));
  parameter.Append("\n");
  parameter.Append("m_VolumeSpacing[2] = ");
  parameter.Append(wxString::Format("%f", m_VolumeSpacing[2]));
  parameter.Append("\n");
  parameter.Append("m_VolumePosition[0] = ");
  parameter.Append(wxString::Format("%f", m_VolumePosition[0]));
  parameter.Append("\n");
  parameter.Append("m_VolumePosition[1] = ");
  parameter.Append(wxString::Format("%f", m_VolumePosition[1]));
  parameter.Append("\n");
  parameter.Append("m_VolumePosition[2] = ");
  parameter.Append(wxString::Format("%f",m_VolumePosition[2]));
  parameter.Append("\n");
  parameter.Append("m_VolumeOrientation[0] = ");
  parameter.Append(wxString::Format("%f", m_VolumeOrientation[0]));
  parameter.Append("\n");
  parameter.Append("m_VolumeOrientation[1] = ");
  parameter.Append(wxString::Format("%f", m_VolumeOrientation[1]));
  parameter.Append("\n");
  parameter.Append("m_VolumeOrientation[2] = ");
  parameter.Append(wxString::Format("%f",m_VolumeOrientation[2]));

  parameter.Append("\n");
  parameter.Append("m_ZeroPadValue = ");
  parameter.Append(wxString::Format("%f", m_ZeroPadValue));

  os << parameter.GetCStr();
  os << std::endl;
  os << "-------------------------------------------------------" << std::endl;
}


void albaOpVolumeResample::PrintDouble6( ostream& os, double array[6], const char *logMessage /*= NULL */ )
{

  if (logMessage) os << logMessage << std::endl;
  os << "xmin, xmax [" << array[0] << " , " << array[1] << "]" << std::endl;
  os << "ymin, ymax [" << array[2] << " , " << array[3] << "]" << std::endl;
  os << "zmin, zmax [" << array[4] << " , " << array[5] << "]" << std::endl;
  os << std::endl;
}

void albaOpVolumeResample::PrintDouble3( ostream& os, double array[3], const char *logMessage /*= NULL*/ )
{
  if (logMessage) os << logMessage << " [" << array[0] << " , " << array[1] << " , " << array[2] << " ]" << std::endl;
  os << std::endl;
}


void albaOpVolumeResample::PrintInt3( ostream& os, int array[3], const char *logMessage /*= NULL*/ )
{
  if (logMessage) os << logMessage << " [" << array[0] << " , " << array[1] << " , " << array[2] << " ]" << std::endl;
  os << std::endl;
}

void albaOpVolumeResample::PrintVolume( ostream& os , albaVME *volume , const char *logMessage /*= NULL*/ )
{
  albaVMEVolumeGray *input = albaVMEVolumeGray::SafeDownCast(volume);
  input->GetOutput()->GetVTKData()->Update();
  vtkDataSet *inputDataSet = input->GetOutput()->GetVTKData();
  if (logMessage) os << logMessage << std::endl;
  os << "data is: ";
  if (inputDataSet->IsA("vtkImageData"))
  {
    os << "vtkImageData" << std::endl;
    vtkImageData *sp = vtkImageData::SafeDownCast(inputDataSet);
    double origin[3];
    sp->GetOrigin(origin);
    PrintDouble3(os, origin, "origin");
    double spacing[3];
    sp->GetSpacing(spacing);
    PrintDouble3(os, spacing, "spacing");
    int dim[3];
    sp->GetDimensions(dim);
    PrintInt3(os, dim, "dim");

  } 
  else if (inputDataSet->IsA("vtkRectilinearGrid"))
  {
    os << "vtkRectilinearGrid" << std::endl;
    vtkRectilinearGrid *rg = vtkRectilinearGrid::SafeDownCast(inputDataSet);
    int dimensions[3];
    rg->GetDimensions(dimensions);
    PrintInt3(os, dimensions, "dim");
  }


  double inBounds[6];
  inputDataSet->GetBounds(inBounds);
  PrintDouble6(os, inBounds, "vtk data bounds");
  double boundingBoxCenter[6];
  inputDataSet->GetCenter(boundingBoxCenter);
  PrintDouble3(os, boundingBoxCenter, "vtk data BB centre");

  double absBounds[6];
  input->GetOutput()->GetVMEBounds(absBounds);
  PrintDouble6(os, absBounds, "vme abs bounds");
  
}

void albaOpVolumeResample::SetNewVolumePosition( double newVolumePosition[3] )
{
  m_NewVolumePosition[0] = newVolumePosition[0];
  m_NewVolumePosition[1] = newVolumePosition[1];
  m_NewVolumePosition[2] = newVolumePosition[2];
}

void albaOpVolumeResample::SetMaxBounds( double maxBound[3] )
{
  m_MaxBoundX = maxBound[0];
  m_MaxBoundY = maxBound[1];
  m_MaxBoundZ = maxBound[2];
}

void albaOpVolumeResample::SetROIPosition( double roiPosition[3] )
{
  m_ROIPosition[0] = roiPosition[0];
  m_ROIPosition[1] = roiPosition[1];
  m_ROIPosition[2] = roiPosition[2];

}

