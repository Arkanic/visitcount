#include <sstream>
#include "nonce.hpp"
#include "SHA256.h"
#include "crow.h"

Nonce::Nonce(unsigned char diff) {
    difficulty = diff;
    issuedChallenges = {};
    urandom = std::ifstream("/dev/urandom", std::ios::in | std::ios::binary);
    if(!urandom) {
        throw std::runtime_error("Failed to open /dev/urandom");
    }
    lastClean = std::chrono::steady_clock::now();
}

std::string Nonce::issueChallenge() {
    // first we clean old cache
    cleanIssuedChallenges();

    std::string challenge;
    while(challenge.size() < CHALLENGE_LENGTH) {
        char byte;
        if(urandom.get(byte)) {
            unsigned char ubyte = static_cast<unsigned char>(byte);
            if(ubyte < (256 - (256 % challenge_characters.size()))) {
                challenge += challenge_characters[ubyte % challenge_characters.size()];
            }
        } else {
            throw std::runtime_error("failed to fetch byte from urandom");
        }
    }

    issuedChallenges.insert({challenge, std::chrono::steady_clock::now()});

    return challenge;
}

unsigned char Nonce::getDifficulty() {
    return difficulty;
}

bool Nonce::validateResponse(std::string nonce) {
    // comma split nonce, left half is input right half is nonce itself
    // assume evil input, sanitize first
    std::stringstream stream(nonce);
    std::string challenge;
    if(!std::getline(stream, challenge, ','))
        return false;
    if(challenge.size() != CHALLENGE_LENGTH)
        return false;
    if(!issuedChallenges.count(challenge)) // ok we know it does exist in the list of yet-to-be-solved hashes
        return false;
    
    std::string noncein;
    if(!std::getline(stream, noncein))
        return false;
    if(noncein.size() > NONCE_MAXSIZE)
        return false;
    
    // now lets try hashing
    std::string tohash = challenge + "," + noncein;
    std::string result = sha256(sha256(tohash));

    short trailingZeroes = 0;
    short i = result.length() - 1;
    while (i >= 0 && result[i] == '0') {
        trailingZeroes++;
        i--;
    }

    deleteChallenge(challenge);

    return trailingZeroes >= difficulty;
}

void Nonce::deleteChallenge(std::string challenge) {
    issuedChallenges.erase(challenge);
}

void Nonce::cleanIssuedChallenges() {
    if(lastClean + std::chrono::seconds(CHALLENGE_CLEAN_PERIOD) > std::chrono::steady_clock::now()) return;

    for(auto challenge = issuedChallenges.begin(); challenge != issuedChallenges.end(); ) {
        if(challenge->second + std::chrono::seconds(CHALLENGE_LIFETIME_SECONDS) < std::chrono::steady_clock::now())
            challenge = issuedChallenges.erase(challenge);
        else
            challenge++;
    }
}