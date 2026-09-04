onmessage = async e => {
    const challenge = e.data.challenge;
    const difficulty = e.data.difficulty;
    const groupsize = 16;

    let success = "";
    let counter = 0;
    const nonceprefix = `${challenge},`;
    let start = Date.now();
    while(!success) {
        // do in batches instead of individually
        const group = Array.from({length: groupsize}, (j, k) => counter + k)
        const results = await Promise.all(group.map(n => doublesha(nonceprefix + `${n}`, difficulty)));
        const i = results.indexOf(true);
        if(i != -1) success = nonceprefix + `${group[i]}`;
        counter += groupsize;
    }
    let finish = Date.now();

    const response = {
        nonce: success,
        time: finish - start
    }
    postMessage(response);
}

async function doublesha(str, difficulty) {
    const msg = new TextEncoder().encode(str);
    let hash = await crypto.subtle.digest("SHA-256", msg);
    hash = new Uint8Array(await crypto.subtle.digest("SHA-256", hash)); // 2nd round

    const fullbytes = difficulty >> 1;
    const length = hash.byteLength;
    for(let i = 0; i < fullbytes; i++) {
        if(hash[length - 1 - i] != 0) return false;
    }
    if((difficulty & 1) && (hash[length - 1 - fullbytes] & 0x0F) != 0) return false;

    return true;
}