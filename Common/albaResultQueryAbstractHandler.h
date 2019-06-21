/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaResultQueryAbstractHandler
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaResultQueryAbstractHandler_H__
#define __albaResultQueryAbstractHandler_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaObject.h"
#include "albaQueryObject.h"
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

typedef std::vector<std::vector<albaQueryObject *> >   WebRowSetQueryObjectsTable;
/**
  class name: albaResultQueryAbstractHandler
  Interface for handle results from a query to a database
*/
class ALBA_EXPORT albaResultQueryAbstractHandler : public albaObject
{
public:
  /** constructor */
  albaResultQueryAbstractHandler();
  /** destructor */
	virtual ~albaResultQueryAbstractHandler(); 
  
  /** RTTI macro */
  albaAbstractTypeMacro(albaResultQueryAbstractHandler, albaObject);

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
#endif //__albaResultQueryAbstractHandler_H__
