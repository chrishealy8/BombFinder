#include "olcPixelGameEngine.h"

#define OLC_PGEX_TTF
#include "olcPGEX_TTF.h"

#include "gui_button.h"
#include "gui_label.h"
#include "gui_manager.h"

#include <queue>


//TODO: Seperate game into states


class BombFinder : public olc::PixelGameEngine
{
private:
	gui::Manager AlphaLabelManager;
	gui::Manager NumLabelManager;
	gui::Manager ButtonManager;
	gui::Label* DifficultyLabel;
	gui::Label* BombsLabel;
	gui::Label* BombsRemainNum;
	gui::Label* ResultLabel;
	gui::Label* BestTimeLabel;
	gui::Label* BestTimeNum;
	gui::Label* TimeLabel;
	gui::Label* TimeNum;
	gui::Button* EasyButton;
	gui::Button* MediumButton;
	gui::Button* HardButton;
	gui::Button* RestartButton;

	olc::Font Noto;
	olc::Font Mont;
	olc::Font MontLarge;

	enum class Difficulty
	{
		Easy,
		Medium,
		Hard
	} DifficultyCurrent = Difficulty::Easy;

	struct Cell
	{
		bool IsRevealed = false;
		bool IsFlagged = false;
		bool IsBomb = false;
		int AdjacentBombs = 0;
	};

	std::vector<Cell> Cells;
	std::vector<olc::vi2d> BombPositions;
	std::vector<float> BombDistances;

	olc::vi2d CellSize{ 56, 56 };
	olc::vi2d CellSpacing{ 4, 4 };

	struct Settings
	{
		int Columns;
		int Rows;
		int TotalCells;
		int Bombs;
	};

	Settings Easy
	{
		8, 8, 64,
		10,
	};

	Settings Medium
	{
		16, 16, 256,
		40,
	};

	Settings Hard
	{
		30, 16, 480,
		99,
	};

	Settings Current;

	struct PlayArea
	{
		olc::vi2d Size;
		olc::vi2d Start;
		olc::vi2d End;
	};

	PlayArea AreaEasy;
	PlayArea AreaMedium;
	PlayArea AreaHard;
	PlayArea AreaCurrent;


	olc::Pixel BackgroundColour{ 18, 18, 18 };
	olc::Pixel CellBorderColour{ olc::VERY_DARK_GREY };
	olc::Pixel CellNormalColour{ olc::VERY_DARK_GREY };
	olc::Pixel CellHoveredColour{ olc::DARK_GREY };
	olc::Pixel CellPressedColour{ olc::GREY };
	olc::Pixel CellRevealedColour{ 12, 12, 12 };
	olc::Pixel CellBombColour{ olc::WHITE };

	olc::Renderable Bomb;
	olc::Renderable Mark;

	bool IsNewGame = true;
	bool IsWin = false;
	bool IsLose = false;
	bool IsEndGame = false;
	int CellsRevealed = 0;
	int BombsFlagged = 0;
	float GameTime = 0.0f;
	float PrevTime = 0.0f;
	int DeathIndex = -1;
	olc::vi2d DeathCell;

