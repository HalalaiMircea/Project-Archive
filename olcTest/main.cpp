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
	struct SnakeSegmentInfo {
		Cartesian pos;
		Cartesian direction;
	};
public:
	CMD_Snake() { m_sAppName = L"CMD_Snake"; }

	virtual bool OnUserCreate() override
	{
		srand(time(NULL));
		bGameOver = false;
		mapWidth = ScreenWidth() * 4 / 5;
		mapHeight = ScreenHeight();
		mHead = { (float)mapWidth / 2, (float)mapHeight / 2, -1, 0 };

		mTail.push_back({ mHead.pos.x + 1, mHead.pos.y , -1, 0 });
		mTail.push_back({ mHead.pos.x + 2, mHead.pos.y , -1, 0 });

		mFruit = { (float)(rand() % (mapWidth - 2)) + 1, (float)(rand() % (mapHeight - 2) + 1) };
		mCurrentScore = 0;
		return true;
	}

	void inputHandling()
	{
		if (IsFocused())
		{
			if (m_keys[VK_UP].bPressed && mHead.direction.y != 1)
				mHead.direction = { 0, -1 };
			if (m_keys[VK_LEFT].bPressed && mHead.direction.x != 1)
				mHead.direction = { -1, 0 };
			if (m_keys[VK_RIGHT].bPressed && mHead.direction.x != -1)
				mHead.direction = { 1, 0 };
			if (m_keys[VK_DOWN].bPressed && mHead.direction.y != -1)
				mHead.direction = { 0, 1 };
		}
	}

	void gameLogic()
	{
		//Teleporting to opposite side when attempting to exit boundries
		if ((int)mHead.pos.x == mapWidth - 1)
			mHead.pos.x = 1;
		if ((int)mHead.pos.x == 0)
			mHead.pos.x = mapWidth - 2;
		if ((int)mHead.pos.y == 0)
			mHead.pos.y = mapHeight - 2;
		if ((int)mHead.pos.y == mapHeight - 1)
			mHead.pos.y = 1;
		//Increase the tail size and spawn new fruit
		if ((int)mHead.pos.x == (int)mFruit.x && (int)mHead.pos.y == (int)mFruit.y)
		{
			mTail.push_back({ -1, -1, 0, 0 });
			mCurrentScore++;
			mFruit.x = (float)(rand() % (mapWidth - 2) + 1);
			mFruit.y = (float)(rand() % (mapHeight - 2) + 1);
		}
		//Detecting colision with tail
		for (SnakeSegmentInfo& tailSegment : mTail)
			if (mHead.pos.x == tailSegment.pos.x && mHead.pos.y == tailSegment.pos.y && mCurrentScore != 0)
				bGameOver = true;
	}

	void updatePositions(float fElapsedTime)
	{
		//Copy direction for each tail segment from the previous one
		/*mTail.at(0).direction.x = mHead.direction.x;
		mTail.at(0).direction.y = mHead.direction.y;
		for (size_t i = 0; i <= mTail.size() - 2; i++)
		{
			mTail.at(i + 1).direction.x = mTail.at(i).direction.x;
			mTail.at(i + 1).direction.y = mTail.at(i).direction.y;
		}*/
		for (size_t i = mTail.size() - 1; i >= 1; i--)
		{
			mTail.at(i).direction.x = mTail.at(i - 1).direction.x;
			mTail.at(i).direction.y = mTail.at(i - 1).direction.y;
		}
		mTail.at(0).direction.x = mHead.direction.x;
		mTail.at(0).direction.y = mHead.direction.y;

		for (size_t i = 0; i < mTail.size(); i++)
		{
			mTail.at(i).pos.x += SPEED.x * mTail.at(i).direction.x * fElapsedTime;
			mTail.at(i).pos.y += SPEED.y * mTail.at(i).direction.y * fElapsedTime;
		}
		mHead.pos.x += SPEED.x * mHead.direction.x * fElapsedTime;
		mHead.pos.y += SPEED.y * mHead.direction.y * fElapsedTime;
	}

	void drawOnScreen()
	{
		//Refresh the screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');
		//Drawing on screen
		for (int i = 0; i < mapWidth; i++)
			for (int j = 0; j < mapHeight; j++)
				if (i == 0 || i == mapWidth - 1 || j == 0 || j == mapHeight - 1)
					Draw(i, j, 9608, COLOUR::FG_DARK_GREEN);
		Draw((int)mHead.pos.x, (int)mHead.pos.y, '@', COLOUR::FG_BLUE);
		for (SnakeSegmentInfo& tailSegment : mTail)
			Draw((int)tailSegment.pos.x, (int)tailSegment.pos.y, 'o', COLOUR::FG_BLUE);
		Draw(mFruit.x, mFruit.y, '$', COLOUR::FG_RED);
		DrawString(mapWidth + 1, mapHeight * 1 / 4, L"YOUR SCORE: " + to_wstring(mCurrentScore));

		//Sleep(50);
	}
	//Game Loop
	virtual bool OnUserUpdate(float fElapsedTime) override
	{
		if (!bGameOver)
		{
			inputHandling();
			gameLogic();
			updatePositions(fElapsedTime);
			drawOnScreen();
		}
		else //Game over
		{
			DrawString(ScreenWidth() / 4, ScreenHeight() / 2, L"Game Over! Press R to restart", COLOUR::FG_RED);
			if (m_keys['R'].bPressed)
			{
				bGameOver = false;
				mCurrentScore = 0;
				mHead = { (float)mapWidth / 2,(float)mapHeight / 2, -1, 0 };
				mFruit = { (float)(rand() % (mapWidth - 2)) + 2, (float)(rand() % (mapHeight - 2) + 2) };
				mTail.erase(mTail.begin(), mTail.end());
				mTail.push_back({ mHead.pos.x + 1, mHead.pos.y , -1, 0 });
				mTail.push_back({ mHead.pos.x + 2, mHead.pos.y , -1, 0 });
			}
		}
		return true;
	}
private:
	int32_t mapWidth, mapHeight;
	uint32_t mCurrentScore, mHighScore;
	Cartesian mFruit;
	const Cartesian SPEED = { 10.0f, 10.0f };
	SnakeSegmentInfo mHead;
	vector<SnakeSegmentInfo> mTail;
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