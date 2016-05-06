/*=========================================================================

 Program: MAF2
 Module: mafSceneNode
 Authors: Silvano Imboden, Gianluigi Crimi
 
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
class mafVME;
class vtkMAFAssembly;
class mafPipe;
class vtkRenderer;
class mafSceneGraph;

//----------------------------------------------------------------------------
// mafSceneNode :
//----------------------------------------------------------------------------
/** mafSceneNode is a node of mafScenegraph. */
class MAF_EXPORT mafSceneNode
{
public:
								mafSceneNode(mafSceneGraph *sg, mafSceneNode *parent, mafVME *vme, vtkRenderer *renderFront, vtkRenderer *renderBack=NULL, vtkRenderer *alwaysVisibleRender = NULL);
  virtual      ~mafSceneNode    ();

  /** Call Select method for the vme's pipe. */
	void Select(bool select);

  /** Call UpdateProperty method for the vme's pipe. */
	void UpdateProperty(bool fromTag = false);

  /** Return the vme's visibility. */
  bool IsVisible() {return m_Pipe != NULL;};

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

	/** Returns Vme */
	mafVME * GetVme() const { return m_Vme; }

	/** Returns Pipe */
	mafPipe * GetPipe() const;

	/** Sets Pipe */
	void SetPipe(mafPipe * pipe);

	/** Deletes current Pipe*/
	void DeletePipe();

	/** Returns RenFront */
	vtkRenderer * GetRenFront() const { return m_RenFront; }

	/** Returns Mutex */
	bool GetMutex() const { return m_Mutex; }

	/** Sets Mutex */
	void SetMutex(bool mutex) { m_Mutex = mutex; }

	/** Returns AssemblyFront */
	vtkMAFAssembly * GetAssemblyFront() const { return m_AssemblyFront; }
	
	/** Returns RenBack */
	vtkRenderer * GetRenBack() const { return m_RenBack; }
	
	/** Returns AssemblyBack */
	vtkMAFAssembly * GetAssemblyBack() const { return m_AssemblyBack; }
	
	/** Returns AlwaysVisibleRenderer */
	vtkRenderer * GetAlwaysVisibleRenderer() const { return m_AlwaysVisibleRenderer; }
		
	/** Returns AlwaysVisibleAssembly */
	vtkMAFAssembly * GetAlwaysVisibleAssembly() const { return m_AlwaysVisibleAssembly; }
		
	/** Returns Next */
	mafSceneNode * GetNext() const { return m_Next; }

	/** Sets Next */
	void SetNext(mafSceneNode * next) { m_Next = next; }

	/** Returns Sg */
	mafSceneGraph * GetSceneGraph() const { return m_Sg; }

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
  
	mafPipe           *m_Pipe;
	bool               m_PipeCreatable;
	vtkRenderer       *m_RenFront;
	int								 m_VisibleChildren;
	bool							 m_CurrentVisibility;
	bool               m_Mutex;

	vtkMAFAssembly    *m_AssemblyFront;
	vtkRenderer       *m_RenBack;
	vtkMAFAssembly    *m_AssemblyBack;
	vtkRenderer       *m_AlwaysVisibleRenderer;
	vtkMAFAssembly    *m_AlwaysVisibleAssembly;
	mafSceneNode      *m_Next;

	mafSceneGraph     *m_Sg;
	mafVME            *m_Vme;
	mafSceneNode      *m_Parent;
};  


#endif // __mafSceneNode_H__
