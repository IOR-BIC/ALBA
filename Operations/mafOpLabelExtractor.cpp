/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpLabelExtractor.cpp,v $
  Language:  C++
  Date:      $Date: 2007-11-28 14:30:43 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani - porting Roberto Mucci 
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafOpLabelExtractor.h"

#include <wx/tokenzr.h>

#include "mafDecl.h"
#include "mmgCheckListBox.h"
#include "mafOp.h"
#include "mafEvent.h"
#include "mmgGui.h"
#include "mafVME.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurface.h"
#include "mafTagItem.h"
#include "mafTagArray.h"

#include "vtkVolumeResample.h"
#include "vtkImageData.h"
#include "vtkDataSet.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkExtractVOI.h"
#include "vtkImageThreshold.h"
#include "vtkMatrix4x4.h"
#include "vtkContourVolumeMapper.h"
#include "vtkStructuredPoints.h"
#include "vtkRectilinearGrid.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPointData.h"
#include "vtkImageToStructuredPoints.h"

#include <list>
#define OUTRANGE_SCALAR -1000


//----------------------------------------------------------------------------
mafOpLabelExtractor::mafOpLabelExtractor(const wxString& label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;

	m_Vme = NULL;
  m_Ds = NULL;
  m_OutputData = NULL;
  m_TagLabel = NULL;
	m_ValLabel     = 0;
	m_SurfaceName  = "label 0";
  m_SmoothVolume = 0;
  m_RadiusFactor = 0.5;
  m_RadiusFactorAfter = 0.5;
  m_StdDev[0] = m_StdDev[1] = 1.5;
  m_StdDev[2] = 2.0;
  m_StdDevAfter[0] = m_StdDevAfter[1] = 1.5;
  m_StdDevAfter[2] = 2.0;
  m_SamplingRate[0] = m_SamplingRate[1] = 3;
  m_SamplingRate[2] = 1;
}
//----------------------------------------------------------------------------
mafOpLabelExtractor::~mafOpLabelExtractor()
//----------------------------------------------------------------------------
{
	mafDEL(m_Vme);

  if (m_OutputData != NULL)
  {
    vtkDEL(m_OutputData);
  }
}
//----------------------------------------------------------------------------
mafOp* mafOpLabelExtractor::Copy()
//----------------------------------------------------------------------------
{
  return (new mafOpLabelExtractor(m_Label));
}
//----------------------------------------------------------------------------
bool mafOpLabelExtractor::Accept(mafNode *vme)
//----------------------------------------------------------------------------
{
	return (vme != NULL && ((mafVME *)vme)->GetOutput()->IsA("mafVMEOutputVolume") && !vme->IsA("mafVMEImage")); 
}
//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
enum {
	ID_LABEL = MINID,	
  ID_SMOOTH,
  ID_RADIUS_FACTOR,
  ID_STD_DEVIATION,
  ID_SAMPLING_RATE,
  ID_RADIUS_FACTOR_AFTER,
  ID_STD_DEVIATION_AFTER,
  ID_LABELS,
	ID_NAME,
};

//-----------------------------------------------------------------------
bool mafOpLabelExtractor::RetrieveTag()
//----------------------------------------------------------------------
{ 
  bool tagPresent = m_Input->GetTagArray()->IsTagPresent("LABELS");
  if (tagPresent)
  {
    m_TagLabel = new mafTagItem;
    m_TagLabel = m_Input->GetTagArray()->GetTag( "LABELS" );
    return tagPresent;
  }
  else
    return !tagPresent;
}

