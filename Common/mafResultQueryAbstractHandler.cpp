/*=========================================================================

 Program: MAF2
 Module: mafResultQueryAbstractHandler
 Authors: Daniele Giunchi
 
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

#include "mafResultQueryAbstractHandler.h"

//----------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafResultQueryAbstractHandler);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafResultQueryAbstractHandler::mafResultQueryAbstractHandler()
:m_MatrixStringResult(NULL)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafResultQueryAbstractHandler::~mafResultQueryAbstractHandler()
//----------------------------------------------------------------------------
{
  InternalResultReset();
}
//----------------------------------------------------------------------------
void mafResultQueryAbstractHandler::InternalResultReset()
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
int mafResultQueryAbstractHandler::GetNumberOfRecords() const
//----------------------------------------------------------------------------
{
  return m_MatrixStringResult.size();
}
//----------------------------------------------------------------------------
int mafResultQueryAbstractHandler::GetNumberOfFields() const
//----------------------------------------------------------------------------
{
  return m_ColumnsNameInformation.size();
}