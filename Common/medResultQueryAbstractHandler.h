/*=========================================================================
Program:   @neufuse
Module:    $RCSfile: medResultQueryAbstractHandler.h,v $
Language:  C++
Date:      $Date: 2009-09-22 07:33:45 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2008
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#ifndef __medResultQueryAbstractHandler_H__
#define __medResultQueryAbstractHandler_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafObject.h"
#include "medQueryObject.h"
#include <vector>
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// typedefs :
//----------------------------------------------------------------------------

typedef std::vector<std::vector<std::string> > WebRowSetStringDataTable;
typedef std::vector<std::string>               WebRowSetColumnTypeVector;
typedef std::vector<std::string>               WebRowSetColumnNameVector;

typedef std::vector<std::vector<medQueryObject *> >   WebRowSetQueryObjectsTable;

class medResultQueryAbstractHandler : public mafObject
{
public:
  medResultQueryAbstractHandler();
	virtual ~medResultQueryAbstractHandler(); 
  
  mafAbstractTypeMacro(medResultQueryAbstractHandler, mafObject);

  //Get result as string matrix
  WebRowSetStringDataTable GetResultAsStringMatrix() {return m_MatrixStringResult;};
  
  //Get column type as list of string
  WebRowSetColumnTypeVector GetColumnsTypeInformationAsStringVector() {return m_ColumnsTypeInformation;};

  //Get column name as list of string
  WebRowSetColumnNameVector GetColumnsNameInformationAsStringVector() {return m_ColumnsNameInformation;};

  //Get result as string matrix
  WebRowSetQueryObjectsTable GetResultAsObjectsMatrix() {return m_MatrixObjectResult;};


  //Get number of records (rows)
  int GetNumberOfRecords() const;

  //Get number of fields (columns)
  int GetNumberOfFields() const;

  //load result of Query
  virtual void LoadQueryResult() = 0;

  //load result of Query
  virtual bool IsFailed() = 0;

protected:
  //clear all the results
  virtual void InternalResultReset();

  WebRowSetStringDataTable m_MatrixStringResult;
  WebRowSetColumnTypeVector m_ColumnsTypeInformation;
  WebRowSetColumnNameVector m_ColumnsNameInformation;

  WebRowSetQueryObjectsTable m_MatrixObjectResult;

private:
  


};
#endif //__medResultQueryAbstractHandler_H__
