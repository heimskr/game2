#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <sys/types.h>
#include <vector>

#include "Area.h"
#include "Processor.h"
#include "defs/RefineryMode.h"

class Game;

std::chrono::milliseconds getTime();

class Context {
	public:
		std::shared_ptr<Game> game;
		bool loaded = false;

		bool showResourcePicker = false;
		bool showAreaTypePicker = false;
		bool showProcessorTypePicker = false;
		bool showInventoryPicker = false;
		bool showProcessorPicker = false;
		bool showRefineryModePicker = false;
		bool showTextInput = false;
		std::function<void(const std::string &)> onResourcePicked = [](const std::string &) {};
		std::function<void(Area::Type)> onAreaTypePicked = [](Area::Type) {};
		std::function<void(Processor::Type)> onProcessorTypePicked = [](Processor::Type) {};
		std::function<void(const std::string &)> onInventoryPicked = [](const std::string &) {};
		std::function<void(std::shared_ptr<Processor>)> onProcessorPicked = [](std::shared_ptr<Processor>) {};
		std::function<void(RefineryMode)> onRefineryModePicked = [](RefineryMode) {};
		std::function<void(const std::string &)> onTextInput = [](const std::string &) {};

		std::string message;
		bool isConfirm = false;
		std::function<void(bool)> onChoice = [](bool) {};

		std::string processorPickerMessage;
		std::string refineryModePickerMessage;

		bool rightDown = false;
		bool downDown = false;
		bool leftDown = false;
		bool upDown = false;

		void newGame();
		void load();
		void save();

		void pickResource(std::function<void(const std::string &)>);
		void pickAreaType(std::function<void(Area::Type)>);
		void pickProcessorType(std::function<void(Processor::Type)>);
		void pickInventory(std::function<void(const std::string &)>);
		void pickProcessor(std::function<void(std::shared_ptr<Processor>)>, const std::string & = "");
		void pickRefineryMode(std::function<void(RefineryMode)>, const std::string & = "");
		void pickText(std::function<void(const std::string &)>);
		void confirm(const std::string &, std::function<void(bool)>);
		void showMessage(const std::string &);

		// Each frame, every function in this vector is called after the UI is rendered. The vector is then cleared.
		std::vector<std::function<void()>> frameActions;

		Game * operator->() {
			return game.get();
		}

		const Game * operator->() const {
			return game.get();
		}
};
