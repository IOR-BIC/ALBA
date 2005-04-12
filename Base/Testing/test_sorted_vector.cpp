
#pragma warning(disable:4786)

#include <set>
#include "sorted_vector.h"
#include <iostream>
#include "assert.h"
#include <time.h>
#include <iomanip>

struct STest{
    std::string s;
    int         id;
};

STest MakeSTest()
{
    static int id= 0;
    int nLen= rand()%10;
    char* letters[]={"0","1"};
    STest stest;
            
    for(int i=0;i<nLen;i++){
        stest.s+= letters[rand()%2];
    }
    stest.id= ++id;
    return stest;
}

bool operator<(const STest& t0,const STest& t1)
{
    int nCmp;
    return (nCmp=t0.s.compare(t1.s))<0||nCmp==0 && t0.id<t1.id;
}


class SimpTimer{
public:
    SimpTimer(const char* msg):m_msg(msg),m_start(clock()){}
    ~SimpTimer()
    {
        m_stop = clock();
        std::cout   <<  "CPU usage ("   << std::setw(35) <<  m_msg.c_str()    <<  ") = " 
                    << (double)(m_stop - m_start)/ CLOCKS_PER_SEC
                    <<  " seconds"   <<  std::endl;
    }
private:
    time_t m_start, m_stop;
    std::string     m_msg;
};


template<class It,class Pred>
bool is_unique(It beg,It beyond,Pred pred)
{
    return std::adjacent_find(beg,beyond,std::not2(pred))==beyond;
}
template<class FwIt,class Pred>
FwIt unsorted_find(FwIt beg,FwIt beyond,Pred pred)
{
    for(FwIt prev    ;(prev=beg)!=beyond && ++beg!=beyond;){
        if( pred(*beg,*prev) ){
            return prev;
        }
    }
    return beyond;
}
template<class T>
void TestSet(std::vector<T>& v)
{
    std::set<T> std_set;
    {SimpTimer t("build std::set");
        for(unsigned i=0;i<v.size();i++){
            std_set.insert(v[i]);
        }
    }
    {SimpTimer t("find in std::set");
        for(unsigned i=0;i<v.size();i++){
            std::set<T>::iterator it= std_set.find(v[i]);
            std::set<T>::size_type nCount= std_set.count(v[i]);
            assert(     nCount==0 && it==std_set.end()
                    ||  nCount!=0 && it!=std_set.end());
        }
    }
}
template<class T>
void TestSortedVector_AsSet(std::vector<T>& v)
{   
    codeproject::sorted_vector<T,true> svec;
    {SimpTimer t("build sortedvec_set (naiv)");
        for(unsigned i=0;i<v.size();i++){
            svec.insert(v[i]);
        }
    }
    assert(is_unique(svec.begin(),svec.end(),svec.key_comp()));
    codeproject::sorted_vector<T,true> svec1;
    {SimpTimer t("build sortedvec_set (opt.)");
        codeproject::sorted_vector<T,true>::Cont& vec= svec1.get_container();
        for(unsigned i=0;i<v.size();i++){
            vec.push_back(v[i]);
            
        }
        svec1.sort();
    }
    assert(svec==svec1);
    assert(is_unique(svec1.begin(),svec1.end(),svec1.key_comp()));

    {SimpTimer t("find in sortedvec_set");
        for(unsigned i=0;i<v.size();i++){
            codeproject::sorted_vector<T,true>::iterator it= svec1.find(v[i]);
            codeproject::sorted_vector<T,true>::size_type nCount= svec1.count(v[i]);
            assert(     nCount==0 && it==svec1.end()
                    ||  nCount!=0 && it!=svec1.end());
        }
    }
    for(unsigned i=0;i<v.size();i++){svec1.erase(v[i]);}
    assert(svec1.empty());
}
template<class T>
void TestMultiSet(std::vector<T>& v)
{
    std::multiset<T> svec;

    {SimpTimer t("build multiset");
        for(unsigned i=0;i<v.size();i++){
            svec.insert(v[i]);
        }
    }
    assert(unsorted_find(svec.begin(),svec.end(),svec.key_comp())==svec.end());
    {SimpTimer t("find in multiset");
        for(unsigned i=0;i<v.size();i++){
            std::multiset<T>::iterator it= svec.find(v[i]);
            std::multiset<T>::size_type nCount= svec.count(v[i]);
            assert(     nCount==0 && it==svec.end()
                    ||  nCount!=0 && it!=svec.end());
        }
    }
}

