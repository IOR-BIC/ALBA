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

#include <vnl/vnl_vector.h>
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
  vnl_vector<double> timeVector = this->GetScalarOutput()->GetScalarData().get_row(0);
  return (timeVector.size() > 0);  
}

//-------------------------------------------------------------------------
void albaVMEAnalog::GetTimeBounds(albaTimeStamp tbounds[2]) 
//-------------------------------------------------------------------------
{
  vnl_vector<double> timeVector = this->GetScalarOutput()->GetScalarData().get_row(0);
	int timeVectSize = timeVector.size();
	if (timeVectSize > 0)
	{
		tbounds[0] = timeVector[0];
		tbounds[1] = timeVector[timeVectSize - 1];
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
  vnl_vector<double> timeVector = this->GetScalarOutput()->GetScalarData().get_row(0);
  for (int n = 0; n < timeVector.size(); n++)
  {
    kframes.push_back(timeVector.get(n));
  }
}
//-------------------------------------------------------------------------
void albaVMEAnalog::GetLocalTimeBounds(albaTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
  GetTimeBounds(tbounds);
}
