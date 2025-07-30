/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpRegisterClusters
 Authors: Paolo Quadrani - porting Daniele Giunchi
 
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

#include "albaOpRegisterClusters.h"

#include "albaDecl.h"
#include <albaGUIBusyInfo.h>

#include "albaEvent.h"
#include "albaGUI.h"

#include "albaGUIDialog.h"

#include "albaVME.h"
#include "albaSmartPointer.h"
#include "albaVMELandmark.h"

#include "vtkALBASmartPointer.h"
#include "albaMatrixVector.h"
#include "albaAbsMatrixPipe.h"

#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkWeightedLandmarkTransform.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkDoubleArray.h"
#include "albaProgressBarHelper.h"

#if defined(_MSC_VER) && _MSC_VER >= 1600
#define strcmpi _strcmpi
#endif

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpRegisterClusters);

//----------------------------------------------------------------------------
albaOpRegisterClusters::albaOpRegisterClusters(wxString label) :
albaOp(label)
{
	m_OpType	= OPTYPE_OP;
	m_Canundo = true;

	m_Target					= NULL;
	m_Registered			= NULL;
	m_RegisteredFollower = NULL;
	m_Follower				= NULL;
	m_Result          = NULL;
	m_Info            = NULL;	

	m_SourceName			="none";
	m_TargetName			="none";
	m_FollowerName		="none";
	m_Apply						= true;
	m_MultiTime				= 0;
	m_RegistrationMode = RIGID;

	m_Weights = NULL;
	m_Matches	=	NULL;

	m_SettingsGuiFlag = false;

	m_GuiSetWeights = NULL;
	m_Dialog = NULL;

	//m_bCloseSource = m_bCloseTarget = NULL;
	m_FilteringMode = albaOpRegisterClusters::Invisible | albaOpRegisterClusters::InfiniteOrNaN;
	m_FilteringUserCoords[0] = m_FilteringUserCoords[1] = m_FilteringUserCoords[2] = 0.0;
}

//----------------------------------------------------------------------------
albaOpRegisterClusters::~albaOpRegisterClusters( )
{	
	albaDEL(m_Result);
	albaDEL(m_Info);
	albaDEL(m_Registered);
	albaDEL(m_RegisteredFollower);

	delete[] m_Weights;
	m_Weights = NULL;

	DestroyMatches();
}

//----------------------------------------------------------------------------
albaOp* albaOpRegisterClusters::Copy()
{
	return new albaOpRegisterClusters(m_Label);
}

//----------------------------------------------------------------------------
//Accept source landmark cloud (may not be time-variant).
bool albaOpRegisterClusters::InternalAccept(albaVME* node)
{
	if (!LMCloudAccept(node))
		return false;

	//source may not be time-variant!
	return !albaVMELandmarkCloud::SafeDownCast(node)->IsAnimated();
};

//----------------------------------------------------------------------------
//Callback for VME_CHOOSE that accepts Closed Landmarkclouds VMEs only.
/*static*/ bool albaOpRegisterClusters::LMCloudAccept(albaVME* node)
{
	return (albaVMELandmarkCloud::SafeDownCast(node));
}

//----------------------------------------------------------------------------
//Callback for VME_CHOOSE that accepts Surface VME only.
/*static*/ bool albaOpRegisterClusters::SurfaceAccept(albaVME* node)
{
	return (albaVMESurface::SafeDownCast(node) != NULL);
}

//----------------------------------------------------------------------------
//Set the input vme for the operation.
//This method invalidates Weights, so specifying weights must be done after calling this method.
/*virtual*/ void albaOpRegisterClusters::SetInput(albaVME* vme)
{
	if (vme == m_Input || (vme != NULL && !Accept(vme)))
		return;	//ignore this because it is either already specified input or it is not acceptable

	delete[] m_Weights;
	m_Weights = NULL;

	DestroyMatches();	//destroy current matches

	if (NULL != (m_Input = vme))
	{
		m_SourceName = m_Input->GetName();		
		CreateMatches();		
	}
	else
		m_SourceName = "none";
}

//----------------------------------------------------------------------------
//Sets the target landmark cloud.
/*virtual*/ void albaOpRegisterClusters::SetTarget(albaVMELandmarkCloud *target)
{
	if (target == m_Target || (target != NULL && !LMCloudAccept(target)))
		return;	//ignore this because it is either already specified input or it is not acceptable

	if (NULL != (m_Target = target))
	{
		m_TargetName = m_Target->GetName();		
		CreateMatches();	//recreate matches		
	}
	else
		m_TargetName = "none";
}

