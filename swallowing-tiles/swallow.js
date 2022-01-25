/*
 * Draws a tile grid with tiles that shrink when you send the mouse near them.
 */

let WIDTH;
let HEIGHT;
const TILESPACING = 40;
const TILESIZE = TILESPACING - 1;

let pointRange = 200; // radius of tiles to effect
const points = []; // x, y of each point

/** makes a new point object */
const point = (x, y, range) => ({
  x,
  y,
  range,
  distanceTo: (a, b) => dist(x, y, a, b),
});

function setup() {
  const cnv = createCanvas(displayWidth, displayHeight);
  cnv.mouseWheel(changeSize);
  cnv.mouseClicked(addPoint);
  WIDTH = round(width / TILESPACING);
  HEIGHT = round(height / TILESPACING);
}

/** draws a swallowed tile, centered at (x, y)
 * @param x: position x
 * @param y: position y
 * @param m1: top left swallow multiplier
 * @param m2: top right swallow multiplier
 * @param m3: bottom right swallow multiplier
 * @param m4: bottom left swallow multiplier
 * @param w: width
 * @param h: height
 */
function drawTile(
  x,
  y,
  m1 = 1,
  m2 = 1,
  m3 = 1,
  m4 = 1,
  w = TILESIZE,
  h = TILESIZE
) {
  beginShape();
  vertex(x - (w / 2) * m1, y - (h / 2) * m1);
  vertex(x + (w / 2) * m2, y - (h / 2) * m2);
  vertex(x + (w / 2) * m3, y + (h / 2) * m3);
  vertex(x - (w / 2) * m4, y + (h / 2) * m4);
  endShape(CLOSE);
}

/** scales the swallow radius with mouse scrolls */
function changeSize(event) {
  if (event.deltaY > 0) {
    pointRange -= 0.3 * Math.abs(event.deltaY);
  } else {
    pointRange += 0.3 * Math.abs(event.deltaY);
  }
}

/** adds a point :) */
function addPoint() {
  points.push(point(mouseX, mouseY, pointRange));
  console.log(pointRange);
}

function limit(x) {
  if (x > 1) return 1;
  else if (x <= 0) return 1;
  else return 1 - x;
}

function draw() {
  background(0);

  noStroke();
  fill(255);

  points.unshift(point(mouseX, mouseY, pointRange));

  for (let x = 0; x < WIDTH; x++) {
    for (let y = 0; y < HEIGHT; y++) {
      const tx = x * TILESPACING + TILESPACING / 2;
      const ty = y * TILESPACING + TILESPACING / 2;

      let [c1, c2, c3, c4] = [1, 1, 1, 1];

      points.forEach((p) => {
        if (p.distanceTo(tx, ty) < p.range) {
          // point distance from each corner, extremely ugly
          const _c1 = Math.abs(
            p.distanceTo(tx - TILESIZE / 2, ty - TILESIZE / 2)
          );
          const _c2 = Math.abs(
            p.distanceTo(tx + TILESIZE / 2, ty - TILESIZE / 2)
          );
          const _c3 = Math.abs(
            p.distanceTo(tx + TILESIZE / 2, ty + TILESIZE / 2)
          );
          const _c4 = Math.abs(
            p.distanceTo(tx - TILESIZE / 2, ty + TILESIZE / 2)
          );
          c1 -= norm(_c1, 0, p.range);
          c2 -= norm(_c2, 0, p.range);
          c3 -= norm(_c3, 0, p.range);
          c4 -= norm(_c4, 0, p.range);
        }
      });
      drawTile(tx, ty, limit(c1), limit(c2), limit(c3), limit(c4));
    }
  }
  points.shift();
}
