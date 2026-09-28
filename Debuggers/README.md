# Debuggers
## Introduction:
A debugger is a tool that controls another program’s execution, allowing you to pause it, inspect its memory, and step through instructions.  
Modern debuggers (like gdb) rely on low-level system calls to perform actions like start or attach to a process, intercept signals and traps, read/write registers and memory, and modify execution flow.

## Problem Statement(s):
In this task, the objective is to build a debugger from scratch by using the ptrace system call.
The debugger should have the following basic functionality:
- run/attach - start a program under debugger control.
- step - step to next instruction
- Create/delete breakpoints at memory address
- Print contents of memory/registers
- [Bonus] Debug symbols/variables, breakpoints at symbols
- [Bonus] Implement backtrace (print info about the call stack)

## Resources:
1. https://werat.dev/blog/what-a-good-debugger-can-do/
2. https://tartanllama.xyz/posts/writing-a-linux-debugger/setup/
3. dotGo 2017 - Liz Rice - Debuggers from scratch

## Submission:
Create a private Github repository and add mentors as collaborators.
Provide a readme detailing features implemented, and how to run the debugger.  
Add screenshots/recordings of the features
## Mentor name and contact details:
1. `Adithya A` (+91 9845543390, github: Adithya1435)  
2. `Ranjit Tanneru` (+91 81239 99357, GitHub: AmissDrake)