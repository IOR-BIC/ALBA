/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateSpline
 Authors: Daniele Giunchi & Matteo Giacomoni
 
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


#include "albaOpCreateSpline.h"
#include "albaDecl.h"
#include "albaEvent.h"

#include "albaVMEPolylineSpline.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCreateSpline);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpCreateSpline::albaOpCreateSpline(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_PolylineSpline = NULL;
}
//----------------------------------------------------------------------------
albaOpCreateSpline::~albaOpCreateSpline( ) 
//----------------------------------------------------------------------------
{
  albaDEL(m_PolylineSpline);
}
//----------------------------------------------------------------------------
albaOp* albaOpCreateSpline::Copy()   
//----------------------------------------------------------------------------
{
	return new albaOpCreateSpline(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpCreateSpline::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsALBAType(albaVME));
}
//----------------------------------------------------------------------------
char** albaOpCreateSpline::GetIcon()
{
#include "pic/MENU_OP_CREATE_POLYLINESPLINE.xpm"
	return MENU_OP_CREATE_POLYLINESPLINE_xpm;
}
//----------------------------------------------------------------------------
void albaOpCreateSpline::OpRun()   
//----------------------------------------------------------------------------
{
  albaNEW(m_PolylineSpline);
  m_PolylineSpline->SetName("Polyline Spline");
  m_Output = m_PolylineSpline;
  albaEventMacro(albaEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void albaOpCreateSpline::OpDo()
//----------------------------------------------------------------------------
{
  m_PolylineSpline->ReparentTo(m_Input);
}
