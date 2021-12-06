/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpRegisterClusters
 Authors: Paolo Quadrani      - porting Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpRegisterClusters_H__
#define __albaOpRegisterClusters_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMEGroup.h"
#include "albaVMEInfoText.h"
#include "albaVMESurface.h"
#include <vector>
#include <string>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUI;
class albaEvent;
class vtkPoints;
class albaGUIDialog;
class vtkPoints;
class vtkDoubleArray;

//----------------------------------------------------------------------------
// albaOpRegisterClusters :
//----------------------------------------------------------------------------
/** This operation takes source landmark cloud, target landmark cloud as an input and finds the best 
affine transformation that would transform the source landmark cloud to the given target landmark cloud. 
The source is deep copied into new VMEs (in a new albaVMEGroup) and the resulting transformation is applied 
to this clone. The operation is capable of computing the transformation for multiple time frames in one shot.
Both landmark clouds must contain landmarks of the same name unless correspondence of these landmarks
is given explicitly. Landmark with an invalid value (because it was occluded when it was measured) at some time 
frame is automatically skipped from the processing of the frame where the invalid value is present. Landmarks 
may have also specified weights of their importance (unreliable landmarks should have small weights).
Several transformation modes are available: RIGID, SIMILARITY and AFFINE -- see below. 
Optionally, some albaVMESurface follower of the source landmark cloud can be also specified,
in which case the operation deep copies this follower as a child of the resulting (registered) landmark cloud.*/
class ALBA_EXPORT albaOpRegisterClusters: public albaOp
{
public:
	//----------------------------------------------------------------------------
	// Constants :
	//----------------------------------------------------------------------------
	enum ID_REGISTER_CLUSTERS
	{
		RIGID =	0,	///<only translation and rotation is allowed
		SIMILARITY,	///<only translation, rotation and scaling is allowed
		AFFINE,			///<any affine transformation is allowed (e.g., shear)
	};

	enum FILTERING_LANDMARKS
	{
		Invisible			=	1,	///<ignore landmarks invisible in some time stamp (this is default)
		InfiniteOrNaN = 2,	///<ignore landmarks with +/-INF or NaN coordinate(s) value(s)
		WithUserValue = 4,	///<ignore landmarks with special coordinates (typically, 0, 0, 0)
	};

protected:
	//GUI constants	
	enum ID_GUI_WIDGETS
	{
		ID_CHOOSE = MINID,
		ID_CHOOSE_SURFACE,
		ID_MULTIPLE_TIME_REGISTRATION,
		ID_APPLY_REGISTRATION,
		ID_REGTYPE,
		ID_WEIGHT,
		ID_LAST
	};

#pragma region Nested Classes
	class MatchingInfo
	{
	protected:
		int m_TargetIndex;	///<-1, if target is to be automatically located, otherwise index of matching target landmark

		std::vector< std::string > m_TargetNames;	///<list of corresponding names of target landmarks

	public:
		MatchingInfo() {
			m_TargetIndex = -1;
		}

		/** Gets the current matched index. 
		Returns -1, if there is no matching present.
		Caller may call MatchTargetName method to find target.*/
		inline int GetTargetIndex() {
			return m_TargetIndex;
		}

		/** Sets the current matched index. */
		inline void SetTargetIndex(int newTargetIndex) {
			m_TargetIndex = newTargetIndex;
		}

		/** Adds a new name of the target landmarks. */
		inline void AddAcceptableTargetName(const char* targetName) {
			std::string str = targetName;
			m_TargetNames.push_back(str);
		}

		/** Removes all acceptable target names.*/
		inline void RemoveAllAcceptableTargetNames() {
			m_TargetNames.clear();
		}

		/** Returns true, if the given targetName is in the list of acceptable list.
		Typically, the caller then sets TargetIndex to speed-up process. */
		bool MatchTargetName(const char* targetName);
	};
#pragma endregion
public:
  albaOpRegisterClusters(wxString label = _("Register Landmark Cloud"));
 ~albaOpRegisterClusters(); 