//----------------------------------------------------------------------------
//Sets the source landmark cloud follower surface.
//Follower is supposed to be a child of source landmark cloud, i.e., it is subject to transformations of source landmark cloud only.
//If any other than a child of source landmark cloud is specified, the result may be unpredictable (though it has no effect on
//the correct registration of source and target landmark clouds. Follower is deep copied into the result
//so that it is transformed by its parent target landmark cloud.
/*virtual*/ void albaOpRegisterClusters::SetFollower(albaVMESurface *follower)
{
	if (follower == m_Follower || (follower != NULL && !SurfaceAccept(follower)))
		return;	//ignore this because it is either already specified input or it is not acceptable

	if (NULL != (m_Follower = follower))
		m_FollowerName = m_Follower->GetName();
	else
		m_FollowerName = "none";
}

//----------------------------------------------------------------------------
//Sets the weight for the given source landmark (identified by index).
//Default weights are 1.0. Use 0.0, if you wish to ignore the landmark completely.
//N.B. this method must be called only after SetInput or SetSource otherwise
//weights may become lost.
/*virtual*/ void albaOpRegisterClusters::SetSourceWeight(int index, double weight)
{
	if (m_Weights == NULL) {
		InitializeWeights();
	}

	m_Weights[index] = weight;
}

//----------------------------------------------------------------------------
//Sets the weight for the given source landmark (identified by name).
//	Default weights are 1.0. Use 0.0, if you wish to ignore the landmark completely.
//	N.B. this method must be called only after SetInput or SetSource otherwise
//	weights may become lost.
/*virtual*/ void albaOpRegisterClusters::SetSourceWeight(const char* sourceName, double weight)
{	
	albaVMELandmarkCloud* source = GetSource();
	if (source == NULL)
		return;	//fatal error

	int index = source->GetLandmarkIndex(sourceName);
	if (index >= 0) {
		SetSourceWeight(index, weight);
	}
}

//----------------------------------------------------------------------------
//Initializes weights.
/*virtual*/ void albaOpRegisterClusters::InitializeWeights()
{
	delete[] m_Weights;
	m_Weights = NULL;

	albaVMELandmarkCloud* source = GetSource();
	if (source != NULL)
	{
		int number = source->GetNumberOfLandmarks();

		m_Weights = new double[number];
		for (int i=0; i <number; i++) {
			m_Weights[i] = 1.0;
		}
	}
}

//----------------------------------------------------------------------------
//Detects correspondence between source and target landmarks exploiting
//the predefined preferences given in m_Matches array.
//N.B. If m_Matches is unallocated, it is constructed first. 
//Call DestroyMatches to release the memory allocated by this method.
void albaOpRegisterClusters::CreateMatches()
{
	albaVMELandmarkCloud* source = GetSource();
	if (source == NULL)
		return;	//source is required	

	//if m_Matches does not exist, create it
	if (m_Matches == NULL)
	{
		int number = source->GetNumberOfLandmarks();
		m_Matches = new MatchingInfo[number];

		for (int i = 0; i < number; i++) {
			m_Matches[i].AddAcceptableTargetName(source->GetLandmarkName(i));
		}		
	}	

	albaVMELandmarkCloud* target = GetTarget();
	if (target != NULL)
	{
		//do matching of target landmarks
		int number = source->GetNumberOfLandmarks();		
		int tarnumber = target->GetNumberOfLandmarks();
		for (int i = 0; i < number; i++)
		{
			//check, if the source landmark has not matching target landmark
			if (m_Matches[i].GetTargetIndex() < 0)
			{
				for (int j = 0; j < tarnumber; j++) 
				{
					if (m_Matches[i].MatchTargetName(target->GetLandmarkName(j))){
						m_Matches[i].SetTargetIndex(j); break;	//we have found our match
					}
				}			
			}
		}		
	}	
}

//----------------------------------------------------------------------------
//Sets explicitly the correspondence between source and target landmarks. 
//If not specified, the correspondence is detected automatically using names of landmarks. 
//N.B. must be called after SetInput (or SetSource) method.
/*virtual*/ void albaOpRegisterClusters::SetMatchingLandmarks(int sourceIndex, int targetIndex)
{
	if (m_Matches == NULL) 
	{
		CreateMatches(); 
		
		if (m_Matches == NULL) {
			return;	//failure
		}
	}

	m_Matches[sourceIndex].SetTargetIndex(targetIndex);
}

