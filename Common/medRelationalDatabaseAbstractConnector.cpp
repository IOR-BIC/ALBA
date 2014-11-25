/*=========================================================================

 Program: MAF2Medical
 Module: medRelationalDatabaseAbstractConnector
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

#include "medRelationalDatabaseAbstractConnector.h"

//----------------------------------------------------------------------------
mafCxxAbstractTypeMacro(medRelationalDatabaseAbstractConnector);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medRelationalDatabaseAbstractConnector::medRelationalDatabaseAbstractConnector(const char *queryString)
:m_Query(queryString),
m_ResultQueryHandler(NULL)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
medRelationalDatabaseAbstractConnector::~medRelationalDatabaseAbstractConnector()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void medRelationalDatabaseAbstractConnector::SetQuery(const char* queryString)
//----------------------------------------------------------------------------
{
  m_Query = queryString;
}
//----------------------------------------------------------------------------
const char* medRelationalDatabaseAbstractConnector::GetQuery() const
//----------------------------------------------------------------------------
{
  return m_Query.c_str();
}
//----------------------------------------------------------------------------
bool medRelationalDatabaseAbstractConnector::IsFailed() const
//----------------------------------------------------------------------------
{ 
  return m_ResultQueryHandler->IsFailed();
}
//----------------------------------------------------------------------------
int medRelationalDatabaseAbstractConnector::GetNumberOfRecords() const
//----------------------------------------------------------------------------
{
  return m_ResultQueryHandler->GetNumberOfRecords();
}
//----------------------------------------------------------------------------
int medRelationalDatabaseAbstractConnector::GetNumberOfFields() const
//----------------------------------------------------------------------------
{
  return m_ResultQueryHandler->GetNumberOfFields();
}
//----------------------------------------------------------------------------
WebRowSetStringDataTable medRelationalDatabaseAbstractConnector::GetQueryResultAsStringMatrix() const
//----------------------------------------------------------------------------
{
  return m_ResultQueryHandler->GetResultAsStringMatrix();
}
//----------------------------------------------------------------------------
WebRowSetQueryObjectsTable medRelationalDatabaseAbstractConnector::GetQueryResultAsObjectsMatrix() const
//----------------------------------------------------------------------------
{
  return m_ResultQueryHandler->GetResultAsObjectsMatrix();
}
//----------------------------------------------------------------------------
WebRowSetColumnTypeVector medRelationalDatabaseAbstractConnector::GetColumnsTypeAsStringVector() const
//----------------------------------------------------------------------------
{
  return m_ResultQueryHandler->GetColumnsTypeInformationAsStringVector();
}
//----------------------------------------------------------------------------
WebRowSetColumnNameVector medRelationalDatabaseAbstractConnector::GetColumnsNameAsStringVector() const
//----------------------------------------------------------------------------
{
  return m_ResultQueryHandler->GetColumnsNameInformationAsStringVector();
}
//----------------------------------------------------------------------------
void medRelationalDatabaseAbstractConnector::SetRelationalDatabaseInformation(medRelationalDatabaseInformation info)
//----------------------------------------------------------------------------
{
  m_RelationalDatabaseInformation.m_Url = info.m_Url;
  m_RelationalDatabaseInformation.m_Username = info.m_Username;
  m_RelationalDatabaseInformation.m_Password = info.m_Password;
  m_RelationalDatabaseInformation.m_Certificate = info.m_Certificate;
  m_RelationalDatabaseInformation.m_ConfName = info.m_ConfName;
}
//----------------------------------------------------------------------------
const medRelationalDatabaseInformation &medRelationalDatabaseAbstractConnector::GetRelationalDatabaseInformation() const
//----------------------------------------------------------------------------
{
  return m_RelationalDatabaseInformation;
}