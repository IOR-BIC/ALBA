/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateVolume
 Authors: Paolo Quadrani, Gianluigi Crimi
 
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


#include "albaOpCreateVolume.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"

#include "albaVMERoot.h"
#include "albaVMEVolumeGray.h"

#include "vtkALBASmartPointer.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCreateVolume);

//----------------------------------------------------------------------------
// Constants:
enum VOLUME_CREATE_ID
{
	ID_SPACING = MINID,
	ID_ORIGIN,
  ID_VOXEL_NUM,
	ID_DIMENSIONS,
	ID_SCALA_VALUE,
};

//----------------------------------------------------------------------------
albaOpCreateVolume::albaOpCreateVolume(const wxString &label) :albaOp(label)
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Volume  = NULL;
  m_Spacing[0] = m_Spacing[1] = m_Spacing[2] = 1.0;
  m_VoxelNum[0] = m_VoxelNum[1] = m_VoxelNum[2] = 10;
  m_Size[0] = m_Size[1] = m_Size[2] = 10.0;
	m_Origin[0] = m_Origin[1] = m_Origin[2] = 0.0;
	m_Density = 0.0;
}
//----------------------------------------------------------------------------
albaOpCreateVolume::~albaOpCreateVolume()
{
  albaDEL(m_Volume);
}
//----------------------------------------------------------------------------
albaOp* albaOpCreateVolume::Copy()   
{
	return new albaOpCreateVolume(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpCreateVolume::InternalAccept(albaVME*node)
{
  return (node && node->IsALBAType(albaVME));
}
//----------------------------------------------------------------------------
void albaOpCreateVolume::OpRun()
{
  m_Gui = new albaGUI(this);
  m_Gui->Label("");
  m_Gui->Vector(ID_ORIGIN, "Origin:", m_Origin);

  m_Gui->Label("");
  m_Gui->Vector(ID_SPACING, "Spacing:", m_Spacing, 0.0000000001);
  m_Gui->Vector(ID_VOXEL_NUM, "Voxel #:", m_VoxelNum, 1);
  m_Gui->Vector(ID_DIMENSIONS, "Size:", m_Size);
  m_Gui->Enable(ID_DIMENSIONS, false);

  m_Gui->Label("");
  m_Gui->Double(ID_SCALA_VALUE, "Value:", &m_Density, 0.0);

  //////////////////////////////////////////////////////////////////////////
  m_Gui->Label("");
  m_Gui->Divider(1);
  m_Gui->OkCancel();
  m_Gui->Label("");

  ShowGui();
}
//----------------------------------------------------------------------------
void albaOpCreateVolume::OnEvent( albaEventBase *alba_event )
{
  switch (alba_event->GetId())
  {
    case ID_SPACING:
    case ID_VOXEL_NUM:
      for (int i = 0; i < 3; i++)
        m_Size[i] = m_Spacing[i] * m_VoxelNum[i];
      m_Gui->Update();
    break;
    case ID_ORIGIN:
    case ID_SCALA_VALUE:
    break;
    case wxOK:
      CreateVolume();
      OpStop(OP_RUN_OK);        
    break;
    case wxCANCEL:
      OpStop(OP_RUN_CANCEL);        
    break;
  }
}

//----------------------------------------------------------------------------
void albaOpCreateVolume::SetVolumeOrigin(double origin[3])
{
	m_Origin[0] = origin[0];
	m_Origin[1] = origin[1];
	m_Origin[2] = origin[2];
}

//----------------------------------------------------------------------------
void albaOpCreateVolume::GetVolumeOrigin(double origin[3])
{
	origin[0] = m_Origin[0];
	origin[1] = m_Origin[1];
	origin[2] = m_Origin[2];
}
//----------------------------------------------------------------------------
void albaOpCreateVolume::SetVolumeSpacing( double spc[3] )
{
  m_Spacing[0] = spc[0];
  m_Spacing[1] = spc[1];
  m_Spacing[2] = spc[2];
}

//----------------------------------------------------------------------------
void albaOpCreateVolume::GetVolumeSpacing( double spc[3] )
{
  spc[0] = m_Spacing[0];
  spc[1] = m_Spacing[1];
  spc[2] = m_Spacing[2];
}

//----------------------------------------------------------------------------
void albaOpCreateVolume::CreateVolume()
{
  int *dim = m_VoxelNum;
  vtkIdType num_comp = dim[0] * dim[1] * dim[2];

  vtkDoubleArray *sca = vtkDoubleArray::New();
  sca->SetNumberOfTuples(num_comp);
  sca->FillComponent(0, m_Density);

  vtkALBASmartPointer<vtkImageData> vol;
  vol->SetSpacing(m_Spacing);
  vol->SetOrigin(m_Origin);
  vol->SetDimensions(m_VoxelNum);
  vol->SetExtent(0, m_VoxelNum[0]-1, 0, m_VoxelNum[1]-1, 0, m_VoxelNum[2]-1);
  vol->GetPointData()->SetScalars(sca);
  sca->Delete();

  albaNEW(m_Volume);
  m_Volume->SetName("volume");
  m_Volume->SetData(vol, -1);
  m_Output = m_Volume;
}

//----------------------------------------------------------------------------
void albaOpCreateVolume::SetVolumeDimensions( double dim[3] )
{
  m_VoxelNum[0] = dim[0];
  m_VoxelNum[1] = dim[1];
  m_VoxelNum[2] = dim[2];
}

//----------------------------------------------------------------------------
void albaOpCreateVolume::GetVolumeDimensions( double dim[3] )
{
  dim[0] = m_VoxelNum[0];
  dim[1] = m_VoxelNum[1];
  dim[2] = m_VoxelNum[2];
}
