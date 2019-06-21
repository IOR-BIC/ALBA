/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: albaPipeSlice.cpp,v $ 
  Language: C++ 
  Date: $Date: 2012-04-06 09:41:19 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaPipeSlice.h"
#include "albaVME.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeSlice);
//----------------------------------------------------------------------------

#include "albaMemDbg.h"

//----------------------------------------------------------------------------
albaPipeSlice::albaPipeSlice() :albaPipe()
//----------------------------------------------------------------------------
{  
  m_Origin[0] = m_Origin[1] = m_Origin[2] = 0.0;
  m_Normal[0] = m_Normal[1] = 0.0;
  m_Normal[2] = 1.0;
}

//----------------------------------------------------------------------------
albaPipeSlice::~albaPipeSlice()
//----------------------------------------------------------------------------
{ 
}

//----------------------------------------------------------------------------
//Get the slice origin coordinates and normal.
//Both, Origin and Normal may be NULL, if the value is not to be retrieved.
/*virtual*/ void albaPipeSlice::GetSlice(double *Origin, double *Normal)
//----------------------------------------------------------------------------
{
  if (Origin != NULL)
  {
    Origin[0] = m_Origin[0];
    Origin[1] = m_Origin[1];
    Origin[2] = m_Origin[2];
  }

  if (Normal != NULL)
  {
    Normal[0] = m_Normal[0];
    Normal[1] = m_Normal[1];
    Normal[2] = m_Normal[2];
  }  
}

//----------------------------------------------------------------------------
//Set the origin and normal of the slice.
//Both, Origin and Normal may be NULL, if the current value is to be preserved.
/*virtual*/ void albaPipeSlice::SetSlice(double *Origin, double *Normal)
//----------------------------------------------------------------------------
{
  if (Origin != NULL)
  {
    m_Origin[0] = Origin[0];
    m_Origin[1] = Origin[1];
    m_Origin[2] = Origin[2];
  }

  if (Normal != NULL)
  {
    m_Normal[0] = Normal[0];
    m_Normal[1] = Normal[1];
    m_Normal[2] = Normal[2];
  }  
}