/*=========================================================================

 Program: MAF2
 Module: mafOpMakeVMETimevaryingTest
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpMakeVMETimeVaryingTest.h"
#include "mafOpMakeVMETimevarying.h"

#include "mafDataVector.h"
#include "mafVMEImage.h"
#include "mafVMEItemVTK.h"

#include "vtkImageData.h"

#include <string>
#include <iostream>
#include <assert.h>


const int ITEMSNUMBER = 4;

class OpMakeVMETimevaryingDerivedTestClass: public mafOpMakeVMETimevarying
{
public:

  OpMakeVMETimevaryingDerivedTestClass(const wxString &label = "Make Timevariyng VME",bool showShadingPlane = false){};
  virtual ~OpMakeVMETimevaryingDerivedTestClass(){};

  mafTypeMacro(OpMakeVMETimevaryingDerivedTestClass, mafOpMakeVMETimevarying);

  bool AcceptTimeStamp(double timestamp){return Superclass::AcceptTimeStamp(timestamp);};
  bool AcceptVME(mafVME * vme){return Superclass::AcceptVME(vme);};
  void AddVME(mafVME * vme, double timestamp){Superclass::AddVME(vme, timestamp);};
  void DeleteVME(int index){Superclass::DeleteVME(index);};

  void Execute(){Superclass::Execute();};
};

//----------------------------------------------------------------------------
mafCxxTypeMacro(OpMakeVMETimevaryingDerivedTestClass);
//----------------------------------------------------------------------------

//-----------------------------------------------------------
void mafOpMakeVMETimevaryingTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpMakeVMETimevarying *op = new mafOpMakeVMETimevarying();
  cppDEL(op);
}
//-----------------------------------------------------------
void mafOpMakeVMETimevaryingTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  mafOpMakeVMETimevarying op;  
}

//-----------------------------------------------------------
void mafOpMakeVMETimevaryingTest::TestAccept()
//-----------------------------------------------------------
{
  mafOpMakeVMETimevarying *op = new mafOpMakeVMETimevarying();

  mafVMEImage *image;
  mafNEW(image);

  assert(op->Accept(image));

  mafDEL(image);
  cppDEL(op);
}

//-----------------------------------------------------------
void mafOpMakeVMETimevaryingTest::TestOpExecute()
//-----------------------------------------------------------
{

  OpMakeVMETimevaryingDerivedTestClass *op = new OpMakeVMETimevaryingDerivedTestClass();

  vtkImageData *imageData[ITEMSNUMBER];
  mafVMEImage *images[ITEMSNUMBER];

  int i;

  for(i=0;i<ITEMSNUMBER;i++)
  {
    imageData[i] = vtkImageData::New();
    imageData[i]->SetDimensions(i+1, i+1, 1);
    imageData[i]->SetOrigin(0, 0, 0);
    imageData[i]->SetSpacing(1,1,1);

    mafNEW(images[i]);

    int result = images[i]->SetData(imageData[i],0.0);
    if(!(result == MAF_OK))
      return;
    images[i]->Update();

  }

  double timestamps[ITEMSNUMBER];
  double delta = 0.1;

  for(i=0;i<ITEMSNUMBER;i++)
  {
    timestamps[i] = 0.0 + i*delta;
    if(op->AcceptVME(images[i]) && op->AcceptTimeStamp(timestamps[i]))
    {
      op->AddVME(images[i], timestamps[i]);
    }
    delta += 0.05;
  }

  op->Execute();

  mafVMEGeneric *resultVME = mafVMEGeneric::SafeDownCast(op->GetOutput());

  CPPUNIT_ASSERT(mafVMEImage::SafeDownCast(resultVME));

  //TIME BOUNDS
  mafTimeStamp bounds[2];
  bounds[0] = 0.0;
  bounds[1] = 0.0;

  resultVME->GetLocalTimeBounds(bounds);

  CPPUNIT_ASSERT(bounds[0] == timestamps[0] && bounds[1] == timestamps[ITEMSNUMBER-1]);

  CPPUNIT_ASSERT(resultVME->GetNumberOfTimeStamps() == ITEMSNUMBER);

  i=0;

  int dim[3], dimCopy[3];

  for (mafDataVector::Iterator it = resultVME->GetDataVector()->Begin(); it != resultVME->GetDataVector()->End(); it++)
  {
    mafVMEItemVTK *item = mafVMEItemVTK::SafeDownCast(it->second);
    assert(item);

    vtkImageData *outputVTK = vtkImageData::SafeDownCast(item->GetData());
    if(outputVTK)
    {
      imageData[i]->GetDimensions(dim);
      outputVTK->GetDimensions(dimCopy);
      CPPUNIT_ASSERT(dim[0] == dimCopy[0] && dim[1] == dimCopy[1] && dim[2] == dimCopy[2]);
    }

    i++;
  }

  for(i=0;i<ITEMSNUMBER;i++)
  {
    vtkDEL(imageData[i]);
    mafDEL(images[i]);
  }
  cppDEL(op);

}

//-----------------------------------------------------------
void mafOpMakeVMETimevaryingTest::TestOpUndo()
//-----------------------------------------------------------
{

}