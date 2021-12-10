/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateVolume
 Authors: Paolo Quadrani
 
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

//----------------------------------------------------------------------------
albaOpCreateVolume::albaOpCreateVolume(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Volume  = NULL;
  m_Spacing[0] = m_Spacing[1] = m_Spacing[2] = 1.0;
  m_Dimensions[0] = m_Dimensions[1] = m_Dimensions[2] = 10.0;
  m_Density = 0.0;
}
//----------------------------------------------------------------------------
albaOpCreateVolume::~albaOpCreateVolume()
//----------------------------------------------------------------------------
{
  albaDEL(m_Volume);
}
//----------------------------------------------------------------------------
albaOp* albaOpCreateVolume::Copy()   
//----------------------------------------------------------------------------
{
	return new albaOpCreateVolume(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpCreateVolume::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsALBAType(albaVME));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum VOLUME_CREATE_ID
{
  ID_SPACING = MINID,
  ID_DIMENSIONS,
  ID_SCALA_VALUE,
};
//----------------------------------------------------------------------------
void albaOpCreateVolume::OpRun()
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUI(this);
	m_Gui->Label("");
  m_Gui->Vector(ID_SPACING, "xyz spc", m_Spacing, 0.0000000001);
  m_Gui->Vector(ID_DIMENSIONS, "xyz dim", m_Dimensions, 0.0000000001);
  m_Gui->Double(ID_SCALA_VALUE, "Density", &m_Density, 0.0);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
  m_Gui->OkCancel();
 	m_Gui->Label("");

  ShowGui();
}
//----------------------------------------------------------------------------
void albaOpCreateVolume::OnEvent( albaEventBase *alba_event )
//----------------------------------------------------------------------------
{
  switch (alba_event->GetId())
  {
    case ID_SPACING:
    case ID_DIMENSIONS:
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
void albaOpCreateVolume::SetVolumeSpacing( double spc[3] )
//----------------------------------------------------------------------------
{
  m_Spacing[0] = spc[0];
  m_Spacing[1] = spc[1];
  m_Spacing[2] = spc[2];
}

//----------------------------------------------------------------------------
void albaOpCreateVolume::GetVolumeSpacing( double spc[3] )
//----------------------------------------------------------------------------
{
  spc[0] = m_Spacing[0];
  spc[1] = m_Spacing[1];
  spc[2] = m_Spacing[2];
}

//----------------------------------------------------------------------------
void albaOpCreateVolume::CreateVolume()
//----------------------------------------------------------------------------
{
  int dim[3], num_comp;
  dim[0] = (int)(m_Dimensions[0] / m_Spacing[0]) + 1;
  dim[1] = (int)(m_Dimensions[1] / m_Spacing[1]) + 1;
  dim[2] = (int)(m_Dimensions[2] / m_Spacing[2]) + 1;
  num_comp = dim[0] * dim[1] * dim[2];

  vtkDoubleArray *sca = vtkDoubleArray::New();
  sca->SetNumberOfTuples(num_comp);
  sca->FillComponent(0, m_Density);

  vtkALBASmartPointer<vtkImageData> vol;
  vol->SetSpacing(m_Spacing);
  vol->SetDimensions(dim);
  vol->SetExtent(0, dim[0]-1, 0, dim[1]-1, 0, dim[2]-1);
  vol->SetNumberOfScalarComponents(1);
  vol->GetPointData()->SetScalars(sca);
  vol->Update();
  sca->Delete();

  albaNEW(m_Volume);
  m_Volume->SetName("volume");
  m_Volume->SetData(vol, -1);
  m_Output = m_Volume;
}

//----------------------------------------------------------------------------
void albaOpCreateVolume::SetVolumeDimensions( double dim[3] )
//----------------------------------------------------------------------------
{
  m_Dimensions[0] = dim[0];
  m_Dimensions[1] = dim[1];
  m_Dimensions[2] = dim[2];
}

//----------------------------------------------------------------------------
void albaOpCreateVolume::GetVolumeDimensions( double dim[3] )
//----------------------------------------------------------------------------
{
  dim[0] = m_Dimensions[0];
  dim[1] = m_Dimensions[1];
  dim[2] = m_Dimensions[2];
}
