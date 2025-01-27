# C-Workshop-Hash-Based-Generic-Data-Structures-Library

### Generic Data Structures Library

---

#### Overview
This project implements a generic data structures library in C, focusing on hash-based data structures, including hash tables and sets. The library emphasizes efficiency, memory management, and modular design, offering a solid foundation for data manipulation. The project was developed as part of the "Programming Workshop in C & C++ (67312)" at the Hebrew University of Jerusalem, Rachel and Selim Benin School of Computer Science and Engineering.

---

#### Features

1. **Generic Data Structures:**
   - **HashSet**: Implements a hash set using closed hashing and quadratic probing to handle collisions efficiently.
   - **Hash Functions**: Modular arithmetic-based hash functions optimized for distribution and collision reduction.

2. **Dynamic Handling:**
   - **Load Factor Management**: Adjusts capacity dynamically based on upper and lower load factors to balance memory usage and performance.
   - **Memory Optimization**: Efficient memory allocation and deallocation to minimize leaks and errors.

3. **Utility and Modularity:**
   - Predefined utility functions for testing and debugging.
   - Modular implementation, including separate components for hash functions, utility functions, and data structure core.

---

#### Usage

The library supports a wide range of operations:
- Insert, delete, and search for elements with average constant-time complexity \( O(1) \) under typical conditions.
- Dynamic resizing ensures performance consistency as the data set scales.

---

#### Requirements

- **Language Standard:** C99
- **Compiler Flags:** 
  - `-Wall -Wextra -Wvla -Werror -g -lm`
- **Tools:** The project must pass compilation, memory checks, and automated testing scripts provided in the course.

---

#### Submission

The deliverables include:
1. Implementation files (`Node.c`, `HashSet.c`).
2. Header files with clear API definitions (`Node.h`, `HashSet.h`).
3. Adherence to provided coding style and documentation standards.

---

#### Testing & Validation

Ensure the following:
- Pass all course-provided test scripts without warnings or errors.
- Verify memory management through tools like `valgrind`.
- Comply with the prescribed coding style for clarity and maintainability.

---