 albaTypeMacro(albaOpRegisterClusters, albaOp);
  
  /*virtual*/ albaOp* Copy();

	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();	

	/** Set the input vme for the operation. 
	This method invalidates Weights, so specifying weights must be done after calling this method. 
	/*virtual*/ void SetInput(albaVME* vme);

	/** Helper method. Sets the source landmark cloud. 
	N.B. This method invalidates Weights, so specifying weights must be done after calling this method. */
	inline void SetSource(albaVMELandmarkCloud *source){
		this->SetInput(source);
	}
	
	/** Helper method. Gets the source landmark cloud. */
	inline albaVMELandmarkCloud* GetSource() const {
		return albaVMELandmarkCloud::SafeDownCast(m_Input);
	}

	/** Sets the target landmark cloud. */
  virtual void SetTarget(albaVMELandmarkCloud *target);

	/** Gets the target landmark cloud. */
	inline virtual albaVMELandmarkCloud* GetTarget() const {
		return m_Target;
	}

	/** Sets the source landmark cloud follower surface.
	Follower is supposed to be a child of source landmark cloud, i.e., it is subject to transformations of source landmark cloud only.
	If any other than a child of source landmark cloud is specified, the result may be unpredictable (though it has no effect on
	the correct registration of source and target landmark clouds. Follower is deep copied into the result 
	so that it is transformed by its parent target landmark cloud.*/
  virtual void SetFollower(albaVMESurface *follower);

	/** Gets the target landmark cloud. */
	inline virtual albaVMESurface* GetFollower() const {
		return m_Follower;
	}

	/** Sets the weight for the given source landmark (identified by index).
	Default weights are 1.0. Use 0.0, if you wish to ignore the landmark completely.
	N.B. this method must be called only after SetInput or SetSource otherwise
	weights may become lost. */
	virtual void SetSourceWeight(int index, double weight);

	/** Sets the weight for the given source landmark (identified by name).
	Default weights are 1.0. Use 0.0, if you wish to ignore the landmark completely.
	N.B. this method must be called only after SetInput or SetSource otherwise
	weights may become lost. */
	virtual void SetSourceWeight(const char* sourceName, double weight);

	/** Gets the array of  weights for the source landmark cloud.
	Caller may change weights as it wishes.
	N.B. may return NULL, if weights are not specified.*/
	inline virtual double* GetSourceWeights() const {		
		return m_Weights;
	}

	/** Sets explicitly the correspondence between source and target landmarks. 
	If not specified, the correspondence is detected automatically using names of landmarks. 
	N.B. must be called after SetInput (or SetSource) method.*/
	virtual void SetMatchingLandmarks(int sourceIndex, int targetIndex);

	/** Sets explicitly the correspondence between source and target landmarks identified by names. 
	N.B. must be called after SetInput (or SetSource) method.*/
	virtual void SetMatchingLandmarks(const char* sourceName, const char* targetName);

	/** Adds an alternative correspondence between source and target landmarks.
	By default a source landmark matches the target landmark of the same name (unless this mechanism
	is completely overridden by calling SetMatchingLandmarks). If some source landmark does not have
	corresponding target landmark, alternative names (specified by this method) are checked to find the 
	correspondence. For example, source landmark RGT is referenced in some target landmark clouds 
	as RGT but in others (coming from different resources) as RGTR. This method enables easy specification
	of such an alias: just call AddAlternativeMatching("RGT", "RGTR"); 
	N.B. must be called after SetInput (or SetSource) method.*/	
	virtual void AddAlternativeMatching(const char* sourceName, const char* targetName);

	/** Sets new registration mode. 
	Valid options are RIGID, SIMILARITY, AFFINE - see ID_REGISTER_CLUSTERS enum*/
	inline void SetRegistrationMode(int mode) {
		m_RegistrationMode = mode;
	}

