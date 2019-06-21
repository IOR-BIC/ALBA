/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmoTest
 Authors: Silvano Imboden
 
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


#include "mmoTest.h"
#include "albaGUIGui.h"

const int ID = MINID;

//----------------------------------------------------------------------------
mmoTest::mmoTest(wxString label)
: albaOp(label)
//----------------------------------------------------------------------------
{
  m_s1	= "abcdefghilmnop";   // try use string with spaces -- weird things happens
  m_s2	= "123456789012345678901234";
  m_i		= 1;
  m_f		= 1.23;
  m_d		= 9.87;
  m_idx	= 1;
}
//----------------------------------------------------------------------------
mmoTest::~mmoTest()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
albaOp* mmoTest::Copy()
//----------------------------------------------------------------------------
{
  return new mmoTest(m_Label);
}
//----------------------------------------------------------------------------
void mmoTest::OpStop(int result) {	HideGui();albaEventMacro(albaEvent(this,result));}
//----------------------------------------------------------------------------
void mmoTest::OnEvent(albaEvent& e)
//----------------------------------------------------------------------------
{
  switch(e.GetId())
  {
  case wxOK:           // perche non OP_RUN_OK anziche wxOK ?
    OpStop(OP_RUN_OK);
    break;
  case wxCANCEL:
    OpStop(OP_RUN_CANCEL);
    break;
  default:
    albaEventMacro(e); 
    break;
  }  
}
//----------------------------------------------------------------------------
void mmoTest::OpRun()
//----------------------------------------------------------------------------
{
  int numchoices = 3;
  wxString choices[3];
  choices[0] = "qui";
  choices[1] = "quo";
  choices[2] = "qua";


  m_Gui = new albaGUIGui(this);
  m_Gui->SetListener(this);

  m_Gui->Label("label");
  m_Gui->Label("label1",albaString("label2")) ;
  m_Gui->Divider();
  m_Gui->Divider(1);
  m_Gui->Divider(2);
  m_Gui->Divider(3);

  m_Gui->Button  (ID,"button");
  m_Gui->Button  (ID,"button",albaString("label"));

  m_Gui->Vector  (ID, "i vector", m_iv);
  m_Gui->Vector  (ID, "f vector", m_fv);
  m_Gui->Vector  (ID, "f vector", m_dv);
  m_Gui->FileOpen(ID,"open",   &m_s1);
  m_Gui->FileSave(ID,"save",   &m_s1);
  m_Gui->Divider(1);
  m_Gui->DirOpen (ID,"dir",    &m_s1);
  m_Gui->Color   (ID,"color",  &m_c);
  m_Gui->Combo   (ID,	"label", &m_idx, numchoices, choices );
  //m_Gui->Radio   (ID,	"label", &m_idx, numchoices, choices );
  m_Gui->Radio   (ID,	"label", &m_idx, numchoices, choices, 3);

  m_Gui->String  (ID, "string",&m_s1);			
  m_Gui->Integer (ID, "int",	 &m_i);				
  m_Gui->Float	 (ID, "float", &m_f);				
  m_Gui->Double	 (ID, "double",&m_d);				

  m_Gui->String  (ID, " ",     &m_s1);			
  m_Gui->Integer (ID, " ",	   &m_i);				
  m_Gui->Float	 (ID, " ",     &m_f);				
  m_Gui->Double	 (ID, " ",		 &m_d);				

  m_Gui->Slider  (ID,  "label", &m_i);
  //m_Gui->FloatSlider  (ID,  "label", &m_f, 1, 20);
  m_Gui->Slider  (ID,  "", &m_i);
  //m_Gui->FloatSlider  (ID,  "", &m_f, 1, 20);
  m_Gui->Bool    (ID, "label", &m_i);
  m_Gui->Bool    (ID, "label", &m_i, true);

  m_Gui->Label("12345678901234567890123456789012345");	// larghezza massima label lunga "12345678901234567890123456789012345"
  m_Gui->Vector  (ID, "i vector", m_iv);
  m_Gui->Vector  (ID, "", m_iv);


  m_Gui->Button  (ID,"button");
  m_Gui->OkCancel();
  m_Gui->Label("");

  ShowGui();
}
//----------------------------------------------------------------------------
void mmoTest::OpDo()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mmoTest::OpUndo()
//----------------------------------------------------------------------------
{
}









