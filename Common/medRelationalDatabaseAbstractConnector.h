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

#ifndef __medRelationalDatabaseAbstractConnector_H__
#define __medRelationalDatabaseAbstractConnector_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafObject.h"
#include "medResultQueryAbstractHandler.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class medResultQueryAbstractHandler;
/**
  struct name :medRelationalDatabaseInformation
  struct that contains database information useful for connection
*/
struct medRelationalDatabaseInformation
{
  std::string m_Url;
  std::string m_Username;
  std::string m_Password;
  std::string m_Certificate;
  std::string m_ConfName;
};
/**
  class name : medRelationalDatabaseAbstractConnector
  class that handle all the process of connection, retrieving data executing query
  ad by medResultQueryAbstractHandler load them
*/
class MAF_EXPORT medRelationalDatabaseAbstractConnector : public mafObject
{
public:
  /** constructor */
  medRelationalDatabaseAbstractConnector(const char *query = "");
  /** destructor */
	virtual ~medRelationalDatabaseAbstractConnector(); 
  
  /** RTTI macro */
  mafAbstractTypeMacro(medRelationalDatabaseAbstractConnector, mafObject);

  /** Set information for connection to database */
  void SetRelationalDatabaseInformation(medRelationalDatabaseInformation info);
  /** Get information for connection to database */
  const medRelationalDatabaseInformation &GetRelationalDatabaseInformation() const;

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
  as WebRowSetQueryObjectsTable (typedef std::vector<std::vector<medQueryObject> >)*/
  WebRowSetQueryObjectsTable GetQueryResultAsObjectsMatrix() const;

protected:
  medResultQueryAbstractHandler *m_ResultQueryHandler;

private:
  std::string m_Query;
  medRelationalDatabaseInformation m_RelationalDatabaseInformation;

};
#endif //__medRelationalDatabaseAbstractConnector_H__