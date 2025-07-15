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
	struct Rank {
		RankCode Task;
		RankCode Death;
		RankCode Result;
	};
	Rank rank;
public:
	static RankSystem& Instance()
	{
		static RankSystem instance;
		return instance;
	}
	RankSystem();
	~RankSystem();
	void SetRank(float task, float alltask, float life) {
		float score = task / alltask;
		if (score >= 80.f / 100.f) rank.Task = RankCode::S;
		else if (score >= 50.f / 100.f) rank.Task = RankCode::A;
		else if (score >= 20.f / 100.f) rank.Task = RankCode::B;
		else  rank.Task = RankCode::C;


		if (life == 3)rank.Death = RankCode::S;
		else if (life == 2)rank.Death = RankCode::A;
		else if (life == 1)rank.Death = RankCode::B;
		else rank.Death = RankCode::C;

		if (std::abs(float(rank.Task) - float(rank.Death)) == 1)
		{
			rank.Result = (float(rank.Task) > float(rank.Death)) ? rank.Task : rank.Death;
		}
		else {
			float average = (float(rank.Task) + float(rank.Death)) / 2.0f;
			int finalValue = static_cast<int>(std::ceil(average));
			rank.Result = static_cast<RankCode>(finalValue);

		}


	}
	Rank GetRank() { return rank };
};

RankSystem::RankSystem()
{
}

RankSystem::~RankSystem()
{
}