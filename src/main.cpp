#include <json.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>

#include "texture.h"
#include "bounds.h"
#include "tinyfiledialogs.h"

#undef main

using json = nlohmann::json;

void drawLines(boundingBox box, SDL_Renderer* renderer);
void drawGrid(SDL_Renderer* renderer);
void drawCube(int type, int angle, int x, int y, SDL_Renderer* renderer);

int main(int argc, char* argv[]) {
	bool running = true;
	std::string loc;

	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	IMG_Init(IMG_INIT_PNG);

	SDL_Window* window = SDL_CreateWindow("SDL_WINDOW", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 540, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_RenderSetLogicalSize(renderer, 1920, 1080);
	SDL_RaiseWindow(window);

	TTF_Font* font = TTF_OpenFont("assets/FreeSans.ttf", 15);
	SDL_Color color = { 0, 0, 0 };

	texture loadFile("Load File", 100, renderer);
	texture newFile("New File", 100, renderer);

	SDL_Point mousePt = { 0, 0 };
	SDL_Rect newFileBox = { 1920 / 2 - newFile.getWidth() / 2, 100, newFile.getWidth(), newFile.getHeight() };
	SDL_Rect loadFileBox = { 1920 / 2 - loadFile.getWidth() / 2, 100 + newFileBox.h + 100, loadFile.getWidth(), loadFile.getHeight() };

	bool isInMenu = true;
	while (isInMenu) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				isInMenu = false;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					running = false;
					break;
				}
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT) {
					if (SDL_PointInRect(&mousePt, &loadFileBox)) {
						const char* fileTypes[1] = {"*.json"};
						loc = tinyfd_openFileDialog("Select a level", "", 1, fileTypes, "", 0);
						isInMenu = false;
					}
					if (SDL_PointInRect(&mousePt, &newFileBox)) {
						isInMenu = false;
					}
				}
				break;
			case SDL_MOUSEMOTION:
				mousePt.x = event.button.x;
				mousePt.y = event.button.y;
				break;
			}
		}
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderClear(renderer);

			
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &newFileBox);
			SDL_RenderFillRect(renderer, &loadFileBox);

			newFile.draw(1920 / 2 - newFile.getWidth() / 2, 100, 0);
			loadFile.draw(1920 / 2 - loadFile.getWidth() / 2, 100 + newFile.getHeight() + 100, 0);

			SDL_RenderPresent(renderer);
	}

	SDL_RaiseWindow(window);


	int x = 0, y = 0, xstrt = 0, ystrt = 0;
	int currentCubeType = 0;

	bool showLines = false;
	bool showGrid = false;

	boundingBox currentCube;
	int currentRot = 0;

	std::vector<boundingBox> cubesPlaced;
	std::vector<int> cubesPlacedType;
	std::vector<int> cubesPlacedRotation;

	json lvlJson;

	if (!loc.empty()) {
		std::ifstream lvlStream(loc);
		lvlStream >> lvlJson;
		try {
			cubesPlaced.push_back(boundingBox(lvlJson["Spawn_Loc"][0], lvlJson["Spawn_Loc"][1], 64, 64));
			cubesPlacedType.push_back(6);
			cubesPlaced.push_back(boundingBox(lvlJson["End_Loc"][0], lvlJson["End_Loc"][1], 64, 64));
			cubesPlacedType.push_back(7);
			cubesPlacedRotation.push_back(0);
			cubesPlacedRotation.push_back(0);

			for (auto pt : lvlJson["map1"]["Blue_Cube"]) {
				cubesPlaced.push_back(boundingBox(pt[0], pt[1], 64, 64));
				cubesPlacedRotation.push_back(pt[2]);
				cubesPlacedType.push_back(0);
			}
			for (auto pt : lvlJson["map1"]["Blue_Spike"]) {
				cubesPlaced.push_back(boundingBox(pt[0], pt[1], 64, 32));
				cubesPlacedRotation.push_back(pt[2]);
				cubesPlacedType.push_back(1);
			}
			for (auto pt : lvlJson["map2"]["Red_Cube"]) {
				cubesPlaced.push_back(boundingBox(pt[0], pt[1], 64, 64));
				cubesPlacedRotation.push_back(pt[2]);
				cubesPlacedType.push_back(2);
			}
			for (auto pt : lvlJson["map2"]["Red_Spike"]) {
				cubesPlaced.push_back(boundingBox(pt[0], pt[1], 64, 32));
				cubesPlacedRotation.push_back(pt[2]);
				cubesPlacedType.push_back(3);
			}
			for (auto pt : lvlJson["map1"]["Neutral_Cube"]) {
				cubesPlaced.push_back(boundingBox(pt[0], pt[1], 64, 64));
				cubesPlacedRotation.push_back(pt[2]);
				cubesPlacedType.push_back(4);
			}
			for (auto pt : lvlJson["map1"]["Neutral_Spike"]) {
				cubesPlaced.push_back(boundingBox(pt[0], pt[1], 64, 32));
				cubesPlacedRotation.push_back(pt[2]);
				cubesPlacedType.push_back(5);
			}
			for (auto pt : lvlJson["map1"]["Neutral_Jump"]) {
				cubesPlaced.push_back(boundingBox(pt[0], pt[1], 64, 64));
				cubesPlacedRotation.push_back(pt[2]);
				cubesPlacedType.push_back(10);
			}
			for (auto pt : lvlJson["map1"]["Blue_Jump"]) {
				cubesPlaced.push_back(boundingBox(pt[0], pt[1], 64, 64));
				cubesPlacedRotation.push_back(pt[2]);
				cubesPlacedType.push_back(8);
			}
			for (auto pt : lvlJson["map2"]["Red_Jump"]) {
				cubesPlaced.push_back(boundingBox(pt[0], pt[1], 64, 64));
				cubesPlacedRotation.push_back(pt[2]);
				cubesPlacedType.push_back(9);
			}
		}
		catch (...) {
			cubesPlaced.clear();
			cubesPlacedType.clear();
			cubesPlacedRotation.clear();

			texture text("Invalid Level... overwrite file?", 100, renderer);
			texture yes("YES", 100, renderer);
			texture no("NO", 100, renderer);

			SDL_Point mousePt = { 0, 0 };
			SDL_Rect yesBox = { 1920 / 2 - yes.getWidth() / 2, 300, yes.getWidth(), yes.getHeight() };
			SDL_Rect noBox = { 1920 / 2 - no.getWidth() / 2, 300 + yesBox.h + 100, no.getWidth(), no.getHeight() };

			bool isInMenu = true;
			while (isInMenu) {
				SDL_Event event;
				while (SDL_PollEvent(&event)) {
					switch (event.type) {
					case SDL_QUIT:
						running = false;
						isInMenu = false;
						break;
					case SDL_KEYDOWN:
						switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
							running = false;
							break;
						}
					case SDL_MOUSEBUTTONDOWN:
						if (event.button.button == SDL_BUTTON_LEFT) {
							if (SDL_PointInRect(&mousePt, &yesBox)) {
								isInMenu = false;
							}
							if (SDL_PointInRect(&mousePt, &noBox)) {
								loc.clear();
								isInMenu = false;
							}
						}
						break;
					case SDL_MOUSEMOTION:
						mousePt.x = event.button.x;
						mousePt.y = event.button.y;
						break;
					}
				}
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				SDL_RenderClear(renderer);


				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
				SDL_RenderFillRect(renderer, &noBox);
				SDL_RenderFillRect(renderer, &yesBox);

				text.draw(1920 / 2 - text.getWidth() / 2, 50, 0);
				yes.draw(yesBox.x, yesBox.y, 0);
				no.draw(noBox.x, noBox.y, 0);

				SDL_RenderPresent(renderer);
			}
		}
	}

	if(running)	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

	SDL_Surface* surf1 = TTF_RenderText_Solid(font, ("x = " + std::to_string(x - xstrt)).c_str(), color);
	SDL_Texture* text1 = SDL_CreateTextureFromSurface(renderer, surf1);

	int textWidth1 = 0;
	int textHeight1 = 0;
	SDL_QueryTexture(text1, NULL, NULL, &textWidth1, &textHeight1);
	SDL_Rect textLoc1 = { 0, 0, textWidth1, textHeight1 };

	SDL_Surface* surf2 = TTF_RenderText_Solid(font, ("y = " + std::to_string(y - ystrt)).c_str(), color);
	SDL_Texture* text2 = SDL_CreateTextureFromSurface(renderer, surf1);

	int textWidth2 = 0;
	int textHeight2 = 0;
	SDL_QueryTexture(text2, NULL, NULL, &textWidth2, &textHeight2);
	SDL_Rect textLoc2 = { 0, 0, textWidth2, textHeight2 };

	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					running = false;
					break;
				case SDLK_LSHIFT:
					showLines = true;
					ystrt = y;
					xstrt = x;
					break;
				case SDLK_LCTRL:
					showGrid = !showGrid;
					break;
				case SDLK_w:
					if (showLines) {
						y--;
					}
					break;
				case SDLK_s:
					if (showLines) {
						y++;
					}
					break;
				case SDLK_a:
					if (showLines) {
						x--;
					}
					break;
				case SDLK_d:
					if (showLines) {
						x++;
					}
					break;
				case SDLK_LEFT:
					currentRot += 90;
					currentRot %= 360;
					break;
				case SDLK_RIGHT:
					currentRot -= 90;
					if (currentRot < 0) currentRot = 360 + currentRot;
					break;
				}
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.sym) {
				case SDLK_LSHIFT:
					showLines = false;
					SDL_WarpMouseInWindow(window, x, y);
					break;
				}
				break;
			case SDL_MOUSEMOTION:
				if (!showLines) {
					if (showGrid) {
						x = int(event.motion.x / 64) * 64;
						y = int(event.motion.y / 64) * 64;
					}
					else {
						x = event.motion.x;
						y = event.motion.y;
					}
				}
				break;
			case SDL_MOUSEWHEEL:
				currentCubeType += -event.wheel.y;
				currentCubeType %= 11;
				if (currentCubeType < 0) currentCubeType = 10;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_MIDDLE && !std::isnan(boxVbundle(cubesPlaced, boundingBox(x, y, 64, 64)))) {
					currentCubeType = cubesPlacedType[boxVbundle(cubesPlaced, boundingBox(x, y, 64, 64))];
					currentRot = cubesPlacedRotation[boxVbundle(cubesPlaced, boundingBox(x, y, 64, 64))];
				}
				if (event.button.button == SDL_BUTTON_LEFT && std::isnan(boxVbundle(cubesPlaced, boundingBox(x, y, 64, 64)))) {
					cubesPlaced.push_back(boundingBox(x, y, 64, 64));
					cubesPlacedType.push_back(currentCubeType);
					cubesPlacedRotation.push_back(currentRot);
				}
				else if (event.button.button == SDL_BUTTON_RIGHT && !std::isnan(boxVbundle(cubesPlaced, boundingBox(x, y, 64, 64)))) {
					cubesPlacedType.erase(cubesPlacedType.begin() + boxVbundle(cubesPlaced, boundingBox(x, y, 64, 64)));
					cubesPlacedRotation.erase(cubesPlacedRotation.begin() + boxVbundle(cubesPlaced, boundingBox(x, y, 64, 64)));
					cubesPlaced.erase(cubesPlaced.begin() + boxVbundle(cubesPlaced, boundingBox(x, y, 64, 64)));
				}
				break;
			}
		}

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		if (showGrid) {
			SDL_SetRenderDrawColor(renderer, 211, 211, 211, 255);
			drawGrid(renderer);
		}

		for (int i = 0; i < cubesPlaced.size(); i++) {
			if (showLines) {
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				drawLines(cubesPlaced[i], renderer);
			}
			drawCube(cubesPlacedType[i], cubesPlacedRotation[i], cubesPlaced[i].getSDLrect().x, cubesPlaced[i].getSDLrect().y, renderer);
		}

		if (showLines) {
			SDL_FreeSurface(surf1);
			SDL_FreeSurface(surf2);
			SDL_DestroyTexture(text1);
			SDL_DestroyTexture(text2);
			surf1 = TTF_RenderText_Solid(font, ("x = " + std::to_string(x - xstrt)).c_str(), color);
			text1 = SDL_CreateTextureFromSurface(renderer, surf1);

			SDL_QueryTexture(text1, NULL, NULL, &textWidth1, &textHeight1);
			textLoc1.w = textWidth1;
			textLoc1.h = textHeight1;

			surf2 = TTF_RenderText_Solid(font, ("y = " + std::to_string(y - ystrt)).c_str(), color);
			text2 = SDL_CreateTextureFromSurface(renderer, surf2);

			SDL_QueryTexture(text2, NULL, NULL, &textWidth2, &textHeight2);
			textLoc2.w = textWidth2;
			textLoc2.h = textHeight2;
			textLoc2.y = textLoc1.h + 10;

			SDL_RenderCopy(renderer, text1, NULL, &textLoc1);
			SDL_RenderCopy(renderer, text2, NULL, &textLoc2);

			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			drawLines(boundingBox(x, y, 64, 64), renderer);
		}

		drawCube(currentCubeType, currentRot, x, y, renderer);

		SDL_RenderPresent(renderer);
	}

	json file;
	file["Spawn_Loc"] = json::array();
	file["End_Loc"] = json::array();

	file["map1"] = json::object();
	file["map2"] = json::object();

	file["map1"]["Blue_Cube"] = json::array();
	file["map1"]["Blue_Spike"] = json::array();
	file["map1"]["Blue_Jump"] = json::array();
	file["map1"]["Neutral_Spike"] = json::array();
	file["map1"]["Neutral_Cube"] = json::array();
	file["map1"]["Neutral_Jump"] = json::array();

	file["map2"]["Red_Cube"] = json::array();
	file["map2"]["Red_Spike"] = json::array();
	file["map2"]["Red_Jump"] = json::array();
	
	file["map2"]["Neutral_Spike"] = json::array();
	file["map2"]["Neutral_Cube"] = json::array();

	for (int i = 0; i < cubesPlaced.size(); i++) {
		switch (cubesPlacedType[i]) {
		case 0:
			file["map1"]["Blue_Cube"].push_back({cubesPlaced[i].getSDLrect().x, cubesPlaced[i].getSDLrect().y, cubesPlacedRotation[i]});
			break;
		case 1:
			file["map1"]["Blue_Spike"].push_back({cubesPlaced[i].getSDLrect().x, cubesPlaced[i].getSDLrect().y, cubesPlacedRotation[i] });
			break;
		case 2:
			file["map2"]["Red_Cube"].push_back({ cubesPlaced[i].getSDLrect().x, cubesPlaced[i].getSDLrect().y, cubesPlacedRotation[i] });
			break;
		case 3:
			file["map2"]["Red_Spike"].push_back({ cubesPlaced[i].getSDLrect().x, cubesPlaced[i].getSDLrect().y, cubesPlacedRotation[i] });
			break;
		case 4:
			file["map1"]["Neutral_Cube"].push_back({ cubesPlaced[i].getSDLrect().x, cubesPlaced[i].getSDLrect().y, cubesPlacedRotation[i] });
			file["map2"]["Neutral_Cube"].push_back({ cubesPlaced[i].getSDLrect().x, cubesPlaced[i].getSDLrect().y, cubesPlacedRotation[i] });
			break;
		case 5:
			file["map1"]["Neutral_Spike"].push_back({ cubesPlaced[i].getSDLrect().x, cubesPlaced[i].getSDLrect().y, cubesPlacedRotation[i] });
			file["map2"]["Neutral_Spike"].push_back({ cubesPlaced[i].getSDLrect().x, cubesPlaced[i].getSDLrect().y, cubesPlacedRotation[i] });
			break;
		case 6:
			if (file["Spawn_Loc"].empty()) {
				file["Spawn_Loc"].push_back(cubesPlaced[i].getSDLrect().x);
				file["Spawn_Loc"].push_back(cubesPlaced[i].getSDLrect().y);
			}
			break;
		case 7:
			if (file["End_Loc"].empty()) {
				file["End_Loc"].push_back(cubesPlaced[i].getSDLrect().x);
				file["End_Loc"].push_back(cubesPlaced[i].getSDLrect().y);
			}
			break;
		case 8:
			file["map1"]["Blue_Jump"].push_back({ cubesPlaced[i].getSDLrect().x, cubesPlaced[i].getSDLrect().y, cubesPlacedRotation[i] });
			break;
		case 9:
			file["map2"]["Red_Jump"].push_back({ cubesPlaced[i].getSDLrect().x, cubesPlaced[i].getSDLrect().y, cubesPlacedRotation[i] });
			break;
		case 10:
			file["map1"]["Neutral_Jump"].push_back({ cubesPlaced[i].getSDLrect().x, cubesPlaced[i].getSDLrect().y, cubesPlacedRotation[i] });
			file["map2"]["Neutral_Jump"].push_back({ cubesPlaced[i].getSDLrect().x, cubesPlaced[i].getSDLrect().y, cubesPlacedRotation[i] });
			break;
		}
	}
	
	if (loc.empty()) {
		std::ofstream o("file.json");
		o << std::setw(4) << file << std::endl;
	}
	else {
		std::ofstream o(loc);
		o << std::setw(4) << file << std::endl;
	}

	SDL_DestroyTexture(text1);
	SDL_FreeSurface(surf1);
	SDL_DestroyTexture(text2);
	SDL_FreeSurface(surf2);
	TTF_CloseFont(font);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
}

