const child_process = require("child_process");
const path = require("path");
const stream = require("stream");

const COMPILED_ALGORITHM_PATH = "../a"; // relative to this JS file

/**
 *
 * @param {number[]} boatLengths
 * @param {number[]} ringPositions
 * @return {number} Maximum number of boats that can be tied at the same time
 */
function mostBoatsTiedGolden(boatLengths, ringPositions) {
  if (boatLengths.length !== ringPositions.length) {
    throw new Error("boatLengths and ringPositions have different length");
  }
  const n = boatLengths.length;

  let mergedInputs = boatLengths
    .map((l, i) => [l, ringPositions[i]])
    .sort((a, b) => a[1] - b[1]);
  boatLengths = mergedInputs.map((m) => m[0]);
  ringPositions = mergedInputs.map((m) => m[1]);

  if (n > 32) {
    throw new Error("too many boats");
  }
  let bestNumTied = 0;
  let bestSolution = 0;
  for (let i = 0; i < 1 << n; i++) {
    let lastFullCell;
    let solutionWorks = true;
    let numTied = 0;
    for (let j = 0; j < n; j++) {
      if (!(i & (1 << j))) {
        continue;
      }
      if (lastFullCell === undefined) {
        lastFullCell = ringPositions[j];
      } else if (lastFullCell > ringPositions[j]) {
        solutionWorks = false;
        break;
      } else {
        lastFullCell = Math.max(
          lastFullCell + boatLengths[j],
          ringPositions[j]
        );
      }
      numTied++;
    }
    if (solutionWorks && numTied > bestNumTied) {
      bestNumTied = numTied;
      bestSolution = i;
    }
  }
  return bestNumTied;
}

/**
 *
 * @param {number[]} boatLengths
 * @param {number[]} ringPositions
 * @return {number} Maximum number of boats that can be tied at the same time
 */
function mostBoatsTiedCpp(boatLengths, ringPositions) {
  const s = stream.Readable();
  let input = ["1", `${boatLengths.length}`];
  for (let i = 0; i < boatLengths.length; i++) {
    input.push(`${boatLengths[i]} ${ringPositions[i]}\n`);
  }

  const output = child_process.execSync(
    path.join(__dirname, COMPILED_ALGORITHM_PATH).replace(/ /g, "\\ "),
    {
      input: input.join("\n"),
    }
  );

  return +output.toString();
}

// https://stackoverflow.com/questions/2450954/how-to-randomize-shuffle-a-javascript-array
function getShuffledArray(array) {
  return array
    .map((a) => ({ sort: Math.random(), value: a }))
    .sort((a, b) => a.sort - b.sort)
    .map((a) => a.value);
}

function genRandomInput(n) {
  let ringPositions = [];
  let lastRingPosition = 0;
  for (let i = 0; i < n; i++) {
    const p = lastRingPosition + Math.floor(Math.random() * 5) + 1;
    ringPositions.push(p);
    lastRingPosition = p;
  }
  ringPositions = getShuffledArray(ringPositions);

  boatLengths = ringPositions.map(() =>
    Math.floor(Math.random() * (Math.random() < 0.8 ? 5 : 100) + 1)
  );

  return { boatLengths, ringPositions };
}

/**
 *
 * @param {number} maxBadInputs Stop early if this many bad inputs are found
 * @return {...Object}
 */
function findBadInputs(maxBadInputs, minN, maxN) {
  const badInputs = [];
  outer: for (let n = minN; n <= maxN; n++) {
    console.log(`n = ${n}`);
    for (let j = 0; j < 1000; j++) {
      const input = genRandomInput(n);
      const args = [input.boatLengths, input.ringPositions];
      if (mostBoatsTiedCpp(...args) != mostBoatsTiedGolden(...args)) {
        badInputs.push(input);
        if (badInputs.length == maxBadInputs) {
          break outer;
        }
      }
    }
  }
  return badInputs;
}

function demoInput(input) {
  const args = [input.boatLengths, input.ringPositions];
  return {
    ...input,
    actual: mostBoatsTiedCpp(...args),
    expected: mostBoatsTiedGolden(...args),
  };
}

// console.log(demoInput({
//   boatLengths: [5, 2, 6, 3, 2, 4, 5],
//   ringPositions: [9, 17, 10, 11, 16, 13, 6],
// }));
// console.log(demoInput({ boatLengths: [9], ringPositions: [8] }));
console.log(findBadInputs(1, 3, 5).map(demoInput));
