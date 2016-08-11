/*=========================================================================

 Program: MAF2
 Module: mafOpExporterAnalogWSTest
 Authors: Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpExporterAnalogWSTest.h"
#include "mafOpExporterAnalogWS.h"

#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

#include "mafString.h"
#include "mafTagArray.h"
#include "mafVMEAnalog.h"
#include "mafVMEOutputScalarMatrix.h"

#include <string>
#include <assert.h>

#include <iostream>

//-----------------------------------------------------------
void mafOpExporterAnalogWSTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpExporterAnalogWS *Exporter = new mafOpExporterAnalogWS("Exporter");
  cppDEL(Exporter);
}
//-----------------------------------------------------------
void mafOpExporterAnalogWSTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  mafOpExporterAnalogWS Exporter; 
}

//-----------------------------------------------------------
void mafOpExporterAnalogWSTest::TestWrite() 
//-----------------------------------------------------------
{
	mafOpExporterAnalogWS *Exporter = new mafOpExporterAnalogWS("Exporter");
	Exporter->TestModeOn();
	mafString filename= GET_TEST_DATA_DIR();
  filename<<"/test_Analog.csv";
	Exporter->SetFileName(filename.GetCStr());
  
  vnl_vector<double> v_time;
  vnl_vector<double> v_first_channel;
  vnl_vector<double> v_second_channel;
  vnl_vector<double> v_third_channel;
  v_time.set_size(10);
  v_first_channel.set_size(10);
  v_second_channel.set_size(10);
  v_third_channel.set_size(10);

  for (int i=0;i<10;i++)
  {
    double d = (i+1)/10.0;
    v_time.put(i,d);
    v_first_channel.put(i,i+1);
    v_second_channel.put(i,(i+1)*2);
    v_third_channel.put(i,(i+1)*3);
  }
  
  vnl_matrix<double> emgScalar;
  emgScalar.set_size(4,10);
  emgScalar.set_row(0,v_time);
  emgScalar.set_row(1,v_first_channel);
  emgScalar.set_row(2,v_second_channel);
  emgScalar.set_row(3,v_third_channel);
  
  mafVMEAnalog* analog_test;
  mafNEW(analog_test);
  analog_test->SetData(emgScalar,-1);
  analog_test->Update();

  mafTagItem tag_Sig;
  tag_Sig.SetName("SIGNALS_NAME");
  tag_Sig.SetNumberOfComponents(3);
  tag_Sig.SetComponent("Unitary",0);
  tag_Sig.SetComponent("Doubled",1);
  tag_Sig.SetComponent("Triplicated",2);
  analog_test->GetTagArray()->SetTag(tag_Sig);
  
  Exporter->SetInput(analog_test);
  Exporter->Write();

  wxString file;
  file.append(filename.GetCStr());
  wxFileInputStream inputFile( file );
  wxTextInputStream text( inputFile );
  wxString line;

  // Check TAG
  line = text.ReadLine();
  CPPUNIT_ASSERT( line.Cmp("ANALOG")==0);

  line = text.ReadLine();
  
  // Check SIGNAL NAMES
  line = text.ReadLine();
  int num_tk;
  wxStringTokenizer tkzName(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
  num_tk = tkzName.CountTokens();
  tkzName.GetNextToken(); 
  tkzName.GetNextToken();
  wxString st = tkzName.GetNextToken();
  CPPUNIT_ASSERT( (num_tk == 4) && (st.Cmp("Doubled") == 0));

  line = text.ReadLine();

  // Check first row
  line = text.ReadLine();
  wxStringTokenizer tkzName2(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
  num_tk = tkzName2.CountTokens();
  wxString st2 = tkzName2.GetNextToken();
  CPPUNIT_ASSERT( (num_tk == 4) && (st2.Cmp("0.1") == 0));

  // Check fifth row
  line = text.ReadLine();
  line = text.ReadLine();
  line = text.ReadLine();
  line = text.ReadLine();
  wxStringTokenizer tkzName3(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
  tkzName3.GetNextToken();
  wxString st3 = tkzName3.GetNextToken();
  CPPUNIT_ASSERT( st3.Cmp("5") == 0);

  // Check last row
  line = text.ReadLine();
  line = text.ReadLine();
  line = text.ReadLine();
  line = text.ReadLine();
  line = text.ReadLine();
  wxStringTokenizer tkzName4(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
  tkzName4.GetNextToken();
  tkzName4.GetNextToken();
  tkzName4.GetNextToken();
  wxString st4 = tkzName4.GetNextToken();
  CPPUNIT_ASSERT( st4.Cmp("30") == 0);

  v_time.clear();
  v_first_channel.clear();
  v_second_channel.clear();
  v_third_channel.clear();
  emgScalar.clear();
  
  mafDEL(analog_test);
  
  delete Exporter;
  Exporter = NULL;
}