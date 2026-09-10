const countPath = "/count";
const incrementPath = "/increment";

// get page count
((async () => {
    const countResponse = await fetch(countPath);
    if(!countResponse.ok) {
        throw new Error("failed to get page viewcount");
    }
    const countData = await countResponse.json();
    console.log(countData.count);

    const evt = new CustomEvent("countdata", {
        detail: {
            count: countData.count,
            difficulty: countData.difficulty
        }
    });
    document.dispatchEvent(evt);
    
    // now work on incrementing amount through webworker
    if(!window.Worker) return;
    const nonceWorker = new Worker("./hash.js");

    nonceWorker.addEventListener("message", async e => {
        const nonceres = e.data;
        // funny math, ms is second / 1000 so H / ms = kH/s
        console.log(`viewcount hash complete, took ${(nonceres.time / 1000).toFixed(3)}s, cycled ${nonceres.count} times, ${(nonceres.count / nonceres.time).toFixed(2)}kH/s`);
        // nonce calculated successfully, time to submit result
        const incrementResponse = await fetch(incrementPath, {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                nonce: nonceres.nonce,
                time: nonceres.time
            })
        });
        const incrementData = await incrementResponse.json();
        if(!incrementResponse.ok) {
            throw new Error(`nonce check failed, provided "${nonceres.nonce}", got ${JSON.stringify(incrementData)}`);
        }

        nonceWorker.terminate();
    });

    nonceWorker.postMessage({
        challenge: countData.challenge,
        difficulty: countData.difficulty
    });
})());