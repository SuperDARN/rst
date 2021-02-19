<!--Copyright (C) 2020 SuperDARN Canada, University of Saskatchewan 
Author(s): Marina Schmidt 
Modifications:

Disclaimer:
-->

# Code Reviews 

All new code is tested for functionality and integration with RST before it is merged into the main codebase. In addition to this testing, a *code review* may be useful, especially when new features or major changes are being proposed. Reviewing code is like copyediting a paper before publication. It helps to ensure that: 

- the code is understandable to users
- the code is consistent and structured appropriately
- the algorithms and software function as intended

Reviewing code encourages collaboration, and it is an excellent opportunity to improve your own programming skills and knowledge of the software. Anyone can provide a code review, even if they are not familiar with the native language, which for RST is C and IDL.


If you are more advanced in the native programming language or science being implemented in the code, a short code review to help find any mistakes or bugs in the code is more than welcome. 
Code reviews can easily become tedious and time consuming, especially for new features and major changes. Therefore, consider focusing on one portion of the code when providing a review. In general, code reviews should not take more than an hour. 

## How to start a Code Review 

1. Pick a Pull Request to code review, which can be found [here](https://github.com/SuperDARN/rst/pulls)
2. Click on `files changed` at the top of the PR conversation below the title on the right side 
    
    !!! Note 
        Please read the previous comments on the PR before starting a code review. If something has already been mentioned in the conversation, then you can leave it alone or give a "thumbs up :+1" to the comment to show support. 

3. Review the changes in this PR. To prevent `scope creep`, avoid commenting on other lines that have not been modified in the PR (open a new issue instead).
4. To comment on a single line of code, hover the cursor over the line numbers on the left side and click the **+** button that appears. To comment on several consecutive lines, hold the **+** button and drag the cursor down to select the relevant lines.
5. Write any feedback or questions in the box. You can also suggest concrete changes to a line of code using the [suggestion](https://haacked.com/archive/2019/06/03/suggested-changes/) syntax

6. Click `Start a review` on the bottom right in the pop up window. 
    
    !!! Warning
        Making several single comments causes a lot of email notifications, so please use the `Start a review` option

* Once you are done, click on `Finish your review` or `Review changes` (if no comments made), and provide some general feedback in the text box. This should include what type of review you did and if you will be continuing your review at a later time/date.  Some examples include:
    - Testing and code review to find a bug 
    - Code review on style/formatting 
    - Partial code review 
* Select:  
    - Comment: general comments to be fixed up but nothing major
      
        !!! Note
            If you did a partial review please select comment. Approve is only done for complete reviews or complete testing 

    - Approve: everything looks good no changes needed
    
        !!! Note
            This should only be done on complete code reviews. You can also revert your approval after it is made if you find something else later. 
        Do this by going down to the merge button and find your name under approvals and click the three dots and select `re-review` 
  
    - Request Changes: major changes to make
      
        !!! Warning
            `Request changes` prevents others from merging the code. Generally you will need to approve the changes before the code can be merged. If you cannot do this then do not use Request changes.

* Submit! 

!!! Note
    To make minor changes directly in the code like typos and grammar changes, you can edit the code directly on that branch (either on Github or on your [own machine](testing.md)), and then commit and push the changes to the branch.  The Developer can then review the changes in your commit.

### What to look for?

Here is a list of what to look for 

- Is the copyright and license disclaimer in the file?
- Did they add the modification line if they made updates?
- Are there doc strings for the class and for functions?
- Are there confusing variables, functions, and/or class names? 
- Are there defaults and do they make sense for the function? 
- Is the code too complex? Could it be simpler? 
- Do the error messages/exceptions make sense and are helpful?
- Could there be more comments on certain sections?
- Are hard coded values documented? 
- Are algorithms or mathematical equations cited to publication, webpage, or book? 
- Does the nomenclature make sense? 
- Are there any cases they may need to reconsider? 
- Would comments or documentation make sense to the general user?

## How to speed up code reviews

Here are some tips and tricks:

- Dedicate a certain amount of time and just review what you can in that time period 
- Scan for missing doc strings and obvious mistakes 
- Focus on what has been changed 
- If an issue repeats then make a comment for them to fix the rest and add in the general feedback when submitting the comments 
- Do not get too pedantic!
- Break up reviews into smaller parts that each focus on a few files or functions (and then comment that you have done a partial review)

## Reviewing a review 
  
As a developer you will need to address any comments that come up from code reviews like you would with reviewers comments in a paper submission. 
Comments should addressed and resolved before merging the code. 

If you do step 2 and look at the comments in `files changed` view you can address multiple comments in one batch. 

### Batch Committing of Suggestions 

First look for all the suggestion comments and if you agree then add to `batch suggestions`, once you have gone through all them 
then at the top in the middle there is `commit suggestions` and this commit all the suggestions for you. 

### Responding to Comments 

If you don't agree with a suggestion, try to find a compromise or acknowledge their effort and explain why you prefer your style.
Remember to hit `Start a review` to prevent multiple email notifications. 

If you made the change requested, then select `resolve conversation` so they know you addressed their comment.

If you are answering a question or responding to some feedback, remember to acknowledge their time and effort, then address it in collaborative manner. 
Remember to hit `Start a review` to prevent multiple email notifications. 

Once all comments are addressed then follow steps 8-9 in the `How to start a Code Review` section. 

## What If You Cannot Comment on a Specific Line of Code?

If you cannot comment on a line of code that was not changed in the PR but affects the overall scope of the PR then leave a comment in the conversation box. If you want to highlight the line(s) you are referencing, open another tab of the repository:

1. Go to the code by clicking on the directories in main body of the GitHub repo page. If its specific to a branch make sure to switch branches at the top left corner. 
2. Click the file you are look at, then go to the line you want to reference
3. Click on the line (hold shift and click on the last line to select multiple)
4. Click on the `...` box that appeared left to the line number 
5. Select *copy permalink*

If this line is not within scope of the PR then select *Reference in new issue* to create an issue around the problem. 

If you find multiple issues in your review that are unable to be in a code review, use [GitHub's checklist](https://docs.github.com/en/github/managing-your-work-on-github/about-task-lists) syntax to make a list for the developer to keep track of the issues to fix. 


## General Guidelines

Here are some general guidelines to follow when code reviewing or responding to one: 

- Dedicate time to review in a given period, avoid being distracted 
- Phrase comments as questions as much as possible
- Remember to comment on the code 
- Acknowledge time spent and say "Thank you" 
- Suggest solutions or help
- Avoid aggressive terminology 
- Avoid reviewing in a bad mood or feeling pressured/rush 
- Review all comments to make sure it sounds helpful and not demeaning 
- Avoid pedantic points, focus on what is needed and not personal preferences 
- Any amount of reviewing small or large is welcome! 
- Approve! Make sure to comment what type of approval you are giving, complete testing or a complete reviewing of the code. This will help others know what needs to be done to get the PR closer to `merge` 

!!! Note
    If you approve the PR and you are the second to approve, or there are already 2 approvals and all concerns are addressed 
    with one approval on testing and one approval on code review, please merge the PR! An exception to this is if it is a release PR or if more than two approvals are requested in the PR instructions. 

!!! Note
    Do not be scared to `approve` or `merge` code as it can be reverted quite easily if it needs to be. Also, reach out and ask if you are unsure! 
