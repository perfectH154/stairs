const path = require("path");
const { app, BrowserWindow, Menu, ipcMain, screen } = require("electron");

const WINDOW_SIZE = 240;

let petWindow = null;
let isAlwaysOnTop = true;
let dragStart = null;

function createPetWindow() {
  petWindow = new BrowserWindow({
    width: WINDOW_SIZE,
    height: WINDOW_SIZE,
    frame: false,
    transparent: true,
    resizable: false,
    hasShadow: false,
    alwaysOnTop: isAlwaysOnTop,
    backgroundColor: "#00000000",
    webPreferences: {
      preload: path.join(__dirname, "preload.js"),
      contextIsolation: true,
      nodeIntegration: false
    }
  });

  petWindow.loadFile(path.join(__dirname, "index.html"));
  petWindow.once("ready-to-show", () => {
    resetWindowPosition();
    petWindow.show();
  });

  petWindow.webContents.on("context-menu", () => {
    showContextMenu();
  });
}

function showContextMenu() {
  const menu = Menu.buildFromTemplate([
    {
      label: "待机",
      click: () => {
        petWindow.webContents.send("pet:set-face", "idle");
      }
    },
    {
      label: "开心",
      click: () => {
        petWindow.webContents.send("pet:set-face", "happy");
      }
    },
    {
      label: "委屈",
      click: () => {
        petWindow.webContents.send("pet:set-face", "sad");
      }
    },
    {
      label: "挥手",
      click: () => {
        petWindow.webContents.send("pet:set-face", "wave");
      }
    },
    { type: "separator" },
    {
      label: isAlwaysOnTop ? "取消置顶" : "置顶",
      type: "checkbox",
      checked: isAlwaysOnTop,
      click: () => {
        isAlwaysOnTop = !isAlwaysOnTop;
        if (petWindow) {
          petWindow.setAlwaysOnTop(isAlwaysOnTop, "floating");
        }
      }
    },
    { type: "separator" },
    {
      label: "重置位置",
      click: () => {
        resetWindowPosition();
        petWindow.webContents.send("pet:reset-face");
      }
    },
    { type: "separator" },
    {
      label: "退出",
      click: () => {
        app.quit();
      }
    }
  ]);

  menu.popup({ window: petWindow });
}

function resetWindowPosition() {
  if (!petWindow) {
    return;
  }

  const area = screen.getPrimaryDisplay().workArea;
  const x = area.x + area.width - WINDOW_SIZE - 80;
  const y = area.y + area.height - WINDOW_SIZE - 80;
  petWindow.setBounds({ x, y, width: WINDOW_SIZE, height: WINDOW_SIZE });
}

function keepInsideWorkArea(x, y, width, height) {
  const display = screen.getDisplayNearestPoint({ x, y });
  const area = display.workArea;

  return {
    x: Math.round(Math.max(area.x, Math.min(x, area.x + area.width - width))),
    y: Math.round(Math.max(area.y, Math.min(y, area.y + area.height - height)))
  };
}

ipcMain.on("pet:begin-drag", (_event, point) => {
  if (!petWindow) {
    return;
  }

  dragStart = {
    mouseX: point.x,
    mouseY: point.y,
    bounds: petWindow.getBounds()
  };
});

ipcMain.on("pet:drag-to", (_event, point) => {
  if (!petWindow || !dragStart) {
    return;
  }

  const nextX = dragStart.bounds.x + point.x - dragStart.mouseX;
  const nextY = dragStart.bounds.y + point.y - dragStart.mouseY;
  const safe = keepInsideWorkArea(nextX, nextY, dragStart.bounds.width, dragStart.bounds.height);

  petWindow.setBounds({
    x: safe.x,
    y: safe.y,
    width: dragStart.bounds.width,
    height: dragStart.bounds.height
  });
});

ipcMain.on("pet:end-drag", () => {
  dragStart = null;
});

app.whenReady().then(() => {
  createPetWindow();
});

app.on("window-all-closed", () => {
  app.quit();
});
