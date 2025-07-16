#pragma once
class Difficulty
{
public:
	static Difficulty& Instance() {
		static Difficulty instance;
		return instance;
	}

	enum mode
	{
		easy,
		normal,
		hard
	};

	int GetDifficulty() { return m_Difficulty; }
	void SetDifficulty(mode difficulty) { m_Difficulty = difficulty; }
private:
	mode m_Difficulty;
};