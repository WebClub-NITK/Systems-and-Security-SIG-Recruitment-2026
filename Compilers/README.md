# Compilers

Compilers translate human-readable code into machine-executable instructions, and they also reject programs that are wrong before they ever run. This task asks you to build a compiler for **KirkPiler**, a tiny ASCII-only language for programming a small microcontroller. A KirkPiler program declares which pins and UARTs it uses, then runs ordinary code. You will:

- build a lexer and a parser from scratch;
- construct an AST;
- perform semantic analysis, including checking that the hardware is used correctly;
- generate LLVM IR (Intermediate Representation).

It's often easiest to implement in Python with llvmlite, but you may use C++ (or any language) if you prefer.

---

## Problem Statement(s)

### Target Hardware (KIRK-8)

- 8 pins: `P0`–`P7`. Every pin can be used as a plain `input` or `output`.
- 2 UARTs. Each UART signal can only be connected to certain pins:

| Signal | Allowed pins |
|---|---|
| `UART0.tx` | P0, P4 |
| `UART0.rx` | P1, P5 |
| `UART1.tx` | P2, P6 |
| `UART1.rx` | P3, P7 |

- Supported baud rates: 9600, 19200, 57600, 115200.

### Language Specifications (Language name: KirkPiler, ASCII-only)

#### Lexical Specifications

- **Character set:** ASCII only.
- **Keywords (reserved):** `pin`, `uart`, `fn`, `let`, `if`, `else`, `while`, `return`, `true`, `false`, `int`, `bool`, `input`, `output`.
- **Pins:** any word matching `P[0-9]+` is a pin token (for example `P3`, or `P9`, which lexes as a pin even though it does not exist).
- **Identifiers:** `[A-Za-z_][A-Za-z0-9_]*`, excluding keywords and pins.
- **Numbers:** base-10 unsigned integers that fit in 32 bits.
- **Strings:** `"…"` containing printable ASCII, closed on the same line. The only escape is `\n`.
- **Comments:** `//` to the end of the line.
- **Delimiters and operators:** `{ } ( ) ; , . : = ->` and `+ - * / % == != < <= > >= && || !`
- **Whitespace:** separators only.

#### Syntactic Specifications

- **Files:** source files use the `.kp` extension. A file is a sequence of `pin` declarations, `uart` declarations and functions, in any order. Exactly one function must be `fn main()`, the entry point, with no parameters and no return type.
- **Pin declaration:** `pin <name> = <pin> input|output;`
- **UART declaration:** `uart UART0|UART1 { tx = <pin>; rx = <pin>; baud = <number>; }`. All three properties are required, and they may appear in any order.
- **Functions:** `fn <name>(<param>: <type>, …) -> <type> { … }`. The `-> <type>` part is omitted for functions that return nothing. The types are `int` (unsigned 32-bit) and `bool`.
- **Statements:** every statement ends with `;`, except `if` and `while`, which end with their block.
  - `let x = e;` declares a variable.
  - `x = e;` assigns to it.
  - `if e { … } else { … }` (the `else` is optional)
  - `while e { … }`
  - `return e;` or `return;`
  - a function call or a device method call.
- **Expressions:** numbers, `true`/`false`, identifiers, calls and parentheses, combined with the operators below. The list is ordered from highest to lowest precedence. All binary operators are left-associative, and comparisons do not chain.
  1. `!`
  2. `* / %`
  3. `+ -`
  4. `== != < <= > >=`
  5. `&&`
  6. `||`
- **Device methods:**

| Receiver | Methods |
|---|---|
| output pin | `high()`, `low()`, `toggle()` |
| input pin | `read()`, which returns an `int`, 0 or 1 |
| UART | `start()`, `stop()`, `print(x)` where `x` is an `int` or a string |
| built-in | `wait(ms)` where `ms` is an `int` |

#### Semantic Specifications

