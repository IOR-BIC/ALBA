/*=========================================================================

 Program: MAF2
 Module: mafOpCreateSpline
 Authors: Daniele Giunchi & Matteo Giacomoni
 
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


#include "mafOpCreateSpline.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMEPolylineSpline.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateSpline);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateSpline::mafOpCreateSpline(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_PolylineSpline = NULL;
}
//----------------------------------------------------------------------------
mafOpCreateSpline::~mafOpCreateSpline( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_PolylineSpline);
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateSpline::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpCreateSpline(m_Label);
}
//----------------------------------------------------------------------------
bool mafOpCreateSpline::Accept(mafVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateSpline::OpRun()   
//----------------------------------------------------------------------------
{
  mafNEW(m_PolylineSpline);
  m_PolylineSpline->SetName("Polyline Spline");
  m_Output = m_PolylineSpline;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mafOpCreateSpline::OpDo()
//----------------------------------------------------------------------------
{
  m_PolylineSpline->ReparentTo(m_Input);
}
