/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVectorTest
 Authors: Daniele Giunchi
 
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

#include <cppunit/config/SourcePrefix.h>
#include "albaBaseTests.h"
#include "albaVector.h"
#include "albaVectorTest.h"
#include "albaReferenceCounted.h"
#include "albaSmartPointer.h"


#include <iostream>
#include <utility>


//----------------------------------------------------------------------------
void albaVectorTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaVectorTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaVector<int> oarray;
  for(int i=0;i<10;i++)
  {
    oarray.Push(i);
  }
}
//----------------------------------------------------------------------------
void albaVectorTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaVector<albaTestObject *> oarray;
  
  for(int i=0; i<10;i++)
  {
    albaTestObject *obj = albaTestObject::New();
    oarray.Push(obj);
  }

  for(int i=0; i<10;i++)
  {
    delete oarray[i];
  }
  
}
//----------------------------------------------------------------------------
void albaVectorTest::TestVectorAPICombination()
//----------------------------------------------------------------------------
{
  int n[10];
  albaVector<albaAutoPointer<albaTestObject> > oarray;

  // Test with an array of smart pointers
  int i;
  for (i=0;i<10;i++)
  {
    n[i]=i;
    albaSmartPointer<albaTestObject> obj;
    obj->Number=&(n[i]);
    oarray.SetItem(i,obj);
  }

  CPPUNIT_ASSERT(oarray.GetNumberOfItems()==10);

  CPPUNIT_ASSERT(oarray[10]==albaVector<albaAutoPointer<albaTestObject> >::m_NullItem);

  for (i=0;i<10;i++)
  {
    CPPUNIT_ASSERT(*(oarray[i]->Number) == i);
    CPPUNIT_ASSERT(oarray[i]);
    CPPUNIT_ASSERT(oarray[i]->GetReferenceCount()==1);
  }

  CPPUNIT_ASSERT(oarray[1].GetPointer()!=NULL);

  oarray.RemoveItem(3);

  CPPUNIT_ASSERT(oarray.GetNumberOfItems()==9);

  for (i=0;i<9;i++)
  {
    CPPUNIT_ASSERT(oarray[i]->GetReferenceCount()==1);
  }

  

  // pop last object into a tmp container
  albaAutoPointer<albaTestObject> tmp;
  oarray.Pop(tmp);

  CPPUNIT_ASSERT(oarray.GetNumberOfItems()==8);
  CPPUNIT_ASSERT(tmp->GetReferenceCount()==1);

  // check the object is still alive
  CPPUNIT_ASSERT(n[9]);
  tmp.UnRegister(NULL);
  CPPUNIT_ASSERT(n[9] == 0);

  //append
  albaSmartPointer<albaTestObject> obj;
  int value = 10;
  obj->Number=&(value);
  int id = oarray.AppendItem(obj);

  CPPUNIT_ASSERT(oarray.GetNumberOfItems()==9);
  CPPUNIT_ASSERT(*(oarray[id]->Number) == 10);

  //replace
  albaSmartPointer<albaTestObject> objr;
  n[8] = 9;
  objr->Number=&(n[8]);
  oarray.ReplaceItem(8,objr);

  CPPUNIT_ASSERT(oarray.GetNumberOfItems()==9);
  CPPUNIT_ASSERT(*(oarray[8]->Number) == 9);
  
  //IsItemPresent (true)
  CPPUNIT_ASSERT(oarray.IsItemPresent(objr));

  //getItem(1)
  albaSmartPointer<albaTestObject> objG1;
  oarray.GetItem(oarray.GetNumberOfItems()-1, objG1);
  CPPUNIT_ASSERT(*(objG1->Number) == *(objr->Number));

  //getItem(2)
  albaSmartPointer<albaTestObject> objG2;
  objG2 = oarray.GetItem(oarray.GetNumberOfItems()-1);
  CPPUNIT_ASSERT(*(objG2->Number) == *(objr->Number));


  //find item (true)
  albaID idFound = -1;
  bool result;
  result = oarray.FindItem(objr , idFound);
  CPPUNIT_ASSERT(result && idFound == oarray.GetNumberOfItems()-1);
  
  //find item (false)
  albaSmartPointer<albaTestObject> objF;
  int valueF = 50;
  albaID idNotFound = -1;
  objF->Number=&(value);
  result = oarray.FindItem(objF , idNotFound);
  CPPUNIT_ASSERT(!result);
  CPPUNIT_ASSERT(idNotFound == -1);
  
  //push
  albaSmartPointer<albaTestObject> objP;
  int valueP = 60;
  objP->Number=&(valueP);
  int idP = oarray.Push(objP);

  CPPUNIT_ASSERT(oarray.GetNumberOfItems()==10);
  CPPUNIT_ASSERT(*(oarray[idP]->Number) == 60);

  //find remove
  oarray.FindAndRemoveItem(objr);

  CPPUNIT_ASSERT(oarray.GetNumberOfItems()==9);
  CPPUNIT_ASSERT(oarray[9] == albaVector<albaAutoPointer<albaTestObject> >::m_NullItem);

  //operator [] assign and retrieve
  albaSmartPointer<albaTestObject> objOp;
  int valueOp = -5;
  objOp->Number=&(valueOp);

  oarray[0] = objOp;
  CPPUNIT_ASSERT(oarray.GetNumberOfItems()==9);
  CPPUNIT_ASSERT(*(oarray[0]->Number) == -5);

  //IsItemPresent (false)
  CPPUNIT_ASSERT(!oarray.IsItemPresent(objr));


  // remove all objects
  oarray.RemoveAllItems();

  // check all the objects has been released
  for (i=0;i<10;i++)
  {
    CPPUNIT_ASSERT(n[i] == 0);
  }

  CPPUNIT_ASSERT(!oarray.Pop(tmp));
}

