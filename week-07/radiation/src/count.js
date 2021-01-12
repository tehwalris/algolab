const numVars = 3;
for (let maxTotalPow = 1; maxTotalPow < 30; maxTotalPow++) {
  function recursiveCount(counts, seenStrings) {
    if (counts.reduce((a, c) => a + c, 0) === maxTotalPow) {
      const s = counts.join(" ");
      if (seenStrings.has(s)) {
        return 0;
      }
      seenStrings.add(s);
      return 1;
    }

    let total = 0;
    for (let i = 0; i < numVars; i++) {
      total += recursiveCount(
        counts.map((c, j) => (i === j ? c + 1 : c)),
        seenStrings
      );
    }
    return total;
  }
  console.log(maxTotalPow, recursiveCount(Array(numVars).fill(0), new Set()));
}
