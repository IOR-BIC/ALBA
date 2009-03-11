/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMEFactoryTest.cpp,v $
Language:  C++
Date:      $Date: 2009-03-11 14:53:24 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "medVMEFactoryTest.h"

#include "mafVME.h"
#include "medVMEFactory.h"
#include "mafNodeFactory.h"

#include "mafVersion.h"
#include <iostream>
#include <algorithm>


//-------------------------------------------------------------------------
/** a simple VME created just for testing purposes. */
class medVMECustom : public mafVME
//-------------------------------------------------------------------------
{
public:
  mafTypeMacro(medVMECustom,mafVME);
  /*virtual*/ void SetMatrix(const mafMatrix &mat){};
  /*virtual*/ void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes){};

protected:
  medVMECustom();
  virtual ~medVMECustom();

private:
  medVMECustom(const medVMECustom&); // Not implemented
  void operator=(const medVMECustom&); // Not implemented
};

//-------------------------------------------------------------------------
mafCxxTypeMacro(medVMECustom);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
medVMECustom::medVMECustom()
//-------------------------------------------------------------------------
{
  
}
//-------------------------------------------------------------------------
medVMECustom::~medVMECustom()
//-------------------------------------------------------------------------
{
  
}
//-------------------------------------------------------------------------
void medVMEFactoryTest::TestGetInstance()
//-------------------------------------------------------------------------
{
  medVMEFactory *vme_factory = NULL;
  vme_factory = (medVMEFactory*) medVMEFactory::GetInstance();
  CPPUNIT_ASSERT(vme_factory != NULL);
}
//-------------------------------------------------------------------------
void medVMEFactoryTest::TestInitialize()
//-------------------------------------------------------------------------
{
  // this custom factory needs only to be initialized
  CPPUNIT_ASSERT(medVMEFactory::Initialize()==MAF_OK);
}
//-------------------------------------------------------------------------
void medVMEFactoryTest::CreateVMEInstance()
//-------------------------------------------------------------------------
{
  // this custom factory needs only to be initialized
  CPPUNIT_ASSERT(medVMEFactory::Initialize()==MAF_OK);
  
  std::list<mafObjectFactory *> list=mafObjectFactory::GetRegisteredFactories();
  CPPUNIT_ASSERT(list.size()==1);

  medVMEFactory *vme_factory = NULL;
  vme_factory = (medVMEFactory*) medVMEFactory::GetInstance();

  mafPlugNode<medVMECustom>("a custom vme"); // plug a vme in the main node factory

  mafVME *vme0 = vme_factory->CreateVMEInstance("medVMENotExisting");
  mafVME *vme1 = vme_factory->CreateVMEInstance("medVMECustom");

  CPPUNIT_ASSERT(vme0 == NULL);
  CPPUNIT_ASSERT(vme1!=NULL);

  CPPUNIT_ASSERT(vme1->IsMAFType(medVMECustom));

  std::vector<std::string> vmes = vme_factory->GetNodeNames();
  int s1 = vmes.size();
  CPPUNIT_ASSERT(s1 == 32); //24 VMES are registered in mafVMEFActory constructor actually + 7 from medVMEFactory, 
                            //please modify also the test when (un)plugged inside maf and update this number
           
  // test factory contents
  //poor proof to register again an already registered vme
  mafPlugNode<medVMECustom>("a custom vme"); // plug a vme in the main node factory again
  vmes = vme_factory->GetNodeNames();
  s1 = vmes.size();
  CPPUNIT_ASSERT(s1 == 32);

  bool found=false;
  for (int i=0;i<vmes.size();i++)
  {
		std::string test;
		test = vmes[i];
    if (vmes[i]=="medVMECustom")
      found=true;
  }
  
	CPPUNIT_ASSERT(found);
  // cleanup factory products
  vme1->Delete();
}
//-------------------------------------------------------------------------
void medVMEFactoryTest::TestGetDescription()
//-------------------------------------------------------------------------
{
  CPPUNIT_ASSERT(medVMEFactory::Initialize()==MAF_OK);

  medVMEFactory *vme_factory = NULL;
  vme_factory = (medVMEFactory*)medVMEFactory::GetInstance();
	CPPUNIT_ASSERT(vme_factory!=NULL);
	std::string value1 = vme_factory->GetDescription();
	std::string value2 = std::string("Factory for Medical VMEs");
	CPPUNIT_ASSERT( value1 == value2 );
}
  
