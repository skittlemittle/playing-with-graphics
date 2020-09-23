// game of life, so original

let bHeight, bWidth, gState;
const resolution = 10;

function drawBox(x, y, color, size) {
    stroke(150);
    const c = color === 'W' ? 0 : 255;
    fill(c);
    rect(x * size, y * size, size, size);
}

// count up immediate neighbors mmmm
function cntNeighbors(grid, x, y) {
    let sum = 0;
    for (let i = -1; i < 2; i++) {
        for (let j = -1; j < 2; j++) {
            sum += grid[x + 1][y + j];
        }
    }
    sum -= grid[x][y];
    return sum;
}

function setup() {
    createCanvas(800, 800);
    bWidth = width / resolution;
    bHeight = height / resolution;
    // fill it with noise
    gState = [...Array(bWidth)].map(() => (
        [...Array(bHeight)].map(() => (
            //(Math.random() % 2 < 0.09) ? 1 : 0
            Math.round(Math.random())
        ))
    ));

}

function draw() {
    background(255);

    const output = gState;
    // game of life game of life
    for (let x = 1; x < bWidth - 1; x++) {
        for (let y = 1; y < bHeight - 1; y++) {
            const nNeighbours = cntNeighbors(output, x, y);

            // set states
            if (output[x][y] === 1) {
                if (nNeighbours < 2 || nNeighbours > 3)
                    gState[x][y] = 0;
            } else {
                if (nNeighbours === 3)
                    gState[x][y] = 1;
            }

            // draw
            if (output[x][y] === 1) {
                drawBox(x, y, 'W', resolution);
            } else {
                drawBox(x, y, 'B', resolution);
            }
        }
    }
    //console.log(gState);
    //console.table(output);
}
