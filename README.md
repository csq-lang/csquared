Raven
---
Raven is a fast, pragmatic general-purpose language mixing imperative and object-oriented programming.

# Build
Raven is made in C99, and uses Make as it's build system.

Right now there are no packages nor built executables, the only way to use Raven is to build it from source:
```bash
# supposing you are in the repo's root
cd compiler
make all
```

Also, raven doesn't generate binaries (nor objects) for now, just x86 ASM, so you have to generate the binary and link manually
```bash
# note that the source's filename can be replaced by anything
./bin/raven -s source.rvn # generate the assembly
as --32 source.s -o source.o # convert the assembly into an object
ld -m elf_i386 source.o -o source.out # generate the binary
./souce.out # run the binary
```

> **Raven only supports Linux x86 for now**.
> Raven will first support Linux, then Windows and finally MacOS (full support coming probably later than version 1).

# Roadmap

## Phase 1
> See [phase 1's description](https://codeberg.org/nykenik24/raven/src/branch/main/docs/plan.md#phase-1-first-version-of-raven) here.

- [x] **Lexer**: converts the raw source into *tokens* (=words) inside a token list. [Source](./compiler/src/parser/lexer.c).
- [x] **Parser**: generates a CST (*Concrete-Syntax Tree*), a raw representation of the program, excluding symbols that are not needed and are purely syntactical.
- [x] **Compiler**: generates *x86/ARM NASM* from the AST and packs it into a binary, which can be ran by the user.
- [ ] **Semantical analyzer**: analyzes the AST before compilation to check for semantic error before wasting resources compiling clearly illegal (semantically) code.
- [ ] **Optimizer**: tries to optimize the code as much as possible: optimizes loops, removes code that doesn't affect the output (dead code), reuses previously calculated values, among other optimizations.
- [ ] **Porting**: add Windows/MacOS compatibilty.

## Phase 2
> See [phase 2's description](https://codeberg.org/nykenik24/raven/src/branch/main/docs/plan.md#phase-2-further-improvements) here.

### Documentation
Documentation-related tasks.

- [ ] **Formal specification**: although to this point a considerable part of the specification would probably be written, 
there will still be things to do, as things like dynamic semantics might be vague or even undocumented before phase 2.
- [ ] **User manual**: even if there is an specification, it's necessary to also make a documentation/user manual 
which is less technical and factual and more informative and helpful. The user manual should, at least, 
allow the user to get familiar with all features present in Raven.
- [ ] **Website**: a simple, static website showcasing the main features of Raven, use cases and both the documentation and specification.

### Compiler
- [ ] **Further optimization**: more specific and complex optimizations aside from the simple, general and generic ones.

<!-- ## Phase 3 -->
<!-- > See [phase 3's description](https://codeberg.org/nykenik24/raven/src/branch/main/docs/plan.md#phase-3-the-stage-2-compiler) here. -->
