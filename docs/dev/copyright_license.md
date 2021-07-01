<!---
(C) copyright 2019 SuperDARN Canada, University of Saskatchewan 
(C) copyright 2021 The University Centre in Svalbard (UNIS)

authors: Marina Schmidt, SuperDARN Canada
         Emma Bland, UNIS

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
       
Modifications:
          Emma Bland, UNIS, 2021-02-12 : added background information about GPL, why copyright information is required, FAQ, and RST license history 

-->


# Licensing of the RST

The SuperDARN Radar Software Toolkit (RST) is licensed under the [GNU General Public License (GPL) v3.0](https://www.gnu.org/licenses/gpl-3.0.en.html). This license guarantees that end users have the freedom to use, modify and share the software. The *Free Software Foundation*, which published the GPL, explains that:

> *"The licenses for most software and other practical works are designed to take away your freedom to share and change the works. By contrast, the GNU General Public License is intended to guarantee your freedom to share and change all versions of a program--to make sure it remains free software for all its users."*


> *"Developers that use the GNU GPL protect your rights with two steps: (1) __assert copyright__ on the software, and (2) __offer you this License__ giving you legal permission to copy, distribute and/or modify it."*

This license ensures that the scientific community (and everyone else) is legally permitted to use, copy, modify and redistribute the RST software package. In this way, the software package can be maintained and improved over time to support the advancement of science. 


## License requirements

The GPL v3.0 License states that:

- The RST source code must be distributed with the software
- License and copyright information must be included at the top of each source code file (more info on copyright below)
- If the code has been modified, there must be a notice stating that it has been modified, by who, and when
- Modifications must be released under the same license as RST (GPL v3.0 or later)


### License notices

The software must include the following notices: 

``` C
/*
<one line to give the program's name and a brief idea of what it does.>
Copyright (C) <year>  <name of author>

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

Modifications:
          <first and last name>, <institution> <year-month-day of the modification> : <comment on the change (optional)> 
*/
```

According to the [GPL license documentation](https://www.gnu.org/licenses/gpl-3.0.en.html): 

> *"It is safest to attach them to the start of each source file to most effectively state the exclusion of warranty; and each file should have at least the “copyright” line and a pointer to where the full notice is found.*"

## License permissions

When developing in RST you are granting permission for your code to be licensed under the GPL. This will be ok in almost all situations. Exceptions may arise if your employer wants to make your program into proprietary software, or if your funding agency has restrictions on the publication of research outputs (e.g. defense contracts). If you suspect that you won't be allowed to contribute code to RST under the GPL, [it is recommended](https://www.gnu.org/licenses/gpl-faq.html#WhatIfSchool) that you negotiate this with your employer/funding agency at an early stage in developing the software. 

!!! IMPORTANT
    Please make sure to review the [license](https://www.gnu.org/licenses/gpl-3.0.en.html), and check with your employer/funding agency that you have permission to distribute your code under the GPLv3. 

## Copyright

The GPL license requires that all code includes a copyright notice. The purpose of the copyright notice is to __protect the rights of developers__ by:

  1. Giving them (and their employer) credit for their work
  2. Preventing their code from being distributed under a different license without their knowledge/permission
  3. Giving them the power to enforce the terms of the license<br>
     *e.g. take someone to court if they violate the license*

### How to copyright your code

You should add copyright information if you have written new code from scratch or substantially modified someone else's code. Examples of substantial modifications to existing code include significant changes to the code's structure or functionality. This is a gray area, so use your best judgment and ask other developers at the pull request stage if you are unsure. 

!!! IMPORTANT
    Ask your employer about what to write in your copyright line. If you contribute to the RST in your free time, then just put your name in the copyright line.


An example copyright line structure is:

``` C
/*
(C) copyright <year> of <institution/entity>

author: <first and last name>, <institution>
/
```

Since RST is developed collaboratively by the SuperDARN community, it may be appropriate to have multiple copyright lines in a single source file, for example: 
``` C
/*
(C) copyright <year> of <institution/entity>
(C) copyright <year> of <another institution/entry>

authors: <first and last name>, <institution>
         <first and last name>, <institution>

Modifications: 
           <first and last name>, <institution> <year-month-day of the modification> : <comment on the change (optional)> 
*/
```

!!! IMPORTANT
    Never remove the existing copyright information from a source file.

#### Minor modifications

Adding copyright notices is appropriate only for __substantial__ modifications. If you make minor modifications to someone else's code (e.g. bug fixes), then you should document this in the modification history (example above), but do not add a new copyright line. 

## Frequently asked questions

__Is it possible to restrict RST to non-commercial use only?__<br/>
No. The license prohibits us from placing any additional restrictions on the use of the software. This is the key principle of free software -- "free" refers to "freedom" (not price), so the software must be available to anyone to use for any purpose. Adding a "non-commercial use only" restriction would require us to release RST under a different license (possibly a non-standard license), which would be very difficult in practice (see next question). Also, any change to the license would apply only to new releases; we cannot prevent commercial use of the previous versions of RST that are already licensed under GPL. 


__Can RST be released with a different license?__<br/>
To release RST with a different license, we would need permission from all of the copyright holders. This means that, in practice, it would be very difficult to re-license the software. This is the intention of the GPL--it ensures that the software can always be used freely. 


__Can JHU/APL revoke the GPL license and take back control of RST?__<br/>
No. JHU/APL gave permission for the RST to be re-licensed under the GPL in ~2012 (RST3.5), and they cannot revoke this permission. JHU/APL still owns the copyright to their code, and RST versions 3.4 and earlier are still licensed by JHU/APL (not under GPL).


__What happens if I don't include copyright information in my code?__<br/>
In most countries, authors automatically hold the copyright to their work even if they don't add a copyright notice. This is to protect the rights of people who are not aware of the law. However, omitting copyright information means that:

- You may be breaching your employment contract
- You are not complying with the GPL requirement to include copyright information
- You may not get credit for your work
- It is more difficult to prove who is the copyright holder


__Do I have to include my institution in the copyright line?__<br/>
That depends on the terms of your employment. Check your employment contract or ask the research office. If you contribute to the RST software package in your free time, then just put your name in the copyright line.


__Can I copyright code to a generic "SuperDARN" organization?__<br/>
No, the copyright holder has to be a legal entity or a person. Your employer may also object to this. If you contribute to the RST software package in your free time, then just put your name in the copyright line.


__Why does every RST source file have license notices at the top? Isn't it sufficient to include the license file in the top-level directory of RST?__<br/>
Since the GPL allows users to modify and redistribute portions of the RST software package, individual source files might become separated from the license file. If this happens, it will be unclear to users what their legal rights are to use/modify/distribute that version of the code (which is a violation of the GPL). Therefore, all source files should clearly indicate that they are licensed under the GPL (writing "see license.txt" is not sufficient).


__If I add a new library to the RST, can that library have a different license?__<br/>
If the new library was originally developed outside of RST, the author of that library can choose to license it under either GPL or LGPL when adding it to the RST. Code that has been developed within RST must be licensed under GPL.


__I've added code to the RST. Can I also release my code under a different license?__<br/>
Yes, provided that you are the copyright holder of the code and that it is a standalone library (developed outside of RST), you are free to license it under different non-exclusive licenses ([more info](https://www.gnu.org/licenses/gpl-faq.html#ReleaseUnderGPLAndNF)). Remember that:

- Once your code is added to the RST, that version of the code is licensed under GPLv3 (or LGPL - see the previous question), and you cannot revoke this
- If your software includes other code from the RST (or other GPL-licensed code), your software can only be licensed under GPL. 


__Am I allowed to copy code from the RST into my own software project? What do I have to do to comply with RST's license?__<br/>

- You are free to copy or modify any portion of the RST code
- You are free to share the software outputs (data files, plots)
- If you choose to share the binaries for your new software: 
    - You must also share the source code
    - Your software must be licensed under GPL (v3 or later)


__Who owns the outputs of RST (processed data files, plots)?__<br/>
RST outputs belong to the end-user who created them. End-users are free to use and share these outputs, provided that they comply with the SuperDARN data policy. This includes respecting embargoes on Discretionary Time and Special Time data, and providing appropriate acknowledgement in publications. The data policy can be found in the [SuperDARN Principle Investigators' Agreement](https://superdarn.ca/piagreement).


__I have more questions about the GPL__<br/>
There's lots of helpful information [here](https://www.gnu.org/licenses/gpl-faq.html).


## History of RST licensing

RST was originally developed at the Johns Hopkins University/Applied Physics Laboratory (JHU/APL). Around 2012, JHU/APL granted permission for RST to be re-licensed so that the SuperDARN community could continue maintaining the software collaboratively. This process has caused confusion over the years, since even scientists who are expert programmers may not be familiar with the intricacies of software licensing and copyright.

A major source of confusion around the RST license is whether it was intended to be licensed under the [GNU General Public License](https://www.gnu.org/licenses/gpl-3.0.en.html) (GPL), or the [GNU __Lesser__ General Public License](https://www.gnu.org/licenses/lgpl-3.0.en.html) (LGPL). This confusion has arisen because, at some point, the LGPL license notice was attached to most of the RST source code. The `AstAlg` library is the exception to this, which has been [clearly marked](https://github.com/SuperDARN/rst-archive/blob/rst.3.1/codebase/analysis/src.lib/astalg/astalg.1.2/LICENSE.txt) with a GPL disclaimer since it was first added to the RST in v3.1. Since `AstAlg` is clearly licensed under the GPL, it follows that the whole of the RST should also be licensed under the GPL. It is possible that the LGPL disclaimer text was added to the remaining RST source code in error since it is very similar to the GPL disclaimer text. To add to this confusion, RST was not distributed with any license file for several releases (v3.5 to v4.3 inclusive). The GPL license file was added in [v4.3.1](https://doi.org/10.5281/zenodo.3634732). The license information across the whole package was corrected in RST4.6.