- `int` arithmetic is unsigned 32-bit and wraps around. `x / 0` and `x % 0` evaluate to `0`.
- Conditions of `if` and `while` must be `bool`. `&&` and `||` short-circuit.
- `let` variables are mutable and block-scoped, and take their type from the initializer. Redeclaring a name that is already visible is an error.
- A function with a return type must return on every path. For this check:
  - a `return` statement always returns;
  - an `if` with an `else` always returns if both branches do;
  - a `while` loop never counts as always returning.
- **Execution:** the declarations are applied in source order, then `main()` runs.
- A UART must be `start()`ed before `print`. If it is not, the program faults at runtime (see *Runtime*).

### Line-by-line mini examples

Pin and UART declarations:

```
pin LED = P2 output;
pin BTN = P3 input;

uart UART0 {
    tx = P0;
    rx = P1;
    baud = 9600;
}
```

A function with a return value:

```
fn max(a: int, b: int) -> int {
    if a > b {
        return a;
    } else {
        return b;
    }
}
```

A loop with device calls:

```
fn blink(times: int) {
    let i = 0;
    while i < times {
        LED.toggle();
        wait(100);
        i = i + 1;
    }
}
```

The entry point:

```
fn main() {
    UART0.start();
    blink(3);
    UART0.print("done\n");
}
```

---

## Tasks

### Lexer (Tokenization) — from scratch

- Hand-write a lexer that recognizes all the tokens above. Do not use regex-based tokenizers or lexer generators.
- Every token records its line and column.
- On an unexpected character, a number that is too large, or an unterminated string, throw an exception that includes the position.

### Parser (AST Generation) — from scratch

- Hand-write a recursive-descent parser that builds an AST. Every node keeps its source position.
- Throw exceptions for missing `;`, unbalanced delimiters and unexpected tokens.

### Semantic Analysis

Throw an exception with the line, the column and a clear message for each of the following.

**Program errors:**

- an undeclared name, or a name declared twice;
- a type mismatch, such as `1 + true` or a non-`bool` condition;
- a wrong number of arguments in a call;
- a function with a return type that can reach its end without returning;
- a missing `fn main()`, or a `main` with the wrong signature.

**Hardware errors:**

- an unknown pin, such as `P9`;
- a pin that cannot carry the requested UART signal. The message must list the valid pins, for example `2:10: P2 cannot be used as UART0.tx (valid pins: P0, P4)`;
- a pin that is used more than once, by any combination of `pin` and `uart` declarations;
- an invalid method for the receiver, such as `read()` on an output pin or `toggle()` on a UART;
- a bad UART declaration: an unknown UART (for example `UART2`), an unknown or duplicate property, or a missing `tx`, `rx` or `baud`;
- an unsupported baud rate.

### LLVM IR Generation (via llvmlite) — expectations

- Emit `i32` for `int` and `i1` for `bool`. Locals are stack slots: read them with loads and write them with stores.
- User function `f` becomes `@f_f`. The declarations become `define void @kp_init()`, which calls the runtime in source order.
- `if` and `while` become basic blocks and branches. `&&` and `||` must short-circuit, which means they cannot be lowered to plain `and`/`or` instructions.
- Guard `/` and `%` against zero, because `udiv` and `urem` by zero are undefined in LLVM.
- A string passed to `print` becomes one `kp_uart_putc` call per character.
- Hardware operations become calls to the runtime functions below.
- Provide CLI flags to print the tokens, the AST and the IR, to run the program, and to toggle your optimization passes.

### Runtime

Your IR calls only these functions. Write them yourself, either in C and link with `clang`, or as Python `ctypes` callbacks for the llvmlite JIT.

```c
void kp_uart_init(int uart, int tx_pin, int rx_pin, int baud);
void kp_pin_mode(int pin, int mode);            // 0 = input, 1 = output
void kp_uart_start(int uart);
void kp_uart_stop(int uart);
void kp_uart_putc(int uart, int ch);
void kp_uart_print_int(int uart, int value);
void kp_pin_write(int pin, int level);          // high() = 1, low() = 0
void kp_pin_toggle(int pin);
int  kp_pin_read(int pin);
void kp_wait(int ms);
```

