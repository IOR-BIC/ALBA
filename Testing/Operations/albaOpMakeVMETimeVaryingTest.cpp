/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMakeVMETimevaryingTest
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaOpMakeVMETimeVaryingTest.h"
#include "albaOpMakeVMETimevarying.h"

#include "albaDataVector.h"
#include "albaVMEImage.h"
#include "albaVMEItemVTK.h"

#include "vtkImageData.h"

#include <string>
#include <iostream>
#include <assert.h>


const int ITEMSNUMBER = 4;

class OpMakeVMETimevaryingDerivedTestClass: public albaOpMakeVMETimevarying
{
public:

  OpMakeVMETimevaryingDerivedTestClass(const wxString &label = "Make Timevariyng VME",bool showShadingPlane = false){};
  virtual ~OpMakeVMETimevaryingDerivedTestClass(){};

  albaTypeMacro(OpMakeVMETimevaryingDerivedTestClass, albaOpMakeVMETimevarying);

  bool AcceptTimeStamp(double timestamp){return Superclass::AcceptTimeStamp(timestamp);};
  bool AcceptVME(albaVME * vme){return Superclass::AcceptVME(vme);};
  void AddVME(albaVME * vme, double timestamp){Superclass::AddVME(vme, timestamp);};
  void DeleteVME(int index){Superclass::DeleteVME(index);};

  void Execute(){Superclass::Execute();};
};

//----------------------------------------------------------------------------
albaCxxTypeMacro(OpMakeVMETimevaryingDerivedTestClass);
//----------------------------------------------------------------------------

//-----------------------------------------------------------
void albaOpMakeVMETimevaryingTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpMakeVMETimevarying *op = new albaOpMakeVMETimevarying();
  cppDEL(op);
}
//-----------------------------------------------------------
void albaOpMakeVMETimevaryingTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  albaOpMakeVMETimevarying op;  
}

//-----------------------------------------------------------
void albaOpMakeVMETimevaryingTest::TestAccept()
//-----------------------------------------------------------
{
  albaOpMakeVMETimevarying *op = new albaOpMakeVMETimevarying();

  albaVMEImage *image;
  albaNEW(image);

  assert(op->Accept(image));

  albaDEL(image);
  cppDEL(op);
}

//-----------------------------------------------------------
void albaOpMakeVMETimevaryingTest::TestOpExecute()
//-----------------------------------------------------------
{

  OpMakeVMETimevaryingDerivedTestClass *op = new OpMakeVMETimevaryingDerivedTestClass();

  vtkImageData *imageData[ITEMSNUMBER];
  albaVMEImage *images[ITEMSNUMBER];

  int i;

  for(i=0;i<ITEMSNUMBER;i++)
  {
    imageData[i] = vtkImageData::New();
    imageData[i]->SetDimensions(i+1, i+1, 1);
    imageData[i]->SetOrigin(0, 0, 0);
    imageData[i]->SetSpacing(1,1,1);

    albaNEW(images[i]);

    int result = images[i]->SetData(imageData[i],0.0);
    if(!(result == ALBA_OK))
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

  albaVMEGeneric *resultVME = albaVMEGeneric::SafeDownCast(op->GetOutput());

  CPPUNIT_ASSERT(albaVMEImage::SafeDownCast(resultVME));

  //TIME BOUNDS
  albaTimeStamp bounds[2];
  bounds[0] = 0.0;
  bounds[1] = 0.0;

  resultVME->GetLocalTimeBounds(bounds);

  CPPUNIT_ASSERT(bounds[0] == timestamps[0] && bounds[1] == timestamps[ITEMSNUMBER-1]);

  CPPUNIT_ASSERT(resultVME->GetNumberOfTimeStamps() == ITEMSNUMBER);

  i=0;

  int dim[3], dimCopy[3];

  for (albaDataVector::Iterator it = resultVME->GetDataVector()->Begin(); it != resultVME->GetDataVector()->End(); it++)
  {
    albaVMEItemVTK *item = albaVMEItemVTK::SafeDownCast(it->second);
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
    albaDEL(images[i]);
  }
  cppDEL(op);

}

//-----------------------------------------------------------
void albaOpMakeVMETimevaryingTest::TestOpUndo()
//-----------------------------------------------------------
{

}