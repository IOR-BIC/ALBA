/*=========================================================================

 Program: MAF2
 Module: mafVMELabeledVolume
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
 
#ifndef __mafVMELabeledVolume_h
#define __mafVMELabeledVolume_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafVME.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUICheckListBox;
class mafGUIFloatSlider;
class mafGUIDialogPreview;
class mafRWI;
class mafInteractorExtractIsosurface;
class mafVMEVolumeGray;
class mafDataPipeCustom;

class vtkRenderer;
class vtkWindowLevelLookupTable;
class vtkStructuredPoints;
class vtkRectilinearGrid;
class vtkProbeFilter;
class vtkTexture;
class vtkPolyDataMapper;
class vtkActor;
class vtkPolyData;
class mmaVolumeMaterial;

//----------------------------------------------------------------------------
// mafVmeDataLabeledVolume :
//----------------------------------------------------------------------------
/** mafVmeDataLabeledVolume - Volume with interface to assign labels to certain portions of a volume, identified by
scalar thresholds  */

class MAF_EXPORT mafVMELabeledVolume : public mafVME
{
public:	

	mafTypeMacro(mafVMELabeledVolume,mafVME);

  /** Return the value of the label. */
  int GetLabelValue( wxString &item );  

  /**  Set a tag with the label values. */
  void SetLabelTag(mafString label, int component);

  /** Remove a tag. */
  void RemoveLabelTag(int component);

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  /*virtual*/ bool IsDataAvailable();

  /** Fill the vector of label. */
  void FillLabelVector(wxString name, bool checked = TRUE);

   /** Modify the vector of label. */
  void ModifyLabelVector(int n, wxString name, bool checked);

   /** Remove an item of the vector of label. */
  void RemoveItemLabelVector(int n);

  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  /** Copy the contents of another VMELabeled into this one. */
  int DeepCopy(mafNode *a);

  /** Compare with another VMELabeled. */
  bool Equals(mafVME *vme);

  /**
  Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
  set or get the Pose for a specified time. When setting, if the time does not exist
  the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
  interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const mafMatrix &mat);

  /**
  Return the list of timestamps for this VME. Timestamps list is 
  obtained merging timestamps for matrixes and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** Update the VME with the scalar values of the labels. */
  virtual void GenerateLabeledVolume();

  /** Set the Link */
  void SetVolumeLink(mafNode *n);

  /** Return the Link */
  mafNode *GetVolumeLink();

  /** return material attribute of this surface if present */
  mmaVolumeMaterial *GetMaterial();

  /** return icon */
  static char** GetIcon();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString("mafPipeBox");};

protected:
  //----------------------------------------------------------------------------
  // widget ID's
  //----------------------------------------------------------------------------
  enum VME_LABELED_VOLUME_DIALOG_ID
  {
    ID_INSERT_LABEL = Superclass::ID_LAST,
    ID_REMOVE_LABEL,
    ID_EDIT_LABEL,
    ID_LABELS,
    ID_FIT,	
    ID_SLICE,
    ID_SLICE_SLIDER,
    ID_INCREASE_SLICE,
    ID_DECREASE_SLICE,
    ID_OK,
    ID_CANCEL,
    ID_D_LABEL_NAME,
    ID_D_LABEL_VALUE,
    ID_D_MIN,
    ID_D_MAX,
    ID_SLIDER_MIN,
    ID_SLIDER_MAX,
    ID_LAST,
  };

  mafVMELabeledVolume();
  virtual ~mafVMELabeledVolume(); 

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();
  
  wxSlider        *m_MinSlider;
  wxSlider        *m_MaxSlider;

  wxTextCtrl     *m_LabelNameCtrl;
  wxTextCtrl     *m_LabelValueCtrl; 

  int m_MinAbsolute, m_MaxAbsolute;	
  int m_Min   , m_Max;
  int m_MinMin, m_MaxMin;
  int m_MinMax, m_MaxMax;	
  int m_MinValue ,m_MaxValue;

  int m_CheckListId;
  int m_ItemSelected;
  int m_LabelIntValue;

  bool  m_CheckMax, m_CheckMin;
  bool  m_EditMode;
  bool m_DataCopied;
  

  std::vector<wxString> m_LabelNameVector;
  std::vector<bool> m_CheckedVector;


  mafGUICheckListBox   *m_LabelCheckBox;  
  mafGUIDialogPreview  *m_Dlg;  
  mafGUIFloatSlider    *m_SliceSlider;
  mafRWI            *m_Rwi;
  mafTagItem        *m_TagLabel;
  mafVME            *m_VolumeLink;
  mafTransform      *m_Transform;

  wxString     m_LabelNameValue; 	
  wxString     m_LabelValueValue;

  double m_Slice;
  double m_SliceMin;
  double m_SliceMax;
  double m_SliceStep;
  double m_Bounds[6];

  double        m_BBox[6];
  double        m_Origin[3];
  double        m_XVect[3];
  double        m_YVect[3];
  
  wxString m_ItemLabel;

  vtkWindowLevelLookupTable * m_LookUpTable;
  vtkWindowLevelLookupTable * m_LookUpTableColor;
  vtkStructuredPoints       * m_SP;
  vtkRectilinearGrid        * m_RG;
  vtkProbeFilter          * m_ProbeFilter;
  vtkTexture        * m_Texture;  
  vtkPolyDataMapper * m_SMapper;
  vtkActor          * m_ActorSlice;  
  vtkDataSet        *m_Dataset;
  vtkPolyData       *m_Polydata;

  /** This method updates the look-up table. */
  void UpdateLookUpTable();

  /** If labels tags are present retrieve them. */
  void RetrieveTag();

  void CopyDataset();

  /** Updates the tags and the items of the checkBox and call the InternalPreUpadate. */
  void UpdateLabel();

  /** update the output data structure */
  void InternalPreUpdate();


  /** Copy the scalars of the VME linked. */
  void UpdateScalars();
  
  /** Builds operation's interface and visualization pipeline. */
  void CreateOpDialog();

  /** Remove operation's interface. */
  void DeleteOpDialog();

  /** Enable/disable VME widgets.*/
  void EnableWidgets(bool enable = true);

  /** Create the pipeline to generate the slice of the volume. */
  void CreateSlicePipeline();

  /** Re-slice the volume according to the new coordinate value. */
  void UpdateSlice();  

  /** This method returns the min value from the label used as input. */
  int GetMin( wxString &item );

  /** This method returns the max value from the label used as input. */
  int GetMax( wxString &item );
};
#endif