//----------------------------------------------------------------------------
//Sets explicitly the correspondence between source and target landmarks identified by names. 
//N.B. must be called after SetInput (or SetSource) method.
/*virtual*/ void albaOpRegisterClusters::SetMatchingLandmarks(const char* sourceName, const char* targetName)
{
	albaVMELandmarkCloud* source = GetSource();
	if (source != NULL)
	{
		int sourceIndex = source->GetLandmarkIndex(sourceName);
		if (sourceIndex >= 0)
		{
			m_Matches[sourceIndex].RemoveAllAcceptableTargetNames();
			m_Matches[sourceIndex].AddAcceptableTargetName(targetName);
		}
	}
}

//----------------------------------------------------------------------------
//Adds an alternative correspondence between source and target landmarks.
//By default a source landmark matches the target landmark of the same name (unless this mechanism
//is completely overridden by calling SetMatchingLandmarks). If some source landmark does not have
//corresponding target landmark, alternative names (specified by this method) are checked to find the 
//correspondence. For example, source landmark RGT is referenced in some target landmark clouds 
//as RGT but in others (coming from different resources) as RGTR. This method enables easy specification
//of such an alias: just call AddAlternativeMatching("RGT", "RGTR");
//N.B. must be called after SetInput (or SetSource) method.	
/*virtual*/ void albaOpRegisterClusters::AddAlternativeMatching(const char* sourceName, const char* targetName)
{
	albaVMELandmarkCloud* source = GetSource();
	if (source != NULL)
	{
		int sourceIndex = source->GetLandmarkIndex(sourceName);
		if (sourceIndex >= 0) {			
			m_Matches[sourceIndex].AddAcceptableTargetName(targetName);
		}
	}
}

//----------------------------------------------------------------------------
//Extract matching points between source and target for the given time.
//Filtering is automatically applied and weights are stored. 
//Returns number of matched points.
//N.B. Call CreateMatches prior to this method, otherwise this method fails.
/*virtual*/ int albaOpRegisterClusters::ExtractMatchingPoints(vtkPoints* sourcePoints, 
	vtkPoints* targetPoints, vtkDoubleArray* weights, double time)
{
	sourcePoints->Reset();
	targetPoints->Reset();
	weights->Reset();

	albaVMELandmarkCloud* source = GetSource();
	albaVMELandmarkCloud* target = GetTarget();
	if (source == NULL || target == NULL)
		return 0;	//nothing to match

	int npSource = source->GetNumberOfLandmarks();
	int npTarget = target->GetNumberOfLandmarks();

	if (npSource == 0 || npTarget == 0) 
		return 0;	//nothing to match
	
	for(int i = 0; i < npSource; i++)
	{
		int j = m_Matches[i].GetTargetIndex();
		if (j < 0)
			continue;	//no matching landmark

		//various filtering
		if ((m_FilteringMode & albaOpRegisterClusters::Invisible) == albaOpRegisterClusters::Invisible) 
		{
			if(!target->GetLandmarkVisibility(j,time))
				continue;	//target landmark is invisible
		}

		double targetPos[3];		
		target->GetLandmark(j)->GetPoint(targetPos,time);

		if ((m_FilteringMode & albaOpRegisterClusters::InfiniteOrNaN) == albaOpRegisterClusters::InfiniteOrNaN) 
		{
			bool bOK = true;
			for (int k = 0; k < 3; k++) {
				if (!(targetPos[k] >= -DBL_MAX && targetPos[k] <= DBL_MAX)) {
					bOK = false; break;		//inf or nan
				}
			}			

			if (!bOK) {
				continue;	//target landmark has invalid coordinate
			}
		}

		if ((m_FilteringMode & albaOpRegisterClusters::WithUserValue) == albaOpRegisterClusters::WithUserValue) 
		{
			if (targetPos[0] == m_FilteringUserCoords[0] &&
				targetPos[1] == m_FilteringUserCoords[1] && targetPos[2] == m_FilteringUserCoords[2]) {
					continue;	//target landmark has invalid coordinate
			}
		}

		//OK, target is valid
		double sourcePos[3];
		source->GetLandmark(i)->GetPoint(sourcePos, time);

		sourcePoints->InsertNextPoint(sourcePos);
		targetPoints->InsertNextPoint(targetPos);
		if (m_Weights != NULL) {
			weights->InsertNextValue(m_Weights[i]);
		}
	}

	return sourcePoints->GetNumberOfPoints();
}

