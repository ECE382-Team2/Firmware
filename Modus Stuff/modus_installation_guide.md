# ModusToolbox Installation Guide (Shortened)

This guide provides the **minimum steps** to install ModusToolbox™ software.  
For the full official installation guide, see: [Infineon ModusToolbox Installation Guide](https://www.infineon.com/ModusToolboxInstallguide).

---

## 1. System Requirements
- **OS**: Windows 10/11, macOS (Ventura/Sonoma/Sequoia), or Ubuntu Linux (22.04/24.04/25.04)  
- **Resources**: ~25 GB free disk space, 8 GB RAM recommended  
- **Notes**:  
  - No spaces/illegal characters in home directory  
  - Python required for some tools (not included)  
  - SEGGER J-Link debugger requires separate installation (v6.98+)

---

## 2. Install ModusToolbox Setup Program

1. **Download Setup Program**  
   Get the installer from:  
   [ModusToolbox Setup Download](https://softwaretools.infineon.com/tools/com.ifx.tb.tool.modustoolboxsetup)

2. **Install (per OS)**  
   - **Windows**: Run installer → choose *Install for me only*  
   - **macOS**: Run `.pkg` → install in Applications  
   - **Linux**: Run  
     ```bash
     sudo apt install ./ModusToolboxSetupInstaller_x.x.x_linux_x64.deb
     ```

3. **Launch Setup Program**  
   - Run automatically after install or launch manually from OS menu.

---

## 3. Install Required Packages

1. In Setup Program, select **ModusToolbox Tools Package** and **ModusToolbox™ CAPSENSE™ and Multi-Sense Pack** (used for programming with the capacative sensors)
   (Dependencies such as GCC and Programming Tools are auto-selected.)  
2. Accept license agreements and click **Start**.  
3. Wait for installation to complete.  
4. Optionally, install **Eclipse IDE** or **VS Code** (links available in Setup). but we will be using VS code for the sake of this guide 

---

## 4. Install Optional Packs
- Select additional **Technology Packs** (Machine Learning, Connectivity, Industrial, Security) as needed.  
- For **Early Access Packs**, Infineon account access may be required.

---

## 5. Run Dashboard (Optional)
- **Windows**: Start Menu → "Dashboard"  
- **Linux**: Run `<install-path>/ModusToolbox/tools_<version>/dashboard`  
- **macOS**: Run *Dashboard.app*

---

✅ You are now ready to develop with ModusToolbox!  

---
