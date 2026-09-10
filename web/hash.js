const charMap = [];

onconnect = e => {
    console.log("connected");
}

onmessage = async e => {
    const challenge = e.data.challenge;
    const difficulty = e.data.difficulty;
    const groupsize = 16;

    if(charMap.length == 0) {
        // generate num -> char table
        for(let i = 0; i < 256; i++) {
            charMap.push(i.toString(16).padStart(2, "0"));
        }
    }

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
        count: counter,
        time: finish - start
    }
    postMessage(response);
}

async function sha256(msg) {
    const msgbuf = new TextEncoder().encode(msg);
    const buf = await crypto.subtle.digest("SHA-256", msgbuf);
    const arr = Array.from(new Uint8Array(buf));
    const hex = arr.map(x => charMap[x]).join("");

    return hex;
}

async function doublesha(str, difficulty) {
    // as it turns out, the crypto operation itself takes far longer than any string conversion,
    // so no performance gain from doing bitwise comparison versus character equivalence...
    const hash = await sha256(await sha256(str));
    const length = hash.length;

    for(let i = length - 1; i > length - 1 - difficulty; i--) {
        if(hash[i] != "0") return false;
    }

    console.log(hash);
    return true;
}