/*=========================================================================

 Program: MAF2Medical
 Module: medResultQueryAbstractHandler
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medResultQueryAbstractHandler_H__
#define __medResultQueryAbstractHandler_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
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
class MAF_EXPORT medResultQueryAbstractHandler : public mafObject
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
