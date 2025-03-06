#pragma once

enum class EPlayer
{
	Player1,
	Player2
};

class GameManager
{
public:
	static GameManager& Get()
	{
		static GameManager fm;
		return fm;
	}

	void AddScore(EPlayer player)
	{
		(player == EPlayer::Player1 ? player1Score : player2Score)++;
	}

	bool checkOverWinScore()
	{
		return (player1Score >= WIN_SCORE) || (player2Score >= WIN_SCORE);
	}

	EPlayer GetWinner()
	{
		return player1Score >= player2Score ? EPlayer::Player1 : EPlayer::Player2;
	}

	void SetUpNewGame()
	{
		player1Score = 0;
		player2Score = 0;
		isPaused = false;
	}

	unsigned int GetPlayer1Score() { return player1Score; }
	unsigned int GetPlayer2Score() { return player2Score; }

	bool IsPause() { return isPaused; }
	bool IsPlaying() { return !isPaused && !shouldStartNewGame; }

	void Paused() { isPaused = true; }
	void Playing() { isPaused = false; }

	void TogglePlaying() const { isPaused != isPaused; }

	void StartNewGame() { shouldStartNewGame = true; }
	void CancelNewGame() { shouldStartNewGame = false; }
	void ReadyForNewGame() { shouldStartNewGame = false; }

	// 새로운 게임 시작 여부
	bool ShouldStartNewGame() const { return shouldStartNewGame; }

private:
	GameManager() {};
	GameManager(const GameManager&) = delete;
	GameManager& operator=(const GameManager&) = delete;
	~GameManager() {}

	const int WIN_SCORE = 3;

	unsigned int player1Score = 0;
	unsigned int player2Score = 0;

	bool isPaused = false;
	bool shouldStartNewGame = false;
};