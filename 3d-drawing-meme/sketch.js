/*
 * 3D acceleromter drawing meme
 * license: MIT
 * GOBLIN CODE THAT DOES NOT WORK
 */

math.config({
  matrix: 'Matrix'
});

// wow this guy uses global vars what a loser
let buffer;
let prevX = 0;
let prevY = 0;
let prevZ = 0;
let velX = 0;
let velY = 0;
let velZ = 0;

let vel;
let prevVel;
let position;
let prevPos;
let prevTime = 0;

function setup() {
  createCanvas(windowWidth, windowHeight, WEBGL);
  // for the line trace
  buffer = createGraphics(width, height, WEBGL);
  buffer.background(0);
  
  vel = createVector(0, 0, 0);
  prevVel = createVector(0, 0, 0);
  position = createVector(0, 0, 0);
  prevPos = createVector(0, 0, 0);
  console.log(`${rotationX}, ${rotationY}, ${rotationZ}`);
}

function draw() {
  angleMode(DEGREES);
  background(0);
  imageMode(CENTER);
  image(buffer, 0, 0, width, height);
  
     // debug garbage
//   document.getElementById("readings").innerHTML = (round(rotationX) + ","+ round(rotationY) +','+ round(rotationZ) +' ::: '+
//                                                     round(accelerationX) +","+ round(accelerationY) +","+ round(accelerationZ));
  
  let acc = createVector(round(accelerationX), round(accelerationY), round(accelerationZ));
  // v = at
  vel.set(acc.x * deltaTime/50, acc.y * deltaTime/50, acc.z * deltaTime/50);
  // s = ut+1/2at^2
  let distance = math.matrix([
    [prevVel.x * deltaTime/50 + (1/2) * acc.x*(deltaTime/50)**2],
    [prevVel.y * deltaTime/50 + (1/2) * acc.y*(deltaTime/50)**2],
    [prevVel.z * deltaTime/50 + (1/2) * acc.z*(deltaTime/50)**2]
  ]);
  // rotate it
  rotatedDistance = rotateVector(distance, rotationZ, rotationX, rotationY);
  // thats where we draw the line to now.
  position.add(
    math.subset(rotatedDistance, math.index(0, 0)),
    math.subset(rotatedDistance, math.index(1, 0)),
    math.subset(rotatedDistance, math.index(2, 0))
  );
//   draw the abomination
  buffer.stroke(50, 200, 100);
  buffer.strokeWeight(1);
  if (frameCount > 1) {
    buffer.line(prevPos.x, prevPos.y, prevPos.z, position.x, position.y, position.z);
  }

  prevVel.set(vel.x, vel.y, vel.z);
  vel.set(0, 0, 0);
  prevPos.set(position.x, position.y, position.z);
}

// rotates the displacement vector, returns vector
function rotateVector(vector, a, b, g) {
  /*
 * THICC T H I C C https://en.wikipedia.org/wiki/Rotation_matrix
 * yaw, pitch, roll => a, b, g ; along the z, y, x axes respectively
 * please work
 */
  const rMatrix = math.matrix([
    [cos(a)*cos(b), cos(a)*sin(b)*sin(g)-sin(a)*cos(g), cos(a)*sin(b)*cos(g)+sin(a)*sin(g)],
    [sin(a)*cos(b), sin(a)*sin(b)*sin(g)+cos(a)*cos(g), sin(a)*sin(b)*cos(g)-cos(a)*sin(g)],
    [-sin(b)      , cos(b)*sin(g)                     , cos(b)*cos(g)]
  ]);
  return math.multiply(rMatrix, vector);
}

