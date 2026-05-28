const petArea = document.getElementById("petArea");
const petImage = document.getElementById("petImage");

const FACE_SRC = {
  idle: "../assets/nature.GIF",
  happy: "../assets/jump.GIF",
  sad: "../assets/sad.GIF",
  thinking: "../assets/wave.GIF"
};

const SAD_TIMEOUT_MS = 60 * 1000;
const CLICK_MOVE_LIMIT = 5;

let currentFace = "idle";
let dragStart = null;
let movedDuringDrag = false;
let sadTimer = null;

function setFace(face) {
  if (currentFace === face) {
    return;
  }

  currentFace = face;
  petImage.src = FACE_SRC[face];
}

function scheduleSadFace() {
  clearTimeout(sadTimer);
  sadTimer = setTimeout(() => {
    setFace("sad");
  }, SAD_TIMEOUT_MS);
}

function resetInteractionTimer() {
  scheduleSadFace();
}

function chooseFace(face) {
  setFace(face);
  resetInteractionTimer();
}

function beginDrag(event) {
  if (event.button !== 0) {
    return;
  }

  dragStart = {
    x: event.screenX,
    y: event.screenY
  };
  movedDuringDrag = false;
  petArea.classList.add("dragging");
  window.petApi.beginDrag(dragStart);
  resetInteractionTimer();
}

function dragTo(event) {
  if (!dragStart) {
    return;
  }

  const dx = event.screenX - dragStart.x;
  const dy = event.screenY - dragStart.y;

  if (Math.abs(dx) > CLICK_MOVE_LIMIT || Math.abs(dy) > CLICK_MOVE_LIMIT) {
    movedDuringDrag = true;
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

  if (!movedDuringDrag) {
    chooseFace("happy");
  }

  movedDuringDrag = false;
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

scheduleSadFace();
