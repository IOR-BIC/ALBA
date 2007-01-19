/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreatePanoramic.cpp,v $
  Language:  C++
  Date:      $Date: 2007-01-19 15:29:03 $
  Version:   $Revision: 1.1 $
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


#include "mmoCreatePanoramic.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVMEVolume.h"
#include "mafVMEAdvancedProber.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoCreatePanoramic::mmoCreatePanoramic(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Panoramic  = NULL;
}
//----------------------------------------------------------------------------
mmoCreatePanoramic::~mmoCreatePanoramic( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_Panoramic);
}
//----------------------------------------------------------------------------
mafOp* mmoCreatePanoramic::Copy()   
//----------------------------------------------------------------------------
{
	return new mmoCreatePanoramic(m_Label);
}
//----------------------------------------------------------------------------
bool mmoCreatePanoramic::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node != NULL && node->IsA("mafVMEVolumeGray"));
}
//----------------------------------------------------------------------------
void mmoCreatePanoramic::OpRun()
//----------------------------------------------------------------------------
{
  mafNEW(m_Panoramic);
  m_Panoramic->SetName("Panoramic");
  m_Output = m_Panoramic;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mmoCreatePanoramic::OpDo()
//----------------------------------------------------------------------------
{
  m_Panoramic->ReparentTo(mafVME::SafeDownCast(m_Input));
  m_Panoramic->SetVolumeLink(m_Input);
}
