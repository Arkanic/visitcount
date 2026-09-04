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
    
    // now work on incrementing amount through webworker
    if(!window.Worker) return;
    const nonceWorker = new Worker("./hash.js");

    nonceWorker.addEventListener("message", async e => {
        const nonceres = e.data;
        console.log(`viewcount hash complete, took ${(nonceres.time / 1000).toFixed(3)}s`);
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
            throw new Error(`nonce check failed, ${incrementData}`);
        }
    });

    nonceWorker.postMessage({
        challenge: countData.challenge,
        difficulty: countData.difficulty
    });
})());