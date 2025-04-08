/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMESlicer
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

#include "albaVMESlicer.h"
#include "albaGUI.h"
#include "albaGUILutPreset.h"

#include "mmaMaterial.h"

#include "albaTransform.h"
#include "albaStorageElement.h"
#include "albaIndent.h"
#include "albaDataPipeCustom.h"
#include "albaVMEVolume.h"
#include "albaVMEOutputSurface.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBADataPipe.h"
#include "vtkMath.h"
#include "vtkImageData.h"
#include "vtkPolyData.h"
#include "vtkALBAVolumeSlicer.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkLinearTransform.h"
#include "vtkPointData.h"
#include "vtkTransform.h"

#include <assert.h>
#include "albaVMEVolumeGray.h"
#include "mmaVolumeMaterial.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMESlicer)

//-------------------------------------------------------------------------
albaVMESlicer::albaVMESlicer()
{
  m_UpdateVTKPropertiesFromMaterial = true;

  albaNEW(m_Transform);
	albaNEW(m_CopyTransform);
  albaVMEOutputSurface *output=albaVMEOutputSurface::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  m_TextureRes = 512;
  m_Xspc = m_Yspc = 0.3;

  vtkALBASmartPointer<vtkImageData> image;
  image->SetExtent(0, m_TextureRes - 1, 0, m_TextureRes - 1, 0, 0);
  image->SetUpdateExtent(0, m_TextureRes - 1, 0, m_TextureRes - 1, 0, 0);
  image->SetSpacing(m_Xspc, m_Yspc, 1.f);

  vtkALBASmartPointer<vtkPolyData> slice;

  vtkNEW(m_PSlicer);
  vtkNEW(m_ISlicer);
  m_PSlicer->SetOutput(slice);
  m_PSlicer->SetTexture(image);
  m_ISlicer->SetOutput(image);
  
  vtkNEW(m_BackTransform);
  m_BackTransform->SetInput(slice);

  DependsOnLinkedNodeOn();

  // attach a datapipe which creates a bridge between VTK and ALBA
  albaDataPipeCustom *dpipe = albaDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);

  dpipe->SetInput(m_BackTransform->GetOutput());
  dpipe->SetNthInput(1,image);

  // set the texture in the output, must do it here, after setting slicer filter's input
  GetSurfaceOutput()->SetTexture((vtkImageData *)((albaDataPipeCustom *)GetDataPipe())->GetVTKDataPipe()->GetOutput(1));
  GetMaterial()->SetMaterialTexture(GetSurfaceOutput()->GetTexture());
  GetMaterial()->m_MaterialType = mmaMaterial::USE_TEXTURE;
  GetMaterial()->m_TextureMappingMode = mmaMaterial::PLANE_MAPPING;

  m_TrilinearInterpolationOn = m_EnableGPU = true;
}

//-------------------------------------------------------------------------
albaVMESlicer::~albaVMESlicer()
{
  //vtkDEL(m_BackTransformParent);
  vtkDEL(m_CopyTransform);
  vtkDEL(m_BackTransform);
  albaDEL(m_Transform);
  SetOutput(NULL);

  vtkDEL(m_PSlicer);
  vtkDEL(m_ISlicer);
}

//-------------------------------------------------------------------------
mmaMaterial *albaVMESlicer::GetMaterial()
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
		albaVMEVolumeGray *slicedVolume = albaVMEVolumeGray::SafeDownCast(GetSlicedVMELink());
		if (slicedVolume && slicedVolume->GetMaterial())
		{
			mmaVolumeMaterial * volumeMaterial = slicedVolume->GetMaterial();
			material->m_ColorLut->DeepCopy(volumeMaterial->m_ColorLut);
		}
		else
			lutPreset(4,GetMaterial()->m_ColorLut);
  }
  return material;
}

//-------------------------------------------------------------------------
int albaVMESlicer::DeepCopy(albaVME *a)
{ 
  if (Superclass::DeepCopy(a)==ALBA_OK)
  {
    albaVMESlicer *slicer = albaVMESlicer::SafeDownCast(a);
    albaVME *linked_node = slicer->GetSlicedVMELink();
    if (linked_node)
    {
      this->SetMandatoryLink("SlicedVME", linked_node);
    }

    m_Transform->SetMatrix(slicer->m_Transform->GetMatrix());
    albaDataPipeCustom *dpipe = albaDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetDependOnAbsPose(true);
      dpipe->SetInput(m_BackTransform->GetOutput());
      dpipe->SetNthInput(1,m_PSlicer->GetTexture());
    }
    m_SlicedName      = slicer->m_SlicedName;
    GetMaterial()->SetMaterialTexture(m_PSlicer->GetTexture());
    return ALBA_OK;
  }  
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
bool albaVMESlicer::Equals(albaVME *vme)
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    return ret = (m_Transform->GetMatrix()==((albaVMESlicer *)vme)->m_Transform->GetMatrix() &&
                  m_SlicedName      == ((albaVMESlicer *)vme)->m_SlicedName &&
                  GetLink("SlicedVME") == ((albaVMESlicer *)vme)->GetLink("SlicedVME"));
  }
  return ret;
}

