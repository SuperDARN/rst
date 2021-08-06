---
name: Deprecation 
about: Remove an option or a feature 

---

# !!!DEPRECATION!!!

!!! warning !!!** You are suggesting on removing a binary/library/function from RST

**Name:** 

**module:** *file_name.c*

**bin/lib:** */directory*

## Issue reference

**Issue:** *issue number*

## Reason

- *What is the reason for removing a bin/lib/function?*
- *Is the package a scope violation?*
- *Is it legacy code?* 
- *Is it under used and no longer supported?*

### Requirements Checklist

- [ ] tested with removal
- [ ] readthedocs documentation updated
- [ ] merge to `develop`
- [ ] Warning messages that the tool is removed

## Warning code

- *Please provide an example of **code** to show how the deprecation will be notified to the user.*

- Please provide **expected output** of the code provided

## Extra notes

*Please provide any other information on this PR here.*

Example info: 
- Is there another package supporting this functionality?
- Does it require a change in the `make.build; make.code` and how?
- Are there **performance hits/improvements**?
- Foes this affect the workflow of previous releases using other functions?
- If yes to above what can a user do to fix future issues from the deprication? 
