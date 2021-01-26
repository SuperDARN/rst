# Branching 

In git, you can create branches which will not affect the main code allowing you to change code/documentation without breaking others code.

# Getting Started 

Before creating a branch it is a good habit to create an [issue](issue.md) to ensure others are aware you are working on something and make sure to assign yourself to the issue. 

1. clone the RST repository:
    
    `git clone git@github.com:SuperDARN/rst.git`

    !!! Warning  
        GitHub is moving to using [SSH keys](https://docs.github.com/en/github/authenticating-to-github/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent) for clone and committing code. Please ensure this is setup.  

2. Ensure you are up to date:
    
    ``` bash
    git fetch
    git pull origin master
    ```

3. Determine what you will be branching off:

      - Is it a HOTFIX: fixing a major bug or updating on currently released documentation?
        Then branch from `master`

            `git checkout master`

      - Is it something that can wait for a release?
        Then branch from `develop`

            `git checkout develop`
      - Is it something branching on existing changed code?
        Then branch from the specific branch name 
            `git checkout <branch_name>`

4. Create a new branch: 
    
      `git checkout -b <branch name>`
  It is encourage to use the following prefixes with an appropriate name following:
      - FIX/ : Indicating a fix to a bug
      - EHN/ : Enhancement or New feature  
      - DOC/ : Documentation 

Once you create a branch it will not appear on GitHub until you push the code. To do this make or change a file as an initial commit:

```bash
git add <filename>
git commit -m <message about what was done>
git push origin <branch name>
```

Then the branch will appear on GitHub. Continue using the above code to keep updating your code until it is documented, complete, and tested.  

Once this is done, see [pull requests](pull_requests.md)