	/** Gets the current registration mode. */
	inline int GetRegistrationMode() {
		return m_RegistrationMode;
	}

	/** Sets if multiple time frames should be registered. 
	N.B. this option is ignored, if target landmark cloud is not animated (time-variant).*/
	inline void SetMultiTime(int enable) {
		m_MultiTime = enable;
	}

	/** Gets non-zero, of multiple time frames should be registered. */
	inline int GetMultiTime() {
		return m_MultiTime;
	}

	/** Sets filtering mode for landmarks.
	This is a bitwise combination of values from enum FILTERING_LANDMARKS.*/
	inline void SetFilteringMode(int filter) {
		m_FilteringMode = filter;
	}

	/** Gets the current filtering mode. 
	This is a bitwise combination of values from enum FILTERING_LANDMARKS.*/
	inline int GetFilteringMode() {
		return m_FilteringMode;
	}

	/** Sets the user specified coordinates of target landmark that should be ignored.
	N.B. This option is valid only, if Filtering Mode contains enum FILTERING_LANDMARKS::WithUserValue.*/
	inline void SetFilteringUserSpecCoords(const double* coords) {
		m_FilteringUserCoords[0] = coords[0]; m_FilteringUserCoords[1] = coords[1]; m_FilteringUserCoords[2] = coords[2];
	}

	/** Sets the user specified coordinates of target landmark that should be ignored. Default is (0,0,0)
	N.B. This option is valid only, if Filtering Mode contains enum FILTERING_LANDMARKS::WithUserValue.*/
	inline void SetFilteringUserSpecCoords(double x, double y, double z) {
		m_FilteringUserCoords[0] = x; m_FilteringUserCoords[1] = y; m_FilteringUserCoords[2] = z;
	}

	/** Gets the user specified coordinates of target landmark that should be ignored.
	N.B. This option is valid only, if Filtering Mode contains enum FILTERING_LANDMARKS::WithUserValue.*/
	inline const double* GetFilteringUserSpecCoords() {
		return m_FilteringUserCoords;
	}

	/** Gets  whether registration matrix should be applied to landmarks so the glyphs are not deformed when affine registration is chosen (0 by default) */
	inline int GetApplyRegistrationMatrix() {
		return m_Apply;
	}

	/** Sets  whether registration matrix should be applied to landmarks so the glyphs are not deformed when affine registration is chosen (0 by default)*/
	inline void SetApplyRegistrationMatrix(int apply) {
		m_Apply = apply;
	}

	/** Gets the result of the operation.
	N.B. OpDo must be called to create the result. */
  inline virtual albaVMEGroup* GetResult(){
		return m_Result;
	}
	
	 /** Process events coming from other objects */ 
	/*virtual*/ void OnEvent(albaEventBase *alba_event);    

public:
	/** Callback for VME_CHOOSE that accepts Closed Landmarkclouds VMEs only.*/
	static bool LMCloudAccept(albaVME* node); 

	/** Callback for VME_CHOOSE that accepts Surface VME only. */
	static bool SurfaceAccept(albaVME* node);

protected:

	/** Return true for the acceptable vme type.
	This method accepts source landmark cloud (may not be time-variant).*/
	bool InternalAccept(albaVME*node);

  /** Extract matching points between source and target for the given time.
	Filtering is automatically applied and weights are stored.
	Returns number of matched points.
	N.B. CreateMatches must be called prior to calling this method.*/
	virtual int ExtractMatchingPoints(vtkPoints* sourcePoints, 
		vtkPoints* targetPoints, vtkDoubleArray* weights, double time = -1);
  
	/** Register the source  on the target according to the registration method selected: rigid, similar or affine. 
	sourcePoints, targetPoints and weights can be extracted using ExtractMatchingPoints method.
	Returns the transformation matrix (must be deleted by the caller).  */
	virtual vtkMatrix4x4* RegisterPoints(vtkPoints* sourcePoints, vtkPoints* targetPoints, vtkDoubleArray* weights);

