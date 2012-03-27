#region Using Directives
using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Collections;
using System.Xml;
using System.Xml.Xsl;
#endregion

namespace FileNameConvention
{
  class Program
  {
    static string[] dictionary = null;
    static string[] externalInherit = null;

    static void Main(string[] args)
    {
      try
      {
        CalculatePercentage(args[0]);
      }
      catch (Exception ex)
      {
        Console.WriteLine(ex.Message);
        Console.WriteLine("Usage: ");
        Console.WriteLine("ProgramName.exe ResultDirectoryName");
      }
    }

    /// <summary>
    /// Calculate percentage of well written file names 
    /// according to the coding standard rules.
    /// </summary>
    /// <param name="resultDirXML">Output directory</param>
    private static void CalculatePercentage(string resultDirXML)
    {
      string currDir = Directory.GetCurrentDirectory();
      string dictionaryFile = currDir + "\\" + "FileNamingDictionary.txt";
      string dirToScanFile = currDir + "\\" + "DirToScan.txt";
      string reportFile = resultDirXML + "\\" + "FileNamingReport.xml";
      bool matchDict = false;
      try
      {
        // Read file related t the list of directory to scan for file naming convention
        StreamReader srDirToScan = new StreamReader(dirToScanFile);
        string dir = srDirToScan.ReadLine();
        while (!srDirToScan.EndOfStream)
        {
          dir += " ";
          dir += srDirToScan.ReadLine();
        }
        string[] dirList = dir.Split(' ');
        srDirToScan.Close();

        // Read dictionary file
        StreamReader srDic = new StreamReader(dictionaryFile);
        string dic = srDic.ReadLine();
        while (!srDic.EndOfStream)
        {
          dic += " ";
          dic += srDic.ReadLine();
        }
        string[] dictionaryAndInherit = dic.Split(' ');
        srDic.Close();
        String sep = "<--ExternalInherit-->";
        int separatorIdx = Array.IndexOf(dictionaryAndInherit, sep);
        if (separatorIdx > 0)
        {
          dictionary = (string[])Array.CreateInstance(typeof(String), separatorIdx);
          externalInherit = (string[])Array.CreateInstance(typeof(String), dictionaryAndInherit.Length - separatorIdx - 1);
          Array.Copy(dictionaryAndInherit, dictionary, separatorIdx);
          Array.Copy(dictionaryAndInherit, separatorIdx + 1, externalInherit, 0, externalInherit.Length);
        }
        else if (separatorIdx == 0)
        {
          throw new Exception("No dictionary elements are presents.");
        }
        else
        {
          // no default inherited classes are defined!!
          dictionary = (string[])dictionaryAndInherit.Clone();
        }

        QueryPerfCounter myTimer = new QueryPerfCounter();
        myTimer.Start();

        // Write report file
        XmlTextWriter xwriter = new XmlTextWriter(reportFile, Encoding.UTF8);
        xwriter.WriteStartDocument(true);
        xwriter.WriteStartElement("root");
        xwriter.WriteElementString("ReportDate", DateTime.Now.ToString());

        string nodeDir;
        foreach (string d in dirList)
        {
          nodeDir = d.Substring(d.LastIndexOf('\\')+1);
          xwriter.WriteStartElement("Directory");
          xwriter.WriteAttributeString("DirName", nodeDir);
          string absoluteDir = currDir + "\\" + d;
          string[] filesHeader = Directory.GetFiles(absoluteDir, "*.h");
          string[] filesHeader_in = Directory.GetFiles(absoluteDir, "*.h.in");
          string[] filesImplem_cpp = Directory.GetFiles(absoluteDir, "*.cpp");
          string[] filesImplem_cxx = Directory.GetFiles(absoluteDir, "*.cxx");
          string[] files = (string[])Array.CreateInstance(typeof(String), filesHeader.Length + filesHeader_in.Length + filesImplem_cpp.Length + filesImplem_cxx.Length);
          filesHeader.CopyTo(files, 0);
          filesHeader_in.CopyTo(files, filesHeader.Length);
          filesImplem_cpp.CopyTo(files, filesHeader.Length + filesHeader_in.Length);
          filesImplem_cxx.CopyTo(files, filesHeader.Length + filesHeader_in.Length + filesImplem_cpp.Length);
          Array.Sort(files);
          int totalFiles = files.Length;
          int wellWrittenFiles = 0;
          string baseName;
          string wrongFiles = "";
          string goodFiles = "";
          foreach (string f in files)
          {
            bool wrong = true;
            baseName = f.Substring(f.LastIndexOf('\\') + 1);
            foreach (string k in dictionary)
            {
              string name = baseName.Substring(0, baseName.LastIndexOf('.'));
              if (name.Equals(k))
              {
                // This is a base class; don't check inheritance.
                wrong = false;
                wellWrittenFiles++;
                break;
              }
              matchDict = baseName.Contains(k);
              if (matchDict)
              {
                // classes inherited from base classes; 
                // check inheritance according to the second rule of the naming convention:
                // e.g. mafView -> mafViewVTK
                if (CheckInheritance(f, k))
                {
                  wrong = false;
                  wellWrittenFiles++;
                  goodFiles += " " + baseName;
                }
                break;
              }
            }
            if (wrong)
            {
              wrongFiles += " " + baseName;
            }
          }
          double percentage = (wellWrittenFiles / Convert.ToDouble(totalFiles));
          xwriter.WriteAttributeString("Percentage", percentage.ToString("P"));
          string[] wfilesList = wrongFiles.Split(' ');
          foreach (string f in wfilesList)
          {
            if (f == "")
            {
              continue;
            }
            xwriter.WriteStartElement("WrongFiles");
            xwriter.WriteAttributeString("FileName", f);
            xwriter.WriteEndElement();
          }
          string[] gfilesList = goodFiles.Split(' ');
		  foreach (string f in gfilesList)
          {
            if (f == "")
            {
              continue;
            }
            xwriter.WriteStartElement("GoodFiles");
            xwriter.WriteAttributeString("FileName", f);
            xwriter.WriteEndElement();
          }

          //End the nodeDir element.
          xwriter.WriteEndElement();
          Array.Clear(files, 0, files.Length);
        }
        myTimer.Stop();

        // Calculate time per iteration in seconds
        myTimer.SetMultiplier(1.0);
        double result = myTimer.Duration();
        xwriter.WriteElementString("ComputationTime", result.ToString("F") + " s");

        //End the "root" element.
        xwriter.WriteEndElement();

        //End the document
        xwriter.WriteEndDocument();
        xwriter.Close();

        // Load the style sheet.
        XslCompiledTransform xslt = new XslCompiledTransform();
        xslt.Load("resultTransform.xslt");

        // Execute the transform and output the results to a file.
        string reportHTMLFile = resultDirXML + "\\" + "..\\HTML\\FileNamingReport.html";
        xslt.Transform(reportFile, reportHTMLFile);

        XslCompiledTransform xsltWrongFiles = new XslCompiledTransform();
        xsltWrongFiles.Load("wrongFileList.xslt");

        string dirResultFile;
        XsltArgumentList xslArg = new XsltArgumentList();
        foreach (string d in dirList)
        {
          nodeDir = d.Substring(d.LastIndexOf('\\') + 1);
          dirResultFile = resultDirXML + "\\" + "..\\HTML\\" + nodeDir + ".html";
          xslArg.AddParam("dir", "", nodeDir);
          StreamWriter sw = new StreamWriter(dirResultFile);
          xsltWrongFiles.Transform(reportFile, xslArg, sw);
          sw.Close();
          xslArg.Clear();
        }
      }
      catch (Exception ex)
      {
        Console.WriteLine(ex.Message);
      }
    }