- Each call prints one log line: the function name without the `kp_` prefix, followed by the arguments. `kp_pin_read` also prints ` -> <result>`.
- The *n*-th `kp_pin_read` call for a pin (counting from 0) returns `n % 2`.
- Using a UART before `start()` prints `FAULT uart <id> not started` and exits with code 1.

### Optimization Passes and Analysis (Optional/Bonus Tasks)

- **UART check at compile time.** Reject any `print` where the UART may not be started on some path. This must work through `if`/`else`, loops and function calls, which requires a control-flow graph and a dataflow analysis.
- **Optimizations.** Implement two small, safe optimizations (for example constant folding, dead-code elimination or unreachable-code removal). Demonstrate their effect by comparing the IR with optimizations on and off, and show that the runtime log is unchanged.
- **SSA.** Emit IR with `phi` nodes and no `alloca`.
- **Better errors.** Report all errors instead of only the first, and suggest close names ("did you mean `LED`?").

---

## Code Quality & Build

- Code must be neat, well-documented and organized by module (lexer, parser, AST, semantic analysis, IR, runtime, passes).
- All errors are thrown as exceptions with clear messages that include the line and the column.
- Include tests for valid programs (compare the runtime logs) and for invalid programs (compare the errors).
- Bonus: include a Makefile with targets such as `build`, `test`, `run` and `fmt`.

---

## Program → IR (simple examples)

IR names and exact registers will differ; show equivalent structure in your output.

### Example 1 — Declarations become `kp_init`

**Code**

```
uart UART0 {
    tx = P0;
    rx = P1;
    baud = 9600;
}
pin LED = P2 output;
pin BTN = P3 input;
```

**Abridged IR**

```llvm
define void @kp_init() {
entry:
  call void @kp_uart_init(i32 0, i32 0, i32 1, i32 9600)
  call void @kp_pin_mode(i32 2, i32 1)
  call void @kp_pin_mode(i32 3, i32 0)
  ret void
}
```

### Example 2 — `while` loop with device calls

**Code**

```
fn blink(times: int) {
    let i = 0;
    while i < times {
        LED.toggle();
        wait(100);
        i = i + 1;
    }
}
```

**Abridged IR**

```llvm
define void @f_blink(i32 %times) {
entry:
  %times.addr = alloca i32
  %i = alloca i32
  store i32 %times, i32* %times.addr
  store i32 0, i32* %i
  br label %while.cond
while.cond:
  %0 = load i32, i32* %i
  %1 = load i32, i32* %times.addr
  %2 = icmp ult i32 %0, %1
  br i1 %2, label %while.body, label %while.end
while.body:
  call void @kp_pin_toggle(i32 2)
  call void @kp_wait(i32 100)
  %3 = load i32, i32* %i
  %4 = add i32 %3, 1
  store i32 %4, i32* %i
  br label %while.cond
while.end:
  ret void
}
```

### Example 3 — Short-circuit `&&` (the pin is only read when `a > 0`)

**Code**

```
fn check(a: int) -> int {
    if a > 0 && BTN.read() == 1 {
        return 1;
    }
    return 0;
}
```

**Abridged IR**

```llvm
define i32 @f_check(i32 %a) {
entry:
  %a.addr = alloca i32
  store i32 %a, i32* %a.addr
  %0 = load i32, i32* %a.addr
  %1 = icmp ugt i32 %0, 0
  br i1 %1, label %and.rhs, label %and.end
and.rhs:
  %2 = call i32 @kp_pin_read(i32 3)
  %3 = icmp eq i32 %2, 1
  br label %and.end
and.end:
  %4 = phi i1 [ false, %entry ], [ %3, %and.rhs ]
  br i1 %4, label %if.then, label %if.end
if.then:
  ret i32 1
if.end:
  ret i32 0
}
```

### Example 4 — Division guarded against zero

**Code**

```
fn div(x: int, y: int) -> int {
    return x / y;
}
```

**Abridged IR**

