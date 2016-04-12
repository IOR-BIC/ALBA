/*=========================================================================

 Program: MAF2
 Module: mafSceneNode
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafSceneNode_H__
#define __mafSceneNode_H__
//----------------------------------------------------------------------------
// forward declarations:
//----------------------------------------------------------------------------
class mafNode;
class vtkMAFAssembly;
class mafPipe;
class vtkRenderer;
class mafSceneGraph;
//@@@ class mafNodeLandmarkCloud;
//@@@ class mafNodeScalar;
//----------------------------------------------------------------------------
// mafSceneNode :
//----------------------------------------------------------------------------
/** mafSceneNode is a node of mafScenegraph. */
class MAF_EXPORT mafSceneNode
{
public:
								mafSceneNode(mafSceneGraph *sg, mafSceneNode *parent, const mafNode* vme, vtkRenderer *ren1, vtkRenderer *ren2=NULL, vtkRenderer *ren3 = NULL);
  virtual      ~mafSceneNode    ();

  /** Call Select method for the vme's pipe. */
	void Select(bool select);

  /** Call Show method for the vme's pipe. */
	//void Show(bool show);  --  //SIL. 21-4-2005: removed unused function Show from mafScenenNode and mafPipe

  /** Call UpdateProperty method for the vme's pipe. */
	void UpdateProperty(bool fromTag = false);

  /** Return the vme's visibility. */
  bool IsVisible() {return m_Pipe != NULL;};

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

  mafNode           *m_Vme;
  mafSceneNode      *m_Parent;
	/** Returns Pipe */
	mafPipe * GetPipe() const;

	/** Sets Pipe */
	void SetPipe(mafPipe * pipe);

	/** Deletes current Pipe*/
	void DeletePipe();

	bool               m_Mutex;
  vtkRenderer       *m_RenFront;
  vtkMAFAssembly    *m_AssemblyFront;
  vtkRenderer       *m_RenBack;
  vtkMAFAssembly    *m_AssemblyBack;
  vtkRenderer       *m_AlwaysVisibleRenderer;
  vtkMAFAssembly    *m_AlwaysVisibleAssembly;
  mafSceneNode      *m_Next;
  mafSceneGraph     *m_Sg;


  //observer to Listen to Clouds Open/CloseEvent
	//@@@ mafNodeLandmarkCloud	*m_cloud;
	
	//@@@ unsigned long m_CloudOpenClose_observer;
	//@@@ unsigned long	m_CloudRadiusModified_observer;
	//@@@ unsigned long	m_CloudSphereResolutionModified_observer;

  /** 
  Static function called by the landmark cloud open/close observer. */
	//@@@ static void	OnOpenCloseEvent(void *arg);

  /** 
  Static function called by the landmark observer when the landmark's radius change. */
	//@@@ static void	OnRadiusModifiedEvent(void *arg);

  /** 
  Static function called by the landmark observer when the landmark's sphere resolution change. */
	//@@@ static void	OnSphereResolutionModifiedEvent(void *arg);

	//modified by STEFY 16-6-2004(begin)
	//observer to Listen to Vector Arrow Modified Scaling Factor and Diameter Event
	//@@@ mafNodeScalar	*m_scalar;
	
	//@@@ unsigned long	m_Scalar_ScalingModified_observer;
	//@@@ unsigned long	m_Scalar_DiameterModified_observer;
	//@@@ unsigned long	m_Scalar_HeadModified_observer;
	 
	/** 
	Static function called by the scalar observer when the vector arrow scaling factor change. */
	//@@@ static void	OnScalingModifiedEvent(void *arg);

	/** 
	Static function called by the scalar observer when the vector arrow diameter scaling factor change. */
	//@@@ static void	OnDiameterModifiedEvent(void *arg);


	/** Returns PipeCreatable */
	bool GetPipeCreatable() const { return m_PipeCreatable; }

	/** Sets PipeCreatable */
	void SetPipeCreatable(bool pipeCreatable) { m_PipeCreatable = pipeCreatable; }

	/**
	Static function called by the scalar observer when the head arrow change. */
	//@@@ static void OnHeadModifiedEvent(void *arg);

protected:
  
	mafPipe           *m_Pipe;
	bool               m_PipeCreatable;
  //bool m_visible;   // a SceneNode is visible if a pipe has been created
};  


#endif // __mafSceneNode_H__
