/*=========================================================================

 Program: MAF2
 Module: mafOpCreateLabeledVolume
 Authors: Roberto Mucci
 
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


#include "mafOpCreateLabeledVolume.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVMEVolumeGray.h"

#include "mafVMELabeledVolume.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateLabeledVolume);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateLabeledVolume::mafOpCreateLabeledVolume(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_LabeledVolume   = NULL;
}
//----------------------------------------------------------------------------
mafOpCreateLabeledVolume::~mafOpCreateLabeledVolume( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_LabeledVolume);
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateLabeledVolume::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpCreateLabeledVolume(m_Label);
}
//----------------------------------------------------------------------------
bool mafOpCreateLabeledVolume::Accept(mafVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVMEVolumeGray));
}
//----------------------------------------------------------------------------
void mafOpCreateLabeledVolume::OpRun()   
//----------------------------------------------------------------------------
{
  mafNEW(m_LabeledVolume);
  m_LabeledVolume->SetName("Labeled Volume");

 
  
  m_Output = m_LabeledVolume;
  m_LabeledVolume->SetVolumeLink(m_Input);
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mafOpCreateLabeledVolume::OpDo()
//----------------------------------------------------------------------------
{
  m_LabeledVolume->ReparentTo(m_Input);
}