//-------------------------------------------------------------------------
albaGUI* albaVMESlicer::CreateGui()
{
  m_Gui = albaVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  albaVME *vol = GetSlicedVMELink();
  m_SlicedName = vol ? vol->GetName() : _("none");
  m_Gui->Button(ID_VOLUME_LINK,&m_SlicedName,_("Volume"), _("Select the volume to be sliced"));

  m_Gui->Divider();
  return m_Gui;
}
//-------------------------------------------------------------------------
void albaVMESlicer::OnEvent(albaEventBase *alba_event)
{
  // events to be sent up or down in the tree are simply forwarded
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case ID_VOLUME_LINK:
      {
        albaString title = _("Choose volume vme");
        e->SetId(VME_CHOOSE);
        e->SetPointer(&albaVMESlicer::VolumeAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        albaVME *n = e->GetVme();
        if (n != NULL)
        {
          SetSlicedVMELink(n);
          m_SlicedName = n->GetName();
          m_Gui->Update();
          e->SetId(CAMERA_UPDATE);
          e->SetArg(0);
          ForwardUpEvent(e);
        }
      }
      break;
      default:
        albaVME::OnEvent(alba_event);
    }
  }
  else
  {
    Superclass::OnEvent(alba_event);
  }
}
//-------------------------------------------------------------------------
albaVMEOutputSurface *albaVMESlicer::GetSurfaceOutput()
{
  return (albaVMEOutputSurface *)GetOutput();
}
//-------------------------------------------------------------------------
void albaVMESlicer::SetMatrix(const albaMatrix &mat)
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
bool albaVMESlicer::IsAnimated()
{
  return false;
}

//-------------------------------------------------------------------------
bool albaVMESlicer::IsDataAvailable()
{
	if(GetSlicedVMELink())
    return GetSlicedVMELink()->IsDataAvailable();
	else
		return false;
}

