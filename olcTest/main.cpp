#include <string>

using namespace std;

#include "olcConsoleGameEngine.h"

class CMD_Snake : public olcConsoleGameEngine
{
public:
	struct Cartesian {
		float x;
		float y;
	};

	CMD_Snake() { m_sAppName = L"CMD_Snake"; }

	virtual bool OnUserCreate() override
	{
		srand(time(NULL));
		bGameOver = false;
		mapWidth = ScreenWidth() * 4 / 5;
		mapHeight = ScreenHeight();
		mHead = { (float)mapWidth / 2,(float)mapHeight / 2 };

		mSnakeTail.push_back({ mHead.x + 1, mHead.y });
		mSnakeTail.push_back({ mHead.x + 2, mHead.y });

		mFruit = { (float)(rand() % (mapWidth - 2)) + 1, (float)(rand() % (mapHeight - 2) + 1) };
		mDirection = { 0, 0 };
		mCurrentScore = 0;
		return true;
	}

	void inputHandling()
	{
		if (m_keys[VK_UP].bPressed && mDirection.y != 1)
			mDirection = { 0, -1 };
		if (m_keys[VK_LEFT].bPressed && mDirection.x != 1)
			mDirection = { -1, 0 };
		if (m_keys[VK_RIGHT].bPressed && mDirection.x != -1)
			mDirection = { 1, 0 };
		if (m_keys[VK_DOWN].bPressed && mDirection.y != -1)
			mDirection = { 0, 1 };
	}

	virtual bool OnUserUpdate(float fElapsedTime) override
	{
		if (!bGameOver)
		{
			//Refresh the screen
			Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');
			//Input
			inputHandling();
			//Game Logic
			for (size_t i = mSnakeTail.size() - 1; i >= 1; i--)
			{
				mSnakeTail.at(i).x = mSnakeTail.at(i - 1).x;
				mSnakeTail.at(i).y = mSnakeTail.at(i - 1).y;
			}
			mSnakeTail.at(0).x = mHead.x;
			mSnakeTail.at(0).y = mHead.y;

			mHead.x += mSpeedX * mDirection.x;
			mHead.y += mSpeedY * mDirection.y;
			//Teleporting to opposite side when attempting to exit boundries
			if (mHead.x == mapWidth - 1)
				mHead.x = 1;
			if (mHead.x == 0)
				mHead.x = mapWidth - 2;
			if (mHead.y == 0)
				mHead.y = mapHeight - 2;
			if (mHead.y == mapHeight - 1)
				mHead.y = 1;
			//Increase the tail size and spawn new fruit
			if (mHead.x == mFruit.x && mHead.y == mFruit.y)
			{
				mSnakeTail.push_back({ -1, -1 });
				mCurrentScore++;
				mFruit.x = (float)(rand() % (mapWidth - 2) + 1);
				mFruit.y = (float)(rand() % (mapHeight - 2) + 1);
			}
			//Detecting colision with tail
			for (Cartesian& tailSegment : mSnakeTail)
				if (mHead.x == tailSegment.x && mHead.y == tailSegment.y && mCurrentScore != 0)
					bGameOver = true;
			//Drawing on screen
			for (int i = 0; i < mapWidth; i++)
				for (int j = 0; j < mapHeight; j++)
					if (i == 0 || i == mapWidth - 1 || j == 0 || j == mapHeight - 1)
						Draw(i, j, 9608, COLOUR::FG_DARK_GREEN);
			Draw(mHead.x, mHead.y, '@', COLOUR::FG_BLUE);
			for (Cartesian& tailSegment : mSnakeTail)
				Draw(tailSegment.x, tailSegment.y, 'o', COLOUR::FG_BLUE);
			Draw(mFruit.x, mFruit.y, '$', COLOUR::FG_RED);
			DrawString(mapWidth + 1, mapHeight * 1 / 4, L"YOUR SCORE: " + to_wstring(mCurrentScore));

			Sleep(50);
		}
		else //Game over
		{
			DrawString(ScreenWidth() / 4, ScreenHeight() / 2, L"Game Over! Press R to restart", COLOUR::FG_RED);
			if (m_keys['R'].bPressed)
			{
				mDirection = { 0, 0 };
				bGameOver = false;
				mCurrentScore = 0;
				mHead = { (float)mapWidth / 2,(float)mapHeight / 2 };
				mFruit = { (float)(rand() % (mapWidth - 2)) + 2, (float)(rand() % (mapHeight - 2) + 2) };
				mSnakeTail.erase(mSnakeTail.begin(), mSnakeTail.end());
				mSnakeTail.push_back({ mHead.x + 1, mHead.y });
				mSnakeTail.push_back({ mHead.x + 2, mHead.y });
			}
		}
		return true;
	}
private:
	int32_t mapWidth, mapHeight;
	uint32_t mCurrentScore, mHighScore;
	Cartesian mHead, mFruit, mDirection;
	float mSpeedX = 1.0f, mSpeedY = 1.0f;
	vector<Cartesian> mSnakeTail;
	bool bGameOver;
};

int main()
{
	CMD_Snake game;
	if (game.ConstructConsole(80, 60, 15, 15)) //100, 60, 14, 14
		game.Start();
	else cout << "Error, console window is too large!";
	return 0;
}