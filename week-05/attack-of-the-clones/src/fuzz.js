const child_process = require("child_process");
const path = require("path");

const COMPILED_ALGORITHM_PATH = "../a"; // relative to this JS file

function algorithmCpp(input) {
  const inputLines = ["1", `${input.jediSegments.length} ${input.m}`];
  for (const segment of input.jediSegments) {
    inputLines.push(`${segment[0]} ${segment[1]}`);
  }

  const output = child_process.execSync(
    path.join(__dirname, COMPILED_ALGORITHM_PATH).replace(/ /g, "\\ "),
    { input: inputLines.join("\n") }
  );

  return +output.toString();
}

function genRandomInput(n) {
  const m = Math.floor(Math.random() * 100) + 1;
  const randomPosition = () => Math.floor(Math.random() * m) + 1;
  const jediSegments = [];
  for (let i = 0; i < n; i++) {
    jediSegments.push([randomPosition(), randomPosition()]);
  }
  return { m, jediSegments };
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
        badInputs.push(input);
        if (badInputs.length === maxBadInputs) {
          return badInputs;
        }
      }
    }
  }
  return badInputs;
}

console.log(fuzz(1, 1, 5)[0]);
