# Firmware

This repository contains all firmware related to the force-torque sensor for this project. It includes code for:

- Capacitive sensing  
- PSoC™ 4100PS ↔ ESP communication  
- ESP Bluetooth communication  

## Software Requirements

### 1. ModusToolbox

See the [Modus Installation Guide](modus_installation_guide.md) for detailed setup instructions.

### 2. VS Code Setup

#### 2.1 Install VS Code & Extension

1. Install VS Code: https://code.visualstudio.com/docs/setup/setup-overview  
2. Install the **ModusToolbox Assistant** extension.

> **Note:** **You should not need to create a new app; this repo will contain all the code you need.** That being said, if you want to , you can create a new ModusToolbox app via the extension UI if needed. Select your board and any peripherals, then download the generated project. Also know that any additional peripherals will translate to completely new projects, so you should only create 1 new application and then add more dependencies later (see “Adding Dependencies” below).

#### 2.2 Clone & Initialize the Repo

After cloning this repo, the `.vscode/` folder and ModusToolbox libraries aren’t included. From your project root, run:

```bash
make getlibs    # Fetch all ModusToolbox dependencies
make vscode     # Generate VS Code configuration files
```

#### 2.3 Adding Dependencies

ModusToolbox libraries are tracked by `.mtb` files in `deps/`. Running `make getlibs` will clone or update each library and wire it into your project.

**Recommended (GUI):**

1. cd into your app folder  
2. Run `make library-manager`  
3. In the UI, search for “CAPSENSE” (or any other library) and click **Update**

**CLI / Manual:**

1. Copy the library’s `.mtb` file into your app’s `deps/` directory  
2. From the project root, run:

    ```bash
    make getlibs
    ```

This clones or updates each repo, checks out the specified tag, and resolves indirect dependencies. Shared repos are placed in `mtb_shared` (or the path set by `CY_GETLIBS_PATH`).