void drawLines(boundingBox box, SDL_Renderer* renderer){
	SDL_Point p1;
	p1.x = box.getSDLrect().x;
	p1.y = box.getSDLrect().y;
	SDL_Point p2;
	p2.x = box.getSDLrect().x + box.getSDLrect().w;
	p2.y = box.getSDLrect().y;
	SDL_Point p3;
	p3.x = box.getSDLrect().x;
	p3.y = box.getSDLrect().y + box.getSDLrect().h;

	SDL_RenderDrawLine(renderer, p1.x, 0, p1.x, 1080);
	SDL_RenderDrawLine(renderer, 0, p1.y, 1920, p1.y);
	SDL_RenderDrawLine(renderer, p2.x, 0, p2.x, 1080);
	SDL_RenderDrawLine(renderer, 0, p3.y, 1920, p3.y);
}

void drawGrid(SDL_Renderer* renderer){
	//horizontal lines
	for (int y = 0; y < 1080; y += 64) {
		SDL_RenderDrawLine(renderer, 0, y, 1920, y);
	}
	//vertical lines
	for (int x = 0; x < 1920; x += 64) {
		SDL_RenderDrawLine(renderer, x, 0, x, 1080);
	}
}

void drawCube(int type, int angle, int x, int y, SDL_Renderer* renderer){
	static texture spawnAnchor("assets/spawnAnchor.png", renderer, 64, 64);
	static texture endAnchor("assets/endAnchor.png", renderer, 64, 64);
	static texture blueCube("assets/Blue_Cube.png", renderer, 64, 64);
	static texture redCube("assets/Red_Cube.png", renderer, 64, 64);
	static texture neutralCube("assets/Neutral_Cube.png", renderer, 64, 64);
	static texture neutralSpike("assets/neutralSpike.png", renderer, 64, 64);
	static texture redSpike("assets/redSpike.png", renderer, 64, 64);
	static texture blueSpike("assets/blueSpike.png", renderer, 64, 64);
	static texture neutralJump("assets/Neutral_Jump.png", renderer, 64, 64);
	static texture redJump("assets/Red_Jump.png", renderer, 64, 64);
	static texture blueJump("assets/Blue_Jump.png", renderer, 64, 64);

	switch (type) {
	case 0:
		blueCube.draw(x, y, angle);
		break;
	case 1:
		blueSpike.draw(x, y, angle);
		break;
	case 2:
		redCube.draw(x, y, angle);
		break;
	case 3:
		redSpike.draw(x, y, angle);
		break;
	case 4:
		neutralCube.draw(x, y, angle);
		break;
	case 5:
		neutralSpike.draw(x, y, angle);
		break;
	case 6:
		spawnAnchor.draw(x, y, angle);
		break;
	case 7:
		endAnchor.draw(x, y, angle);
		break;
	case 8:
		blueJump.draw(x, y, angle);
		break;
	case 9:
		redJump.draw(x, y, angle);
		break;
	case 10:
		neutralJump.draw(x, y, angle);
		break;
	}
}
