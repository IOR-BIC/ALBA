Howto use QA (rev_01)

1) download qa.zip
2) insert in you cmake add_subdirectory(<qa dir>)
3) check NEEDED path in mafPath.py.conf, replacing the subdirectories with the correct ones.
4) launch cmake and the scripts will be copied inside build directory
5) when qa is copied, run in order:
   - python GeneratorQA.py
   - python ScriptLauncher.py
   - python Formatter.py

5) in QAResults will be create and formatted the output as a html site.
