#pragma once

class EntityLoop {
private:
	static std::unique_ptr<EntityLoop> instance;
	EntityLoop() {}
public:

	struct Box {
		Vector2 position;
		float width;
		float height;
	};

	struct PlayerInfo {
		std::vector<Box> Boxes;
	};
	PlayerInfo PlayerInf;

	struct GameCache {
		__int64 Uworld = 0;
		__int64 Level = 0;
		__int64 PlayerController = 0;
	};
	GameCache GmCache;

	struct Offsets {
		__int64 Uworld = 0x12300E58;
		__int64 Level = 0xc0;

		__int64 GameInstance = 0x1D8;
		__int64 PlayerController = 0x30;
		__int64 CurrentActor = 0xA0;
		__int64 ActorCount = 0xA8;
		__int64 PlayerState = 0x2B0;
		__int64 CameraManager = 0x348;
		__int64 Comp2World = 0x1c0;
		__int64 BoneArray = 0x570;
	};
	Offsets Offset;

	bool MainLoop(Memory& mem, EntityLoop& Entity, Cache& cache, Config& conf, Overlay& overlay);

	EntityLoop(const EntityLoop&) = delete;
	EntityLoop& operator=(const EntityLoop&) = delete;

	static EntityLoop& getInstance() {
		if (!instance) {
			instance.reset(new EntityLoop());
		}
		return *instance;
	}
};