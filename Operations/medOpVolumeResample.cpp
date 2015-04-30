/*=========================================================================

 Program: MAF2
 Module: medOpVolumeResample
 Authors: Marco Petrone
 
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


#include "medOpVolumeResample.h"

#include "mafEvent.h"
#include "mafGUI.h"
#include "mmaMaterial.h"

#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEGizmo.h"
#include "mafGizmoHandle.h"
#include "mafGizmoTranslate.h"
#include "mafGizmoRotate.h"
#include "mafGizmoHandle.h"
#include "mafGizmoROI.h"
#include "mafVMEItemVTK.h"
#include "mafTagArray.h"
#include "mafDataVector.h"

#include "mafTransform.h"
#include "mafTransformFrame.h"
#include "mafInteractorGenericMouse.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFVolumeResample.h"
#include "vtkOutlineSource.h"
#include "vtkProperty.h"
#include "vtkPolyData.h"
#include "vtkStructuredPoints.h"
#include "vtkRectilinearGrid.h"
#include "vtkPointData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkCubeSource.h"
#include "vtkDoubleArray.h"


#include <vector>
#include <algorithm>

#define round(x) (x<0?ceil((x)-0.5):floor((x)+0.5))

#define SPACING_PERCENTAGE_BOUNDS 0.1

using namespace std;

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpVolumeResample);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpVolumeResample::medOpVolumeResample(const wxString &label /* =  */,bool showShadingPlane /* = false */) : mafOp(label)
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
medOpVolumeResample::~medOpVolumeResample()
//----------------------------------------------------------------------------
{
  GizmoDelete();

  if (m_VMEDummy)
  {
  	m_VMEDummy->ReparentTo(NULL);
  }
  mafDEL(m_VMEDummy);
  mafDEL(m_CenterVolumeRefSysMatrix);
	mafDEL(m_ResampledVme);
}
//----------------------------------------------------------------------------
bool medOpVolumeResample::Accept(mafNode* vme) 
//----------------------------------------------------------------------------
{
	mafEvent e(this,VIEW_SELECTED);
	mafEventMacro(e);
  return (vme && vme->IsMAFType(mafVMEVolumeGray));
}
//----------------------------------------------------------------------------
void medOpVolumeResample::InternalUpdateBounds(double bounds[6], bool center)
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
void medOpVolumeResample::CreateGizmos()
//----------------------------------------------------------------------------
{
  mafEvent e(this,VIEW_SELECTED);
  mafEventMacro(e);
  m_ViewSelectedMessage = e.GetBool();

	m_GizmoROI = new mafGizmoROI(mafVME::SafeDownCast(m_Input), this, mafGizmoHandle::FREE,m_VMEDummy,m_ShowShadingPlane);
  m_GizmoROI->ShowShadingPlane(true);
	m_GizmoROI->Show(true && m_ViewSelectedMessage);
	
  if(m_ViewSelectedMessage)
    m_GizmoROI->GetBounds(m_VolumeBounds);
  else
    mafVME::SafeDownCast(m_Input)->GetOutput()->GetVMELocalBounds(m_VolumeBounds);


	SetBoundsToVMELocalBounds();

	mafTransform::GetOrientation(*(((mafVME*)m_Input)->GetOutput()->GetAbsMatrix()),m_VolumeOrientation);

	mafVMEVolumeGray *inputVolume = mafVMEVolumeGray::SafeDownCast(m_Input);
	inputVolume->GetOutput()->GetVTKData()->GetCenter(m_VolumeCenterPosition);

	//Compute the center of Volume in absolute coordinate to center gizmo
	vtkMAFSmartPointer<vtkPoints> point;
	point->InsertNextPoint(m_VolumeCenterPosition);
	vtkMAFSmartPointer<vtkPolyData> polydata;
	polydata->SetPoints(point);
	vtkMAFSmartPointer<vtkTransform> transform;
	transform->Identity();
	transform->SetMatrix(inputVolume->GetOutput()->GetMatrix()->GetVTKMatrix());
	transform->Update();
	vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformFilter;
	transformFilter->SetInput(polydata);
	transformFilter->SetTransform(transform);
	transformFilter->Update();
	transformFilter->GetOutput()->GetCenter(m_VolumeCenterPosition);

	vtkMAFSmartPointer<vtkTransform> startTransform;
	startTransform->Identity();
	startTransform->Translate(m_VolumeCenterPosition);
	startTransform->RotateX(m_VolumeOrientation[0]);
	startTransform->RotateY(m_VolumeOrientation[1]);
	startTransform->RotateZ(m_VolumeOrientation[2]);
	startTransform->Update();

	//Save ref sys of center volume
	mafNEW(m_CenterVolumeRefSysMatrix);
	m_CenterVolumeRefSysMatrix->Identity();
	m_CenterVolumeRefSysMatrix->SetVTKMatrix(startTransform->GetMatrix());

	if(!m_TestMode)
	{
		m_GizmoTranslate = new mafGizmoTranslate(mafVME::SafeDownCast(m_Input), this);
		m_GizmoTranslate->SetRefSys(mafVME::SafeDownCast(m_Input));
		m_GizmoTranslate->SetAbsPose(m_CenterVolumeRefSysMatrix);
		m_GizmoTranslate->Show(true && e.GetBool());
		m_GizmoRotate = new mafGizmoRotate(mafVME::SafeDownCast(m_Input), this);
		m_GizmoRotate->SetRefSys(mafVME::SafeDownCast(m_Input));
		m_GizmoRotate->SetAbsPose(m_CenterVolumeRefSysMatrix);
		m_GizmoRotate->Show(false);
		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
	}
}
//----------------------------------------------------------------------------
void medOpVolumeResample::AutoSpacing()
//----------------------------------------------------------------------------
{
  vtkDataSet *vme_data = ((mafVME *)m_Input)->GetOutput()->GetVTKData();

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
void medOpVolumeResample::UpdateGui()
//----------------------------------------------------------------------------
{
	m_MaxBoundX = m_VolumeBounds[1] - m_VolumeBounds[0];
	m_MaxBoundY = m_VolumeBounds[3] - m_VolumeBounds[2];
	m_MaxBoundZ = m_VolumeBounds[5] - m_VolumeBounds[4];

  if(m_ViewSelectedMessage)
    m_GizmoROI->GetBounds(m_VolumeBounds);
  else
    mafVME::SafeDownCast(m_Input)->GetOutput()->GetVMELocalBounds(m_VolumeBounds);
	if(m_Gui)
		m_Gui->Update();
}
//----------------------------------------------------------------------------
void medOpVolumeResample::SetBoundsToVMEBounds()
//----------------------------------------------------------------------------
{
  double bounds[6];
  ((mafVME *)m_Input)->GetOutput()->GetVMEBounds(bounds);

  InternalUpdateBounds(bounds,true);
  m_VolumeOrientation[0] = m_VolumeOrientation[1] = m_VolumeOrientation[2] = 0;
}
//----------------------------------------------------------------------------
void medOpVolumeResample::SetBoundsToVME4DBounds()
//----------------------------------------------------------------------------
{
  double bounds[6];
  ((mafVME *)m_Input)->GetOutput()->GetVME4DBounds(bounds);

  InternalUpdateBounds(bounds,true);
  m_VolumeOrientation[0] = m_VolumeOrientation[1] = m_VolumeOrientation[2] = 0;
}
//----------------------------------------------------------------------------
void medOpVolumeResample::SetBoundsToVMELocalBounds()
//----------------------------------------------------------------------------
{
  double bounds[6];
  ((mafVME *)m_Input)->GetOutput()->GetVMELocalBounds(bounds);

  InternalUpdateBounds(bounds,false);
  ((mafVME *)m_Input)->GetOutput()->GetAbsPose(m_VolumePosition,m_VolumeOrientation);
	
	m_ROIOrientation[0] = m_ROIOrientation[1] = m_ROIOrientation[2] = 0;
	m_ROIPosition[0] = m_ROIPosition[1] = m_ROIPosition[2] = 0;
}
//----------------------------------------------------------------------------
void medOpVolumeResample::GizmoDelete()
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

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
mafOp *medOpVolumeResample::Copy()
//----------------------------------------------------------------------------
{
	return new medOpVolumeResample(m_Label,m_ShowShadingPlane);
}
//----------------------------------------------------------------------------
void medOpVolumeResample::InizializeVMEDummy()   
//----------------------------------------------------------------------------
{
	mafNEW(m_VMEDummy);
	vtkMAFSmartPointer<vtkCubeSource> cube;
	m_VMEDummy->SetData(vtkPolyData::SafeDownCast(cube->GetOutput()),0.0);
	m_VMEDummy->SetVisibleToTraverse(false);
	m_VMEDummy->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
	m_VMEDummy->ReparentTo(m_Input->GetRoot());
	m_VMEDummy->SetAbsMatrix(*(((mafVME*)m_Input)->GetOutput()->GetAbsMatrix()));
  m_VMEDummy->SetName("Dummy");
}
//----------------------------------------------------------------------------
void medOpVolumeResample::OpRun()   
//----------------------------------------------------------------------------
{
	InizializeVMEDummy();
	CreateGizmos();
	if(!this->m_TestMode)
		CreateGui();
	UpdateGui();
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medOpVolumeResample::OpDo()
//----------------------------------------------------------------------------
{
	m_ResampledVme->ReparentTo(m_Input->GetParent());
}
//----------------------------------------------------------------------------
void medOpVolumeResample::Resample()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafTransform> resamplingBoxPose;
  resamplingBoxPose->SetOrientation(m_ROIOrientation);
  resamplingBoxPose->SetPosition(m_NewVolumePosition);

  mafSmartPointer<mafTransformFrame> localPoseTransformFrame;
  localPoseTransformFrame->SetInput(resamplingBoxPose);
  
  mafSmartPointer<mafTransformFrame> outputToInputTransformFrame;
  
  mafString outputVmeName = "resampled_";
	outputVmeName += m_Input->GetName();

  m_ResampledVme = (mafVMEVolumeGray *)m_Input->NewInstance();
  m_ResampledVme->Register(m_ResampledVme);
  m_ResampledVme->GetTagArray()->DeepCopy(m_Input->GetTagArray());
  
  mafTagItem *ti = NULL;
  ti = m_ResampledVme->GetTagArray()->GetTag("VME_NATURE");
  if(ti)
  {
    ti->SetValue("SYNTHETIC");
  }
  else
  {
    mafTagItem tag_Nature;
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
  for (int i = 0; i < ((mafVMEGenericAbstract *)m_Input)->GetDataVector()->GetNumberOfItems(); i++)
  {
    if (mafVMEItemVTK *input_item = mafVMEItemVTK::SafeDownCast(((mafVMEGenericAbstract *)m_Input)->GetDataVector()->GetItemByIndex(i)))
    {
      if (vtkDataSet *inputData = input_item->GetData())
      {
        // the resample filter
        vtkMAFSmartPointer<vtkMAFVolumeResample> volumeResampleFilter;
        volumeResampleFilter->SetZeroValue(m_ZeroPadValue);

        // Set the target be vme's parent frame. And Input frame to the root. I've to 
        // set at each iteration since I'm using the SetMatrix, which doesn't support
        // transform pipelines.
        mafSmartPointer<mafMatrix> outputParentAbsPose;
        m_ResampledVme->GetParent()->GetOutput()->GetAbsMatrix(*outputParentAbsPose.GetPointer(),input_item->GetTimeStamp());
        localPoseTransformFrame->SetInputFrame(outputParentAbsPose);

        mafSmartPointer<mafMatrix> inputParentAbsPose;
        ((mafVME *)m_Input->GetParent())->GetOutput()->GetAbsMatrix(*inputParentAbsPose.GetPointer(),input_item->GetTimeStamp());
        localPoseTransformFrame->SetTargetFrame(inputParentAbsPose);
        localPoseTransformFrame->Update();

        mafSmartPointer<mafMatrix> outputAbsPose;
        m_ResampledVme->GetOutput()->GetAbsMatrix(*outputAbsPose.GetPointer(),input_item->GetTimeStamp());
        outputToInputTransformFrame->SetInputFrame(resamplingBoxPose->GetMatrixPointer());

        mafSmartPointer<mafMatrix> inputAbsPose;
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

        mafMatrix::GetVersor(0,outputToInputTransformFrameMatrix,xAxis);
        mafMatrix::GetVersor(1,outputToInputTransformFrameMatrix,yAxis);
        
        volumeResampleFilter->SetVolumeAxisX(xAxis);
        volumeResampleFilter->SetVolumeAxisY(yAxis);
        
        vtkMAFSmartPointer<vtkStructuredPoints> outputSPVtkData;
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
        mafLogMessage(stringStream.str().c_str());

        stringStream.str("");
        this->PrintSelf(stringStream);
        mafLogMessage(stringStream.str().c_str());

        outputSPVtkData->SetSource(NULL);
        outputSPVtkData->SetOrigin(m_VolumeBounds[0],m_VolumeBounds[2],m_VolumeBounds[4]);

        m_ResampledVme->SetDataByDetaching(outputSPVtkData, input_item->GetTimeStamp());
        m_ResampledVme->Update();
      }
    }
  }
	mafMatrix identity;
	m_ResampledVme->SetMatrix(identity);
	m_Output = m_ResampledVme;

  std::ostringstream stringStream;
  PrintVolume(stringStream, m_Output,"Output Volume");
  mafLogMessage(stringStream.str().c_str(), "Output Volume");
}
//----------------------------------------------------------------------------
void medOpVolumeResample::OpUndo()
//----------------------------------------------------------------------------
{   
	assert(m_ResampledVme);
	mafEventMacro(mafEvent(this,VME_REMOVE,m_ResampledVme));
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
	ID_HELP,
};

enum GIZMOS
{
	ID_GIZMO_TRANSLATE = 0,
	ID_GIZMO_ROTATE,
};
//----------------------------------------------------------------------------
void medOpVolumeResample::UpdateGizmoData(mafEvent *e)
//----------------------------------------------------------------------------
{
	switch(e->GetId())
	{
	case ID_VOLUME_ORIENTATION:
		{
			mafSmartPointer<mafMatrix> LocMatr;
			mafSmartPointer<mafMatrix> NewAbsMatr;
			mafTransform::SetOrientation(*(LocMatr.GetPointer()),m_ROIOrientation[0],m_ROIOrientation[1],m_ROIOrientation[2]);
			mafSmartPointer<mafTransformFrame> mflTr;
			mflTr->SetInput(LocMatr);
			mflTr->SetInputFrame(m_CenterVolumeRefSysMatrix);
			mflTr->Update();
			NewAbsMatr->DeepCopy(&(mflTr->GetMatrix()));

			// build objects
			mafSmartPointer<mafMatrix> M;
			mafMatrix invOldAbsPose;
			mafSmartPointer<mafMatrix> newAbsPose;

			// incoming matrix is a translation matrix
			newAbsPose->DeepCopy(m_GizmoRotate->GetAbsPose());
			// copy rotation part from OldAbsPose into NewAbsPose
			mafTransform::CopyRotation(*NewAbsMatr,*newAbsPose);
			invOldAbsPose.DeepCopy(m_GizmoRotate->GetAbsPose());
			invOldAbsPose.Invert();
			mafMatrix::Multiply4x4(*newAbsPose.GetPointer(), invOldAbsPose, *M.GetPointer());
			// update gizmo abs pose
			m_GizmoRotate->SetAbsPose(newAbsPose, 0.0);

			vtkTransform *tranVMEDummy = vtkTransform::New();
			tranVMEDummy->PostMultiply();
			tranVMEDummy->SetMatrix(m_VMEDummy->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
			tranVMEDummy->Concatenate(M->GetVTKMatrix());
			tranVMEDummy->Update();

			mafSmartPointer<mafMatrix> newAbsMatrVMEDummy;
			newAbsMatrVMEDummy->DeepCopy(tranVMEDummy->GetMatrix());
			newAbsMatrVMEDummy->SetTimeStamp(0.0);

			m_VMEDummy->SetAbsMatrix(*newAbsMatrVMEDummy);
		}
		break;
	case ID_VOLUME_ORIGIN:
		{
			mafSmartPointer<mafMatrix> LocMatr;
			mafSmartPointer<mafMatrix> NewAbsMatr;
			mafTransform::SetPosition(*(LocMatr.GetPointer()),m_ROIPosition[0],m_ROIPosition[1],m_ROIPosition[2]);
			mafSmartPointer<mafTransformFrame> mflTr;
			mflTr->SetInput(LocMatr);
			mflTr->SetInputFrame(m_CenterVolumeRefSysMatrix);
			mflTr->Update();
			NewAbsMatr->DeepCopy(&(mflTr->GetMatrix()));

			// build objects
			mafSmartPointer<mafMatrix> M;
			mafMatrix invOldAbsPose;
			mafSmartPointer<mafMatrix> newAbsPose;

			// incoming matrix is a translation matrix
			newAbsPose->DeepCopy(NewAbsMatr);
			// copy rotation part from OldAbsPose into NewAbsPose
			mafTransform::CopyRotation(*(m_GizmoTranslate->GetAbsPose()),*newAbsPose.GetPointer());
			invOldAbsPose.DeepCopy(m_GizmoTranslate->GetAbsPose());
			invOldAbsPose.Invert();
			mafMatrix::Multiply4x4(*newAbsPose.GetPointer(), invOldAbsPose, *M.GetPointer());
			// update gizmo abs pose
			m_GizmoTranslate->SetAbsPose(newAbsPose, 0.0);

			vtkTransform *tranVMEDummy = vtkTransform::New();
			tranVMEDummy->PostMultiply();
			tranVMEDummy->SetMatrix(m_VMEDummy->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
			tranVMEDummy->Concatenate(M->GetVTKMatrix());
			tranVMEDummy->Update();

			mafSmartPointer<mafMatrix> newAbsMatrVMEDummy;
			newAbsMatrVMEDummy->DeepCopy(tranVMEDummy->GetMatrix());
			newAbsMatrVMEDummy->SetTimeStamp(0.0);

			m_VMEDummy->SetAbsMatrix(*newAbsMatrVMEDummy);
		}
		break;
	}
}
//----------------------------------------------------------------------------
void medOpVolumeResample::CreateGui() 
//----------------------------------------------------------------------------
{
	m_Gui = new mafGUI(this);
	mafEvent buildHelpGui;
	buildHelpGui.SetSender(this);
	buildHelpGui.SetId(GET_BUILD_HELP_GUI);
	mafEventMacro(buildHelpGui);

	if (buildHelpGui.GetArg() == true)
	{
		m_Gui->Button(ID_HELP, "Help","");	
	}

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
  ((mafVME *)m_Input)->GetOutput()->GetVTKData()->GetScalarRange(range);
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

	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medOpVolumeResample::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (maf_event->GetSender() == this->m_Gui)
	{
		OnEventThis(maf_event); 
	}
	else if(maf_event->GetSender() == this->m_GizmoROI)
	{
		OnEventGizmoROI(maf_event);
	}
	else if(maf_event->GetSender() == this->m_GizmoTranslate)
	{
		OnEventGizmoTranslate(maf_event);
	}
	else if(maf_event->GetSender() == this->m_GizmoRotate)
	{
		OnEventGizmoRotate(maf_event);
	}
	else
	{
		if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
		{
			switch(e->GetId())
			{
			case ID_TRANSFORM:
        if(m_ViewSelectedMessage)
          m_GizmoROI->GetBounds(m_VolumeBounds);
        else
          mafVME::SafeDownCast(m_Input)->GetOutput()->GetVMELocalBounds(m_VolumeBounds);
					m_Gui->Update();
					mafEventMacro(*e);
					break;
				default:
					mafEventMacro(*e);
					break;
			}
		}
		else
			mafEventMacro(*maf_event);
	}
}
//----------------------------------------------------------------------------
void medOpVolumeResample::OnEventGizmoROI(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		UpdateGui();
		switch(e->GetId())
		{
		case ID_TRANSFORM:
			{
			}
			break;
		default:
			mafEventMacro(*e);
			break;
		}
	}
}
//----------------------------------------------------------------------------
void medOpVolumeResample::OnEventThis(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
		case ID_HELP:
		{
			mafEvent helpEvent;
			helpEvent.SetSender(this);
			mafString operationLabel = this->m_Label;
			helpEvent.SetString(&operationLabel);
			helpEvent.SetId(OPEN_HELP_PAGE);
			mafEventMacro(helpEvent);
		}
		break;

		case ID_SHOW_HANDLE:
			{
				m_GizmoROI->ShowHandles(m_ShowHandle != 0);
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
			}
			break;
		case ID_SHOW_GIZMO_TRANSFORM:
			{
				m_GizmoRotate->Show(m_ShowGizmoTransform&&(m_GizmoChoose==ID_GIZMO_ROTATE));
				m_GizmoTranslate->Show(m_ShowGizmoTransform&&(m_GizmoChoose==ID_GIZMO_TRANSLATE));
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
			}
			break;
			case ID_VOLUME_ORIENTATION:
				{
					ShiftCenterResampled();

					UpdateGizmoData(e);

					mafEventMacro(mafEvent(this, CAMERA_UPDATE));
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

					mafEventMacro(mafEvent(this, CAMERA_UPDATE));
				}
				break;
      case ID_VOLUME_DIR_X:		
      case ID_VOLUME_DIR_Y:
      case ID_VOLUME_DIR_Z:
				{
					double inputVolumeBBCentre[3];
					mafVMEVolumeGray *volume = mafVMEVolumeGray::SafeDownCast(m_Input);
					volume->GetOutput()->GetVTKData()->GetCenter(inputVolumeBBCentre);

					m_VolumeBounds[0] = inputVolumeBBCentre[0] - (m_MaxBoundX/2);
					m_VolumeBounds[1] = inputVolumeBBCentre[0] + (m_MaxBoundX/2);
					m_VolumeBounds[2] = inputVolumeBBCentre[1] - (m_MaxBoundY/2);
					m_VolumeBounds[3] = inputVolumeBBCentre[1] + (m_MaxBoundY/2);
					m_VolumeBounds[4] = inputVolumeBBCentre[2] - (m_MaxBoundZ/2);
					m_VolumeBounds[5] = inputVolumeBBCentre[2] + (m_MaxBoundZ/2);
					m_GizmoROI->SetBounds(m_VolumeBounds);

					mafEventMacro(mafEvent(this, CAMERA_UPDATE));
				}
				break;
      case ID_VOLUME_SPACING:
      break;
      case ID_VOLUME_VMEBOUNDS:
        SetBoundsToVMEBounds();
        //UpdateGizmoData();
        UpdateGui();
        mafEventMacro(mafEvent(this, CAMERA_UPDATE));
      break;
      case ID_VOLUME_4DBOUNDS:
        SetBoundsToVME4DBounds();
        //UpdateGizmoData();
        UpdateGui();
        mafEventMacro(mafEvent(this, CAMERA_UPDATE));
      break;
      case ID_VOLUME_VMELOCALBOUNDS:
        SetBoundsToVMELocalBounds();
        //UpdateGizmoData();
        UpdateGui();
        mafEventMacro(mafEvent(this, CAMERA_UPDATE));
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
      mafEventMacro(mafEvent(this, CAMERA_UPDATE));

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

					mafEventMacro(mafEvent(this,CAMERA_UPDATE));
				}
				break;
      default:
        mafEventMacro(*e);
      break;
    }	
  }
}
//----------------------------------------------------------------------------
bool medOpVolumeResample::CheckSpacing()
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
void medOpVolumeResample::OnEventGizmoTranslate(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	switch(maf_event->GetId())
	{
	case ID_TRANSFORM:
		{    
			// post multiplying matrixes coming from the gizmo to the vme
			// gizmo does not set vme pose  since they cannot scale
			PostMultiplyEventMatrix(maf_event);
		}
		break;

	default:
		{
			mafEventMacro(*maf_event);
		}
	}
}
//----------------------------------------------------------------------------
void medOpVolumeResample::OnEventGizmoRotate(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	switch(maf_event->GetId())
	{
	case ID_TRANSFORM:
		{    
			// post multiplying matrixes coming from the gizmo to the vme
			// gizmo does not set vme pose  since they cannot scale
			PostMultiplyEventMatrix(maf_event);
		}
		break;

	default:
		{
			mafEventMacro(*maf_event);
		}
	}
}
//----------------------------------------------------------------------------
void medOpVolumeResample::PostMultiplyEventMatrix(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		// handle incoming transform events
		mafSmartPointer<mafMatrix> newAbsMatr;
		if(e->GetSender()==m_GizmoTranslate)
			newAbsMatr->DeepCopy(m_GizmoTranslate->GetAbsPose()->GetVTKMatrix());
		else if(e->GetSender()==m_GizmoRotate)
			newAbsMatr->DeepCopy(m_GizmoRotate->GetAbsPose()->GetVTKMatrix());

		newAbsMatr->SetTimeStamp(0.0);

		if(e->GetSender()==m_GizmoTranslate)//translate
		{

			mafSmartPointer<mafTransformFrame> mflTr;
			mflTr->SetInput(newAbsMatr);
			mflTr->SetTargetFrame(m_CenterVolumeRefSysMatrix);
			mflTr->Update();

			mafTransform::GetPosition(mflTr->GetMatrix(),m_ROIPosition);

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
			mafSmartPointer<mafTransformFrame> mflTr;
			mflTr->SetInput(newAbsMatr);
			mflTr->SetTargetFrame(m_CenterVolumeRefSysMatrix);
			mflTr->Update();
			mafTransform::GetOrientation(mflTr->GetMatrix(),m_ROIOrientation);
			ShiftCenterResampled();
		}
		m_Gui->Update();

		vtkMAFSmartPointer<vtkTransform> tran_bound_box;
		tran_bound_box->PostMultiply();
		tran_bound_box->SetMatrix(m_VMEDummy->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
		tran_bound_box->Concatenate(e->GetMatrix()->GetVTKMatrix());
		tran_bound_box->Update();

		mafSmartPointer<mafMatrix> newAbsMatrBox;
		newAbsMatrBox->DeepCopy(tran_bound_box->GetMatrix());
		newAbsMatrBox->SetTimeStamp(0.0);

		m_VMEDummy->SetAbsMatrix(*newAbsMatrBox);

		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
	}
}
//----------------------------------------------------------------------------
void medOpVolumeResample::SetSpacing(double spacing[3]) 
//----------------------------------------------------------------------------
{
	m_VolumeSpacing[0] = spacing[0];
	m_VolumeSpacing[1] = spacing[1];
	m_VolumeSpacing[2] = spacing[2];
}
//----------------------------------------------------------------------------
void medOpVolumeResample::GetSpacing( double spacing[3] )
//----------------------------------------------------------------------------
{
  spacing[0] = m_VolumeSpacing[0];
  spacing[1] = m_VolumeSpacing[1];
  spacing[2] = m_VolumeSpacing[2];
}
//----------------------------------------------------------------------------
void medOpVolumeResample::SetBounds(double bounds[6],int type) 
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