```llvm
define i32 @f_div(i32 %x, i32 %y) {
entry:
  %x.addr = alloca i32
  %y.addr = alloca i32
  store i32 %x, i32* %x.addr
  store i32 %y, i32* %y.addr
  %0 = load i32, i32* %x.addr
  %1 = load i32, i32* %y.addr
  %iszero = icmp eq i32 %1, 0
  %safe = select i1 %iszero, i32 1, i32 %1
  %q = udiv i32 %0, %safe
  %2 = select i1 %iszero, i32 0, i32 %q
  ret i32 %2
}
```

### Example 5 — Full program and its runtime log

**Code** (the declarations from Example 1 and `blink` from Example 2, plus:)

```
fn main() {
    UART0.start();
    let presses = 0;
    let n = 0;
    while n < 4 {
        if BTN.read() == 1 {
            presses = presses + 1;
            blink(2);
        }
        n = n + 1;
    }
    UART0.print("presses: ");
    UART0.print(presses);
}
```

**Runtime log** (the `...` line stands for the remaining characters of `presses: `)

```
uart_init 0 0 1 9600
pin_mode 2 1
pin_mode 3 0
uart_start 0
pin_read 3 -> 0
pin_read 3 -> 1
pin_toggle 2
wait 100
pin_toggle 2
wait 100
pin_read 3 -> 0
pin_read 3 -> 1
pin_toggle 2
wait 100
pin_toggle 2
wait 100
uart_putc 0 112
...
uart_putc 0 32
uart_print_int 0 2
```

### Example 6 — Rejected programs

| Code | Error |
|---|---|
| `uart UART0 { tx = P2; rx = P1; baud = 9600; }` | `P2 cannot be used as UART0.tx (valid pins: P0, P4)` |
| `pin LED = P0 output;` together with `uart UART0 { tx = P0; … }` | `pin P0 is already used by LED` |
| `uart UART0 { tx = P0; rx = P1; baud = 1234; }` | `unsupported baud rate 1234` |
| `pin LED = P2 output;` … `let x = LED.read();` | `read() is not valid on output pin LED` |
| `fn sign(x: int) -> int { if x > 0 { return 1; } }` | `function 'sign' can reach its end without returning` |

---

## Resources

- Overview: https://dev.to/lefebvre/compilers-101---overview-and-lexer-3i0m
- Lexing: https://web.stanford.edu/class/cs143/lectures/lecture04.pdf
- BNF Grammar: https://cs61.seas.harvard.edu/site/2020/BNFGrammars/
- Parsing: https://web.stanford.edu/class/archive/cs/cs143/cs143.1156/handouts/parsing.pdf
- Recursive Descent Parsing: https://web.stanford.edu/class/cs143/lectures/lecture06.pdf
- Scopes and Name Resolution: https://craftinginterpreters.com/resolving-and-binding.html
- Compiler Design Course: https://www.youtube.com/watch?v=9p_s457RSQE&list=PLTsf9UeqkRebOYdw4uqSN0ugRShSmHrzH
- Dragon Book: https://www-2.dc.uba.ar/staff/becher/dragon.pdf
- llvmlite: https://github.com/numba/llvmlite
- LLVM Language Reference: https://llvm.org/docs/LangRef.html
- "Kaleidoscope" Tutorial: https://llvm.org/docs/tutorial/
- Control-Flow Graphs and Data Flow (for the bonus): https://www.cs.cornell.edu/courses/cs6120/2020fa/lesson/2/ · https://www.cs.cornell.edu/courses/cs6120/2020fa/lesson/4/

---

## Submission

- Submit a GitHub repository link. The repository should be private until the end of recruitments. Add the mentors as collaborators (GitHub usernames: ).
- Include a README file covering the details of your implementation, including but not limited to:
  - the important challenges you faced;
  - the choices you made;
  - brief explanations of the procedures you implemented.

**Mentor name and contact details:**
- Abhinav S Rao (+91 8660033892, github: ABHINAV-S-RAO)
- Aryan Palimkar (+91 8971098003, github: Aryan-Palimkar)
