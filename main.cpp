	#ifndef REFACTOR_CODE

    #include "BowlingGame.hpp"
    #include "GameState.hpp"
    #include "ScoringStrategy.hpp"
    
    #endif

    #include <iostream>

    #ifdef REFACTOR_CODE

    	// Forward declarations
	class BowlingGame;
	
	// Scoring Strategy Interface
	class ScoringStrategy {
	public:
	    virtual ~ScoringStrategy() = default;
	    // Changed to non-const BowlingGame& to allow recordThrows
	    virtual int calculateScore(const std::vector<int>& rolls, int& rollIndex, BowlingGame& game) = 0;
	};
	
	// Game State Interface
	class GameState {
	public:
	    virtual ~GameState() = default;
	    virtual void processFrame(BowlingGame& game, const std::vector<int>& rolls, int& rollIndex) = 0;
	    virtual bool isLastFrame() const = 0;
	};
	
	// Concrete States (defined before BowlingGame to avoid forward declaration issues)
	class NormalFrameState : public GameState {
	private:
	    std::unique_ptr<ScoringStrategy> strategy;
	
	public:
	    NormalFrameState();
	    void processFrame(BowlingGame& game, const std::vector<int>& rolls, int& rollIndex) override;
	    bool isLastFrame() const override { return false; }
	};
	
	class TenthFrameState : public GameState {
	private:
	    std::unique_ptr<ScoringStrategy> strategy;
	
	public:
	    TenthFrameState();
	    void processFrame(BowlingGame& game, const std::vector<int>& rolls, int& rollIndex) override;
	    bool isLastFrame() const override { return true; }
	};
	
	// Bowling Game Class
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
	
	// Concrete Strategies
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
	
	// NormalFrameState and TenthFrameState methods
	NormalFrameState::NormalFrameState() {
	    strategy = std::make_unique<NormalFrameStrategy>();
	}
	
	void NormalFrameState::processFrame(BowlingGame& game, const std::vector<int>& rolls, int& rollIndex) {
	    int score = strategy->calculateScore(rolls, rollIndex, game);
	    game.addScore(score);
	    game.nextFrame();
	    if (game.getCurrentFrame() == 10) {
	        game.setState(std::make_unique<TenthFrameState>());
	    }
	}
	
	TenthFrameState::TenthFrameState() {
	    strategy = std::make_unique<TenthFrameStrategy>();
	}
	
	void TenthFrameState::processFrame(BowlingGame& game, const std::vector<int>& rolls, int& rollIndex) {
	    int score = strategy->calculateScore(rolls, rollIndex, game);
	    game.addScore(score);
	    // Add addional frame past 10th frame to end game
	    game.nextFrame(); 
	}


    #endif //REFACTOR_CODE
    
    int main() {
	    // Add user scores or can be taken as input
	    std::vector<int> rolls = {
	        4, 5,   // Frame 1
	        5, 5,   // Frame 2 (spare)
	        1, 7,   // Frame 3
	        6, 2,   // Frame 4
	        10, 0,  // Frame 5 (strike)
	        5, 4,   // Frame 6
	        2, 7,   // Frame 7
	        9, 1,   // Frame 8 (spare)
	        6, 4,   // Frame 9 (spare)
	        6, 4, 1 // Frame 10 (spare + extra throw due to prior spares/strikes)
	    };
	    BowlingGame game;
	    game.play(rolls);
	
	    std::cout << "Frame-by-frame cumulative scores:\n";
	    const auto& scores = game.getFrameScores();
	    for (size_t i = 0; i < scores.size(); ++i) {
	        std::cout << "Frame " << (i + 1) << ": " << scores[i] << "\n";
	    }
	
	    return 0;
	}
