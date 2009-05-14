/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFLargeDataSetCallback.cxx,v $ 
  Language: C++ 
  Date: $Date: 2009-05-14 15:03:31 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "mafDefines.h"		//must be included first
#include "vtkObject.h"
#include "vtkMAFLargeDataSetCallback.h"


/*virtual*/ void vtkMAFLargeDataSetCallback
	::Execute(vtkObject* caller, unsigned long eventId, void* callData)
{	
	if (m_Listener != NULL)
	{
		if (eventId == vtkCommand::ProgressEvent)
		{
			mafEvent ev(this, PROGRESSBAR_SET_VALUE, (long)(*((double*)callData)*100));
			m_Listener->OnEvent(&ev);
		}
		else if (eventId == vtkCommand::StartEvent)
		{
			mafEvent ev(this, PROGRESSBAR_SHOW);
			m_Listener->OnEvent(&ev);

			ev.SetId(PROGRESSBAR_SET_VALUE);
			ev.SetArg(0);
			m_Listener->OnEvent(&ev);

			mafString szStr = caller->GetClassName(); //"Processing ";
			ev.SetId(PROGRESSBAR_SET_TEXT);
			ev.SetString(&szStr);
		}
		else if (eventId == vtkCommand::EndEvent)
		{
			mafEvent ev(this, PROGRESSBAR_HIDE);
			m_Listener->OnEvent(&ev);
		}
	}
}	