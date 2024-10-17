#include "../General.h"
#include "EntityLoop.h"

#include "Utils/Matrix.h"

std::unique_ptr<EntityLoop> EntityLoop::instance = nullptr;

CameraClass ViewPoint;
CameraClass GetCameraInfo(EntityLoop& Entity, Memory& mem) {
	auto location_pointer = mem.Read<uintptr_t>(Entity.GmCache.Uworld + 0x110);
	auto rotation_pointer = mem.Read<uintptr_t>(Entity.GmCache.Uworld + 0x120);

	struct RotationInfo { double pitch; char pad_0008[24]; double yaw; char pad_0028[424]; double roll; } rotationInfo;

	rotationInfo.pitch = mem.Read<double>(rotation_pointer);
	rotationInfo.yaw = mem.Read<double>(rotation_pointer + 0x20);
	rotationInfo.roll = mem.Read<double>(rotation_pointer + 0x1d0); // if outdated boxes will not stay on players

	ViewPoint.Location = mem.Read<Vector3>(location_pointer);
	ViewPoint.Rotation.x = asin(rotationInfo.roll) * (180.0 / M_PI);
	ViewPoint.Rotation.y = ((atan2(rotationInfo.pitch * -1, rotationInfo.yaw) * (180.0 / M_PI)) * -1) * -1;
	ViewPoint.FieldOfView = mem.Read<float>(Entity.GmCache.PlayerController + 0x394) * 90.f;

	return ViewPoint;
}

Vector3 GetBoneWithRotation(EntityLoop& Entity, Memory& mem, uintptr_t skeletal_mesh, int bone_index) {
	uintptr_t bone_array = mem.Read<uintptr_t>(skeletal_mesh + Entity.Offset.BoneArray); //if (!bone_array) { return Vector3(); }
	if (bone_array == NULL) bone_array = mem.Read<uintptr_t>(skeletal_mesh + Entity.Offset.BoneArray + 0x10); //if (!bone_array) { return Vector3(); }
	FTransform bone = mem.Read<FTransform>(bone_array + (bone_index * 0x60));
	FTransform component_to_world = mem.Read<FTransform>(skeletal_mesh + Entity.Offset.Comp2World);
	D3DMATRIX matrix = MatrixMultiplication(bone.ToMatrixWithScale(), component_to_world.ToMatrixWithScale());
	return Vector3(matrix._41, matrix._42, matrix._43);
}

