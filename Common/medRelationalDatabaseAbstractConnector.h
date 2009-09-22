/*=========================================================================
Program:   @neufuse
Module:    $RCSfile: medRelationalDatabaseAbstractConnector.h,v $
Language:  C++
Date:      $Date: 2009-09-22 07:33:45 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2008
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#ifndef __medRelationalDatabaseAbstractConnector_H__
#define __medRelationalDatabaseAbstractConnector_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafObject.h"
#include "medResultQueryAbstractHandler.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class medResultQueryAbstractHandler;

struct medRelationalDatabaseInformation
{
  std::string m_Url;
  std::string m_Username;
  std::string m_Password;
  std::string m_Certificate;
  std::string m_ConfName;
};

class medRelationalDatabaseAbstractConnector : public mafObject
{
public:
  medRelationalDatabaseAbstractConnector(const char *query = "");
	virtual ~medRelationalDatabaseAbstractConnector(); 
  
  mafAbstractTypeMacro(medRelationalDatabaseAbstractConnector, mafObject);

  //Set Get information for connection to database
  void SetRelationalDatabaseInformation(medRelationalDatabaseInformation info);
  const medRelationalDatabaseInformation &GetRelationalDatabaseInformation() const;

  // Set the current query that will be used
  void SetQuery(const char* queryString);
  const char* GetQuery() const;

  //execute the query
  virtual void QueryExecute() = 0;

  //check the validation of execution
  virtual bool IsFailed() const;

  //return the number of records in terms of row
  int GetNumberOfRecords() const;

  //return the number of records in terms of row
  int GetNumberOfFields() const;

  //retrieve result in form of string matrix, 
  //as WebRowSetStringDataTable (typedef std::vector<std::vector<std::string> >)
  WebRowSetStringDataTable GetQueryResultAsStringMatrix() const;

  //retrieve result in form of string matrix, 
  //as WebRowSetColumnTypeVector (typedef std::vector<std::string >)
  WebRowSetColumnTypeVector GetColumnsTypeAsStringVector() const;

  //retrieve result in form of string matrix, 
  //as WebRowSetColumnNameVector (typedef std::vector<std::string >)
  WebRowSetColumnNameVector GetColumnsNameAsStringVector() const;

  //retrieve result in form of object matrix, 
  //as WebRowSetQueryObjectsTable (typedef std::vector<std::vector<medQueryObject> >)
  WebRowSetQueryObjectsTable GetQueryResultAsObjectsMatrix() const;

protected:
  medResultQueryAbstractHandler *m_ResultQueryHandler;

private:
  std::string m_Query;
  medRelationalDatabaseInformation m_RelationalDatabaseInformation;

};
#endif //__medRelationalDatabaseAbstractConnector_H__