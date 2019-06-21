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

#ifndef __albaRelationalDatabaseAbstractConnector_H__
#define __albaRelationalDatabaseAbstractConnector_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaObject.h"
#include "albaResultQueryAbstractHandler.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaResultQueryAbstractHandler;
/**
  struct name :albaRelationalDatabaseInformation
  struct that contains database information useful for connection
*/
struct albaRelationalDatabaseInformation
{
  std::string m_Url;
  std::string m_Username;
  std::string m_Password;
  std::string m_Certificate;
  std::string m_ConfName;
};
/**
  class name : albaRelationalDatabaseAbstractConnector
  class that handle all the process of connection, retrieving data executing query
  ad by albaResultQueryAbstractHandler load them
*/
class ALBA_EXPORT albaRelationalDatabaseAbstractConnector : public albaObject
{
public:
  /** constructor */
  albaRelationalDatabaseAbstractConnector(const char *query = "");
  /** destructor */
	virtual ~albaRelationalDatabaseAbstractConnector(); 
  
  /** RTTI macro */
  albaAbstractTypeMacro(albaRelationalDatabaseAbstractConnector, albaObject);

  /** Set information for connection to database */
  void SetRelationalDatabaseInformation(albaRelationalDatabaseInformation info);
  /** Get information for connection to database */
  const albaRelationalDatabaseInformation &GetRelationalDatabaseInformation() const;

  /** Set the current query that will be used */
  void SetQuery(const char* queryString);
  /** Get the current query that will be used */
  const char* GetQuery() const;

  /** execute the query */
  virtual void QueryExecute() = 0;

  /** check the validation of execution */
  virtual bool IsFailed() const;

  /** return the number of records in terms of row */
  int GetNumberOfRecords() const;

  /** return the number of records in terms of row */
  int GetNumberOfFields() const;

  /** retrieve result in form of string matrix, 
  as WebRowSetStringDataTable (typedef std::vector<std::vector<std::string> >)*/
  WebRowSetStringDataTable GetQueryResultAsStringMatrix() const;

  /** retrieve result in form of string matrix, 
  as WebRowSetColumnTypeVector (typedef std::vector<std::string >)*/
  WebRowSetColumnTypeVector GetColumnsTypeAsStringVector() const;

  /** retrieve result in form of string matrix, 
  as WebRowSetColumnNameVector (typedef std::vector<std::string >)*/
  WebRowSetColumnNameVector GetColumnsNameAsStringVector() const;

  /** retrieve result in form of object matrix, 
  as WebRowSetQueryObjectsTable (typedef std::vector<std::vector<albaQueryObject> >)*/
  WebRowSetQueryObjectsTable GetQueryResultAsObjectsMatrix() const;

protected:
  albaResultQueryAbstractHandler *m_ResultQueryHandler;

private:
  std::string m_Query;
  albaRelationalDatabaseInformation m_RelationalDatabaseInformation;

};
#endif //__albaRelationalDatabaseAbstractConnector_H__