template<class T>
void TestSortedVector_AsMultiSet(std::vector<T>& v)
{
    codeproject::sorted_vector<T> svec;
    {SimpTimer t("build sortedvec_multiset (naiv)");
        for(unsigned i=0;i<v.size();i++){
            svec.insert(v[i]);
        }
    }
    codeproject::sorted_vector<T> svec1;
    {SimpTimer t("build sortedvec_multiset (opt.)");
    codeproject::sorted_vector<T>::Cont& vec= svec1.get_container();
        for(unsigned i=0;i<v.size();i++){
            vec.push_back(v[i]);
            
        }
        svec1.stable_sort();
    }
    assert(svec==svec1);
    assert(unsorted_find(svec.begin(),svec.end(),svec.key_comp())==svec.end());
    {SimpTimer t("find in sortedvec_multiset");
        for(unsigned i=0;i<v.size();i++){
            int n= rand();
            codeproject::sorted_vector<T>::iterator it= svec1.find(v[i]);
            codeproject::sorted_vector<T>::size_type nCount= svec1.count(v[i]);
            assert(     nCount==0 && it==svec1.end()
                    ||  nCount!=0 && it!=svec1.end());
        }
    }
/*test various functions*/
    const codeproject::sorted_vector<T> svec2(svec);
    assert(svec==svec2);
    for(unsigned i=0;i<v.size();i++){assert(svec2.find(v[i])!=svec2.end());}
    codeproject::sorted_vector<T> svec3(v.begin(),v.end());
    assert(svec3==svec2);
    codeproject::sorted_vector<T> svec4(v.begin(),v.begin()+(v.end()-v.begin())/2);
    svec4= svec3;
    assert(svec4==svec3);
    while(svec4.size()>0){
        svec4.pop_back();
    }
}




template<class T>
void ExecTests(std::vector<T>& v)
{
    std::cout   <<  "std::set versus 'sorted_vector as set'"   <<  std::endl; 
    TestSet(v);
    TestSortedVector_AsSet(v);
    std::cout    <<  "std::multiset versus 'sorted_vector as multiset'" <<  std::endl;
    TestMultiSet(v);
    TestSortedVector_AsMultiSet(v);
}

template<class T,class Pred>
void TestSetOperations(const std::vector<T>& v0,
                       const std::vector<T>& v1,
                       const std::vector<T>& v2,
                       Pred pred)
{
//A) compute the result of the set-operation: (v0-v1)+v2 - intersect(v1,v2)
    codeproject::sorted_vector<T,true,Pred> svec(v0.begin(),v0.end(),pred);
    codeproject::sorted_vector<T,true,Pred> svec_v1(pred); svec_v1= v1;
    codeproject::sorted_vector<T,true,Pred> svec_v2(pred); svec_v2= v2;
    unsigned i,j;
    for(i=0;i<v1.size();i++){svec.erase(v1[i]);}
    for(j=0;j<v2.size();j++){svec.insert(v2[j]);}
    unsigned m;
    for(m=0;m<svec_v1.size();m++){
        if( svec_v2.find(svec_v1[m])!=svec_v2.end() ){
            svec.erase(svec_v1[m]);
        }
    }
//B) compute the result of the set-operation (v0+v2)-v1
    codeproject::sorted_vector<T,true,Pred> svec1(v0.begin(),v0.end(),pred);
    for(unsigned k=0;k<v2.size();k++){svec1.insert(v2[k]);}
    for(unsigned l=0;l<v1.size();l++){svec1.erase(v1[l]);}
    codeproject::sorted_vector<T,true,Pred>::size_type nSize= svec.size();
    codeproject::sorted_vector<T,true,Pred>::size_type nSize1= svec1.size();
//test whether results are the same
    assert(svec==svec1);
}


codeproject::sorted_vector<int,true> 
BuildIntersection(std::vector<int>& v0,std::vector<int>& v1)
{
    codeproject::sorted_vector<int,true> svec(v0.begin(),v0.end());
    codeproject::sorted_vector<int,true> svecIntersection;
    for(unsigned  i=0;i<v1.size();i++){
        if( svec.find(v1[i])!=svec.end() ){
            svecIntersection.insert(v1[i]);
        }
    }
    return svecIntersection;
}
codeproject::sorted_vector<int,true> 
BuildIntersection1(std::vector<int>& v0,std::vector<int>& v1)
{
    codeproject::sorted_vector<int,true> svec(v0.begin(),v0.end());
    codeproject::sorted_vector<int,true> svecIntersection;
    codeproject::sorted_vector<int,true>::Cont& vInterSect= 
                            svecIntersection.get_container();
    for(unsigned i=0;i<v1.size();i++){
        if( svec.find(v1[i])!=svec.end() ){
            vInterSect.push_back(v1[i]);
        }
    }
    svecIntersection.sort();
    return svecIntersection;
}


struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

int SGITest()
{
  using namespace std;
  using namespace codeproject;
  const int N = 6;
  const char* a[N] = {"isomer", "ephemeral", "prosaic", 
                      "nugatory", "artichoke", "serif"};
  const char* b[N] = {"flat", "this", "artichoke",
                      "frigate", "prosaic", "isomer"};

  sorted_vector<const char*,true,ltstr> A(a, a + N);
  sorted_vector<const char*,true,ltstr> B(b, b + N);
  sorted_vector<const char*,true,ltstr> C;

  cout << "Set A: ";
  copy(A.begin(), A.end(), ostream_iterator<const char*>(cout, " "));
  cout << endl;
  cout << "Set B: ";
  copy(B.begin(), B.end(), ostream_iterator<const char*>(cout, " "));   
  cout << endl;

  cout << "Union: ";
  set_union(A.begin(), A.end(), B.begin(), B.end(),
            ostream_iterator<const char*>(cout, " "),
            ltstr());   
  cout << endl;
  return 0;
}

void TestAllSet()
{
    using namespace std;
	using namespace codeproject;
    typedef sorted_vector<const char*,true,ltstr> StrSet;
    StrSet months;
    months.insert("jan");
    months.insert("feb");
    months.insert("mar");
    months.insert("apr");
    months.insert("may");
    months.insert("jun");
    months.insert("jul");
    months.insert("aug");
    months.insert("sep");
    months.insert("oct");
    months.insert("nov");
    months.insert("dec");
    StrSet::iterator it= months.find("jul");
    assert(strcmp(*it,"jul")==0);
    cout    <<  "previous of jul (in alphabetical order) is " << (it[-1]) <<  endl;
    cout    <<  "next of jul (in alphabetical order) is " << (it[1])  <<  endl;

    cout << "months in alphabetical order: "    <<  endl;
    copy(months.begin(),months.end(),ostream_iterator<const char*>(cout," ")); 
    cout << endl	<<	"months in reverse alphabetical order: "   <<  endl;
    copy(months.rbegin(),months.rend(),ostream_iterator<const char*>(cout," "));
    /*test count*/
    {
        for(StrSet::iterator it= months.begin();it!=months.end();++it){
            assert(months.count(*it)==1);
        }
    }
    /*test copy construction and comparison operators*/
    StrSet monthsCopy(months);
    assert(     months==monthsCopy 
            &&  months<=monthsCopy && months>=monthsCopy
            &&  !(months<monthsCopy) && !(months>monthsCopy));

    std::pair<StrSet::iterator,StrSet::iterator> pairMismatch=
                    mismatch(months.begin(),months.end(),monthsCopy.begin());
    assert(pairMismatch.first==months.end() && pairMismatch.second==monthsCopy.end());

    /*test insertion of already present element*/
    copy(months.begin(),months.end(),inserter(monthsCopy,monthsCopy.begin()));
    assert(months.size()==monthsCopy.size());

    /*test insert member functions*/
    months.insert(monthsCopy.begin(),monthsCopy.end());
    assert(months==monthsCopy);
    StrSet months1(months.begin(),months.begin()+3);
    months1.insert(months.begin()+1,months.end());
    assert(months1==months);
    months1.insert("aug");
    months1.insert("xxx");
	months1.insert(months1.find("xxx"),"yyy");
    months1.insert("zzz");
    assert(months1>months && months1.size()==months.size()+3);
    /*test erase member functions*/
    months1.erase(months1.find("xxx"),months1.end());
    assert(months1.size()==months.size());

    /*test lower_bound,upper_bound,equal_range*/
    assert( strcmp(*months.lower_bound("jul"),"jul")==0);
   

    cout << endl;
}


int main()
{
//timed tests
    std::vector<int> v;
    int i;
    for(i=0;i<500;i++){v.push_back(rand());}
    std::cout   <<  "--------------Tests with element type int-------------"   <<  std::endl;
    ExecTests(v);
 
    std::vector<STest> vt;
    for(i=0;i<500;i++){vt.push_back(MakeSTest());}
     std::cout   <<  "-Tests with element type 'STest' (string,int)--------"   <<  std::endl;
    ExecTests(v);

//set operations-test
    std::vector<int> v1,v2;
    for(i=0;i<100;i++){v1.push_back(rand());}
    for(i=0;i<100;i++){v2.push_back(rand());}
    TestSetOperations(v,v1,v2,std::greater<int>());

    assert(BuildIntersection(v1,v2)==BuildIntersection1(v1,v2));
    SGITest();
	TestAllSet();
    return 0;
}