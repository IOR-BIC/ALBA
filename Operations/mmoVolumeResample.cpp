/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoVolumeResample.cpp,v $
  Language:  C++
  Date:      $Date: 2006-09-22 10:11:57 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
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


#include "mmoVolumeResample.h"

#include "mafEvent.h"
#include "mmgGui.h"
#include "mmaMaterial.h"

#include "mafVME.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEGizmo.h"
#include "mafVMEItemVTK.h"
#include "mafTagArray.h"
#include "mafDataVector.h"

#include "mafTransform.h"
#include "mafTransformFrame.h"

#include "vtkMAFSmartPointer.h"
#include "vtkVolumeResample.h"
#include "vtkOutlineSource.h"
#include "vtkProperty.h"
#include "vtkPolyData.h"
#include "vtkStructuredPoints.h"
#include "vtkRectilinearGrid.h"
#include "vtkPointData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoVolumeResample);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoVolumeResample::mmoVolumeResample(wxString label) : mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
  m_Canundo	= true;

  m_ResampledVme = NULL;
	
  // initialize Crop OBB parameters
  m_VolumePosition[0]    = m_VolumePosition[1]    = m_VolumePosition[2]    = 0;
  m_VolumeOrientation[0] = m_VolumeOrientation[1] = m_VolumeOrientation[2] = 0;

  m_VolumeBounds[0] = m_VolumeBounds[1] = m_VolumeBounds[2] = \
  m_VolumeBounds[3] = m_VolumeBounds[4] = m_VolumeBounds[5] = 0;

  m_VolumeSpacing[0] = m_VolumeSpacing[1] = m_VolumeSpacing[2] = 1;
  
	m_ResampleBox      = NULL;
	m_ResampleBoxVme  = NULL;

  m_ZeroPadValue = 0;
}
//----------------------------------------------------------------------------
mmoVolumeResample::~mmoVolumeResample()
//----------------------------------------------------------------------------
{
	mafDEL(m_ResampledVme);
	mafDEL(m_ResampleBoxVme);
	vtkDEL(m_ResampleBox);
}
//----------------------------------------------------------------------------
bool mmoVolumeResample::Accept(mafNode* vme) 
//----------------------------------------------------------------------------
{
  return (vme && vme->IsMAFType(mafVMEVolumeGray));
}
//----------------------------------------------------------------------------
void mmoVolumeResample::InternalUpdateBounds(double bounds[6], bool center)
//----------------------------------------------------------------------------
{
  if (center)
  {
    m_VolumePosition[0] = (bounds[1] + bounds[0]) / 2.0;
    m_VolumePosition[1] = (bounds[3] + bounds[2]) / 2.0;
    m_VolumePosition[2] = (bounds[5] + bounds[4]) / 2.0;

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
    m_VolumePosition[0] = 0;
    m_VolumePosition[1] = 0;
    m_VolumePosition[2] = 0;

    for(int i = 0; i < 6; i++)
    {
      m_VolumeBounds[i] = bounds[i];
    }
  }  
}
//----------------------------------------------------------------------------
void mmoVolumeResample::AutoSpacing()
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
      if (m_VolumeSpacing[0] > spcx)
        m_VolumeSpacing[0] = spcx;
    }
    
    for (int yi = 1; yi < rgrid->GetYCoordinates()->GetNumberOfTuples (); yi++)
    {
      double spcy = rgrid->GetYCoordinates()->GetTuple1(yi)-rgrid->GetYCoordinates()->GetTuple1(yi-1);
      if (m_VolumeSpacing[1] > spcy)
        m_VolumeSpacing[1] = spcy;
    }

    for (int zi = 1; zi < rgrid->GetZCoordinates()->GetNumberOfTuples (); zi++)
    {
      double spcz = rgrid->GetZCoordinates()->GetTuple1(zi)-rgrid->GetZCoordinates()->GetTuple1(zi-1);
      if (m_VolumeSpacing[2] > spcz)
        m_VolumeSpacing[2] = spcz;
    }
  }

  // project spacing on new axes.
  // Note: TransformVector ignores the translation column!
  mafSmartPointer<mafTransformFrame> input_to_output;
  input_to_output->SetInputFrame(m_ResampleBoxVme->GetOutput()->GetAbsMatrix());
  input_to_output->SetTargetFrame(((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());
  input_to_output->Update();
  input_to_output->TransformPoint(m_VolumeSpacing,m_VolumeSpacing);

  for(int i = 0; i < 3; i++)
  {
    m_VolumeSpacing[i] = fabs(m_VolumeSpacing[i]);
  }
}
//----------------------------------------------------------------------------
void mmoVolumeResample::UpdateGui()
//----------------------------------------------------------------------------
{
	m_ResampleBox->GetBounds(m_VolumeBounds);
	m_Gui->Update();  
}
//----------------------------------------------------------------------------
void mmoVolumeResample::UpdateGizmoData()
//----------------------------------------------------------------------------
{
  m_ResampleBox->SetBounds(m_VolumeBounds[0], m_VolumeBounds[1], \
                  m_VolumeBounds[2], m_VolumeBounds[3], \
                  m_VolumeBounds[4], m_VolumeBounds[5]);

  m_ResampleBoxVme->SetAbsPose(m_VolumePosition[0],m_VolumePosition[1],m_VolumePosition[2], \
                       m_VolumeOrientation[0],m_VolumeOrientation[1],m_VolumeOrientation[2]);
}
//----------------------------------------------------------------------------
void mmoVolumeResample::CreateGizmoCube()
//----------------------------------------------------------------------------
{
  m_ResampleBox = vtkOutlineSource::New();

  // set up a default resampling volume aligned according to local VME axes
	//SetBoundsToVMEBounds();
  SetBoundsToVMELocalBounds();

  mafNEW(m_ResampleBoxVme);
  m_ResampleBoxVme->SetName("GizmoCube");
  m_ResampleBoxVme->SetData(m_ResampleBox->GetOutput());
  m_ResampleBoxVme->GetMaterial()->m_Diffuse[0] = 1;
  m_ResampleBoxVme->GetMaterial()->m_Diffuse[1] = 0;
  m_ResampleBoxVme->GetMaterial()->m_Diffuse[2] = 0;
  m_ResampleBoxVme->GetMaterial()->UpdateProp();
  m_ResampleBoxVme->ReparentTo((mafVME *)m_Input->GetRoot());
  
  UpdateGizmoData();
  AutoSpacing();
}

//----------------------------------------------------------------------------
void mmoVolumeResample::SetBoundsToVMEBounds()
//----------------------------------------------------------------------------
{
  double bounds[6];
  ((mafVME *)m_Input)->GetOutput()->GetVMEBounds(bounds);

  InternalUpdateBounds(bounds,true);
  m_VolumeOrientation[0] = m_VolumeOrientation[1] = m_VolumeOrientation[2] = 0;
}
//----------------------------------------------------------------------------
void mmoVolumeResample::SetBoundsToVME4DBounds()
//----------------------------------------------------------------------------
{
  double bounds[6];
  ((mafVME *)m_Input)->GetOutput()->GetVME4DBounds(bounds);

  InternalUpdateBounds(bounds,true);
  m_VolumeOrientation[0]=m_VolumeOrientation[1]=m_VolumeOrientation[2]=0;
}
//----------------------------------------------------------------------------
void mmoVolumeResample::SetBoundsToVMELocalBounds()
//----------------------------------------------------------------------------
{
  double bounds[6];
  ((mafVME *)m_Input)->GetOutput()->GetVMELocalBounds(bounds);

  InternalUpdateBounds(bounds,false);
  ((mafVME *)m_Input)->GetOutput()->GetAbsPose(m_VolumePosition,m_VolumeOrientation);
}
//----------------------------------------------------------------------------
void mmoVolumeResample::GizmoDelete()
//----------------------------------------------------------------------------
{	
	//add the gizmo to the views
	mafEventMacro(mafEvent(this, VME_REMOVE, m_ResampleBoxVme));
  vtkDEL(m_ResampleBox);
  mafDEL(m_ResampleBoxVme);
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
mafOp *mmoVolumeResample::Copy()
//----------------------------------------------------------------------------
{
	return new mmoVolumeResample(m_Label);
}
//----------------------------------------------------------------------------
void mmoVolumeResample::OpRun()   
//----------------------------------------------------------------------------
{
  // extract information from input data
	CreateGizmoCube();
	if(!this->m_TestMode)
		CreateGui();
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mmoVolumeResample::OpDo()
//----------------------------------------------------------------------------
{
	m_ResampledVme->ReparentTo(m_Input);
}
//----------------------------------------------------------------------------
void mmoVolumeResample::Resample()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafTransform> box_pose;
  box_pose->SetOrientation(m_VolumeOrientation);
  box_pose->SetPosition(m_VolumePosition);

  mafSmartPointer<mafTransformFrame> local_pose;
  local_pose->SetInput(box_pose);
  
  mafSmartPointer<mafTransformFrame> output_to_input;
  
  // In a future version if not a "Natural" data the filter should operate in place.
	mafString new_vme_name = "resampled_";
	new_vme_name += m_Input->GetName();

	mafNEW(m_ResampledVme);
	m_ResampledVme->SetName(new_vme_name);

	m_Input->Modified();
	mafVME *Node = mafVME::SafeDownCast(m_Input);
  /*m_ResampledVme = (mafVMEVolumeGray *)m_Input->NewInstance();
  m_ResampledVme->Register(m_ResampledVme);
  m_ResampledVme->GetTagArray()->DeepCopy(m_Input->GetTagArray()); // copy tags
  m_ResampledVme->SetName(new_vme_name);

  m_ResampledVme->ReparentTo(m_Input->GetParent());
  m_ResampledVme->SetMatrix(box_pose->GetMatrix());*/

  int output_extent[6];
  output_extent[0] = 0;
  output_extent[1] = (m_VolumeBounds[1] - m_VolumeBounds[0]) / m_VolumeSpacing[0];
  output_extent[2] = 0;
  output_extent[3] = (m_VolumeBounds[3] - m_VolumeBounds[2]) / m_VolumeSpacing[1];
  output_extent[4] = 0;
  output_extent[5] = (m_VolumeBounds[5] - m_VolumeBounds[4]) / m_VolumeSpacing[2];

  double w,l,sr[2];
  for (int i = 0; i < ((mafVMEGenericAbstract *)m_Input)->GetDataVector()->GetNumberOfItems(); i++)
  {
    if (mafVMEItemVTK *input_item = mafVMEItemVTK::SafeDownCast(((mafVMEGenericAbstract *)m_Input)->GetDataVector()->GetItemByIndex(i)))
    {
      if (vtkDataSet *input_data = input_item->GetData())
      {
        // the resample filter
        vtkMAFSmartPointer<vtkVolumeResample> resampler;
        resampler->SetZeroValue(m_ZeroPadValue);

        // Set the target be vme's parent frame. And Input frame to the root. I've to 
        // set at each iteration since I'm using the SetMatrix, which doesn't support
        // transform pipelines.
        mafSmartPointer<mafMatrix> output_parent_abs_pose;
        Node->GetParent()->GetOutput()->GetAbsMatrix(*output_parent_abs_pose.GetPointer(),input_item->GetTimeStamp());
        local_pose->SetInputFrame(output_parent_abs_pose);

        mafSmartPointer<mafMatrix> input_parent_abs_pose;
        ((mafVME *)m_Input->GetParent())->GetOutput()->GetAbsMatrix(*input_parent_abs_pose.GetPointer(),input_item->GetTimeStamp());
        local_pose->SetTargetFrame(input_parent_abs_pose);
        local_pose->Update();

        mafSmartPointer<mafMatrix> output_abs_pose;
        Node->GetOutput()->GetAbsMatrix(*output_abs_pose.GetPointer(),input_item->GetTimeStamp());
        output_to_input->SetInputFrame(output_abs_pose);

        mafSmartPointer<mafMatrix> input_abs_pose;
        ((mafVME *)m_Input)->GetOutput()->GetAbsMatrix(*input_abs_pose.GetPointer(),input_item->GetTimeStamp());
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
        output_data->SetSpacing(m_VolumeSpacing);
        // TODO: here I probably should allow a data type casting... i.e. a GUI widget
        output_data->SetScalarType(input_data->GetPointData()->GetScalars()->GetDataType());
        output_data->SetExtent(output_extent);
        output_data->SetUpdateExtent(output_extent);
        
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

        m_ResampledVme->SetDataByDetaching(output_data, input_item->GetTimeStamp());
        m_ResampledVme->Update();
      }
    }
  }
	m_ResampledVme->ReparentTo(m_Input); //Re-parenting a VME implies that it is also added to the tree.
  m_Output = m_ResampledVme; // Used to make the UnDo: if the output var is set, the undo is done by default.
}
//----------------------------------------------------------------------------
void mmoVolumeResample::OpUndo()
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
  ID_VOLUME_ZERO_VALUE
};

enum BOUNDS
{
	ID_VME4DBOUNDS = 0,
	ID_VMELOCALBOUNDS,
	ID_VMEBOUNDS,
	ID_PERSONALBOUNDS,
};

//----------------------------------------------------------------------------
void mmoVolumeResample::CreateGui() 
//----------------------------------------------------------------------------
{
	m_Gui = new mmgGui(this);

  m_Gui->Button(ID_VOLUME_VMELOCALBOUNDS,"VME Local Bounds","","set the crop bounding box to the oriented VME bounds (default option)");
  m_Gui->Button(ID_VOLUME_VMEBOUNDS,"VME Global Bounds","","set the crop bounding box to the VME global bounds");
  m_Gui->Button(ID_VOLUME_4DBOUNDS,"VME 4D Bounds","","set the crop bounding box to the current VME 4D bounds");
  m_Gui->Label("");
	m_Gui->Label("Resample Bounding Box Extent",true);
	m_Gui->VectorN(ID_VOLUME_DIR_X, "X", &m_VolumeBounds[0], 2);
	m_Gui->VectorN(ID_VOLUME_DIR_Y, "Y", &m_VolumeBounds[2], 2);
	m_Gui->VectorN(ID_VOLUME_DIR_Z, "Z", &m_VolumeBounds[4], 2);

  m_Gui->Label("Resample Volume Parameters",true);
  m_Gui->Label("Bouding Box Origin",false);
  m_Gui->Vector(ID_VOLUME_ORIGIN, "", m_VolumePosition,MINFLOAT,MAXFLOAT,2,"output volume origin");

  m_Gui->Label("Bouding Box Orientation",false);
  m_Gui->Vector(ID_VOLUME_ORIENTATION, "", m_VolumeOrientation,MINFLOAT,MAXFLOAT,2,"output volume orientation");
	
  m_Gui->Label("");

  m_Gui->Label("Volume Spacing",false);
  m_Gui->Vector(ID_VOLUME_SPACING, "", this->m_VolumeSpacing,MINFLOAT,MAXFLOAT,4,"output volume spacing");
  m_Gui->Button(ID_VOLUME_AUTOSPACING,"AutoSpacing","","compute auto spacing by rotating original spacing");

  UpdateGizmoData();

  m_Gui->Label("");
  
  double range[2];
  wxString str_range;
  ((mafVME *)m_Input)->GetOutput()->GetVTKData()->GetScalarRange(range);
  str_range.Printf("[ %.3f , %.3f ]",range[0],range[1]);
  
  m_Gui->Label("Scalar Range:");
  m_Gui->Label(str_range);

  m_Gui->Label("");
  m_Gui->Label("Padding Value");
  m_Gui->Double(ID_VOLUME_ZERO_VALUE,"",&m_ZeroPadValue);

	m_Gui->Label("");
	m_Gui->OkCancel();
	ShowGui();
}

//----------------------------------------------------------------------------
void mmoVolumeResample::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_VOLUME_ORIGIN:
      case ID_VOLUME_ORIENTATION:
      case ID_VOLUME_DIR_X:		
      case ID_VOLUME_DIR_Y:
      case ID_VOLUME_DIR_Z:
        UpdateGizmoData();
        mafEventMacro(mafEvent(this, CAMERA_UPDATE));
      break;
      case ID_VOLUME_SPACING:
      break;
      case ID_VOLUME_VMEBOUNDS:
        SetBoundsToVMEBounds();
        UpdateGizmoData();
        UpdateGui();
        mafEventMacro(mafEvent(this, CAMERA_UPDATE));
      break;
      case ID_VOLUME_4DBOUNDS:
        SetBoundsToVME4DBounds();
        UpdateGizmoData();
        UpdateGui();
        mafEventMacro(mafEvent(this, CAMERA_UPDATE));
      break;
      case ID_VOLUME_VMELOCALBOUNDS:
        SetBoundsToVMELocalBounds();
        UpdateGizmoData();
        UpdateGui();
        mafEventMacro(mafEvent(this, CAMERA_UPDATE));
      break;
      case ID_VOLUME_AUTOSPACING:
        AutoSpacing();
        UpdateGui();
      break;
      case wxOK:
				Resample();
        GizmoDelete();
        HideGui();
        mafEventMacro(mafEvent(this,OP_RUN_OK));
      break;
      case wxCANCEL:
        GizmoDelete();
        HideGui();
        mafEventMacro(mafEvent(this,OP_RUN_CANCEL));		
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
      default:
        mafEventMacro(*e);
      break;
    }	
  }
}
//----------------------------------------------------------------------------
void mmoVolumeResample::SetSpacing(double Spacing[3]) 
//----------------------------------------------------------------------------
{
	m_VolumeSpacing[0] = Spacing[0];
	m_VolumeSpacing[1] = Spacing[1];
	m_VolumeSpacing[2] = Spacing[2];
}
//----------------------------------------------------------------------------
void mmoVolumeResample::SetBounds(double Bounds[6],int Type) 
//----------------------------------------------------------------------------
{
	switch (Type)
	{
	case ID_VME4DBOUNDS:
		SetBoundsToVME4DBounds();
		break;
	case ID_VMELOCALBOUNDS:
		SetBoundsToVMELocalBounds();
		break;
	case ID_VMEBOUNDS:
		this->SetBoundsToVMEBounds();
		break;
	case ID_PERSONALBOUNDS:
		m_VolumeBounds[0] = Bounds[0];
		m_VolumeBounds[1] = Bounds[1];
		m_VolumeBounds[2] = Bounds[2];
		m_VolumeBounds[3] = Bounds[3];
		m_VolumeBounds[4] = Bounds[4];
		m_VolumeBounds[5] = Bounds[5];
		break;
	}
}