//----------------------------------------------------------------------------
void albaVectorTest::TestGetNumberOfItems()
//----------------------------------------------------------------------------
{
  int n[10];
  albaVector<albaAutoPointer<albaTestObject> > oarray;

  // Test with an array of smart pointers
  int i;
  for (i=0;i<10;i++)
  {
    n[i]=i;
    albaSmartPointer<albaTestObject> obj;
    obj->Number=&(n[i]);
    oarray.SetItem(i,obj);
  }

  CPPUNIT_ASSERT(oarray.GetNumberOfItems()==10);
}
//----------------------------------------------------------------------------
void albaVectorTest::TestRemoveItem()
//----------------------------------------------------------------------------
{
  int n[10];
  albaVector<albaAutoPointer<albaTestObject> > oarray;

  // Test with an array of smart pointers
  int i;
  for (i=0;i<10;i++)
  {
    n[i]=i;
    albaSmartPointer<albaTestObject> obj;
    obj->Number=&(n[i]);
    oarray.SetItem(i,obj);
  }
}
//----------------------------------------------------------------------------
void albaVectorTest::TestAppendItem()
//----------------------------------------------------------------------------
{
  int n[10];
  albaVector<albaAutoPointer<albaTestObject> > oarray;

  // Test with an array of smart pointers
  int i;
  for (i=0;i<10;i++)
  {
    n[i]=i;
    albaSmartPointer<albaTestObject> obj;
    obj->Number=&(n[i]);
    oarray.SetItem(i,obj);
  }

  albaSmartPointer<albaTestObject> obj;
  int value = 100;
  obj->Number=&(value);
  int id = oarray.AppendItem(obj);

  CPPUNIT_ASSERT(oarray.GetNumberOfItems()==11);
  CPPUNIT_ASSERT(*(oarray[id]->Number) == 100);
}
//----------------------------------------------------------------------------
void albaVectorTest::TestReplaceItem()
//----------------------------------------------------------------------------
{
  int n[10];
  albaVector<albaAutoPointer<albaTestObject> > oarray;

  // Test with an array of smart pointers
  int i;
  for (i=0;i<10;i++)
  {
    n[i]=i;
    albaSmartPointer<albaTestObject> obj;
    obj->Number=&(n[i]);
    oarray.SetItem(i,obj);
  }

  albaSmartPointer<albaTestObject> objr;
  int value = 100;
  objr->Number=&(value);
  oarray.ReplaceItem(8,objr);

  CPPUNIT_ASSERT(*(oarray[8]->Number) == 100);
}
//----------------------------------------------------------------------------
void albaVectorTest::TestIsItemPresent()
//----------------------------------------------------------------------------
{
  int n[10];
  albaVector<albaAutoPointer<albaTestObject> > oarray;

  // Test with an array of smart pointers
  int i;
  albaSmartPointer<albaTestObject> objI;
  albaSmartPointer<albaTestObject> objN;
  int value = 1000;
  objN->Number=&(value);

  for (i=0;i<10;i++)
  {
    n[i]=i;
    albaSmartPointer<albaTestObject> obj;
    obj->Number=&(n[i]);
    if(i == 0)
      objI = obj;
    oarray.SetItem(i,obj);
  }

  CPPUNIT_ASSERT(oarray.IsItemPresent(objI));
  CPPUNIT_ASSERT(!oarray.IsItemPresent(objN)); //not present
}
//----------------------------------------------------------------------------
void albaVectorTest::TestGetSetItem()
//----------------------------------------------------------------------------
{
  int n[10];
  albaVector<albaAutoPointer<albaTestObject> > oarray;
 
  // Test with an array of smart pointers
  int i;
  
  for (i=0;i<10;i++)
  {
    n[i]=i;
    albaSmartPointer<albaTestObject> obj;
    obj->Number=&(n[i]);
    oarray.SetItem(i,obj);
  }

  //getItem(1)
  albaSmartPointer<albaTestObject> objG1;
  oarray.GetItem(oarray.GetNumberOfItems()-1, objG1);
  CPPUNIT_ASSERT(*(objG1->Number) == 9);

  //getItem(2)
  albaSmartPointer<albaTestObject> objG2;
  objG2 = oarray.GetItem(oarray.GetNumberOfItems()-1);
  CPPUNIT_ASSERT(*(objG2->Number) == 9);
}
//----------------------------------------------------------------------------
void albaVectorTest::TestFindItem()
//----------------------------------------------------------------------------
{
  int n[10];
  albaVector<albaAutoPointer<albaTestObject> > oarray;

  // Test with an array of smart pointers
  int i;
  albaSmartPointer<albaTestObject> objF;
  for (i=0;i<10;i++)
  {
    n[i]=i;
    albaSmartPointer<albaTestObject> obj;
    obj->Number=&(n[i]);
    if(i==0)
      objF = obj;
    oarray.SetItem(i,obj);
  }

  //find item (true)
  albaID idFound = -1;
  bool result;
  result = oarray.FindItem(objF , idFound);
  CPPUNIT_ASSERT(result && idFound == 0);

  //find item (false)
  albaSmartPointer<albaTestObject> objN;
  int valueF = 50;
  albaID idNotFound = -1;
  objN->Number=&(valueF);
  result = oarray.FindItem(objN , idNotFound);
  CPPUNIT_ASSERT(!result);
  CPPUNIT_ASSERT(idNotFound == -1);
}
//----------------------------------------------------------------------------
void albaVectorTest::TestPush()
//----------------------------------------------------------------------------
{
  int n[10];
  albaVector<albaAutoPointer<albaTestObject> > oarray;

  // Test with an array of smart pointers
  int i;
  for (i=0;i<10;i++)
  {
    n[i]=i;
    albaSmartPointer<albaTestObject> obj;
    obj->Number=&(n[i]);
    oarray.SetItem(i,obj);
  }

  albaSmartPointer<albaTestObject> objP;
  int valueP = 60;
  objP->Number=&(valueP);
  int idP = oarray.Push(objP);

  CPPUNIT_ASSERT(oarray.GetNumberOfItems()==11);
  CPPUNIT_ASSERT(*(oarray[idP]->Number) == 60);
}
//----------------------------------------------------------------------------
void albaVectorTest::TestFindAndRemoveItem()
//----------------------------------------------------------------------------
{
  int n[10];
  albaVector<albaAutoPointer<albaTestObject> > oarray;

  // Test with an array of smart pointers
  int i;
  albaSmartPointer<albaTestObject> objFR;
  for (i=0;i<10;i++)
  {
    n[i]=i;
    albaSmartPointer<albaTestObject> obj;
    obj->Number=&(n[i]);
    if(i==9)
      objFR = obj;
    oarray.SetItem(i,obj);
  }

  //find remove
  oarray.FindAndRemoveItem(objFR);

  CPPUNIT_ASSERT(oarray.GetNumberOfItems()==9);
  CPPUNIT_ASSERT(oarray[9] == albaVector<albaAutoPointer<albaTestObject> >::m_NullItem);
}
//----------------------------------------------------------------------------
void albaVectorTest::TestRemoveAllItems()
//----------------------------------------------------------------------------
{
  int n[10];
  albaVector<albaAutoPointer<albaTestObject> > oarray;

  // Test with an array of smart pointers
  int i;
  for (i=0;i<10;i++)
  {
    n[i]=i;
    albaSmartPointer<albaTestObject> obj;
    obj->Number=&(n[i]);
    oarray.SetItem(i,obj);
  }

  // remove all objects
  oarray.RemoveAllItems();
  CPPUNIT_ASSERT(oarray.GetNumberOfItems()==0);
}
//----------------------------------------------------------------------------
void albaVectorTest::TestPop()
//----------------------------------------------------------------------------
{
  int n[10];
  albaVector<albaAutoPointer<albaTestObject> > oarray;

  // Test with an array of smart pointers
  int i;
  for (i=0;i<10;i++)
  {
    n[i]=i;
    albaSmartPointer<albaTestObject> obj;
    obj->Number=&(n[i]);
    oarray.SetItem(i,obj);
  }

  albaAutoPointer<albaTestObject> tmp;
  oarray.Pop(tmp);

  CPPUNIT_ASSERT(oarray.GetNumberOfItems()==9);
  CPPUNIT_ASSERT(tmp->GetReferenceCount()==1);
  CPPUNIT_ASSERT(*(tmp->Number)==9);
}