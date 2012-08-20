/*=========================================================================

 Program: MAF2Medical
 Module: medResultQueryAbstractHandler
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "medDefines.h" 
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
  int i;
  for(i = 0;i<m_MatrixStringResult.size();i++)
  {
    m_MatrixStringResult[i].clear();
  }
  m_MatrixStringResult.clear();

  for(i = 0;i<m_MatrixObjectResult.size();i++)
  {
    for (int j=0;j<m_MatrixObjectResult[i].size();++j)
    {
      delete m_MatrixObjectResult[i][j];
    }
    m_MatrixObjectResult[i].clear();
  }
  m_MatrixObjectResult.clear();
  
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