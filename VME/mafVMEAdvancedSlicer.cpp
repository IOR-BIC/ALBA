/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEAdvancedSlicer.cpp,v $
  Language:  C++
  Date:      $Date: 2007-02-06 15:52:34 $
  Version:   $Revision: 1.9 $
  Authors:   Daniele Giunchi , Matteo Giacomoni
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMEAdvancedSlicer.h"
#include "mafTransform.h"
#include "mafStorageElement.h"
#include "mafIndent.h"
#include "mafDataPipeCustom.h"
#include "mafVMEVolume.h"
#include "mafVMEOutputSurface.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "mmgGui.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFDataPipe.h"
#include "vtkMath.h"
#include "vtkImageData.h"
#include "vtkPolyData.h"
#include "vtkVolumeSlicer.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkLinearTransform.h"
#include "vtkPointData.h"
#include "vtkPlaneSource.h"
#include "vtkLookupTable.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkLandmarkTransform.h"
#include "vtkPoints.h"

#define VALUE 1.5

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEAdvancedSlicer)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEAdvancedSlicer::mafVMEAdvancedSlicer()
//-------------------------------------------------------------------------
{
  mafNEW(m_Transform);
  mafVMEOutputSurface *output=mafVMEOutputSurface::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  m_TextureRes = 256;
  m_Xspc = m_Yspc = 0.3;

  vtkMAFSmartPointer<vtkImageData> image;
  image->SetExtent(0, m_TextureRes - 1, 0, m_TextureRes - 1, 0, 0);
  image->SetUpdateExtent(0, m_TextureRes - 1, 0, m_TextureRes - 1, 0, 0);
  image->SetSpacing(m_Xspc, m_Yspc, 1.f);

  vtkMAFSmartPointer<vtkPolyData> slice;

  vtkNEW(m_PSlicer);
  vtkNEW(m_ISlicer);
	vtkNEW(m_Plane);
  m_PSlicer->SetOutput(slice);
  m_PSlicer->SetTexture(image);
  m_ISlicer->SetOutput(image);

	m_Height = 50;
	m_Width = 50;
  
  vtkNEW(m_BackTransform);
  //vtkMAFSmartPointer<vtkTransformPolyDataFilter> back_trans;
  m_BackTransform->SetInput(m_Plane->GetOutput());
  m_BackTransform->SetTransform(m_Transform->GetVTKTransform()->GetInverse());

  // attach a datapipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);

  dpipe->SetInput(m_BackTransform->GetOutput());
  dpipe->SetNthInput(1,image);

  // set the texture in the output, must do it here, after setting slicer filter's input
  GetSurfaceOutput()->SetTexture((vtkImageData *)((mafDataPipeCustom *)GetDataPipe())->GetVTKDataPipe()->GetOutput(1));
  GetMaterial()->SetMaterialTexture(GetSurfaceOutput()->GetTexture());
  GetMaterial()->m_MaterialType = mmaMaterial::USE_TEXTURE;
  GetMaterial()->m_TextureMappingMode = mmaMaterial::PLANE_MAPPING;

	m_VMEAccept = new mafVMEAccept();

	vtkNEW(m_Texture);
	vtkNEW(m_Lut);

	m_Normal[0] = 0.0;
	m_Normal[1] = 0.0;
	m_Normal[2] = 1.0;

	m_Pos[0] = 50.0;
	m_Pos[1] = 50.0;
	m_Pos[2] = 50.0;
}

//-------------------------------------------------------------------------
mafVMEAdvancedSlicer::~mafVMEAdvancedSlicer()
//-------------------------------------------------------------------------
{
  vtkDEL(m_BackTransform);
  mafDEL(m_Transform);
  SetOutput(NULL);

  vtkDEL(m_PSlicer);
  vtkDEL(m_ISlicer);
	vtkDEL(m_Lut);
	vtkDEL(m_Texture);
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMEAdvancedSlicer::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
  }
  return material;
}

//-------------------------------------------------------------------------
int mafVMEAdvancedSlicer::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMEAdvancedSlicer *vme_slicer=mafVMEAdvancedSlicer::SafeDownCast(a);
    m_Transform->SetMatrix(vme_slicer->m_Transform->GetMatrix());
    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetDependOnAbsPose(true);
      dpipe->SetInput(m_BackTransform->GetOutput());
      dpipe->SetNthInput(1,m_PSlicer->GetTexture());
    }
    GetMaterial()->SetMaterialTexture(m_PSlicer->GetTexture());
    return MAF_OK;
  }  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMEAdvancedSlicer::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    return m_Transform->GetMatrix()==((mafVMEAdvancedSlicer *)vme)->m_Transform->GetMatrix();
  }
  return false;
}


