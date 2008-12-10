/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpCreateVolume.cpp,v $
  Language:  C++
  Date:      $Date: 2008-12-10 10:16:17 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Paolo Quadrani
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


#include "mafOpCreateVolume.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"

#include "mafVMERoot.h"
#include "mafVMEVolumeGray.h"

#include "vtkMAFSmartPointer.h"
#include "vtkStructuredPoints.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateVolume);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateVolume::mafOpCreateVolume(const wxString &label) :
mafOp(label)
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
mafOpCreateVolume::~mafOpCreateVolume()
//----------------------------------------------------------------------------
{
  mafDEL(m_Volume);
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateVolume::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpCreateVolume(m_Label);
}
//----------------------------------------------------------------------------
bool mafOpCreateVolume::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
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
void mafOpCreateVolume::OpRun()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  m_Gui->Vector(ID_SPACING, "xyz spc", m_Spacing, 0.0000000001);
  m_Gui->Vector(ID_DIMENSIONS, "xyz dim", m_Dimensions, 0.0000000001);
  m_Gui->Double(ID_SCALA_VALUE, "density", &m_Density, 0.0);
  m_Gui->OkCancel();

  ShowGui();
}
//----------------------------------------------------------------------------
void mafOpCreateVolume::OnEvent( mafEventBase *maf_event )
//----------------------------------------------------------------------------
{
  switch (maf_event->GetId())
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
void mafOpCreateVolume::SetVolumeSpacing( double spc[3] )
//----------------------------------------------------------------------------
{
  m_Spacing[0] = spc[0];
  m_Spacing[1] = spc[1];
  m_Spacing[2] = spc[2];
}

//----------------------------------------------------------------------------
void mafOpCreateVolume::GetVolumeSpacing( double spc[3] )
//----------------------------------------------------------------------------
{
  spc[0] = m_Spacing[0];
  spc[1] = m_Spacing[1];
  spc[2] = m_Spacing[2];
}

//----------------------------------------------------------------------------
void mafOpCreateVolume::CreateVolume()
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

  vtkMAFSmartPointer<vtkStructuredPoints> vol;
  vol->SetSpacing(m_Spacing);
  vol->SetDimensions(dim);
  vol->SetExtent(0, dim[0]-1, 0, dim[1]-1, 0, dim[2]-1);
  vol->SetNumberOfScalarComponents(1);
  vol->GetPointData()->SetScalars(sca);
  vol->Update();
  sca->Delete();

  mafNEW(m_Volume);
  m_Volume->SetName("volume");
  m_Volume->SetData(vol, -1);
  m_Output = m_Volume;
}

//----------------------------------------------------------------------------
void mafOpCreateVolume::SetVolumeDimensions( double dim[3] )
//----------------------------------------------------------------------------
{
  m_Dimensions[0] = dim[0];
  m_Dimensions[1] = dim[1];
  m_Dimensions[2] = dim[2];
}

//----------------------------------------------------------------------------
void mafOpCreateVolume::GetVolumeDimensions( double dim[3] )
//----------------------------------------------------------------------------
{
  dim[0] = m_Dimensions[0];
  dim[1] = m_Dimensions[1];
  dim[2] = m_Dimensions[2];
}
