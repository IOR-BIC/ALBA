/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateSpline.cpp,v $
  Language:  C++
  Date:      $Date: 2007-03-15 14:22:25 $
  Version:   $Revision: 1.2 $
  Authors:   Daniele Giunchi & Matteo Giacomoni
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


#include "mmoCreateSpline.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMEPolylineSpline.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoCreateSpline);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmoCreateSpline::mmoCreateSpline(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_PolylineSpline = NULL;
}
//----------------------------------------------------------------------------
mmoCreateSpline::~mmoCreateSpline( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_PolylineSpline);
}
//----------------------------------------------------------------------------
mafOp* mmoCreateSpline::Copy()   
//----------------------------------------------------------------------------
{
	return new mmoCreateSpline(m_Label);
}
//----------------------------------------------------------------------------
bool mmoCreateSpline::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mmoCreateSpline::OpRun()   
//----------------------------------------------------------------------------
{
  mafNEW(m_PolylineSpline);
  m_PolylineSpline->SetName("Polyline Spline");
  m_Output = m_PolylineSpline;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mmoCreateSpline::OpDo()
//----------------------------------------------------------------------------
{
  m_PolylineSpline->ReparentTo(m_Input);
}
