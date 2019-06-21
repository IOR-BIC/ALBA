/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaSceneNode
 Authors: Silvano Imboden, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaSceneNode_H__
#define __albaSceneNode_H__
//----------------------------------------------------------------------------
// forward declarations:
//----------------------------------------------------------------------------
class albaVME;
class vtkALBAAssembly;
class albaPipe;
class vtkRenderer;
class albaSceneGraph;

//----------------------------------------------------------------------------
// albaSceneNode :
//----------------------------------------------------------------------------
/** albaSceneNode is a node of albaScenegraph. */
class ALBA_EXPORT albaSceneNode
{
public:
								albaSceneNode(albaSceneGraph *sg, albaSceneNode *parent, albaVME *vme, vtkRenderer *renderFront, vtkRenderer *renderBack=NULL, vtkRenderer *alwaysVisibleRender = NULL);
  virtual      ~albaSceneNode    ();

  /** Call Select method for the vme's pipe. */
	void Select(bool select);

  /** Call UpdateProperty method for the vme's pipe. */
	void UpdateProperty(bool fromTag = false);

  /** Return the vme's visibility. */
  bool IsVisible() {return m_Pipe != NULL;};

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

	/** Returns Vme */
	albaVME * GetVme() const { return m_Vme; }

	/** Returns Pipe */
	albaPipe * GetPipe() const;

	/** Sets Pipe */
	void SetPipe(albaPipe * pipe);

	/** Deletes current Pipe*/
	void DeletePipe();

	/** Returns RenFront */
	vtkRenderer * GetRenFront() const { return m_RenFront; }

	/** Returns Mutex */
	bool GetMutex() const { return m_Mutex; }

	/** Sets Mutex */
	void SetMutex(bool mutex) { m_Mutex = mutex; }

	/** Returns AssemblyFront */
	vtkALBAAssembly * GetAssemblyFront() const { return m_AssemblyFront; }
	
	/** Returns RenBack */
	vtkRenderer * GetRenBack() const { return m_RenBack; }
	
	/** Returns AssemblyBack */
	vtkALBAAssembly * GetAssemblyBack() const { return m_AssemblyBack; }
	
	/** Returns AlwaysVisibleRenderer */
	vtkRenderer * GetAlwaysVisibleRenderer() const { return m_AlwaysVisibleRenderer; }
		
	/** Returns AlwaysVisibleAssembly */
	vtkALBAAssembly * GetAlwaysVisibleAssembly() const { return m_AlwaysVisibleAssembly; }
		
	/** Returns Next */
	albaSceneNode * GetNext() const { return m_Next; }

	/** Sets Next */
	void SetNext(albaSceneNode * next) { m_Next = next; }

	/** Returns Sg */
	albaSceneGraph * GetSceneGraph() const { return m_Sg; }

	/** Returns PipeCreatable */
	bool GetPipeCreatable() const { return m_PipeCreatable; }

	/** Sets PipeCreatable */
	void SetPipeCreatable(bool pipeCreatable) { m_PipeCreatable = pipeCreatable; }

	/** Sets RenFront */
	void SetRenFront(vtkRenderer * renFront) { m_RenFront = renFront; }

protected:

	/** Called from child on show, if the visibility is updated ChildShow is called on parent */
	void ChildShow();
		
	/** Called from child on hide, if the visibility is updated ChildHide is called on parent */
	void ChildHide();
	
	/** callen onhow/hide childShow/childHide to update local visibility and forward up the changes*/
	void UpdateVisibility();

	/** Called on changes when a node is show/hided to notify assemby to recreate internal structures */
	void ModifyRootAssembly();
  
	albaPipe           *m_Pipe;
	bool               m_PipeCreatable;
	vtkRenderer       *m_RenFront;
	int								 m_VisibleChildren;
	bool							 m_CurrentVisibility;
	bool               m_Mutex;

	vtkALBAAssembly    *m_AssemblyFront;
	vtkRenderer       *m_RenBack;
	vtkALBAAssembly    *m_AssemblyBack;
	vtkRenderer       *m_AlwaysVisibleRenderer;
	vtkALBAAssembly    *m_AlwaysVisibleAssembly;
	albaSceneNode      *m_Next;

	albaSceneGraph     *m_Sg;
	albaVME            *m_Vme;
	albaSceneNode      *m_Parent;
};  


#endif // __albaSceneNode_H__
