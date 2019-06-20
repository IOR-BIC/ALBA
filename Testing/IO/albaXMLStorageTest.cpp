#include "albaObject.h"
#include "albaStorable.h"
#include "albaXMLStorage.h"
#include "albaStorageElement.h"
#include "albaCoreFactory.h"
#include "albaIndent.h"
#include <iostream>

//------------------------------------------------------------------------------
class albaDummyObject: public albaObject, public albaStorable
//------------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaDummyObject,albaObject);
  void Print(std::ostream &os,const int tabs=0) {Superclass::Print(os,tabs);os<<albaIndent(tabs)<<"Name: \""<<m_Name.GetCStr()<<"\""<<std::endl;}
  albaDummyObject() {}
  virtual int InternalStore(albaStorageElement *element){return element->StoreText("Name",m_Name);}
  virtual int InternalRestore(albaStorageElement *element){return element->RestoreText("Name",m_Name);}
  albaString m_Name;
};

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaDummyObject);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
class albaStorableTestObject: public albaObject, public albaStorable
//------------------------------------------------------------------------------
{
public:
  albaTypeMacro(albaStorableTestObject,albaObject);
  void Print(std::ostream &os,const int tabs=0);
  albaStorableTestObject();
  ~albaStorableTestObject() {cppDEL(m_Dummy);}
  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);
  double m_FValue;
  int m_IValue;
  double m_FVector[4];
  int m_IVector[4];
  std::vector<double> m_FSTDVector;
  std::vector<int> m_ISTDVector;
  std::vector<albaString> m_StrVector;
  albaString m_Text;
  albaDummyObject *m_Dummy;
  // char *CData8;
  // short *CData16;
  // long *CData32;
};

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaStorableTestObject);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaStorableTestObject::albaStorableTestObject()
//------------------------------------------------------------------------------
{
  m_FValue=0;
  m_IValue=0;
  m_Dummy=NULL;
  m_FVector[0]=m_FVector[1]=m_FVector[2]=m_FVector[3]=0;
  m_IVector[0]=m_IVector[1]=m_IVector[2]=m_IVector[3]=0;
  m_FSTDVector.resize(4);
  m_ISTDVector.resize(4);
  m_StrVector.resize(4);
  m_FSTDVector[0]=m_FSTDVector[1]=m_FSTDVector[2]=m_FSTDVector[3]=0;
  m_ISTDVector[0]=m_ISTDVector[1]=m_ISTDVector[2]=m_ISTDVector[3]=0;
  m_StrVector[0]=m_StrVector[1]=m_StrVector[2]=m_StrVector[3]="";
}
//------------------------------------------------------------------------------
// example of serialization code
int albaStorableTestObject::InternalStore(albaStorageElement *element)
//------------------------------------------------------------------------------
{
  element->StoreDouble("FValue",m_FValue);
  element->StoreInteger("IValue",m_IValue);
  element->StoreVectorN("FVector",m_FVector,4);
  element->StoreVectorN("IVector",m_IVector,4);
  element->StoreVectorN("FSTDVector",m_FSTDVector,4);
  element->StoreVectorN("ISTDVector",m_ISTDVector,4);
  element->StoreVectorN("StrVector",m_StrVector,4,"Items");
  element->StoreText("Text",m_Text);
  element->StoreObject("Dummy",m_Dummy);
  return ALBA_OK;
}

//------------------------------------------------------------------------------
// example of deserialization code
int albaStorableTestObject::InternalRestore(albaStorageElement *element)
//------------------------------------------------------------------------------
{
  if (
    element->RestoreDouble("FValue",m_FValue)||
    element->RestoreInteger("IValue",m_IValue)||
    element->RestoreVectorN("FVector",m_FVector,4)||
    element->RestoreVectorN("IVector",m_IVector,4)||
    element->RestoreVectorN("FSTDVector",m_FSTDVector,4)||
    element->RestoreVectorN("ISTDVector",m_ISTDVector,4)||
    element->RestoreVectorN("StrVector",m_StrVector,4,"Items")||
    element->RestoreText("Text",m_Text)||
    (m_Dummy=dynamic_cast<albaDummyObject *>(element->RestoreObject("Dummy")))==NULL)
    return ALBA_ERROR;

  return ALBA_OK;
}

