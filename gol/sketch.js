// game of life, so original

let bHeight, bWidth, gState;
const resolution = 5;

function cntNeighbors(grid, x, y) {
    let sum = 0;
    for (let i = -1; i < 2; i++) {
        for (let j = -1; j < 2; j++) {
            let c = (x + i + bWidth) % bWidth;
            let r = (y + j + bHeight) % bHeight;
            sum += grid[c][r];
        }
    }
    sum -= grid[x][y];
    return sum;
}

function setup() {
    createCanvas(displayWidth, displayHeight);
    bWidth = round(width / resolution);
    bHeight = round(height / resolution);
    // fill it with noise
    gState = [...Array(bWidth)].map(() => (
        [...Array(bHeight)].map(() => (
            Math.round(Math.random())
        ))
    ));
}

function draw() {
    background(255);

    // stupid
    const next = [...Array(bWidth)].map(() => (new Array(bHeight)));

    // game of life game of life
    for (let x = 0; x < bWidth; x++) {
        for (let y = 0; y < bHeight; y++) {
            const nNeighbours = cntNeighbors(gState, x, y);
            const state = gState[x][y];

            // set states
            if (state === 0 && nNeighbours === 3) {
                next[x][y] = 1;
            } else if (state == 1 && (nNeighbours < 2 || nNeighbours > 3)) {
                next[x][y] = 0;
            } else {
                next[x][y] = state;
            }

            // draw
            if (state === 1) {
                noStroke();
                fill(0);
                rect(x * resolution, y * resolution, resolution, resolution);
            }
        }
    }
    gState = next;
}
