/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExporterWrappedMeter
 Authors: Daniele Giunchi
 
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


#include "albaOpExporterWrappedMeter.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"

#include "albaVMEWrappedMeter.h"
#include "albaVMEOutputWrappedMeter.h"
#include "albaVMEComputeWrapping.h"

#include "albaOpImporterLandmark.h"
#include "albaDefines.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMERoot.h"
#include "albaVMEWrappedMeter.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "vtkDataSet.h"

#include <fstream>


//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpExporterWrappedMeter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpExporterWrappedMeter::albaOpExporterWrappedMeter(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_EXPORTER;
  m_Canundo = true;
  
  m_File    = "";
}
//----------------------------------------------------------------------------
albaOpExporterWrappedMeter::~albaOpExporterWrappedMeter( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
albaOp* albaOpExporterWrappedMeter::Copy()   
//----------------------------------------------------------------------------
{
	albaOpExporterWrappedMeter *cp = new albaOpExporterWrappedMeter(m_Label);
	cp->m_File = m_File;
	return cp;
}
//----------------------------------------------------------------------------
bool albaOpExporterWrappedMeter::InternalAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return (node && node->IsALBAType(albaVMEComputeWrapping));
}
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum WRAPPED_METER_EXPORTER_ID
{
  ID_CHOOSE_FILENAME = MINID,
};
//----------------------------------------------------------------------------
void albaOpExporterWrappedMeter::OpRun()   
//----------------------------------------------------------------------------
{
  albaString wildc = "Wrapped Meter Coordinates(*.txt)|*.txt";

  m_Gui = new albaGUI(this);
	//m_Gui->FileSave(ID_CHOOSE_FILENAME,"stl file", &m_File, wildc,"Save As...");
	m_Gui->OkCancel();
  
	m_Gui->Divider();

	ShowGui();
}
//----------------------------------------------------------------------------
void albaOpExporterWrappedMeter::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
    switch(e->GetId())
    {
      case wxOK:
				{
				  albaString initialFileName;
          initialFileName = albaGetLastUserFolder().c_str();
          initialFileName.Append("\\ActionLine.txt");

				  albaString wildc = "configuration file (*.txt)|*.txt";
				  m_File = albaGetSaveFile(initialFileName.GetCStr(), wildc).c_str();

				  if (m_File == "") return;

				  //Test();		  //using this method to try test case
				  Export();
				  
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
void albaOpExporterWrappedMeter::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	albaEventMacro(albaEvent(this,result));        
}

//----------------------------------------------------------------------------
void albaOpExporterWrappedMeter::Export()
//----------------------------------------------------------------------------
{

	wxBusyInfo *wait;
	if(!m_TestMode)
	{
		wait = new wxBusyInfo("Please wait, Exporting...");
	}


	m_OutputFile.open(m_File, std::ios::out);
	if (m_OutputFile.fail()) {
		wxMessageBox("Error opening configuration file");
		return ;
	}

	//must be a cicle in all vme of a msf
	/*albaVMEIterator *iter = NULL;
	iter = m_Input->GetRoot()->NewIterator();

	for (albaVME *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
	{
			m_Meters.push_back(node);
	}
	iter->Delete();
	*/
	
	m_Meters.push_back(m_Input);

	if(m_Meters.size() != 0)
	{
		ExportWrappedMeter();
	}

	m_Meters.clear();
	if(!m_TestMode)
	{
		delete wait;
	}


}
//----------------------------------------------------------------------------
void albaOpExporterWrappedMeter::ExportWrappedMeter()
//----------------------------------------------------------------------------
{

	albaVMERoot *root = ((albaVMERoot*)m_Meters[0]->GetRoot());
	root->GetTimeStamps(m_Times);

	//for(int j=0; j< 10; j++)//for test output format
	for(int j=0; j< m_Times.size(); j++)
	{
		m_CurrentTime = m_Times[j];
		albaEventMacro(albaEvent(this, TIME_SET, m_CurrentTime, 0));

		for(int i=0;i< m_Meters.size();i++)
		{
			m_CurrentVme = m_Meters[i];

			if(albaVMEComputeWrapping::SafeDownCast(m_CurrentVme))
			{
				ExportWrappedMeterCoordinates(i,j);
			}
		}
	}

	WriteOnFile();
}
//----------------------------------------------------------------------------
void albaOpExporterWrappedMeter::ExportWrappedMeterCoordinates(int index, int indexTime)
//----------------------------------------------------------------------------
{

	albaVMEComputeWrapping *vmeWrappedMeter =  albaVMEComputeWrapping::SafeDownCast(m_CurrentVme);
	vmeWrappedMeter->GetOutput()->GetVTKData()->Modified();
	vmeWrappedMeter->GetOutput()->GetVTKData()->Update();
	vmeWrappedMeter->Modified();
	vmeWrappedMeter->Update();
	int size;
	int numberOfKeyPoints;

	if(indexTime == 0)
	{
		vnl_matrix<double> M;
		size = albaVMEComputeWrapping::SafeDownCast(m_CurrentVme)->GetNumberExportPoints();
		
		int numberOfRows = m_Times.size();
		int numberOfColumns = (2+4) *3;//(start + 4 key points + end )*3
		
		M.set_size(numberOfRows, numberOfColumns);
		
		m_MetersCoordinatesList.push_back(M);
	}
	
	if (vmeWrappedMeter->GetWrappedClass()==albaVMEComputeWrapping::NEW_METER)
	{
		//origin
		double *value = vmeWrappedMeter->GetStartPointCoordinate();
		m_MetersCoordinatesList[index].put(indexTime,0,value[0]);
		m_MetersCoordinatesList[index].put(indexTime,1,value[1]);
		m_MetersCoordinatesList[index].put(indexTime,2,value[2]);

		//middle points
		numberOfKeyPoints = vmeWrappedMeter->GetNumberExportPoints();//vmeWrappedMeter->GetNumberMiddlePoints();
		for(int k=0; k< numberOfKeyPoints;k++)
		{
			value = vmeWrappedMeter->GetExportPointCoordinate(k);
			m_MetersCoordinatesList[index].put(indexTime,3*(k+1),value[0]);
			m_MetersCoordinatesList[index].put(indexTime,3*(k+1)+1,value[1]);
			m_MetersCoordinatesList[index].put(indexTime,3*(k+1)+2,value[2]);
		}

		//end point
		value = vmeWrappedMeter->GetEndPointCoordinate();
		m_MetersCoordinatesList[index].put(indexTime,3*5,value[0]);
		m_MetersCoordinatesList[index].put(indexTime,3*5+1,value[1]);
		m_MetersCoordinatesList[index].put(indexTime,3*5+2,value[2]);
	}else if (vmeWrappedMeter->GetWrappedClass()==albaVMEComputeWrapping::OLD_METER)
	{
		if (vmeWrappedMeter->GetWrappedMode2() == albaVMEComputeWrapping::MANUAL_WRAP)
		{
			for(int i=0; i<vmeWrappedMeter->GetNumberMiddlePoints();i++)
			{
				m_OutputFile << vmeWrappedMeter->GetMiddlePointCoordinate(i)[0] << '\t'
					<< vmeWrappedMeter->GetMiddlePointCoordinate(i)[1] << '\t'
					<< vmeWrappedMeter->GetMiddlePointCoordinate(i)[2] << std::endl;
			}
		}else if (vmeWrappedMeter->GetWrappedMode2() == albaVMEComputeWrapping::AUTOMATED_WRAP)
		{
			m_OutputFile << vmeWrappedMeter->GetWrappedGeometryTangent1()[0] << '\t'
				<< vmeWrappedMeter->GetWrappedGeometryTangent1()[1] << '\t'
				<< vmeWrappedMeter->GetWrappedGeometryTangent1()[2] << std::endl;

			m_OutputFile << vmeWrappedMeter->GetWrappedGeometryTangent2()[0] << '\t'
				<< vmeWrappedMeter->GetWrappedGeometryTangent2()[1] << '\t'
				<< vmeWrappedMeter->GetWrappedGeometryTangent2()[2] << std::endl;
		}
	}
	m_KeyNumList.push_back(numberOfKeyPoints);
	//WriteOnFile(numberOfKeyPoints);
}


//----------------------------------------------------------------------------
//num is number of key points
void albaOpExporterWrappedMeter::WriteOnFile()
//----------------------------------------------------------------------------
{
	
	unsigned int columns;
	unsigned int rows;
	int nKey = 4; 
	double value ;


	for(int i=0; i< m_Meters.size(); i++)
	{
		m_OutputFile << m_Meters[i]->GetName() << std::endl; // vme name
		m_OutputFile << std::setw(6)<<"time"<<std::setw(4)<<std::setw(30)<<"origin"<< std::setw(30)<< "viaPoint1"<< std::setw(30)<<"viaPoint2"<< std::setw(30)<<"viaPoint3"<< std::setw(30)<<"viaPoint4"<< std::setw(30)<<"insertion"<<'\t'<<std::endl;
		columns = m_MetersCoordinatesList[i].columns();
		rows = m_MetersCoordinatesList[i].rows();

		for(int m=0; m< rows; m++){
			m_OutputFile << std::fixed << std::setw(6) << m<<std::setw(4)<< '\t';//output time
			int num = m_KeyNumList[m];

			for(int n=0; n< columns; n++)
			{
				if ( n<(num+1)*3  ||   n>= (nKey+1)*3  )//start and key points and end point
				{
					value =  m_MetersCoordinatesList[i].get(m,n);
					if (n%3 == 0)
					{
						//m_OutputFile << std::fixed << std::setprecision(3) << std::setw(8) << '['<<value <<','<< '\t' ;
						m_OutputFile << std::fixed << std::setprecision(3) << '['<< std::setw(8)<<value <<','<< '\t' ;
					}else if (n%3 == 1)
					{
						m_OutputFile << std::fixed << std::setprecision(3) << std::setw(8)<<  value <<','<< "\t";
					}else if (n%3 == 2)
					{
						m_OutputFile << std::fixed << std::setprecision(3) << std::setw(8)<< value <<']'<< "\t";
					}	
				}else if (  n >= (num+1)*3  && n< (nKey+1)*3 )
				{
					if ( n%3 == 0)
					{
						m_OutputFile << std::fixed << std::setprecision(3) << std::setw(16) << "NAN"<<std::setw(13)<< '\t' ;
					}
				}				
			}
			m_OutputFile << std::endl;
		}
		m_OutputFile.close();
	}
	
	
}

/*albaVMEWrappedMeter *vmeWrappedMeter =  albaVMEWrappedMeter::SafeDownCast(m_Input);
vmeWrappedMeter->Update();
albaVMEOutputWrappedMeter *out_wm = albaVMEOutputWrappedMeter::SafeDownCast(vmeWrappedMeter->GetOutput());
out_wm->Update();*/
/*
//----------------------------------------------------------------------------
void albaOpExporterWrappedMeter::ExportWrappedMeterCoordinates()
//----------------------------------------------------------------------------
{


  medVMEComputeWrapping *vmeMeter = medVMEComputeWrapping::SafeDownCast(m_Input);
  vmeMeter->Update();
  albaVMEOutputComputeWrapping *out_cm = albaVMEOutputComputeWrapping::SafeDownCast(vmeMeter->GetOutput());
  out_cm->Update();

  std::ofstream outputFile(m_File, std::ios::out);

  if (outputFile == NULL) {
    wxMessageBox("Error opening configuration file");
    return ;
  }

  if (vmeMeter->GetWrappedClass()==medVMEComputeWrapping::NEW_METER)
  {

	  outputFile<<std::setw(10)<<"time"<< std::setw(30)<<"origin"<< std::setw(30)<< "viaPoint1"<< std::setw(30)<<"viaPoint2"<< std::setw(30)<<"viaPointn"<< std::setw(30)<<"viaPointn"<< std::setw(30)<<"insertion"<<'\t'<<std::endl;

	  outputFile<< '1' << '[' <<
		   std::fixed << std::setprecision(3) << std::setw(8)<< vmeMeter->GetStartPointCoordinate()[0]<<'\t'<<','<<
		   std::fixed << std::setprecision(3) << std::setw(8)<< vmeMeter->GetStartPointCoordinate()[1]<<'\t' <<','<<
		   std::fixed << std::setprecision(3) << std::setw(8)<< vmeMeter->GetStartPointCoordinate()[2]<<']';

	  int size = vmeMeter->GetNumberExportPoints();
	  double *point;
	  if (size>0)
	  {
		  for (int i=0;i<size;i++)
		  {
			  outputFile <<'['<<
				  std::fixed << std::setprecision(3) << std::setw(8) << vmeMeter->GetExportPointCoordinate(i)[0]  << '\t'<<','<<
				  std::fixed << std::setprecision(3) << std::setw(8) << vmeMeter->GetExportPointCoordinate(i)[1] << '\t'<<','<< 
				  std::fixed << std::setprecision(3) << std::setw(8) << vmeMeter->GetExportPointCoordinate(i)[2] << '\t'<<']';
		  }
	  }
		for (int j=0;j<4-size;j++)
		{
			outputFile<< std::setw(30)<<"NAN"<<'\t';
		}

	  outputFile<<'['<<
		  std::fixed<<std::setprecision(3)<<std::setw(8)<<vmeMeter->GetEndPointCoordinate()[0]<< '\t'<<','<<
		  std::fixed<<std::setprecision(3)<<std::setw(8)<<vmeMeter->GetEndPointCoordinate()[1]<< '\t'<<','<<
		  std::fixed<<std::setprecision(3)<<std::setw(8)<<vmeMeter->GetEndPointCoordinate()[2]<< '\t'<<']'<<std::endl;

  }else if (vmeMeter->GetWrappedClass()==medVMEComputeWrapping::OLD_METER)
  {
	  if (vmeMeter->GetWrappedMode2() == medVMEComputeWrapping::MANUAL_WRAP)
	{
		for(int i=0; i<vmeMeter->GetNumberMiddlePoints();i++)
		{
			outputFile << vmeMeter->GetMiddlePointCoordinate(i)[0] << '\t'
				<< vmeMeter->GetMiddlePointCoordinate(i)[1] << '\t'
				<< vmeMeter->GetMiddlePointCoordinate(i)[2] << std::endl;
		}
	}else if (vmeMeter->GetWrappedMode2() == medVMEComputeWrapping::AUTOMATED_WRAP)
	{
		outputFile << vmeMeter->GetWrappedGeometryTangent1()[0] << '\t'
			<< vmeMeter->GetWrappedGeometryTangent1()[1] << '\t'
			<< vmeMeter->GetWrappedGeometryTangent1()[2] << std::endl;

		outputFile << vmeMeter->GetWrappedGeometryTangent2()[0] << '\t'
			<< vmeMeter->GetWrappedGeometryTangent2()[1] << '\t'
			<< vmeMeter->GetWrappedGeometryTangent2()[2] << std::endl;
	}
  }
  



  
  outputFile.close();
  

}
*/
/*if(vmeWrappedMeter->GetWrappedMode() == albaVMEWrappedMeter::MANUAL_WRAP)
{
for(int i=0; i<vmeWrappedMeter->GetNumberMiddlePoints();i++)
{
outputFile << vmeWrappedMeter->GetMiddlePointCoordinate(i)[0] << '\t'
<< vmeWrappedMeter->GetMiddlePointCoordinate(i)[1] << '\t'
<< vmeWrappedMeter->GetMiddlePointCoordinate(i)[2] << std::endl;
}
}
else if(vmeWrappedMeter->GetWrappedMode() == albaVMEWrappedMeter::AUTOMATED_WRAP)
{
outputFile << vmeWrappedMeter->GetWrappedGeometryTangent1()[0] << '\t'
<< vmeWrappedMeter->GetWrappedGeometryTangent1()[1] << '\t'
<< vmeWrappedMeter->GetWrappedGeometryTangent1()[2] << std::endl;

outputFile << vmeWrappedMeter->GetWrappedGeometryTangent2()[0] << '\t'
<< vmeWrappedMeter->GetWrappedGeometryTangent2()[1] << '\t'
<< vmeWrappedMeter->GetWrappedGeometryTangent2()[2] << std::endl;
}*/
//-----------------------------------------------------------
void albaOpExporterWrappedMeter::Test() 
//-----------------------------------------------------------
{
/*	albaVMEStorage *storage = albaVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

*/
	//create landmarks and relative landmark cloud
	albaOpImporterLandmark *importer=new albaOpImporterLandmark("importer");
	importer->TestModeOn();
	//importer->SetInput(storage->GetRoot());
	//albaString filename=ALBA_DATA_ROOT;
	albaString filename = "C:\\ALBA\\Medical\\Testing\\unittestData\\RAW_MAL\\cloud_to_be_imported";
	//filename<<"../unittestData";
	//filename<<"/RAW_MAL/cloud_to_be_imported";
	importer->SetFileName(filename.GetCStr());
	importer->Read();
	
	albaVMELandmarkCloud *cloud=(albaVMELandmarkCloud *)importer->GetOutput();

	albaVMEComputeWrapping *wrappedMeter;
	albaNEW(wrappedMeter);

	wrappedMeter->SetMeterLink("StartVME",cloud->GetLandmark(0));
	wrappedMeter->SetMeterLink("EndVME1",cloud->GetLandmark(1));

	wrappedMeter->SetMeterLink(cloud->GetLandmark(2)->GetName(),cloud->GetLandmark(2));
	wrappedMeter->SetMeterLink(cloud->GetLandmark(3)->GetName(),cloud->GetLandmark(3));
	wrappedMeter->SetMeterLink(cloud->GetLandmark(4)->GetName(),cloud->GetLandmark(4));
	//wrappedMeter->PushIdVector(cloud->GetId()); // for landmark middlepoint is memorized as sequence of cloud id and interal id of the landmark
	//wrappedMeter->PushIdVector(2); //this is for the vector syncronized with the gui widget, that is not used in gui test


	wrappedMeter->ReparentTo(cloud);
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
	albaOpExporterWrappedMeter *exporter=new albaOpExporterWrappedMeter("test exporter");
	exporter->SetInput(wrappedMeter);
	albaString fileExp=ALBA_DATA_ROOT;
	fileExp<<"/RAW_MAL/ExportWrappedMeter.txt";
	exporter->TestModeOn();
	exporter->SetFileName(fileExp);
	exporter->ExportWrappedMeterCoordinates(0,0);

	std::fstream control(fileExp);

	int result = ALBA_OK;
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
			result = ALBA_ERROR;

		counter++;
	}

	if(result == ALBA_OK){

	}
	control.close();

	wrappedMeter->ReparentTo(NULL);

	albaDEL(exporter);
	albaDEL(wrappedMeter);
	cppDEL(importer);
}