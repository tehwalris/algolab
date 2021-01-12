const child_process = require("child_process");
const path = require("path");

const COMPILED_ALGORITHM_PATH = "../a"; // relative to this JS file

function algorithmCpp(input) {
  const inputLines = ["1", `${input.h} ${input.t}`];
  for (const segment of input.points) {
    inputLines.push(`${segment[0]} ${segment[1]} ${segment[2]}`);
  }

  const output = child_process.execSync(
    path.join(__dirname, COMPILED_ALGORITHM_PATH).replace(/ /g, "\\ "),
    { input: inputLines.join("\n") }
  );

  return +output.toString();
}

function randomPosition() {
  return (
    (Math.random() < 0.5 ? -1 : 1) * Math.floor(Math.random() * ((1 << 10) + 1))
  );
}

function genRandomInput(n) {
  const h = 1 + Math.floor(Math.random() * n);
  const t = n - h;

  const points = [];
  for (let i = 0; i < n; i++) {
    points.push([randomPosition(), randomPosition(), randomPosition()]);
  }
  return { h, t, points };
}

function fuzz(maxBadInputs, minN, maxN) {
  const badInputs = [];
  for (let n = minN; n <= maxN; n++) {
    console.log(`n = ${n}`);
    for (let j = 0; j < 1000; j++) {
      const input = genRandomInput(n);
      try {
        algorithmCpp(input);
      } catch (e) {
        console.error(e);
        badInputs.push(input);
        if (badInputs.length === maxBadInputs) {
          return badInputs;
        }
      }
    }
  }
  return badInputs;
}

console.log(fuzz(1, 2, 5)[0]);
