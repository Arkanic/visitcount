#include <unordered_set>
#include <string>
#include <fstream>

#define CHALLENGE_CHARACTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
#define CHALLENGE_LENGTH 32
// low because we're not going to ask for a 32 zero hash are we?
#define NONCE_MAXSIZE 16

class Nonce {
    private:
        std::unordered_set<std::string> issuedChallenges;
        unsigned char difficulty;
        const std::string challenge_characters = CHALLENGE_CHARACTERS;
        std::ifstream urandom;
    
    public:
        Nonce(unsigned char difficulty);
        std::string issueChallenge();
        unsigned char getDifficulty();
        bool validateResponse(std::string nonce);
        void deleteChallenge(std::string challenge);
};