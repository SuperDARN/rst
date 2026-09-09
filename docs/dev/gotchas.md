<!--
This is a U.S. government work and not under copyright protection in the U.S.
author: Angeline G Burrell
-->

# Development Gotchas

The RST codebase has a long legacy, and it can be difficult to begin expanding
or maintaining.  Commonly enountered issues are recored here to help new
developers.

1. The library order in RST tool makefiles is important. Incorrect order may
   cause compilation failure on some operating systems and memory leaks or
   segmentation faults on other systems.  This can be difficult to debug if your
   system does not cause a compilation failure.
2. The validity of SuperDARN binary files may be tested simply using the
   `dmapdump` function.  It prints the contents of any SuperDARN binary file to
   standard out in plain text.
