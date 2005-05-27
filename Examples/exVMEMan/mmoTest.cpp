/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoTest.cpp,v $
  Language:  C++
  Date:      $Date: 2005-05-27 13:45:42 $
  Version:   $Revision: 1.5 $
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

//----------------------------------------------------------------------------
mmoTest::mmoTest(wxString label)
: mafOp(label)
//----------------------------------------------------------------------------
{
  m_s1	= "abcdefghilmnop";   // try use string with spaces -- weird things happens
  m_s2	= "123456789012345678901234";
  m_sopen = "";
  m_ssave = "";
  m_sdir = "";
  m_string1 = "";
  m_string2 = "";
  m_i1	= 1;
  m_i2	= 2;
  m_f		= 1.23;
  m_d1	= 9.87;
  m_d2	= 19.9;
  m_idx	= 1;
  m_bool1 = 0;
  m_bool2 = 0;
  m_sliderInt1= 0;
  m_sliderInt2= 0;
  m_iv[0] = m_iv[1] = m_iv[2] = 0;
  m_iv1[0] = m_iv1[1] = m_iv1[2] = 0;
  m_iv2[0] = m_iv2[1] = m_iv2[2] = 0;
  m_iv3[0] = m_iv3[1] = m_iv3[2] = 0;
  m_fv[0] = m_fv[1] = m_fv[2] = 0.0;
  m_dv[0] = m_dv[1] = m_dv[2] = 0.0;
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
  return new mmoTest(m_Label);
}
enum 
{
  ID0 = MINID,
  ID1,
  ID2,
  ID3,
  ID4,
  ID5,
  ID6,
  ID7,
  ID8,
  ID9,
  ID10,
  ID11,
  ID12,
  ID13,
  ID14,
  ID15,
  ID16,
  ID17,
  ID18,
  ID19,
  ID20,
  ID21,
  ID22,
  ID23,
  ID24,
  ID25
};

//----------------------------------------------------------------------------
void mmoTest::OpStop(int result) {	HideGui();mafEventMacro(mafEvent(this,result));}
//----------------------------------------------------------------------------
void mmoTest::OnEvent(mafEventBase *event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(event))
  {
    switch(e->GetId())
    {
      case wxOK:
        OpStop(OP_RUN_OK);
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);
      break;
      default:
        e->Log(); 
      break;
    }  
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

  m_Gui = new mmgGui(this);

  m_Gui->Label("label");
  m_Gui->Label("label1",mafString("label2")) ;
  m_Gui->Divider();
  m_Gui->Divider(1);
  m_Gui->Divider(2);
  m_Gui->Divider(3);

  m_Gui->Button  (ID0,"button");
  m_Gui->Button  (ID1,"button","label");

  m_Gui->Vector  (ID2, "i vector", m_iv1);
  //m_Gui->Vector  (ID3, "f vector", m_fv);
  m_Gui->Vector  (ID4, "d vector", m_dv);
  m_Gui->FileOpen(ID5,"open",   &m_sopen);
  m_Gui->FileSave(ID6,"save",   &m_ssave);
  m_Gui->Divider(1);
  m_Gui->DirOpen (ID7,"dir",    &m_sdir);
  m_Gui->Color   (ID8,"color",  &m_c);
  m_Gui->Combo   (ID9,	"label", &m_idx, numchoices, choices );
  //m_Gui->Radio   (ID,	"label", &m_idx, numchoices, choices );
  //m_Gui->Radio   (ID10,	"label", &m_idx, numchoices, choices, 3);

  m_Gui->String  (ID11, "string",&m_string1);			
  m_Gui->Integer (ID12, "int",	 &m_i1);
  //m_Gui->Float	 (ID13, "float", &m_f);				
  m_Gui->Double	 (ID14, "double",&m_d1);				

  m_Gui->String  (ID15, "",     &m_string2);
  m_Gui->Integer (ID16, "",	   &m_i2);
  //m_Gui->Float	 (ID17, " ",     &m_f);
  m_Gui->Double	 (ID18, "",		 &m_d2);

  m_Gui->Slider  (ID19,  "label", &m_sliderInt1);
  //m_Gui->FloatSlider  (ID,  "label", &m_f, 1, 20);
  m_Gui->Slider  (ID20,  "", &m_sliderInt2);
  //m_Gui->FloatSlider  (ID,  "", &m_f, 1, 20);
  m_Gui->Bool    (ID21, "label", &m_bool1);
  m_Gui->Bool    (ID22, "label", &m_bool2, true);

  m_Gui->Label("12345678901234567890123456789012345");	// larghezza massima label lunga "12345678901234567890123456789012345"
  m_Gui->Vector  (ID23, "i vector", m_iv2);
  m_Gui->Vector  (ID24, "", m_iv3);

  m_Gui->Button  (ID25,"button");
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
