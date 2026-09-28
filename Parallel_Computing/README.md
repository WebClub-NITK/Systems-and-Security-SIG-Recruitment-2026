# Parallel Computing

**Tags**:
[C](https://en.wikipedia.org/wiki/C_(programming_language)),
[Rust](https://rust-lang.org/),
[Multi-processing](https://en.wikipedia.org/wiki/Multiprocessing),
[Pthreads](https://en.wikipedia.org/wiki/Pthreads),
[Lock-Free Programming](https://preshing.com/20120612/an-introduction-to-lock-free-programming/),
[EGL](https://www.khronos.org/egl/),
[OpenGLES](https://www.khronos.org/opengles/),
[Termux](https://termux.dev/en/)

---

### Introduction

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Parallel programming is a paradigm of computer processing in which tasks or processes are broken down into smaller, independent units that can be executed simultaneously by the CPU. The goal of parallel programming is to leverage the processing power of multi-core CPUs, GPUs, or other computers or servers to improve the performance and speed of a program or computation. It is especially relevant in today's computing landscape, where heterogeneous and distributed systems are common.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Parallel programming is often used to tackle computationally intensive tasks such as data analysis, simulations, rendering, and more; by dividing the workload among multiple processing units. It can significantly reduce the time required to complete these tasks, making it a valuable skill for programmers and developers.

---

### Problem Statements

The following rule is applicable to the problem statements given below:

**You are permitted to use the Linux/Unix syscall APIs, C stdlib, Rust stdlib, Android NDK and libraries/APIs that are specifically mentioned in the tasks and their resources. Use of any other libraries/languages will not be preferred.**

+ **Task I**:  
  Write a multi-threaded program in C (using the pthreads API) **or** in Rust, to find the sum of all the elements in a randomly generated array of 64-bit unsigned integers, that has `N` elements where, `N` ≥ 1024. Use the following strategies to compute the sum using **only** 4 threads (assuming `i` is the thread index):
    1. Thread `i` computes the sum of the elements at index `x` (in the array), where `x mod 4 = i`
    2. Thread `i` computes the sum of the elements from the index `i * N / 4` upto `(i + 1) * N / 4` in the array

  Compare the amount of total time the programs take to finish. Also compare them against the regular, single-threaded case and find the most optimal strategy and the reason for choosing the same.
  This task **must** be executed on a Linux machine/Android device, via Termux.

+ **Task II**:  
  Implement a Lock-Free *SPSC* Ring-Buffer whose length is fixed; a known power of 2 (assume some small value), using an array as its backing storage. You can use this buffer/array to store data of any type of your choosing (use generics if implementing in Rust). Build this from scratch **without** using any *SPSC* types provided by the stdlib.
  
  Write this in C (using `threads.h` & `stdatomic.h`) **or** Rust. Log the elements to terminal as they are added/removed. This task **must** be executed on a Linux machine/Android device, via Termux.

+ **Task III**:  
  Write an optimized ESSL (similar to GLSL) compute shader and its corresponding program in C/Rust (or both, i.e. interop via FFI) using EGLv1.4+ & OpenGLESv3.1+ APIs from the Android NDK (latest, LTS) to run the simulation of a simple forest fire spreading on a two-dimensional `M`x`M` matrix/grid. Use the EGL extension, `EGL_KHR_surfaceless_context` if its supported on your device to make things easier.
  
  Each cell in the grid represents a tree that can exist in one of three states:
    1. Healthy (H) – A tree that can catch fire
    2. Burning (B) – A tree currently on fire
    3. Nothing (N) – A fully burnt tree or no tree
  
  The simulation evolves in discrete time steps (epochs) as per the following rules:
    1. A Burning (B) cell becomes a Nothing (N) cell in the next epoch
    2. A Healthy (H) cell becomes a Burning (B) cell with a probability, p = 0.15 in the next epoch, if **at least one of its neighboring cells** is a Burning (B) cell in the current epoch
    3. All other cells retain their current state

  The simulation continues until there are no Burning (B) cells remaining. Run the simulation for varying values of `M` and calculate the number of epochs it takes for the fire to extinguish. If `M` is small (around 10-20), print out the tree grid every epoch.
  This task **must** be executed on an Android device, via Termux.

**There are no hard requirements to fully complete these tasks. Take your time to understand the theory and its practical implications. Partial attempts shall be considered, but harder tasks will carry greater weightage.**

---

### Resources

+ **Common**:
  + [Linux man-pages](https://www.man7.org/linux/man-pages/)
  + [C Reference](https://en.cppreference.com/c)
  + [Rust std Docs](https://doc.rust-lang.org/std/)
  + [Android NDK download](https://developer.android.com/ndk/downloads)

+ **Task I**:
  + [Threads in Single-Core Systems (YouTube)](https://youtu.be/M9HHWFp84f0)
  + [Threads in Multi-Core Systems (YouTube)](https://youtu.be/5sw9XJokAqw)
  + [Pthreads (YouTube)](https://www.youtube.com/watch?v=uA8X5zNOGw8&list=PL9IEJIKnBJjFZxuqyJ9JqVYmuFZHr7CFM)
  + Rust [`thread`](https://doc.rust-lang.org/std/thread/index.html) & [`sync`](https://doc.rust-lang.org/std/sync/index.html) Docs

+ **Task II**:
  + [SPSC Ring-Buffer](https://en.wikipedia.org/wiki/Circular_buffer)
  + [Synchronization Primitives (YouTube)](https://youtu.be/IMceN4_rieo)
  + [Atomic Operations](https://preshing.com/20130618/atomic-vs-non-atomic-operations/)
  + [SPSC Lock-Free FIFO from Ground Up (YouTube)](https://youtu.be/K3P_Lmq6pw0)
  + [C Reference for threads.h & stdatomic.h](https://en.cppreference.com/c/thread)
  + Rust [`thread`](https://doc.rust-lang.org/std/thread/index.html) & [`atomic`](https://doc.rust-lang.org/std/sync/atomic/index.html) Docs

+ **Task III**:
  + [How do GPUs work? (YouTube)](https://www.youtube.com/watch?v=h9Z4oGN89MU&themeRefresh=1)
  + [Compute Shaders in OpenGL](https://wikis.khronos.org/opengl/Compute_Shader)
  + [GPU compute on Android with OpenGLES (Code & Slides)](https://github.com/Radonoxius/Android-GPU-Compute-Talk)
  + [EGLv1.4 Quick Reference](https://www.khronos.org/files/egl-1-4-quick-reference-card.pdf)
  + [EGLv1.5 Detailed Specifications](https://registry.khronos.org/EGL/specs/eglspec.1.5.pdf)
  + [EGL_KHR_surfaceless_context Extension](https://registry.khronos.org/EGL/extensions/KHR/EGL_KHR_surfaceless_context.txt)
  + [OpenGLESv3.2 Quick Reference](https://www.khronos.org/files/opengles32-quick-reference-card.pdf)
  + [OpenGLESv3.2 Detailed Specifications](https://registry.khronos.org/OpenGL/specs/es/3.2/es_spec_3.2.pdf)
  + [ESSLv3.2 Detailed Specifications](https://registry.khronos.org/OpenGL/specs/es/3.2/GLSL_ES_Specification_3.20.pdf)

---

### Submission

Create a **private** GitHub repo and put all of your code/observations and a detailed README in there. Make sure to keep everything organized in different folders for each task. If possible, put up all the compiled programs in your releases page.

Add the mentors as collaborators to your repo once you're done.

---

### Mentor's Details

1. `Nimesh Acharya` (<+91 87624 21203>, GitHub: [Radonoxius](https://github.com/Radonoxius))

2. `Ranjit Tanneru` (<+91 81239 99357>, GitHub: [AmissDrake](https://github.com/AmissDrake))
