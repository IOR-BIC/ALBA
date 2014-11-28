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

#ifndef __mafRelationalDatabaseAbstractConnector_H__
#define __mafRelationalDatabaseAbstractConnector_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafObject.h"
#include "mafResultQueryAbstractHandler.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafResultQueryAbstractHandler;
/**
  struct name :mafRelationalDatabaseInformation
  struct that contains database information useful for connection
*/
struct mafRelationalDatabaseInformation
{
  std::string m_Url;
  std::string m_Username;
  std::string m_Password;
  std::string m_Certificate;
  std::string m_ConfName;
};
/**
  class name : mafRelationalDatabaseAbstractConnector
  class that handle all the process of connection, retrieving data executing query
  ad by mafResultQueryAbstractHandler load them
*/
class MAF_EXPORT mafRelationalDatabaseAbstractConnector : public mafObject
{
public:
  /** constructor */
  mafRelationalDatabaseAbstractConnector(const char *query = "");
  /** destructor */
	virtual ~mafRelationalDatabaseAbstractConnector(); 
  
  /** RTTI macro */
  mafAbstractTypeMacro(mafRelationalDatabaseAbstractConnector, mafObject);

  /** Set information for connection to database */
  void SetRelationalDatabaseInformation(mafRelationalDatabaseInformation info);
  /** Get information for connection to database */
  const mafRelationalDatabaseInformation &GetRelationalDatabaseInformation() const;

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
  as WebRowSetQueryObjectsTable (typedef std::vector<std::vector<mafQueryObject> >)*/
  WebRowSetQueryObjectsTable GetQueryResultAsObjectsMatrix() const;

protected:
  mafResultQueryAbstractHandler *m_ResultQueryHandler;

private:
  std::string m_Query;
  mafRelationalDatabaseInformation m_RelationalDatabaseInformation;

};
#endif //__mafRelationalDatabaseAbstractConnector_H__