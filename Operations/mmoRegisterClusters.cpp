/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoRegisterClusters.cpp,v $
  Language:  C++
  Date:      $Date: 2006-11-28 10:24:21 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani - porting Daniele Giunchi  
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDecl.h"
#include <wx/busyinfo.h>

#include "mmoRegisterClusters.h"

#include "mafEvent.h"
#include "mmgGui.h"
#include "mafOp.h"

#include "mmgDialog.h"


#include "mafVME.h"
//#include "mafVMEItemVTK.h"
#include "mafVMESurface.h"
#include "vtkMAFSmartPointer.h"
#include "mafSmartPointer.h"
#include "mafVMELandmark.h"

#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkWeightedLandmarkTransform.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"



//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoRegisterClusters);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
enum ID_REGISTER_CLUSTERS
{
	RIGID =0,
	SIMILARITY,
	AFFINE
};
//----------------------------------------------------------------------------
mmoRegisterClusters::mmoRegisterClusters(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  
	m_Source					= NULL;
  m_Target					= NULL;
  m_Registered			= NULL;
	m_Follower				= NULL;
  m_PointsSource		= NULL;
  m_PointsTarget		= NULL;
  
	m_SourceName			="none";
  m_TargetName			="none";
	m_FollowerName		="none";
	m_Apply						= 0;
	m_MultiTime				= 0;
	m_RegistrationMode = RIGID;

	m_cluster_accept         = new mafClusterAccept;
	m_cluster_surface_accept = new mafClusterSurfaceAccept;

	m_CommonPoints = NULL;
	m_Weight		   = NULL;

  m_SettingsGuiFlag = false;

  m_GuiSetWeights = NULL;
  m_Dialog = NULL;
}
//----------------------------------------------------------------------------
mmoRegisterClusters::~mmoRegisterClusters( ) 
//----------------------------------------------------------------------------
{
	vtkDEL(m_Follower);
//	vtkDEL(m_Registered);
	vtkDEL(m_PointsSource);
	vtkDEL(m_PointsTarget);

  delete m_cluster_accept;
  delete m_cluster_surface_accept;

  if(m_Weight)
	{
		delete[] m_Weight;
		m_Weight = NULL;
	}
}
//----------------------------------------------------------------------------
mafOp* mmoRegisterClusters::Copy()   
//----------------------------------------------------------------------------
{
	return new mmoRegisterClusters(m_Label);
}
//----------------------------------------------------------------------------
bool mmoRegisterClusters::Accept(mafNode* node)
//----------------------------------------------------------------------------
{
	if(!node) return false;
   //modified by Marco. 2-6-2004 //modified by Marco. 2-6-2004  performance optimization
	if( node->IsA("mafVMELandmarkCloud") && !((mafVMELandmarkCloud*)node)->IsOpen() )
  {
    if(!mafVMELandmarkCloud::SafeDownCast(node)->IsAnimated())
      return true;
  }
  return false;
};
//----------------------------------------------------------------------------
// widget id's
//----------------------------------------------------------------------------
enum 
{
	ID_CHOOSE = MINID,
	ID_CHOOSE_SURFACE,
	ID_MULTIPLE_TIME_REGISTRATION,
	ID_APPLY_REGISTRATION,
	ID_REGTYPE,
	ID_WEIGHT,
	
};
//----------------------------------------------------------------------------
void mmoRegisterClusters::OpRun()   
//----------------------------------------------------------------------------
{
  m_Source = (mafVMELandmarkCloud*)m_Input;
  m_SourceName = m_Input->GetName();
	
	int num_choices = 3;
	const wxString choices_string[] = {_("rigid"), _("similarity"), _("affine")}; 
	mafString wildcard = "Dictionary (*.txt)|*.txt|All Files (*.*)|*.*";

  m_Gui = new mmgGui(this);
  m_Gui->SetListener(this);
	
	m_Gui->Label("source :",true);
	m_Gui->Label(&m_SourceName);
  
	m_Gui->Label("target :",true);
	m_Gui->Label(&m_TargetName);
	
	m_Gui->Label("follower surface:",true);
	m_Gui->Label(&m_FollowerName);
	
	m_Gui->Button(ID_CHOOSE,"target ");
	m_Gui->Button(ID_CHOOSE_SURFACE,"follower surface");
	
	m_Gui->Button(ID_WEIGHT,"weighted registration");
  m_Gui->Enable(ID_WEIGHT,false);
	
	m_Gui->Combo(ID_REGTYPE, "reg. type", &m_RegistrationMode, num_choices, choices_string); 
	
	m_Gui->Bool(ID_MULTIPLE_TIME_REGISTRATION,"multi-time",&m_MultiTime,1);
	m_Gui->Enable(ID_MULTIPLE_TIME_REGISTRATION,false);
	
	m_Gui->Label("Apply registration matrix to landmarks");
	m_Gui->Bool(ID_APPLY_REGISTRATION,"Apply",&m_Apply,1);
	m_Gui->Enable(ID_APPLY_REGISTRATION,false);
	
  m_Gui->OkCancel();

  m_Gui->Enable(wxOK,false);
  ShowGui();

  assert(!m_PointsSource && !m_PointsTarget);
  m_PointsSource = vtkPoints::New();
	m_PointsTarget = vtkPoints::New();

}
//----------------------------------------------------------------------------
void mmoRegisterClusters::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if(mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
		  case ID_CHOOSE:
		  {
			  mafString s("Choose cloud");
			  mafEvent e(this,VME_CHOOSE, &s, (long)m_cluster_accept);
			  mafEventMacro(e);
			  mafNode *vme = e.GetVme();
		    OnChooseVme(vme);
        if(vme != NULL)
          m_Gui->Enable(ID_WEIGHT,true);
		  }
		  break;
		  case ID_CHOOSE_SURFACE:
		  {
			  mafString s("Choose surface");
			  mafEvent e(this,VME_CHOOSE, &s, (long)m_cluster_surface_accept);
			  mafEventMacro(e);
			  mafNode *vme = e.GetVme();
		    OnChooseVme(vme);
		  }
		  break;
		  case ID_REGTYPE:
			  if(m_RegistrationMode == AFFINE)
				  m_Gui->Enable(ID_APPLY_REGISTRATION, true);
			  else
				  m_Gui->Enable(ID_APPLY_REGISTRATION, false);
		  break;
		  case ID_WEIGHT:	
			  {
          m_SettingsGuiFlag = true;
				  int x_init,y_init;
				  x_init = mafGetFrame()->GetPosition().x;
				  y_init = mafGetFrame()->GetPosition().y;

          m_Dialog = new mmgDialog("setting weights", mafCLOSEWINDOW);
				  m_Dialog->SetSize(x_init+40,y_init+40,220,220);
  				
				  m_GuiSetWeights = new mmgGui(this);
				  m_GuiSetWeights->SetListener(this);
  			
				  /////////////////////////////////////////////////////
				  if(m_CommonPoints)
				  {
					  mafVMELandmark *lmk;
					  m_CommonPoints->Open();
					  const char *name_lmk;
					  int number = m_CommonPoints->GetNumberOfLandmarks();

					  if(!m_Weight)
						  {
							  m_Weight = new double[number];
							  for (int i=0; i <number; i++)
								  m_Weight[i] = 1.0;
  							
						  }
  					
					  for (int i=0; i <number; i++)
					  {
						  lmk = m_CommonPoints->GetLandmark(i);
						  name_lmk = lmk->GetName();
						  m_GuiSetWeights->Label(name_lmk);
						  m_GuiSetWeights->Double(-1,"",&m_Weight[i]);
					  }
					  m_CommonPoints->Close();
				  }

				  
          
          m_GuiSetWeights->Show(true);
				  m_GuiSetWeights->Reparent(m_Dialog);
				  m_GuiSetWeights->FitGui();
				  m_GuiSetWeights->SetSize(200, 220);
   			  m_GuiSetWeights->OkCancel();
          m_GuiSetWeights->Update();
 
				  m_Dialog->Add(m_GuiSetWeights,1,wxEXPAND);   
				  m_Dialog->SetAutoLayout(TRUE);		

          m_Dialog->ShowModal();
			  }
		  break;
		  case wxOK:
        if(m_SettingsGuiFlag == false)
			    OpStop(OP_RUN_OK);
        else
        {
          m_GuiSetWeights->Close();
          m_Dialog->Close();
          m_SettingsGuiFlag = false;
        }
		  break;
		  case wxCANCEL:
        if(m_SettingsGuiFlag == false)
			    OpStop(OP_RUN_CANCEL);
        else
        {
          m_GuiSetWeights->Close();
          m_Dialog->Close();
          delete[] m_Weight;	
					  m_Weight = NULL;
          m_SettingsGuiFlag = false;
        }
		  break;
		  default:
			  mafEventMacro(*e);
		  break;
	  }
  }
}
//----------------------------------------------------------------------------
void mmoRegisterClusters::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void mmoRegisterClusters::OpDo()
//----------------------------------------------------------------------------
{
	wxBusyInfo wait("Please wait, working..."); 

  //check for the multi-time registration
	if(m_MultiTime)
	{
    std::vector<mafTimeStamp> timeStamps;
    m_Target->GetLocalTimeStamps(timeStamps);
		int numTimeStamps = m_Target->GetNumberOfTimeStamps();

		//mafProgressBarShowMacro();
    mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
    
		//mafProgressBarSetTextMacro("Multi time registration...");
		
		for (int t = 0; t < numTimeStamps; t++)
		{
			double currTime = timeStamps[t];
			long p = t * 100 / numTimeStamps;
		//	mafProgressBarSetValueMacro(p);
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,p));
			//Set the new time for the vme used to register the one frame source 
      m_Target->SetTimeStamp(currTime); //setcurrenttime
      m_Target->Update(); //>UpdateAllData();

			if(ExtractMatchingPoints(currTime))
      {
				RegisterPoints(currTime);
      }
      
	
		}
    timeStamps.clear();

    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));

	}
	else
	{
		//RegisterPoints(m_Source->GetCurrentTime());
		RegisterPoints();
	}
	
	if(m_Registered)
	{
		if(m_Apply)
		{
			//Apply all matrix vector to the polydata so the gliphs are not deformed 
			//when affine registration is choosed
			double cTime;
      std::vector<mafTimeStamp> time;
			m_Registered->GetLocalTimeStamps(time); // time is to be deleted
			int num = m_Registered->GetNumberOfLocalTimeStamps();
			
			vtkMAFSmartPointer<vtkPolyData> data;
			mafSmartPointer<mafMatrix> matrix; //modified by Marco. 2-2-2004
			vtkMAFSmartPointer<vtkTransform> transform;
			vtkMAFSmartPointer<vtkTransformPolyDataFilter> transformData;
			transformData->SetTransform(transform);
      
      if(!m_Registered->IsOpen())
					  m_Registered->Open();

			if(m_MultiTime)
			{
				for (int tm = 0; tm < num; tm++)
				{
					cTime = time[tm];
          m_Registered->SetTimeStamp(cTime); //Setcurrenttime
          // TODO: should not be necessary any more
          m_Registered->Update(); //>UpdateCurrentData();
					
          //data = (vtkPolyData *)m_Registered->GetOutput()->GetVTKData(); //GetCurrentData();
					
					/** Variante */
					vtkMAFSmartPointer<vtkPoints> points;
					

          for(int i=0; i< m_Registered->GetNumberOfLandmarks(); i++)
					{
					  double coords[3];
					  m_Registered->GetLandmark(i)->GetPoint(coords);
					  points->InsertNextPoint(coords);
          }
					data->SetPoints(points);
					data->Update();

					
					
          // TODO: refactoring to use directly the matrix pipe
          transform->SetMatrix(m_Registered->GetOutput()->GetMatrix()->GetVTKMatrix());  //modified by Marco. 2-2-2004
					transformData->SetInput(data);
					transformData->Update();
					
					matrix->Identity();
					m_Registered->SetPose(*matrix,cTime);
					

					/** Variante */
					for(int i=0; i< transformData->GetOutput()->GetNumberOfPoints(); i++)
				  {
					  double coords[3];
            transformData->GetOutput()->GetPoint(i, coords);
					  m_Registered->SetLandmark(i, coords[0], coords[1], coords[2] , cTime);
					}

          //m_Registered->SetDataByDetaching(vtkPolyData::SafeDownCast(transformData->GetOutput()) ,cTime );
				}
			}
			else
			{
				cTime = m_Registered->GetTimeStamp(); //GetCurrentTime();
        //data = (vtkPolyData *)m_Registered->GetOutput()->GetVTKData(); //GetCurrentData();

				/** Variante */
				vtkMAFSmartPointer<vtkPoints> points;
				

				for(int i=0; i< m_Registered->GetNumberOfLandmarks(); i++)
				{
				double coords[3];
				m_Registered->GetLandmark(i)->GetPoint(coords);
				points->InsertNextPoint(coords);
				}
				data->SetPoints(points);
				data->Update();


				//m_Registered->GetMatrix(matrix,cTime);
  
				// TODO: refactoring to use directly the matrix pipe
        transform->SetMatrix(m_Registered->GetOutput()->GetMatrix()->GetVTKMatrix());
				transformData->SetInput(data);
				transformData->Update();
				
				matrix->Identity();
				m_Registered->SetPose(*matrix,cTime);

				/** Variante */
				for(int i=0; i< transformData->GetOutput()->GetNumberOfPoints(); i++)
				{
				double coords[3];
				transformData->GetOutput()->GetPoint(i, coords);
				m_Registered->SetLandmark(i, coords[0], coords[1], coords[2] , cTime);
				}

        //m_Registered->SetDataByDetaching((vtkPolyData *)transformData->GetOutput(), cTime);
			}

      m_Registered->Close();
			time.clear();
		}
		mafEventMacro(mafEvent(this, VME_ADD, m_Registered));
	}
	
	if(m_Follower)
	{
		wxString name = wxString::Format("%s registered on %s",m_Follower->GetName(), m_Target->GetName());
		m_Follower->SetName(name);
		mafEventMacro(mafEvent(this, VME_ADD, m_Follower));
	}

  mafEventMacro(mafEvent(this,TIME_SET,-1.0));
}
//----------------------------------------------------------------------------
void mmoRegisterClusters::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_Registered);
  mafEventMacro(mafEvent(this, VME_REMOVE, m_Registered));
	vtkDEL(m_Registered);
	if(m_Follower)
	{
		mafEventMacro(mafEvent(this, VME_REMOVE, m_Follower));
		vtkDEL(m_Follower);
	}
}
//----------------------------------------------------------------------------
int mmoRegisterClusters::ExtractMatchingPoints(double time)
//----------------------------------------------------------------------------
{
	m_PointsSource->Reset();
	m_PointsTarget->Reset();

  m_Source->Update();
  vtkDataSet *polySource =m_Source->GetOutput()->GetVTKData();
  vtkDataSet *polyTarget =m_Target->GetOutput()->GetVTKData();

  polySource->Update();
  polyTarget->Update();


	int npSource = polySource->GetNumberOfPoints();
	int npTarget = polyTarget->GetNumberOfPoints();

  
	if(npSource == 0 ) return 0;
	if(npTarget == 0) return 0;

	int i;
	int j;

	//numero dei punti comuni tra m_Source e m_Target
	int ncp = 0;

	//modified by STEFY 24-5-2004(begin)
	m_CommonPoints = mafVMELandmarkCloud::New();
	//modified by STEFY 24-5-2004(end)

	bool found_one = false;

	for(i=0;i<npSource;i++)
	{
		wxString SourceLandmarkName = m_Source->GetLandmarkName(i);

		bool found = false;
		for(j=0;j<npTarget;j++)
		{
			wxString TargetLandmarkName = m_Target->GetLandmarkName(j);
			if(mafString(SourceLandmarkName) == mafString(TargetLandmarkName))
			{
				found = true;
				found_one = true;
				break;
			}
		}

		if (found)
		{
			if(m_Target->GetLandmarkVisibility(j,time))
			{
				
				m_PointsSource->InsertNextPoint(polySource->GetPoint(i));
				m_PointsTarget->InsertNextPoint(polyTarget->GetPoint(j));

				//modified by STEFY 24-5-2004(begin)
				m_CommonPoints->AppendLandmark(SourceLandmarkName);
				//modified by STEFY 24-5-2004(end)
				ncp++;
			}
		}
	}

	if(!found_one)
	{
		wxMessageBox("No matching landmarks found!","Alert", wxOK , NULL);
	}
	else if(found_one && (ncp == 0) && (m_MultiTime == 0))
	{
		wxMessageBox("No visible matching landmarks found at this timestamp!","Alert", wxOK , NULL);
	}

	return ncp;
}
//----------------------------------------------------------------------------
void mmoRegisterClusters::RegisterPoints(double currTime)
//----------------------------------------------------------------------------
{
	assert(m_PointsSource && m_PointsTarget);

	vtkWeightedLandmarkTransform *RegisterTransform = vtkWeightedLandmarkTransform::New();

	RegisterTransform->SetSourceLandmarks(m_PointsSource);	
	RegisterTransform->SetTargetLandmarks(m_PointsTarget);	
	
	if(m_Weight)
	{
		if(m_CommonPoints)
		{
			int number = m_CommonPoints->GetNumberOfLandmarks();	
			RegisterTransform->SetWeights(m_Weight,number);	
		}
	}

	
	switch (m_RegistrationMode)						
	{
		case RIGID:
  		RegisterTransform->SetModeToRigidBody();
		break;
		case SIMILARITY:
  		RegisterTransform->SetModeToSimilarity();
		break;
		case AFFINE:
  		RegisterTransform->SetModeToAffine();
		break;
	}
  RegisterTransform->Update();

  vtkMatrix4x4 *t_matrix = vtkMatrix4x4::New();
	t_matrix->Identity();

  if(m_Registered == NULL )
	{
		wxString name = wxString::Format("%s registered on %s",m_Source->GetName(), m_Target->GetName());
		mafNEW(m_Registered);
		m_Registered->DeepCopy(m_Source);
		m_Registered->SetName(name);
	}
	
	//postmultiply the registration matrix by the abs matrix of the target to position the
	//registered  at the correct position in the space
  mafMatrix *mat;
	mafNEW(mat);
	mat->Identity();
  m_Target->GetOutput()->GetAbsMatrix(*mat,currTime);  //modified by Marco. 2-2-2004
  vtkMatrix4x4::Multiply4x4(mat->GetVTKMatrix(),RegisterTransform->GetMatrix(),t_matrix);
  
	int numLandmarks = m_Target->GetNumberOfVisibleLandmarks(currTime);

	if((numLandmarks < 2) || ((numLandmarks < 4) && (m_RegistrationMode == AFFINE)))
	{
		RegisterTransform->Delete();
		t_matrix->Delete();
		return;
	}
	else
	{
    m_Registered->SetTimeStamp(currTime); //SetCurrentTime(currTime);
		m_Registered->SetPose(t_matrix,currTime);

		if(m_Follower)
		{
      m_Follower->SetTimeStamp(currTime); //SetCurrentTime(currTime);
			m_Follower->SetPose(t_matrix,currTime);
		}
	}

  vtkDEL(RegisterTransform);
	vtkDEL(t_matrix);
}
//----------------------------------------------------------------------------
void mmoRegisterClusters::OnChooseVme(mafNode *vme)
//----------------------------------------------------------------------------
{
	if(!vme) // user choose cancel - keep everithing as before
		return;

	if(vme->IsA("mafVMESurface"))
	{
		if(m_Follower == NULL)
			m_Follower = mafVMESurface::New();
		if(m_Follower->CanCopy(vme))
			m_Follower->DeepCopy(vme);
		else
		{
			wxMessageBox("Bad follower!", "Alert", wxOK, NULL);
			vtkDEL(m_Follower);
			return;
		}
		m_FollowerName = m_Follower->GetName();
	}
	else if(vme->IsA("mafVMELandmarkCloud"))
	{
    /*if(!Accept(vme))
		{
			wxMessageBox("Bad target!", "Alert", wxOK, NULL);
			vtkDEL(m_Target);
			return;
		}*/
		m_Target = (mafVMELandmarkCloud *)vme;
		m_TargetName = m_Target->GetName();
		if(m_Target->IsAnimated())
			m_Gui->Enable(ID_MULTIPLE_TIME_REGISTRATION,true);
		ExtractMatchingPoints(); 
		m_Gui->Enable(wxOK,true);
	}

	if(!vme->IsA("mafVMESurface")  &&  !vme->IsA("mafVMELandmarkCloud"))
	{
		wxMessageBox("Bad vme type!", "Alert", wxOK, NULL);
		return;
	}
	m_Gui->Update();
}
