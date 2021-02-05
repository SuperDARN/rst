<!--Copyright (C) 2021 SuperDARN Canada, University of Saskatchewan 
Author(s): Marina Schmidt 
Modifications:

Disclaimer:
-->

# Pull Requests 

Pull Requests in GitHub provide an interface for others to review and test your code. Testing pull releases is a good way to start contributing to the RST.


## Code Check List

Before submitting a PR, please check the following:

- copyright and disclaimer, or modification line is added
- the code compiles and runs with no additional warnings
- the new code has been tested on your own machine
- user documentation has been included/updated
- the code is on its own *git* branch
- all code is pushed to the remote branch on Github

## Creating the PR 

To create a PR, navigate to [RST's Pull Requests](https://github.com/SuperDARN/rst/pulls)
and click on `New pull request`. If templates are available, select the one that is most appropriate.

### Merging Direction 

In general, new code should be merged to `develop`. Possible exceptions to this rule are:

- if a new feature/update will be added in several stages, merge to an intermediate branch (`git checkout -b` from the intermediate branch)
- merge to `master` **ONLY** if it is a **HOTFIX** or **RELEASE** branch. 

    !!! Note
        **HOTFIX** is a substantial fix for a bug that prevents users from installing or using RST or gives inaccurate data. Documentation fixes can 
        also be seen as a **HOTFIX** as they do not require a new release or DOI.

- Small changes to documentation can be merged to `master` (e.g. fixing typos)

### Writing a PR

Follow the template to fill out the required sections. If there is no template, please provide the following information: 

- Informative title 
- Description of the changes you made 
- What people should focus on when testing/reviewing the code
- How to install the code if this has changed
- Code fragments showing how to test the code and the expected output/plots
- Any extra details that might be useful during testing

### Extra Tidbits

Pull requests on GitHub provide several additional features to help you get a useful code review:

- Request a review from a specific person
- Add labels to categorize the PR (e.g. bugfix, enhancement)
- Indicate that the PR is part of a project, if applicable
- Assign a milestone, if applicable
- Link the PR to an issue, if applicable

### Draft Pull Requests

Draft Pull Requests are a way to tell reviewers that your code is not ready to be `merged` yet. This might be useful in the following situations:

- The draft PR is dependent on another PR being merged or tested first
- The code has a major bug and you would like some assistance to fix it
- The code needs further work, but you would like some feedback on what you have already done

## Guidelines

Here are some general guidelines to follow with Pull Requests: 

- Do not merge your own code!
  - If at least 2 people have "approved" the code, anyone (including the code author) may merge it
- Make sure your code is complete before creating a PR (to reduce additional commit emails)
- Make sure your code is tested on various operating systems if needed
- Optional code is reviewed in a Code Review