Vector3 ProjectWorldToScreen(Overlay& overlay, EntityLoop& Entity, Memory& mem, Vector3 WorldLocation) {
	CameraClass ViewPoint = GetCameraInfo(Entity, mem);
	D3DMATRIX tempMatrix = Matrix(ViewPoint.Rotation);
	Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);
	Vector3 vDelta = WorldLocation - ViewPoint.Location;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));
	if (vTransformed.z < 1.f) { vTransformed.z = 1.f; }
	return Vector3((overlay.OverlayInf.screenWidth / 2.0f) + vTransformed.x * (((overlay.OverlayInf.screenWidth / 2.0f) / tanf(ViewPoint.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, (overlay.OverlayInf.screenHeight / 2.0f) - vTransformed.y * (((overlay.OverlayInf.screenWidth / 2.0f) / tanf(ViewPoint.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, 0);
}

bool EntityLoop::MainLoop(Memory& mem, EntityLoop& Entity, Cache& cache, Config& conf, Overlay& overlay) {
	uint64_t BaseAddress = mem.GetBaseAddy(cache.ProcessInfo.ProcessName);
	if (!BaseAddress) {
		printf("Failed To Get BaseAddress!\n");
		Sleep(1500);
		return 0;
	}
	printf("BaseAddress 0x%p\n", BaseAddress);

	__int16 DosHeader = mem.Read<__int16>(BaseAddress);
	 if (!DosHeader) { 
		printf("Failed To Read DosHeader!\n");
		Sleep(1500);
		return 0;
	 }
	printf("DosHeader 0x%p\n", DosHeader);

	__int64 UWorld = 0;
	__int64 va_text = 0;
	for (auto i = 0; i < FLT_MAX; i++) {
		va_text = BaseAddress + i * 0x1000;
		GmCache.Uworld = mem.Read<__int64>(va_text + Offset.Uworld);
		GmCache.Level = mem.Read<__int64>(GmCache.Uworld + 0x30);
		if (GmCache.Uworld && GmCache.Level && mem.Read<__int64>(GmCache.Level + Offset.Level) == GmCache.Uworld) {
			break;
		}
	}
	if (!GmCache.Uworld) { printf("Failed Getting Uworld!\n"); ExitThread(0x1337); }
	if (!GmCache.Level) { printf("Failed Getting Level!\n"); ExitThread(0x1337); }
	printf("UWorld %X\n", GmCache.Uworld);
	printf("Level %X\n", GmCache.Level);

	while (1) {

		__int64 GameInstance = mem.Read<__int64>(GmCache.Uworld + Offset.GameInstance);
		if (!GameInstance) { printf("Failed Getting GameInstance!\n"); }
		printf("GameInstance %X\n", GameInstance);

		__int64 PersistentLevel = mem.Read<__int64>(GmCache.Uworld + 0x30); //UWorld::PersistentLevel
		if (!PersistentLevel) { printf("Failed Getting PersistentLevel!\n"); }
		printf("PersistentLevel %X\n", PersistentLevel);

		__int64 LocalPlayerPtr = mem.Read<__int64>(GameInstance + 0x38); //UGameInstance::LocalPlayers
		if (!LocalPlayerPtr) { printf("Failed Getting LocalPlayerPtr!\n"); }
		printf("LocalPlayerPtr %X\n", LocalPlayerPtr);

		__int64 LocalPlayer = mem.Read<__int64>(LocalPlayerPtr);
		if (!LocalPlayer) { printf("Failed Getting LocalPlayer!\n"); }
		printf("LocalPlayer %X\n", LocalPlayer);

		GmCache.PlayerController = mem.Read<__int64>(LocalPlayerPtr);
		if (!GmCache.PlayerController) { printf("Failed Getting PlayerController!\n"); }
		printf("PlayerController %X\n", GmCache.PlayerController);

		__int64 PlayerState = mem.Read<__int64>(GmCache.PlayerController + Offset.PlayerState);
		if (!PlayerState) { printf("Failed Getting PlayerController!\n"); }
		printf("PlayerState %X\n", PlayerState);

		__int64 CameraManager = mem.Read<__int64>(GmCache.PlayerController + Offset.CameraManager);
		if (!CameraManager) { printf("Failed Getting CameraManager!\n"); }
		printf("CameraManager %X\n", CameraManager);

		__int64 PlayerArray = mem.Read<__int64>(PersistentLevel + Offset.CurrentActor);
		if (!PlayerArray) { printf("Failed Getting PlayerArray!\n"); }
		printf("PlayerArray %X\n", PlayerArray);

		__int64 PlayerCount = mem.Read<__int64>(PersistentLevel + Offset.ActorCount);
		if (!PlayerCount) { printf("Failed Getting PlayerCount!\n"); }
		printf("PlayerCount %d\n", PlayerCount);

		int FailSafe = 0;
		for (int i = 0; i < PlayerCount; i++) {
			if (FailSafe > 50) { PlayerCount = 0; continue; }

			__int64 CurrentActor = mem.Read<__int64>(PlayerArray + i * 0x8); // Padding
			if (!CurrentActor) { FailSafe++; continue; }

			__int64 CurrentActorMesh = mem.Read<uint64_t>(CurrentActor + 0x310); //Actor::ActorMesh
			if (!CurrentActorMesh) { FailSafe++; continue; };

			Vector3 HeadPos = GetBoneWithRotation(Entity, mem, CurrentActorMesh, 110);
			Vector3 FeetPos = GetBoneWithRotation(Entity, mem, CurrentActorMesh, 0);

			Vector3 Bottom = ProjectWorldToScreen(overlay, Entity, mem, FeetPos);
			Vector3 Top = ProjectWorldToScreen(overlay, Entity, mem, Vector3(HeadPos.x, HeadPos.y, HeadPos.z + 25));

			if (conf.Setting.ShowBox) {
				float CornerHeight = fabs(Top.y - Bottom.y);
				float CornerWidth = CornerHeight * 0.550f;

				Box box;
				box.position = Vector2(Top.x - (CornerWidth / 2), Top.y);
				box.width = CornerWidth;
				box.height = CornerHeight;
				PlayerInf.Boxes.push_back(box);
			}
		}
		Sleep(1);
	}
	return true;
}