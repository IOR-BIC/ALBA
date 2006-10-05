/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoLandmarkExporter.cpp,v $
  Language:  C++
  Date:      $Date: 2006-10-05 08:44:58 $
  Version:   $Revision: 1.1 $
  Authors:   Stefania Paperini , Daniele Giunchi (porting MAf 2.0)
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

#include "mmoLandmarkExporter.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mmgGui.h"

#include <iostream>
#include <fstream>
#include <vector>

#include "mafVMERoot.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMEGroup.h"
#include "mafNodeIterator.h"
#include "mafVMERawMotionData.h"

#include "vtkDataSetReader.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkPolyDataWriter.h"
#include "vtkRectilinearGridWriter.h"
#include "vtkStructuredPointsWriter.h"

//----------------------------------------------------------------------------
mmoLandmarkExporter::mmoLandmarkExporter(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_File = "";
  m_Input = NULL;
  m_State = NULL;

}
//----------------------------------------------------------------------------
mmoLandmarkExporter::~mmoLandmarkExporter( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum 
{
  
};
//----------------------------------------------------------------------------
bool mmoLandmarkExporter::Accept(mafNode *node)   
//----------------------------------------------------------------------------
{ 
  if(node && !node->IsMAFType(mafVMERoot) && node->IsMAFType(mafVMELandmarkCloud))
    return true;

  for(int i=0; i<node->GetNumberOfChildren(); i++)
  {
    if(node->GetChild(i)->IsMAFType(mafVMELandmarkCloud)) return true;
  }

  return false;
}
//----------------------------------------------------------------------------
void mmoLandmarkExporter::OpRun()   
//----------------------------------------------------------------------------
{
	assert(m_Input);
	wxString proposed = (mafGetApplicationDirectory() + "/Data/External/").c_str();
	proposed += m_Input->GetName();
	proposed += ".txt";
	wxString wildc = "ascii file (*.txt)|*.txt";
	wxString f = mafGetSaveFile(proposed,wildc).c_str(); 

	int result = OP_RUN_CANCEL;
	if(f != "") 
		{
			m_File = f;
			ExportLandmark();
			result = OP_RUN_OK;
		}
	mafEventMacro(mafEvent(this,result));

}
/*//----------------------------------------------------------------------------
void mmoLandmarkExporter::OnEvent(mafEvent& e)
//----------------------------------------------------------------------------
{
	switch(e.GetId())
	{
		case VME_ADD:
		{
			//trap the VME_ADD of the mmoCollapse and mmoExplode to update the
			//m_input, then forward the message to mafDMLlogicMDI
			this->m_input = e.GetVme();
			mafEventMacro(mafEvent(this,VME_ADD,this->m_input));
		}
		break;
		default:
			mafEventMacro(e);
		break;
	}
}
*/
/*
//----------------------------------------------------------------------------
void mmoLandmarkExporter::OpStop(int result)
//----------------------------------------------------------------------------
{
	mafEventMacro(mafEvent(this,result));        
}
*/
//----------------------------------------------------------------------------
void mmoLandmarkExporter::ExportLandmark()   
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
	  wxBusyInfo wait("Saving landmark position: Please wait");
  }
  //file creation
	const char* fileName = (m_File);
	std::ofstream f_Out;
	f_Out.open(fileName);

  if(m_Input->IsMAFType(mafVMELandmarkCloud))
  {

    std::vector<mafTimeStamp> timeStamps;
    mafVME *vmeTemp = mafVME::SafeDownCast(m_Input);
    vmeTemp->GetTimeStamps(timeStamps);

    mafVMELandmarkCloud *vmeCloud = mafVMELandmarkCloud::SafeDownCast(vmeTemp);

		int numberLandmark = vmeCloud->GetNumberOfLandmarks();
    
    // if cloud is closed , open it

  	if (!vmeCloud->IsOpen())
		{
      vmeCloud->Open();
		}

    // pick up the values and write them into the file
    for (int index = 0; index < timeStamps.size(); index++)
		{		
		  f_Out<< "Time" <<"\t"<< timeStamps[index] <<"\n";	
			
      for(int j=0; j < numberLandmark; j++)
			{
			  wxString name = vmeCloud->GetLandmarkName(j);
				const char* nameLandmark = (name);																				
											
				mafVMELandmark *landmark = vmeCloud->GetLandmark(nameLandmark);
																				
				double xLandmark, yLandmark, zLandmark;
				landmark->GetPoint(xLandmark,yLandmark,zLandmark,timeStamps[index]);
				f_Out<< nameLandmark<<"\t"<< xLandmark <<"\t"<< yLandmark<<"\t"<< zLandmark <<"\n";																																					
			} 
								
		}

    // and now, close the cloud
    if (vmeCloud->IsOpen())
		{
      vmeCloud->Close();
    }			
  
  }
  else
  {

    int numberChildren = m_Input->GetNumberOfChildren();

		if(numberChildren > 0)
		{	
		  std::vector<mafTimeStamp> timeStamps;
			mafVME *vmeTemp = mafVME::SafeDownCast(m_Input);
      vmeTemp->GetTimeStamps(timeStamps);

			m_State = new bool[numberChildren];					
			for (int i= 0; i< numberChildren; i++)
			{
        mafNode *child = vmeTemp->GetChild(i);
				if (child->IsMAFType(mafVMELandmarkCloud))
				{
				  mafVMELandmarkCloud *vmeCloud = mafVMELandmarkCloud::SafeDownCast(child);
					m_State[i] = vmeCloud->IsOpen();
          
           // if cloud is closed , open it
					if (!m_State[i])
					{
					  
						vmeCloud->Open();
						
	        }
         			
				} 
			}
      // pick up the values and write them into the file
      for (int index = 0; index < timeStamps.size(); index++)
			{
        f_Out<< "Time" <<"\t"<< timeStamps[index] <<"\n";
        
				for (int i=0; i< numberChildren; i++)
				{
				  mafNode *child = m_Input->GetChild(i);
					if (child->IsMAFType(mafVMELandmarkCloud))
					{
					  mafVMELandmarkCloud *vmeCloud = mafVMELandmarkCloud::SafeDownCast(child);																		
						int numberLandmark = vmeCloud->GetNumberOfLandmarks();
																												
            for(int j=0; j < numberLandmark; j++)
						{
						  wxString name = vmeCloud->GetLandmarkName(j);
							const char* nameLandmark = (name);																				
															
							mafVMELandmark *landmark = vmeCloud->GetLandmark(nameLandmark);
																								
							double xLandmark, yLandmark, zLandmark;
							landmark->GetPoint(xLandmark,yLandmark,zLandmark,timeStamps[index]);
							f_Out<< nameLandmark<<"\t"<< xLandmark <<"\t"<< yLandmark<<"\t"<< zLandmark <<"\n";																																					
						} 


					}
										
				}
						
			}
       // and now, close the cloud
      for (i=0; i< numberChildren; i++)
      {
		  	mafNode *child = m_Input->GetChild(i);
								
			  if (child->IsMAFType(mafVMELandmarkCloud))
			  {
			    mafVMELandmarkCloud *vmeCloud = mafVMELandmarkCloud::SafeDownCast(child);	
				  if (!m_State[i])
				  {
				    vmeCloud->Close();
          }

			  }

      }
					
    }
  }

  f_Out.close();
	if(m_State)
		{
			delete[] m_State;
			m_State = NULL;
		}

}
/*
//----------------------------------------------------------------------------
void mmoLandmarkExporter::OpUndo()   
/**  
//----------------------------------------------------------------------------
{
  //nothing to do; will you erase the file ? 
}
*/
//----------------------------------------------------------------------------
mafOp* mmoLandmarkExporter::Copy()   
/** restituisce una copia di se stesso, serve per metterlo nell'undo stack */
//----------------------------------------------------------------------------
{
  mmoLandmarkExporter *cp = new mmoLandmarkExporter(m_Label);
  cp->m_Canundo = m_Canundo;
  cp->m_OpType = m_OpType;
  cp->m_Listener = m_Listener;
  cp->m_Next = NULL; // senza riscontri ho cmq messo la maiuscola

  cp->m_File = m_File;
  cp->m_Input = m_Input;
  return cp;
}
