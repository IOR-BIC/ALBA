#include "mafObject.h"
#include "mafStorable.h"
#include "mafXMLStorage.h"
#include "mafStorageElement.h"
#include "mafCoreFactory.h"
#include "mafIndent.h"
#include <iostream>

//------------------------------------------------------------------------------
class mafDummyObject: public mafObject, public mafStorable
//------------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafDummyObject,mafObject);
  void Print(std::ostream &os,const int tabs=0) {Superclass::Print(os,tabs);os<<mafIndent(tabs)<<"Name: \""<<m_Name.GetCStr()<<"\""<<std::endl;}
  mafDummyObject() {}
  virtual int InternalStore(mafStorageElement *element){return element->StoreText("Name",m_Name);}
  virtual int InternalRestore(mafStorageElement *element){return element->RestoreText("Name",m_Name);}
  mafString m_Name;
};

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafDummyObject);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
class mafStorableTestObject: public mafObject, public mafStorable
//------------------------------------------------------------------------------
{
public:
  mafTypeMacro(mafStorableTestObject,mafObject);
  void Print(std::ostream &os,const int tabs=0);
  mafStorableTestObject();
  ~mafStorableTestObject() {cppDEL(m_Dummy);}
  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);
  double m_FValue;
  int m_IValue;
  double m_FVector[4];
  int m_IVector[4];
  std::vector<double> m_FSTDVector;
  std::vector<int> m_ISTDVector;
  std::vector<mafString> m_StrVector;
  mafString m_Text;
  mafDummyObject *m_Dummy;
  // char *CData8;
  // short *CData16;
  // long *CData32;
};

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafStorableTestObject);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafStorableTestObject::mafStorableTestObject()
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
int mafStorableTestObject::InternalStore(mafStorageElement *element)
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
  return MAF_OK;
}

//------------------------------------------------------------------------------
// example of deserialization code
int mafStorableTestObject::InternalRestore(mafStorageElement *element)
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
    (m_Dummy=dynamic_cast<mafDummyObject *>(element->RestoreObject("Dummy")))==NULL)
    return MAF_ERROR;

  return MAF_OK;
}

//------------------------------------------------------------------------------
// object debug printing to output stream
void mafStorableTestObject::Print(std::ostream &os,const int tabs)
//------------------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
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
  mafCoreFactory::Initialize();
  // plug the nested object type in the factory (necessary
  // to be stored/restored with StoreObject()/RestoreObject() )
  mafPlugObject<mafDummyObject>("Dummy Object");

  mafStorableTestObject foo;
  
  mafDummyObject *dummy = new mafDummyObject;
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
  mafXMLStorage storage;
  storage.SetURL("testfile.xml"); // where to store
  storage.SetFileType("TestXML"); // identification type
  storage.SetVersion("1.745");    // file version

  mafStorable *storable=mafStorable::SafeCastToObject(&foo);
  MAF_TEST(storable!=NULL);

  storage.SetDocument(storable);

  int ret=storage.Store();
  MAF_TEST(ret==MAF_OK);

  // XML storage to restore
  mafXMLStorage restore;
  restore.SetURL("testfile.xml");
  restore.SetFileType("TestXML");
  restore.SetVersion("1.745");

  // create a new object to restore into
  mafStorableTestObject new_foo;
  restore.SetDocument(&new_foo);
  ret=restore.Restore();

  MAF_TEST(ret==MAF_OK);

  // cast from mafStorable to mafObject
  mafObject *doc=restore.GetDocument()->CastToObject();
  MAF_TEST(doc==&new_foo); // check casting succeded

  std::cerr<<"*** Restored object ***\n";  
  new_foo.Print(std::cerr);

  // check restored object contents
  MAF_TEST(new_foo.m_FValue==1.5);
  MAF_TEST(new_foo.m_IValue==15);
  
  MAF_TEST(new_foo.m_FVector[0]==1.0);
  MAF_TEST(new_foo.m_FVector[1]==1.1);
  MAF_TEST(new_foo.m_FVector[2]==1.2);
  MAF_TEST(new_foo.m_FVector[3]==1.3);
  
  MAF_TEST(new_foo.m_IVector[0]==10);
  MAF_TEST(new_foo.m_IVector[1]==11);
  MAF_TEST(new_foo.m_IVector[2]==12);
  MAF_TEST(new_foo.m_IVector[3]==13);

  MAF_TEST(new_foo.m_FSTDVector[0]==2.0);
  MAF_TEST(new_foo.m_FSTDVector[1]==2.1);
  MAF_TEST(new_foo.m_FSTDVector[2]==2.2);
  MAF_TEST(new_foo.m_FSTDVector[3]==2.3);
  
  MAF_TEST(new_foo.m_ISTDVector[0]==20);
  MAF_TEST(new_foo.m_ISTDVector[1]==21);
  MAF_TEST(new_foo.m_ISTDVector[2]==22);
  MAF_TEST(new_foo.m_ISTDVector[3]==23);

  MAF_TEST(new_foo.m_StrVector[0]=="qui");
  MAF_TEST(new_foo.m_StrVector[1]=="quo");
  MAF_TEST(new_foo.m_StrVector[2]=="qua");
  MAF_TEST(new_foo.m_StrVector[3]=="paperino");

#ifdef WIN32
  MAF_TEST(new_foo.m_Text=="Saved String àèéìòù§°ç");
#else
  MAF_TEST(new_foo.m_Text=="Saved String Ã Ã¨Ã©Ã¬Ã²Ã¹");
#endif

  // check the restored nested object has been correctly restored
  MAF_TEST(new_foo.m_Dummy!=NULL);
  MAF_TEST(new_foo.m_Dummy->IsA(mafDummyObject::GetStaticTypeId()))
  MAF_TEST(new_foo.m_Dummy->m_Name == "myDummyObject");

  //delete dummy; // clean memory
  
  std::cout<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
