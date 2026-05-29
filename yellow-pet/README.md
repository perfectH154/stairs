# Yellow Pet

Yellow Pet is a small Windows desktop pet built with Electron, HTML, CSS, and plain JavaScript.

It opens as a transparent, frameless, always-on-top window and displays GIF animations from the `assets` folder.

## Download

Download the ready-to-run Windows EXE for your system:

- [Download Yellow Pet x64](https://github.com/perfectH154/stairs/releases/latest/download/Yellow%20Pet-1.0.0-x64.exe) for most modern 64-bit Windows computers.
- [Download Yellow Pet ia32](https://github.com/perfectH154/stairs/releases/latest/download/Yellow%20Pet-1.0.0-ia32.exe) for older 32-bit Windows computers.

Windows may show a security warning because this app is not code-signed. Click `More info`, then `Run anyway`.

## Features

- Transparent frameless desktop pet window
- Always on top by default
- Keeps GIF aspect ratio with no stretching
- Drag the pet with the left mouse button
- Right-click the pet to switch between idle, happy, sad, and wave
- The pet stays idle unless you choose another state from the right-click menu
- Right-click menu:
  - Switch face state
  - Enable or disable always on top
  - Reset position
  - Quit

## Project Structure

```text
yellow-pet/
  assets/
    nature.GIF
    jump.GIF
    sad.GIF
    wave.GIF
  src/
    index.html
    styles.css
    main.js
    preload.js
    renderer.js
  .gitignore
  package.json
  README.md
```

## Requirements

- Windows
- Node.js 18 or newer
- npm

## Install

Open PowerShell in the project folder:

```powershell
cd C:\Users\han\Documents\face\yellow-pet
npm install
```

## Run

```powershell
npm start
```

## Replace GIF Assets

The current GIF mapping is:

```text
nature.GIF -> idle
jump.GIF   -> happy
sad.GIF    -> sad
wave.GIF   -> wave
```

The default window size is `240x240`.

To change it, edit this line in `src/main.js`:

```js
const WINDOW_SIZE = 240;
```

GIFs are displayed with `object-fit: contain`, so they keep their original ratio.

## Right-Click Menu

Right-click the desktop pet to open the menu:

- `待机`
- `开心`
- `委屈`
- `挥手`
- `置顶` / `取消置顶`
- `重置位置`
- `退出`

## Build EXE

Create portable Windows executables for both 64-bit and 32-bit Windows:

```powershell
npm run dist
```

The output will be created in:

```text
dist/
```

The generated portable executables are:

```text
dist/Yellow Pet-1.0.0-x64.exe
dist/Yellow Pet-1.0.0-ia32.exe
```

For a quick unpacked build:

```powershell
npm run pack
```

## Notes For GitHub

- Do not commit `node_modules`.
- Keep GIF file names stable unless you also update `src/renderer.js` and `src/index.html`.
- This project uses plain JavaScript, not TypeScript.
- The renderer has no direct Node.js access. It communicates with Electron through `preload.js`.
