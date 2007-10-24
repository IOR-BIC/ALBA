/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpCreateLabeledVolume.cpp,v $
  Language:  C++
  Date:      $Date: 2007-10-24 08:48:16 $
  Version:   $Revision: 1.1 $
  Authors:   Roberto Mucci
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


#include "medOpCreateLabeledVolume.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVMEVolumeGray.h"

#include "mafVMELabeledVolume.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpCreateLabeledVolume);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpCreateLabeledVolume::medOpCreateLabeledVolume(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_LabeledVolume   = NULL;
}
//----------------------------------------------------------------------------
medOpCreateLabeledVolume::~medOpCreateLabeledVolume( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_LabeledVolume);
}
//----------------------------------------------------------------------------
mafOp* medOpCreateLabeledVolume::Copy()   
//----------------------------------------------------------------------------
{
	return new medOpCreateLabeledVolume(m_Label);
}
//----------------------------------------------------------------------------
bool medOpCreateLabeledVolume::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVMEVolumeGray));
}
//----------------------------------------------------------------------------
void medOpCreateLabeledVolume::OpRun()   
//----------------------------------------------------------------------------
{
  mafNEW(m_LabeledVolume);
  m_LabeledVolume->SetName("Labeled Volume");

 
  
  m_Output = m_LabeledVolume;
  m_LabeledVolume->SetVolumeLink(m_Input);
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void medOpCreateLabeledVolume::OpDo()
//----------------------------------------------------------------------------
{
  m_LabeledVolume->ReparentTo(m_Input);
}
