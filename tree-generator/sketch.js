// 2D trees
// skittlemittle

const w = 600;
const h = 750;

function setup() {
  createCanvas(w, h);
  drawTree();
}

function branch(len, minLen = 20) {
  let angle = random(-(PI / 6), PI / 6);
  line(0, 0, 0, -len);
  translate(0, -len);
  if (len > minLen) {
    // random amount of branches
    for (let i = 0; i < round(random(1, 4)); i++) {
      angle = random(-(PI / 6), PI / 6);
      push();
      rotate(angle);
      branch(len * random(0.4, 1));
      pop();
    }
  }
}

function drawTree() {
  background(0);
  stroke(255);
  push();
  translate(w / 2, h);
  branch(random(50, 150));
  pop();
}

function keyPressed() {
  if (keyCode !== 32) return; // space
  drawTree();
}
