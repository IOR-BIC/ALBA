/*=========================================================================

 Program: MAF2
 Module: mafOpImporterLandmark
 Authors: Daniele Giunchi, Simone Brazzale
 
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

#include "mafOpImporterLandmark.h"
#include <wx/busyinfo.h>


#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafVME.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafTagArray.h"
#include "mafSmartPointer.h"

#include <fstream>
#include "mafProgressBarHelper.h"

const bool DEBUG_MODE = true;

//----------------------------------------------------------------------------
mafOpImporterLandmark::mafOpImporterLandmark(wxString label) : mafOp(label)
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo	= false;
  m_TypeSeparation = 0;
	
	m_VmeCloud		= NULL;
	m_CoordOnly = false;
}
//----------------------------------------------------------------------------
mafOpImporterLandmark::~mafOpImporterLandmark( ) 
{
  mafDEL(m_VmeCloud);
}
//----------------------------------------------------------------------------
mafOp* mafOpImporterLandmark::Copy()   
{
	mafOpImporterLandmark *cp = new mafOpImporterLandmark(m_Label);
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
	cp->m_Listener = m_Listener;
	cp->m_Next = NULL;

	cp->m_CoordOnly = m_CoordOnly;
	cp->m_VmeCloud = m_VmeCloud;
  cp->m_TypeSeparation = m_TypeSeparation;
	return cp;
}
//----------------------------------------------------------------------------
void mafOpImporterLandmark::OpRun()   
{
	wxString fileDir;
	
	m_File = "";
	wxString pgd_wildc	= "Landmark (*.*)|*.*";

	fileDir = mafGetLastUserFolder().c_str();
	  
	mafString f = mafGetOpenFile(fileDir,pgd_wildc).c_str(); 
	if(f != "")
	{
	  m_File = f;

    if (!m_TestMode)
    {
      m_Gui = new mafGUI(this);
			wxString choices[4] = { _("Comma"),_("Space"),_("Semicolon"),_("Tab") };
      m_Gui->Radio(ID_TYPE_SEPARATION,"Separator",&m_TypeSeparation,4,choices,1,"");   
      m_Gui->Divider();
      m_Gui->Bool(ID_TYPE_FILE,"Coordinates only",&m_CoordOnly,true,"Check if the format is \"x y z\"");
			m_Gui->Divider();
			m_Gui->Label("");
			m_Gui->Divider(1);
      m_Gui->OkCancel();
      ShowGui();
    }
	}
  else
  {
    mafEventMacro(mafEvent(this, OP_RUN_CANCEL));
  }

}
//----------------------------------------------------------------------------
void mafOpImporterLandmark::	OnEvent(mafEventBase *maf_event) 
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
				mafEventMacro(*e);
		}
  }
}
//----------------------------------------------------------------------------
void mafOpImporterLandmark::OpDo()   
{
  Read();

  wxString path, name, ext;
  wxSplitPath(m_File.c_str(),&path,&name,&ext);
  m_VmeCloud->SetName(name);
	
	mafTagItem tag_Nature;
	tag_Nature.SetName("VME_NATURE");
	tag_Nature.SetValue("NATURAL");

	m_VmeCloud->GetTagArray()->SetTag(tag_Nature);
  
	mafEventMacro(mafEvent(this,VME_ADD,m_VmeCloud));
}
//----------------------------------------------------------------------------
void mafOpImporterLandmark::OpStop(int result)
{
	HideGui();
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mafOpImporterLandmark::Read()   
{
  // need the number of landmarks for the progress bar
  std::ifstream  landmarkNumberPromptFileStream(m_File);
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


	mafNEW(m_VmeCloud);
  if (m_TestMode == true)
  {
	  m_VmeCloud->TestModeOn();
  }

  std::ifstream  landmarkFileStream(m_File);
 
  mafProgressBarHelper progressHelper(m_Listener);
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

    if(line[0] == '#' || mafString(line) == "") 
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
     
			if (m_VmeCloud->GetLandmarkIndex(name.c_str()) == -1)
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

			bool visibility = (x == -9999 && y == -9999 && z == -9999);
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

void mafOpImporterLandmark::ConvertLine(char *line, int count, char separator, wxString &name, double &x, double &y, double &z)
{
	wxString str = wxString(line);

	if (m_CoordOnly)
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
