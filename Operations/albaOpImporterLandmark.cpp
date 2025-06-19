/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterLandmark
 Authors: Daniele Giunchi, Simone Brazzale
 
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

#include "albaOpImporterLandmark.h"
#include <albaGUIBusyInfo.h>


#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaVME.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaTagArray.h"
#include "albaSmartPointer.h"

#include <fstream>
#include "albaProgressBarHelper.h"
#include "wx/filename.h"


//----------------------------------------------------------------------------
albaOpImporterLandmark::albaOpImporterLandmark(wxString label) : albaOp(label)
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo	= false;
  m_TypeSeparation = 0;
	
	m_VmeCloud		= NULL;
	m_OnlyCoordinates = false;
}
//----------------------------------------------------------------------------
albaOpImporterLandmark::~albaOpImporterLandmark( ) 
{
  albaDEL(m_VmeCloud);
}
//----------------------------------------------------------------------------
albaOp* albaOpImporterLandmark::Copy()   
{
	albaOpImporterLandmark *cp = new albaOpImporterLandmark(m_Label);
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
	cp->m_Listener = m_Listener;
	cp->m_Next = NULL;

	cp->m_OnlyCoordinates = m_OnlyCoordinates;
	cp->m_VmeCloud = m_VmeCloud;
  cp->m_TypeSeparation = m_TypeSeparation;
	return cp;
}
//----------------------------------------------------------------------------
void albaOpImporterLandmark::OpRun()   
{
	wxString fileDir;
	
	m_File = "";
	wxString pgd_wildc	= "Landmark (*.*)|*.*";

	fileDir = albaGetLastUserFolder();
	  
	albaString f = albaGetOpenFile(fileDir,pgd_wildc); 
	if(f != "")
	{
	  m_File = f;

    if (!m_TestMode)
    {
      m_Gui = new albaGUI(this);
			wxString choices[4] = { _("Comma"),_("Space"),_("Semicolon"),_("Tab") };
      m_Gui->Radio(ID_TYPE_SEPARATION,"Separator",&m_TypeSeparation,4,choices,1,"");   
      m_Gui->Divider();
      m_Gui->Bool(ID_TYPE_FILE,"Coordinates only",&m_OnlyCoordinates,true,"Check if the format is \"x y z\"");
			m_Gui->Divider();
			m_Gui->Label("");
			m_Gui->Divider(1);
      m_Gui->OkCancel();
      ShowGui();
    }
	}
  else
  {
    albaEventMacro(albaEvent(this, OP_RUN_CANCEL));
  }

}
//----------------------------------------------------------------------------
void albaOpImporterLandmark::	OnEvent(albaEventBase *alba_event) 
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
		switch (e->GetId())
		{
			case wxOK:
				OpStop(OP_RUN_OK);
				break;
			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
				break;
			case ID_TYPE_FILE:
			case ID_TYPE_SEPARATION:
				break;
			default:
				albaEventMacro(*e);
		}
  }
}
//----------------------------------------------------------------------------
void albaOpImporterLandmark::OpDo()   
{
  Read();

  wxString path, name, ext;
  wxFileName::SplitPath(m_File.ToAscii(),&path,&name,&ext);
  m_VmeCloud->SetName(name);
	
	albaTagItem tag_Nature;
	tag_Nature.SetName("VME_NATURE");
	tag_Nature.SetValue("NATURAL");

	m_VmeCloud->GetTagArray()->SetTag(tag_Nature);
  
	GetLogicManager()->VmeAdd(m_VmeCloud);
}
//----------------------------------------------------------------------------
void albaOpImporterLandmark::OpStop(int result)
{
	HideGui();
	albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
void albaOpImporterLandmark::Read()   
{
  // need the number of landmarks for the progress bar
  std::ifstream  landmarkNumberPromptFileStream(m_File.ToAscii());
  int numberOfLines = 0;
	char line[512], separator;
	double x = 0, y = 0, z = 0, t = 0;
	long counter = 0, linesReaded = 0;
	wxString name;

  while(!landmarkNumberPromptFileStream.fail())
  {
    landmarkNumberPromptFileStream.getline(line,512);
    numberOfLines++;
  }
  landmarkNumberPromptFileStream.close();


	albaNEW(m_VmeCloud);
  if (m_TestMode == true)
  {
	  m_VmeCloud->TestModeOn();
  }

  std::ifstream  landmarkFileStream(m_File.ToAscii());
 
  albaProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Reading Landmark Cloud");
	
	switch (m_TypeSeparation) //_("Comma"),_("Space"),_("Semicolon"),_("Tab")
	{
		case 0:
			separator = ',';
			break;
		case 1:
			separator = ' ';
			break;
		case 2:
			separator = ';';
		case 3:
			separator = '\t';
			break;
	}
  

  while(!landmarkFileStream.fail())
  {
		landmarkFileStream.getline(line, 512);

    if(line[0] == '#' || albaString(line) == "") 
    {
			//skip comments and empty lines
			linesReaded++;
      continue;
    }
    else if(strncmp(line, "Time",4)==0)
    {
			char *time = line + 5;
			t = atof(time);
      counter = 0;
			linesReaded++;
      continue;
    }
    else
		{      
			ConvertLine(line, counter, separator, name, x, y, z);
     
			if (m_VmeCloud->GetLandmarkIndex(name.ToAscii()) == -1)
			{
				//New Landmark
				m_VmeCloud->AppendLandmark(x, y, z, name);
				if (t != 0)
				{
					//this landmark is present only 
					int idx = m_VmeCloud->GetLandmarkIndex(name);
					m_VmeCloud->SetLandmarkVisibility(idx, false, 0);
				}

			}
			else
			{
				//Set an existing landmark
				m_VmeCloud->SetLandmark(name, x, y, z, t);
			}

			bool visibility = !(x == -9999 && y == -9999 && z == -9999);
			m_VmeCloud->SetLandmarkVisibility(counter, visibility, t);
      
			counter++;
			linesReaded++;

      progressHelper.UpdateProgressBar(linesReaded * 100 / numberOfLines);
    }
  }
  m_VmeCloud->Modified();
  m_VmeCloud->ReparentTo(m_Input);

  landmarkFileStream.close();

  m_Output = m_VmeCloud;
}

void albaOpImporterLandmark::ConvertLine(char *line, int count, char separator, wxString &name, double &x, double &y, double &z)
{
	wxString str = wxString(line);

	if (m_OnlyCoordinates)
	{
		name = "LM ";
		name << count;
	}
	else
	{
		name = str.BeforeFirst(separator);
		str = str.After(separator);
	}

	wxString xStr = str.BeforeFirst(separator);
	str = str.AfterFirst(separator);
	wxString yStr = str.BeforeFirst(separator);
	wxString zStr = str.AfterFirst(separator);

	xStr.ToDouble(&x);
	yStr.ToDouble(&y);
	zStr.ToDouble(&z);
}
