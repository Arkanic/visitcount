#include <unordered_map>
#include <string>
#include <fstream>
#include <chrono>

#define CHALLENGE_CHARACTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
#define CHALLENGE_LENGTH 32
#define CHALLENGE_LIFETIME_SECONDS 15
#define CHALLENGE_CLEAN_PERIOD 15
// low because we're not going to ask for a 32 zero hash are we?
#define NONCE_MAXSIZE 16
#define NONCE_INPUT_MAXSIZE NONCE_MAXSIZE + CHALLENGE_LENGTH + 1

class Nonce {
    private:
        std::unordered_map<std::string, std::chrono::time_point<std::chrono::steady_clock>> issuedChallenges;
        unsigned char difficulty;
        const std::string challenge_characters = CHALLENGE_CHARACTERS;
        std::ifstream urandom;
        std::chrono::time_point<std::chrono::steady_clock> lastClean;
    
    public:
        Nonce(unsigned char difficulty);
        std::string issueChallenge();
        unsigned char getDifficulty();
        bool validateResponse(std::string nonce);
        void deleteChallenge(std::string challenge);
    
    private:
        void cleanIssuedChallenges();
};