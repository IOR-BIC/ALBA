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
  virtual int InternalStore(mafStorageElement *element){return element->StoreText(m_Name,"Name");}
  virtual int InternalRestore(mafStorageElement *element){return element->RestoreText(m_Name,"Name");}
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
  ~mafStorableTestObject() {}
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
  element->StoreDouble(m_FValue,"FValue");
  element->StoreInteger(m_IValue,"IValue");
  element->StoreVectorN(m_FVector,4,"FVector");
  element->StoreVectorN(m_IVector,4,"IVector");
  element->StoreVectorN(m_FSTDVector,4,"FSTDVector");
  element->StoreVectorN(m_ISTDVector,4,"ISTDVector");
  element->StoreVectorN(m_StrVector,4,"StrVector","Items");
  element->StoreText(m_Text,"Text");
  element->StoreObject(m_Dummy,"Dummy");
  return MAF_OK;
}

//------------------------------------------------------------------------------
// example of deserialization code
int mafStorableTestObject::InternalRestore(mafStorageElement *element)
//------------------------------------------------------------------------------
{
  if (
    element->RestoreDouble(m_FValue,"FValue")||
    element->RestoreInteger(m_IValue,"IValue")||
    element->RestoreVectorN(m_FVector,4,"FVector")||
    element->RestoreVectorN(m_IVector,4,"IVector")||
    element->RestoreVectorN(m_FSTDVector,4,"FSTDVector")||
    element->RestoreVectorN(m_ISTDVector,4,"ISTDVector")||
    element->RestoreVectorN(m_StrVector,4,"StrVector","Items")||
    element->RestoreText(m_Text,"Text")||
    (m_Dummy=dynamic_cast<mafDummyObject *>(element->RestoreObject("Dummy")))==NULL)
    return MAF_ERROR;

  return MAF_OK;
}

//------------------------------------------------------------------------------
// object dubuf printing to output stream
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
  mafCoreFactory::Initialize();
  mafPlugObject<mafDummyObject>("Dummy Object");

  mafStorableTestObject foo;
  
  mafDummyObject dummy;
  dummy.m_Name = "myDummyObject";
  
  foo.m_FValue=1.5;
  foo.m_IValue=15;

  foo.m_FVector[0]=1.0;
  foo.m_FVector[1]=1.1;
  foo.m_FVector[2]=1.2;
  foo.m_FVector[3]=1.3;
  
  foo.m_IVector[0]=10;
  foo.m_IVector[1]=11;
  foo.m_IVector[2]=12;
  foo.m_IVector[3]=13;

  foo.m_FSTDVector[0]=2.0;
  foo.m_FSTDVector[1]=2.1;
  foo.m_FSTDVector[2]=2.2;
  foo.m_FSTDVector[3]=2.3;
  
  foo.m_ISTDVector[0]=20;
  foo.m_ISTDVector[1]=21;
  foo.m_ISTDVector[2]=22;
  foo.m_ISTDVector[3]=23;

  foo.m_StrVector[0]="qui";
  foo.m_StrVector[1]="quo";
  foo.m_StrVector[2]="qua";
  foo.m_StrVector[3]="paperino";

#ifdef WIN32
  foo.m_Text="Saved String àèéìòù§°ç";
#else
  foo.m_Text="Saved String Ã Ã¨Ã©Ã¬Ã²Ã¹";
#endif

  foo.m_Dummy=&dummy;

  foo.Print(std::cerr);
  
  mafXMLStorage storage;
  storage.SetURL("testfile.xml");
  storage.SetFileType("TestXML");
  storage.SetVersion("1.745");

  mafStorable *storable=mafStorable::SafeDownCast(&foo);
  MAF_TEST(storable!=NULL);

  storage.SetRoot(storable);

  int ret=storage.Store();
  MAF_TEST(ret==MAF_OK);

  mafXMLStorage restore;
  restore.SetURL("testfile.xml");
  restore.SetFileType("TestXML");
  restore.SetVersion("1.745");

  mafStorableTestObject new_foo;
  restore.SetRoot(&new_foo);
  ret=restore.Restore();

  MAF_TEST(ret==MAF_OK);

  mafObject *root=restore.GetRoot()->CastToObject();
  MAF_TEST(root==&new_foo);

  std::cerr<<"*** Restored object ***\n";  
  new_foo.Print(std::cerr);

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

  MAF_TEST(new_foo.m_Dummy!=NULL);
  MAF_TEST(new_foo.m_Dummy->IsType(mafDummyObject))
  MAF_TEST(new_foo.m_Dummy->m_Name == "myDummyObject");
  
  std::cout<<"Test completed successfully!"<<std::endl;

  return MAF_OK;
}
