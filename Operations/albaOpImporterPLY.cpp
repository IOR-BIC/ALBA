/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpImporterPLY
 Authors: Gianluigi Crimi
 
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

#include "albaOpImporterPLY.h"
#include "albaGUIBusyInfo.h"

#include "albaDecl.h"
#include "albaTagItem.h"
#include "albaTagArray.h"
#include "albaVME.h"
#include "albaVMESurface.h"
#include "albaSmartPointer.h"
#include "vtkALBASmartPointer.h"

#include "vtkPLYReader.h"
#include "vtkPolyData.h"

#include "wx\filename.h"

#include <fstream>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpImporterPLY);

//----------------------------------------------------------------------------
albaOpImporterPLY::albaOpImporterPLY(const wxString &label) :
albaOp(label)
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_Files.clear();
}
//----------------------------------------------------------------------------
albaOpImporterPLY::~albaOpImporterPLY()
{
  for(unsigned i = 0; i < m_ImportedPLYs.size(); i++)
    albaDEL(m_ImportedPLYs[i]);
}
//----------------------------------------------------------------------------
bool albaOpImporterPLY::InternalAccept(albaVME*node)
{
  return true;
}
//----------------------------------------------------------------------------
albaOp* albaOpImporterPLY::Copy()   
{
  albaOpImporterPLY *cp = new albaOpImporterPLY(m_Label);
  cp->m_Files = m_Files;
  return cp;
}
//----------------------------------------------------------------------------
void albaOpImporterPLY::OpRun()   
{
	albaString	fileDir = albaGetLastUserFolder();

  if (!m_TestMode && m_Files.size() == 0)
  {
    albaString wildc = "Polygon File Format (*.ply)|*.ply";
    std::vector<wxString> files;
    albaString f;

    m_Files.clear();
    albaGetOpenMultiFiles(fileDir.GetCStr(),wildc.GetCStr(), files);
    for(unsigned i = 0; i < files.size(); i++)
    {
      f = files[i];
      m_Files.push_back(f);
    }
  }
	
	int result = OP_RUN_CANCEL;

	if(m_Files.size() != 0) 
	{
		result = OP_RUN_OK;
    ImportPLY();
	}

	albaEventMacro(albaEvent(this,result));
}
//----------------------------------------------------------------------------
void albaOpImporterPLY::OpDo()
{
  for(unsigned i = 0; i < m_ImportedPLYs.size(); i++)
  {
    if (m_ImportedPLYs[i])
    {
      m_ImportedPLYs[i]->ReparentTo(m_Input);
    }
  }
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaOpImporterPLY::OpUndo()
{
  for(unsigned i = 0; i < m_ImportedPLYs.size(); i++)
  {
    if (m_ImportedPLYs[i])
    {
      GetLogicManager()->VmeRemove(m_ImportedPLYs[i]);
    }
  }
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
int albaOpImporterPLY::ImportPLY()
{
  albaGUIBusyInfo wait("Loading file: ...",m_TestMode);  

  unsigned int i;
  for(i = 0; i < m_ImportedPLYs.size(); i++)
    albaDEL(m_ImportedPLYs[i]);
  m_ImportedPLYs.clear();

  for(unsigned kk = 0; kk < m_Files.size(); kk++)
  {
    albaString fn;
    fn = m_Files[kk];
  
    vtkALBASmartPointer<vtkPLYReader> reader;
	  albaEventMacro(albaEvent(this,BIND_TO_PROGRESSBAR,reader));
    reader->SetFileName(fn);
	  reader->Update();

    wxString path, name, ext;
    wxFileName::SplitPath(fn.GetCStr(),&path,&name,&ext);

    albaVMESurface *importedPLY;
    albaNEW(importedPLY);
    importedPLY->SetName(name);
	  importedPLY->SetDataByDetaching(reader->GetOutput(),0);

    albaTagItem tag_Nature;
    tag_Nature.SetName("VME_NATURE");
    tag_Nature.SetValue("NATURAL");
    importedPLY->GetTagArray()->SetTag(tag_Nature);

	  
    m_ImportedPLYs.push_back(importedPLY);
  }

	return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaOpImporterPLY::SetFileName(const char *file_name)
{
  m_Files.resize(1);
  m_Files[0] = file_name;
}
//----------------------------------------------------------------------------
void albaOpImporterPLY::GetImportedPLY(std::vector<albaVMESurface*> &importedPLY)
{
  importedPLY.clear();
  importedPLY.resize(m_ImportedPLYs.size());
  for (unsigned int i=0; i< m_ImportedPLYs.size(); i++)
  {
    importedPLY[i] = m_ImportedPLYs[i];
  }
}

