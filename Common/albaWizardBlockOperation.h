/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardBlockOperation
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaWizardBlockOperation_H__
#define __albaWizardBlockOperation_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaWizardBlock.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  Class Name: albaWizardOperaiontionBlock.
  Class for operation block inside the wizard 
  This classes manage all the operation required to run an operation
  *Open the view Required by the op (if necesary)
  *Selects the input VME
  *Show other useful VME
  *Run the operation
  *Hide some VME after operation termination
*/
class ALBA_EXPORT albaWizardBlockOperation : public albaWizardBlock
{
public:
  
  /** Default constructor */
  albaWizardBlockOperation(const char *name);

  /** Default destructor */
  ~albaWizardBlockOperation();

  /** Set The name of the operation required view */
  void SetRequiredView(const char *View);

  /** Get The name of the operation required view */
  wxString GetRequiredView();

  /** Resize and reposition the View */
  void ResizeView(double x, double y, double width, double height);

  /** Tile windows horizontally, vertically or in cascade mode */
  void TileWindows( const char *tile_windows );

  /** Set The name of the required view to delete*/
  void SetViewToDelete(const char *View);

  /** Set the path for VME selection */
  void VmeSelect(const char *path);

  /** Get the path for VME selection */
  wxString GetVmeSelect(){return m_VmeSelect;};

  /** Set the path of the VMEs that is need to show for the operation.
      The path starts from the selected vme. 
      Multiple calls to this function correspond on multiple VME show.*/
  void VmeShowAdd(const char *path);

  /** Returns the list of vme to show */
  std::vector < wxString > *GetVmeShowList(){return &m_VmeShow;};

  /** Set the path of the VMEs that is need to hide after the operation.
      The path starts from the selected vme.
      Multiple calls to this function correspond on multiple VME show*/
  void VmeHideAdd(const char *path);

  /** Returns the list of vme to Hide */
  std::vector < wxString > *GetVmeHideList(){return &m_VmeHide;};

  /** Set the name of the operation required by this block 
      Set to an empty string to obtain only show and selections
      There are some wizard special operations:
      SAVE Save the msf
      SAVE_AS Save the msf with name
      OPEN an msf
      DELETE delete a VME (and his children) from tree
      NEW create a new, empty msf
      PAUSE open an operation with only one "next step" button
      RELOAD reload current msf, an msf must be loaded or saved before operation call*/
   void SetRequiredOperation(const char *name);

   /** Returns the name of the operation required by this block 
       Return an empty string if no operation is required */
   wxString GetRequiredOperation();


   /** If Set  Auto Show Selected VME to false the selected vme will not 
       auto showed before operation start*/
   void SetAutoShowSelectedVME(bool autoShow);

   /** Get Auto Show Selected VME status */
   bool GetAutoShowSelectedVME();
protected:

    
  /** Starts the execution of the block */
  void ExcutionBegin();

  /** Ends the execution of the block */
  void ExcutionEnd();
  
private:

  wxString m_Operation;
  wxString m_RequiredView;
  wxString m_VmeSelect;
  bool m_AutoShowSelectedVME;
  std::vector < wxString > m_VmeShow;
  std::vector < wxString > m_VmeHide;
  albaString m_viewtodelete;
  bool m_viewhastobedeleted;
  albaString m_tile_windows;
  bool m_windowhastoberesized;
  bool m_windowhastobetiled;
  double m_x;
  double m_y;
  double m_width;
  double m_height;
};
#endif
