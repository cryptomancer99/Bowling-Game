#include <vector>
#include <memory>


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
