#include <vector>
#include <memory>

class BowlingGame {
private:
    int totalScore = 0;
    int currentFrame = 1;
    std::vector<int> frameScores;
    std::unique_ptr<GameState> state;
    std::vector<std::pair<int, int>> throwsPerFrame; // Store throws for each frame

public:
    BowlingGame() {
        state = std::make_unique<NormalFrameState>();
    }

    void setState(std::unique_ptr<GameState> newState) {
        state = std::move(newState);
    }

    void addScore(int score) {
        totalScore += score;
        if (currentFrame <= 10) {
            frameScores.push_back(totalScore);
        }
    }

    void recordThrows(int firstThrow, int secondThrow) {
        throwsPerFrame.push_back({firstThrow, secondThrow});
    }

    bool hasSpareOrStrikeInFrame(int frame) const {
        if (frame < 1 || frame > throwsPerFrame.size()) return false;
        int first = throwsPerFrame[frame - 1].first;
        int second = throwsPerFrame[frame - 1].second;
        return first == 10 || (first + second == 10);
    }

    int getTotalScore() const { return totalScore; }
    int getCurrentFrame() const { return currentFrame; }
    void nextFrame() { ++currentFrame; }
    const std::vector<int>& getFrameScores() const { return frameScores; }

    void play(const std::vector<int>& rolls) {
        int rollIndex = 0;
        while (currentFrame <= 10 && rollIndex < rolls.size()) {
            state->processFrame(*this, rolls, rollIndex);
        }
    }
};
