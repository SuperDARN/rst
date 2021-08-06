<!--Copyright (C) 2020 SuperDARN Canada, University of Saskatchewan 
Author(s): Marina Schmidt 
Modifications:

Disclaimer:
-->


# Testing Pull Requests 

One way to help RST get developed faster and smoother is by testing [Pull Requests](https://github.com/SuperDARN/rst/pulls). 
This can take fifteen minutes or less to do, helps the developer merge their code,
and allows you to be on the author list!

Every contribution helps! 

## Setting up the testing environment

1. Clone the [RST repository](https://github.com/SuperDARN/rst.git) by typing 
`git clone https://github.com/SuperDARN/rst.git` into the terminal or command line.

2. Then "checkout" the branch you need to test. The author of the Pull Request should give you some information on how to test the code, and if you need to checkout other branches to compare results. To checkout the branch you want to test:

        git fetch 
        git checkout <branch name>
        git pull origin <branch name>
    These lines *fetch* metadata on any new branches made, *checkout* the branch you want to test, and then *pull* changes to that branch to make sure you have the latest version.
  
    !!! Note 
        If you cannot determine the name of the branch you should be testing, look at the top of the page below the Pull Request title. Here you will see the branch name and also the "base branch" that the changes  will be merged to.

3. Build and compile the code using the normal RST [installation](../user_guide/linux_install.md) procedure.
      
        make.build && make.code

5. Follow any testing instructions provided by the Pull Request author, and any examples in the documentation. Where relevant, make up your own examples to check that the code works in other common situations (e.g. both new- and old-format files)
Report anything in the Pull Request comments about how you tested the code, what your output is, and any feedback or other information.
Note: To suggest changes to and/or comment on certain sections of code, navigate to the Files Changed tab where it is possible to select multiple lines of code by clicking and dragging the '+' symbol next to line of code.
6. Once you are satisfied with the Pull Request, **approve** it by clicking on the top `Files changed` then clicking Review changes and select *Approve*. Make sure to comment what you did in testing for your approval. 

!!! Note
    If the Pull Request has more than two approvals (one for code review and another for testing) with no comments to respond to, then please merge the code. Merge button is at the bottom of the Pull Request page. 
    
!!! Note
    If you identify a new bug during testing that is not related to the current Pull Request, report this bug by opening a new Issue.