//----------------------------------------------------------------------------
void mafOpLabelExtractor::OpRun()   
//----------------------------------------------------------------------------
{
	m_Gui = new mmgGui(this);
	m_Gui->SetListener(this);

	m_Gui->Bool(ID_SMOOTH,_("smooth"),&m_SmoothVolume,0,_("gaussian smooth for extracting big surface"));
  m_Gui->Divider(2);
  m_Gui->Float(ID_RADIUS_FACTOR,_("rad. factor"),&m_RadiusFactor,0.1,MAXFLOAT,0,2,_("max distance to consider for the smooth."));
  m_Gui->Vector(ID_STD_DEVIATION,_("std dev."),m_StdDev, 0.1, MAXFLOAT,2,_("standard deviation for the smooth."));
  m_Gui->Divider();
  m_Gui->Vector(ID_SAMPLING_RATE,_("sample rate"),m_SamplingRate, 1, MAXINT,_("sampling rate for volume sub-sampling."));
  m_Gui->Divider();
  m_Gui->Float(ID_RADIUS_FACTOR_AFTER,_("rad. factor"),&m_RadiusFactorAfter,0.1,MAXFLOAT,0,2,_("max distance to consider for the smooth."));
  m_Gui->Vector(ID_STD_DEVIATION_AFTER,_("std dev."),m_StdDevAfter, 0.1, MAXFLOAT,2,_("standard deviation for the smooth."));
  m_Gui->Divider(2);
  m_Gui->Divider();

  if (m_Input->IsA("medVMELabeledVolume"))
  {
    m_LabelCheckBox = m_Gui->CheckList(ID_LABELS,_("Labels"),360,_("Chose label to extract"));

    typedef std::list< wxString > LIST;
    LIST myList;     
    LIST::iterator myListIter;   

    // If there is a tag named "LABELS" then I have to load the labels in the correct position in the listbox
    if  (RetrieveTag())
    {
      int noc = m_TagLabel->GetNumberOfComponents();
      if(noc != 0)
      {
        for ( unsigned int i = 0; i < noc; i++ )
        {
          wxString label = m_TagLabel->GetValue( i );
          if ( label != "" )
          {
            myList.push_back( label );
          }
        }     

        int checkListId = 0;
        for( myListIter = myList.begin(); myListIter != myList.end(); myListIter++ )
        {
          for ( unsigned int j = 0; j < noc; j++ )
          {
            wxString component = m_TagLabel->GetValue( j );
            if ( component != "" )
            {
              wxString labelName = *myListIter;
              if ( component == labelName )
              {
                m_LabelCheckBox->AddItem(checkListId, component, false);
                FillLabelVector(component, false);
                checkListId++;
              }
            }
          }  
        }
      }
    }  
  }
  else
  {
    m_Gui->Double(ID_LABEL,_("label"), &m_ValLabel);
    m_Gui->String(ID_NAME,_("name"),&m_SurfaceName);
  }

  m_Gui->Divider();
	m_Gui->OkCancel();

  m_Gui->Enable(ID_RADIUS_FACTOR,m_SmoothVolume != 0);
  m_Gui->Enable(ID_STD_DEVIATION,m_SmoothVolume != 0);
  m_Gui->Enable(ID_RADIUS_FACTOR_AFTER,m_SmoothVolume != 0);
  m_Gui->Enable(ID_STD_DEVIATION_AFTER,m_SmoothVolume != 0);
  m_Gui->Enable(ID_SAMPLING_RATE,m_SmoothVolume != 0);
	ShowGui();
}

//----------------------------------------------------------------------------
void mafOpLabelExtractor::FillLabelVector(wxString name, bool checked)
//----------------------------------------------------------------------------
{
  m_LabelNameVector.push_back(name);
  m_CheckedVector.push_back(checked);
}

