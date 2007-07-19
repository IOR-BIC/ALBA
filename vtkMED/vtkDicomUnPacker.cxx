/*=========================================================================

  Module:    vtkDicomUnPacker.cxx
  Language:  C++
  Date:      06/2000
  Version:   .1
  Thanks:    

=========================================================================*/
#include "vtkDicomUnPacker.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"

vtkCxxRevisionMacro(vtkDicomUnPacker, "$Revision: 1.8 $");
vtkStandardNewMacro(vtkDicomUnPacker);

//----------------------------------------------------------------------------
// Construct an instance of vtkDicomUnPacker with no data.
vtkDicomUnPacker::vtkDicomUnPacker()
//----------------------------------------------------------------------------
{
  setlocale(LC_ALL, "C");

  DebugFlag = 0;
	FlipImage = 0;
	Flag      = 0;
  DictionaryRead = false;
  UseDefaultDictionary = 1;
	DictionaryFileName = NULL;
	strcpy(PatientName,"                   ");
	strcpy(StudyUID,"                   ");
	strcpy(Study,"                   ");
	strcpy(PatientBirthDate,"          ");
	strcpy(CTMode[0],"           ");
	strcpy(CTMode[1],"           ");
	strcpy(StudyDate,"           ");
	strcpy(PatientID, "          ");
	strcpy(PatientSex," ");
	strcpy(Modality,"  ");
	//SetDictionaryFileName("dicom3.txt");
	int InstanceNumber = -1;
  int CardiacNumberOfImages = -1;
  double TriggerTime = -1.0;
	UnPackFromFileOn();
  DICT_line = 0;
	Status = 0;
}
//----------------------------------------------------------------------------
vtkDicomUnPacker::~vtkDicomUnPacker()
//----------------------------------------------------------------------------
{
	// Free the compressor
	SetDictionaryFileName(NULL);
}
//----------------------------------------------------------------------------
void vtkDicomUnPacker::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  vtkImageUnPacker::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------
void vtkDicomUnPacker::parser_multepl(tipo str , FILE * fp, long Length, VALUE * VAL)
//----------------------------------------------------------------------------
{
  long p,r;
  char c;
/***********  init  *********/
  r = 0; 
  VAL->type = str;
  VAL->mult = 0;
/***************************/ 

	for (p=0; p< Length; p++)
	{
		c=fgetc(fp);
		if(c=='\\') 
		{ 
			VAL->stringa[VAL->mult][r]='\0'; r=0; VAL->mult++; 
		}
		else 
		{        
			VAL->stringa[VAL->mult][r]=c;  
			r++;
			VAL->stringa[VAL->mult][r]='\0';
		}
	}
	   
	if (str == num)     
	{
		for (r=0; r<=VAL->mult; r++) 
			VAL->num[r]=atof(VAL->stringa[r]);
	}
	else 
	{
		VAL->stringa[VAL->mult][r]=0;
	}
}
//----------------------------------------------------------------------------
long vtkDicomUnPacker::find (long Group, long Element , DICOM DICT[], long n_line)
//----------------------------------------------------------------------------
{
	/* FROM (Group, Element)  returns the m_Position of rispective pattern into Dictionary */ 
  long n;
  long index=-1;
  if ((Group>20479) & (Group<20736)) Group=20480; /*  repeating groups 0x50xx */ 
  if ((Group>24575) & (Group<24832)) Group=24576; /*  repeating group  0x60xx */ 
  for (n=0; n<n_line; n++)  if((DICT[n].Group==Group) & (DICT[n].Element==Element)) { index=n; break;}
  return index;
}
//----------------------------------------------------------------------------
void vtkDicomUnPacker::SetDictionaryFileName(const char *filename)
//----------------------------------------------------------------------------
 {
   DictionaryRead = false;
   DictionaryFileName = filename; 
   this->Modified();
 }
