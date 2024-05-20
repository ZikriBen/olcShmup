#ifndef DIFFICULTY_H
#define DIFFICULTY_H
#include <string>
#include <unordered_map>

#pragma once

enum class GameDifficulty {
	EASY = 1,
	NORMAL = 2,
	HARD = 3
};

class Difficulty {
public:
	std::unordered_map<std::string, float> diffMap;
	Difficulty() {};
	void setDifficulty(GameDifficulty diff) {
		if (diff == GameDifficulty::EASY) {
			diffMap["WorldSpeed"] = 30.0f;
			diffMap["PlayerHealth"] = 100.0f;
			diffMap["EnemyHealth"] = 2.0f;
			diffMap["BossHealth"] = 10.0f;
			diffMap["PlayerFireRate"] = 0.1;
			diffMap["ScoreFactor"] = 1;
		}
		else if (diff == GameDifficulty::HARD) {
			diffMap["WorldSpeed"] = 60.0f;
			diffMap["PlayerHealth"] = 10.0f;
			diffMap["EnemyHealth"] = 6.0f;
			diffMap["BossHealth"] = 40.0f;
			diffMap["PlayerFireRate"] = 0.4;
			diffMap["ScoreFactor"] = 3;
		}
		else {
			diffMap["WorldSpeed"] = 40.0f;
			diffMap["PlayerHealth"] = 40.0f;
			diffMap["EnemyHealth"] = 5.0f;
			diffMap["BossHealth"] = 20.0f;
			diffMap["PlayerFireRate"] = 0.2f;
			diffMap["ScoreFactor"] = 2.0f;
		}
	}
};

#endif //DIFFICULTY_H