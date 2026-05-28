const petArea = document.getElementById("petArea");
const petImage = document.getElementById("petImage");

const FACE_SRC = {
  idle: "../assets/nature.GIF",
  happy: "../assets/jump.GIF",
  sad: "../assets/sad.GIF",
  wave: "../assets/wave.GIF"
};

let currentFace = "idle";
let dragStart = null;

function setFace(face) {
  if (currentFace === face) {
    return;
  }

  currentFace = face;
  petImage.src = FACE_SRC[face];
}

function chooseFace(face) {
  setFace(face);
}

function beginDrag(event) {
  if (event.button !== 0) {
    return;
  }

  dragStart = {
    x: event.screenX,
    y: event.screenY
  };
  petArea.classList.add("dragging");
  window.petApi.beginDrag(dragStart);
}

function dragTo(event) {
  if (!dragStart) {
    return;
  }

  window.petApi.dragTo({
    x: event.screenX,
    y: event.screenY
  });
}

function endDrag() {
  if (!dragStart) {
    return;
  }

  window.petApi.endDrag();
  petArea.classList.remove("dragging");
  dragStart = null;
}

petArea.addEventListener("mousedown", beginDrag);
window.addEventListener("mousemove", dragTo);
window.addEventListener("mouseup", endDrag);

window.petApi.onSetFace((face) => {
  chooseFace(face);
});

window.petApi.onResetFace(() => {
  chooseFace("idle");
});
