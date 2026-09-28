# Version Control Systems
## Introduction:
Version control systems are tools used by developers to track changes to files, allowing multiple devs to collaborate on large projects  
Git is a distributed version control system used by developers to track changes in source code over time. Instead of storing entire copies of files for each version, Git stores content as snapshots of your project—efficiently saving only what changed.
## Problem Statement(s):
In this task, you should implement a rudimentary git client which realizes the functionality of the following git commands:
1. git init: Initialize an empty git repository in the directory. This includes creating a directory similar to .git which holds metadata about the directory. 
2. git add: Add files to the staging area. This involves computing hashes (eg. using SHA256) of file contents and mapping them to file names, and marking them as staged.
3. git commit: Save the current state of the repository. Your git client should create the following objects:
4. Tree object - Stores hierarchical data about the state of the directory
Commit object - Stores a reference to the tree object, the commit message, author and information about the parent commit
5. [Bonus] git log: display commit history
6. [Bonus] git status: compare working directory with the last commit
7. [Major Major bonus]: git branch/checkout etc: functionality of having multiple branches.


## Resources:
- git-scm book: https://git-scm.com/book/en/v2 (chapter 10 is especially helpful for this task)
- Write yourself a git
- A visual guide to git internals
- Atlassian

## Submission:
Create a private GitHub repository and add mentors as collaborators.
Attach a README file explaining how to run the client and details of all commands implemented.
Add a screen recording of all the features implemented.

## Mentor name and contact details:
1. `Shanjiv A` (+91 97313 62003, Github ID: shanjiv177)
2. `Ranjit Tanneru` (+91 81239 99357, Github ID: AmissDrake)