//----------------------------------------------------------------------------
//Register the source  on the target according to the registration method selected: rigid, similar or affine. 
//sourcePoints, targetPoints and weights can be extracted using ExtractMatchingPoints method.
//Returns the transformation matrix (must be deleted by the caller). 
/*virtual*/ vtkMatrix4x4* albaOpRegisterClusters::RegisterPoints(vtkPoints* sourcePoints, 
	vtkPoints* targetPoints, vtkDoubleArray* weights)
{
	vtkLandmarkTransform *registerTransform;

	if (m_RegistrationMode == AFFINE)
		registerTransform = vtkLandmarkTransform::New();
	else
		registerTransform = vtkWeightedLandmarkTransform::New();

	registerTransform->SetSourceLandmarks(sourcePoints);
	registerTransform->SetTargetLandmarks(targetPoints);

	if (m_RegistrationMode!=AFFINE && weights != NULL && weights->GetNumberOfTuples() != 0) {
		vtkWeightedLandmarkTransform::SafeDownCast(registerTransform)->SetWeights(weights->GetPointer(0), weights->GetNumberOfTuples());
	}

	switch (m_RegistrationMode)
	{
	case RIGID:
		registerTransform->SetModeToRigidBody();
		break;
	case SIMILARITY:
		registerTransform->SetModeToSimilarity();
		break;
	case AFFINE:
		registerTransform->SetModeToAffine();
		break;
	}
	registerTransform->Update();

	vtkMatrix4x4* ret = registerTransform->GetMatrix();
	ret->Register(NULL);

	vtkDEL(registerTransform);
	return ret;
}

//----------------------------------------------------------------------------
//Calculates deviation between sourcePoints transformed by t_matrix and targetPoints.
double albaOpRegisterClusters::CalculateDeviation(vtkPoints* sourcePoints, vtkPoints* targetPoints, vtkMatrix4x4* t_matrix)
{	
	//calculate deviation
	double deviation = 0.0;

	int nPoints = sourcePoints->GetNumberOfPoints();
	for(int i = 0; i < nPoints; i++)
	{
		double coord[4];
		double result[4];
		double target[3];
		double dx, dy, dz;
		sourcePoints->GetPoint(i, coord);
		coord[3] = 1.0;

		targetPoints->GetPoint(i, target);

		//transform point
		t_matrix->MultiplyPoint(coord, result);

		dx = target[0] - result[0];
		dy = target[1] - result[1];
		dz = target[2] - result[2];

		deviation += dx * dx + dy * dy + dz * dz;
	}

	if(nPoints != 0)
		deviation /= nPoints;

	return sqrt(deviation);
}

//----------------------------------------------------------------------------
//Register the source  on the target  at the given time (-1 == current time) according 
//to the registration method selected: rigid, similar or affine.
//Returns true, if the registration succeeded, false otherwise.
//N.B. the method assumes that m_Info, m_Registered and m_RegisteredFollower (if Follower is valid) exist.
//CreateMatches must be called prior to calling this method.
/**virtual*/ bool albaOpRegisterClusters::RegisterSource(double currTime)
{
	vtkALBASmartPointer< vtkPoints > sourcePoints;
	vtkALBASmartPointer< vtkPoints > targetPoints;
	vtkALBASmartPointer< vtkDoubleArray > weights;

	int ncp = ExtractMatchingPoints(sourcePoints, targetPoints, weights, currTime);
	if (ncp < 2 || ((ncp < 4) && (m_RegistrationMode == AFFINE)))	//check, if we have enough points to match
		return false;

	vtkMatrix4x4* matReg = RegisterPoints(sourcePoints, targetPoints, weights);
	double deviation = CalculateDeviation(sourcePoints, targetPoints, matReg);
	albaString label;
	label.Printf("Registration residual: %f", deviation);
	m_Info->SetLabel(label);
	
	//t_matrix now contains the correct transformation matrix for the current time
	m_Registered->SetTimeStamp(currTime); //SetCurrentTime(currTime);
		
	m_Registered->SetMatrix(matReg);
	m_Registered->Modified();
	m_Registered->Update();

	if(m_RegisteredFollower != NULL)
	{
		albaMatrix followerMtr, regFollowerMtr;
		m_Follower->GetOutput()->GetAbsMatrix(followerMtr,currTime);
		albaMatrix::Multiply4x4(matReg, followerMtr, regFollowerMtr);

		m_RegisteredFollower->SetTimeStamp(currTime); //SetCurrentTime(currTime);
		m_RegisteredFollower->SetAbsMatrix(regFollowerMtr,currTime);
		m_RegisteredFollower->Modified();
		m_RegisteredFollower->Update();
	}

	matReg->UnRegister(NULL);	//no longer needed
	return true;
}