//-------------------------------------------------------------------------
void albaVMESlicer::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
{
  kframes.clear(); // no timestamps
}
//-----------------------------------------------------------------------
albaVME *albaVMESlicer::GetSlicedVMELink()
{
  return GetLink("SlicedVME");
}
//-----------------------------------------------------------------------
void albaVMESlicer::SetSlicedVMELink(albaVME *node)
{
  SetLink("SlicedVME", node);
  Modified();
}
//-----------------------------------------------------------------------
void albaVMESlicer::InternalPreUpdate()
{
  albaVME *vol = GetSlicedVMELink();
	if (vol)
	{
		vtkDataSet *vtkdata = vol->GetOutput()->GetVTKData();
		if (vtkdata)
		{
			double pos[3];
			float vectX[3], vectY[3], n[3];

			//transform
			m_CopyTransform->SetMatrix(m_Transform->GetMatrix());
			m_CopyTransform->Update();

			albaSmartPointer<albaTransform> slicedVMETransform;
			slicedVMETransform->SetMatrix(vol->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
			slicedVMETransform->Update();
			slicedVMETransform->Invert();
			slicedVMETransform->Update();

			albaSmartPointer<albaTransform> parentTransform;
			parentTransform->SetMatrix(GetParent()->GetOutput()->GetAbsMatrix()->GetVTKMatrix());
			parentTransform->Update();

			parentTransform->Concatenate(slicedVMETransform, 0);
			parentTransform->Update();

			m_CopyTransform->Concatenate(parentTransform, 0);
			m_CopyTransform->Update();
			m_CopyTransform->GetPosition(pos);
			m_CopyTransform->GetVersor(0, vectX);
			m_CopyTransform->GetVersor(1, vectY);

			vtkMath::Normalize(vectX);
			vtkMath::Normalize(vectY);
			vtkMath::Cross(vectY, vectX, n);
			vtkMath::Normalize(n);
			vtkMath::Cross(n, vectX, vectY);
			vtkMath::Normalize(vectY);

			vtkdata->Update();
			vtkDataArray *scalars = vtkdata->GetPointData()->GetScalars();
			if (scalars == NULL)
			{
				return;
			}

			vtkImageData *texture = m_PSlicer->GetTexture();
			texture->SetScalarType(scalars->GetDataType());
			texture->SetNumberOfScalarComponents(scalars->GetNumberOfComponents());
			texture->Modified();

			mmaMaterial * material = GetMaterial();
			material->SetMaterialTexture(texture);
			albaVMEVolumeGray *slicedVolume = albaVMEVolumeGray::SafeDownCast(vol);
			albaVMEOutputVolume *volumeOutput = slicedVolume ? albaVMEOutputVolume::SafeDownCast(slicedVolume->GetOutput()) : NULL;
			if (slicedVolume)
			{
				mmaVolumeMaterial * volumeMaterial = volumeOutput->GetMaterial();
				volumeMaterial->m_ColorLut->GetTableRange(material->m_TableRange);
			}
			else
			{
				texture->GetScalarRange(material->m_TableRange);
			}

			if (m_UpdateVTKPropertiesFromMaterial)
				material->UpdateProp();

			m_PSlicer->SetInput(vtkdata);
			m_PSlicer->SetPlaneOrigin(pos);
			m_PSlicer->SetPlaneAxisX(vectX);
			m_PSlicer->SetPlaneAxisY(vectY);

			m_ISlicer->SetInput(vtkdata);
			m_ISlicer->SetPlaneOrigin(pos);
			m_ISlicer->SetPlaneAxisX(vectX);
			m_ISlicer->SetPlaneAxisY(vectY);

			m_BackTransform->SetTransform(m_CopyTransform->GetVTKTransform()->GetInverse());
			m_BackTransform->Update();
		}
	}

  m_SlicedName = vol ? vol->GetName() : _("none");
}

//-----------------------------------------------------------------------
void albaVMESlicer::InternalUpdate()
{
  albaVME *vol = GetSlicedVMELink();
	if (vol)
	{
		vol->Update();
		if (vtkDataSet *vtkdata = vol->GetOutput()->GetVTKData())
		{
			m_PSlicer->Update();
			m_ISlicer->Update();

			vtkDataArray *scalars = vtkdata->GetPointData()->GetScalars();
			if (scalars == NULL)
			{
				return;
			}

			vtkImageData *texture = m_PSlicer->GetTexture();

			mmaMaterial * material = GetMaterial();

			material->SetMaterialTexture(texture);

			albaVMEVolumeGray *slicedVolume = albaVMEVolumeGray::SafeDownCast(vol);
			albaVMEOutputVolume *volumeOutput = slicedVolume ? albaVMEOutputVolume::SafeDownCast(slicedVolume->GetOutput()) : NULL;
			if (slicedVolume)
			{
				mmaVolumeMaterial * volumeMaterial = volumeOutput->GetMaterial();
				volumeMaterial->m_ColorLut->GetTableRange(material->m_TableRange);
			}
			else
			{
				texture->GetScalarRange(material->m_TableRange);
			}

			if (m_UpdateVTKPropertiesFromMaterial == true)
			{
				GetMaterial()->UpdateProp();
			}
		}
	}
}
//-----------------------------------------------------------------------
int albaVMESlicer::InternalStore(albaStorageElement *parent)
{  
  if (Superclass::InternalStore(parent)==ALBA_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
    return ALBA_OK;
  }
  return ALBA_ERROR;
}

//-----------------------------------------------------------------------
int albaVMESlicer::InternalRestore(albaStorageElement *node)
{
  if (Superclass::InternalRestore(node)==ALBA_OK)
  {
    albaMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==ALBA_OK)
    {
      m_Transform->SetMatrix(matrix);
      
      return ALBA_OK;
    }
  }

  return ALBA_ERROR;
}

//-----------------------------------------------------------------------
void albaVMESlicer::Print(std::ostream& os, const int tabs)
{
  Superclass::Print(os,tabs);
  albaIndent indent(tabs);

  albaMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}
//-------------------------------------------------------------------------
char** albaVMESlicer::GetIcon() 
{
  #include "albaVMESlicer.xpm"
  return albaVMESlicer_xpm;
}
//-------------------------------------------------------------------------
void albaVMESlicer::SetTrilinearInterpolation(bool on) 
{
  m_TrilinearInterpolationOn = on;
  if(m_ISlicer)
    m_ISlicer->SetTrilinearInterpolation(on);
}

//-------------------------------------------------------------------------
void albaVMESlicer::SetEnableGPU(bool val)
{
	m_EnableGPU = val;
	if (m_ISlicer)
		m_ISlicer->SetGPUEnabled(m_EnableGPU);
}
