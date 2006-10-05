/*=========================================================================

 Program:   Visualization Toolkit
 Module:    $RCSfile: mafVMERawMotionData.cpp,v $
 Language:  C++
 Date:      $Date: 2006-10-05 08:46:10 $
 Version:   $Revision: 1.1 $

=========================================================================*/
#include "mafVMERawMotionData.h"
#include "vtkObjectFactory.h"
#include "vtkCommand.h"
#include "mafString.h"	


#include "mafVMELandmarkCloud.h"  

#include <vnl\vnl_matrix.h>      

#include "mafVMEItem.h"
#include "itkRawMotionImporterUtility.h"
#include <iostream>
#include <string>

//----------------------------------------------------------------------------
//If there is no dictionary or if the dictionary
//does not define a NOT_USED value I use
//the value -9999.00 to identify landmarks that
//are not visible at a given timestamp.

const double CONST_NOT_USED_IOR_MAL = -9999.00;

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMERawMotionData)
//-------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafVMERawMotionData::mafVMERawMotionData()
//----------------------------------------------------------------------------
{ 
	this->Dictionary = 0;
}
//----------------------------------------------------------------------------
mafVMERawMotionData::~mafVMERawMotionData()
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void mafVMERawMotionData::SetDictionaryFileName(const char *name)
//----------------------------------------------------------------------------
{
  this->DictionaryFileName = name;
  this->DictionaryOn();
  this->Modified();
}

//----------------------------------------------------------------------------
void mafVMERawMotionData::SetFileName(const char *name)
//----------------------------------------------------------------------------
{
  this->FileName = name;
  this->Modified();
}

//----------------------------------------------------------------------------
int mafVMERawMotionData::Read()
//----------------------------------------------------------------------------
{
		
  itkRawMotionImporterUtility utils;

  vnl_matrix<double> M;

  if (utils.ReadMatrix(M,this->FileName))
  {
    mafErrorMacro("File does not exist!");
	  return 1;
  }
  
  //Read dictionary
  std::string v_lmname, v_segment_name, temp_string;

  double not_used_identifier = -1;

	if (this->GetDictionary())
	{	
    std::ifstream v_dictionary(this->DictionaryFileName, std::ios::in);

		int v_current_col = 0;	

		if(v_dictionary.is_open() != 0)
		{
			//get the first string
			v_dictionary >> temp_string;
			
			if (temp_string == "NOT_USED")
			{
				v_dictionary >> not_used_identifier;
			}
			else
			{
				not_used_identifier = CONST_NOT_USED_IOR_MAL;
				//go back to the beginning of the stream
				v_dictionary.seekg(std::ios::beg); 
			}

			while(v_dictionary >> v_lmname)	
			{
			  v_dictionary >> v_segment_name;			
        v_current_dlc = mafVMELandmarkCloud::SafeDownCast(this->FindInTreeByName(v_segment_name.c_str()));
			  if (v_current_dlc == NULL)
			  {
			    //Create the new cloud
			    mafNEW(v_current_dlc);
			    v_current_dlc->SetRadius(15);
			    v_current_dlc->SetName(v_segment_name.c_str());

			    //Add new cloud to vme tree
			    this->AddChild(v_current_dlc);
			  }
						
				v_current_dlc->AppendLandmark(v_lmname.c_str());

				for (int i = 0; i < M.rows(); i++)
				{ 
				  v_current_dlc->SetLandmark(v_lmname.c_str(),
					                          M(i, v_current_col),						
					                          M(i, v_current_col + 1),
					                          M(i, v_current_col + 2), 
					                            i);
						
					//check if the landmark is visible for the given timestamp;
					//if not i set his visibility to 0;
					if (M(i, v_current_col) == not_used_identifier || 
							M(i, v_current_col + 1) == not_used_identifier ||
							M(i, v_current_col + 2) == not_used_identifier)
					{
						double pippo = M(i, v_current_col + 2);
						v_current_dlc->SetLandmarkVisibility(v_lmname.c_str(), 0, i);
					}
				}	
				v_current_col +=  3;
//				v_current_dlc = NULL;
			}//while	
		}//if vdict is open
		else
		{
		  //vcl_cout << "File does not exist!\n";
		  return 1;
		}

	}//if this->getdict
	else //this->dictionary
	{
		//If there is no dictionary available
		//create only one cloud and add all the landmark to it

    not_used_identifier = 9999;
			
		int current_lm = 0;
		mafNEW(v_dlc);
    v_dlc->SetName("dummy segment");
		v_dlc->SetRadius(15);
    		
	  //Create (M.columns() / 3) landmarks
		for (int j = 0; j < M.columns(); j += 3)
		{
      mafString lm_name;
			lm_name ="lm_";
      lm_name << current_lm;
              
			v_dlc->AppendLandmark(lm_name);
			current_lm++;

			for (int i = 0; i < M.rows(); i++)
			{ 
				v_dlc->SetLandmark(lm_name,
				M(i, j),						
				M(i, j + 1),
				M(i, j + 2), 
				i);

				//check if the landmark is visible for the given timestamp;
				//if not i set his visibility to 0;
				if (fabs(M(i, j)) > not_used_identifier || 
						fabs(M(i, j + 1)) > not_used_identifier ||
						fabs(M(i, j + 2)) > not_used_identifier)
				{
					v_dlc->SetLandmarkVisibility(lm_name, 0, i);
				}
			}	
			
		}						
		this->AddChild(v_dlc);						
//		v_dlc->Delete();
//		v_dlc = NULL;
	}//if
	  
  return 0;
}
