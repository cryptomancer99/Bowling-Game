#include <vector>
#include <memory>

class ScoringStrategy {
public:
virtual ~ScoringStrategy() = default;
// Changed to non-const BowlingGame& to allow recordThrows
virtual int calculateScore(const std::vector<int>& rolls, int& rollIndex, BowlingGame& game) = 0;
};


class NormalFrameStrategy : public ScoringStrategy {
public:
    int calculateScore(const std::vector<int>& rolls, int& rollIndex, BowlingGame& game) override {
        int score = 0;
        if (rollIndex + 1 >= rolls.size()) return score; // Ensure 2 throws available

        int firstThrow = rolls[rollIndex];
        int secondThrow = rolls[rollIndex + 1];
        game.recordThrows(firstThrow, secondThrow);

        if (firstThrow == 10) { // Strike
            score = 10;
            if (rollIndex + 3 < rolls.size()) { // Need next 2 rolls for bonus
                score += rolls[rollIndex + 2] + rolls[rollIndex + 3];
            }
            rollIndex += 2; // Move past the frame (2 throws, even with strike)
        } else { // Spare or Open
            int frameTotal = firstThrow + secondThrow;
            score = frameTotal;
            if (frameTotal == 10 && rollIndex + 2 < rolls.size()) { // Spare
                score += rolls[rollIndex + 2];
            }
            rollIndex += 2;
        }
        return score;
    }
};

class TenthFrameStrategy : public ScoringStrategy {
public:
    int calculateScore(const std::vector<int>& rolls, int& rollIndex, BowlingGame& game) override {
        int score = 0;
        if (rollIndex + 1 >= rolls.size()) return score; // Need at least 2 throws

        int firstThrow = rolls[rollIndex];
        int secondThrow = rolls[rollIndex + 1];
        game.recordThrows(firstThrow, secondThrow);

        score = firstThrow + secondThrow;

        // Check for spare or strike in this frame or any previous frame
        bool extraThrowAllowed = (firstThrow == 10 || firstThrow + secondThrow == 10);
        for (int i = 1; i < 10 && !extraThrowAllowed; ++i) {
            extraThrowAllowed = game.hasSpareOrStrikeInFrame(i);
        }

        if (extraThrowAllowed && rollIndex + 2 < rolls.size()) {
            int thirdThrow = rolls[rollIndex + 2];
            score += thirdThrow;
            rollIndex += 3;
        } else {
            rollIndex += 2;
        }
        return score;
    }
};