//------------------------------------------------------------------------------
// object debug printing to output stream
void albaStorableTestObject::Print(std::ostream &os,const int tabs)
//------------------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  albaIndent indent(tabs);
  os<<indent<<"FValue: "<<m_FValue<<std::endl;
  os<<indent<<"IValue: "<<m_IValue<<std::endl;
  os<<indent<<"FVector: {"<<m_FVector[0]<<","<<m_FVector[1]<<","<<m_FVector[2]<<","<<m_FVector[3]<<"}"<<std::endl;
  os<<indent<<"IVector: {"<<m_IVector[0]<<","<<m_IVector[1]<<","<<m_IVector[2]<<","<<m_IVector[3]<<"}"<<std::endl;
  os<<indent<<"FSTDVector: {"<<m_FSTDVector[0]<<","<<m_FSTDVector[1]<<","<<m_FSTDVector[2]<<","<<m_FSTDVector[3]<<"}"<<std::endl;
  os<<indent<<"ISTDVector: {"<<m_ISTDVector[0]<<","<<m_ISTDVector[1]<<","<<m_ISTDVector[2]<<","<<m_ISTDVector[3]<<"}"<<std::endl;
  os<<indent<<"StrVector: {\""<<m_StrVector[0].GetCStr()<<"\",\""<<m_StrVector[1].GetCStr()<<"\",\""<<m_StrVector[2].GetCStr()<<"\",\""<<m_StrVector[3].GetCStr()<<"\"}"<<std::endl;
  os<<indent<<"Text: \""<<m_Text.GetCStr()<<"\""<<std::endl;
  os<<indent<<"Dummy Object:"<<std::endl;
  if (m_Dummy!=NULL) {m_Dummy->Print(os,indent.GetNextIndent());} else {os <<indent<<"(NULL)"<<std::endl;}
}
//------------------------------------------------------------------------------
int main()
//------------------------------------------------------------------------------
{
  // instantiate an object factory
  albaCoreFactory::Initialize();
  // plug the nested object type in the factory (necessary
  // to be stored/restored with StoreObject()/RestoreObject() )
  albaPlugObject<albaDummyObject>("Dummy Object");

  albaStorableTestObject foo;
  
  albaDummyObject *dummy = new albaDummyObject;
  dummy->m_Name = "myDummyObject";
  
  // set the float and integer values
  foo.m_FValue=1.5;
  foo.m_IValue=15;

  // set the float array
  foo.m_FVector[0]=1.0;
  foo.m_FVector[1]=1.1;
  foo.m_FVector[2]=1.2;
  foo.m_FVector[3]=1.3;
  
  // set the integer array
  foo.m_IVector[0]=10;
  foo.m_IVector[1]=11;
  foo.m_IVector[2]=12;
  foo.m_IVector[3]=13;

  // test the STL float vector
  foo.m_FSTDVector[0]=2.0;
  foo.m_FSTDVector[1]=2.1;
  foo.m_FSTDVector[2]=2.2;
  foo.m_FSTDVector[3]=2.3;
  
  // test the STL integer vector
  foo.m_ISTDVector[0]=20;
  foo.m_ISTDVector[1]=21;
  foo.m_ISTDVector[2]=22;
  foo.m_ISTDVector[3]=23;

  // test the string vector
  foo.m_StrVector[0]="qui";
  foo.m_StrVector[1]="quo";
  foo.m_StrVector[2]="qua";
  foo.m_StrVector[3]="paperino";

  // test the storage of special characters in XML UTF-8
#ifdef WIN32
  foo.m_Text="Saved String àèéìòù§°ç";
#else
  foo.m_Text="Saved String Ã Ã¨Ã©Ã¬Ã²Ã¹";
#endif

  // set the object pointer
  foo.m_Dummy=dummy;

  // debug print
  foo.Print(std::cerr);
  
  // create an XML storage for saving
  albaXMLStorage storage;
  storage.SetURL("testfile.xml"); // where to store
  storage.SetFileType("TestXML"); // identification type
  storage.SetVersion("1.745");    // file version

  albaStorable *storable=albaStorable::SafeCastToObject(&foo);
  ALBA_TEST(storable!=NULL);

  storage.SetDocument(storable);

  int ret=storage.Store();
  ALBA_TEST(ret==ALBA_OK);

  // XML storage to restore
  albaXMLStorage restore;
  restore.SetURL("testfile.xml");
  restore.SetFileType("TestXML");
  restore.SetVersion("1.745");

  // create a new object to restore into
  albaStorableTestObject new_foo;
  restore.SetDocument(&new_foo);
  ret=restore.Restore();

  ALBA_TEST(ret==ALBA_OK);

  // cast from albaStorable to albaObject
  albaObject *doc=restore.GetDocument()->CastToObject();
  ALBA_TEST(doc==&new_foo); // check casting succeded

  std::cerr<<"*** Restored object ***\n";  
  new_foo.Print(std::cerr);

  // check restored object contents
  ALBA_TEST(new_foo.m_FValue==1.5);
  ALBA_TEST(new_foo.m_IValue==15);
  
  ALBA_TEST(new_foo.m_FVector[0]==1.0);
  ALBA_TEST(new_foo.m_FVector[1]==1.1);
  ALBA_TEST(new_foo.m_FVector[2]==1.2);
  ALBA_TEST(new_foo.m_FVector[3]==1.3);
  
  ALBA_TEST(new_foo.m_IVector[0]==10);
  ALBA_TEST(new_foo.m_IVector[1]==11);
  ALBA_TEST(new_foo.m_IVector[2]==12);
  ALBA_TEST(new_foo.m_IVector[3]==13);

  ALBA_TEST(new_foo.m_FSTDVector[0]==2.0);
  ALBA_TEST(new_foo.m_FSTDVector[1]==2.1);
  ALBA_TEST(new_foo.m_FSTDVector[2]==2.2);
  ALBA_TEST(new_foo.m_FSTDVector[3]==2.3);
  
  ALBA_TEST(new_foo.m_ISTDVector[0]==20);
  ALBA_TEST(new_foo.m_ISTDVector[1]==21);
  ALBA_TEST(new_foo.m_ISTDVector[2]==22);
  ALBA_TEST(new_foo.m_ISTDVector[3]==23);

  ALBA_TEST(new_foo.m_StrVector[0]=="qui");
  ALBA_TEST(new_foo.m_StrVector[1]=="quo");
  ALBA_TEST(new_foo.m_StrVector[2]=="qua");
  ALBA_TEST(new_foo.m_StrVector[3]=="paperino");

#ifdef WIN32
  ALBA_TEST(new_foo.m_Text=="Saved String àèéìòù§°ç");
#else
  ALBA_TEST(new_foo.m_Text=="Saved String Ã Ã¨Ã©Ã¬Ã²Ã¹");
#endif

  // check the restored nested object has been correctly restored
  ALBA_TEST(new_foo.m_Dummy!=NULL);
  ALBA_TEST(new_foo.m_Dummy->IsA(albaDummyObject::GetStaticTypeId()))
  ALBA_TEST(new_foo.m_Dummy->m_Name == "myDummyObject");

  //delete dummy; // clean memory
  
  std::cout<<"Test completed successfully!"<<std::endl;

  return ALBA_OK;
}
