/*=========================================================================

 Program: MAF2
 Module: mafRelationalDatabaseAbstractConnector
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

#include "mafRelationalDatabaseAbstractConnector.h"

//----------------------------------------------------------------------------
mafCxxAbstractTypeMacro(mafRelationalDatabaseAbstractConnector);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafRelationalDatabaseAbstractConnector::mafRelationalDatabaseAbstractConnector(const char *queryString)
:m_Query(queryString),
m_ResultQueryHandler(NULL)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafRelationalDatabaseAbstractConnector::~mafRelationalDatabaseAbstractConnector()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafRelationalDatabaseAbstractConnector::SetQuery(const char* queryString)
//----------------------------------------------------------------------------
{
  m_Query = queryString;
}
//----------------------------------------------------------------------------
const char* mafRelationalDatabaseAbstractConnector::GetQuery() const
//----------------------------------------------------------------------------
{
  return m_Query.c_str();
}
//----------------------------------------------------------------------------
bool mafRelationalDatabaseAbstractConnector::IsFailed() const
//----------------------------------------------------------------------------
{ 
  return m_ResultQueryHandler->IsFailed();
}
//----------------------------------------------------------------------------
int mafRelationalDatabaseAbstractConnector::GetNumberOfRecords() const
//----------------------------------------------------------------------------
{
  return m_ResultQueryHandler->GetNumberOfRecords();
}
//----------------------------------------------------------------------------
int mafRelationalDatabaseAbstractConnector::GetNumberOfFields() const
//----------------------------------------------------------------------------
{
  return m_ResultQueryHandler->GetNumberOfFields();
}
//----------------------------------------------------------------------------
WebRowSetStringDataTable mafRelationalDatabaseAbstractConnector::GetQueryResultAsStringMatrix() const
//----------------------------------------------------------------------------
{
  return m_ResultQueryHandler->GetResultAsStringMatrix();
}
//----------------------------------------------------------------------------
WebRowSetQueryObjectsTable mafRelationalDatabaseAbstractConnector::GetQueryResultAsObjectsMatrix() const
//----------------------------------------------------------------------------
{
  return m_ResultQueryHandler->GetResultAsObjectsMatrix();
}
//----------------------------------------------------------------------------
WebRowSetColumnTypeVector mafRelationalDatabaseAbstractConnector::GetColumnsTypeAsStringVector() const
//----------------------------------------------------------------------------
{
  return m_ResultQueryHandler->GetColumnsTypeInformationAsStringVector();
}
//----------------------------------------------------------------------------
WebRowSetColumnNameVector mafRelationalDatabaseAbstractConnector::GetColumnsNameAsStringVector() const
//----------------------------------------------------------------------------
{
  return m_ResultQueryHandler->GetColumnsNameInformationAsStringVector();
}
//----------------------------------------------------------------------------
void mafRelationalDatabaseAbstractConnector::SetRelationalDatabaseInformation(mafRelationalDatabaseInformation info)
//----------------------------------------------------------------------------
{
  m_RelationalDatabaseInformation.m_Url = info.m_Url;
  m_RelationalDatabaseInformation.m_Username = info.m_Username;
  m_RelationalDatabaseInformation.m_Password = info.m_Password;
  m_RelationalDatabaseInformation.m_Certificate = info.m_Certificate;
  m_RelationalDatabaseInformation.m_ConfName = info.m_ConfName;
}
//----------------------------------------------------------------------------
const mafRelationalDatabaseInformation &mafRelationalDatabaseAbstractConnector::GetRelationalDatabaseInformation() const
//----------------------------------------------------------------------------
{
  return m_RelationalDatabaseInformation;
}