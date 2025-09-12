#pragma once

#include <algorithm> // std::abs
#include <cmath> // std::ceil
class RankSystem
{

private:
	enum class RankCode
	{
		S,
		A,
		B,
		C,
	};
public:
	struct Rank {
		RankCode Task;
		RankCode Time;
		RankCode Result;
	};
	Rank rank;

	static RankSystem& Instance()
	{
		static RankSystem instance;
		return instance;
	}
	RankSystem() {};
	~RankSystem() {};
	void SetRank(float score, float maxScore, float time, bool isTutorial) {
		float scale = 1.0f;
		if (isTutorial)
		{
			scale = 1.f / 3.f;
		}
		float score_ = score / maxScore;
		if (score_ >= 80.f / 100.f) rank.Task = RankCode::S;
		else if (score_ >= 50.f / 100.f) rank.Task = RankCode::A;
		else if (score_ >= 20.f / 100.f) rank.Task = RankCode::B;
		else  rank.Task = RankCode::C;

		if (time <= 0.f * scale) rank.Time = RankCode::S;
		else if (time <= 60.f * scale) rank.Time = RankCode::A;
		else if (time <= 120.f * scale) rank.Time = RankCode::B;
		else  rank.Time = RankCode::C;


		//if (life == 3)rank.Death = RankCode::S;
		//else if (life == 2)rank.Death = RankCode::A;
		//else if (life == 1)rank.Death = RankCode::B;
		//else rank.Death = RankCode::C;

		if (std::abs(float(rank.Task) - float(rank.Time)) == 1)
		{
			rank.Result = (float(rank.Task) > float(rank.Time)) ? rank.Task : rank.Time;
		}
		else {
			float average = (float(rank.Task) + float(rank.Time)) / 2.0f;
			int finalValue = static_cast<int>(std::ceil(average));
			rank.Result = static_cast<RankCode>(finalValue);

		}


	}
	Rank GetRank() { return rank; };
};
