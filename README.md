# Caustic3 Vita Wrapper
![110352994-d9775d80-8036-11eb-8a68-cf0cd8ae3615](https://github.com/user-attachments/assets/cc5a5b71-b3cf-4637-ad25-460042b2c8fc)

**Porting project of the mobile DAW "Caustic3" to the PlayStation Vita**

---

## 🎹 What is Caustic3?

Caustic3 is a modular Digital Audio Workstation (DAW) originally developed for Android by SingleCellSoftware.  
It features a rack-based interface with a variety of virtual synthesizers, samplers, effects, a mixer, and a pattern-based sequencer – all optimized for mobile music production.

---

## 🎯 Project Goal

This project aims to run the original `libcaustic.so` (from the Android APK) natively on the PS Vita using the VitaSDK toolchain.  
It will leverage:
- a custom ELF loader for Android `.so` libraries (`soloader`)
- audio backend mapping from OpenSL ES to `SceAudioOut`
- file/path redirection for Android-style `assets/` and `/sdcard/` calls
- UI, audio engine and effects running fully on Vita hardware

If successful, **Caustic3 will become the first fully functional DAW available on the PlayStation Vita.**

---

## ⚙️ Current Status (as of October 2025)

- ✅ VPK builds and installs correctly on Vita
- ✅ Assets and libcaustic.so included from the original APK
- ✅ Logging and file access working (`debug.log`, `ping.txt`)
- ✅ Custom Soloader integration implemented
- 🔄 `libcaustic.so` loads but requires `so_util` for Android compatibility
- ⏳ Next step: Patch loading and resolve symbols via `so_util`
- 🔄 Integrate so_util: load patches and resolve missing symbols; add automated symbol check (dump & diff)
- 🧭 Complete path mapping: map /sdcard and /storage/emulated/0 → ux0:/data/CAUSTIC3/; normalize paths; enforce RO from app0:/assets and RW to data
- 🧰 Android shims: route __android_log_print → debug_log; provide minimal libandroid/AAsset stubs or redirect through the path mapper; stub remaining Android APIs as needed
- 🎧 Audio backend: choose OpenSL-ES wrapper vs direct SceAudioOut; implement ring buffer + audio thread; pause/resume; decide 44.1 kHz vs 48 kHz and resample if needed; underrun handling
- 📦 Build/packaging: consolidate CMake install rules for assets/configs; deterministic asset copy; first-run data dir bootstrap for ux0:/data/CAUSTIC3
- 🧪 Sanity tests: asset read→copy test; symbol-resolution smoke test; fixed-tone (sine) audio test; verify file permissions on real hardware
- 🔍 Diagnostics: optional on-screen overlay (FPS, audio underruns); log verbosity levels; log truncation/rotation
- 📚 Docs: build steps, deployment, troubleshooting; credit soloader and so_util; list known issues and workarounds
- 🚧 Known issues to watch: path case sensitivity; potential 44.1 ↔ 48 kHz mismatch; missing Android runtime-permission equivalents; PSTV vs handheld audio timing differences

---

## 📦 APK Source

The `libcaustic.so` and assets used in this project are extracted from:

> **caustic-3-3-2-0.apk**  
> (original Android version from SingleCellSoftware)

---

## 📁 Folder Structure

```
/vita-project/
├── src/                → loader, logging and main app source
├── lib/                → contains libcaustic.so and loader stubs
├── assets/             → copied from APK assets/
├── build/              → build output directory
└── CMakeLists.txt      → current working configuration
```

---
## ❗ Legal Note

- **Caustic3** and all related trademarks are the property of their original creator, **SingleCellSoftware**.
- The original app is no longer maintained and has been removed from all major app stores.
- **Porting permission has been granted by the original developer** for a full, native PS Vita port.
- Special thanks to the PS Vita homebrew community.
- Feedback and code reviews by experienced Vita developers and reverse engineers are welcome.


---

## 🤝 Credits

- **SingleCellSoftware** – for creating Caustic3  
- **v-atamanenko / Rinnegatamante / Vita SDK team** – for loader and platform tools  
- **Trackelf** – PS Vita integration and adaptation
