/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateLabeledVolume
 Authors: Roberto Mucci
 
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


#include "albaOpCreateLabeledVolume.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaVMEVolumeGray.h"

#include "albaVMELabeledVolume.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCreateLabeledVolume);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpCreateLabeledVolume::albaOpCreateLabeledVolume(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_LabeledVolume   = NULL;
}
//----------------------------------------------------------------------------
albaOpCreateLabeledVolume::~albaOpCreateLabeledVolume( ) 
//----------------------------------------------------------------------------
{
  albaDEL(m_LabeledVolume);
}
//----------------------------------------------------------------------------
albaOp* albaOpCreateLabeledVolume::Copy()   
//----------------------------------------------------------------------------
{
	return new albaOpCreateLabeledVolume(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpCreateLabeledVolume::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsALBAType(albaVMEVolumeGray));
}
//----------------------------------------------------------------------------
void albaOpCreateLabeledVolume::OpRun()   
//----------------------------------------------------------------------------
{
  albaNEW(m_LabeledVolume);
  m_LabeledVolume->SetName("Labeled Volume");

 
  
  m_Output = m_LabeledVolume;
  m_LabeledVolume->SetVolumeLink(m_Input);
  albaEventMacro(albaEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void albaOpCreateLabeledVolume::OpDo()
//----------------------------------------------------------------------------
{
  m_LabeledVolume->ReparentTo(m_Input);
}
