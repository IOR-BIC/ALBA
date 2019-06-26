/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewPlotTest
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_VMEPoseTest_H__
#define __CPP_UNIT_VMEPoseTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include "albaVME.h"
#include "albaVMEOutput.h"
#include "albaMatrixPipe.h"

//-------------------------------------------------------------------------
/** a simple VME created just for testing purposes.
    This VME stores pose matrices and data at integral time stamps. The data
    is represented by a single float number representing the extent of a symmetric
    object.*/
class albaVMETestHelper : public albaVME
{
public:
  albaTypeMacro(albaVMETestHelper,albaVME);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /**
    Compare two VME. Two VME are considered equivalent if they have equivalent 
    items, TagArrays, MatrixVectors, Name and Type. */
  virtual bool Equals(albaVME *vme);
 
  /**
    Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
    set or get the Pose for a specified time. When setting, if the time does not exist
    the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
    interpolates on the fly according to the matrix interpolator.*/
  void SetMatrix(const albaMatrix &mat);

  /** Set the data inside the internal data vector */
  void SetData(const double data, albaTimeStamp t) {m_DataVector[t]=data;}

  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrixes and VME items*/
  virtual void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes);

  std::vector<albaSmartPointer<albaMatrix> > &GetMatrixVector() {return m_MatrixVector;}

protected:
  albaVMETestHelper();

  std::vector<albaSmartPointer<albaMatrix> > m_MatrixVector;
  std::vector<double>                      m_DataVector;
};

//-------------------------------------------------------------------------
/** Simple VME output concrete class for test purposes.
  */
class albaVMETestOutputHelper : public albaVMEOutput
{
public:
  albaTypeMacro(albaVMETestOutputHelper,albaVMEOutput)
		  
protected:
  albaVMETestOutputHelper(){}; // to be allocated with New()
  virtual ~albaVMETestOutputHelper(){}; // to be deleted with Delete()

private:
  albaVMETestOutputHelper(const albaVMETestOutputHelper&); // Not implemented
  void operator=(const albaVMETestOutputHelper&); // Not implemented
};

//-------------------------------------------------------------------------
/** simple matrix pipe to produce output data! */
class ALBA_EXPORT albaMatrixPipeTestHelper:public albaMatrixPipe
{
public:
	albaTypeMacro(albaMatrixPipeTestHelper,albaMatrixPipe);

	/** This DataPipe accepts only VME's with internal DataArray. */
	virtual bool Accept(albaVME *vme) {return Superclass::Accept(vme)&&vme->IsALBAType(albaVMETestHelper);}

protected:
	/** update the output matrix */
	virtual void InternalUpdate();  
};

class VMEDataTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( VMEDataTest );
  CPPUNIT_TEST( VmeDataMainTest );
  CPPUNIT_TEST_SUITE_END();

protected:
  void VmeDataMainTest();
};

#endif