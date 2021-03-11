<!--Copyright (C) 2020 SuperDARN Canada, University of Saskatchewan 
Author(s): Marina Schmidt 

Modifications:

Disclaimer:
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
-->


## Branching 

In git, "branches" are used to make changes to the code without affecting the main codebase or the work of other developers. Once the changes on a new branch have been tested in a "pull request", the new branch is merged into the main codebase. 

## Getting Started 

!!! Note
    Before you start writing new code, please create a new issue to describe what you are planning to do, and "assign" the issue to yourself. This lets other developers know what you are working on. See [issues](issues.md)

1. Clone the RST repo: 

        git clone git@github.com:SuperDARN/rst.git

    It is recommended by GitHub to create a [SSH key](https://docs.github.com/en/github/authenticating-to-github/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent)

2. Change to the `rst` folder
        
        cd rst

* Update the code
      
        git fetch
        git pull origin master

* Decide what branch to break off from:
    * HOTFIX: a fix that needs to be in master ASAP then branch from `master`
    * Documentation: existing  main documentation with an update then branch from `master`
    * New Documentation: documentation that doesn't exist in the main documentation then branch from `develop`
    * New code/fix that can wait for a release then branch from `develop`
    * Code based on another branch then branch from that branch name
  
        git checkout <brach name>

* Decide on the new branch name. It is recommended to use the following Prefixes:  
    * HOTFIX/ : a bug that needs to be fixed ASAP and pushed to `master`
    * FIX/ : a bug fix that can wait to be released 
    * EHN/ : an enhancement or new feature to the `develop` code
    * DOC/ : new or updating existing documentation 
    * DEP/ : deprecating code from the codebase
   
        git checkout -b <prefix/><branch name>

* Now you have created your own branch locally. Make the modifications to the code on this branch, and then run the following commands to commit the changes:
    
        git add <file changed>
        git commit -m <brief description of the change>

* Now "push" the changes to GitHub:

        git push origin <branch name>

* Repeat the above commands above as you work on the code changes 
* Once you are completed, documented, and tested your code then you can create a pull request, see [pull request](pull_request.md)