//----------------------------------------------------------------------------
void albaOpRegisterClusters::OpRun()
{
	if (!m_TestMode)
	{
		int num_choices = 3;
		const wxString choices_string[] = { _("Rigid"), _("Similarity"), _("Affine") };
		albaString wildcard = "Dictionary (*.txt)|*.txt|All Files (*.*)|*.*";

		m_Gui = new albaGUI(this);
		m_Gui->SetListener(this);

		m_Gui->Label(_("Source:"), true);
		m_Gui->Label(&m_SourceName);

		m_Gui->Label(_("Target:"), true);
		m_Gui->Label(&m_TargetName);
		m_Gui->Button(ID_CHOOSE, _("Target "));

		m_Gui->Label(_("Follower surface:"), true);
		m_Gui->Label(&m_FollowerName);
		m_Gui->Button(ID_CHOOSE_SURFACE, _("Follower surface"));


		m_Gui->Divider(1);
		m_Gui->Combo(ID_REGTYPE, _("Reg. type"), &m_RegistrationMode, num_choices, choices_string);
		m_Gui->Button(ID_WEIGHT, _("Weighted registration"));
		m_Gui->Divider(1);
		m_Gui->Enable(ID_WEIGHT, false);


		m_Gui->Bool(ID_MULTIPLE_TIME_REGISTRATION, _("Multi-Time"), &m_MultiTime, 1);
		m_Gui->Enable(ID_MULTIPLE_TIME_REGISTRATION, false);
		m_Gui->Bool(ID_APPLY_REGISTRATION, _("Apply to landmarks"), &m_Apply, 1, _("Apply registration matrix to landmarks"));

		//////////////////////////////////////////////////////////////////////////
		m_Gui->Label("");
		m_Gui->Divider(1);
		m_Gui->OkCancel();
		m_Gui->Label("");

		m_Gui->Enable(wxOK, false);

		ShowGui();
	}
}

//----------------------------------------------------------------------------
//Called from OpDo to create all VMEs for the result. 
/*virtual*/ void albaOpRegisterClusters::CreateResultVMEs()
{
	albaVMELandmarkCloud* source = GetSource();
	albaVMELandmarkCloud* target = GetTarget();

	albaNEW(m_Result);	
	m_Result->SetName(albaString::Format("%s registered into %s", source->GetName(), m_Target->GetName()));
	GetLogicManager()->VmeAdd(m_Result);
		
	albaNEW(m_Info);	
	m_Info->SetName(albaString::Format("Info for registration %s into %s",source->GetName(), target->GetName()));
	GetLogicManager()->VmeAdd(m_Info);
	m_Info->ReparentTo(m_Result);

	wxString name = albaString::Format("%s registered on %s",source->GetName(), target->GetName());
	albaNEW(m_Registered);
	m_Registered->DeepCopy(source);
	m_Registered->CopyLandmarks(source);
	m_Registered->SetName(name);
	GetLogicManager()->VmeAdd(m_Registered);
	m_Registered->ReparentTo(m_Result);

	if (m_Follower != NULL)
	{
		albaNEW(m_RegisteredFollower);
		m_RegisteredFollower->DeepCopy(m_Follower);		
		m_RegisteredFollower->SetName(name);
		wxString name = albaString::Format("%s registered on %s",m_Follower->GetName(), target->GetName());
		GetLogicManager()->VmeAdd(m_RegisteredFollower);
		m_RegisteredFollower->ReparentTo(m_Result);
	}
}

//----------------------------------------------------------------------------
//Called from OpUndo to destroy all VMEs of the result. 
//This method can be called also from OpDo when the registration process fails.
/*virtual*/ void albaOpRegisterClusters::DestroyResultVMEs()
{
	if (m_Result != NULL)
	{
		GetLogicManager()->VmeRemove(m_Info);
		albaDEL(m_Info);

		GetLogicManager()->VmeRemove(m_Registered);
		albaDEL(m_Registered);

		if (m_RegisteredFollower != NULL)
		{
			GetLogicManager()->VmeRemove(m_RegisteredFollower);
			albaDEL(m_RegisteredFollower);
		}

		GetLogicManager()->VmeRemove(m_Result);
		albaDEL(m_Result);
	}
}

