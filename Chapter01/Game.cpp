// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

const int thickness = 15;
const float paddleH = 100.0f;
const int ballCount = 2;

Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mTicksCount(0)
,mIsRunning(true)
,mPlayerOnePaddleDir(0)
,mPlayerTwoPaddleDir(0)
{

}

bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	
	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Chapter 1)", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set)
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	
	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	//players
	mPlayerOnePaddlePos.x = 10.0f;
	mPlayerOnePaddlePos.y = 768.0f/2.0f;

	mPlayerTwoPaddlePos.x = 1024 - thickness - 10;
	mPlayerTwoPaddlePos.y = 768.0f / 2.0f;

	_balls.reserve(ballCount);
	//init balls
	for (int i = 0; i < ballCount; i++)
	{
		_balls.push_back(Ball());
		_balls[i].mBallPos.x = 1024.0f / 2.0f;
		_balls[i].mBallPos.y = 768.0f / 2.0f;
		_balls[i].mBallVel.x = -100.0f*(i+1);
		_balls[i].mBallVel.y = 235.0f;
	}

	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	
	// Update paddle direction based on W/S keys
	mPlayerOnePaddleDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPlayerOnePaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPlayerOnePaddleDir += 1;
	}

	mPlayerTwoPaddleDir = 0;
	if (state[SDL_SCANCODE_I])
	{
		mPlayerTwoPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_K])
	{
		mPlayerTwoPaddleDir += 1;
	}
}

void Game::UpdateGame()
{
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();
	
	// Update paddle position based on direction
	if (mPlayerOnePaddleDir != 0)
	{
		mPlayerOnePaddlePos.y += mPlayerOnePaddleDir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mPlayerOnePaddlePos.y < (paddleH/2.0f + thickness))
		{
			mPlayerOnePaddlePos.y = paddleH/2.0f + thickness;
		}
		else if (mPlayerOnePaddlePos.y > (768.0f - paddleH/2.0f - thickness))
		{
			mPlayerOnePaddlePos.y = 768.0f - paddleH/2.0f - thickness;
		}
	}

	// Update paddle position based on direction
	if (mPlayerTwoPaddleDir != 0)
	{
		mPlayerTwoPaddlePos.y += mPlayerTwoPaddleDir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mPlayerTwoPaddlePos.y < (paddleH / 2.0f + thickness))
		{
			mPlayerTwoPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (mPlayerTwoPaddlePos.y > (768.0f - paddleH / 2.0f - thickness))
		{
			mPlayerTwoPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}
	
	for (Ball &ball : _balls)
	{
		// Update ball position based on ball velocity
		ball.mBallPos.x += ball.mBallVel.x * deltaTime;
		ball.mBallPos.y += ball.mBallVel.y * deltaTime;

		// Bounce if needed
		// Did we intersect with the paddle?
		float playerAdiff = mPlayerOnePaddlePos.y - ball.mBallPos.y;
		float playerBdiff = mPlayerTwoPaddlePos.y - ball.mBallPos.y;
		// Take absolute value of difference
		playerAdiff = (playerAdiff > 0.0f) ? playerAdiff : -playerAdiff;
		playerBdiff = (playerBdiff > 0.0f) ? playerBdiff : -playerBdiff;
		if (
			// Our y-difference is small enough
			playerAdiff <= paddleH / 2.0f &&
			// We are in the correct x-position
			ball.mBallPos.x <= 25.0f && ball.mBallPos.x >= 20.0f &&
			// The ball is moving to the left
			ball.mBallVel.x < 0.0f)
		{
			ball.mBallVel.x *= -1.0f;
		}
		else if (
			// Our y-difference is small enough
			playerBdiff <= paddleH / 2.0f &&
			// We are in the correct x-position
			ball.mBallPos.x <= 999.0f && ball.mBallPos.x >= 990.0f &&
			// The ball is moving to the rightiiiiii
			ball.mBallVel.x > 0.0f)
		{
			ball.mBallVel.x *= -1.0f;
		}
		// Did the ball go off the screen? (if so, end game)
		else if (ball.mBallPos.x <= 0.0f)
		{
			mIsRunning = false;
		}
		else if (ball.mBallPos.x >= 1024.0f)
		{
			mIsRunning = false;
		}

		// Did the ball collide with the top wall?
		if (ball.mBallPos.y <= thickness && ball.mBallVel.y < 0.0f)
		{
			ball.mBallVel.y *= -1;
		}
		// Did the ball collide with the bottom wall?
		else if (ball.mBallPos.y >= (768 - thickness) &&
			ball.mBallVel.y > 0.0f)
		{
			ball.mBallVel.y *= -1;
		}
	}
}

void Game::GenerateOutput()
{
	// Set draw color to blue
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		255,	// B
		255		// A
	);
	
	// Clear back buffer
	SDL_RenderClear(mRenderer);

	// Draw walls
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	
	// Draw top wall
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw bottom wall
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw paddles
	SDL_Rect playerOne{
		static_cast<int>(mPlayerOnePaddlePos.x),
		static_cast<int>(mPlayerOnePaddlePos.y - paddleH/2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &playerOne);

	SDL_Rect playerTwo{
	static_cast<int>(mPlayerTwoPaddlePos.x),
	static_cast<int>(mPlayerTwoPaddlePos.y - paddleH / 2),
	thickness,
	static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &playerTwo);
	
	for (Ball &ball : _balls)
	{
		// Draw balls
		SDL_Rect drawnball{
			static_cast<int>(ball.mBallPos.x - thickness / 2),
			static_cast<int>(ball.mBallPos.y - thickness / 2),
			thickness,
			thickness
		};
		SDL_RenderFillRect(mRenderer, &drawnball);
	}
	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}
