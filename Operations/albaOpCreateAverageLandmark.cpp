/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateAverageLandmark
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved.

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

#include "albaOpCreateAverageLandmark.h"
#include "albaDecl.h"
#include "albaEvent.h"

#include "albaVMERoot.h"
#include "albaVME.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpCreateAverageLandmark);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpCreateAverageLandmark::albaOpCreateAverageLandmark(const wxString &label) :
albaOp(label)
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;

	m_LimbCloud = NULL;
	m_NewIndex = 0;
}
//----------------------------------------------------------------------------
albaOpCreateAverageLandmark::~albaOpCreateAverageLandmark()
{

}
//----------------------------------------------------------------------------
albaOp* albaOpCreateAverageLandmark::Copy()   
{
	return new albaOpCreateAverageLandmark(m_Label);
}
//----------------------------------------------------------------------------
bool albaOpCreateAverageLandmark::InternalAccept(albaVME*node)
{
	return (node && node->IsA("albaVMELandmarkCloud"));	
}

//----------------------------------------------------------------------------
void albaOpCreateAverageLandmark::OpRun()
{
	m_LimbCloud = (albaVMELandmarkCloud *)m_Input;

  albaEventMacro(albaEvent(this,OP_RUN_OK));
}

//----------------------------------------------------------------------------
void albaOpCreateAverageLandmark::OpDo()
{
	int nI, nJ;
	std::vector<albaTimeStamp> kframes;

	m_LimbCloud->GetTimeStamps(kframes);
	char newLMName[100];
	int  avInd = 0;
	sprintf(newLMName, "Average");

	while (m_LimbCloud->GetLandmarkIndex(newLMName) >= 0)
	{
		sprintf(newLMName, "Average%d", avInd);
		avInd++;
	}
	m_NewIndex = m_LimbCloud->AppendLandmark(newLMName);

	for (nI = 0; nI < kframes.size(); nI++)
	{
		double xa = 0.0, ya = 0.0, za = 0.0;
		
		for (nJ = 0; nJ < m_LimbCloud->GetNumberOfLandmarks(); nJ++)
		{
			double x, y, z;
			if (m_NewIndex == nJ)
				continue;
			m_LimbCloud->GetLandmark(nJ, x, y, z, kframes[nI]);
			xa += x;
			ya += y;
			za += z;
		}
		xa /= m_LimbCloud->GetNumberOfLandmarks() - 1;
		ya /= m_LimbCloud->GetNumberOfLandmarks() - 1;
		za /= m_LimbCloud->GetNumberOfLandmarks() - 1;
		m_LimbCloud->SetLandmark(m_NewIndex, xa, ya, za, kframes[nI]);
		m_LimbCloud->Modified();
	}

	
	GetLogicManager()->VmeShow(m_LimbCloud->GetLandmark(newLMName), true); 
	GetLogicManager()->CameraUpdate();

	return;
}

//----------------------------------------------------------------------------
void albaOpCreateAverageLandmark::OpUndo()
{
	m_LimbCloud->RemoveLandmark(m_NewIndex);
	m_LimbCloud->Modified();

	//Restore previous selection
	GetLogicManager()->VmeSelect(m_Input);
	GetLogicManager()->CameraUpdate();
}