	float BestTimes[3] = { 999.0f, 999.0f, 999.0f };
	float BestCurrent = 999.0f;

public:
	BombFinder()
	{
		sAppName = "BombFinder";
	}

public:
	bool OnUserCreate() override
	{
		int sW = ScreenWidth();
		int sH = ScreenHeight();

		AreaEasy.Size = { Easy.Columns * CellSize.x + (Easy.Columns - 1) * CellSpacing.x, Easy.Rows * CellSize.y + (Easy.Rows - 1) * CellSpacing.y };
		AreaEasy.Start = { (sW - AreaEasy.Size.x) / 2, (sH - AreaEasy.Size.y) / 2 + 45 };
		AreaEasy.End = { (sW + AreaEasy.Size.x) / 2, (sH + AreaEasy.Size.y) / 2 + 45 };

		AreaMedium.Size = { Medium.Columns * CellSize.x + (Medium.Columns - 1) * CellSpacing.x, Medium.Rows * CellSize.y + (Medium.Rows - 1) * CellSpacing.y };
		AreaMedium.Start = { (sW - AreaMedium.Size.x) / 2, (sH - AreaMedium.Size.y) / 2 + 45 };
		AreaMedium.End = { (sW + AreaMedium.Size.x) / 2, (sH + AreaMedium.Size.y) / 2 + 45 };

		AreaHard.Size = { Hard.Columns * CellSize.x + (Hard.Columns - 1) * CellSpacing.x, Hard.Rows * CellSize.y + (Hard.Rows - 1) * CellSpacing.y };
		AreaHard.Start = { (sW - AreaHard.Size.x) / 2, (sH - AreaHard.Size.y) / 2 + 45 };
		AreaHard.End = { (sW + AreaHard.Size.x) / 2, (sH + AreaHard.Size.y) / 2 + 45 };

		olc::Font::init();
		Noto = olc::Font("./assets/fonts/NotoSansMono-Bold.ttf", 36);
		Mont = olc::Font("./assets/fonts/Montserrat-SemiBoldItalic.ttf", 24);
		MontLarge = olc::Font("./assets/fonts/Montserrat-SemiBoldItalic.ttf", 30);
		AlphaLabelManager.SetFont(&Mont);
		NumLabelManager.SetFont(&Noto);
		ButtonManager.SetFont(&MontLarge);

		DifficultyLabel = new gui::Label(olc::vf2d(AreaHard.Start.x, 15), olc::vf2d(100, 30), "Difficulty", AlphaLabelManager);
		EasyButton = new gui::Button(olc::vf2d(AreaHard.Start.x, 45), olc::vf2d(35, 35), "1", ButtonManager);
		MediumButton = new gui::Button(olc::vf2d(AreaHard.Start.x + 40, 45), olc::vf2d(35, 35), "2", ButtonManager);
		HardButton = new gui::Button(olc::vf2d(AreaHard.Start.x + 80, 45), olc::vf2d(35, 35), "3", ButtonManager);

		BombsLabel = new gui::Label(olc::vf2d(sW / 4 - 50, 15), olc::vf2d(100, 30), "Bombs", AlphaLabelManager, gui::Label::Alignment::Centre);
		BombsRemainNum = new gui::Label(olc::vf2d(sW / 4 - 50, 45), olc::vf2d(100, 30), std::to_string(Current.Bombs), NumLabelManager, gui::Label::Alignment::Centre);

		RestartButton = new gui::Button(olc::vf2d(sW / 2 - 70, 25), olc::vf2d(140, 50), "Restart", ButtonManager);

		ResultLabel = new gui::Label(olc::vf2d(sW / 2 + 100, 40), olc::vf2d(100, 30), "Win", AlphaLabelManager, gui::Label::Alignment::Centre);
		ResultLabel->IsHidden = true;

		BestTimeLabel = new gui::Label(olc::vf2d(3 * sW / 4, 15), olc::vf2d(100, 30), "Best", AlphaLabelManager, gui::Label::Alignment::Right);
		BestTimeNum = new gui::Label(olc::vf2d(3 * sW / 4, 45), olc::vf2d(100, 30), std::to_string(BestCurrent), NumLabelManager, gui::Label::Alignment::Right);

		TimeLabel = new gui::Label(olc::vf2d(sW - 170, 15), olc::vf2d(100, 30), "Time", AlphaLabelManager, gui::Label::Alignment::Right);
		TimeNum = new gui::Label(olc::vf2d(sW - 170, 45), olc::vf2d(100, 30), "0", NumLabelManager, gui::Label::Alignment::Right);

		Bomb.Load("./assets/icons/bomb.png", nullptr, true);
		Mark.Load("./assets/icons/xmark.png", nullptr, true);

		ResetGame(Difficulty::Easy);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{		
		ButtonManager.Update(this);

		if (GetKey(olc::K1).bPressed || EasyButton->IsReleased)
		{
			ResetGame(Difficulty::Easy);
		}
		else if (GetKey(olc::K2).bPressed || MediumButton->IsReleased)
		{
			ResetGame(Difficulty::Medium);
		}
		else if (GetKey(olc::K3).bPressed || HardButton->IsReleased)
		{
			ResetGame(Difficulty::Hard);
		}
		else if (GetKey(olc::SPACE).bPressed || RestartButton->IsReleased)
		{
			ResetGame(DifficultyCurrent);
		}

		// Calculate hovered cell
		int hoveredCellIndex = -1;
		if (!IsEndGame)
		{
			auto& mousePos = GetMousePos();
			olc::vi2d mouseCell;
			if (mousePos.x > AreaCurrent.Start.x && mousePos.y > AreaCurrent.Start.y && mousePos.x < AreaCurrent.End.x && mousePos.y < AreaCurrent.End.y)
			{
				olc::vi2d playAreaMousePos = mousePos - AreaCurrent.Start;
				mouseCell = (playAreaMousePos / (CellSize + CellSpacing)).floor();

				hoveredCellIndex = mouseCell.y * Current.Columns + mouseCell.x;
				auto& hoveredCell = Cells[hoveredCellIndex];

				// Handle mouse clicks
				if (GetMouse(0).bReleased)
				{
					if (IsNewGame)
					{
						Cells.assign(Cells.size(), {});
						GenerateGame(mouseCell);
						IsNewGame = false;
						RevealSafeCells(mouseCell);
					}
					else if (!hoveredCell.IsRevealed && !hoveredCell.IsFlagged)
					{
						if (hoveredCell.IsBomb)
						{
							IsLose = true;
							DeathIndex = hoveredCellIndex;
							DeathCell = mouseCell;
							CalculateBombDistancesFromDeath();
						}
						else if (hoveredCell.AdjacentBombs == 0)
						{
							RevealSafeCells(mouseCell);
						}
						else
						{
							hoveredCell.IsRevealed = true;
							++CellsRevealed;
						}
					}
				}

				else if (GetMouse(1).bReleased && !IsNewGame && !hoveredCell.IsRevealed)
				{
					if (hoveredCell.IsFlagged)
					{
						hoveredCell.IsFlagged = false;
						--BombsFlagged;
					}
					else
					{
						hoveredCell.IsFlagged = true;
						++BombsFlagged;
					}
					BombsRemainNum->SetText(std::to_string(Current.Bombs - BombsFlagged));
				}
			}
		}

		// Determine win state
		if (!IsLose && CellsRevealed == Current.TotalCells - Current.Bombs)
		{
			IsWin = true;

			if (GameTime < BestCurrent)
			{
				BestCurrent = GameTime;
				switch (DifficultyCurrent)
				{
				case Difficulty::Easy:
					BestTimes[0] = GameTime;
					break;
				case Difficulty::Medium:
					BestTimes[1] = GameTime;
					break;
				case Difficulty::Hard:
					BestTimes[2] = GameTime;
					break;
				}
				BestTimeNum->SetText(std::to_string(int(BestCurrent)));
			}
		}
		
		IsEndGame = IsWin || IsLose;
		if (!IsEndGame && !IsNewGame)
		{
			PrevTime = GameTime;
			GameTime += fElapsedTime;
			if (int(GameTime) != int(PrevTime))
				TimeNum->SetText(std::to_string(int(GameTime)));
		}
		else if (IsEndGame && ResultLabel->IsHidden)
		{
			ResultLabel->SetText(IsWin ? "WIN" : "LOSE");
			ResultLabel->IsHidden = false;
		}


		Clear(BackgroundColour);

		AlphaLabelManager.Draw(this);
		NumLabelManager.Draw(this);
		ButtonManager.Draw(this);

		// Draw cells
		for (int i = 0; i < Cells.size(); i++)
		{
			olc::Pixel colour;
			std::u32string text = U"";
			int row = i / Current.Columns;
			int column = i % Current.Columns;
			olc::vi2d position = AreaCurrent.Start + olc::vi2d{ column * (CellSize.x + CellSpacing.x), row * (CellSize.y + CellSpacing.y) };
			float decalScale = 1.0f;
			olc::vi2d textPos = position + olc::vi2d{ 18, 40 };

			if (Cells[i].IsRevealed)
			{
				if (Cells[i].AdjacentBombs > 0)
				{
					std::string s = std::to_string(Cells[i].AdjacentBombs);
					text = std::u32string(s.begin(), s.end());
					FillRect(position, CellSize, CellRevealedColour);
					Noto.DrawString(text, textPos, GetAdjacentBombsColour(Cells[i].AdjacentBombs));
				}
				else
				{
					FillRect(position, CellSize, CellRevealedColour);
				}
				DrawRect(position, CellSize, CellBorderColour);
			}
			else
			{
				if (i == hoveredCellIndex)
				{
					if (GetMouse(0).bHeld || GetMouse(1).bHeld)
					{
						colour = CellPressedColour;
					}
					else
					{
						colour = CellHoveredColour;
					}
				}
				else
				{
					colour = CellNormalColour;
				}
				FillRect(position, CellSize, colour);

				if (Cells[i].IsFlagged)
				{
					DrawDecal(position, Mark.Decal(), { decalScale, decalScale });
				}
				DrawRect(position, CellSize, CellBorderColour);
			}

			if (i == DeathIndex)
			{
				SetDecalMode(olc::DecalMode::ADDITIVE);
				FillRectDecal(position, CellSize + olc::vf2d(1, 1), olc::RED);
				SetDecalMode(olc::DecalMode::NORMAL);
			}
		}

		if (IsLose)
		{
			PulseBombs(DeathCell, fElapsedTime);
		}

		return true;
	}

	void ResetGame(Difficulty diff)
	{
		//TODO: Fix grouped button selection hack
		EasyButton->IsSelected = false;
		MediumButton->IsSelected = false;
		HardButton->IsSelected = false;

		switch (diff)
		{
		case Difficulty::Easy:
			Current = Easy;
			AreaCurrent = AreaEasy;
			BestCurrent = BestTimes[0];
			EasyButton->IsSelected = true;
			break;

		case Difficulty::Medium:
			Current = Medium;
			AreaCurrent = AreaMedium;
			BestCurrent = BestTimes[1];
			MediumButton->IsSelected = true;
			break;

		case Difficulty::Hard:
			Current = Hard;
			AreaCurrent = AreaHard;
			BestCurrent = BestTimes[2];
			HardButton->IsSelected = true;
			break;
		}

		IsNewGame = true;
		IsWin = false;
		IsLose = false;
		DeathIndex = -1;
		CellsRevealed = 0;
		BombsFlagged = 0;
		BombsRemainNum->SetText(std::to_string(Current.Bombs - BombsFlagged));
		GameTime = 0.0f;
		TimeNum->SetText(std::to_string(int(GameTime)));
		BestTimeNum->SetText(std::to_string(int(BestCurrent)));
		ResultLabel->IsHidden = true;
		Cells.resize(Current.TotalCells);
		Cells.assign(Cells.size(), {});

		DifficultyCurrent = diff;
	}

	void SeedBombs(olc::vi2d selectedCell)
	{
		BombPositions.clear();
		BombPositions.reserve(Current.Bombs);
		std::srand(std::time(nullptr));

		while (BombPositions.size() < Current.Bombs)
		{
			int x = std::rand() % Current.Columns;
			int y = std::rand() % Current.Rows;
			int index = y * Current.Columns + x;
			Cell& cell = Cells.at(index);

			// Selected cell and all adjacent cells must not be bombs
			if (!cell.IsBomb && (std::abs(x - selectedCell.x) > 1 || std::abs(y - selectedCell.y) > 1))
			{
				cell.IsBomb = true;
				BombPositions.emplace_back(x, y);
			}
		}
	}

	void CalculateAdjacentBombs()
	{
		for (olc::vi2d& bombPos : BombPositions)
		{
			for (int yOffset = -1; yOffset <= 1; yOffset++)
			{
				for (int xOffset = -1; xOffset <= 1; xOffset++)
				{
					if (xOffset == 0 && yOffset == 0)
						continue;

					int adjacentX = bombPos.x + xOffset;
					int adjacentY = bombPos.y + yOffset;
					int adjacentIndex = adjacentY * Current.Columns + adjacentX;
					if (adjacentX >= 0 && adjacentX < Current.Columns && adjacentY >= 0 && adjacentY < Current.Rows)
					{
						++Cells.at(adjacentIndex).AdjacentBombs;
					}
				}
			}
		}
	}

	void GenerateGame(olc::vi2d selectedCell)
	{
		SeedBombs(selectedCell);
		CalculateAdjacentBombs();
	}

	void RevealSafeCells(olc::vi2d selectedCell)
	{
		// Flood fill algorithm
		std::queue<olc::vi2d> cellQueue;
		cellQueue.push(selectedCell);

		while (!cellQueue.empty())
		{
			olc::vi2d cell = cellQueue.front();
			cellQueue.pop();

			if (cell.x < 0 || cell.x >= Current.Columns || cell.y < 0 || cell.y >= Current.Rows)
			{
				continue;
			}

			int index = cell.y * Current.Columns + cell.x;
			if (Cells.at(index).IsRevealed || Cells.at(index).IsBomb)
			{
				continue;
			}

			Cells.at(index).IsRevealed = true;
			++CellsRevealed;

			if (Cells.at(index).AdjacentBombs != 0)
			{
				continue;
			}

			cellQueue.push(cell + olc::vi2d{ -1, -1 });
			cellQueue.push(cell + olc::vi2d{ 0, -1 });
			cellQueue.push(cell + olc::vi2d{ 1, -1 });
			cellQueue.push(cell + olc::vi2d{ -1,  0 });
			cellQueue.push(cell + olc::vi2d{ 1,  0 });
			cellQueue.push(cell + olc::vi2d{ -1,  1 });
			cellQueue.push(cell + olc::vi2d{ 0,  1 });
			cellQueue.push(cell + olc::vi2d{ 1,  1 });
		}
	}

	// Return a tint of grey/white based on the number of adjacent bombs
	// TODO: Move this to a utility class
	olc::Pixel GetAdjacentBombsColour(int adjacentBombs)
	{
		olc::Pixel baseColour = olc::VERY_DARK_GREY;
		return baseColour + olc::Pixel{ 48, 48, 48 } * adjacentBombs;
	}

	void CalculateBombDistancesFromDeath()
	{
		BombDistances.clear();
		BombDistances.reserve(BombPositions.size());

		for (auto& pos : BombPositions)
		{
			float distance = std::sqrt(std::pow(pos.x - DeathCell.x, 2) + std::pow(pos.y - DeathCell.y, 2));
			BombDistances.push_back(-distance);
		}
	}

	// Pulse the size of the bomb decals at a time based on the distance from the selected bomb cell
	void PulseBombs(olc::vi2d selectedCell, float time)
	{
		float pulsePropogationSpeed = 10.0f;
		float pulseStartTime = 0.0f;
		float pulseEndTime = 0.4f * pulsePropogationSpeed;
		float pulseDuration = pulseEndTime - pulseStartTime;
		float pulseMidTime = pulseStartTime + pulseDuration / 2.0f;
		float growthFactor = 0.4f;

		for (float& distance : BombDistances)
			distance += (time * pulsePropogationSpeed);

		for (int i = 0; i < BombPositions.size(); i++)
		{
			float pTime = BombDistances.at(i);
			float scale;
			if (pTime >= pulseStartTime && pTime < pulseEndTime)
			{
				if (pTime < pulseMidTime)
				{
					scale = 1.0f + (pTime - pulseStartTime) / pulseDuration * growthFactor;
				}
				else
				{
					scale = 1.0f + (pulseEndTime - pTime) / pulseDuration * growthFactor;
				}
			}
			else
			{
				scale = 1.0f;
			}

			olc::vi2d position = AreaCurrent.Start + olc::vi2d{ BombPositions.at(i).x * (CellSize.x + CellSpacing.x), BombPositions.at(i).y * (CellSize.y + CellSpacing.y) };
			olc::vf2d decalSize = olc::vf2d(CellSize) * scale;
			olc::vf2d decalPosition = position + (olc::vf2d(CellSize) - decalSize) / 2.0f; // Center decal in cell

			FillRect(position, CellSize, CellBombColour);
			DrawDecal(decalPosition, Bomb.Decal(), { scale, scale });

			// Draw red tint over the bomb that killed the player
			if (BombPositions.at(i) == selectedCell)
			{
				SetDecalMode(olc::DecalMode::ADDITIVE);
				FillRectDecal(position, CellSize, olc::RED);
				SetDecalMode(olc::DecalMode::NORMAL);
			}
		}
	}
};


int main()
{
	BombFinder app;
	if (app.Construct(1920, 1080, 1, 1))
		app.Start();

	return 0;
}