//-------------------------------------------------------------------------
mafVMEOutputSurface *mafVMEAdvancedSlicer::GetSurfaceOutput()
//-------------------------------------------------------------------------
{
  return (mafVMEOutputSurface *)GetOutput();
}
//-------------------------------------------------------------------------
void mafVMEAdvancedSlicer::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
bool mafVMEAdvancedSlicer::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
bool mafVMEAdvancedSlicer::IsDataAvailable()
//-------------------------------------------------------------------------
{
	if(GetVolumeLink())
		return ((mafVME*)GetVolumeLink())->IsDataAvailable();
	else if(GetParent())
		return ((mafVME*)GetParent())->IsDataAvailable();
	else
		return false;
}

//-------------------------------------------------------------------------
void mafVMEAdvancedSlicer::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}
//-----------------------------------------------------------------------
void mafVMEAdvancedSlicer::SetNormal(float normal[3])
//-----------------------------------------------------------------------
{
	m_Normal[0] = normal[0];
	m_Normal[1] = normal[1];
	m_Normal[2] = normal[2];

	Modified();
}
//-----------------------------------------------------------------------
void mafVMEAdvancedSlicer::InternalPreUpdate()
//-----------------------------------------------------------------------
{
	mafVMEVolume *vol=mafVMEVolume::SafeDownCast(GetVolumeLink()) ? mafVMEVolume::SafeDownCast(GetVolumeLink()) : mafVMEVolume::SafeDownCast(GetParent());
  if(vol)
  {
    if (vtkDataSet *vtkdata=vol->GetOutput()->GetVTKData())
    {

			vtkdata->Update();

			vtkImageData *image=vtkImageData::SafeDownCast(vtkdata);

			double spacing[3];
			image->GetSpacing(spacing);
			int x0=(int)(m_Pos[0]/spacing[0] - (m_Width / (2*spacing[0])));
			int x1=(int)(m_Pos[0]/spacing[0] + (m_Width / (2*spacing[0])));
			int y0=(int)(m_Pos[1]/spacing[1] - (m_Height / (2*spacing[1])));
			int y1=(int)(m_Pos[1]/spacing[1] + (m_Height / (2*spacing[1])));
			image->SetOrigin(m_Pos[0] - m_Width/2,m_Pos[1] - m_Height/2,m_Pos[2]);
			image->SetExtent(x0,x1,y0,y1,0,200);
			image->SetUpdateExtent(x0,x1,y0,y1,0,200);

			image->Crop();
			image->UpdateData();

			vtkImageData *texture = m_PSlicer->GetTexture();
			texture->SetScalarType(image->GetPointData()->GetScalars()->GetDataType());
			texture->SetNumberOfScalarComponents(image->GetPointData()->GetScalars()->GetNumberOfComponents());
			texture->SetExtent(0, m_Width/m_Xspc - 1, 0, m_Height/m_Yspc - 1, 0, 0);
			texture->SetUpdateExtent(0, m_Width/m_Xspc - 1, 0, m_Height/m_Yspc - 1, 0, 0);
			texture->SetSpacing(m_Xspc, m_Yspc, 1.f);
			texture->Modified();

			vtkMAFSmartPointer<vtkPoints> source_points;
			source_points->SetNumberOfPoints(2);
			source_points->SetPoint(0,0,0,0);
			source_points->SetPoint(1,1,0,0);

			vtkMAFSmartPointer<vtkPoints> target_points;
			target_points->SetNumberOfPoints(2);
			target_points->SetPoint(0,m_Pos);
			vtkMath::Normalize(m_Normal);
			double p2[3];
			p2[0]=m_Pos[0] + m_Normal[0];
			p2[1]=m_Pos[1] + m_Normal[1];
			p2[2]=m_Pos[2] + m_Normal[2];
			target_points->SetPoint(1,p2);

			vtkMAFSmartPointer<vtkLandmarkTransform> trans_matrix;
			trans_matrix->SetSourceLandmarks(source_points.GetPointer());
			trans_matrix->SetTargetLandmarks(target_points.GetPointer());
			trans_matrix->SetModeToRigidBody();
			trans_matrix->Update();

			vtkMAFSmartPointer<vtkTransform> trans;
			trans->SetMatrix(trans_matrix->GetMatrix());

			vtkMAFSmartPointer<vtkTransformPolyDataFilter> trans_poly;
			trans_poly->SetTransform(trans);


			//+++ create plane
			vtkNEW(m_Plane);
			m_Plane->SetOrigin(0,m_Width/2.0,m_Height/2.0);
			m_Plane->SetPoint1(0,-m_Width/2.0,m_Height/2.0);
			m_Plane->SetPoint2(0,m_Width/2.0,-m_Height/2.0);
			m_Plane->SetXResolution((int)(m_Width/(VALUE*m_Xspc)));
			m_Plane->SetYResolution((int)(m_Height/(VALUE*m_Yspc)));
			m_Plane->Update();

			trans_poly->SetInput(m_Plane->GetOutput());
			trans_poly->Update();


			double vectX[3],vectY[3],o[3];
			m_Plane->GetOrigin(o);
			m_Plane->GetPoint1(vectX);
			m_Plane->GetPoint2(vectY);

			vectX[0]-=o[0];
			vectX[1]-=o[1];
			vectX[2]-=o[2];
			vectY[0]-=o[0];
			vectY[1]-=o[1];
			vectY[2]-=o[2];

			vtkMath::Normalize(vectX);
			vtkMath::Normalize(vectY);

			float fvectX[3],fvectY[3];
			fvectX[0]=(float)vectX[0];
			fvectX[1]=(float)vectX[1];
			fvectX[2]=(float)vectX[2];
			fvectY[0]=(float)vectY[0];
			fvectY[1]=(float)vectY[1];
			fvectY[2]=(float)vectY[2];

      m_PSlicer->SetInput(image);
      m_PSlicer->SetPlaneOrigin(m_Pos);
      m_PSlicer->SetPlaneAxisX(fvectX);
      m_PSlicer->SetPlaneAxisY(fvectY);

      m_ISlicer->SetInput(image);
      m_ISlicer->SetPlaneOrigin(m_Pos);
      m_ISlicer->SetPlaneAxisX(fvectX);
      m_ISlicer->SetPlaneAxisY(fvectY);

			double sr[2];
			vol->GetOutput()->GetVTKData()->GetScalarRange(sr);
			GetMaterial()->m_ColorLut->SetTableRange(sr);
    }
  }
}

