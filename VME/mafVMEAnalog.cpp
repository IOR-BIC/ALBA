/*=========================================================================

 Program: MAF2
 Module: mafVMEAnalog
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

#include "mafVMEAnalog.h"

#include <vnl/vnl_vector.h>
#include "mafGUI.h"
#include "mafVMEOutputScalarMatrix.h"

#include "mafDataVector.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEAnalog)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEAnalog::mafVMEAnalog()
//-------------------------------------------------------------------------
{
  m_CurrentTime   = 0.0;
}
//-------------------------------------------------------------------------
mafVMEAnalog::~mafVMEAnalog()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
mafGUI* mafVMEAnalog::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = mafVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->Divider();
  return m_Gui;
}

//-----------------------------------------------------------------------
void mafVMEAnalog::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
}

//-------------------------------------------------------------------------
bool mafVMEAnalog::IsAnimated()
//-------------------------------------------------------------------------
{
  vnl_vector<double> timeVector = this->GetScalarOutput()->GetScalarData().get_row(0);
  return (timeVector.size() > 0);  
}

//-------------------------------------------------------------------------
void mafVMEAnalog::GetTimeBounds(mafTimeStamp tbounds[2]) 
//-------------------------------------------------------------------------
{
  vnl_vector<double> timeVector = this->GetScalarOutput()->GetScalarData().get_row(0);
  tbounds[0] = timeVector[0];
  tbounds[1] = timeVector[timeVector.size()-1];
}

//-------------------------------------------------------------------------
void mafVMEAnalog::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
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
void mafVMEAnalog::GetLocalTimeBounds(mafTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
  GetTimeBounds(tbounds);
}
