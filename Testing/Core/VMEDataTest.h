/*=========================================================================

 Program: MAF2
 Module: mafViewPlotTest
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

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
#include "mafVME.h"
#include "mafVMEOutput.h"
#include "mafMatrixPipe.h"

//-------------------------------------------------------------------------
/** a simple VME created just for testing purposes.
    This VME stores pose matrices and data at integral time stamps. The data
    is represented by a single float number representing the extent of a symmetric
    object.*/
class mafVMETestHelper : public mafVME
{
public:
  mafTypeMacro(mafVMETestHelper,mafVME);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /**
    Compare two VME. Two VME are considered equivalent if they have equivalent 
    items, TagArrays, MatrixVectors, Name and Type. */
  virtual bool Equals(mafVME *vme);
 
  /**
    Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
    set or get the Pose for a specified time. When setting, if the time does not exist
    the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
    interpolates on the fly according to the matrix interpolator.*/
  void SetMatrix(const mafMatrix &mat);

  /** Set the data inside the internal data vector */
  void SetData(const double data, mafTimeStamp t) {m_DataVector[t]=data;}

  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrixes and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  std::vector<mafSmartPointer<mafMatrix> > &GetMatrixVector() {return m_MatrixVector;}

protected:
  mafVMETestHelper();

  std::vector<mafSmartPointer<mafMatrix> > m_MatrixVector;
  std::vector<double>                      m_DataVector;
};

//-------------------------------------------------------------------------
/** Simple VME output concrete class for test purposes.
  */
class mafVMETestOutputHelper : public mafVMEOutput
{
public:
  mafTypeMacro(mafVMETestOutputHelper,mafVMEOutput)
		  
protected:
  mafVMETestOutputHelper(){}; // to be allocated with New()
  virtual ~mafVMETestOutputHelper(){}; // to be deleted with Delete()

private:
  mafVMETestOutputHelper(const mafVMETestOutputHelper&); // Not implemented
  void operator=(const mafVMETestOutputHelper&); // Not implemented
};

//-------------------------------------------------------------------------
/** simple matrix pipe to produce output data! */
class MAF_EXPORT mafMatrixPipeTestHelper:public mafMatrixPipe
{
public:
	mafTypeMacro(mafMatrixPipeTestHelper,mafMatrixPipe);

	/** This DataPipe accepts only VME's with internal DataArray. */
	virtual bool Accept(mafVME *vme) {return Superclass::Accept(vme)&&vme->IsMAFType(mafVMETestHelper);}

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