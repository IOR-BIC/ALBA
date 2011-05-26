/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFLargeDataSetCallback.cxx,v $ 
  Language: C++ 
  Date: $Date: 2011-05-26 08:51:00 $ 
  Version: $Revision: 1.1.2.3 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "vtkMAFLargeDataSetCallback.h"

#include "mafObserver.h"

#include "vtkObject.h"

/*virtual*/ void vtkMAFLargeDataSetCallback
	::Execute(vtkObject* caller, unsigned long eventId, void* callData)
{	
	if (Listener != NULL)
	{
		if (eventId == vtkCommand::ProgressEvent)
		{
			mafEvent ev(this, PROGRESSBAR_SET_VALUE, (long)(*((double*)callData)*100));
			Listener->OnEvent(&ev);
		}
		else if (eventId == vtkCommand::StartEvent)
		{
			mafEvent ev(this, PROGRESSBAR_SHOW);
			Listener->OnEvent(&ev);

			ev.SetId(PROGRESSBAR_SET_VALUE);
			ev.SetArg(0);
			Listener->OnEvent(&ev);

      mafString szStr = caller->GetClassName(); //"Processing ";
			ev.SetId(PROGRESSBAR_SET_TEXT);
			ev.SetString(&szStr);
		}
		else if (eventId == vtkCommand::EndEvent)
		{
			mafEvent ev(this, PROGRESSBAR_HIDE);
			Listener->OnEvent(&ev);
		}
	}
}
/*virtual*/ void vtkMAFLargeDataSetCallback::SetListener(mafObserver* listener)
  {
    Listener = listener;
  }