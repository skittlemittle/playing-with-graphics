// Makes a cooling map for use with: https://github.com/skittlemittle/playing-with-graphics/tree/master/fire
// skittlemittle

const w = 8;
const h = 32;
const pSize = 40;
let coolingMap = [w * h];

// returns cooling map
function makeMap(density = 0.8, passes = 5) {
    let buff1 = [...Array(w * h)].map(() =>
        (Math.random() % 2 > density) ? 128 : 0 // darker maps look better
    );
    let buff2 = [...Array(w * h)].fill(0);
    // smooth it
    for (let p = 0; p < passes; p++) {
        for (let x = 1; x < w - 1; x++) {
            for (let y = 1; y < h - 1; y++) {
                // 4 neighboring pixels, wraps around corners
                const n1 = buff1[(x + 1) + y * w];
                const n2 = buff1[(x - 1) + y * w];
                const n3 = buff1[x + (y + 1) * w];
                const n4 = buff1[x + (y - 1) * w];

                buff2[x + y * w] = Math.round((n1 + n2 + n3 + n4) * 0.25);
            }
        }
        // swamp
        buff1 = buff2.slice();
    }
    return buff2;
}

function setup() {
    createCanvas(w * pSize, h * pSize);
}

function keyPressed() {
    if (keyCode !== 32) return;// if not spacebar skip
    background(0);
    noStroke();

    coolingMap = makeMap();
    let string = "";

    for (let x = 0; x < w; x++) {
        for (let y = 0; y < h; y++) {
            const cur = coolingMap[x + y * w];
            string += cur + ',';
            fill(cur);
            rect(x * pSize, y * pSize, pSize, pSize);
        }
    }
    document.getElementById('buffer-out').textContent = string;
    return false;
}
