#include "mafTagArray.h"
#include "mafDecl.h"
#include "mafString.h"
#include "mafXMLStorage.h"
#include <iostream>

//----------------------------------------------------------------------------
//                                main
//---------------------------------------------------------------------------

int main()
{
  const char *strings[4]={"qui","quo","qua","paperino"};
  const double numbers[4]={1.1,1.2,1.3,1.234};

  int i=0;
  
  //Test TagItem constructors
  mafTagItem titem("TestTAG","String Value");
  mafTagItem tnum("TestNUM",1235.67890123456e20);
  mafTagItem tmulti("MultiString",strings,4);
  mafTagItem tmulti_num("MultiNum",numbers,4);
  
  // test TagItem's contents
  MAF_TEST(mafString::Equals("TestTAG",titem.GetName()));
  MAF_TEST(titem.GetType()==MAF_STRING_TAG);
  MAF_TEST(mafString::Equals(titem.GetValue(),"String Value"));
  
  MAF_TEST(mafString::Equals("TestNUM",tnum.GetName()));
  MAF_TEST(tnum.GetType()==MAF_NUMERIC_TAG);
  MAF_TEST(tnum.GetValueAsDouble()==1235.67890123456e20);
  
  MAF_TEST(mafString::Equals("MultiString",tmulti.GetName()));
  MAF_TEST(tmulti.GetType()==MAF_STRING_TAG);
  for (i=0;i<4;i++)
  {
    MAF_TEST(mafString::Equals(tmulti.GetValue(i),strings[i]));
  }

  MAF_TEST(mafString::Equals("MultiNum",tmulti_num.GetName()));
  MAF_TEST(tmulti_num.GetType()==MAF_NUMERIC_TAG);
  for (i=0;i<4;i++)
  {
    MAF_TEST(tmulti_num.GetValueAsDouble(i)==numbers[i]);
  }

  // test query function to extract the value as a single string 
  mafString value;
  tmulti.GetValueAsSingleString(value);
  mafString result="(\"qui\",\"quo\",\"qua\",\"paperino\")";
  MAF_TEST( value==result );

  tmulti_num.SetNumberOfComponents(6);
  tmulti_num.SetComponent(3.3,2);
  tmulti_num.SetComponent(3.5,4);
  tmulti_num.SetComponent(3.456,5);

  MAF_TEST(tmulti_num.GetNumberOfComponents()==6);
  MAF_TEST(tmulti_num.GetComponentAsDouble(2)==3.3);
  MAF_TEST(tmulti_num.GetComponentAsDouble(4)==3.5);
  MAF_TEST(tmulti_num.GetComponentAsDouble(5)==3.456);

  // Test adding some TagItems to a TagArray
  mafTagItem ti1, ti2, ti3, ti4;
  mafTagArray test_tag_array;
  
	ti1.SetName("qui");
	ti2.SetName("quo");
	ti3.SetName("archimede pitagorico");
	ti4.SetName("pico de paperis");

  ti1.SetValue("item1");
	ti2.SetValue("item2");
	ti3.SetValue(3);
	ti4.SetValue(4);

	ti1.SetType(MAF_STRING_TAG);
	ti2.SetType(MAF_STRING_TAG);
	ti3.SetType(MAF_NUMERIC_TAG);
	ti4.SetType(MAF_NUMERIC_TAG);

	test_tag_array.SetTag(ti1);
	test_tag_array.SetTag(ti2);
	test_tag_array.SetTag(ti3);
	test_tag_array.SetTag(ti4);

  
  
  // extract all items of a given type
  std::vector<mafTagItem *> pti_vector;
  test_tag_array.GetTagsByType(MAF_STRING_TAG, pti_vector);

  std::cerr << "\n";
  std::cerr << "\n";
  std::cerr << "Tags of type MAF_STRING_TAG are:" << "\n";
  std::cerr << "\n";

  // check type of extracted items
  std::vector<mafTagItem *>::iterator v_iterator;
  for (v_iterator = pti_vector.begin(); v_iterator != pti_vector.end(); v_iterator++)
  {
    std::cerr << (*v_iterator)->GetName() << "\n";
    MAF_TEST((*v_iterator)->GetType()==MAF_STRING_TAG);
  }		

  pti_vector.clear();

  // extract numerical items
  test_tag_array.GetTagsByType(MAF_NUMERIC_TAG, pti_vector);

  std::cerr << "\n";
  std::cerr << "\n";
  std::cerr << "Tags of type MAF_NUMERIC_TAG are:" << "\n";
  std::cerr << "\n";
  
  // test extracted items type
  for (v_iterator = pti_vector.begin(); v_iterator != pti_vector.end(); v_iterator++)
  {
	  std::cerr << (*v_iterator)->GetName() << "\n";
    MAF_TEST((*v_iterator)->GetType()==MAF_NUMERIC_TAG);
  }


  // test TagArray storing/restoring...

  // create an XML storage for storing the mafTagArray
  mafXMLStorage storage;
  storage.SetURL("testTagArray.xml");
  storage.SetFileType("TagArrayXML");
  
  storage.SetRoot(&test_tag_array);
  int ret=storage.Store();
  
  MAF_TEST(ret==MAF_OK);

  // create a new XML storage to restore the TagArray
  mafXMLStorage restore;
  restore.SetURL("testTagArray.xml");
  restore.SetFileType("TagArrayXML");
  
  // create the root to restore the TagArray inside
  mafTagArray new_tag_array;
  restore.SetRoot(&new_tag_array);
  ret=restore.Restore();

  MAF_TEST(ret==MAF_OK);

  // check restored TagArray contents
  MAF_TEST(new_tag_array==test_tag_array);

  std::cout<<"Test completed successfully!"<<std::endl;
  
  return 0;
}
