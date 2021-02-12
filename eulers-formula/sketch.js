// animated eulers formula
const ZOOM = 100;
let theta = 0;

/* returns a list of the first n terms of exp(x) */
function e(x, n) {
  if (!(x instanceof Complex)) return null;
  return [...new Array(n)].map((_, i) => x.pow(i).div(factorial(i)));
}

function factorial(x) {
  if (x === 0) return 1;
  return x * factorial(x - 1);
}

function draw() {
  background(0);

  stroke(255);
  noFill();

  push();
  translate(width / 2, height / 2);
  ellipse(0, 0, 1 * ZOOM * 2);

  stroke(153, 0, 204);
  const vectors = e(Complex(0, 1).mul(Complex(theta)), 100);
  for (let term of vectors) {
    term = term.mul(ZOOM);
    line(0, 0, term.re, -term.im);
    translate(term.re, -term.im);
  }
  pop();
  // "text"
  noStroke();
  fill(0, 102, 153);
  text(`theta = ${theta}`, 50, 50);

  theta += 0.01;
  if (theta >= Math.PI * 2) theta = 0;
}

function setup() {
  createCanvas(600, 600);
}
