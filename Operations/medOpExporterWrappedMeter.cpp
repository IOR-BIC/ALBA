/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpExporterWrappedMeter.cpp,v $
  Language:  C++
  Date:      $Date: 2009-06-03 15:31:10 $
  Version:   $Revision: 1.3.2.2 $
  Authors:   Daniele Giunchi
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


#include "medOpExporterWrappedMeter.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"

#include "medVMEWrappedMeter.h"
#include "medVMEOutputWrappedMeter.h"
#include "medVMEComputeWrapping.h"

#include "medOpImporterLandmark.h"
#include "medDefines.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMERoot.h"
#include "medVMEWrappedMeter.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "vtkDataSet.h"

#include <fstream>


//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpExporterWrappedMeter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpExporterWrappedMeter::medOpExporterWrappedMeter(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_EXPORTER;
  m_Canundo = true;
  
  m_File    = "";
}
//----------------------------------------------------------------------------
medOpExporterWrappedMeter::~medOpExporterWrappedMeter( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* medOpExporterWrappedMeter::Copy()   
//----------------------------------------------------------------------------
{
	medOpExporterWrappedMeter *cp = new medOpExporterWrappedMeter(m_Label);
	cp->m_File = m_File;
	return cp;
}
//----------------------------------------------------------------------------
bool medOpExporterWrappedMeter::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(medVMEComputeWrapping));
}
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum WRAPPED_METER_EXPORTER_ID
{
  ID_CHOOSE_FILENAME = MINID,
};
//----------------------------------------------------------------------------
void medOpExporterWrappedMeter::OpRun()   
//----------------------------------------------------------------------------
{
  mafString wildc = "Wrapped Meter Coordinates(*.txt)|*.txt";

  m_Gui = new mafGUI(this);
	//m_Gui->FileSave(ID_CHOOSE_FILENAME,"stl file", &m_File, wildc,"Save As...");
	m_Gui->OkCancel();
  
	m_Gui->Divider();

	ShowGui();
}
//----------------------------------------------------------------------------
void medOpExporterWrappedMeter::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
    switch(e->GetId())
    {
      case wxOK:
				{
          mafString initialFileName;
          initialFileName = mafGetApplicationDirectory().c_str();
          initialFileName.Append("\\ActionLine.txt");

          mafString wildc = "configuration file (*.txt)|*.txt";
          m_File = mafGetSaveFile(initialFileName.GetCStr(), wildc).c_str();

          if (m_File == "") return;

		  //Test();		  //using this method to try test case
		  ExportWrappedMeterCoordinates();
				  
				  OpStop(OP_RUN_OK);
				}
      break;
      case ID_CHOOSE_FILENAME:
        m_Gui->Enable(wxOK,m_File != "");
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
void medOpExporterWrappedMeter::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void medOpExporterWrappedMeter::ExportWrappedMeterCoordinates()
//----------------------------------------------------------------------------
{
  /*medVMEWrappedMeter *vmeWrappedMeter =  medVMEWrappedMeter::SafeDownCast(m_Input);
  vmeWrappedMeter->Update();
  medVMEOutputWrappedMeter *out_wm = medVMEOutputWrappedMeter::SafeDownCast(vmeWrappedMeter->GetOutput());
  out_wm->Update();*/

  medVMEComputeWrapping *vmeMeter = medVMEComputeWrapping::SafeDownCast(m_Input);
  vmeMeter->Update();
  medVMEOutputComputeWrapping *out_cm = medVMEOutputComputeWrapping::SafeDownCast(vmeMeter->GetOutput());
  out_cm->Update();

  std::ofstream outputFile(m_File, std::ios::out);

  if (outputFile == NULL) {
    wxMessageBox("Error opening configuration file");
    return ;
  }

  if (vmeMeter->GetWrappedClass()==medVMEComputeWrapping::NEW_METER)
  {
		/*if (vmeMeter->GetWrappedMode()==  medVMEComputeWrapping )
		{
		}*/
	  outputFile<< vmeMeter->GetStartPointCoordinate()[0]<< '\t'
		  <<vmeMeter->GetStartPointCoordinate()[1]<<'\t'
		  <<vmeMeter->GetStartPointCoordinate()[1]<<std::endl;
	  outputFile<< vmeMeter->GetEndPointCoordinate()[0]<< '\t'
		  <<vmeMeter->GetEndPointCoordinate()[1]<<'\t'
		  <<vmeMeter->GetEndPointCoordinate()[1]<<std::endl;

  }else if (vmeMeter->GetWrappedClass()==medVMEComputeWrapping::OLD_METER)
  {
	  if (vmeMeter->GetWrappedMode() == medVMEComputeWrapping::MANUAL_WRAP)
	{
		for(int i=0; i<vmeMeter->GetNumberMiddlePoints();i++)
		{
			outputFile << vmeMeter->GetMiddlePointCoordinate(i)[0] << '\t'
				<< vmeMeter->GetMiddlePointCoordinate(i)[1] << '\t'
				<< vmeMeter->GetMiddlePointCoordinate(i)[2] << std::endl;
		}
	}else if (vmeMeter->GetWrappedMode() == medVMEComputeWrapping::AUTOMATED_WRAP)
	{
		outputFile << vmeMeter->GetWrappedGeometryTangent1()[0] << '\t'
			<< vmeMeter->GetWrappedGeometryTangent1()[1] << '\t'
			<< vmeMeter->GetWrappedGeometryTangent1()[2] << std::endl;

		outputFile << vmeMeter->GetWrappedGeometryTangent2()[0] << '\t'
			<< vmeMeter->GetWrappedGeometryTangent2()[1] << '\t'
			<< vmeMeter->GetWrappedGeometryTangent2()[2] << std::endl;
	}
  }
  


  /*if(vmeWrappedMeter->GetWrappedMode() == medVMEWrappedMeter::MANUAL_WRAP)
  {
    for(int i=0; i<vmeWrappedMeter->GetNumberMiddlePoints();i++)
    {
      outputFile << vmeWrappedMeter->GetMiddlePointCoordinate(i)[0] << '\t'
        << vmeWrappedMeter->GetMiddlePointCoordinate(i)[1] << '\t'
        << vmeWrappedMeter->GetMiddlePointCoordinate(i)[2] << std::endl;
    }
  }
  else if(vmeWrappedMeter->GetWrappedMode() == medVMEWrappedMeter::AUTOMATED_WRAP)
  {
    outputFile << vmeWrappedMeter->GetWrappedGeometryTangent1()[0] << '\t'
      << vmeWrappedMeter->GetWrappedGeometryTangent1()[1] << '\t'
      << vmeWrappedMeter->GetWrappedGeometryTangent1()[2] << std::endl;

    outputFile << vmeWrappedMeter->GetWrappedGeometryTangent2()[0] << '\t'
      << vmeWrappedMeter->GetWrappedGeometryTangent2()[1] << '\t'
      << vmeWrappedMeter->GetWrappedGeometryTangent2()[2] << std::endl;
  }*/
  
  outputFile.close();
  

}

//-----------------------------------------------------------
void medOpExporterWrappedMeter::Test() 
//-----------------------------------------------------------
{
/*	mafVMEStorage *storage = mafVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

*/
	//create landmarks and relative landmark cloud
	medOpImporterLandmark *importer=new medOpImporterLandmark("importer");
	importer->TestModeOn();
	//importer->SetInput(storage->GetRoot());
	//mafString filename=MED_DATA_ROOT;
	mafString filename = "C:\\MAF\\Medical\\Testing\\unittestData\\RAW_MAL\\cloud_to_be_imported";
	//filename<<"../unittestData";
	//filename<<"/RAW_MAL/cloud_to_be_imported";
	importer->SetFileName(filename.GetCStr());
	importer->Read();
	
	mafVMELandmarkCloud *cloud=(mafVMELandmarkCloud *)importer->GetOutput();
	cloud->Open();


	medVMEComputeWrapping *wrappedMeter;
	mafNEW(wrappedMeter);
	//mafNode *n = mafNode::SafeDownCast(cloud);
//wrappedMeter->SetMeterLink(wrappedMeter,);

	wrappedMeter->SetMeterLink("StartVME",cloud->GetLandmark(0));
	wrappedMeter->SetMeterLink("EndVME1",cloud->GetLandmark(1));

	wrappedMeter->SetMeterLink(cloud->GetLandmark(2)->GetName(),cloud->GetLandmark(2));
	wrappedMeter->SetMeterLink(cloud->GetLandmark(3)->GetName(),cloud->GetLandmark(3));
	wrappedMeter->SetMeterLink(cloud->GetLandmark(4)->GetName(),cloud->GetLandmark(4));
	//wrappedMeter->PushIdVector(cloud->GetId()); // for landmark middlepoint is memorized as sequence of cloud id and interal id of the landmark
	//wrappedMeter->PushIdVector(2); //this is for the vector syncronized with the gui widget, that is not used in gui test


	wrappedMeter->SetParent(cloud);
	wrappedMeter->GetOutput()->GetVTKData()->Update();
	wrappedMeter->Modified();
	wrappedMeter->Update();

	double l1[3], l2[3], l3[3]; 
	cloud->GetLandmark(2)->GetPoint(l1);
	cloud->GetLandmark(3)->GetPoint(l2);
	cloud->GetLandmark(4)->GetPoint(l3);

	const short int controlDimension = 9;
	double controlValues[controlDimension];
	controlValues[0] = l1[0];controlValues[1] = l1[1];controlValues[2] = l1[2];
	controlValues[3] = l2[0];controlValues[4] = l2[1];controlValues[5] = l2[2];
	controlValues[6] = l3[0];controlValues[7] = l3[1];controlValues[8] = l3[2];

	//printf("\n%.2f %.2f %.2f\n", l1[0] , l1[1], l1[2]);
	//printf("\n%.2f %.2f %.2f\n", l2[0] , l2[1], l2[2]);
	//printf("\n%.2f %.2f %.2f\n", l3[0] , l3[1], l3[2]);

	//Inizialize exporter
	medOpExporterWrappedMeter *exporter=new medOpExporterWrappedMeter("test exporter");
	exporter->SetInput(wrappedMeter);
	mafString fileExp=MED_DATA_ROOT;
	fileExp<<"/RAW_MAL/ExportWrappedMeter.txt";
	exporter->TestModeOn();
	exporter->SetFileName(fileExp);
	exporter->ExportWrappedMeterCoordinates();

	std::fstream control(fileExp);

	int result = MAF_OK;
	double pos1, pos2, pos3;

	short int counter = 0;
	while(counter < controlDimension / 3)
	{
		control >> pos1;
		control >> pos2;
		control >> pos3;

		if((pos1-controlValues[counter*3]  > 0.01)   || 
			(pos2-controlValues[counter*3+1] > 0.01)   ||
			(pos3-controlValues[counter*3+2] > 0.01)   )
			result = MAF_ERROR;

		counter++;
	}

	if(result == MAF_OK){

	}
	control.close();

	wrappedMeter->SetParent(NULL);

	mafDEL(exporter);
	mafDEL(wrappedMeter);
	cppDEL(importer);

	//mafDEL(storage);

}