void medOpVolumeResample::SetROIOrientation(double ROIOrientation[3]) 
{
  m_ROIOrientation[0] = ROIOrientation[0];
  m_ROIOrientation[1] = ROIOrientation[1];
  m_ROIOrientation[2] = ROIOrientation[2];
}

void medOpVolumeResample::SetVolumePosition(double  volumePosition[3]) 
{
  m_VolumePosition[0] = volumePosition[0];
  m_VolumePosition[1] = volumePosition[1];
  m_VolumePosition[2] = volumePosition[2];
}

//----------------------------------------------------------------------------
void medOpVolumeResample::ShiftCenterResampled() 
//----------------------------------------------------------------------------
{

	double inputVolumeLocalVTKBBCenter[3];

	((mafVME *)m_Input)->GetOutput()->GetVTKData()->GetCenter(inputVolumeLocalVTKBBCenter);

	vtkMAFSmartPointer<vtkPoints> points;
	points->InsertNextPoint(inputVolumeLocalVTKBBCenter);

	vtkMAFSmartPointer<vtkPolyData> poly;
	poly->SetPoints(points);
	poly->Update();

	vtkMAFSmartPointer<vtkTransform> t;
	t->RotateX(m_ROIOrientation[0]);
	t->RotateY(m_ROIOrientation[1]);
	t->RotateZ(m_ROIOrientation[2]);
	t->Update();

	vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
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
void medOpVolumeResample::OpStop(int result)
//----------------------------------------------------------------------------
{
	if (!m_TestMode)
	{
		HideGui();
	}
	mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
void medOpVolumeResample::PrintSelf(ostream& os)
//----------------------------------------------------------------------------
{
  os << "-------------------------------------------------------" << std::endl;
  os << "medOpVolumeResample PrintSelf:" << std::endl;

  PrintVolume(os, m_Input, "Input Volume");
  
  mafString parameter;
  
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


void medOpVolumeResample::PrintDouble6( ostream& os, double array[6], const char *logMessage /*= NULL */ )
{

  if (logMessage) os << logMessage << std::endl;
  os << "xmin, xmax [" << array[0] << " , " << array[1] << "]" << std::endl;
  os << "ymin, ymax [" << array[2] << " , " << array[3] << "]" << std::endl;
  os << "zmin, zmax [" << array[4] << " , " << array[5] << "]" << std::endl;
  os << std::endl;
}

void medOpVolumeResample::PrintDouble3( ostream& os, double array[3], const char *logMessage /*= NULL*/ )
{
  if (logMessage) os << logMessage << " [" << array[0] << " , " << array[1] << " , " << array[2] << " ]" << std::endl;
  os << std::endl;
}


void medOpVolumeResample::PrintInt3( ostream& os, int array[3], const char *logMessage /*= NULL*/ )
{
  if (logMessage) os << logMessage << " [" << array[0] << " , " << array[1] << " , " << array[2] << " ]" << std::endl;
  os << std::endl;
}

void medOpVolumeResample::PrintVolume( ostream& os , mafNode *volume , const char *logMessage /*= NULL*/ )
{
  mafVMEVolumeGray *input = mafVMEVolumeGray::SafeDownCast(volume);
  input->GetOutput()->GetVTKData()->Update();
  vtkDataSet *inputDataSet = input->GetOutput()->GetVTKData();
  if (logMessage) os << logMessage << std::endl;
  os << "data is: ";
  if (inputDataSet->IsA("vtkStructuredPoints"))
  {
    os << "vtkStructuredPoints" << std::endl;
    vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(inputDataSet);
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

void medOpVolumeResample::SetNewVolumePosition( double newVolumePosition[3] )
{
  m_NewVolumePosition[0] = newVolumePosition[0];
  m_NewVolumePosition[1] = newVolumePosition[1];
  m_NewVolumePosition[2] = newVolumePosition[2];
}

void medOpVolumeResample::SetMaxBounds( double maxBound[3] )
{
  m_MaxBoundX = maxBound[0];
  m_MaxBoundY = maxBound[1];
  m_MaxBoundZ = maxBound[2];
}

void medOpVolumeResample::SetROIPosition( double roiPosition[3] )
{
  m_ROIPosition[0] = roiPosition[0];
  m_ROIPosition[1] = roiPosition[1];
  m_ROIPosition[2] = roiPosition[2];

}

