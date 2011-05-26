/*=========================================================================
Program:   @neufuse
Module:    $RCSfile: medResultQueryAbstractHandler.h,v $
Language:  C++
Date:      $Date: 2011-05-26 07:51:33 $
Version:   $Revision: 1.1.2.3 $
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
#include "medDefines.h"
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
/**
  class name: medResultQueryAbstractHandler
  Interface for handle results from a query to a database
*/
class MED_EXPORT medResultQueryAbstractHandler : public mafObject
{
public:
  /** constructor */
  medResultQueryAbstractHandler();
  /** destructor */
	virtual ~medResultQueryAbstractHandler(); 
  
  /** RTTI macro */
  mafAbstractTypeMacro(medResultQueryAbstractHandler, mafObject);

  /**Get result as string matrix */
  WebRowSetStringDataTable GetResultAsStringMatrix() {return m_MatrixStringResult;};
  
  /** Get column type as list of string */
  WebRowSetColumnTypeVector GetColumnsTypeInformationAsStringVector() {return m_ColumnsTypeInformation;};

  /** Get column name as list of string */
  WebRowSetColumnNameVector GetColumnsNameInformationAsStringVector() {return m_ColumnsNameInformation;};

  /** Get result as string matrix */
  WebRowSetQueryObjectsTable GetResultAsObjectsMatrix() {return m_MatrixObjectResult;};


  /** Get number of records (rows) */
  int GetNumberOfRecords() const;

  /** Get number of fields (columns) */
  int GetNumberOfFields() const;

  /** load result of Query */
  virtual void LoadQueryResult() = 0;

  /** load result of Query */
  virtual bool IsFailed() = 0;

protected:
  /** clear all the results */
  virtual void InternalResultReset();

  WebRowSetStringDataTable m_MatrixStringResult;
  WebRowSetColumnTypeVector m_ColumnsTypeInformation;
  WebRowSetColumnNameVector m_ColumnsNameInformation;

  WebRowSetQueryObjectsTable m_MatrixObjectResult;

private:
  


};
#endif //__medResultQueryAbstractHandler_H__