//----------------------------------------------------------------------------
void albaOpRegisterClusters::OpDo()
{	
	albaGUIBusyInfo wait(_("Please wait, working..."),m_TestMode);
		
	//create result VMEs
	CreateResultVMEs();

	//do the operation	
	albaVMELandmarkCloud* source = GetSource();
	albaVMELandmarkCloud* target = GetTarget();

	//Make sure we have matches
	CreateMatches();

	bool bRegistrationOK = false;
	if (m_MultiTime == 0)
	{
		if (false == (bRegistrationOK = RegisterSource(-1))) {
			albaLogMessage("No matching landmarks found!");
		}
	}
	else
	{
		std::vector<albaTimeStamp> timeStamps;
		target->GetLocalTimeStamps(timeStamps);
		int numTimeStamps = target->GetNumberOfTimeStamps();

		albaProgressBarHelper progressHelper(m_Listener);
		progressHelper.SetTextMode(m_TestMode);
		progressHelper.InitProgressBar();
		
		for (int t = 0; t < numTimeStamps; t++)
		{
			double currTime = timeStamps[t];						
			progressHelper.UpdateProgressBar(t * 100 / numTimeStamps);
			
			if (RegisterSource(currTime))
				bRegistrationOK = true;	//at least something has been registered
			else
				albaLogMessage("No visible matching landmarks found at timestamp %f", currTime);			
		}

		timeStamps.clear();

	} //end else
		
	if (!bRegistrationOK)
	{
		//OK, so registration failed completely
		DestroyResultVMEs();		
	}
	else
	{	
		if (m_Apply != 0) 
			ApplyRegistrationMatrix();
				
		//conversion from time variant landmark cloud with non time variant landmark to
		// non variant landmark cloud with time variant landmark
		if(m_MultiTime)
			SetRegistrationMatrixForLandmarks();

	}

}

//----------------------------------------------------------------------------
void albaOpRegisterClusters::OpUndo()
{
	DestroyResultVMEs();
}

//----------------------------------------------------------------------------
// Converts time-variant m_Registered landmark cloud containing static landmarks into
//static landmark cloud (m_Registered) with time-variant landmarks.
/*virtual*/ void albaOpRegisterClusters::SetRegistrationMatrixForLandmarks()
{

	std::vector<albaTimeStamp> timeStamps;
	m_Target->GetLocalTimeStamps(timeStamps);
	int numTimeStamps = m_Target->GetNumberOfTimeStamps();

	albaVMELandmarkCloud *landmarkCloudWithTimeVariantLandmarks;
	albaNEW(landmarkCloudWithTimeVariantLandmarks);
	GetLogicManager()->VmeAdd(landmarkCloudWithTimeVariantLandmarks);
	landmarkCloudWithTimeVariantLandmarks->ReparentTo(m_Result);

	landmarkCloudWithTimeVariantLandmarks->SetName(m_Registered->GetName());

	for (int t = 0; t < numTimeStamps; t++)
	{
		double cTime = timeStamps[t];
		m_Registered->SetTimeStamp(cTime); //Set current time
		m_Registered->Update(); //>UpdateCurrentData();

		for(int i=0; i< m_Registered->GetNumberOfLandmarks(); i++)
		{
			albaVMELandmark *landmark;
			landmark = albaVMELandmark::SafeDownCast(landmarkCloudWithTimeVariantLandmarks->GetLandmark(i));
			if(landmark == NULL)
			{
				albaNEW(landmark);
				GetLogicManager()->VmeAdd(landmark);
				landmark->SetName(m_Registered->GetLandmark(i)->GetName());
				landmark->ReparentTo(landmarkCloudWithTimeVariantLandmarks);
				//BES: 30.3.2011 - now landmark can be released, since it is already ReparentTo => memory leak fix
				landmark->UnRegister(this);
			}

			double pos[3], rot[3];
			m_Registered->GetLandmark(i)->GetOutput()->GetAbsPose(pos,rot,cTime);

			landmarkCloudWithTimeVariantLandmarks->SetLandmarkVisibility(i,m_Registered->GetLandmarkVisibility(i,cTime),cTime);
			landmark->SetTimeStamp(cTime);
			landmark->SetAbsPose(pos,rot,cTime);

			//avoid matrix error log for the first creation of landmarks
			albaMatrix *matrix = landmark->GetMatrixVector()->GetMatrix(cTime);
			matrix->SetElement(0,0,1);
			matrix->SetElement(1,1,1);
			matrix->SetElement(2,2,1);

			landmark->Modified();
			landmark->Update();
		}
	}

	landmarkCloudWithTimeVariantLandmarks->Update();

	GetLogicManager()->VmeRemove(m_Registered);
	albaDEL(m_Registered);

	m_Registered = landmarkCloudWithTimeVariantLandmarks;	
	timeStamps.clear();
}		