//----------------------------------------------------------------------------
int vtkDicomUnPacker::load_dictionary_from_file(DICOM DICT[])
//----------------------------------------------------------------------------
{   /* return  N_line and structure DICOM */ 
  char pre=' ', cur=' ';
  uint32 p_line;
  uint8 r;
  char pt[5];
  FILE *Dictionary;
	
	if (DictionaryFileName == NULL || (Dictionary=fopen(DictionaryFileName, "rb")) == NULL)
	{
		return -1;
	}

	p_line=0;
	while ( (int) cur != EOF)
	{
		cur=( char)fgetc(Dictionary);
    if (cur =='=')
		{
			switch(pre)
			{
				case 'p' :
					fgetc(Dictionary);/* skip character " */
					r=0; cur=fgetc(Dictionary);
					while (cur != '"')
					{		
						if(cur=='x')	cur='0'; /*  repeating groups  */                 
						*(pt + r) = cur;
						cur=(char)fgetc(Dictionary); 
						r++;
					} 
					*(pt + r)='\0';
					DICT[p_line].Group= strtol(pt,NULL,16);
        break;
				case 't' :
					fgetc(Dictionary);/* skip character " */
					r=0; cur=fgetc(Dictionary);
					while (cur != '"')
					{
						*(pt + r) = cur;	
					  cur=(char)fgetc(Dictionary); 
            r++;
					} 
					*(pt + r)='\0';
					DICT[p_line].Element= strtol(pt,NULL,16);
			  break;
        case 'S' :
					fgetc(Dictionary);/* skip character " */
					r=0; cur=fgetc(Dictionary);
					while (cur != '"')
					{
						DICT[p_line].Version[r]=cur;
					  cur=(char)fgetc(Dictionary); 
            r++;
					}    
          DICT[p_line].Version[r]='\0';  
			  break;
        case 'R' :
					fgetc(Dictionary);/* skip character " */
					r=0; cur=fgetc(Dictionary);
					while (cur != '"')
					{
						DICT[p_line].Val_Repr[r]=cur;
					  cur=(char)fgetc(Dictionary); 
            r++;
					} 
					DICT[p_line].Val_Repr[r] ='\0';
			  break;
        case 'M' :
					fgetc(Dictionary);/* skip character " */
					r=0; cur=fgetc(Dictionary);
					while (cur != '"')
					{
						DICT[p_line].Val_Mult[r]=cur;
					  cur=(char)fgetc(Dictionary);
            r++;
					} 
					DICT[p_line].Val_Mult[r] ='\0'; 
			  break;
        case 'd' :
					fgetc(Dictionary);/* skip character " */
					r=0; cur=fgetc(Dictionary);
					while (cur != '"')
					{
						DICT[p_line].Keyword[r]=cur;
					  cur=(char)fgetc(Dictionary);  
            r++;
					} 
					DICT[p_line].Keyword[r] ='\0';
			  break;
        case 'e' :
					fgetc(Dictionary);/* skip character " */
					r=0; cur=fgetc(Dictionary);
					while (cur != '"')
					{
						DICT[p_line].Name[r]=cur;
						cur=(char)fgetc(Dictionary); 
            r++;
					} 
					DICT[p_line].Name[r] ='\0';
			  break;		   
			}
		}
		else if ( (int)cur == 10 )
			p_line++; /* carriage return */  
		pre=cur;
  }
  fclose(Dictionary);
  DictionaryRead = true;
  return p_line;
}
//----------------------------------------------------------------------------
int vtkDicomUnPacker::load_dictionary(DICOM DICT[])
//----------------------------------------------------------------------------
{
/*  int num_elem = sizeof(DICOMDefaultDictionary)/sizeof(DICOMDefaultDictionary[0]);
  int p_line = 0;
  for (; p_line < num_elem; p_line++)
  {
    DICT[p_line].Group    = DICOMDefaultDictionary[p_line].Group;
    DICT[p_line].Element  = DICOMDefaultDictionary[p_line].Element;
    strcpy(DICT[p_line].Version,DICOMDefaultDictionary[p_line].Version.c_str());
    strcpy(DICT[p_line].Val_Repr,DICOMDefaultDictionary[p_line].Val_Repr.c_str());
    strcpy(DICT[p_line].Val_Mult,DICOMDefaultDictionary[p_line].Val_Mult.c_str());
    strcpy(DICT[p_line].Keyword,DICOMDefaultDictionary[p_line].Keyword.c_str());
    strcpy(DICT[p_line].Name,DICOMDefaultDictionary[p_line].Name.c_str());
  }

  DictionaryRead = true;
  return p_line;*/
  return -1;
}
//----------------------------------------------------------------------------
uint16 read16 ( FILE* fp, char little_endian)  
//----------------------------------------------------------------------------
{
	int n;
	uint8 t1, t2;   
	n = fread(&t1,sizeof(t1), 1, fp); 
	n = fread(&t2,sizeof(t2), 1, fp);

	if (little_endian)  return t1 + t2*256;
	else                return t1*256 + t2;
}    
//----------------------------------------------------------------------------
uint32 read32 ( FILE* fp, char little_endian )  
//----------------------------------------------------------------------------
{
  int n;
  uint8 t1, t2, t3, t4;
  n = fread(&t1, sizeof t1, 1, fp);                         
  n = fread(&t2, sizeof t2, 1, fp);                         
  n = fread(&t3, sizeof t3, 1, fp);                         
  n = fread(&t4, sizeof t4, 1, fp); 

  if (little_endian)    return t1 + t2*256 + t3*256*256 + t4*256*256*256;
  else                  return t1*256*256*256 + t2*256*256 + t3*256 + t4;
}
//----------------------------------------------------------------------------
void read (FILE* fp, char little_endian, char &value) 
//----------------------------------------------------------------------------
{
	value = fgetc(fp);
}
//----------------------------------------------------------------------------
void read (FILE* fp, char little_endian, short &value) 
//----------------------------------------------------------------------------
{
	value = read16(fp, little_endian);
}
//----------------------------------------------------------------------------
void read (FILE* fp, char little_endian, long &value) 
//----------------------------------------------------------------------------
{
	value = read32(fp, little_endian);
}
//----------------------------------------------------------------------------
int vtkDicomUnPacker::read_dicom_header(DICOM RESULT[], VALUE VALUES[], uint32 *size_image, uint32 *result_line)
//----------------------------------------------------------------------------
{
	bool   time_to_exit = false;
	char   first_one		= 1;
	char   explicitVR;
	int		 fseek_result = 0;
	uint8 *t;
	uint8 *pvr;
	uint8  vr0,vr1;
	uint16 groupWord;
	uint16 elementWord;
	uint32 elementLength;

	uint32 RESULT_line;
	long   pos;
	FILE  *fp;
	char   little_endian = 1;  /* set default little endian */ 

/*******************************    INIT    ***************************/
	if (FileName == NULL) 
  {
    vtkErrorMacro("Dicom File Name NULL"); 
    return -1;
  }
	if (DictionaryFileName == NULL) 
  {
    vtkErrorMacro ("Dictionary File Name NULL"); 
    return -1;
  }
	t = (uint8 *)calloc(4,sizeof(uint8));
	if (!DictionaryRead)
	{
    if (UseDefaultDictionary)
    {
      DICT_line = load_dictionary(DICT);  /* LOAD DICTIONARY dict_line = lines number of dictionary DICT */
    }
    else
    {
      DICT_line = load_dictionary_from_file(DICT);  /* LOAD DICTIONARY dict_line = lines number of dictionary DICT */
    }
	}
	if (DICT_line < 0) 
  {
    vtkErrorMacro(" Error reading DICOM Dictionary"); 
    return -1;
  }
	if ((fp=fopen(FileName, "rb")) == NULL) 
  {
    vtkErrorMacro (" Dicom File Not Reached \n"); 
    return -1;
  }
	RESULT_line = 0;
  /**********************************************************************/
	while (!time_to_exit)
	{ 
		explicitVR = 0;
		groupWord      = read16(fp,little_endian);
		if(feof(fp))
		{
			Status=-1;
			return -1;
		}
		elementWord    = read16(fp,little_endian);
		if(feof(fp))
		{
			Status=-1;
			return -1;
		}
		elementLength  = read32(fp,little_endian);
		if (first_one) 
		{ 	
			if (groupWord != 0 && elementWord == 0 && elementLength == 4)  
				first_one = 0;
			else if  (!little_endian &&  elementLength == 0x04000000)  
			{
				first_one = 0;
        little_endian = 1;
        rewind(fp);/* begin of file */ 
			}
			else if  (little_endian && elementLength == 0x04000000)     
			{
				first_one = 0;
        little_endian = 0;
				rewind(fp);/* begin of file */
			}     	               
      else 
			{
				first_one= 0 ;
        elementLength = 0;  //2006-04-04 by danno
	      fseek_result = fseek(fp,128,0); /* skip preamble bytes next 4 bytes should be "DICM" */ 
	      int a=fread(t, 1 , 4, fp);
	      if (t[0] != 'D' || t[1] != 'I' || t[2] != 'C' || t[3] != 'M')  
				{
					/* it's not proper part 10 - try w/out the 128 byte preamble */ 
	        rewind(fp);
	        groupWord   = read16(fp,little_endian);
	        elementWord    = read16(fp,little_endian);
	        elementLength  = read32(fp,little_endian); 
        }
			}
		}
		else 
		{ /* not first_one */ 
			pvr = (unsigned char*) &elementLength;
      if ( (pvr[0]=='O' && pvr[1]=='B') ||
           (pvr[0]=='U' && pvr[1]=='N') ||
           (pvr[0]=='O' && pvr[1]=='W') ||
			     (pvr[0]=='U' && pvr[1]=='T') ||
           (pvr[0]=='S' && pvr[1]=='Q') ) 
			{
				vr0=pvr[0]; vr1=pvr[1];
				explicitVR=1;
        elementLength = read32(fp,little_endian);
		  }
      else if ( (pvr[0]=='A' && pvr[1]=='E') ||
                (pvr[0]=='A' && pvr[1]=='S') ||
                (pvr[0]=='A' && pvr[1]=='T') ||
                (pvr[0]=='C' && pvr[1]=='S') ||
                (pvr[0]=='D' && pvr[1]=='A') ||
                (pvr[0]=='D' && pvr[1]=='S') ||
                (pvr[0]=='D' && pvr[1]=='T') ||
                (pvr[0]=='F' && pvr[1]=='L') ||
                (pvr[0]=='F' && pvr[1]=='D') ||
                (pvr[0]=='I' && pvr[1]=='S') ||
                (pvr[0]=='L' && pvr[1]=='O') ||
                (pvr[0]=='L' && pvr[1]=='T') ||
                (pvr[0]=='P' && pvr[1]=='N') ||
                (pvr[0]=='S' && pvr[1]=='H') ||
                (pvr[0]=='S' && pvr[1]=='L') ||
                (pvr[0]=='S' && pvr[1]=='S') ||
                (pvr[0]=='S' && pvr[1]=='T') ||
                (pvr[0]=='T' && pvr[1]=='M') ||
                (pvr[0]=='U' && pvr[1]=='I') ||
                (pvr[0]=='U' && pvr[1]=='L') ||
								(pvr[0]=='U' && pvr[1]=='S') ) 
			{
				vr0 = pvr[0]; 
        vr1 = pvr[1];
        explicitVR = 1;
        elementLength &= 0xffff0000; 
        elementLength >>= 16;      
      }
		}
		pos = find(groupWord, elementWord, DICT, DICT_line);
		if(((long)elementLength)==-1) 
			elementLength=0;
		if (pos<0) 
		{ /* NO DEFINED into CURRENT DICTIONARY */ 
			RESULT[RESULT_line].intoDictionary=no;
      RESULT[RESULT_line].Group=groupWord; 
      RESULT[RESULT_line].Element=elementWord;
      RESULT[RESULT_line].Version[0]='\0';
      RESULT[RESULT_line].Val_Repr[0]='\0';
      RESULT[RESULT_line].Val_Mult[0]='\0';
      RESULT[RESULT_line].Keyword[0]='\0';
	    RESULT[RESULT_line].Name[0]='\0';
		}
		else
		{
			RESULT[RESULT_line] = DICT[pos]; 
      RESULT[RESULT_line].intoDictionary = yes;
    }

		if ( RESULT[RESULT_line].intoDictionary == yes) 
		{
			if (explicitVR) 
			{
				RESULT[RESULT_line].Val_Repr[0] = vr0;
        RESULT[RESULT_line].Val_Repr[1] = vr1;
        RESULT[RESULT_line].Val_Repr[2] = '\0';
      }
      vr0 = RESULT[RESULT_line].Val_Repr[0];
      vr1 = RESULT[RESULT_line].Val_Repr[1];

      if( (vr0=='A') && (vr1=='E') || /* string of characters without numerical meaning                */ 
          (vr0=='A') && (vr1=='S') || /* string of characters without numerical meaning 	AGE	     */ 
          (vr0=='C') && (vr1=='S') || /* string of characters without numerical meaning                */
          (vr0=='D') && (vr1=='A') || /* string of characters without numerical meaning    DATE        */
          (vr0=='D') && (vr1=='T') || /* string of characters without numerical meaning    DATE TIME   */
          (vr0=='L') && (vr1=='O') || /* string of characters without numerical meaning    LONG STRING */
          (vr0=='L') && (vr1=='T') || /* string of characters without numerical meaning    LONG TEXT   */
          (vr0=='P') && (vr1=='N') || /* string of characters without numerical meaning    PERSON NAME */ 
          (vr0=='S') && (vr1=='H') || /* string of characters without numerical meaning    SHORT STRING*/   
          (vr0=='S') && (vr1=='T') || /* string of characters without numerical meaning    SHORT TEXT  */
          (vr0=='T') && (vr1=='M') || /* string of characters without numerical meaning    TIME        */
          (vr0=='U') && (vr1=='I') )  /* string of characters without numerical meaning    UNIQUE IDENTIFIER */
			{
				parser_multepl(string, fp, elementLength, &VALUES[RESULT_line]);
			}
			else if( (vr0=='D') && (vr1=='S') || /* string of characters representing floating number */ 
               (vr0=='I') && (vr1=='S') )  /* string of characters representing integer  number */ 
			{
				parser_multepl(num, fp, elementLength,& VALUES[RESULT_line]);
			}
			else if( (vr0=='U') && (vr1=='S') ) 
			{ /* unsigned short */ 
				VALUES[RESULT_line].type = num;
        VALUES[RESULT_line].mult = 0;
        VALUES[RESULT_line].num[VALUES[RESULT_line].mult] = 0;
        if(elementLength == 2)  
				{
					VALUES[RESULT_line].num[VALUES[RESULT_line].mult] = read16(fp,little_endian);
				}
        else  
				{
					fseek_result = fseek(fp,elementLength,SEEK_CUR);/* multeplicity is not considered on unsigned short */
				}
			}
			else if( (vr0=='U') && (vr1=='L') ) 
			{ /* unsigned long */ 
				VALUES[RESULT_line].type = num;
				VALUES[RESULT_line].mult = 0;
        VALUES[RESULT_line].num[VALUES[RESULT_line].mult] = 0;
				if(elementLength == 4)
				{
					VALUES[RESULT_line].num[VALUES[RESULT_line].mult] = read32(fp,little_endian);
				}
        else  
				{
					fseek_result = fseek(fp,elementLength,SEEK_CUR);/* multeplicity is not considered on unsigned long */
				}
			}
			else if( (vr0=='O') && (vr1=='B') ||
               (vr0=='O') && (vr1=='W') ) 
			{  /* only for size image */ 
				if ((groupWord == 0x7fe0) & (elementWord == 0x0010)) 
				{
					time_to_exit = true;
          *size_image  = elementLength;
				}
        else 
				{
					fseek_result = fseek(fp,elementLength,SEEK_CUR);
				}
			}
      else 
			{
				fseek_result = fseek(fp,elementLength,SEEK_CUR);
			}
		} /*  END   if ( RESULT[RESULT_line].intoDictionary == yes) */ 
		else  
		{
			fseek_result = fseek(fp,elementLength,SEEK_CUR);
			if( (RESULT_line == SIZE_TAG && fseek_result != 0) || feof(fp))
			{
				Status = -1;
				return -1;
			}
		}
    RESULT_line++;
  } /* END  while (time_to_exit) */ 
	*result_line = RESULT_line;
	free((uint8 *)t);
	fclose(fp);
	return *result_line;
}
//----------------------------------------------------------------------------
template <class T>
int read_dicom_string_image(char *filename, T *IMAGE, double slope_value, double intercept_value, int rows, int cols, int flip=0, int Signed=1)
//----------------------------------------------------------------------------
{
	bool    time_to_exit = false;
	char    first_one    = 1;
	char    explicitVR;
	uint8  *t;
	uint8  *pvr;
	uint16  groupWord;
	uint16  elementWord;
	uint32  elementLength;
	FILE   *fp;
	char    little_endian = 1;  /* set default little endian */ 
	double  HU_value;
	T value;
	// int cols = m_DimX;
	// int rows = m_DimY;

  /*********************************   INIT   ***************************/
  t = (uint8 *) calloc(4,sizeof(uint8));
  if ((fp = fopen(filename, "rb")) == NULL)
  {
    return -1;
  } 

  /**********************************************************************/
  while (!time_to_exit)
	{ 
		explicitVR = 0;
		groupWord      = read16(fp,little_endian);
		elementWord    = read16(fp,little_endian);
		elementLength  = read32(fp,little_endian);
		if (first_one) 
		{
			if (groupWord != 0 && elementWord == 0 && elementLength == 4)  
				first_one = 0;
			else if  (!little_endian &&  elementLength == 0x04000000)  
			{
				first_one = 0;
				little_endian = 1;
				rewind(fp);/* inizio del file */ 
			}
			else if  (little_endian && elementLength == 0x04000000)
			{
				first_one = 0;
				little_endian = 0;
				rewind(fp);/* inizio del file */ 
			}     	               
			else 
			{
				first_one= 0 ;
        elementLength = 0;  //2006-04-04 by danno
				fseek(fp,128,0); /* skip preamble bytes next 4 bytes should be "DICM" */ 
				fread(t, 1 , 4, fp);
				if (t[0] != 'D' || t[1] != 'I' || t[2] != 'C' || t[3] != 'M')  
				{
					/* it's not proper part 10 - try w/out the 128 byte preamble */ 
					rewind(fp);
					groupWord   = read16(fp,little_endian);
					elementWord    = read16(fp,little_endian);
					elementLength  = read32(fp,little_endian); 
				}
			}
		}
		else 
		{ /* not first_one */ 
			pvr = (unsigned char*) &elementLength;
			if ( (pvr[0]=='O' && pvr[1]=='B') ||
					 (pvr[0]=='U' && pvr[1]=='N') ||
					 (pvr[0]=='O' && pvr[1]=='W') ||
					 (pvr[0]=='U' && pvr[1]=='T') ||
					 (pvr[0]=='S' && pvr[1]=='Q') ) 
			{
				explicitVR=1;
				elementLength = read32(fp,little_endian);
		  }
      else if ( (pvr[0]=='A' && pvr[1]=='E') ||
								(pvr[0]=='A' && pvr[1]=='S') ||
                (pvr[0]=='A' && pvr[1]=='T') ||
                (pvr[0]=='C' && pvr[1]=='S') ||
                (pvr[0]=='D' && pvr[1]=='A') ||
                (pvr[0]=='D' && pvr[1]=='S') ||
                (pvr[0]=='D' && pvr[1]=='T') ||
                (pvr[0]=='F' && pvr[1]=='L') ||
                (pvr[0]=='F' && pvr[1]=='D') ||
                (pvr[0]=='I' && pvr[1]=='S') ||
                (pvr[0]=='L' && pvr[1]=='O') ||
                (pvr[0]=='L' && pvr[1]=='T') ||
                (pvr[0]=='P' && pvr[1]=='N') ||
                (pvr[0]=='S' && pvr[1]=='H') ||
                (pvr[0]=='S' && pvr[1]=='L') ||
                (pvr[0]=='S' && pvr[1]=='S') ||
                (pvr[0]=='S' && pvr[1]=='T') ||
                (pvr[0]=='T' && pvr[1]=='M') ||
                (pvr[0]=='U' && pvr[1]=='I') ||
                (pvr[0]=='U' && pvr[1]=='L') ||
								(pvr[0]=='U' && pvr[1]=='S') ) 
			{
				explicitVR=1;
				elementLength &= 0xffff0000; 
				elementLength >>= 16;      
      }
		}
		if(((long)elementLength)==-1) 
			elementLength=0;
		if ((groupWord == 0x7fe0) & (elementWord == 0x0010)) 
		{
			int r,c;
			if (flip)
			{
				for (r = rows - 1; r >= 0; r--) 
				{
					for (c = 0; c < cols; c++)
					{
						read(fp,little_endian,value); // data shold be always in lttle endian
						HU_value = value;
						if(Signed!=1&&value < 0)
						{
							HU_value = sizeof(T)==1?(unsigned char)value:(unsigned short)value;
						}
						HU_value = HU_value * slope_value + intercept_value;
						IMAGE[cols*r+c] = (T) HU_value;
					}
				}
			}
			else
			{
				for (r = 0; r < rows; r++) 
				{
					for (c = 0; c < cols; c++)
					{
						read(fp,little_endian,value); // data shold be always in lttle endian
						HU_value = value;
						if(Signed!=1&&value < 0)
						{
							HU_value = sizeof(T)==1?(unsigned char)value:(unsigned short)value;
						}
						HU_value = HU_value * slope_value + intercept_value;
						IMAGE[cols*r+c] = (T) HU_value;
					}
				}
			}
			time_to_exit = true;
		}
		fseek(fp,elementLength,SEEK_CUR);
  } /* END  while (time_to_exit) */ 
	free((uint8 *)t);
	fclose(fp);
	return 0;
}
//----------------------------------------------------------------------------
int vtkDicomUnPacker::ReadImageInformation(vtkPackedImage *packed)
//----------------------------------------------------------------------------
{
	uint32 p;

	if (Flag)
	{
		return 0;
	}

	Flag=1;

	if (UnPackFromFile)
	{
		if (!FileName)
		{
			vtkErrorMacro("You must set a file name");
			return -1;
		}

		// Read image parameters from FILE
		if (read_dicom_header(RESULT,VALUES, &ImageSize, &TAGNumbers) < 0) 
			return -1;
	}
	else
	{
		if (packed->GetPackType()!=VTK_IMG_PACK_DICOM)
		{
			vtkErrorMacro("This class supports only DICOM images.");
			return -1;
		}
	}	

	m_BitsAllocated=16;
	m_BitsStored=16;
	m_DimX=512;
	m_DimY=512;
	m_HighBit=15;
	m_PixelRepresentation=0;
	m_SmallestImagePixelValue=0;
	m_LargestImagePixelValue=0;
	m_Slope=1;
	m_Intercept=0;
	m_Spacing[0]=2.0;
	m_Spacing[1]=2.0;
	m_Position[0]=0.0;
	m_Position[1]=0.0;
	m_Position[2]=0.0;
//	SliceLocation=0.0;
	m_Orientation[0]=0.0;
	m_Orientation[1]=0.0;
	m_Orientation[2]=0.0;
	strcpy(m_PhotometricInterpretation,"          ");

	for (p=0; p<TAGNumbers; p++)
  {
		if ((RESULT[p].Group==8) & (RESULT[p].Element==8)) 
    {
      int n = strlen(VALUES[p].stringa[0]);
      if (n<256)
      {
        strncpy(ImageType[0], (char *)&(VALUES[p].stringa[0]), n);
        ImageType[0][n] = '\0';
      }
      n = strlen(VALUES[p].stringa[1]);
      if (n<256)
      {
        strncpy(ImageType[1], (char *)&(VALUES[p].stringa[1]), n);
        ImageType[1][n] = '\0';
      }
      n = strlen(VALUES[p].stringa[2]);
      if (n<256)
      {
        strncpy(ImageType[2], (char *)&(VALUES[p].stringa[2]), n);
        ImageType[2][n] = '\0';
      }
    }
		if ((RESULT[p].Group==8) & (RESULT[p].Element==32)) 
			strncpy(StudyDate, (char *)&(VALUES[p].stringa[0]), 14);
		if ((RESULT[p].Group==8) & (RESULT[p].Element==96)) 
			strncpy(Modality, (char *)&(VALUES[p].stringa[0]), 2);
    if ((RESULT[p].Group==8) & (RESULT[p].Element==4158)) 
      strncpy(CTMode[1], (char *)&(VALUES[p].stringa[0]), 14);

		if ((RESULT[p].Group==16) & (RESULT[p].Element==16)) 
			strncpy(PatientName, (char *)&(VALUES[p].stringa[0]), 255);
		if ((RESULT[p].Group==16) & (RESULT[p].Element==32))
			strncpy(PatientID, (char *)&(VALUES[p].stringa[0]), 10);
    if ((RESULT[p].Group==16) & (RESULT[p].Element==48)) 
			strncpy(PatientBirthDate, (char *)&(VALUES[p].stringa[0]), 255);
		if ((RESULT[p].Group==16) & (RESULT[p].Element==64)) 
			strncpy(PatientSex, (char *)&(VALUES[p].stringa[0]), 1);
		
    if ((RESULT[p].Group==24) & (RESULT[p].Element==34)) 
      strncpy(CTMode[0], (char *)&(VALUES[p].stringa[0]), 14);
		
		//Modified by Matteo 03/07/06
		if ((RESULT[p].Group==24) & (RESULT[p].Element==4192)) 
			TriggerTime = (double) VALUES[p].num[0];
		if ((RESULT[p].Group==24) & (RESULT[p].Element==4240)) 
			CardiacNumberOfImages = (double) VALUES[p].num[0];
		if ((RESULT[p].Group==32) & (RESULT[p].Element==19)) 
			InstanceNumber = (int) VALUES[p].num[0];
		//End Matteo

    if ((RESULT[p].Group==32) & (RESULT[p].Element==13)) 
			strncpy(StudyUID, (char *)&(VALUES[p].stringa[0]), 255);
		if ((RESULT[p].Group==32) & (RESULT[p].Element==16)) 
			strncpy(Study, (char *)&(VALUES[p].stringa[0]), 79);
		if ((RESULT[p].Group==32) & (RESULT[p].Element==50)) 
		{
			m_Position[0]=(double) VALUES[p].num[0];
			m_Position[1]=(double) VALUES[p].num[1];
			m_Position[2]=(double) VALUES[p].num[2];
		}
		if ((RESULT[p].Group==32) & (RESULT[p].Element==4161)) 
		{
			//SliceLocation=(double) VALUES[p].num[0];
			m_Position[0]=0.0;
			m_Position[1]=0.0;
			//m_Position[2]=SliceLocation;
			m_Position[2] = VALUES[p].num[0];
		}
		if ((RESULT[p].Group==32) & (RESULT[p].Element==55))
		{	
			m_Orientation[0]=(double) VALUES[p].num[0];
			m_Orientation[1]=(double) VALUES[p].num[1];
			m_Orientation[2]=(double) VALUES[p].num[2];
		}

		if ((RESULT[p].Group==40) & (RESULT[p].Element==256)) 
      m_BitsAllocated=(int) VALUES[p].num[0];
		if ((RESULT[p].Group==40) & (RESULT[p].Element==257)) 
      m_BitsStored=(int) VALUES[p].num[0];  
		if ((RESULT[p].Group==40) & (RESULT[p].Element==258)) 
      m_HighBit=(int) VALUES[p].num[0];
		if ((RESULT[p].Group==40) & (RESULT[p].Element==259)) 
      m_PixelRepresentation=(int) VALUES[p].num[0];
		if ((RESULT[p].Group==40) & (RESULT[p].Element==262)) 
      m_SmallestImagePixelValue=(int) VALUES[p].num[0];
		if ((RESULT[p].Group==40) & (RESULT[p].Element==263)) 
      m_LargestImagePixelValue=(int) VALUES[p].num[0];
		if ((RESULT[p].Group==40) & (RESULT[p].Element==16)) 
      m_DimY=(int) VALUES[p].num[0];                       
		if ((RESULT[p].Group==40) & (RESULT[p].Element==17)) 
      m_DimX=(int) VALUES[p].num[0];
		if ((RESULT[p].Group==40) & (RESULT[p].Element==4178)) 
      m_Intercept=(int) VALUES[p].num[0];
		if ((RESULT[p].Group==40) & (RESULT[p].Element==4179)) 
      m_Slope= VALUES[p].num[0];
		if ((RESULT[p].Group==40) & (RESULT[p].Element==4)) 
      strcpy(m_PhotometricInterpretation, (char *)&(VALUES[p].stringa[0]));
		if ((RESULT[p].Group==40) & (RESULT[p].Element==48)) 
		{
			m_Spacing[0]=(double) VALUES[p].num[0];
			m_Spacing[1]=(double) VALUES[p].num[1];
		}
  }

/*	if (m_BitsStored != 16 && m_BitsStored != 8 && m_BitsStored != 12)
		{
			vtkErrorMacro("Supports only 8, 12, 16 bits DICOM images.");
			return -1;
		}*/
	if (m_BitsAllocated != 8 && m_BitsAllocated != 16)
		{
			vtkErrorMacro("Unsupported DICOM image.");
			return -1;
		}

	// Set the image parameters necessary for cache allocation
	SetDataExtent(0,m_DimX-1,0,m_DimY-1,0,0);
	switch (m_BitsAllocated)
	{
	case 8:
		SetDataScalarType(VTK_CHAR);
		break;
	case 16:
		SetDataScalarType(VTK_SHORT);
		break;
	}

	SetNumberOfScalarComponents(1); // Only gray scale CT images supported

	GetOutput()->SetSpacing(m_Spacing);
	GetOutput()->SetOrigin(m_Position);

	return 0;
}
//----------------------------------------------------------------------------
// This function does nothing is only an example of unpacking function
int vtkDicomUnPacker::vtkImageUnPackerUpdate(vtkPackedImage *packed, vtkImageData *data)
//----------------------------------------------------------------------------
{
	int ret;

	if (!Flag)
	{
		vtkGenericWarningMacro("DEBUG: ExecuteInformation not executed, cannot continue with decompression");
		return -1;
	}

	if (!UnPackFromFile)
	{
		// Check if the image is of the right type
		if (packed->GetPackType()!=VTK_IMG_PACK_DICOM)
		{
			vtkErrorMacro("This class supports only DICOM images.");
			return -1;
		}
	}

	int *ext=data->GetExtent();

	switch (m_BitsAllocated) 
	{
		case 8:
		{
			ret = read_dicom_string_image(FileName, (char *)data->GetScalarPointer(), m_Slope, m_Intercept, m_DimY, m_DimX,FlipImage);
		}
		break;
		case 16:
		{
			ret = read_dicom_string_image(FileName, ( short *)data->GetScalarPointer(), m_Slope, m_Intercept, m_DimY, m_DimX,FlipImage,m_PixelRepresentation);
		}
		break;
	}

	Flag=0;

	return ret;
}
//----------------------------------------------------------------------------
char *vtkDicomUnPacker::GetCTMode()
//----------------------------------------------------------------------------
{
	if(strcmp(Modality,"CT") == 0 || strcmp(Modality,"XA") == 0) 
		return CTMode[0];
	else
		return CTMode[1];
}
//----------------------------------------------------------------------------
char *vtkDicomUnPacker::GetImageType(int id_caracrteristic)
//----------------------------------------------------------------------------
{
  char *img_type = (id_caracrteristic >= 0 && id_caracrteristic < 3) ? ImageType[id_caracrteristic] : ImageType[0];
  return img_type;
}
//----------------------------------------------------------------------------
void vtkDicomUnPacker::GetSliceLocation(double pos[3])
//----------------------------------------------------------------------------
{
  pos[0] = m_Position[0];
  pos[1] = m_Position[1];
  pos[2] = m_Position[2];
}
