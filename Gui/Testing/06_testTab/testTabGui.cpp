/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testTabGui.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:48:09 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testTabGui.h" 

//----------------------------------------------------------------------------
testTabGui::testTabGui(wxWindow* parent)
//----------------------------------------------------------------------------
{
  int ID =0;
  int numchoices = 3;
  wxString choices[3];
  choices[0] = "qui";
  choices[1] = "quo";
  choices[2] = "qua";

  m_idx =0;
  m_s1 = "pippo";
  m_s2 = "pollo";
  m_iv[0]=m_iv[1]=m_iv[2]=0;
  m_fv[0]=m_fv[1]=m_fv[2]=1.23456f;
  m_dv[0]=m_dv[1]=m_dv[2]=2.34567;
  m_dv2[0]=m_dv2[1]=m_dv2[2]=3.45678;

  
  m_gui = new mmgGui(this);
  
  m_gui->Label("label");
  m_gui->Label("label1","label2");
  m_gui->Divider();
  m_gui->Divider(1);
  m_gui->Divider(2);
  m_gui->Divider(3);


  m_gui->Button  (ID,"button");
  m_gui->Button  (ID,"button","label");

  m_gui->Vector  (ID, "i vector", m_iv);
  m_gui->Vector  (ID, "f vector", m_fv);
  m_gui->Vector  (ID, "f vector", m_dv);
  m_gui->FileOpen(ID,"open",   &m_s1);
  m_gui->FileSave(ID,"save",   &m_s1);
  m_gui->Divider(1);
  m_gui->DirOpen (ID,"dir",    &m_s1);
  m_gui->Color   (ID,"color",  &m_c);
  m_gui->Combo   (ID,	"label", &m_idx, numchoices, choices );
  //m_gui->Radio   (ID,	"label", &m_idx, numchoices, choices );
  m_gui->Radio   (ID,	"label", &m_idx, numchoices, choices, 3);

  m_gui->String  (ID, "string",&m_s1);			
  m_gui->Integer (ID, "int",	 &m_i);				
  m_gui->Float	 (ID, "float", &m_f);				
  m_gui->Double	 (ID, "double",&m_d);				

  m_gui->String  (ID, " ",     &m_s1);			
  m_gui->Integer (ID, " ",	   &m_i);				
  m_gui->Float	 (ID, " ",     &m_f);				
  m_gui->Double	 (ID, " ",		 &m_d);				

  m_gui->Slider  (ID,  "label", &m_i);
  //m_gui->FloatSlider  (ID,  "label", &m_f, 1, 20);
  m_gui->Slider  (ID,  "", &m_i);
  //m_gui->FloatSlider  (ID,  "", &m_f, 1, 20);

  //m_gui->Bool    (ID, "label", &m_i);
  //m_gui->Bool    (ID, "label", &m_i, true);

  m_gui->Label("12345678901234567890123456789012345");	// larghezza massima label lunga "12345678901234567890123456789012345"

  m_gui->Vector  (ID, "i vector", m_iv);
  m_gui->Vector  (ID, "", m_iv);
  
  /*
  m_gui->Label("12345678901234567890123456789");	// larghezza massima label lunga "123456789012345678901234567890"
  m_gui->Label( &m_s1 );							
  m_gui->Label("1234567890", m_s2);											// larghezza massima label corta "1234567890"
  m_gui->Label("1234567890",&m_s2);				
  m_gui->Label("12345678901234567890123456789",true);	
  m_gui->Label( &m_s1,true );							
  m_gui->Label("1234567890", m_s2,true);											
  m_gui->Label("1234567890",&m_s2,true);				
  */
  /*
  wxListBox				*lb   = m_gui->ListBox    (ID);
  mmgCheckListBox *cl   = m_gui->CheckList  (ID);
  wxGrid					*grid = m_gui->Grid       (ID);

  lb->Append("qui");
  lb->Append("quo");
  lb->Append("qua");

  cl->AddItem(0,"qui",true);
  cl->AddItem(1,"quo",false);
  cl->AddItem(2,"qua",true);
  */
  /*
  m_gui->Vector  (ID, "f vector", m_fv);
  m_gui->Vector  (ID, "d vector", m_dv);

  m_gui->Vector  (ID, "", m_iv);
  m_gui->Vector  (ID, "", m_fv);
  m_gui->Vector  (ID, "", m_dv);

  m_gui->VectorN (ID, "n vector", m_dv2,	1);
  m_gui->VectorN (ID, "n vector", m_dv2,	2);
  m_gui->VectorN (ID, "n vector", m_dv2,	3);
  m_gui->VectorN (ID, "n vector", m_dv2,	4);
  m_gui->VectorN (ID, "n vector", m_dv2,	5);
  m_gui->VectorN (ID, "n vector", m_dv2,	6);

  m_gui->VectorN (ID, "", m_dv2,	1);
  m_gui->VectorN (ID, "", m_dv2,	2);
  m_gui->VectorN (ID, "", m_dv2,	3);
  m_gui->VectorN (ID, "", m_dv2,	4);
  m_gui->VectorN (ID, "", m_dv2,	5);
  m_gui->VectorN (ID, "", m_dv2,	6);
  */

  m_gui->Float   (ID, "label", &m_f); // no bounds, flag=0, decimal digit=2
  m_gui->Float   (ID, "label", &m_f, 0, 10, 1); // flag = full width -> no label , 10 decimal digit          <<< error in num repr.
  /*
  m_gui->Double  (ID, "label", &m_d); // no bounds, flag=0, decimal digit=2
  m_gui->Double  (ID, "label", &m_d, 0, 10, 1); // flag = full width -> no label , 10 decimal digit					<<< error in num repr.
  */

  m_gui->Button  (ID,"button");
  m_gui->OkCancel();
  m_gui->Label("");


  m_guih = new mmgGuiHolder(parent,-1,true,true);
  //m_guih->SetSize(100,100);
  m_guih->Put(m_gui);
  m_guih->Show(true);

  //m_gui->Reparent(parent);
  //m_gui->FitGui();
  //m_gui->Update();
  //m_gui->Show(true);
}
//----------------------------------------------------------------------------
testTabGui::~testTabGui()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
wxWindow* testTabGui::GetGui()
//----------------------------------------------------------------------------
{
  if(m_guih) return m_guih;
  return m_guih;
}
//----------------------------------------------------------------------------
void testTabGui::OnEvent(mafEvent& e)
//----------------------------------------------------------------------------
{
  e.Log(); 
  /*
  switch(e.GetId())
  {
  default:
    e.Log();
  break; 
  }
  */
}