//----------------------------------------------------------------------------
void mafOpLabelExtractor::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
		  case wxOK:
			  ExtractLabel();
			  if(((mafVME *)m_Input)->GetOutput()->GetVTKData() != NULL)
				  OpStop(OP_RUN_OK);
			  else
			  {
				  wxString msg = wxString::Format("Label %d not found!", m_ValLabel);
				  wxMessageBox(msg,"Warning");
				  OpStop(OP_RUN_CANCEL);
			  }
		  break;
		  case wxCANCEL:
			  OpStop(OP_RUN_CANCEL);
		  break;
      case ID_SMOOTH:
        m_Gui->Enable(ID_RADIUS_FACTOR,m_SmoothVolume != 0);
        m_Gui->Enable(ID_STD_DEVIATION,m_SmoothVolume != 0);
        m_Gui->Enable(ID_SAMPLING_RATE,m_SmoothVolume != 0);
        m_Gui->Enable(ID_RADIUS_FACTOR_AFTER,m_SmoothVolume != 0);
        m_Gui->Enable(ID_STD_DEVIATION_AFTER,m_SmoothVolume != 0);
      break;
      case ID_LABELS:
        {
          for (int i = 0; i < m_CheckedVector.size(); i++)
          {
            if (m_CheckedVector.at(i) != m_LabelCheckBox->IsItemChecked(i))
            {
              m_CheckedVector[i] = m_LabelCheckBox->IsItemChecked(i);
              break;
            }
          } 
        }
        break;
      default:
          mafEventMacro(*e); 
        break;
    }
  }
}
//----------------------------------------------------------------------------
void mafOpLabelExtractor::UpdateDataLabel()
//----------------------------------------------------------------------------
{
  mafNode *linkedNode = m_Input->GetLink("VolumeLink");
  mafSmartPointer<mafVME> linkedVolume = mafVME::SafeDownCast(linkedNode);

  //Get dataset from volume linked to
  vtkDataSet *data = linkedVolume->GetOutput()->GetVTKData();
  data->Update();

  //Create a copy of the dataset not to modify the original one
  m_Ds = data->NewInstance();
  m_Ds->DeepCopy(data);

}

//----------------------------------------------------------------------------
void mafOpLabelExtractor::GenerateLabeledVolume()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkDataArray> originalScalars = m_OutputData->GetPointData()->GetScalars(); 
  vtkMAFSmartPointer<vtkDataArray> labelScalars = m_OutputData->GetPointData()->GetScalars();

  std::vector<int> minVector;
  std::vector<int> maxVector;
  std::vector<int> labelIntVector;

  int counter= 0;
  //Fill the vectors of range and label value
  for (int c = 0; c < m_CheckedVector.size(); c++)
  {
    if (m_CheckedVector.at(c))
    {
      wxString label = m_LabelNameVector.at(c);
      wxStringTokenizer tkz(label,wxT(' '),wxTOKEN_RET_EMPTY_ALL);
      mafString labelName = tkz.GetNextToken().c_str();
      mafString labelIntStr = tkz.GetNextToken().c_str();
      int labelIntValue = atoi(labelIntStr);
      labelIntVector.push_back(labelIntValue);
      //Set the label value for vtkImageThreshold
      m_ValLabel = labelIntValue;
      mafString minStr = tkz.GetNextToken().c_str();
      int minValue = atof(minStr);
      minVector.push_back(minValue);
      mafString maxStr = tkz.GetNextToken().c_str();
      int mxValue = atof(maxStr);
      maxVector.push_back(mxValue);
      counter++;
    }
  }

  //Modify the scalars value, with the value of the label checked
  int labelVlaue;
  if (counter != 0)
  {
    int not = originalScalars->GetNumberOfTuples();
    for ( int i = 0; i < not; i++ )
    {
      bool modified = false;
      double scalarValue = labelScalars->GetComponent( i, 0 );
      for (int c = 0; c < labelIntVector.size(); c++)
      {
        if ( scalarValue >= minVector.at(c) && scalarValue <= maxVector.at(c))
        { 
          labelVlaue = labelIntVector.at(c);
          labelScalars->SetTuple1( i, labelVlaue ); 
          modified = true;
        }
      }
      if (!modified)
      {
        labelScalars->SetTuple1( i, OUTRANGE_SCALAR ); 
      }
    }

    labelScalars->Modified();
    m_OutputData->GetPointData()->SetScalars(labelScalars);
    m_OutputData->Modified();
  }
}

