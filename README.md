# B15F Communication Protocol

This project implements a communication protocol between two B15F boards (HTWK Leipzig).

## Overview
The goal is to create a **bidirectional synchronous** protocol, allowing both boards to send and receive data simultaneously.

## Current Status
Currently, only **half-synchronous communication** is working: one board sends while the other receives. Synchronization is handled via flags and acknowledgements.

## To-Do
- Complete bidirectional transfer
- Improve error handling
- Refactor synchronization logic

## Documentation
More details: [B15F API Reference](https://devfix.org/b15f/html/classB15F.html#a8b4533d232c55ef2aa967e39e2d23380)

---

Still a work in progress. Contributions or feedback welcome!
