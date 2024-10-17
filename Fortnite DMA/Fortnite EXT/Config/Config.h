#pragma once

class Config {
private:
	static std::unique_ptr<Config> instance;
	Config() {}
public:

	struct Settings {
		// Menu Toggle
		bool ShowMenu = true;
		bool ShowStats = true;

		// Aim Settings
		bool Aimbot = false;
		bool ShowFOV = false;
		float FOVColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float FOVSize = 20;

		// Visuals
		bool ShowBox = false;
		float BoxColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		bool ShowSkeleton = false;
		float SkeletonColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	};
	Settings Setting;

	Config(const Config&) = delete;
	Config& operator=(const Config&) = delete;

	static Config& getInstance() {
		if (!instance) {
			instance.reset(new Config());
		}
		return *instance;
	}
};