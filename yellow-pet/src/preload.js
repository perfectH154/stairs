const { contextBridge, ipcRenderer } = require("electron");

contextBridge.exposeInMainWorld("petApi", {
  beginDrag: (point) => ipcRenderer.send("pet:begin-drag", point),
  dragTo: (point) => ipcRenderer.send("pet:drag-to", point),
  endDrag: () => ipcRenderer.send("pet:end-drag"),
  onSetFace: (callback) => {
    ipcRenderer.on("pet:set-face", (_event, face) => callback(face));
  },
  onResetFace: (callback) => {
    ipcRenderer.on("pet:reset-face", () => callback());
  }
});
