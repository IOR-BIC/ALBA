/*=========================================================================
Program:   @neufuse
Module:    $RCSfile: medResultQueryAbstractHandler.cpp,v $
Language:  C++
Date:      $Date: 2009-09-22 07:33:45 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medResultQueryAbstractHandler.h"

//----------------------------------------------------------------------------
mafCxxAbstractTypeMacro(medResultQueryAbstractHandler);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medResultQueryAbstractHandler::medResultQueryAbstractHandler()
:m_MatrixStringResult(NULL)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
medResultQueryAbstractHandler::~medResultQueryAbstractHandler()
//----------------------------------------------------------------------------
{
  InternalResultReset();
}
//----------------------------------------------------------------------------
void medResultQueryAbstractHandler::InternalResultReset()
//----------------------------------------------------------------------------
{
  int i = 0;
  for(;i<m_MatrixStringResult.size();i++)
  {
    m_MatrixStringResult[i].clear();

    /*int j=0;
    for(;j<m_ColumnsTypeInformation.size();j++)
    {
      delete m_MatrixObjectResult[i][j];
    }
    m_MatrixObjectResult[i].clear();*/
    
  }
  m_MatrixStringResult.clear();
  //m_MatrixObjectResult.clear();
  
  m_ColumnsTypeInformation.clear();
  m_ColumnsNameInformation.clear();

}
//----------------------------------------------------------------------------
int medResultQueryAbstractHandler::GetNumberOfRecords() const
//----------------------------------------------------------------------------
{
  return m_MatrixStringResult.size();
}
//----------------------------------------------------------------------------
int medResultQueryAbstractHandler::GetNumberOfFields() const
//----------------------------------------------------------------------------
{
  return m_ColumnsNameInformation.size();
}