//-----------------------------------------------------------------------
void mafVMEAdvancedSlicer::InternalUpdate()
//-----------------------------------------------------------------------
{
  mafVME *vol=mafVMEVolume::SafeDownCast(GetVolumeLink()) ? mafVMEVolume::SafeDownCast(GetVolumeLink()) : mafVMEVolume::SafeDownCast(GetParent());
  if(vol)
  {
    vol->Update();
    if (vtkDataSet *vtkdata=vol->GetOutput()->GetVTKData())
    {		
      m_PSlicer->Update();
      m_ISlicer->Update();
    }
  }
}
//-----------------------------------------------------------------------
void mafVMEAdvancedSlicer::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  mafMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}
//-------------------------------------------------------------------------
mmgGui* mafVMEAdvancedSlicer::CreateGui()
//-------------------------------------------------------------------------
{
	m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
	m_Gui->SetListener(this);
	m_Gui->Divider();
	mafVMEVolume *vol=mafVMEVolume::SafeDownCast(GetVolumeLink()) ? mafVMEVolume::SafeDownCast(GetVolumeLink()) : mafVMEVolume::SafeDownCast(GetParent());
	m_VolumeName = vol ? vol->GetName() : _("none");
	m_Gui->Button(ID_VOLUME_LINK,&m_VolumeName,_("Volume"), _("Select the volume to be probed"));

	m_Gui->Integer(ID_WIDTH,_("Width"),&m_Width);
	m_Gui->Integer(ID_HEIGHT,_("Height"),&m_Height);

	m_Gui->Vector(ID_POSITION,_("Position"),m_Pos);
	m_Gui->Vector(ID_NORMAL,_("Normal"),m_Normal);

	m_Gui->Divider(1);
	return m_Gui;
}
//-----------------------------------------------------------------------
mafNode *mafVMEAdvancedSlicer::GetVolumeLink()
//-----------------------------------------------------------------------
{
	return GetLink("Volume");
}
//-------------------------------------------------------------------------
void mafVMEAdvancedSlicer::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
	// events to be sent up or down in the tree are simply forwarded
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
			case ID_VOLUME_LINK:
				{
					mafID button_id = e->GetId();
					mafString title = _("Choose Volume VME link");
					e->SetId(VME_CHOOSE);
					e->SetArg((long)m_VMEAccept);
					e->SetString(&title);
					ForwardUpEvent(e);
					mafNode *n = e->GetVme();
					if (n != NULL)
					{
						
						SetVolumeLink(n);
						m_VolumeName = n->GetName();
						m_Gui->Update();
						Modified();
					}
				}
				break;
			case ID_HEIGHT:
			case ID_WIDTH:
				{
					mafVME *vol=mafVMEVolume::SafeDownCast(GetVolumeLink()) ? mafVMEVolume::SafeDownCast(GetVolumeLink()) : mafVMEVolume::SafeDownCast(GetParent());

					if (vtkDataSet *vtkdata=vol->GetOutput()->GetVTKData())
					{
						if(vtkdata->IsA("vtkImageData"))
						{
							Modified();
							ForwardUpEvent(mafEvent(this,CAMERA_UPDATE));
						}
					}
				}
				break;
			case ID_POSITION:
			case ID_NORMAL:
				Modified();
				ForwardUpEvent(mafEvent(this,CAMERA_UPDATE));
				m_Gui->Update();
			default:
				mafNode::OnEvent(maf_event);
		}
	}
	else
	{
		Superclass::OnEvent(maf_event);
	}
}
//-----------------------------------------------------------------------
void mafVMEAdvancedSlicer::SetVolumeLink(mafNode *volume)
//-----------------------------------------------------------------------
{
	SetLink("Volume", volume);
	Modified();
}