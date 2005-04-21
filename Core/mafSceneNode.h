/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafSceneNode.h,v $
  Language:  C++
  Date:      $Date: 2005-04-21 13:18:01 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef _mafSceneNode_H_
#define _mafSceneNode_H_
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
class mafSceneNode
{
public:
								mafSceneNode(mafSceneGraph *sg, mafSceneNode *parent, mafNode* vme, vtkRenderer *ren1, vtkRenderer *ren2=NULL);
  virtual      ~mafSceneNode    ();

  /** Call Select method for the vme's pipe. */
	void Select(bool select);

  /** Call Show method for the vme's pipe. */
	void Show(bool show);

  /** Call UpdateProperty method for the vme's pipe. */
	void UpdateProperty(bool fromTag = false);

  /** Return the vme's visibility. */
  bool IsVisible() {return m_visible;};

  mafNode            *m_vme;
  mafSceneNode      *m_parent;
  mafPipe           *m_pipe;
  bool               m_pipe_creatable;
  bool               m_mutex;
  vtkRenderer       *m_ren1;
  vtkMAFAssembly    *m_asm1;
  vtkRenderer       *m_ren2;
  vtkMAFAssembly    *m_asm2;

  mafSceneNode      *m_next;
	mafSceneGraph     *m_sg;

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

	/** 
	Static function called by the scalar observer when the head arrow change. */
	//@@@ static void OnHeadModifiedEvent(void *arg);

protected:
  
  bool m_visible;   
};  
#endif // _mafSceneNode_H_
