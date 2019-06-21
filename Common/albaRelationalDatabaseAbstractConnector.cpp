/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaRelationalDatabaseAbstractConnector
 Authors: Daniele Giunchi
 
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

#include "albaRelationalDatabaseAbstractConnector.h"

//----------------------------------------------------------------------------
albaCxxAbstractTypeMacro(albaRelationalDatabaseAbstractConnector);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaRelationalDatabaseAbstractConnector::albaRelationalDatabaseAbstractConnector(const char *queryString)
:m_Query(queryString),
m_ResultQueryHandler(NULL)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
albaRelationalDatabaseAbstractConnector::~albaRelationalDatabaseAbstractConnector()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaRelationalDatabaseAbstractConnector::SetQuery(const char* queryString)
//----------------------------------------------------------------------------
{
  m_Query = queryString;
}
//----------------------------------------------------------------------------
const char* albaRelationalDatabaseAbstractConnector::GetQuery() const
//----------------------------------------------------------------------------
{
  return m_Query.c_str();
}
//----------------------------------------------------------------------------
bool albaRelationalDatabaseAbstractConnector::IsFailed() const
//----------------------------------------------------------------------------
{ 
  return m_ResultQueryHandler->IsFailed();
}
//----------------------------------------------------------------------------
int albaRelationalDatabaseAbstractConnector::GetNumberOfRecords() const
//----------------------------------------------------------------------------
{
  return m_ResultQueryHandler->GetNumberOfRecords();
}
//----------------------------------------------------------------------------
int albaRelationalDatabaseAbstractConnector::GetNumberOfFields() const
//----------------------------------------------------------------------------
{
  return m_ResultQueryHandler->GetNumberOfFields();
}
//----------------------------------------------------------------------------
WebRowSetStringDataTable albaRelationalDatabaseAbstractConnector::GetQueryResultAsStringMatrix() const
//----------------------------------------------------------------------------
{
  return m_ResultQueryHandler->GetResultAsStringMatrix();
}
//----------------------------------------------------------------------------
WebRowSetQueryObjectsTable albaRelationalDatabaseAbstractConnector::GetQueryResultAsObjectsMatrix() const
//----------------------------------------------------------------------------
{
  return m_ResultQueryHandler->GetResultAsObjectsMatrix();
}
//----------------------------------------------------------------------------
WebRowSetColumnTypeVector albaRelationalDatabaseAbstractConnector::GetColumnsTypeAsStringVector() const
//----------------------------------------------------------------------------
{
  return m_ResultQueryHandler->GetColumnsTypeInformationAsStringVector();
}
//----------------------------------------------------------------------------
WebRowSetColumnNameVector albaRelationalDatabaseAbstractConnector::GetColumnsNameAsStringVector() const
//----------------------------------------------------------------------------
{
  return m_ResultQueryHandler->GetColumnsNameInformationAsStringVector();
}
//----------------------------------------------------------------------------
void albaRelationalDatabaseAbstractConnector::SetRelationalDatabaseInformation(albaRelationalDatabaseInformation info)
//----------------------------------------------------------------------------
{
  m_RelationalDatabaseInformation.m_Url = info.m_Url;
  m_RelationalDatabaseInformation.m_Username = info.m_Username;
  m_RelationalDatabaseInformation.m_Password = info.m_Password;
  m_RelationalDatabaseInformation.m_Certificate = info.m_Certificate;
  m_RelationalDatabaseInformation.m_ConfName = info.m_ConfName;
}
//----------------------------------------------------------------------------
const albaRelationalDatabaseInformation &albaRelationalDatabaseAbstractConnector::GetRelationalDatabaseInformation() const
//----------------------------------------------------------------------------
{
  return m_RelationalDatabaseInformation;
}