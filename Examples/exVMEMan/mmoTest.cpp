/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoTest.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-18 21:09:13 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mmoTest.h"
#include "mmgGui.h"

const int ID = MINID;

//----------------------------------------------------------------------------
mmoTest::mmoTest(wxString label)
: mafOp(label)
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
mafOp* mmoTest::Copy()
//----------------------------------------------------------------------------
{
  return new mmoTest(m_label);
}
//----------------------------------------------------------------------------
void mmoTest::OpStop(int result) {	HideGui();mafEventMacro(mafEvent(this,result));}
//----------------------------------------------------------------------------
void mmoTest::OnEvent(mafEvent& e)
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
    mafEventMacro(e); 
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


  m_gui = new mmgGui(this);
  m_gui->SetListener(this);

  m_gui->Label("label");
  m_gui->Label("label1",wxString("label2")) ;
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
  m_gui->Bool    (ID, "label", &m_i);
  m_gui->Bool    (ID, "label", &m_i, true);

  m_gui->Label("12345678901234567890123456789012345");	// larghezza massima label lunga "12345678901234567890123456789012345"
  m_gui->Vector  (ID, "i vector", m_iv);
  m_gui->Vector  (ID, "", m_iv);


  m_gui->Button  (ID,"button");
  m_gui->OkCancel();
  m_gui->Label("");

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