    /// <summary>
    /// Check if the file name match some patterns of the dictionary
    /// if the name starts with the last dictionary key's.
    /// </summary>
    /// <param name="baseName">Name of the file to check</param>
    /// <param name="dicKey">dictionary keyword matched</param>
    /// <returns>
    /// true if the class inherits from another class that match the same keyword 
    /// or from the base class, otherwise false is returned.
    /// </returns>
    private static bool CheckInheritance(string f, string dicKey)
    {
      string fileToOpen = f;
      if (fileToOpen.Contains(".cpp"))
      {
        fileToOpen = fileToOpen.Replace(".cpp", ".h");
      }
      if (fileToOpen.Contains(".cxx"))
      {
        fileToOpen = fileToOpen.Replace(".cxx", ".h");
      }
      if (!File.Exists(fileToOpen))
      {
        // Implementation file without .h!!
        return true;
      }
      try
      {
        StreamReader sr = new StreamReader(fileToOpen);
        string fileContent = sr.ReadToEnd();
        sr.Close();
        // Extract the name without extension to check the class inheritance.
        string baseName = fileToOpen.Substring(fileToOpen.LastIndexOf('\\') + 1);
        string name = baseName.Substring(0, baseName.LastIndexOf('.'));
        string wordToSearch;
        wordToSearch = ": public";
        int lineIdx = fileContent.IndexOf(wordToSearch);
        if (lineIdx < 0)
        {
          return true;
        }
        else
        {
          int startIdx = lineIdx + wordToSearch.Length;
          string parentClasses = fileContent.Substring(startIdx, fileContent.IndexOf('\r', lineIdx) - startIdx);
          parentClasses = parentClasses.TrimStart(' ');
          if (parentClasses.Contains(dicKey))
          {
            string libAcronym = dictionary[dictionary.Length - 1];
            if (dicKey == libAcronym)
            {
              // *********************** NOT IMPLEMENTED!! *************************
              // Generic MAF class: check that class name extend parentClasses name
              // *********************** NOT IMPLEMENTED!! *************************

              // dicKey is the library acronym.
              foreach (string s in dictionary)
              {
                if (parentClasses.Contains(s))
                {
                  if (s == libAcronym)
                  {
                    return true;
                  }
                  else
                  {
                    return false;
                  }
                }
              }
            }
            // Check if the class inherits from base class.
            return true;
          }
          else
          {
            // Check if the class inherits from external library.
            foreach (string k in externalInherit)
            {
              if (parentClasses.Contains(k))
              {
                return true;
              }
            }
          }
        }
      }
      catch (Exception ex)
      {
        Console.WriteLine(ex.Message);
      }
      return false;
    }
  }
}
