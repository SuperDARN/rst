---
name: Bug Fix
about: Fixing a known bug

---

# Bug Fix 
**Name:** 

**Issue Reference:**


## Modified

**Function Name:** *filename.c* 

**bin/lib:** */directory_name*

*Please provide any other information on what is modified to fix this bug*

## Scope
- Which bug does this address?
- *use point form or check boxs*
- What does this bug fix not address? 

### Requirements Checklist
- [ ] tested
- [ ] Update code documentation (if applicable)
- [ ] Update readthedocs documentation (if applicable)
- [ ] merge to `develop`

### Urgent Patch Release
*If applicable please reason why this bug fix should be an urgent patch release?*
*Examples:*
- fixes the code from breaking with main use
- fixes inaccuracies in how the data is analyzed  

## Test code

### Code example with the bug

**Branch used:** `branch_name`

- *Please provide **code** and **output** with the bug*

### Code example with the fix

**Branch used:** `branch name`

- *Please provide **code** and **output** with the fix*

### Where to obtain test files

*Please provide instructions if applicable on how to access files needed to test the code.*

## Extra notes
*Please provide any other information on this PR here.*

Example info: 
- does it use metadata?
- does it require a change in the `make.build/make.code`?
- are there performance hits/improvements?
- are there problems you cannot fix?
- compiler version? `gcc --version`
- operating system?
