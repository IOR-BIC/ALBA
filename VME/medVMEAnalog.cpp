/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medVMEAnalog.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 11:19:42 $
  Version:   $Revision: 1.3 $
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

#include "medVMEAnalog.h"

#include <vnl/vnl_vector.h>
#include "mafGUI.h"
#include "mafVMEOutputScalarMatrix.h"

#include "mafDataVector.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(medVMEAnalog)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
medVMEAnalog::medVMEAnalog()
//-------------------------------------------------------------------------
{
  m_CurrentTime   = 0.0;
}
//-------------------------------------------------------------------------
medVMEAnalog::~medVMEAnalog()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
mafGUI* medVMEAnalog::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = mafVME::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->Divider();
  return m_Gui;
}

//-----------------------------------------------------------------------
void medVMEAnalog::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
}

//-------------------------------------------------------------------------
bool medVMEAnalog::IsAnimated()
//-------------------------------------------------------------------------
{
  vnl_vector<double> timeVector = this->GetScalarOutput()->GetScalarData().get_row(0);
  return (timeVector.size() > 0);  
}

//-------------------------------------------------------------------------
void medVMEAnalog::GetTimeBounds(mafTimeStamp tbounds[2]) 
//-------------------------------------------------------------------------
{
  vnl_vector<double> timeVector = this->GetScalarOutput()->GetScalarData().get_row(0);
  tbounds[0] = timeVector[0];
  tbounds[1] = timeVector[timeVector.size()-1];
}

//-------------------------------------------------------------------------
void medVMEAnalog::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
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
void medVMEAnalog::GetLocalTimeBounds(mafTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
  GetTimeBounds(tbounds);
}