//----------------------------------------------------------------------------
// Applies registration matrix. Called from OpDo, if m_Apply is non-zero.
/*virtual*/ void albaOpRegisterClusters::ApplyRegistrationMatrix()
{
	//Apply all matrix vector to the polydata so the gliphs are not deformed
	//when affine registration is choosed
	double cTime;
	std::vector<albaTimeStamp> time;
	m_Registered->GetLocalTimeStamps(time); // time is to be deleted
	int num = m_Registered->GetNumberOfLocalTimeStamps();

	vtkALBASmartPointer<vtkPolyData> data;
	albaSmartPointer<albaMatrix> matrix; //modified by Marco. 2-2-2004
	
	if(m_MultiTime)
	{
		for (int tm = 0; tm < num; tm++)
		{
			cTime = time[tm];
			m_Registered->SetTimeStamp(cTime); //Set current time
			m_Registered->Update(); 

			vtkALBASmartPointer<vtkPoints> points;

			for(int i=0; i< m_Registered->GetNumberOfLandmarks(); i++)
			{
				double coords[3];
				m_Registered->GetLandmark(i)->GetPoint(coords);
				points->InsertNextPoint(coords);
			}
			data->SetPoints(points);
						
			matrix->Identity();
			m_Registered->SetPose(*matrix,cTime);

			for(int i=0; i< data->GetNumberOfPoints(); i++)
			{
				double coords[3];
				data->GetPoint(i, coords);
				m_Registered->SetLandmark(i, coords[0], coords[1], coords[2] , cTime);
			}
		}
	}
	else
	{
		cTime = m_Registered->GetTimeStamp(); //GetCurrentTime();
	
		vtkALBASmartPointer<vtkPoints> points;
		for(int i=0; i< m_Registered->GetNumberOfLandmarks(); i++)
		{
			double coords[3];
			m_Registered->GetLandmark(i)->GetPoint(coords);
			points->InsertNextPoint(coords);
		}
		data->SetPoints(points);
		
		matrix->Identity();
		m_Registered->SetMatrix(*matrix);

		for(int i=0; i< data->GetNumberOfPoints(); i++)
		{
			double coords[3];
			data->GetPoint(i, coords);
			m_Registered->SetLandmark(i, coords[0], coords[1], coords[2] , cTime);
		}
	}

	time.clear();
}


#pragma region GUI
//----------------------------------------------------------------------------
void albaOpRegisterClusters::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
			case ID_CHOOSE:
				OnChooseLandmarkCloud();
				break;

			case ID_CHOOSE_SURFACE:
				OnChooseSurface();
				break;

			case ID_REGTYPE:
				m_Gui->Enable(ID_WEIGHT, m_Target != NULL && m_RegistrationMode != AFFINE);
				break;

			case ID_WEIGHT:
				OnChangeWeights();
				break;

			case wxOK:
				if (m_SettingsGuiFlag == false)
					OpStop(OP_RUN_OK);
				else
				{
					m_GuiSetWeights->Close();
					m_Dialog->Close();
					m_SettingsGuiFlag = false;
				}
				break;

			case wxCANCEL:
				if (m_SettingsGuiFlag == false)
					OpStop(OP_RUN_CANCEL);
				else
				{
					m_GuiSetWeights->Close();
					m_Dialog->Close();

					delete[] m_Weights;
					m_Weights = NULL;

					m_SettingsGuiFlag = false;
				}
				break;

			default:
				albaEventMacro(*e);
				break;
		}
	}
}

