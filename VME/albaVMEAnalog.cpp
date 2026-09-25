/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEAnalog
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

#include "albaVMEAnalog.h"

#include "albaGUI.h"
#include "albaVMEOutputScalarMatrix.h"

#include "albaDataVector.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEAnalog)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEAnalog::albaVMEAnalog()
//-------------------------------------------------------------------------
{
  m_CurrentTime   = 0.0;
}
//-------------------------------------------------------------------------
albaVMEAnalog::~albaVMEAnalog()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
albaGUI* albaVMEAnalog::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = albaVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->Divider();
  return m_Gui;
}

//-----------------------------------------------------------------------
void albaVMEAnalog::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  albaIndent indent(tabs);
}

//-------------------------------------------------------------------------
bool albaVMEAnalog::IsAnimated()
//-------------------------------------------------------------------------
{
  return (this->GetScalarOutput()->GetScalarData().GetColsNum() > 0);
}

//-------------------------------------------------------------------------
void albaVMEAnalog::GetTimeBounds(albaTimeStamp tbounds[2]) 
//-------------------------------------------------------------------------
{
  albaDynamicMatrix scalarData = this->GetScalarOutput()->GetScalarData();

	//time vector is on row 0 
	if (scalarData.GetRowsNum() > 0 && scalarData.GetColsNum() >= 2)
	{
		tbounds[0] = scalarData(0, 0);
		tbounds[1] = scalarData(0, scalarData.GetColsNum() - 1);
	}
	else
	{
		tbounds[0] = tbounds[1] = 0;
	}
}

//-------------------------------------------------------------------------
void albaVMEAnalog::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear();
  albaDynamicMatrix scalarData = this->GetScalarOutput()->GetScalarData();
	
	//time vector is on row 0
  for (int n = 0; n < scalarData.GetColsNum(); n++)
  {
    kframes.push_back(scalarData(0,n));
  }
}
//-------------------------------------------------------------------------
void albaVMEAnalog::GetLocalTimeBounds(albaTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
  GetTimeBounds(tbounds);
}