	//Register the source  on the target  at the given time (-1 == current time) according 
	//to the registration method selected: rigid, similar or affine.
	//Returns true, if the registration succeeded, false otherwise.
	//N.B. the method assumes that m_Info, m_Registered and m_RegisteredFollower (if Follower is valid) exist.
	//CreateMatches must be called prior to calling this method.
	virtual bool RegisterSource(double currTime);

	/** Initializes weights. */
	virtual void InitializeWeights();

	/** Called from OpDo to create all VMEs for the result. */
	virtual void CreateResultVMEs();

	/** Called from OpUndo to destroy all VMEs of the result. 
	This method can be called also from OpDo when the registration process fails.*/
	virtual void DestroyResultVMEs();
	
	/** Applies registration matrix. Called from OpDo, if m_Apply is non-zero.*/
	virtual void ApplyRegistrationMatrix();

	/** Converts time-variant m_Registered landmark cloud containing static landmarks into
	static landmark cloud (m_Registered) with time-variant landmarks.*/
	virtual void SetRegistrationMatrixForLandmarks();

protected:
	/** Detects correspondence between source and target landmarks exploiting
	the predefined preferences given in m_Matches array.
	N.B. If m_Matches is unallocated, it is constructed first. 
	Call DestroyMatches to release the memory allocated by this method.*/
	void CreateMatches();

	/** Releases memory consumed by m_Matches. */
	inline void DestroyMatches() {
		delete[] m_Matches;
		m_Matches = NULL;
	}

	/** Calculates deviation between sourcePoints transformed by t_matrix and targetPoints. */
	double CalculateDeviation(vtkPoints* sourcePoints, vtkPoints* targetPoints, vtkMatrix4x4* t_matrix);

#pragma region GUI

	/** Check the correctness of the vme's type. */
	void OnChooseVme(albaVME *vme);

	/** Called when ID_WEIGHT event is raised. */
	virtual void OnChangeWeights();

	/** Called when ID_CHOOSE event is raised. */
	virtual void OnChooseLandmarkCloud();

	/** Called when ID_CHOOSE_SURFACE event is raised. */
	virtual void OnChooseSurface();
#pragma endregion
  
protected:	
	double*									m_Weights;		///<weights of source landmarks (source landmark is in m_Input)
	MatchingInfo*						m_Matches;		///<correspondence between each source landmark and target landmarks
	
	albaVMELandmarkCloud*    m_Target;			///<target landmark cloud
	albaVMELandmarkCloud*    m_Registered;	///<the result of the registration of source to target
	
	albaVMESurface*					m_Follower;						///<surface follower of source
	albaVMESurface*					m_RegisteredFollower;	///<the result of the registration of source to target

  albaVMEGroup*						m_Result;			///<group where the results are stored
  albaVMEInfoText*					m_Info;				///<report of registration
	
	int m_RegistrationMode; ///<one of ID_REGISTER_CLUSTERS enum values
	int m_MultiTime;				///<non-zero, if multiple frames of target should be processed.
	int m_Apply;						///<non-zero, if matrix vectors should be applied so the glyphs are not deformed when affine registration is chosen (0 by default)

	int m_FilteringMode;							///<mode for filtering of landmarks, a bitwise combination of FILTERING_LANDMARKS enum
	double m_FilteringUserCoords[3];	///<coordinates considered to be invalid (used only if  m_FilteringMode contains WithUserValue)   

#pragma region GUI
	albaGUIDialog *m_Dialog;			///<main dialog
	albaString   m_SourceName;		///<name of source
	albaString   m_TargetName;		///<name of target
	albaString   m_FollowerName;	///<name of follower
	albaGUI *m_GuiSetWeights;

	bool m_SettingsGuiFlag;			///<true, if GUI is valid
		
#pragma endregion //GUI
 };
#endif