//----------------------------------------------------------------------------
void mafOpLabelExtractor::ExtractLabel()
//----------------------------------------------------------------------------
{
  mafVME *vme = NULL;
  
  if (m_Input->IsA("medVMELabeledVolume"))
  {
    UpdateDataLabel();
  }
  else
  {
    mafSmartPointer<mafVME> vmeLabeled = (mafVME *)m_Input;
    m_Ds = vmeLabeled->GetOutput()->GetVTKData();
    m_Ds->Update();
  }

  vtkMAFSmartPointer<vtkImageData> vol;
  vtkMAFSmartPointer<vtkImageToStructuredPoints> imageToSp;

  //setting the ImageData for RectilinearGrid
  if (m_Ds->IsA("vtkRectilinearGrid"))
  {
    vtkMAFSmartPointer<vtkRectilinearGrid> rgrid = (vtkRectilinearGrid*) m_Ds;

    double bounds[6];
    rgrid->GetBounds(bounds);

    vtkMAFSmartPointer<vtkStructuredPoints> sp;

    double xmin = bounds[0];
    double xmax = bounds[1];
    double ymin = bounds[2];
    double ymax = bounds[3];
    double zmin = bounds[4];
    double zmax = bounds[5];	 

    double volumeSpacing[3];

    volumeSpacing[0] = VTK_DOUBLE_MAX;
    volumeSpacing[1] = VTK_DOUBLE_MAX;
    volumeSpacing[2] = VTK_DOUBLE_MAX;
 
    for (int xi = 1; xi < rgrid->GetXCoordinates()->GetNumberOfTuples (); xi++)
    {
      double spcx = rgrid->GetXCoordinates()->GetTuple1(xi)-rgrid->GetXCoordinates()->GetTuple1(xi-1);
      if (volumeSpacing[0] > spcx && spcx >= 0.1)
        volumeSpacing[0] = spcx;
    }
    if (volumeSpacing[0] <= 0.1)
    {
      volumeSpacing[0] = 1;
    }

    for (int yi = 1; yi < rgrid->GetYCoordinates()->GetNumberOfTuples (); yi++)
    {
      double spcy = rgrid->GetYCoordinates()->GetTuple1(yi)-rgrid->GetYCoordinates()->GetTuple1(yi-1);
      if (volumeSpacing[1] > spcy && spcy >= 0.1)
        volumeSpacing[1] = spcy;
    }
    if (volumeSpacing[1] <= 0.1)
    {
      volumeSpacing[1] = 1;
    }

    for (int zi = 1; zi < rgrid->GetZCoordinates()->GetNumberOfTuples (); zi++)
    {
      double spcz = rgrid->GetZCoordinates()->GetTuple1(zi)-rgrid->GetZCoordinates()->GetTuple1(zi-1);
      if (volumeSpacing[2] > spcz && spcz != 0)
        volumeSpacing[2] = spcz;
    }
    if (volumeSpacing[2] <= 0.1)
    {
      volumeSpacing[2] = 1;
    }

    int output_extent[6];
    output_extent[0] = 0;
    output_extent[1] = (bounds[1] - bounds[0]) / volumeSpacing[0];
    output_extent[2] = 0;
    output_extent[3] = (bounds[3] - bounds[2]) / volumeSpacing[1];
    output_extent[4] = 0;
    output_extent[5] = (bounds[5] - bounds[4]) / volumeSpacing[2];

    // the resample filter
    vtkMAFSmartPointer<vtkVolumeResample> resampler;
    resampler->SetZeroValue(0);

    double origin[3];
    origin[0] = bounds[0];
    origin[1] = bounds[2];
    origin[2] = bounds[4];

    resampler->SetVolumeOrigin(origin[0],origin[1],origin[2]);

    sp->SetSpacing(volumeSpacing);
    sp->SetScalarType(rgrid->GetPointData()->GetScalars()->GetDataType());
    sp->SetExtent(output_extent);
    sp->SetUpdateExtent(output_extent);

    double sr[2];
    rgrid->GetScalarRange(sr);

    double w = sr[1] - sr[0];
    double l = (sr[1] + sr[0]) * 0.5;

    resampler->SetWindow(w);
    resampler->SetLevel(l);
    resampler->SetInput(rgrid);
    resampler->SetOutput(sp);
    resampler->AutoSpacingOff();
    resampler->Update();

    sp->SetSource(NULL);
    sp->SetOrigin(bounds[0],bounds[2],bounds[4]);
    m_OutputData = sp->NewInstance();
    m_OutputData->DeepCopy(sp);

    if (m_Input->IsA("medVMELabeledVolume"))
    {
      GenerateLabeledVolume();
    }

    vol->DeepCopy((vtkImageData *)m_OutputData);
    //mafDEL(m_OutputData);
  }
  else
  {
    vtkMAFSmartPointer<vtkStructuredPoints> sp = (vtkStructuredPoints *)m_Ds;
    vol->DeepCopy((vtkImageData *)sp);  
  }
  

	vtkMAFSmartPointer<vtkImageThreshold> it;
  it->SetInput(vol);

	double maxValue;
	switch(vol->GetScalarType()) 
	{
	  case VTK_UNSIGNED_SHORT:
      maxValue = VTK_UNSIGNED_SHORT_MAX;
		break;
	  case VTK_SHORT:
			maxValue = VTK_SHORT_MAX;
		break;
		case VTK_UNSIGNED_CHAR:
			maxValue = VTK_UNSIGNED_CHAR_MAX;
		break;
		case VTK_CHAR:
			maxValue = VTK_CHAR_MAX;
		break;
    case VTK_DOUBLE:
      maxValue = VTK_DOUBLE_MAX;
    break;
	  default:
			maxValue = 255;
		break;
	}
  it->SetInValue(maxValue);
  it->SetOutValue(0);
	it->SetOutputScalarType(vol->GetScalarType());
  it->ThresholdBetween(m_ValLabel, m_ValLabel);
  it->Update();

  vtkMAFSmartPointer<vtkImageGaussianSmooth> smooth;
  vtkMAFSmartPointer<vtkImageGaussianSmooth> smoothAfter;
  vtkMAFSmartPointer<vtkExtractVOI> extract;

  if(m_SmoothVolume)
  {
    int b[6];
    smooth->SetInput(it->GetOutput());
    smooth->SetRadiusFactor(m_RadiusFactor);
    smooth->SetStandardDeviation(m_StdDev[0],m_StdDev[1],m_StdDev[2]);
    smooth->SetDimensionality(3);
    smooth->Update();
    smooth->GetOutput()->GetExtent(b);
    extract->SetInput(smooth->GetOutput());
    extract->SetVOI(b);
    extract->SetSampleRate(m_SamplingRate);
    extract->Update();
    smoothAfter->SetInput(extract->GetOutput());
    smoothAfter->SetRadiusFactor(m_RadiusFactorAfter);
    smoothAfter->SetStandardDeviation(m_StdDevAfter[0],m_StdDevAfter[1],m_StdDevAfter[2]);
    smoothAfter->SetDimensionality(3);
    smoothAfter->Update();
    vol->DeepCopy(smoothAfter->GetOutput());
  }
  else
  {
    vol->DeepCopy(it->GetOutput());
  }

  imageToSp->SetInput(vol);
  imageToSp->Update();
 
  vtkMAFSmartPointer<vtkContourVolumeMapper> contourMapper;
  contourMapper->SetInput((vtkDataSet *)imageToSp->GetOutput());
  contourMapper->SetContourValue(m_ValLabel);

	vtkMAFSmartPointer<vtkPolyData> surface;
  contourMapper->GetOutput(0, surface);	
	contourMapper->Update();

  mafNEW(m_Vme);
	m_Vme->SetData(surface, 0.0);
  m_Vme->SetName(m_SurfaceName.c_str());
  m_Vme->Update();
  m_Output = m_Vme;
}

//----------------------------------------------------------------------------
void mafOpLabelExtractor::SetLabel(double labelValue)
//----------------------------------------------------------------------------
{
  m_ValLabel = labelValue;
}

//----------------------------------------------------------------------------
void mafOpLabelExtractor::SmoothMode(bool smoothMode)
//----------------------------------------------------------------------------
{
  m_SmoothVolume = smoothMode;
}
