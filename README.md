# 👻 Master of Deception – C++ Trojan

**Standalone C++ Trojan** – designed for stealthy infiltration, espionage, and persistent backdoor access.

---

## 🎯 **Role**
Spy & Infiltrator – masquerades, opens backdoors, creates decoys, and hides its presence.

---

## ⚡ **Capabilities**

| Ability | Description |
|---------|-------------|
| **Perfect Disguise** | Changes its process name to `sshd` (or any specified) to blend in with system processes. |
| **Backdoor Creation** | Opens a persistent reverse shell to a C2 server (configurable IP/port). |
| **Phantom Clone** | Drops decoy files (e.g., `Important_Update.exe`) and spawns fake processes to confuse analysts. |
| **Silent Access** | Suppresses bash history, clears system logs, and disables auditd to avoid triggering alarms. |
| **Hidden Presence** | Renames itself to a dot-file, adds itself to cron, systemd, and rc.local for persistence. |

---

## ⚠️ **Disclaimer**

> **For educational and authorized testing only.**  
> Unauthorised use is illegal. The author assumes no responsibility for misuse.

---

## 📦 **Requirements**

- **Compiler:** `g++` (C++11 or later)
- **Build tools:** `make`, `binutils`
- **Linux/Unix** environment (uses POSIX functions)

---

## 🔧 **Compilation**

```bash
g++ -O2 -Wall -pthread -o trojan_deception trojan_deception.cpp