//----------------------------------------------------------------------------
//Called when ID_CHOOSE event is raised.
void albaOpRegisterClusters::OnChooseLandmarkCloud()
{
	albaString s(_("Choose cloud"));
	albaEvent e(this,VME_CHOOSE, &s);
	e.SetPointer(&albaOpRegisterClusters::LMCloudAccept);
	albaEventMacro(e);

	albaVME *vme = e.GetVme();
	OnChooseVme(vme);

	m_Gui->Enable(ID_WEIGHT, m_Target != NULL && m_RegistrationMode != AFFINE);
}

//----------------------------------------------------------------------------
//Called when ID_CHOOSE_SURFACE event is raised.
void albaOpRegisterClusters::OnChooseSurface()
{
	albaString s(_("Choose surface"));
	albaEvent e(this,VME_CHOOSE, &s);
	e.SetPointer(&albaOpRegisterClusters::SurfaceAccept);
	albaEventMacro(e);

	albaVME *vme = e.GetVme();
	OnChooseVme(vme);
}

//----------------------------------------------------------------------------
void albaOpRegisterClusters::OnChooseVme(albaVME *vme)
{
	if(vme == NULL) // user choose cancel - keep everything as before
		return;

	albaVMESurface* surface = albaVMESurface::SafeDownCast(vme);
	if (surface != NULL)
	{
		SetFollower(surface);
		if(m_Follower == NULL) {
			wxMessageBox(_("Bad follower!"), _("Alert"), wxOK, NULL);
		}
	}
	else
	{
		albaVMELandmarkCloud* target = albaVMELandmarkCloud::SafeDownCast(vme);
		if (target != NULL)
		{
			SetTarget(target);
			if (m_Target == NULL) {
				wxMessageBox(_("Bad target!"), _("Alert"), wxOK, NULL);
			}
			else
			{
				if(m_Target->IsAnimated())
					m_Gui->Enable(ID_MULTIPLE_TIME_REGISTRATION,true);

				m_Gui->Enable(wxOK,true);
			}
		}
		else
		{
			wxMessageBox(_("Bad vme type!"), _("Alert"), wxOK, NULL);
		}
	}

	m_Gui->Update();
}

//----------------------------------------------------------------------------
//Called when ID_WEIGHT event is raised.
/*virtual*/ void albaOpRegisterClusters::OnChangeWeights()
{
	m_SettingsGuiFlag = true;
	int x_init,y_init;
	x_init = albaGetFrame()->GetPosition().x;
	y_init = albaGetFrame()->GetPosition().y;

	m_Dialog = new albaGUIDialog(_("setting weights"), albaCLOSEWINDOW);
	m_Dialog->SetSize(x_init+40,y_init+40,220,220);

	m_GuiSetWeights = new albaGUI(this);
	m_GuiSetWeights->SetListener(this);

	/////////////////////////////////////////////////////	
	albaVMELandmarkCloud* source = GetSource();
	if (source != NULL)
	{	
		CreateMatches();	//make sure, we have matches
		InitializeWeights();
		
		int number = source->GetNumberOfLandmarks();
		for (int i = 0; i < number; i++)
		{
			//if the source landmark has matching target landmark, display it,
			//otherwise we will ignore it
			if (m_Matches[i].GetTargetIndex() >= 0)
			{			
				m_GuiSetWeights->Label(source->GetLandmarkName(i));
				m_GuiSetWeights->Double(-1,"",&m_Weights[i]);
			}
		}		
	}

	m_GuiSetWeights->Show(true);
	m_GuiSetWeights->Reparent(m_Dialog);
	m_GuiSetWeights->FitGui();
	m_GuiSetWeights->SetSize(200, 220);
	m_GuiSetWeights->OkCancel();
	m_GuiSetWeights->Divider();
	m_GuiSetWeights->Update();

	m_Dialog->Add(m_GuiSetWeights,1,wxEXPAND);
	m_Dialog->SetAutoLayout(true);

	m_Dialog->ShowModal();
}

#pragma endregion

#pragma region Nested Classes
//----------------------------------------------------------------------------
//Returns true, if the given targetName is in the list of acceptable list.
//Typically, the caller then sets TargetIndex to speed-up process. 
bool albaOpRegisterClusters::MatchingInfo::MatchTargetName(const char* targetName)
{
	int count = (int)m_TargetNames.size();
	for (int i = 0; i < count; i++) 
	{
		if (0 == strcmpi(m_TargetNames[i].c_str(), targetName))
			return true;	//match, we have found it!
	}

	return false;
}
#pragma endregion