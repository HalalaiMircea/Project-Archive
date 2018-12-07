#include <string>

using namespace std;

#include "olcConsoleGameEngine.h"

class CMD_Snake : public olcConsoleGameEngine
{
public:
	struct fCartesian {
		float x;
		float y;
	};
	struct iCartesian {
		int x;
		int y;
	};

	CMD_Snake() { m_sAppName = L"CMD_Snake"; }

	virtual bool OnUserCreate() override
	{
		srand(time(NULL));
		bGameOver = false;
		mapWidth = ScreenWidth() * 4 / 5;
		mapHeight = ScreenHeight();
		mHead = { (float)mapWidth / 2,(float)mapHeight / 2 };

		mSnakeTail.push_back({ (int)mHead.x + 1, (int)mHead.y });
		mSnakeTail.push_back({ (int)mHead.x + 2, (int)mHead.y });

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
			Fill(0, 0, ScreenWidth(), ScreenHeight(), ' ');
			//Input
			if (IsFocused())
				inputHandling();
			//Game Logic
			for (size_t i = mSnakeTail.size() - 1; i >= 1; i--)
			{
				mSnakeTail.at(i).x = (int)mSnakeTail.at(i - 1).x;
				mSnakeTail.at(i).y = (int)mSnakeTail.at(i - 1).y;
			}
			mSnakeTail.at(0).x = (int)mHead.x;
			mSnakeTail.at(0).y = (int)mHead.y;

			mHead.x += mSpeedX * mDirection.x * fElapsedTime;
			mHead.y += mSpeedY * mDirection.y * fElapsedTime;
			//Teleporting to opposite side when attempting to exit boundries
			if ((int)mHead.x == mapWidth - 1)
				mHead.x = 1;
			if ((int)mHead.x == 0)
				mHead.x = mapWidth - 2;
			if ((int)mHead.y == 0)
				mHead.y = mapHeight - 2;
			if ((int)mHead.y == mapHeight - 1)
				mHead.y = 1;
			//Increase the tail size and spawn new fruit
			if ((int)mHead.x == (int)mFruit.x && (int)mHead.y == (int)mFruit.y)
			{
				mSnakeTail.push_back({ -1, -1 });
				mCurrentScore++;
				mFruit.x = (float)(rand() % (mapWidth - 2) + 1);
				mFruit.y = (float)(rand() % (mapHeight - 2) + 1);
			}
			//Detecting colision with tail
			for (iCartesian& tailSegment : mSnakeTail)
				if ((int)mHead.x == (int)tailSegment.x && (int)mHead.y == (int)tailSegment.y && mCurrentScore != 0)
					bGameOver = true;
			//Drawing on screen
			for (int i = 0; i < mapWidth; i++)
				for (int j = 0; j < mapHeight; j++)
					if (i == 0 || i == mapWidth - 1 || j == 0 || j == mapHeight - 1)
						Draw(i, j, 9608, COLOUR::FG_DARK_GREEN);		//Draw the walls
			Draw(mHead.x, mHead.y, '@', COLOUR::FG_BLUE);
			for (iCartesian& tailSegment : mSnakeTail)
				Draw(tailSegment.x, tailSegment.y, 'o', COLOUR::FG_BLUE);
			Draw(mFruit.x, mFruit.y, '$', COLOUR::FG_RED);
			DrawString(mapWidth + 1, mapHeight * 1 / 4, L"YOUR SCORE: " + to_wstring(mCurrentScore));

			//Sleep(50);
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
				mSnakeTail.push_back({ (int)mHead.x + 1, (int)mHead.y });
				mSnakeTail.push_back({ (int)mHead.x + 2, (int)mHead.y });
			}
		}
		return true;
	}
private:
	int32_t mapWidth, mapHeight;
	uint32_t mCurrentScore, mHighScore;
	fCartesian mHead, mFruit, mDirection;
	float mSpeedX = 10.0f, mSpeedY = 10.0f;
	vector<iCartesian> mSnakeTail;
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