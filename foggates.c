#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "common.c"

typedef enum {
	ThingsBetwixt = 0,
	Majula,
	ForestOfTheFallenGiants,
	BrightstoneCoveTseldora,
	AldiasKeep,
	TheLostBastilleBelfryLuna,
	HarvestValleyEarthenPeak,
	NomansWharf,
	IronKeepBelfrySol,
	HuntsmanCopseUndeadPurgatory,
	TheGutterBlackGulch,
	DragonAerieDragonShrine,
	MajulaShadedWoods,
	HeidesToNoMansWharf,
	HeidesTowerofFlame,
	ShadedWoodsShrineofWinter,
	DoorsofPharros,
	GraveofSaints,
	MemoryofVammarOrroandJeigh,
	ShrineofAmana,
	DrangleicCastleThroneofWant,
	UndeadCrypt,
	DragonMemories,
	DarkChasmofOld,
	ShulvaSanctumCity,
	BrumeTower,
	FrozenEleumLoyce,
	MemoryoftheKing,
	__MapNameCount,
} MapName;

typedef enum {
	Boss_None,
	TheLastGiant,
	ThePursuer,
	Dragonrider,
	OldDragonslayer,
	FlexileSentry,
	RuinSentinels,
	TheLostSinner,
	BelfryGargoyles,
	SkeletonLords,
	ExecutionersChariot,
	CovetousDemon,
	MythaTheBanefulQueen,
	SmelterDemon,
	OldIronKing,
	ScorpionessNajka,
	RoyalRatAuthority,
	ProwlingMagusAndCongregation,
	TheDukesDearFreja,
	RoyalRatVanguard,
	TheRotten,
	TwinDragonrider,
	LookingGlassKnight,
	DemonofSong,
	VelstadtTheRoyalAegis,
	VendrickBoss,
	GuardianDragon,
	AncientDragonBoss,
	GiantLord,
	ThroneWatcherAndDefender,
	Nashandra,
	AldiaScholarOfTheFirstSin,
	Darklurker,
	ElanaTheSqualidQueen,
	SinhTheSlumberingDragon,
	Gankfight,
	FumeKnight,
	SirAlonne,
	BlueSmelterDemon,
	AavaTheKingsPet,
	BurntIvoryKing,
	LudAndZallen,
	__BossCount,
} BossName;

int boss_destruction_flags[] = {
		[Boss_None]					   = -1,
		[TheLastGiant]                 = 110000081,
		[ThePursuer]                   = 110000086,
		[Dragonrider]                  = 131000081,
		[OldDragonslayer]              = 131000086,
		[FlexileSentry]                = 118000081,
		[RuinSentinels]                = 116000081,
		[TheLostSinner]                = 116000086,
		[BelfryGargoyles]              = 116000091,
		[SkeletonLords]                = 123000086,
		[ExecutionersChariot]          = 123000081,
		[CovetousDemon]                = 117000081,
		[MythaTheBanefulQueen]         = 117000091,
		[SmelterDemon]                 = 119000081,
		[OldIronKing]                  = 119000086,
		[ScorpionessNajka]             = 132000081,
		[RoyalRatAuthority]            = 133000081,
		[ProwlingMagusAndCongregation] = 114000096,
		[TheDukesDearFreja]            = 114000081,
		[RoyalRatVanguard]             = 134000081,
		[TheRotten]                    = 125000081,
		[TwinDragonrider]              = 221000081,
		[LookingGlassKnight]           = 221000086,
		[DemonofSong]                  = 211000081,
		[VelstadtTheRoyalAegis]        = 224000081,
		[VendrickBoss]                 = 224000086,
		[GuardianDragon]               = 115000081,
		[AncientDragonBoss]            = 127000081,
		[GiantLord]                    = 210000081,
		[ThroneWatcherAndDefender]     = 221000091,
		[Nashandra]                    = 221000096,
		[AldiaScholarOfTheFirstSin]    = 221000006,
		[Darklurker]                   = 403000081,
		[ElanaTheSqualidQueen]         = 535000081,
		[SinhTheSlumberingDragon]      = 535000096,
		[Gankfight]                    = 535000091,
		[FumeKnight]                   = 536000081,
		[SirAlonne]                    = 536000086,
		[BlueSmelterDemon]             = 536000091,
		[AavaTheKingsPet]              = 537000081,
		[BurntIvoryKing]               = 537000086,
		[LudAndZallen]                 = 537000091,
};

static_assert(ARRAY_LEN(boss_destruction_flags) == __BossCount, "Missing destruction flag");

typedef enum {
	WPBT_None = 0,
	WPBT_Inaccessible,
	WPBT_PharrosLockstone,
	WPBT_FragrantBranchOfYore,
} WarpPointBlockType;

#define FogGateNames \
	X(Tutorial1Entry) \
	X(Tutorial1Exit) \
	X(Tutorial2Entry) \
	X(Tutorial2Exit) \
	X(Tutorial3Entry) \
	X(Tutorial3Exit) \
	X(MajulaToForestOfFallenGiants) \
	X(MajulaToGraveOfSaints) \
	X(MajulaToHuntsmanCopse) \
	X(MajulaToRotundaLockstone) \
	X(MajulaToGutter) \
	X(MajulaToShadedWoods) \
	X(LastGiantEntry) \
	X(PursuerEntry) \
	X(PursuerExit) \
	X(ForestOfFallenGiantsBalcony) \
	X(ForestOfFallenGiantsToCale) \
	X(ForestOfFallenGiantsAfterFirstBonfire) \
	X(ForestOfFallenGiantsFromMajula) \
	X(ForestOfFallenGiantsToPursuerArena) \
	X(CongregationEntry) \
	X(CongregationExit) \
	X(BrightstoneCoveToDoorsOfPharros) \
	X(DukesDearFreyjaEntry) \
	X(DukesDearFreyjaExit) \
	X(GuardianDragonEntry) \
	X(GuardianDragonExit) \
	X(AldiasKeepEntrance) \
	X(GargoylesEntry) \
	X(GargoylesExit) \
	X(RuinSentinelsEntry) \
	X(RuinSentinelsExit) \
	X(RuinSentinelsHiddenDoor1Exit) \
	X(RuinSentinelsHiddenDoor2Exit) \
	X(RuinSentinelsHiddenDoor3Exit) \
	X(RuinSentinelsHiddenDoor4Exit) \
	X(RuinSentinesUpperExit) \
	X(LostBastilleToSinnersRise) \
	X(LostBastilleToShipFromWharf) \
	X(LostBastilleToBelfryLuna) \
	X(LostSinnerEntry) \
	X(LostSinnerExit) \
	X(CovetousDemonEntry) \
	X(EarthenPeakNearWindmillBurnLocation) \
	X(MythaEntry) \
	X(CovetousDemonExit) \
	X(MythaExit) \
	X(HarvestValleyToSkeletonLords) \
	X(FlexileSentryExit) \
	X(FlexileSentryEntry) \
	X(NoMansWharfToHeides) \
	X(SmeltorDemonEntry) \
	X(IronKeepNearFlameThrowerAndTurtle) \
	X(BelfrySolExit) \
	X(BelfrySolEntry) \
	X(OldIronKingEntry) \
	X(OldIronKingExit) \
	X(SmeltorDemonExit) \
	X(SmeltorDemonToBonfire) \
	X(IronKeepToBelfryAtPharrosLockstone) \
	X(IronKeepToEarthenPeak) \
	X(SkeletonLordsEntry) \
	X(ChariotEntry) \
	X(SkeletonLordsExit) \
	X(ChariotExit) \
	X(HuntsmanCopseToMajula) \
	X(GutterNearAntQueen) \
	X(TheRottenExit) \
	X(BlackGulchEntrance) \
	X(TheGutterFromGraveOfSaints) \
	X(TheRottenEntry) \
	X(DragonAerieToAldiasKeep) \
	X(AncientDragon) \
	X(ShadedWoodsFromMajula) \
	X(DragonriderEntry) \
	X(DragonriderExit) \
	X(HeidesToMajula) \
	X(HeidesToWharf) \
	X(OldDragonslayerEntry) \
	X(OldDragonslayerExit) \
	X(ScorpionessNajkaExit) \
	X(ShadedWoodsToMistyArea) \
	X(ScorpionessNajkaEntry) \
	X(RoyalRatAuthorityExit) \
	X(DoorOfPharrosToOrdealEndBonfire) \
	X(DoorOfPharrosToRatKingDomain) \
	X(RoyalRatAuthorityEntry) \
	X(GraveOfSaintsFromPit) \
	X(GraveOfSaintsNearUpperBonfire) \
	X(RoyalRatVanguardEntry) \
	X(RoyalRatVanguardExit) \
	X(GiantLordEntry) \
	X(GiantLordExit) \
	X(DemonOfSongEntry) \
	X(ShrineOfAmanaTo2ndBonfire) \
	X(ShrineOfAmanaTo3rdBonfire) \
	X(DemonOfSongExit) \
	X(ShrineOfAmanaToDrangleicCastle) \
	X(LookingGlassKnightEntry) \
	X(TwinDragonridersEntry) \
	X(TwinDragonridersExit) \
	X(LookingGlassKnightExit) \
	X(FinalFightArena) \
	X(DrangleicCastleToChasmPortal) \
	X(DrangleicCastleToShadedWoods) \
	X(UndeadCryptToShrineOfAmana) \
	X(VelsdatEntry) \
	X(VelsdatExit) \
	X(UndeadCryptFromAgdayneToBonfire) \
	X(Vendrick) \
	X(DarkChasmFromBlackGulchExit) \
	X(DarkChasmFromDrangleicCastleExit) \
	X(DarkChasmFromShadedWoodsExit) \
	X(DarkLurkerExit) \
	X(SihnTheSlumberingDragonEntry) \
	X(ElanaTheSqualidQueenEntry) \
	X(GankSquadBossEntry) \
	X(GankSquadBossExit) \
	X(ShulvaToGankFight1) \
	X(ShulvaEntrance) \
	X(BetweenElanaAndSihn) \
	X(ShulvaToGankFight2) \
	X(NearJesterThomas) \
	X(BrumeTowerFrom2ndBonfireToCentralRoom) \
	X(BrumeTowerToFoyerBonfireFromOutside) \
	X(BrumeTowerToScorchingIronSceptorKey) \
	X(SirAlonneEntry) \
	X(SirAlonneExit) \
	X(FumeKnightEntry) \
	X(FumeKnightExit) \
	X(BlueSmeltorDemonEntry) \
	X(BlueSmeltorDemonExit) \
	X(BrumeTowerEntranceFromLift) \
	X(BrumeTowerToBlueSmelterDungeon) \
	X(AavaTheKingsPetExit) \
	X(EleumLoyceAfterGettingEyeToSeeGhosts) \
	X(IvoryKing) \
	X(LudAndZallenEntry) \
	X(AavaTheKingsPetEntry) \
	X(LudAndZallenExit) \
	X(EleumLoyceCathedraEntrance)

#define WarpPoints \
	X(GameStartSpawn) \
	X(NearPursuerGiantMemoryEntry) \
	X(NearPateGiantMemoryEntry) \
	X(GiantLordMemoryEntry) \
	X(NearPursuerGiantMemoryExit) \
	X(NearPateGiantMemoryExit) \
	X(GiantLordMemoryExit) \
	X(SirAlonneArmorDLCEntry) \
	X(SirAlonneMemoryExit) \
	X(ChasmPortalFromBlackGulch) \
	X(ChasmPortalFromShadedWoods) \
	X(ChasmPortalFromCastle) \
	X(ChasmGulchExitWarp) \
	X(ChasmShadedWoodsExitWarp) \
	X(ChasmCastleExitWarp) \
	X(ChasmDarkLurkerExitWarp) \
	X(IvoryKingFightEnd) \
	X(CoffinWarp) \
	X(LudAndZallenExitWarp) \
	X(DragonMemoriesCove) \
	X(DragonMemoriesMemory) \
	X(MemoryOfTheKingCrypt) \
	X(MemoryOfTheKingMemory)

#define OtherWarpNodes \
	X(NearPursuerBirdEntry) \
	X(NearPursuerBirdExit) \
	X(PirateShipWharf) \
	X(PirateShipBastille) \
	X(DLC1EntranceBaseGame) \
	X(DLC2EntranceBaseGame) \
	X(DLC3EntranceBaseGame) \
	X(DLC1EntranceDLC) \
	X(DLC2EntranceDLC) \
	X(DLC3EntranceDLC) \
	X(DarkLurkerSpawn)

typedef enum {
#define X(V) V ## Src,
	WarpPoints
#undef X
#define X(V) V ## Dst,
	WarpPoints
#undef X
#define X(V) V ## Front,
	FogGateNames
#undef X
#define X(V) V ## Back,
	FogGateNames
#undef X
#define X(V) V,
	OtherWarpNodes
#undef X
	__WarpNodeCount,
} WarpNode;

#define CONCAT(a, b) #a #b

const char *WarpNodeNames[] = {
#define X(name) CONCAT(name, Src),
  WarpPoints
#undef X
#define X(name) CONCAT(name, Dst),
  WarpPoints
#undef X
#define X(name) CONCAT(name, Front),
  FogGateNames
#undef X
#define X(name) CONCAT(name, Back),
  FogGateNames
#undef X
#define X(name) #name,
  OtherWarpNodes
#undef X
};

#define FogGateStart Tutorial1EntryFront
#define FogGateEnd EleumLoyceCathedraEntranceFront
#define AllFogGatesStart FogGateStart
#define AllFogGatesEnd EleumLoyceCathedraEntranceBack
#define AllFogGatesCount AllFogGatesEnd - AllFogGatesStart + 1
#define AllConnectionsStart GameStartSpawnSrc
#define AllConnectionsEnd DLC3EntranceDLC
#define AllConnectionsCount AllConnectionsEnd - AllConnectionsStart + 1

#define CUTSCENES_COUNT 8

bool point_has_cutscene(WarpNode node) {
	if (node == LastGiantEntryBack
		|| node == PursuerEntryBack
		|| node == LostSinnerEntryBack
		|| node == ChariotEntryBack
		|| node == OldIronKingEntryBack
		|| node == DukesDearFreyjaEntryBack
		|| node == TheRottenEntryBack
		|| node == VelsdatEntryBack
	) return true;
	return false;
}

bool warpnode_is_foggate(WarpNode node) {
	printf("Warning: this will only check entry gates");
	return node >= FogGateStart && node <= FogGateEnd;
}

bool warpnode_is_foggate_v2(WarpNode node) {
	return node >= AllFogGatesStart && node <= AllFogGatesEnd;
}

typedef struct {
	bool is_foggate;
	int fog_gate_id; // can be generated when reading the map_file
	int warp_pt_id; // needs to be filled dynamically when generating event loc
	int event_param_id; // needs to be filled when reading files
	int warp_obj_inst_id; // needs to be filled when reading files
	MapName map;
	WarpNode cutscene_gate_id;
	int cutscene_event_id; // needs to be filled when generating files
	bool pvp;
	bool reverse; // to reverse the logic of generation of warp points
	BossName boss;
	WarpPointBlockType blocktype;
	const char *obj_id;
	V3f offset;
} WarpPoint;

#define NEW_WARP_PT(...) (WarpPoint) {\
	.is_foggate = true,\
	.fog_gate_id = -1,\
	.warp_pt_id = -1,\
	.event_param_id = -1,\
	.warp_obj_inst_id = -1,\
	.map = __MapNameCount,\
	.cutscene_gate_id = __WarpNodeCount,\
	.cutscene_event_id = -1,\
	.pvp = false,\
	.reverse = false,\
	.boss = Boss_None,\
	.blocktype = WPBT_None,\
	.obj_id = NULL,\
	.offset = {0},\
	__VA_ARGS__,\
}

static WarpPoint warp_points[] = {
	[GameStartSpawnSrc]              = NEW_WARP_PT(.is_foggate = false, .map = ThingsBetwixt),
	[GameStartSpawnDst]              = NEW_WARP_PT(.is_foggate = false, .map = ThingsBetwixt),
	// warp gates
	[DarkLurkerSpawn]                = NEW_WARP_PT(.is_foggate = false, .map = DarkChasmofOld),
	//
	[NearPursuerBirdEntry]           = NEW_WARP_PT(.is_foggate = false, .map = ForestOfTheFallenGiants),
	[NearPursuerGiantMemoryEntrySrc] = NEW_WARP_PT(.is_foggate = false, .map = ForestOfTheFallenGiants),
	[NearPateGiantMemoryEntrySrc]    = NEW_WARP_PT(.is_foggate = false, .map = ForestOfTheFallenGiants),
	[GiantLordMemoryEntrySrc]        = NEW_WARP_PT(.is_foggate = false, .map = ForestOfTheFallenGiants),
	[NearPursuerGiantMemoryExitDst]  = NEW_WARP_PT(.is_foggate = false, .map = ForestOfTheFallenGiants),
	[NearPateGiantMemoryExitDst]     = NEW_WARP_PT(.is_foggate = false, .map = ForestOfTheFallenGiants),
	[GiantLordMemoryExitDst]         = NEW_WARP_PT(.is_foggate = false, .map = ForestOfTheFallenGiants),
	//
	[NearPursuerBirdExit]            = NEW_WARP_PT(.is_foggate = false, .map = TheLostBastilleBelfryLuna),
	[NearPursuerGiantMemoryEntryDst] = NEW_WARP_PT(.is_foggate = false, .map = MemoryofVammarOrroandJeigh),
	[NearPateGiantMemoryEntryDst]    = NEW_WARP_PT(.is_foggate = false, .map = MemoryofVammarOrroandJeigh),
	[GiantLordMemoryEntryDst]        = NEW_WARP_PT(.is_foggate = false, .map = MemoryofVammarOrroandJeigh),
	[NearPursuerGiantMemoryExitSrc]  = NEW_WARP_PT(.is_foggate = false, .map = MemoryofVammarOrroandJeigh),
	[NearPateGiantMemoryExitSrc]     = NEW_WARP_PT(.is_foggate = false, .map = MemoryofVammarOrroandJeigh),
	[GiantLordMemoryExitSrc]         = NEW_WARP_PT(.is_foggate = false, .map = MemoryofVammarOrroandJeigh),
	//
	[PirateShipWharf]                = NEW_WARP_PT(.is_foggate = false, .map = NomansWharf),
	[PirateShipBastille]             = NEW_WARP_PT(.is_foggate = false, .map = NomansWharf),
	//
	[DLC1EntranceBaseGame]           = NEW_WARP_PT(.is_foggate = false, .map = TheGutterBlackGulch),
	[DLC2EntranceBaseGame]           = NEW_WARP_PT(.is_foggate = false, .map = IronKeepBelfrySol),
	[DLC3EntranceBaseGame]           = NEW_WARP_PT(.is_foggate = false, .map = ShadedWoodsShrineofWinter),
	[DLC1EntranceDLC]                = NEW_WARP_PT(.is_foggate = false, .map = ShulvaSanctumCity),
	[DLC2EntranceDLC]                = NEW_WARP_PT(.is_foggate = false, .map = BrumeTower),
	[DLC3EntranceDLC]                = NEW_WARP_PT(.is_foggate = false, .map = FrozenEleumLoyce),
	//
	[SirAlonneArmorDLCEntrySrc]      = NEW_WARP_PT(.is_foggate = false, .map = BrumeTower),
	[SirAlonneArmorDLCEntryDst]      = NEW_WARP_PT(.is_foggate = false, .map = BrumeTower),
	[SirAlonneMemoryExitSrc]         = NEW_WARP_PT(.is_foggate = false, .map = BrumeTower),
	[SirAlonneMemoryExitDst]         = NEW_WARP_PT(.is_foggate = false, .map = BrumeTower),
	//
	[ChasmPortalFromBlackGulchSrc]   = NEW_WARP_PT(.is_foggate = false, .map = TheGutterBlackGulch),
	[ChasmPortalFromShadedWoodsSrc]  = NEW_WARP_PT(.is_foggate = false, .map = ShadedWoodsShrineofWinter),
	[ChasmPortalFromCastleSrc]       = NEW_WARP_PT(.is_foggate = false, .map = DrangleicCastleThroneofWant),
	//
	[ChasmPortalFromBlackGulchDst]   = NEW_WARP_PT(.is_foggate = false, .map = DarkChasmofOld),
	[ChasmPortalFromShadedWoodsDst]  = NEW_WARP_PT(.is_foggate = false, .map = DarkChasmofOld),
	[ChasmPortalFromCastleDst]       = NEW_WARP_PT(.is_foggate = false, .map = DarkChasmofOld),
	//
	[ChasmGulchExitWarpSrc]          = NEW_WARP_PT(.is_foggate = false, .map = DarkChasmofOld),
	[ChasmShadedWoodsExitWarpSrc]    = NEW_WARP_PT(.is_foggate = false, .map = DarkChasmofOld),
	[ChasmCastleExitWarpSrc]         = NEW_WARP_PT(.is_foggate = false, .map = DarkChasmofOld),
	[ChasmDarkLurkerExitWarpSrc]     = NEW_WARP_PT(.is_foggate = false, .map = DarkChasmofOld),
	[ChasmGulchExitWarpDst]          = NEW_WARP_PT(.is_foggate = false, .map = TheGutterBlackGulch),
	[ChasmShadedWoodsExitWarpDst]    = NEW_WARP_PT(.is_foggate = false, .map = ShadedWoodsShrineofWinter),
	[ChasmCastleExitWarpDst]         = NEW_WARP_PT(.is_foggate = false, .map = DrangleicCastleThroneofWant),
	[ChasmDarkLurkerExitWarpDst]     = NEW_WARP_PT(.is_foggate = false), // TODO: check where this loc will be also when it would spawn in the darklurker boss
	//
	[IvoryKingFightEndSrc]           = NEW_WARP_PT(.is_foggate = false, .map = FrozenEleumLoyce),
	[IvoryKingFightEndDst]           = NEW_WARP_PT(.is_foggate = false, .map = FrozenEleumLoyce),
	[CoffinWarpSrc]                  = NEW_WARP_PT(.is_foggate = false, .map = FrozenEleumLoyce),
	[CoffinWarpDst]                  = NEW_WARP_PT(.is_foggate = false, .map = FrozenEleumLoyce),
	[LudAndZallenExitWarpSrc]        = NEW_WARP_PT(.is_foggate = false, .map = FrozenEleumLoyce),
	[LudAndZallenExitWarpDst]        = NEW_WARP_PT(.is_foggate = false, .map = FrozenEleumLoyce),
	//
	[DragonMemoriesCoveSrc]          = NEW_WARP_PT(.is_foggate = false, .map = BrightstoneCoveTseldora),
	[DragonMemoriesCoveDst]          = NEW_WARP_PT(.is_foggate = false, .map = DragonMemories),
	[DragonMemoriesMemoryDst]        = NEW_WARP_PT(.is_foggate = false, .map = BrightstoneCoveTseldora),
	[DragonMemoriesMemorySrc]        = NEW_WARP_PT(.is_foggate = false, .map = DragonMemories),
	[MemoryOfTheKingCryptSrc]        = NEW_WARP_PT(.is_foggate = false, .map = UndeadCrypt),
	[MemoryOfTheKingCryptDst]        = NEW_WARP_PT(.is_foggate = false, .map = MemoryoftheKing),
	[MemoryOfTheKingMemoryDst]       = NEW_WARP_PT(.is_foggate = false, .map = UndeadCrypt),
	[MemoryOfTheKingMemorySrc]       = NEW_WARP_PT(.is_foggate = false, .map = MemoryoftheKing),

	// Things betwixt
	[Tutorial1EntryFront] = NEW_WARP_PT(.map = ThingsBetwixt, .obj_id = "o00_0500_0001"),
	[Tutorial1ExitFront]  = NEW_WARP_PT(.map = ThingsBetwixt, .obj_id = "o00_0500_0003"),
	[Tutorial2EntryFront] = NEW_WARP_PT(.map = ThingsBetwixt, .obj_id = "o00_0500_0004", .blocktype = WPBT_FragrantBranchOfYore),
	[Tutorial2ExitFront]  = NEW_WARP_PT(.map = ThingsBetwixt, .obj_id = "o00_0500_0000"),
	[Tutorial3EntryFront] = NEW_WARP_PT(.map = ThingsBetwixt, .obj_id = "o00_0500_0005"),
	[Tutorial3ExitFront]  = NEW_WARP_PT(.map = ThingsBetwixt, .obj_id = "o00_0500_0002"),
	[Tutorial1EntryBack]  = NEW_WARP_PT(.map = ThingsBetwixt, .obj_id = "o00_0500_0001"),
	[Tutorial1ExitBack]   = NEW_WARP_PT(.map = ThingsBetwixt, .obj_id = "o00_0500_0003"),
	[Tutorial2EntryBack]  = NEW_WARP_PT(.map = ThingsBetwixt, .obj_id = "o00_0500_0004"),
	[Tutorial2ExitBack]   = NEW_WARP_PT(.map = ThingsBetwixt, .obj_id = "o00_0500_0000"),
	[Tutorial3EntryBack]  = NEW_WARP_PT(.map = ThingsBetwixt, .obj_id = "o00_0500_0005"),
	[Tutorial3ExitBack]   = NEW_WARP_PT(.map = ThingsBetwixt, .obj_id = "o00_0500_0002"),

	// Majula
	[MajulaToForestOfFallenGiantsFront] = NEW_WARP_PT(.map = Majula, .obj_id = "o00_0501_0100", .pvp = true),
	[MajulaToGraveOfSaintsFront]        = NEW_WARP_PT(
											.map = Majula, .obj_id = "o00_0501_0101", .pvp = true, 
											.reverse = true, .offset = (V3f){0.0f, 43.455f - 42.057f, 0.0f}
										),
	[MajulaToHuntsmanCopseFront]        = NEW_WARP_PT(
											.map = Majula, .obj_id = "o00_0501_0102",
											.pvp = true, .offset = (V3f){0.0f,  1.390f -  0.413f, 0.0f}
										),
	[MajulaToRotundaLockstoneFront]     = NEW_WARP_PT(.map = Majula, .obj_id = "o00_0501_0103", .pvp = true),
	[MajulaToGutterFront]               = NEW_WARP_PT(
											.map = Majula, .obj_id = "o00_0501_0104", .pvp = true,
											.offset = (V3f){0.0f, 71.218f - 69.941f, 0.0f}
										),
	[MajulaToShadedWoodsFront]          = NEW_WARP_PT(.map = Majula, .obj_id = "o00_0502_0103", .pvp = true),
	[MajulaToForestOfFallenGiantsBack]  = NEW_WARP_PT(.map = Majula, .obj_id = "o00_0501_0100", .pvp = true),
	[MajulaToGraveOfSaintsBack]         = NEW_WARP_PT(
											.map = Majula, .obj_id = "o00_0501_0101", .reverse = true,
											.pvp = true, .offset = (V3f){0.0f, 43.455f - 42.057f, 0.0f}
										),
	[MajulaToHuntsmanCopseBack]         = NEW_WARP_PT(
											.map = Majula, .obj_id = "o00_0501_0102",
											.pvp = true, .offset = (V3f){0.0f,  1.390f -  0.413f, 0.0f}
										),
	[MajulaToRotundaLockstoneBack]      = NEW_WARP_PT(.map = Majula, .obj_id = "o00_0501_0103", .pvp = true),
	[MajulaToGutterBack]                = NEW_WARP_PT(
											.map = Majula, .obj_id = "o00_0501_0104", .pvp = true,
											.offset = (V3f){0.0f, 71.218f - 69.941f, 0.0f}
										),
	[MajulaToShadedWoodsBack]           = NEW_WARP_PT(.map = Majula, .obj_id = "o00_0502_0103", .pvp = true),

	// Forest of the fallen giants
	[LastGiantEntryFront]                        = NEW_WARP_PT(.map = ForestOfTheFallenGiants, .obj_id = "o00_0501_0001"),
	[PursuerEntryFront]                          = NEW_WARP_PT(.map = ForestOfTheFallenGiants, .obj_id = "o00_0501_0003", .offset = (V3f){0.0f, 10.090f - 6.001f, 0.0f}),
	[PursuerExitFront]                           = NEW_WARP_PT(.map = ForestOfTheFallenGiants, .obj_id = "o00_0500_0004", .boss = ThePursuer, .cutscene_gate_id = PursuerEntryBack),
	[ForestOfFallenGiantsBalconyFront]           = NEW_WARP_PT(.map = ForestOfTheFallenGiants, .obj_id = "o00_0500_0000"),
	[ForestOfFallenGiantsToCaleFront]            = NEW_WARP_PT(.map = ForestOfTheFallenGiants, .obj_id = "o00_0500_0003"),
	[ForestOfFallenGiantsAfterFirstBonfireFront] = NEW_WARP_PT(.map = ForestOfTheFallenGiants, .obj_id = "o00_0500_0002"),
	[ForestOfFallenGiantsFromMajulaFront]        = NEW_WARP_PT(.map = ForestOfTheFallenGiants, .obj_id = "o00_0501_0002", .pvp = true, .reverse = true),
	[ForestOfFallenGiantsToPursuerArenaFront]    = NEW_WARP_PT(.map = ForestOfTheFallenGiants, .obj_id = "o00_0500_0005", .pvp = true, .reverse = true),

	[LastGiantEntryBack]                         = NEW_WARP_PT(.map = ForestOfTheFallenGiants, .obj_id = "o00_0501_0001", .boss = TheLastGiant),
	[PursuerEntryBack]                           = NEW_WARP_PT(.map = ForestOfTheFallenGiants, .obj_id = "o00_0501_0003", .boss = ThePursuer, .offset = (V3f){0.0f, 10.090f - 6.001f, 0.0f}),
	[PursuerExitBack]                            = NEW_WARP_PT(.map = ForestOfTheFallenGiants, .obj_id = "o00_0500_0004"),
	[ForestOfFallenGiantsBalconyBack]            = NEW_WARP_PT(.map = ForestOfTheFallenGiants, .obj_id = "o00_0500_0000"),
	[ForestOfFallenGiantsToCaleBack]             = NEW_WARP_PT(.map = ForestOfTheFallenGiants, .obj_id = "o00_0500_0003"),
	[ForestOfFallenGiantsAfterFirstBonfireBack]  = NEW_WARP_PT(.map = ForestOfTheFallenGiants, .obj_id = "o00_0500_0002"),
	[ForestOfFallenGiantsFromMajulaBack]         = NEW_WARP_PT(.map = ForestOfTheFallenGiants, .obj_id = "o00_0501_0002", .pvp = true, .reverse = true),
	[ForestOfFallenGiantsToPursuerArenaBack]     = NEW_WARP_PT(.map = ForestOfTheFallenGiants, .obj_id = "o00_0500_0005", .pvp = true, .reverse = true),

	// Brightstone cove tseldora
	[CongregationEntryFront]               = NEW_WARP_PT(.map = BrightstoneCoveTseldora, .obj_id = "o00_0500_0000"),
	[CongregationExitFront]                = NEW_WARP_PT(.map = BrightstoneCoveTseldora, .obj_id = "o00_0500_0001", .boss = ProwlingMagusAndCongregation),
	[BrightstoneCoveToDoorsOfPharrosFront] = NEW_WARP_PT(.map = BrightstoneCoveTseldora, .obj_id = "o00_0500_0002", .pvp = true),
	[DukesDearFreyjaEntryFront]            = NEW_WARP_PT(.map = BrightstoneCoveTseldora, .obj_id = "o00_0502_0000"),
	[DukesDearFreyjaExitFront]             = NEW_WARP_PT(.map = BrightstoneCoveTseldora, .obj_id = "o00_0501_0001", .boss = TheDukesDearFreja, .cutscene_gate_id = DukesDearFreyjaEntryBack, .reverse = true),

	[CongregationEntryBack]                = NEW_WARP_PT(.map = BrightstoneCoveTseldora, .obj_id = "o00_0500_0000", .boss = ProwlingMagusAndCongregation),
	[CongregationExitBack]                 = NEW_WARP_PT(.map = BrightstoneCoveTseldora, .obj_id = "o00_0500_0001"),
	[BrightstoneCoveToDoorsOfPharrosBack]  = NEW_WARP_PT(.map = BrightstoneCoveTseldora, .obj_id = "o00_0500_0002", .pvp = true),
	[DukesDearFreyjaEntryBack]             = NEW_WARP_PT(.map = BrightstoneCoveTseldora, .obj_id = "o00_0502_0000", .boss = TheDukesDearFreja),
	[DukesDearFreyjaExitBack]              = NEW_WARP_PT(.map = BrightstoneCoveTseldora, .obj_id = "o00_0501_0001", .reverse = true),

	// Aldias keep
	[GuardianDragonEntryFront] = NEW_WARP_PT(.map = AldiasKeep, .obj_id = "o00_0501_0000"),
	[GuardianDragonExitFront]  = NEW_WARP_PT(.map = AldiasKeep, .obj_id = "o00_0501_0001", .boss = GuardianDragon),
	[AldiasKeepEntranceFront]  = NEW_WARP_PT(.map = AldiasKeep, .obj_id = "o00_0501_0002", .pvp = true, .reverse = true),

	[GuardianDragonEntryBack]  = NEW_WARP_PT(.map = AldiasKeep, .obj_id = "o00_0501_0000", .boss = GuardianDragon),
	[GuardianDragonExitBack]   = NEW_WARP_PT(.map = AldiasKeep, .obj_id = "o00_0501_0001"),
	[AldiasKeepEntranceBack]   = NEW_WARP_PT(.map = AldiasKeep, .obj_id = "o00_0501_0002", .pvp = true, .reverse = true),

	// The lost bastille
	[GargoylesEntryFront]               = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0000"),
	[RuinSentinelsExitFront]            = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0001", .reverse = true, .boss = RuinSentinels),
	[LostBastilleToSinnersRiseFront]    = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0005"),
	[GargoylesExitFront]                = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0006", .boss = BelfryGargoyles),
	[RuinSentinelsHiddenDoor1ExitFront] = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0007", .boss = RuinSentinels),
	[RuinSentinelsHiddenDoor2ExitFront] = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0008", .boss = RuinSentinels),
	[RuinSentinelsHiddenDoor3ExitFront] = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0009", .boss = RuinSentinels),
	[RuinSentinelsHiddenDoor4ExitFront] = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0010", .boss = RuinSentinels),
	[RuinSentinesUpperExitBack]         = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0011", .reverse = true),
	[LostBastilleToShipFromWharfFront]  = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0012", .pvp = true, .reverse = true),
	[LostBastilleToBelfryLunaFront]     = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0013", .pvp = true, .reverse = true, .blocktype = WPBT_PharrosLockstone),
	[RuinSentinelsEntryFront]           = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0501_0000"),
	[LostSinnerEntryFront]              = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0501_0002"),
	[LostSinnerExitFront]               = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0501_0003"),

	[GargoylesEntryBack]                = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0000", .boss = BelfryGargoyles),
	[RuinSentinelsExitBack]             = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0001", .reverse = true),
	[LostBastilleToSinnersRiseBack]     = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0005"),
	[GargoylesExitBack]                 = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0006"),
	[RuinSentinelsHiddenDoor1ExitBack]  = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0007"),
	[RuinSentinelsHiddenDoor2ExitBack]  = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0008"),
	[RuinSentinelsHiddenDoor3ExitBack]  = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0009"),
	[RuinSentinelsHiddenDoor4ExitBack]  = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0010"),
	[RuinSentinesUpperExitFront]        = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0011", .reverse = true),
	[LostBastilleToShipFromWharfBack]   = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0012", .pvp = true, .reverse = true),
	[LostBastilleToBelfryLunaBack]      = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0013", .pvp = true, .reverse = true),
	[RuinSentinelsEntryBack]            = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0501_0000", .boss = RuinSentinels, .blocktype = WPBT_Inaccessible),
	[LostSinnerEntryBack]               = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0501_0002", .boss = TheLostSinner),
	[LostSinnerExitBack]                = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0501_0003"),

	// Harvest valley and earthen peak
	[CovetousDemonEntryFront]                  = NEW_WARP_PT(.map = HarvestValleyEarthenPeak, .obj_id = "o00_0500_0000"),
	[EarthenPeakNearWindmillBurnLocationFront] = NEW_WARP_PT(.map = HarvestValleyEarthenPeak, .obj_id = "o00_0500_0001"),
	[MythaEntryFront]                          = NEW_WARP_PT(.map = HarvestValleyEarthenPeak, .obj_id = "o00_0500_0002"),
	[CovetousDemonExitFront]                   = NEW_WARP_PT(.map = HarvestValleyEarthenPeak, .obj_id = "o00_0500_0003", .boss = CovetousDemon),
	[MythaExitFront]                           = NEW_WARP_PT(.map = HarvestValleyEarthenPeak, .obj_id = "o00_0501_0000", .boss = MythaTheBanefulQueen),
	[HarvestValleyToSkeletonLordsFront]        = NEW_WARP_PT(.map = HarvestValleyEarthenPeak, .obj_id = "o00_0501_0001", .pvp = true, .reverse = true),

	[CovetousDemonEntryBack]                   = NEW_WARP_PT(.map = HarvestValleyEarthenPeak, .obj_id = "o00_0500_0000", .boss = CovetousDemon),
	[EarthenPeakNearWindmillBurnLocationBack]  = NEW_WARP_PT(.map = HarvestValleyEarthenPeak, .obj_id = "o00_0500_0001"),
	[MythaEntryBack]                           = NEW_WARP_PT(.map = HarvestValleyEarthenPeak, .obj_id = "o00_0500_0002", .boss = MythaTheBanefulQueen),
	[CovetousDemonExitBack]                    = NEW_WARP_PT(.map = HarvestValleyEarthenPeak, .obj_id = "o00_0500_0003"),
	[MythaExitBack]                            = NEW_WARP_PT(.map = HarvestValleyEarthenPeak, .obj_id = "o00_0501_0000"),
	[HarvestValleyToSkeletonLordsBack]         = NEW_WARP_PT(.map = HarvestValleyEarthenPeak, .obj_id = "o00_0501_0001", .pvp = true, .reverse = true),

	// No mans wharf
	[FlexileSentryExitFront]   = NEW_WARP_PT(.map = NomansWharf, .obj_id = "o00_0500_0000", .boss = FlexileSentry),
	[FlexileSentryEntryFront]  = NEW_WARP_PT(.map = NomansWharf, .obj_id = "o00_0500_0001"),
	[NoMansWharfToHeidesFront] = NEW_WARP_PT(.map = NomansWharf, .obj_id = "o00_0501_0000", .pvp = true),

	[FlexileSentryExitBack]    = NEW_WARP_PT(.map = NomansWharf, .obj_id = "o00_0500_0000"),
	[FlexileSentryEntryBack]   = NEW_WARP_PT(.map = NomansWharf, .obj_id = "o00_0500_0001", .boss = FlexileSentry),
	[NoMansWharfToHeidesBack]  = NEW_WARP_PT(.map = NomansWharf, .obj_id = "o00_0501_0000", .pvp = true),

	// Iron keep
	[SmeltorDemonEntryFront]                  = NEW_WARP_PT(.map = IronKeepBelfrySol, .obj_id = "o00_0500_0000"),
	[IronKeepNearFlameThrowerAndTurtleFront]  = NEW_WARP_PT(.map = IronKeepBelfrySol, .obj_id = "o00_0500_0002"),
	[BelfrySolExitFront]                      = NEW_WARP_PT(.map = IronKeepBelfrySol, .obj_id = "o00_0500_0003"),
	[BelfrySolEntryFront]                     = NEW_WARP_PT(.map = IronKeepBelfrySol, .obj_id = "o00_0500_0004"),
	[OldIronKingEntryFront]                   = NEW_WARP_PT(.map = IronKeepBelfrySol, .obj_id = "o00_0500_0005"),
	[OldIronKingExitFront]                    = NEW_WARP_PT(.map = IronKeepBelfrySol, .obj_id = "o00_0500_0006", .boss = OldIronKing, .cutscene_gate_id = OldIronKingEntryBack),
	[SmeltorDemonExitFront]                   = NEW_WARP_PT(.map = IronKeepBelfrySol, .obj_id = "o00_0500_0007", .boss = SmelterDemon),
	[SmeltorDemonToBonfireFront]              = NEW_WARP_PT(.map = IronKeepBelfrySol, .obj_id = "o00_0500_0008", .pvp = true),
	[IronKeepToBelfryAtPharrosLockstoneFront] = NEW_WARP_PT(.map = IronKeepBelfrySol, .obj_id = "o00_0500_0010", .pvp = true, .reverse = true, .blocktype = WPBT_PharrosLockstone),
	[IronKeepToEarthenPeakFront]              = NEW_WARP_PT(.map = IronKeepBelfrySol, .obj_id = "o00_0501_0000", .pvp = true, .reverse = true),

	[SmeltorDemonEntryBack]                   = NEW_WARP_PT(.map = IronKeepBelfrySol, .obj_id = "o00_0500_0000", .boss = SmelterDemon),
	[IronKeepNearFlameThrowerAndTurtleBack]   = NEW_WARP_PT(.map = IronKeepBelfrySol, .obj_id = "o00_0500_0002"),
	[BelfrySolExitBack]                       = NEW_WARP_PT(.map = IronKeepBelfrySol, .obj_id = "o00_0500_0003"),
	[BelfrySolEntryBack]                      = NEW_WARP_PT(.map = IronKeepBelfrySol, .obj_id = "o00_0500_0004"),
	[OldIronKingEntryBack]                    = NEW_WARP_PT(.map = IronKeepBelfrySol, .obj_id = "o00_0500_0005", .boss = OldIronKing),
	[OldIronKingExitBack]                     = NEW_WARP_PT(.map = IronKeepBelfrySol, .obj_id = "o00_0500_0006"),
	[SmeltorDemonExitBack]                    = NEW_WARP_PT(.map = IronKeepBelfrySol, .obj_id = "o00_0500_0007"),
	[SmeltorDemonToBonfireBack]               = NEW_WARP_PT(.map = IronKeepBelfrySol, .obj_id = "o00_0500_0008", .pvp = true),
	[IronKeepToBelfryAtPharrosLockstoneBack]  = NEW_WARP_PT(.map = IronKeepBelfrySol, .obj_id = "o00_0500_0010", .pvp = true, .reverse = true),
	[IronKeepToEarthenPeakBack]               = NEW_WARP_PT(.map = IronKeepBelfrySol, .obj_id = "o00_0501_0000", .pvp = true, .reverse = true),

	// Huntsman Copse
	[SkeletonLordsEntryFront]    = NEW_WARP_PT(.map = HuntsmanCopseUndeadPurgatory, .obj_id = "o00_0501_0000"),
	[SkeletonLordsExitFront]     = NEW_WARP_PT(.map = HuntsmanCopseUndeadPurgatory, .obj_id = "o00_0501_0001", .boss = SkeletonLords),
	[ChariotEntryFront]          = NEW_WARP_PT(.map = HuntsmanCopseUndeadPurgatory, .obj_id = "o00_0501_0004"),
	[ChariotExitFront]           = NEW_WARP_PT(.map = HuntsmanCopseUndeadPurgatory, .obj_id = "o00_0501_0006", .boss = ExecutionersChariot, .cutscene_gate_id = ChariotEntryBack),
	[HuntsmanCopseToMajulaFront] = NEW_WARP_PT(.map = HuntsmanCopseUndeadPurgatory, .obj_id = "o00_0501_0007", .pvp = true, .reverse = true),

	[SkeletonLordsEntryBack]     = NEW_WARP_PT(.map = HuntsmanCopseUndeadPurgatory, .obj_id = "o00_0501_0000", .boss = SkeletonLords),
	[SkeletonLordsExitBack]      = NEW_WARP_PT(.map = HuntsmanCopseUndeadPurgatory, .obj_id = "o00_0501_0001"),
	[ChariotEntryBack]           = NEW_WARP_PT(.map = HuntsmanCopseUndeadPurgatory, .obj_id = "o00_0501_0004", .boss = ExecutionersChariot),
	[ChariotExitBack]            = NEW_WARP_PT(.map = HuntsmanCopseUndeadPurgatory, .obj_id = "o00_0501_0006"),
	[HuntsmanCopseToMajulaBack]  = NEW_WARP_PT(.map = HuntsmanCopseUndeadPurgatory, .obj_id = "o00_0501_0007", .pvp = true, .reverse = true),

	// The gutter and black gulch
	[GutterNearAntQueenFront]         = NEW_WARP_PT(.map = TheGutterBlackGulch, .obj_id = "o00_0500_0000"),
	[TheRottenExitFront]              = NEW_WARP_PT(.map = TheGutterBlackGulch, .obj_id = "o00_0500_0001", .boss = TheRotten, .cutscene_gate_id = TheRottenEntryBack),
	[BlackGulchEntranceFront]         = NEW_WARP_PT(.map = TheGutterBlackGulch, .obj_id = "o00_0501_0000"),
	[TheGutterFromGraveOfSaintsFront] = NEW_WARP_PT(.map = TheGutterBlackGulch, .obj_id = "o00_0501_0001", .pvp = true, .reverse = true),
	[TheRottenEntryFront]             = NEW_WARP_PT(.map = TheGutterBlackGulch, .obj_id = "o00_0503_0100"),

	[GutterNearAntQueenBack]          = NEW_WARP_PT(.map = TheGutterBlackGulch, .obj_id = "o00_0500_0000"),
	[TheRottenExitBack]               = NEW_WARP_PT(.map = TheGutterBlackGulch, .obj_id = "o00_0500_0001"),
	[BlackGulchEntranceBack]          = NEW_WARP_PT(.map = TheGutterBlackGulch, .obj_id = "o00_0501_0000"),
	[TheGutterFromGraveOfSaintsBack]  = NEW_WARP_PT(.map = TheGutterBlackGulch, .obj_id = "o00_0501_0001", .pvp = true, .reverse = true),
	[TheRottenEntryBack]              = NEW_WARP_PT(.map = TheGutterBlackGulch, .obj_id = "o00_0503_0100", .boss = TheRotten),


	// Dragon aerie
	[DragonAerieToAldiasKeepFront] = NEW_WARP_PT(.map = DragonAerieDragonShrine, .obj_id = "o00_0501_0000", .pvp = true, .reverse = true),
	[AncientDragonFront]           = NEW_WARP_PT(.map = DragonAerieDragonShrine, .obj_id = "o00_0502_0000"),

	[DragonAerieToAldiasKeepBack]  = NEW_WARP_PT(.map = DragonAerieDragonShrine, .obj_id = "o00_0501_0000", .pvp = true, .reverse = true),
	[AncientDragonBack]            = NEW_WARP_PT(.map = DragonAerieDragonShrine, .obj_id = "o00_0502_0000", .boss = AncientDragonBoss),

	// Shaded woods -> Majula
	[ShadedWoodsFromMajulaFront] = NEW_WARP_PT(.map = MajulaShadedWoods, .obj_id = "o00_0500_0100", .pvp = true),
	[ShadedWoodsFromMajulaBack]  = NEW_WARP_PT(.map = MajulaShadedWoods, .obj_id = "o00_0500_0100", .pvp = true),

	// Heides tower of flame
	[DragonriderEntryFront]     = NEW_WARP_PT(.map = HeidesTowerofFlame, .obj_id = "o00_0500_0000"),
	[DragonriderExitFront]      = NEW_WARP_PT(.map = HeidesTowerofFlame, .obj_id = "o00_0500_0001", .boss = Dragonrider),
	[HeidesToMajulaFront]       = NEW_WARP_PT(.map = HeidesTowerofFlame, .obj_id = "o00_0500_0002", .pvp = true),
	[HeidesToWharfFront]        = NEW_WARP_PT(.map = HeidesTowerofFlame, .obj_id = "o00_0500_0100", .pvp = true),
	[OldDragonslayerEntryFront] = NEW_WARP_PT(.map = HeidesTowerofFlame, .obj_id = "o00_0501_0000"),
	[OldDragonslayerExitFront]  = NEW_WARP_PT(.map = HeidesTowerofFlame, .obj_id = "o00_0501_0001", .boss = OldDragonslayer),

	[DragonriderEntryBack]      = NEW_WARP_PT(.map = HeidesTowerofFlame, .obj_id = "o00_0500_0000", .boss = Dragonrider),
	[DragonriderExitBack]       = NEW_WARP_PT(.map = HeidesTowerofFlame, .obj_id = "o00_0500_0001"),
	[HeidesToMajulaBack]        = NEW_WARP_PT(.map = HeidesTowerofFlame, .obj_id = "o00_0500_0002", .pvp = true),
	[HeidesToWharfBack]         = NEW_WARP_PT(.map = HeidesTowerofFlame, .obj_id = "o00_0500_0100", .pvp = true),
	[OldDragonslayerEntryBack]  = NEW_WARP_PT(.map = HeidesTowerofFlame, .obj_id = "o00_0501_0000", .boss = OldDragonslayer),
	[OldDragonslayerExitBack]   = NEW_WARP_PT(.map = HeidesTowerofFlame, .obj_id = "o00_0501_0001"),

	// Shaded woods -> Shrine of winter
	[ScorpionessNajkaExitFront]   = NEW_WARP_PT(.map = ShadedWoodsShrineofWinter, .obj_id = "o00_0500_0000", .boss = ScorpionessNajka),
	[ShadedWoodsToMistyAreaFront] = NEW_WARP_PT(.map = ShadedWoodsShrineofWinter, .obj_id = "o00_0501_0001", .pvp = true, .reverse = true),
	[ScorpionessNajkaEntryFront]  = NEW_WARP_PT(.map = ShadedWoodsShrineofWinter, .obj_id = "o00_0503_0000", .offset = (V3f){0.0f, -85.080f + 85.672f, 0.0f}),

	[ScorpionessNajkaExitBack]    = NEW_WARP_PT(.map = ShadedWoodsShrineofWinter, .obj_id = "o00_0500_0000"),
	[ShadedWoodsToMistyAreaBack]  = NEW_WARP_PT(.map = ShadedWoodsShrineofWinter, .obj_id = "o00_0501_0001", .pvp = true, .reverse = true),
	[ScorpionessNajkaEntryBack]   = NEW_WARP_PT(.map = ShadedWoodsShrineofWinter, .obj_id = "o00_0503_0000", .offset = (V3f){0.0f, -85.080f + 85.672f, 0.0f}, .boss = ScorpionessNajka),


	// Door of pharros
	[RoyalRatAuthorityExitFront]           = NEW_WARP_PT(.map = DoorsofPharros, .obj_id = "o00_0500_0000", .boss = RoyalRatAuthority),
	[DoorOfPharrosToOrdealEndBonfireFront] = NEW_WARP_PT(.map = DoorsofPharros, .obj_id = "o00_0500_0001", .pvp = true),
	[DoorOfPharrosToRatKingDomainFront]    = NEW_WARP_PT(.map = DoorsofPharros, .obj_id = "o00_0500_0002", .pvp = true),
	[RoyalRatAuthorityEntryFront]          = NEW_WARP_PT(.map = DoorsofPharros, .obj_id = "o00_0501_0000"),

	[RoyalRatAuthorityExitBack]            = NEW_WARP_PT(.map = DoorsofPharros, .obj_id = "o00_0500_0000"),
	[DoorOfPharrosToOrdealEndBonfireBack]  = NEW_WARP_PT(.map = DoorsofPharros, .obj_id = "o00_0500_0001", .pvp = true),
	[DoorOfPharrosToRatKingDomainBack]     = NEW_WARP_PT(.map = DoorsofPharros, .obj_id = "o00_0500_0002", .pvp = true),
	[RoyalRatAuthorityEntryBack]           = NEW_WARP_PT(.map = DoorsofPharros, .obj_id = "o00_0501_0000", .boss = RoyalRatAuthority),

	// Grave of saints
	[GraveOfSaintsFromPitFront]          = NEW_WARP_PT(.map = GraveofSaints, .obj_id = "o00_0500_0000", .pvp = true, .reverse = true),
	[GraveOfSaintsNearUpperBonfireFront] = NEW_WARP_PT(.map = GraveofSaints, .obj_id = "o00_0501_0000", .pvp = true),
	[RoyalRatVanguardEntryFront]         = NEW_WARP_PT(.map = GraveofSaints, .obj_id = "o00_0501_0001"),
	[RoyalRatVanguardExitFront]          = NEW_WARP_PT(.map = GraveofSaints, .obj_id = "o00_0501_0002", .boss = RoyalRatVanguard),

	[GraveOfSaintsFromPitBack]           = NEW_WARP_PT(.map = GraveofSaints, .obj_id = "o00_0500_0000", .pvp = true, .reverse = true),
	[GraveOfSaintsNearUpperBonfireBack]  = NEW_WARP_PT(.map = GraveofSaints, .obj_id = "o00_0501_0000", .pvp = true),
	[RoyalRatVanguardEntryBack]          = NEW_WARP_PT(.map = GraveofSaints, .obj_id = "o00_0501_0001", .boss = RoyalRatVanguard),
	[RoyalRatVanguardExitBack]           = NEW_WARP_PT(.map = GraveofSaints, .obj_id = "o00_0501_0002"),

	// Memory of vammarBack, orro and jeigh
	[GiantLordEntryFront] = NEW_WARP_PT(.map = MemoryofVammarOrroandJeigh, .obj_id = "o00_0500_0000"),
	[GiantLordExitFront]  = NEW_WARP_PT(.map = MemoryofVammarOrroandJeigh, .obj_id = "o00_0500_0001", .boss = GiantLord),

	[GiantLordEntryBack]  = NEW_WARP_PT(.map = MemoryofVammarOrroandJeigh, .obj_id = "o00_0500_0000", .boss = GiantLord),
	[GiantLordExitBack]   = NEW_WARP_PT(.map = MemoryofVammarOrroandJeigh, .obj_id = "o00_0500_0001"),

	// Shrine of amana
	[DemonOfSongEntryFront]               = NEW_WARP_PT(.map = ShrineofAmana, .obj_id = "o00_0501_0000"),
	[ShrineOfAmanaTo2ndBonfireFront]      = NEW_WARP_PT(.map = ShrineofAmana, .obj_id = "o00_0501_0001"),
	[ShrineOfAmanaTo3rdBonfireFront]      = NEW_WARP_PT(.map = ShrineofAmana, .obj_id = "o00_0501_0005"),
	[DemonOfSongExitFront]                = NEW_WARP_PT(.map = ShrineofAmana, .obj_id = "o00_0501_0006", .offset = (V3f){0.0f, 32.121f - 31.336f, 0.0f}, .boss = DemonofSong),
	[ShrineOfAmanaToDrangleicCastleFront] = NEW_WARP_PT(.map = ShrineofAmana, .obj_id = "o00_0501_0007", .pvp = true, .reverse = true),

	[DemonOfSongEntryBack]                = NEW_WARP_PT(.map = ShrineofAmana, .obj_id = "o00_0501_0000", .boss = DemonofSong),
	[ShrineOfAmanaTo2ndBonfireBack]       = NEW_WARP_PT(.map = ShrineofAmana, .obj_id = "o00_0501_0001"),
	[ShrineOfAmanaTo3rdBonfireBack]       = NEW_WARP_PT(.map = ShrineofAmana, .obj_id = "o00_0501_0005"),
	[DemonOfSongExitBack]                 = NEW_WARP_PT(.map = ShrineofAmana, .obj_id = "o00_0501_0006", .offset = (V3f){0.0f, 32.121f - 31.336f, 0.0f}),
	[ShrineOfAmanaToDrangleicCastleBack]  = NEW_WARP_PT(.map = ShrineofAmana, .obj_id = "o00_0501_0007", .pvp = true, .reverse = true),

	// Drangleic castle
	[LookingGlassKnightEntryFront]      = NEW_WARP_PT(.map = DrangleicCastleThroneofWant, .obj_id = "o00_0501_0000"),
	[TwinDragonridersEntryFront]        = NEW_WARP_PT(.map = DrangleicCastleThroneofWant, .obj_id = "o00_0501_0001"),
	[TwinDragonridersExitFront]         = NEW_WARP_PT(.map = DrangleicCastleThroneofWant, .obj_id = "o00_0501_0004", .boss = TwinDragonrider),
	[LookingGlassKnightExitFront]       = NEW_WARP_PT(.map = DrangleicCastleThroneofWant, .obj_id = "o00_0501_0005", .offset = (V3f){0.0f, 75.655434f - 75.142822f, 0.0f}, .boss = LookingGlassKnight),
	[FinalFightArenaFront]              = NEW_WARP_PT(.map = DrangleicCastleThroneofWant, .obj_id = "o00_0501_0006"),
	[DrangleicCastleToChasmPortalFront] = NEW_WARP_PT(.map = DrangleicCastleThroneofWant, .obj_id = "o00_0501_0008", .pvp = true, .offset = (V3f){-437.288f + 436.508f, -70.787f + 72.757f, 0.0f}),
	[DrangleicCastleToShadedWoodsFront] = NEW_WARP_PT(.map = DrangleicCastleThroneofWant, .obj_id = "o00_0504_0000", .pvp = true, .reverse = true),

	[LookingGlassKnightEntryBack]       = NEW_WARP_PT(.map = DrangleicCastleThroneofWant, .obj_id = "o00_0501_0000", .boss = LookingGlassKnight),
	[TwinDragonridersEntryBack]         = NEW_WARP_PT(.map = DrangleicCastleThroneofWant, .obj_id = "o00_0501_0001", .boss = TwinDragonrider),
	[TwinDragonridersExitBack]          = NEW_WARP_PT(.map = DrangleicCastleThroneofWant, .obj_id = "o00_0501_0004"),
	[LookingGlassKnightExitBack]        = NEW_WARP_PT(.map = DrangleicCastleThroneofWant, .obj_id = "o00_0501_0005", .offset = (V3f){0.0f, 75.655434f - 75.142822f, 0.0f}),
	[FinalFightArenaBack]               = NEW_WARP_PT(.map = DrangleicCastleThroneofWant, .obj_id = "o00_0501_0006", .boss = ThroneWatcherAndDefender),
	[DrangleicCastleToChasmPortalBack]  = NEW_WARP_PT(.map = DrangleicCastleThroneofWant, .obj_id = "o00_0501_0008", .pvp = true, .offset = (V3f){-437.288f + 436.508f, -70.787f + 72.757f, 0.0f}),
	[DrangleicCastleToShadedWoodsBack]  = NEW_WARP_PT(.map = DrangleicCastleThroneofWant, .obj_id = "o00_0504_0000", .pvp = true, .reverse = true),

	[UndeadCryptToShrineOfAmanaFront]      = NEW_WARP_PT(.map = UndeadCrypt, .obj_id = "o00_0500_0000"),
	[VelsdatEntryFront]                    = NEW_WARP_PT(.map = UndeadCrypt, .obj_id = "o00_0501_0000"),
	[VelsdatExitFront]                     = NEW_WARP_PT(.map = UndeadCrypt, .obj_id = "o00_0501_0001", .boss = VelstadtTheRoyalAegis, .cutscene_gate_id = VelsdatEntryBack),
	[UndeadCryptFromAgdayneToBonfireFront] = NEW_WARP_PT(.map = UndeadCrypt, .obj_id = "o00_0502_0000", .pvp = true),
	[VendrickFront]                        = NEW_WARP_PT(.map = UndeadCrypt, .obj_id = "o00_0505_0000"),
	
	[UndeadCryptToShrineOfAmanaBack]       = NEW_WARP_PT(.map = UndeadCrypt, .obj_id = "o00_0500_0000"),
	[VelsdatEntryBack]                     = NEW_WARP_PT(.map = UndeadCrypt, .obj_id = "o00_0501_0000", .boss = VelstadtTheRoyalAegis),
	[VelsdatExitBack]                      = NEW_WARP_PT(.map = UndeadCrypt, .obj_id = "o00_0501_0001"),
	[UndeadCryptFromAgdayneToBonfireBack]  = NEW_WARP_PT(.map = UndeadCrypt, .obj_id = "o00_0502_0000", .pvp = true),
	[VendrickBack]                         = NEW_WARP_PT(.map = UndeadCrypt, .obj_id = "o00_0505_0000", .boss = VendrickBoss),
	
	// Dark chasm of old
	[DarkChasmFromBlackGulchExitFront]      = NEW_WARP_PT(.map = DarkChasmofOld, .obj_id = "o00_0501_0000"),
	[DarkChasmFromDrangleicCastleExitFront] = NEW_WARP_PT(.map = DarkChasmofOld, .obj_id = "o00_0501_0001"),
	[DarkChasmFromShadedWoodsExitFront]     = NEW_WARP_PT(.map = DarkChasmofOld, .obj_id = "o00_0501_0002"),
	[DarkLurkerExitFront]                   = NEW_WARP_PT(.map = DarkChasmofOld, .obj_id = "o00_0501_0003", .offset = (V3f){0.0f, 13.792f - 11.966f, 0.0f}, .boss = Darklurker),

	[DarkChasmFromBlackGulchExitBack]       = NEW_WARP_PT(.map = DarkChasmofOld, .obj_id = "o00_0501_0000"),
	[DarkChasmFromDrangleicCastleExitBack]  = NEW_WARP_PT(.map = DarkChasmofOld, .obj_id = "o00_0501_0001"),
	[DarkChasmFromShadedWoodsExitBack]      = NEW_WARP_PT(.map = DarkChasmofOld, .obj_id = "o00_0501_0002"),
	[DarkLurkerExitBack]                    = NEW_WARP_PT(.map = DarkChasmofOld, .obj_id = "o00_0501_0003", .offset = (V3f){0.0f, 13.792f - 11.966f, 0.0f}),

	// Shulva sanctum city
	[SihnTheSlumberingDragonEntryFront] = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0500_0001", .offset = (V3f){0.0f, 79.928f - 79.436f, 0.0f}),
	[ElanaTheSqualidQueenEntryFront]    = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0000", .offset = (V3f){0.0f, 71.928f - 71.637f, 0.0f}),
	[GankSquadBossEntryFront]           = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0002"),
	[GankSquadBossExitFront]            = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0003", .boss = Gankfight),
	[ShulvaToGankFight1Front]           = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0005", .pvp = true),
	[ShulvaEntranceFront]               = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0007", .pvp = true),
	[BetweenElanaAndSihnFront]          = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0008"),
	[ShulvaToGankFight2Front]           = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0011"),
	[NearJesterThomasFront]             = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0012", .offset = (V3f){0.0f, 41.540f - 41.289f, 0.0f}),

	[SihnTheSlumberingDragonEntryBack]  = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0500_0001", .offset = (V3f){0.0f, 79.928f - 79.436f, 0.0f}, .boss = SinhTheSlumberingDragon),
	[ElanaTheSqualidQueenEntryBack]     = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0000", .offset = (V3f){0.0f, 71.928f - 71.637f, 0.0f}, .boss = ElanaTheSqualidQueen),
	[GankSquadBossEntryBack]            = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0002", .boss = Gankfight),
	[GankSquadBossExitBack]             = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0003"),
	[ShulvaToGankFight1Back]            = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0005", .pvp = true),
	[ShulvaEntranceBack]                = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0007", .pvp = true),
	[BetweenElanaAndSihnBack]           = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0010", .offset = (V3f){0.0f, 5.0f * (74.744f - 73.291f) / 12.0f, 0.0f}, .reverse = true),
	[ShulvaToGankFight2Back]            = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0011"),
	[NearJesterThomasBack]              = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0012", .offset = (V3f){0.0f, 41.540f - 41.289f, 0.0f}),


	// Brume Tower
	[BrumeTowerFrom2ndBonfireToCentralRoomFront] = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0500_0000"),
	[BrumeTowerToFoyerBonfireFromOutsideFront]   = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0500_0001"),
	[BrumeTowerToScorchingIronSceptorKeyFront]   = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0500_0002"),
	[SirAlonneEntryFront]                        = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0500_0003"),
	[SirAlonneExitFront]                         = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0500_0004", .boss = SirAlonne),
	[FumeKnightEntryFront]                       = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0501_0000", .offset = (V3f){0.0f, 59.822f - 59.297f, 0.0f}),
	[FumeKnightExitFront]                        = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0501_0001", .reverse = true, .offset = (V3f){0.0f, 61.497f - 60.998f, 0.0f}, .boss = FumeKnight),
	[BlueSmeltorDemonEntryFront]                 = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0501_0002", .offset = (V3f){0.0f, 26.875f - 26.556f, 0.0f}),
	[BlueSmeltorDemonExitFront]                  = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0501_0003", .boss = BlueSmelterDemon),
	[BrumeTowerEntranceFromLiftFront]            = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0501_0006"),
	[BrumeTowerToBlueSmelterDungeonFront]        = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0501_0007"),

	[BrumeTowerFrom2ndBonfireToCentralRoomBack]  = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0500_0000"),
	[BrumeTowerToFoyerBonfireFromOutsideBack]    = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0500_0001"),
	[BrumeTowerToScorchingIronSceptorKeyBack]    = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0500_0002"),
	[SirAlonneEntryBack]                         = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0500_0003", .boss = SirAlonne),
	[SirAlonneExitBack]                          = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0500_0004"),
	[FumeKnightEntryBack]                        = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0501_0000", .offset = (V3f){0.0f, 59.822f - 59.297f, 0.0f}, .boss = FumeKnight),
	[FumeKnightExitBack]                         = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0501_0001", .reverse = true, .offset = (V3f){0.0f, 61.497f - 60.998f, 0.0f}),
	[BlueSmeltorDemonEntryBack]                  = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0501_0002", .offset = (V3f){0.0f, 26.875f - 26.556f, 0.0f}, .boss = BlueSmelterDemon),
	[BlueSmeltorDemonExitBack]                   = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0501_0003"),
	[BrumeTowerEntranceFromLiftBack]             = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0501_0006"),
	[BrumeTowerToBlueSmelterDungeonBack]         = NEW_WARP_PT(.map = BrumeTower, .obj_id = "o00_0501_0007"),


	// Frozen eleum loyce
	[AavaTheKingsPetExitFront]                  = NEW_WARP_PT(.map = FrozenEleumLoyce, .obj_id = "o00_0500_0001", .boss = AavaTheKingsPet),
	[EleumLoyceAfterGettingEyeToSeeGhostsFront] = NEW_WARP_PT(.map = FrozenEleumLoyce, .obj_id = "o00_0500_0002"),
	[IvoryKingFront]                            = NEW_WARP_PT(.map = FrozenEleumLoyce, .obj_id = "o00_0500_0003"),
	[LudAndZallenEntryFront]                    = NEW_WARP_PT(.map = FrozenEleumLoyce, .obj_id = "o00_0501_0003"),
	[AavaTheKingsPetEntryFront]                 = NEW_WARP_PT(.map = FrozenEleumLoyce, .obj_id = "o00_0501_0005"),
	[LudAndZallenExitFront]                     = NEW_WARP_PT(.map = FrozenEleumLoyce, .obj_id = "o00_0501_0006", .boss = LudAndZallen),
	[EleumLoyceCathedraEntranceFront]           = NEW_WARP_PT(.map = FrozenEleumLoyce, .obj_id = "o00_0501_0007", .pvp = true, .offset = (V3f){0.0f, 25.841f - 23.046f, 0.0f}),
	
	[AavaTheKingsPetExitBack]                   = NEW_WARP_PT(.map = FrozenEleumLoyce, .obj_id = "o00_0500_0001"),
	[EleumLoyceAfterGettingEyeToSeeGhostsBack]  = NEW_WARP_PT(.map = FrozenEleumLoyce, .obj_id = "o00_0500_0002"),
	[IvoryKingBack]                             = NEW_WARP_PT(.map = FrozenEleumLoyce, .obj_id = "o00_0500_0003", .boss = BurntIvoryKing, .blocktype = WPBT_Inaccessible),
	[LudAndZallenEntryBack]                     = NEW_WARP_PT(.map = FrozenEleumLoyce, .obj_id = "o00_0501_0003", .boss = LudAndZallen),
	[AavaTheKingsPetEntryBack]                  = NEW_WARP_PT(.map = FrozenEleumLoyce, .obj_id = "o00_0501_0005", .boss = AavaTheKingsPet),
	[LudAndZallenExitBack]                      = NEW_WARP_PT(.map = FrozenEleumLoyce, .obj_id = "o00_0501_0006"),
	[EleumLoyceCathedraEntranceBack]            = NEW_WARP_PT(.map = FrozenEleumLoyce, .obj_id = "o00_0501_0007", .pvp = true, .offset = (V3f){0.0f, 25.841f - 23.046f, 0.0f}),
};

static_assert(ARRAY_LEN(warp_points) == __WarpNodeCount, "Missing warp point definition");


typedef enum ConnectionType {
	Conn_None,
	Conn_FogGate,
	Conn_RotundaLockstone,
	Conn_SoldiersKey,
	Conn_KingsRing,
	Conn_FirstFourSouls,
	Conn_Catring,
	Conn_AshenMistHeart,
	Conn_OldKeyInBastille,
	Conn_BastilleKey,
	Conn_PharrosLockstone,
	Conn_ScorchingIronSceptor,
	Conn_ActivateBrumeTower,
	Conn_DarkCovenentJoined,
	Conn_ForgottenKey,
	Conn_DLC1Key,
	Conn_DLC2Key,
	Conn_DLC3Key,
	Conn_ShulvaSanctumKey,
	Conn_ActivateDragonStone,
	Conn_DLC3Eye,
	Conn_DLC3Unfreezed,
	Conn_FrigidOutskirtsKey,
	Conn_ShipBellRang,
	Conn_IronKeyToFireLizards,
	Conn_DragonStone,
	Conn_LudAndZallenDead,
	Conn_Firebomb,
	Conn_Gilligan_Ladder,
	Conn_KeyToKingsPassage,
	Conn_HouseKey,
	Conn_AldiaKey,
	Conn_FangKey,
	Conn_VendrickSoul,
	Conn_BowCrossbow,
	Conn_BrumeTowerKey,
	Conn_Torch,
	Conn_LenigrastKey,
	Conn_TseldoraDenKey,
	Conn_FireDefense,
	Conn_SmelterWedge,
	Conn_BrightstoneKey,
	Conn_RatTail,
	Conn_TokenOfFidelity,
	Conn_TokenOfSpite,
	Conn_DefeatNajka,
	Conn_RingOfWhispers,
	Conn_PateCreightonQuest,
	Conn_DullEmber,
	Conn_UndeadLockawayKey,
	Conn_Tutorial3Branch,
	Conn_RosabethBranch,
	Conn_WharfBranch,
	Conn_BastilleBranch,
	Conn_StraidBranch,
	Conn_FangKeyBranch,
	Conn_RepairSpellBranch,
	Conn_WarlockMaskBranch,
	Conn_SpellCastSetBranch,
	Conn_VengralBodyBranch,
	Conn_TarkBranch,
	Conn_GulchBranch,
	Conn_AmanaBranch,
	Conn_OgreBranch,
	Conn_AldiaBranch1,
	Conn_AldiaBranch2,
	Conn_DragonAerieBranch,
} ConnectionType;

typedef unsigned __int128 bitmask128_t;

#define CT(value) (bitmask128_t)1 << (value)

void print_connection_type(ConnectionType ct) {
	switch (ct) {
		case Conn_None                 : printf("None");                    break;
		case Conn_FogGate              : printf("FogGate");                 break;
		case Conn_RotundaLockstone     : printf("RotundaLockstone");        break;
		case Conn_SoldiersKey          : printf("SoldiersKey");             break;
		case Conn_KingsRing            : printf("KingsRing");               break;
		case Conn_FirstFourSouls       : printf("FirstFourSouls");          break;
		case Conn_Catring              : printf("Catring");                 break;
		case Conn_AshenMistHeart       : printf("AshenMistHeart");          break;
		case Conn_OldKeyInBastille     : printf("OldKeyInBastille");        break;
		case Conn_BastilleKey          : printf("BastilleKey");             break;
		case Conn_PharrosLockstone     : printf("PharrosLockstone");        break;
		case Conn_ScorchingIronSceptor : printf("ScorchingIronSceptor");    break;
		case Conn_ActivateBrumeTower   : printf("ActivateBrumeTower");      break;
		case Conn_DarkCovenentJoined   : printf("DarkCovenentJoined");      break;
		case Conn_ForgottenKey         : printf("ForgottenKey");            break;
		case Conn_DLC1Key              : printf("DLC1Key");                 break;
		case Conn_DLC2Key              : printf("DLC2Key");                 break;
		case Conn_DLC3Key              : printf("DLC3Key");                 break;
		case Conn_ShulvaSanctumKey     : printf("ShulvaSanctumKey");        break;
		case Conn_ActivateDragonStone  : printf("ActivateDragonStone");     break;
		case Conn_DLC3Eye              : printf("DLC3Eye");                 break;
		case Conn_DLC3Unfreezed        : printf("DLC3Unfreezed");           break;
		case Conn_FrigidOutskirtsKey   : printf("FrigidOutskirtsKey");      break;
		case Conn_ShipBellRang         : printf("ShipBellRang");            break;
		case Conn_IronKeyToFireLizards : printf("IronKeyToFireLizards");    break;
		case Conn_DragonStone          : printf("DragonStone");             break;
		case Conn_LudAndZallenDead     : printf("LudAndZallenDead");        break;
		case Conn_Firebomb             : printf("Firebomb");                break;
		case Conn_Gilligan_Ladder      : printf("Gilligan_Ladder");         break;
		case Conn_KeyToKingsPassage    : printf("KeyToKingsPassage");       break;
		case Conn_HouseKey             : printf("HouseKey");                break;
		case Conn_AldiaKey             : printf("AldiaKey");                break;
		case Conn_FangKey              : printf("FangKey");                 break;
		case Conn_VendrickSoul         : printf("VendrickSoul");            break;
		case Conn_BowCrossbow          : printf("BowCrossbow");             break;
		case Conn_BrumeTowerKey        : printf("BrumeTowerKey");           break;
		case Conn_Torch                : printf("Torch");                   break;
		case Conn_LenigrastKey         : printf("LenigrastKey");            break;
		case Conn_TseldoraDenKey       : printf("TseldoraDenKey");          break;
		case Conn_FireDefense          : printf("FireDefense");             break;
		case Conn_SmelterWedge         : printf("SmelterWedge");            break;
		case Conn_BrightstoneKey       : printf("BrightstoneKey");          break;
		case Conn_RatTail              : printf("RatTail");                 break;
		case Conn_TokenOfFidelity      : printf("TokenOfFidelity");         break;
		case Conn_TokenOfSpite         : printf("TokenOfSpite");            break;
		case Conn_DefeatNajka          : printf("DefeatNajka");             break;
		case Conn_RingOfWhispers       : printf("RingOfWhispers");          break;
		case Conn_PateCreightonQuest   : printf("PateCreightonQuest");      break;
		case Conn_DullEmber            : printf("DullEmber");               break;
		case Conn_UndeadLockawayKey    : printf("UndeadLockawayKey");       break;
		case Conn_Tutorial3Branch      : printf("Conn_Tutorial3Branch");    break;
		case Conn_RosabethBranch       : printf("Conn_RosabethBranch");     break;
		case Conn_WharfBranch          : printf("Conn_WharfBranch");        break;
		case Conn_BastilleBranch       : printf("Conn_BastilleBranch");     break;
		case Conn_StraidBranch         : printf("Conn_StraidBranch");       break;
		case Conn_FangKeyBranch        : printf("Conn_FangKeyBranch");      break;
		case Conn_RepairSpellBranch    : printf("Conn_RepairSpellBranch");  break;
		case Conn_WarlockMaskBranch    : printf("Conn_WarlockMaskBranch");  break;
		case Conn_SpellCastSetBranch   : printf("Conn_SpellCastSetBranch"); break;
		case Conn_VengralBodyBranch    : printf("Conn_VengralBodyBranch");  break;
		case Conn_TarkBranch           : printf("Conn_TarkBranch");         break;
		case Conn_GulchBranch          : printf("Conn_GulchBranch");        break;
		case Conn_AmanaBranch          : printf("Conn_AmanaBranch");        break;
		case Conn_OgreBranch           : printf("Conn_OgreBranch");         break;
		case Conn_AldiaBranch1         : printf("Conn_AldiaBranch1");       break;
		case Conn_AldiaBranch2         : printf("Conn_AldiaBranch2");       break;
		case Conn_DragonAerieBranch    : printf("Conn_DragonAerieBranch");  break;
	}
}

#define MAX_CONNECTION_TYPES 34

typedef struct {
	WarpNode to;
	bitmask128_t type;
	bool is_warp;
} Connection;

typedef struct {
	Connection *items;
	size_t count;
	size_t capacity;
} ConnectionGroup;

typedef struct {
	ConnectionGroup *items;
	size_t count;
	size_t capacity;
} Connections;

Connections connections = {0};

#define connection_init(from, n_elem) do {\
	assert(connections.items[from].count == 0);\
	da_init(&connections.items[from], n_elem);\
	connections.count++;\
} while (0)

#define connection_add(from, ...) da_append(&connections.items[(from)], ((Connection){.is_warp = false, .type = Conn_None, __VA_ARGS__}));

void generate_connections() {
	// assert(connections.items == NULL); // TODO: fix this 
	if (connections.capacity != __WarpNodeCount) da_init(&connections, __WarpNodeCount);
	memset(connections.items, 0, sizeof(*connections.items)*connections.capacity);
	size_t warp_node_count = 0;

	connection_init(GameStartSpawnSrc, 1);
	connection_add(GameStartSpawnSrc, .to = GameStartSpawnDst, .is_warp = true);
	warp_node_count++;


	// things betwixt

	connection_init(GameStartSpawnDst, 8);
	connection_add(GameStartSpawnDst, .to = Tutorial1EntryFront);
	connection_add(GameStartSpawnDst, .to = Tutorial2EntryFront);
	connection_add(GameStartSpawnDst, .to = Tutorial3EntryFront, .type = CT(Conn_Tutorial3Branch));
	connection_add(GameStartSpawnDst, .to = MajulaToForestOfFallenGiantsFront);
	connection_add(GameStartSpawnDst, .to = MajulaToGraveOfSaintsFront, .type = CT(Conn_Catring)); // TODO: cannot add Conn_Gilligan_Ladder to this type
	connection_add(GameStartSpawnDst, .to = MajulaToRotundaLockstoneFront);
	connection_add(GameStartSpawnDst, .to = MajulaToGutterFront, .type = CT(Conn_Catring));
	connection_add(GameStartSpawnDst, .to = MajulaToShadedWoodsFront);
	warp_node_count++;

	connection_init(Tutorial1EntryFront, 7);
	connection_add(Tutorial1EntryFront, .to = Tutorial2EntryFront);
	connection_add(Tutorial1EntryFront, .to = Tutorial3EntryFront, .type = CT(Conn_Tutorial3Branch));
	connection_add(Tutorial1EntryFront, .to = MajulaToForestOfFallenGiantsFront);
	connection_add(Tutorial1EntryFront, .to = MajulaToGraveOfSaintsFront, .type = CT(Conn_Catring));
	connection_add(Tutorial1EntryFront, .to = MajulaToRotundaLockstoneFront);
	connection_add(Tutorial1EntryFront, .to = MajulaToGutterFront, .type = CT(Conn_Catring));
	connection_add(Tutorial1EntryFront, .to = MajulaToShadedWoodsFront);
	warp_node_count++;

	connection_init(Tutorial2EntryFront, 7);
	connection_add(Tutorial2EntryFront, .to = Tutorial1EntryFront);
	connection_add(Tutorial2EntryFront, .to = Tutorial3EntryFront, .type = CT(Conn_Tutorial3Branch));
	connection_add(Tutorial2EntryFront, .to = MajulaToForestOfFallenGiantsFront);
	connection_add(Tutorial2EntryFront, .to = MajulaToGraveOfSaintsFront, .type = CT(Conn_Catring));
	connection_add(Tutorial2EntryFront, .to = MajulaToRotundaLockstoneFront);
	connection_add(Tutorial2EntryFront, .to = MajulaToGutterFront, .type = CT(Conn_Catring));
	connection_add(Tutorial2EntryFront, .to = MajulaToShadedWoodsFront);
	warp_node_count++;

	connection_init(Tutorial3EntryFront, 7);
	connection_add(Tutorial3EntryFront, .to = Tutorial1EntryFront, .type = CT(Conn_Tutorial3Branch));
	connection_add(Tutorial3EntryFront, .to = Tutorial2EntryFront, .type = CT(Conn_Tutorial3Branch));
	connection_add(Tutorial3EntryFront, .to = MajulaToForestOfFallenGiantsFront, .type = CT(Conn_Tutorial3Branch));
	connection_add(Tutorial3EntryFront, .to = MajulaToGraveOfSaintsFront, .type = CT(Conn_Catring) | CT(Conn_Tutorial3Branch));
	connection_add(Tutorial3EntryFront, .to = MajulaToRotundaLockstoneFront, .type = CT(Conn_Tutorial3Branch));
	connection_add(Tutorial3EntryFront, .to = MajulaToGutterFront, .type = CT(Conn_Catring) | CT(Conn_Tutorial3Branch));
	connection_add(Tutorial3EntryFront, .to = MajulaToShadedWoodsFront, .type = CT(Conn_Tutorial3Branch));
	warp_node_count++;

	connection_init(MajulaToForestOfFallenGiantsFront, 7);
	connection_add(MajulaToForestOfFallenGiantsFront, .to = Tutorial1EntryFront);
	connection_add(MajulaToForestOfFallenGiantsFront, .to = Tutorial2EntryFront);
	connection_add(MajulaToForestOfFallenGiantsFront, .to = Tutorial3EntryFront, .type = CT(Conn_Tutorial3Branch));
	connection_add(MajulaToForestOfFallenGiantsFront, .to = MajulaToGraveOfSaintsFront, .type = CT(Conn_Catring));
	connection_add(MajulaToForestOfFallenGiantsFront, .to = MajulaToRotundaLockstoneFront);
	connection_add(MajulaToForestOfFallenGiantsFront, .to = MajulaToGutterFront, .type = CT(Conn_Catring));
	connection_add(MajulaToForestOfFallenGiantsFront, .to = MajulaToShadedWoodsFront);
	warp_node_count++;

	connection_init(MajulaToGraveOfSaintsFront, 1);
	connection_add(MajulaToGraveOfSaintsFront, .to = MajulaToGutterFront, .type = CT(Conn_Catring));
	warp_node_count++;

	connection_init(MajulaToRotundaLockstoneFront, 7);
	connection_add(MajulaToRotundaLockstoneFront, .to = Tutorial1EntryFront);
	connection_add(MajulaToRotundaLockstoneFront, .to = Tutorial2EntryFront);
	connection_add(MajulaToRotundaLockstoneFront, .to = Tutorial3EntryFront, .type = CT(Conn_Tutorial3Branch));
	connection_add(MajulaToRotundaLockstoneFront, .to = MajulaToGraveOfSaintsFront, .type = CT(Conn_Catring));
	connection_add(MajulaToRotundaLockstoneFront, .to = MajulaToForestOfFallenGiantsFront);
	connection_add(MajulaToRotundaLockstoneFront, .to = MajulaToGutterFront, .type = CT(Conn_Catring));
	connection_add(MajulaToRotundaLockstoneFront, .to = MajulaToShadedWoodsFront);
	warp_node_count++;

	connection_init(MajulaToGutterFront, 0);
	warp_node_count++;

	connection_init(MajulaToShadedWoodsFront, 7);
	connection_add(MajulaToShadedWoodsFront, .to = Tutorial1EntryFront);
	connection_add(MajulaToShadedWoodsFront, .to = Tutorial2EntryFront);
	connection_add(MajulaToShadedWoodsFront, .to = Tutorial3EntryFront, .type = CT(Conn_Tutorial3Branch));
	connection_add(MajulaToShadedWoodsFront, .to = MajulaToGraveOfSaintsFront, .type = CT(Conn_Catring));
	connection_add(MajulaToShadedWoodsFront, .to = MajulaToForestOfFallenGiantsFront);
	connection_add(MajulaToShadedWoodsFront, .to = MajulaToGutterFront, .type = CT(Conn_Catring));
	connection_add(MajulaToShadedWoodsFront, .to = MajulaToRotundaLockstoneFront);
	warp_node_count++;

	connection_init(Tutorial1EntryBack, 1);
	connection_add(Tutorial1EntryBack, .to = Tutorial1ExitFront);
	warp_node_count++;

	connection_init(Tutorial1ExitFront, 1);
	connection_add(Tutorial1ExitFront, .to = Tutorial1EntryBack);
	warp_node_count++;

	connection_init(Tutorial2EntryBack, 1);
	connection_add(Tutorial2EntryBack, .to = Tutorial2ExitFront);
	warp_node_count++;

	connection_init(Tutorial2ExitFront, 0);
	warp_node_count++;

	connection_init(Tutorial3EntryBack, 1);
	connection_add(Tutorial3EntryBack, .to = Tutorial3ExitFront);
	warp_node_count++;

	connection_init(Tutorial3ExitFront, 0);
	warp_node_count++;

	connection_init(Tutorial1ExitBack, 8);
	connection_add(Tutorial1ExitBack, .to = Tutorial1EntryFront);
	connection_add(Tutorial1ExitBack, .to = Tutorial2EntryFront);
	connection_add(Tutorial1ExitBack, .to = Tutorial3EntryFront, .type = CT(Conn_Tutorial3Branch));
	connection_add(Tutorial1ExitBack, .to = MajulaToForestOfFallenGiantsFront);
	connection_add(Tutorial1ExitBack, .to = MajulaToGraveOfSaintsFront, .type = CT(Conn_Catring));
	connection_add(Tutorial1ExitBack, .to = MajulaToRotundaLockstoneFront);
	connection_add(Tutorial1ExitBack, .to = MajulaToGutterFront, .type = CT(Conn_Catring));
	connection_add(Tutorial1ExitBack, .to = MajulaToShadedWoodsFront);
	warp_node_count++;

	connection_init(Tutorial2ExitBack, 8);
	connection_add(Tutorial2ExitBack, .to = Tutorial1EntryFront);
	connection_add(Tutorial2ExitBack, .to = Tutorial2EntryFront);
	connection_add(Tutorial2ExitBack, .to = Tutorial3EntryFront, .type = CT(Conn_Tutorial3Branch));
	connection_add(Tutorial2ExitBack, .to = MajulaToForestOfFallenGiantsFront);
	connection_add(Tutorial2ExitBack, .to = MajulaToGraveOfSaintsFront, .type = CT(Conn_Catring));
	connection_add(Tutorial2ExitBack, .to = MajulaToRotundaLockstoneFront);
	connection_add(Tutorial2ExitBack, .to = MajulaToGutterFront, .type = CT(Conn_Catring));
	connection_add(Tutorial2ExitBack, .to = MajulaToShadedWoodsFront);
	warp_node_count++;

	connection_init(Tutorial3ExitBack, 8);
	connection_add(Tutorial3ExitBack, .to = Tutorial1EntryFront);
	connection_add(Tutorial3ExitBack, .to = Tutorial2EntryFront);
	connection_add(Tutorial3ExitBack, .to = Tutorial3EntryFront, .type = CT(Conn_Tutorial3Branch));
	connection_add(Tutorial3ExitBack, .to = MajulaToForestOfFallenGiantsFront);
	connection_add(Tutorial3ExitBack, .to = MajulaToGraveOfSaintsFront, .type = CT(Conn_Catring));
	connection_add(Tutorial3ExitBack, .to = MajulaToRotundaLockstoneFront);
	connection_add(Tutorial3ExitBack, .to = MajulaToGutterFront, .type = CT(Conn_Catring));
	connection_add(Tutorial3ExitBack, .to = MajulaToShadedWoodsFront);
	warp_node_count++;

#define connection_two_way(src, dst, ...) do {\
	connection_init((src), 1);\
	connection_add((src), .to = (dst), __VA_ARGS__);\
	connection_init((dst), 1);\
	connection_add((dst), .to = (src), __VA_ARGS__);\
} while (0)

	// forest of fallen giants

	connection_two_way(ForestOfFallenGiantsFromMajulaFront, MajulaToForestOfFallenGiantsBack);
	warp_node_count++;
	warp_node_count++;

	connection_two_way(ForestOfFallenGiantsFromMajulaBack, ForestOfFallenGiantsAfterFirstBonfireFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(ForestOfFallenGiantsAfterFirstBonfireBack, 4);
	connection_add(ForestOfFallenGiantsAfterFirstBonfireBack, .to = ForestOfFallenGiantsToCaleFront);
	connection_add(ForestOfFallenGiantsAfterFirstBonfireBack, .to = GiantLordMemoryEntrySrc, .type = CT(Conn_KingsRing) | CT(Conn_AshenMistHeart) | CT(Conn_SoldiersKey));
	connection_add(ForestOfFallenGiantsAfterFirstBonfireBack, .to = LastGiantEntryFront, .type = CT(Conn_Catring) | CT(Conn_IronKeyToFireLizards));
	connection_add(ForestOfFallenGiantsAfterFirstBonfireBack, .to = ForestOfFallenGiantsBalconyBack, .type = CT(Conn_Catring) | CT(Conn_IronKeyToFireLizards));
	warp_node_count++;

	connection_init(GiantLordMemoryEntrySrc, 1);
	connection_add(GiantLordMemoryEntrySrc, .to = GiantLordMemoryEntryDst, .is_warp = true);
	warp_node_count++;

	connection_init(GiantLordMemoryEntryDst, 1);
	connection_add(GiantLordMemoryEntryDst, .to = GiantLordEntryFront);
	warp_node_count++;

	connection_init(GiantLordEntryFront, 0);
	warp_node_count++;

	connection_two_way(GiantLordEntryBack, GiantLordExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(GiantLordExitBack, 1);
	connection_add(GiantLordExitBack, .to = GiantLordMemoryExitSrc);
	warp_node_count++;

	connection_init(GiantLordMemoryExitSrc, 1);
	connection_add(GiantLordMemoryExitSrc, .to = GiantLordMemoryExitDst, .is_warp = true);
	warp_node_count++;

	connection_init(GiantLordMemoryExitDst, 5);
	connection_add(GiantLordMemoryExitDst, .to = ForestOfFallenGiantsToCaleFront);
	connection_add(GiantLordMemoryExitDst, .to = GiantLordMemoryEntrySrc, .type = CT(Conn_AshenMistHeart));
	connection_add(GiantLordMemoryExitDst, .to = LastGiantEntryFront, .type = CT(Conn_Catring) | CT(Conn_IronKeyToFireLizards));
	connection_add(GiantLordMemoryExitDst, .to = ForestOfFallenGiantsBalconyBack, .type = CT(Conn_Catring) | CT(Conn_IronKeyToFireLizards));
	connection_add(GiantLordMemoryExitDst, .to = ForestOfFallenGiantsAfterFirstBonfireBack);
	warp_node_count++;

	connection_init(ForestOfFallenGiantsToCaleFront, 4);
	connection_add(ForestOfFallenGiantsToCaleFront, .to = GiantLordMemoryEntrySrc, .type = CT(Conn_SoldiersKey) | CT(Conn_KingsRing) | CT(Conn_AshenMistHeart));
	connection_add(ForestOfFallenGiantsToCaleFront, .to = LastGiantEntryFront, .type = CT(Conn_Catring) | CT(Conn_IronKeyToFireLizards));
	connection_add(ForestOfFallenGiantsToCaleFront, .to = ForestOfFallenGiantsBalconyBack, .type = CT(Conn_Catring) | CT(Conn_IronKeyToFireLizards));
	connection_add(ForestOfFallenGiantsToCaleFront, .to = ForestOfFallenGiantsAfterFirstBonfireBack, .type = CT(Conn_SoldiersKey) | CT(Conn_KingsRing));
	warp_node_count++;

	connection_init(ForestOfFallenGiantsBalconyBack, 4);
	connection_add(ForestOfFallenGiantsBalconyBack, .to = LastGiantEntryFront);
	connection_add(ForestOfFallenGiantsBalconyBack, .to = ForestOfFallenGiantsToCaleFront);
	connection_add(ForestOfFallenGiantsBalconyBack, .to = GiantLordMemoryEntrySrc, .type = CT(Conn_KingsRing) | CT(Conn_AshenMistHeart) | CT(Conn_SoldiersKey) | CT(Conn_Firebomb));
	connection_add(ForestOfFallenGiantsBalconyBack, .to = ForestOfFallenGiantsAfterFirstBonfireBack, .type = CT(Conn_SoldiersKey) | CT(Conn_KingsRing));
	warp_node_count++;

	connection_init(LastGiantEntryFront, 4);
	connection_add(LastGiantEntryFront, .to = ForestOfFallenGiantsBalconyBack);
	connection_add(LastGiantEntryFront, .to = ForestOfFallenGiantsToCaleFront);
	connection_add(LastGiantEntryFront, .to = GiantLordMemoryEntrySrc, .type = CT(Conn_KingsRing) | CT(Conn_AshenMistHeart) | CT(Conn_SoldiersKey));
	connection_add(LastGiantEntryFront, .to = ForestOfFallenGiantsAfterFirstBonfireBack, .type = CT(Conn_SoldiersKey) | CT(Conn_KingsRing));
	warp_node_count++;

	connection_init(LastGiantEntryBack, 0);
	warp_node_count++;

	connection_init(ForestOfFallenGiantsToCaleBack, 8);
	connection_add(ForestOfFallenGiantsToCaleBack, .to = ForestOfFallenGiantsBalconyFront);
	connection_add(ForestOfFallenGiantsToCaleBack, .to = ForestOfFallenGiantsToPursuerArenaFront, .type = CT(Conn_SoldiersKey));
	connection_add(ForestOfFallenGiantsToCaleBack, .to = NearPateGiantMemoryEntrySrc, .type = CT(Conn_AshenMistHeart));
	connection_add(ForestOfFallenGiantsToCaleBack, .to = ForestOfFallenGiantsToCaleFront, .type = CT(Conn_Firebomb));
	connection_add(ForestOfFallenGiantsToCaleBack, .to = GiantLordMemoryEntrySrc, .type = CT(Conn_SoldiersKey) | CT(Conn_KingsRing) | CT(Conn_AshenMistHeart) | CT(Conn_Firebomb));
	connection_add(ForestOfFallenGiantsToCaleBack, .to = LastGiantEntryFront, .type = CT(Conn_Catring) | CT(Conn_IronKeyToFireLizards) | CT(Conn_Firebomb));
	connection_add(ForestOfFallenGiantsToCaleBack, .to = ForestOfFallenGiantsBalconyBack, .type = CT(Conn_Catring) | CT(Conn_IronKeyToFireLizards) | CT(Conn_Firebomb));
	connection_add(ForestOfFallenGiantsToCaleBack, .to = ForestOfFallenGiantsAfterFirstBonfireBack, .type = CT(Conn_Firebomb) | CT(Conn_SoldiersKey) | CT(Conn_KingsRing));
	warp_node_count++;

	connection_init(NearPateGiantMemoryEntrySrc, 1);
	connection_add(NearPateGiantMemoryEntrySrc, .to = NearPateGiantMemoryEntryDst, .is_warp = true);
	warp_node_count++;

	connection_init(NearPateGiantMemoryEntryDst, 1);
	connection_add(NearPateGiantMemoryEntryDst, .to = NearPateGiantMemoryExitSrc);
	warp_node_count++;

	connection_init(NearPateGiantMemoryExitSrc, 1);
	connection_add(NearPateGiantMemoryExitSrc, .to = NearPateGiantMemoryExitDst, .is_warp = true);
	warp_node_count++;

	connection_init(NearPateGiantMemoryExitDst, 9);
	connection_add(NearPateGiantMemoryExitDst, .to = ForestOfFallenGiantsToCaleBack);
	connection_add(NearPateGiantMemoryExitDst, .to = ForestOfFallenGiantsBalconyFront);
	connection_add(NearPateGiantMemoryExitDst, .to = ForestOfFallenGiantsToPursuerArenaFront, .type = CT(Conn_SoldiersKey));
	connection_add(NearPateGiantMemoryExitDst, .to = ForestOfFallenGiantsToCaleFront, .type = CT(Conn_Firebomb));
	connection_add(NearPateGiantMemoryExitDst, .to = GiantLordMemoryEntrySrc, .type = CT(Conn_SoldiersKey) | CT(Conn_KingsRing) | CT(Conn_AshenMistHeart) | CT(Conn_Firebomb));
	connection_add(NearPateGiantMemoryExitDst, .to = LastGiantEntryFront, .type = CT(Conn_Catring) | CT(Conn_IronKeyToFireLizards) | CT(Conn_Firebomb));
	connection_add(NearPateGiantMemoryExitDst, .to = ForestOfFallenGiantsBalconyBack, .type = CT(Conn_Catring) | CT(Conn_IronKeyToFireLizards) | CT(Conn_Firebomb));
	connection_add(NearPateGiantMemoryExitDst, .to = ForestOfFallenGiantsAfterFirstBonfireBack, .type = CT(Conn_Firebomb) | CT(Conn_SoldiersKey) | CT(Conn_KingsRing));
	warp_node_count++;

	connection_init(ForestOfFallenGiantsToPursuerArenaFront, 0);
	warp_node_count++;

	connection_init(PursuerExitBack, 11);
	connection_add(PursuerExitBack, .to = ForestOfFallenGiantsToCaleBack);
	connection_add(PursuerExitBack, .to = ForestOfFallenGiantsBalconyFront);
	connection_add(PursuerExitBack, .to = ForestOfFallenGiantsToPursuerArenaFront, .type = CT(Conn_SoldiersKey));
	connection_add(PursuerExitBack, .to = NearPateGiantMemoryEntrySrc, .type = CT(Conn_AshenMistHeart));
	connection_add(PursuerExitBack, .to = ForestOfFallenGiantsToCaleFront, .type = CT(Conn_Firebomb));
	connection_add(PursuerExitBack, .to = GiantLordMemoryEntrySrc, .type = CT(Conn_SoldiersKey) | CT(Conn_KingsRing) | CT(Conn_AshenMistHeart) | CT(Conn_Firebomb));
	connection_add(PursuerExitBack, .to = LastGiantEntryFront, .type = CT(Conn_Catring) | CT(Conn_IronKeyToFireLizards) | CT(Conn_Firebomb));
	connection_add(PursuerExitBack, .to = ForestOfFallenGiantsBalconyBack, .type = CT(Conn_Catring) | CT(Conn_IronKeyToFireLizards) | CT(Conn_Firebomb));
	connection_add(PursuerExitBack, .to = ForestOfFallenGiantsAfterFirstBonfireBack, .type = CT(Conn_Firebomb) | CT(Conn_SoldiersKey) | CT(Conn_KingsRing));
	connection_add(PursuerExitBack, .to = NearPursuerBirdEntry);
	connection_add(PursuerExitBack, .to = NearPursuerGiantMemoryEntrySrc, .type = CT(Conn_AshenMistHeart));
	warp_node_count++;

	connection_init(NearPursuerBirdEntry, 1);
	connection_add(NearPursuerBirdEntry, .to = NearPursuerBirdExit, .is_warp = true);
	warp_node_count++;

	connection_init(NearPursuerGiantMemoryEntrySrc, 1);
	connection_add(NearPursuerGiantMemoryEntrySrc, .to = NearPursuerGiantMemoryEntryDst, .is_warp = true);
	warp_node_count++;

	connection_init(NearPursuerGiantMemoryEntryDst, 1);
	connection_add(NearPursuerGiantMemoryEntryDst, .to = NearPursuerGiantMemoryExitSrc);
	warp_node_count++;

	connection_init(NearPursuerGiantMemoryExitSrc, 1);
	connection_add(NearPursuerGiantMemoryExitSrc, .to = NearPursuerGiantMemoryExitDst, .is_warp = true);
	warp_node_count++;

	connection_init(NearPursuerGiantMemoryExitDst, 11);
	connection_add(NearPursuerGiantMemoryExitDst, .to = PursuerExitBack);
	connection_add(NearPursuerGiantMemoryExitDst, .to = ForestOfFallenGiantsToCaleBack);
	connection_add(NearPursuerGiantMemoryExitDst, .to = ForestOfFallenGiantsBalconyFront);
	connection_add(NearPursuerGiantMemoryExitDst, .to = ForestOfFallenGiantsToPursuerArenaFront, .type = CT(Conn_SoldiersKey));
	connection_add(NearPursuerGiantMemoryExitDst, .to = NearPateGiantMemoryEntrySrc, .type = CT(Conn_AshenMistHeart));
	connection_add(NearPursuerGiantMemoryExitDst, .to = ForestOfFallenGiantsToCaleFront, .type = CT(Conn_Firebomb));
	connection_add(NearPursuerGiantMemoryExitDst, .to = GiantLordMemoryEntrySrc, .type = CT(Conn_SoldiersKey) | CT(Conn_KingsRing) | CT(Conn_AshenMistHeart) | CT(Conn_Firebomb));
	connection_add(NearPursuerGiantMemoryExitDst, .to = LastGiantEntryFront, .type = CT(Conn_Catring) | CT(Conn_IronKeyToFireLizards) | CT(Conn_Firebomb));
	connection_add(NearPursuerGiantMemoryExitDst, .to = ForestOfFallenGiantsBalconyBack, .type = CT(Conn_Catring) | CT(Conn_IronKeyToFireLizards) | CT(Conn_Firebomb));
	connection_add(NearPursuerGiantMemoryExitDst, .to = ForestOfFallenGiantsAfterFirstBonfireBack, .type = CT(Conn_Firebomb) | CT(Conn_SoldiersKey) | CT(Conn_KingsRing));
	connection_add(NearPursuerGiantMemoryExitDst, .to = NearPursuerBirdEntry);
	warp_node_count++;

	connection_init(ForestOfFallenGiantsBalconyFront, 8);
	connection_add(ForestOfFallenGiantsBalconyFront, .to = ForestOfFallenGiantsToCaleBack);
	connection_add(ForestOfFallenGiantsBalconyFront, .to = ForestOfFallenGiantsToPursuerArenaFront, .type = CT(Conn_SoldiersKey));
	connection_add(ForestOfFallenGiantsBalconyFront, .to = NearPateGiantMemoryEntrySrc, .type = CT(Conn_AshenMistHeart));
	connection_add(ForestOfFallenGiantsBalconyFront, .to = ForestOfFallenGiantsToCaleFront, .type = CT(Conn_Firebomb));
	connection_add(ForestOfFallenGiantsBalconyFront, .to = GiantLordMemoryEntrySrc, .type = CT(Conn_SoldiersKey) | CT(Conn_KingsRing) | CT(Conn_AshenMistHeart) | CT(Conn_Firebomb));
	connection_add(ForestOfFallenGiantsBalconyFront, .to = LastGiantEntryFront, .type = CT(Conn_Catring) | CT(Conn_IronKeyToFireLizards) | CT(Conn_Firebomb));
	connection_add(ForestOfFallenGiantsBalconyFront, .to = ForestOfFallenGiantsBalconyBack, .type = CT(Conn_Catring) | CT(Conn_IronKeyToFireLizards) | CT(Conn_Firebomb));
	connection_add(ForestOfFallenGiantsBalconyFront, .to = ForestOfFallenGiantsAfterFirstBonfireBack, .type = CT(Conn_Firebomb) | CT(Conn_SoldiersKey) | CT(Conn_KingsRing));
	warp_node_count++;

	connection_two_way(ForestOfFallenGiantsToPursuerArenaBack, PursuerEntryFront);
	warp_node_count++;
	warp_node_count++;

	connection_two_way(PursuerEntryBack, PursuerExitFront);
	warp_node_count++;
	warp_node_count++;


	// lost bastille
	connection_init(NearPursuerBirdExit, 1);
	connection_add(NearPursuerBirdExit, .to = RuinSentinelsEntryFront, .type = CT(Conn_BastilleBranch));
	warp_node_count++;

	connection_init(RuinSentinelsEntryFront, 0); // TODO: confirm this
	warp_node_count++;

	connection_init(RuinSentinelsEntryBack, 5);
	connection_add(RuinSentinelsEntryBack, .to = RuinSentinelsExitFront);
	connection_add(RuinSentinelsEntryBack, .to = RuinSentinelsHiddenDoor1ExitFront);
	connection_add(RuinSentinelsEntryBack, .to = RuinSentinelsHiddenDoor2ExitFront);
	connection_add(RuinSentinelsEntryBack, .to = RuinSentinelsHiddenDoor3ExitFront);
	connection_add(RuinSentinelsEntryBack, .to = RuinSentinelsHiddenDoor4ExitFront);
	warp_node_count++;

	connection_init(RuinSentinelsHiddenDoor1ExitFront, 4);
	connection_add(RuinSentinelsHiddenDoor1ExitFront, .to = RuinSentinelsExitFront);
	connection_add(RuinSentinelsHiddenDoor1ExitFront, .to = RuinSentinelsHiddenDoor2ExitFront);
	connection_add(RuinSentinelsHiddenDoor1ExitFront, .to = RuinSentinelsHiddenDoor3ExitFront);
	connection_add(RuinSentinelsHiddenDoor1ExitFront, .to = RuinSentinelsHiddenDoor4ExitFront);
	warp_node_count++;

	connection_init(RuinSentinelsHiddenDoor1ExitBack, 0);
	warp_node_count++;

	connection_init(RuinSentinelsHiddenDoor2ExitFront, 4);
	connection_add(RuinSentinelsHiddenDoor2ExitFront, .to = RuinSentinelsExitFront);
	connection_add(RuinSentinelsHiddenDoor2ExitFront, .to = RuinSentinelsHiddenDoor1ExitFront);
	connection_add(RuinSentinelsHiddenDoor2ExitFront, .to = RuinSentinelsHiddenDoor3ExitFront);
	connection_add(RuinSentinelsHiddenDoor2ExitFront, .to = RuinSentinelsHiddenDoor4ExitFront);
	warp_node_count++;

	connection_init(RuinSentinelsHiddenDoor2ExitBack, 0);
	warp_node_count++;

	connection_init(RuinSentinelsHiddenDoor3ExitFront, 4);
	connection_add(RuinSentinelsHiddenDoor3ExitFront, .to = RuinSentinelsExitFront);
	connection_add(RuinSentinelsHiddenDoor3ExitFront, .to = RuinSentinelsHiddenDoor1ExitFront);
	connection_add(RuinSentinelsHiddenDoor3ExitFront, .to = RuinSentinelsHiddenDoor2ExitFront);
	connection_add(RuinSentinelsHiddenDoor3ExitFront, .to = RuinSentinelsHiddenDoor4ExitFront);
	warp_node_count++;

	connection_init(RuinSentinelsHiddenDoor3ExitBack, 0);
	warp_node_count++;

	connection_init(RuinSentinelsHiddenDoor4ExitFront, 4);
	connection_add(RuinSentinelsHiddenDoor4ExitFront, .to = RuinSentinelsExitFront);
	connection_add(RuinSentinelsHiddenDoor4ExitFront, .to = RuinSentinelsHiddenDoor1ExitFront);
	connection_add(RuinSentinelsHiddenDoor4ExitFront, .to = RuinSentinelsHiddenDoor2ExitFront);
	connection_add(RuinSentinelsHiddenDoor4ExitFront, .to = RuinSentinelsHiddenDoor3ExitFront);
	warp_node_count++;

	connection_init(RuinSentinelsHiddenDoor4ExitBack, 0);
	warp_node_count++;

	connection_init(RuinSentinelsExitBack, 6);
	connection_add(RuinSentinelsExitBack, .to = RuinSentinelsExitFront);
	connection_add(RuinSentinelsExitBack, .to = RuinSentinelsHiddenDoor1ExitFront);
	connection_add(RuinSentinelsExitBack, .to = RuinSentinelsHiddenDoor2ExitFront);
	connection_add(RuinSentinelsExitBack, .to = RuinSentinelsHiddenDoor3ExitFront);
	connection_add(RuinSentinelsExitBack, .to = RuinSentinelsHiddenDoor4ExitFront);
	connection_add(RuinSentinelsExitBack, .to = RuinSentinesUpperExitFront);
	warp_node_count++;

	connection_init(RuinSentinesUpperExitFront, 6);
	connection_add(RuinSentinesUpperExitFront, .to = RuinSentinelsExitFront);
	connection_add(RuinSentinesUpperExitFront, .to = RuinSentinelsHiddenDoor1ExitFront);
	connection_add(RuinSentinesUpperExitFront, .to = RuinSentinelsHiddenDoor2ExitFront);
	connection_add(RuinSentinesUpperExitFront, .to = RuinSentinelsHiddenDoor3ExitFront);
	connection_add(RuinSentinesUpperExitFront, .to = RuinSentinelsHiddenDoor4ExitFront);
	connection_add(RuinSentinesUpperExitFront, .to = RuinSentinelsExitBack);
	warp_node_count++;

	connection_init(RuinSentinelsExitFront, 6);
	connection_add(RuinSentinelsExitFront, .to = RuinSentinelsHiddenDoor1ExitFront);
	connection_add(RuinSentinelsExitFront, .to = RuinSentinelsHiddenDoor2ExitFront);
	connection_add(RuinSentinelsExitFront, .to = RuinSentinelsHiddenDoor3ExitFront);
	connection_add(RuinSentinelsExitFront, .to = RuinSentinelsHiddenDoor4ExitFront);
	warp_node_count++;

	connection_init(RuinSentinesUpperExitBack, 3);
	connection_add(RuinSentinesUpperExitBack, .to = RuinSentinelsEntryFront, .type = CT(Conn_BastilleBranch));
	connection_add(RuinSentinesUpperExitBack, .to = LostBastilleToSinnersRiseFront);
	connection_add(RuinSentinesUpperExitBack, .to = LostBastilleToShipFromWharfBack, .type = CT(Conn_OldKeyInBastille));
	connection_add(RuinSentinesUpperExitBack, .to = LostBastilleToBelfryLunaFront, .type = CT(Conn_PharrosLockstone));
	warp_node_count++;

	connection_init(LostBastilleToSinnersRiseFront, 0);
	warp_node_count++;

	connection_init(LostBastilleToSinnersRiseBack, 1);
	connection_add(LostBastilleToSinnersRiseBack, .to = LostSinnerEntryFront);
	warp_node_count++;

	connection_init(LostSinnerEntryFront, 1);
	connection_add(LostSinnerEntryFront, .to = LostBastilleToSinnersRiseBack, .type = CT(Conn_BastilleKey));
	warp_node_count++;

	connection_init(LostSinnerEntryBack, 1);
	connection_add(LostSinnerEntryBack, .to = LostSinnerExitFront);
	warp_node_count++;

	connection_init(LostSinnerExitFront, 0);
	warp_node_count++;

	connection_init(LostSinnerExitBack, 0);
	warp_node_count++;

	connection_init(LostBastilleToShipFromWharfBack, 2);
	connection_add(LostBastilleToShipFromWharfBack, .to = RuinSentinelsEntryFront, .type = CT(Conn_BastilleBranch));
	connection_add(LostBastilleToShipFromWharfBack, .to = LostBastilleToSinnersRiseFront, .type = CT(Conn_OldKeyInBastille));
	warp_node_count++;

	connection_init(LostBastilleToShipFromWharfFront, 0);
	warp_node_count++;

	connection_init(PirateShipBastille, 1);
	connection_add(PirateShipBastille, .to = LostBastilleToShipFromWharfFront);
	warp_node_count++;

	connection_init(LostBastilleToBelfryLunaFront, 4);
	connection_add(LostBastilleToBelfryLunaFront, .to = RuinSentinesUpperExitBack);
	connection_add(LostBastilleToBelfryLunaFront, .to = RuinSentinelsEntryFront, .type = CT(Conn_BastilleBranch));
	connection_add(LostBastilleToBelfryLunaFront, .to = LostBastilleToSinnersRiseFront);
	connection_add(LostBastilleToBelfryLunaFront, .to = LostBastilleToShipFromWharfBack, .type = CT(Conn_OldKeyInBastille));
	warp_node_count++;

	connection_two_way(LostBastilleToBelfryLunaBack, GargoylesEntryFront);
	warp_node_count++;
	warp_node_count++;

	connection_two_way(GargoylesExitFront, GargoylesEntryBack);
	warp_node_count++;
	warp_node_count++;

	connection_init(GargoylesExitBack, 0);
	warp_node_count++;

	// heides tower of flame
	connection_init(MajulaToRotundaLockstoneBack, 2);
	connection_add(MajulaToRotundaLockstoneBack, .to = MajulaToHuntsmanCopseFront, .type = CT(Conn_RotundaLockstone));
	connection_add(MajulaToRotundaLockstoneBack, .to = HeidesToMajulaFront);
	warp_node_count++;

	connection_init(MajulaToHuntsmanCopseFront, 0);
	warp_node_count++;

	connection_init(HeidesToMajulaFront, 0);
	warp_node_count++;

	connection_init(HeidesToMajulaBack, 2);
	connection_add(HeidesToMajulaBack, .to = OldDragonslayerEntryFront);
	connection_add(HeidesToMajulaBack, .to = DragonriderEntryFront);
	warp_node_count++;

	connection_init(DragonriderEntryFront, 2);
	connection_add(DragonriderEntryFront, .to = OldDragonslayerEntryFront);
	connection_add(DragonriderEntryFront, .to = HeidesToMajulaBack);
	warp_node_count++;

	connection_init(OldDragonslayerEntryFront, 0);
	warp_node_count++;

	connection_two_way(OldDragonslayerEntryBack, OldDragonslayerExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(OldDragonslayerExitBack, 0);
	warp_node_count++;

	connection_two_way(DragonriderEntryBack, DragonriderExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_two_way(DragonriderExitBack, HeidesToWharfFront);
	warp_node_count++;
	warp_node_count++;

	connection_two_way(HeidesToWharfBack, NoMansWharfToHeidesFront);
	warp_node_count++;
	warp_node_count++;

	// no man's wharf
	connection_two_way(NoMansWharfToHeidesBack, FlexileSentryEntryFront);
	warp_node_count++;
	warp_node_count++;

	connection_two_way(FlexileSentryEntryBack, FlexileSentryExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(FlexileSentryExitBack, 1);
	connection_add(FlexileSentryExitBack, .to = PirateShipWharf);
	warp_node_count++;

	connection_init(PirateShipWharf, 1);
	connection_add(PirateShipWharf, .to = PirateShipBastille, .is_warp = true);
	warp_node_count++;

	// huntsman copse
	connection_two_way(MajulaToHuntsmanCopseBack, HuntsmanCopseToMajulaFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(HuntsmanCopseToMajulaBack, 2);
	connection_add(HuntsmanCopseToMajulaBack, .to = ChariotEntryFront);
	connection_add(HuntsmanCopseToMajulaBack, .to = SkeletonLordsEntryFront);
	warp_node_count++;

	connection_init(ChariotEntryFront, 2);
	connection_add(ChariotEntryFront, .to = HuntsmanCopseToMajulaBack);
	connection_add(ChariotEntryFront, .to = SkeletonLordsEntryFront);
	warp_node_count++;

	connection_init(SkeletonLordsEntryFront, 2);
	connection_add(SkeletonLordsEntryFront, .to = HuntsmanCopseToMajulaBack);
	connection_add(SkeletonLordsEntryFront, .to = ChariotEntryFront);
	warp_node_count++;

	connection_two_way(ChariotEntryBack, ChariotExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(ChariotExitBack, 0);
	warp_node_count++;

	connection_two_way(SkeletonLordsEntryBack, SkeletonLordsExitFront);
	warp_node_count++;
	warp_node_count++;

	// harvest valley
	connection_init(SkeletonLordsExitBack, 1);
	connection_add(SkeletonLordsExitBack, .to = HarvestValleyToSkeletonLordsFront);
	warp_node_count++;

	connection_init(HarvestValleyToSkeletonLordsFront, 0);
	warp_node_count++;

	connection_init(HarvestValleyToSkeletonLordsBack, 1);
	connection_add(HarvestValleyToSkeletonLordsBack, .to = CovetousDemonEntryFront);
	warp_node_count++;

	connection_init(CovetousDemonEntryFront, 0);
	warp_node_count++;

	connection_two_way(CovetousDemonEntryBack, CovetousDemonExitFront);
	warp_node_count++;
	warp_node_count++;

	// earthen peak
	connection_init(CovetousDemonExitBack, 3);
	connection_add(CovetousDemonExitBack, .to = EarthenPeakNearWindmillBurnLocationFront);
	connection_add(CovetousDemonExitBack, .to = EarthenPeakNearWindmillBurnLocationBack);
	connection_add(CovetousDemonExitBack, .to = MythaEntryFront);
	warp_node_count++;

	connection_init(EarthenPeakNearWindmillBurnLocationFront, 3);
	connection_add(EarthenPeakNearWindmillBurnLocationFront, .to = CovetousDemonExitBack);
	connection_add(EarthenPeakNearWindmillBurnLocationFront, .to = EarthenPeakNearWindmillBurnLocationBack);
	connection_add(EarthenPeakNearWindmillBurnLocationFront, .to = MythaEntryFront);
	warp_node_count++;

	connection_init(EarthenPeakNearWindmillBurnLocationBack, 3);
	connection_add(EarthenPeakNearWindmillBurnLocationBack, .to = CovetousDemonExitBack);
	connection_add(EarthenPeakNearWindmillBurnLocationBack, .to = EarthenPeakNearWindmillBurnLocationFront);
	connection_add(EarthenPeakNearWindmillBurnLocationBack, .to = MythaEntryFront);
	warp_node_count++;

	connection_init(MythaEntryFront, 3);
	connection_add(MythaEntryFront, .to = CovetousDemonExitBack);
	connection_add(MythaEntryFront, .to = EarthenPeakNearWindmillBurnLocationFront);
	connection_add(MythaEntryFront, .to = EarthenPeakNearWindmillBurnLocationBack);
	warp_node_count++;

	connection_two_way(MythaEntryBack, MythaExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_two_way(MythaExitBack, IronKeepToEarthenPeakFront);
	warp_node_count++;
	warp_node_count++;

	// iron keep

	connection_init(IronKeepToEarthenPeakBack, 5);
	connection_add(IronKeepToEarthenPeakBack, .to = SmeltorDemonEntryFront);
	connection_add(IronKeepToEarthenPeakBack, .to = IronKeepToBelfryAtPharrosLockstoneFront, .type = CT(Conn_PharrosLockstone));
	connection_add(IronKeepToEarthenPeakBack, .to = IronKeepNearFlameThrowerAndTurtleFront);
	connection_add(IronKeepToEarthenPeakBack, .to = IronKeepNearFlameThrowerAndTurtleBack);
	connection_add(IronKeepToEarthenPeakBack, .to = OldIronKingEntryFront);
	warp_node_count++;

	connection_init(SmeltorDemonEntryFront, 4);
	connection_add(SmeltorDemonEntryFront, .to = IronKeepToBelfryAtPharrosLockstoneFront, .type = CT(Conn_PharrosLockstone));
	connection_add(SmeltorDemonEntryFront, .to = IronKeepNearFlameThrowerAndTurtleFront);
	connection_add(SmeltorDemonEntryFront, .to = IronKeepNearFlameThrowerAndTurtleBack);
	connection_add(SmeltorDemonEntryFront, .to = OldIronKingEntryFront);
	warp_node_count++;

	connection_init(IronKeepToBelfryAtPharrosLockstoneFront, 4);
	connection_add(IronKeepToBelfryAtPharrosLockstoneFront, .to = SmeltorDemonEntryFront);
	connection_add(IronKeepToBelfryAtPharrosLockstoneFront, .to = IronKeepNearFlameThrowerAndTurtleFront);
	connection_add(IronKeepToBelfryAtPharrosLockstoneFront, .to = IronKeepNearFlameThrowerAndTurtleBack);
	connection_add(IronKeepToBelfryAtPharrosLockstoneFront, .to = OldIronKingEntryFront);
	warp_node_count++;

	connection_init(IronKeepNearFlameThrowerAndTurtleFront, 4);
	connection_add(IronKeepNearFlameThrowerAndTurtleFront, .to = IronKeepToBelfryAtPharrosLockstoneFront, .type = CT(Conn_PharrosLockstone));
	connection_add(IronKeepNearFlameThrowerAndTurtleFront, .to = IronKeepNearFlameThrowerAndTurtleBack);
	connection_add(IronKeepNearFlameThrowerAndTurtleFront, .to = SmeltorDemonEntryFront);
	connection_add(IronKeepNearFlameThrowerAndTurtleFront, .to = OldIronKingEntryFront);
	warp_node_count++;

	connection_init(IronKeepToBelfryAtPharrosLockstoneBack, 6);
	connection_add(IronKeepToBelfryAtPharrosLockstoneBack, .to = IronKeepToBelfryAtPharrosLockstoneFront, .type = CT(Conn_PharrosLockstone));
	connection_add(IronKeepToBelfryAtPharrosLockstoneBack, .to = IronKeepNearFlameThrowerAndTurtleBack);
	connection_add(IronKeepToBelfryAtPharrosLockstoneBack, .to = IronKeepNearFlameThrowerAndTurtleFront);
	connection_add(IronKeepToBelfryAtPharrosLockstoneBack, .to = SmeltorDemonEntryFront);
	connection_add(IronKeepToBelfryAtPharrosLockstoneBack, .to = OldIronKingEntryFront);
	connection_add(IronKeepToBelfryAtPharrosLockstoneBack, .to = BelfrySolEntryFront);
	warp_node_count++;

	connection_two_way(SmeltorDemonEntryBack, SmeltorDemonExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(SmeltorDemonExitBack, 3);
	connection_add(SmeltorDemonExitBack, .to = SmeltorDemonToBonfireFront);
	connection_add(SmeltorDemonExitBack, .to = SmeltorDemonEntryBack);
	connection_add(SmeltorDemonExitBack, .to = SmeltorDemonExitFront);
	warp_node_count++;

	connection_init(SmeltorDemonToBonfireFront, 3);
	connection_add(SmeltorDemonToBonfireFront, .to = SmeltorDemonExitBack);
	connection_add(SmeltorDemonToBonfireFront, .to = SmeltorDemonEntryBack);
	connection_add(SmeltorDemonToBonfireFront, .to = SmeltorDemonExitFront);
	warp_node_count++;

	connection_init(SmeltorDemonToBonfireBack, 6);
	connection_add(SmeltorDemonToBonfireBack, .to = IronKeepToBelfryAtPharrosLockstoneFront, .type = CT(Conn_PharrosLockstone));
	connection_add(SmeltorDemonToBonfireBack, .to = IronKeepNearFlameThrowerAndTurtleBack);
	connection_add(SmeltorDemonToBonfireBack, .to = IronKeepNearFlameThrowerAndTurtleFront);
	connection_add(SmeltorDemonToBonfireBack, .to = SmeltorDemonEntryFront);
	connection_add(SmeltorDemonToBonfireBack, .to = OldIronKingEntryFront);
	connection_add(SmeltorDemonToBonfireBack, .to = BelfrySolEntryFront);
	warp_node_count++;

	connection_init(BelfrySolEntryFront, 0);
	warp_node_count++;

	connection_init(BelfrySolEntryBack, 1);
	connection_add(BelfrySolEntryBack, .to = BelfrySolExitFront);
	warp_node_count++;

	connection_init(BelfrySolExitFront, 1);
	connection_add(BelfrySolExitFront, .to = BelfrySolEntryBack);
	warp_node_count++;

	connection_init(BelfrySolExitBack, 7);
	connection_add(BelfrySolExitBack, .to = IronKeepToBelfryAtPharrosLockstoneFront, .type = CT(Conn_PharrosLockstone));
	connection_add(BelfrySolExitBack, .to = IronKeepToBelfryAtPharrosLockstoneBack);
	connection_add(BelfrySolExitBack, .to = IronKeepNearFlameThrowerAndTurtleBack);
	connection_add(BelfrySolExitBack, .to = IronKeepNearFlameThrowerAndTurtleFront);
	connection_add(BelfrySolExitBack, .to = SmeltorDemonEntryFront);
	connection_add(BelfrySolExitBack, .to = OldIronKingEntryFront);
	connection_add(BelfrySolExitBack, .to = BelfrySolEntryFront);
	warp_node_count++;

	connection_two_way(IronKeepNearFlameThrowerAndTurtleBack, OldIronKingEntryFront);
	warp_node_count++;
	warp_node_count++;

	connection_two_way(OldIronKingEntryBack, OldIronKingExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(OldIronKingExitBack, 1);
	connection_add(OldIronKingExitBack, .to = DLC2EntranceBaseGame);
	warp_node_count++;

	connection_init(DLC2EntranceBaseGame, 2);
	connection_add(DLC2EntranceBaseGame, .to = DLC2EntranceDLC, .is_warp = true);
	connection_add(DLC2EntranceBaseGame, .to = OldIronKingExitBack);
	warp_node_count++;

	// dlc2
	connection_init(DLC2EntranceDLC, 2);
	connection_add(DLC2EntranceDLC, .to = BrumeTowerEntranceFromLiftFront, .type = CT(Conn_DLC2Key));
	connection_add(DLC2EntranceDLC, .to = DLC2EntranceBaseGame, .is_warp = true);
	warp_node_count++;

	connection_init(BrumeTowerEntranceFromLiftFront, 0);
	warp_node_count++;

	connection_init(BrumeTowerEntranceFromLiftBack, 1);
	connection_add(BrumeTowerEntranceFromLiftBack, .to = BrumeTowerFrom2ndBonfireToCentralRoomFront);
	warp_node_count++;

	connection_init(BrumeTowerFrom2ndBonfireToCentralRoomFront, 0);
	warp_node_count++;

	connection_init(BrumeTowerFrom2ndBonfireToCentralRoomBack, 6);
	connection_add(BrumeTowerFrom2ndBonfireToCentralRoomBack, .to = SirAlonneArmorDLCEntrySrc, .type = CT(Conn_ActivateBrumeTower) | CT(Conn_AshenMistHeart));
	connection_add(BrumeTowerFrom2ndBonfireToCentralRoomBack, .to = BrumeTowerToFoyerBonfireFromOutsideFront);
	connection_add(BrumeTowerFrom2ndBonfireToCentralRoomBack, .to = BrumeTowerToFoyerBonfireFromOutsideBack, .type = CT(Conn_ActivateBrumeTower));
	connection_add(BrumeTowerFrom2ndBonfireToCentralRoomBack, .to = BrumeTowerToBlueSmelterDungeonFront, .type = CT(Conn_ActivateBrumeTower) | CT(Conn_BrumeTowerKey));
	connection_add(BrumeTowerFrom2ndBonfireToCentralRoomBack, .to = FumeKnightEntryFront, .type = CT(Conn_ActivateBrumeTower));
	connection_add(BrumeTowerFrom2ndBonfireToCentralRoomBack, .to = BrumeTowerToScorchingIronSceptorKeyFront, .type = CT(Conn_ActivateBrumeTower));
	warp_node_count++;

	connection_init(BrumeTowerToScorchingIronSceptorKeyBack, 0);
	warp_node_count++;

	connection_init(SirAlonneArmorDLCEntrySrc, 1);
	connection_add(SirAlonneArmorDLCEntrySrc, .to = SirAlonneArmorDLCEntryDst, .is_warp = true);
	warp_node_count++;

	connection_init(SirAlonneArmorDLCEntryDst, 1);
	connection_add(SirAlonneArmorDLCEntryDst, .to = SirAlonneEntryFront);
	warp_node_count++;

	connection_init(SirAlonneEntryFront, 0);
	warp_node_count++;

	connection_two_way(SirAlonneEntryBack, SirAlonneExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(SirAlonneExitBack, 1);
	connection_add(SirAlonneExitBack, .to = SirAlonneMemoryExitSrc);
	warp_node_count++;

	connection_init(SirAlonneMemoryExitSrc, 1);
	connection_add(SirAlonneMemoryExitSrc, .to = SirAlonneMemoryExitDst, .is_warp = true);
	warp_node_count++;

	connection_init(SirAlonneMemoryExitDst, 6);
	connection_add(SirAlonneMemoryExitDst, .to = SirAlonneArmorDLCEntrySrc, .type = CT(Conn_AshenMistHeart));
	connection_add(SirAlonneMemoryExitDst, .to = BrumeTowerToFoyerBonfireFromOutsideFront, .type = CT(Conn_ActivateBrumeTower));
	connection_add(SirAlonneMemoryExitDst, .to = BrumeTowerToFoyerBonfireFromOutsideBack, .type = CT(Conn_ActivateBrumeTower));
	connection_add(SirAlonneMemoryExitDst, .to = BrumeTowerToBlueSmelterDungeonFront, .type = CT(Conn_ActivateBrumeTower) | CT(Conn_BrumeTowerKey));
	connection_add(SirAlonneMemoryExitDst, .to = FumeKnightEntryFront, .type = CT(Conn_ActivateBrumeTower));
	connection_add(SirAlonneMemoryExitDst, .to = BrumeTowerToScorchingIronSceptorKeyFront, .type = CT(Conn_ActivateBrumeTower));
	warp_node_count++;

	connection_init(BrumeTowerToFoyerBonfireFromOutsideFront, 0);
	warp_node_count++;

	connection_init(BrumeTowerToFoyerBonfireFromOutsideBack, 5);
	connection_add(BrumeTowerToFoyerBonfireFromOutsideBack, .to = SirAlonneArmorDLCEntrySrc, .type = CT(Conn_ActivateBrumeTower) | CT(Conn_AshenMistHeart));
	connection_add(BrumeTowerToFoyerBonfireFromOutsideBack, .to = BrumeTowerToFoyerBonfireFromOutsideFront, .type = CT(Conn_ActivateBrumeTower));
	connection_add(BrumeTowerToFoyerBonfireFromOutsideBack, .to = BrumeTowerToBlueSmelterDungeonFront, .type = CT(Conn_ActivateBrumeTower) | CT(Conn_BrumeTowerKey));
	connection_add(BrumeTowerToFoyerBonfireFromOutsideBack, .to = FumeKnightEntryFront, .type = CT(Conn_ActivateBrumeTower));
	connection_add(BrumeTowerToFoyerBonfireFromOutsideBack, .to = BrumeTowerToScorchingIronSceptorKeyFront);
	warp_node_count++;

	connection_init(BrumeTowerToBlueSmelterDungeonFront, 6);
	connection_add(BrumeTowerToBlueSmelterDungeonFront, .to = SirAlonneArmorDLCEntrySrc, .type = CT(Conn_ActivateBrumeTower) | CT(Conn_BrumeTowerKey) | CT(Conn_AshenMistHeart));
	connection_add(BrumeTowerToBlueSmelterDungeonFront, .to = BrumeTowerToFoyerBonfireFromOutsideFront, .type = CT(Conn_ActivateBrumeTower) | CT(Conn_BrumeTowerKey));
	connection_add(BrumeTowerToBlueSmelterDungeonFront, .to = BrumeTowerToFoyerBonfireFromOutsideBack, .type = CT(Conn_ActivateBrumeTower) | CT(Conn_BrumeTowerKey));
	connection_add(BrumeTowerToBlueSmelterDungeonFront, .to = BrumeTowerFrom2ndBonfireToCentralRoomBack, .type = CT(Conn_ActivateBrumeTower) | CT(Conn_BrumeTowerKey));
	connection_add(BrumeTowerToBlueSmelterDungeonFront, .to = FumeKnightEntryFront, .type = CT(Conn_ActivateBrumeTower) | CT(Conn_BrumeTowerKey));
	connection_add(BrumeTowerToBlueSmelterDungeonFront, .to = BrumeTowerToScorchingIronSceptorKeyFront, .type = CT(Conn_ActivateBrumeTower) | CT(Conn_BrumeTowerKey));
	warp_node_count++;

	connection_init(FumeKnightEntryFront, 0);
	warp_node_count++;

	connection_two_way(FumeKnightEntryBack, FumeKnightExitFront);
	warp_node_count++;
	warp_node_count++;
	
	connection_init(FumeKnightExitBack, 0);
	warp_node_count++;

	connection_init(BrumeTowerToScorchingIronSceptorKeyFront, 6);
	connection_add(BrumeTowerToScorchingIronSceptorKeyFront, .to = SirAlonneArmorDLCEntrySrc, .type = CT(Conn_ActivateBrumeTower) | CT(Conn_AshenMistHeart));
	connection_add(BrumeTowerToScorchingIronSceptorKeyFront, .to = BrumeTowerToFoyerBonfireFromOutsideFront, .type = CT(Conn_ActivateBrumeTower));
	connection_add(BrumeTowerToScorchingIronSceptorKeyFront, .to = BrumeTowerToFoyerBonfireFromOutsideBack);
	connection_add(BrumeTowerToScorchingIronSceptorKeyFront, .to = BrumeTowerFrom2ndBonfireToCentralRoomBack, .type = CT(Conn_ActivateBrumeTower));
	connection_add(BrumeTowerToScorchingIronSceptorKeyFront, .to = FumeKnightEntryFront, .type = CT(Conn_ActivateBrumeTower));
	connection_add(BrumeTowerToScorchingIronSceptorKeyFront, .to = BrumeTowerToBlueSmelterDungeonFront, .type = CT(Conn_ActivateBrumeTower) | CT(Conn_BrumeTowerKey));
	warp_node_count++;

	connection_init(BrumeTowerToBlueSmelterDungeonBack, 1);
	connection_add(BrumeTowerToBlueSmelterDungeonBack, .to = BlueSmeltorDemonEntryFront);
	warp_node_count++;

	connection_init(BlueSmeltorDemonEntryFront, 0);
	warp_node_count++;

	connection_two_way(BlueSmeltorDemonEntryBack, BlueSmeltorDemonExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(BlueSmeltorDemonExitBack, 7);
	connection_add(BlueSmeltorDemonExitBack, .to = BrumeTowerToBlueSmelterDungeonFront);
	connection_add(BlueSmeltorDemonExitBack, .to = SirAlonneArmorDLCEntrySrc, .type = CT(Conn_ActivateBrumeTower) | CT(Conn_BrumeTowerKey) | CT(Conn_AshenMistHeart));
	connection_add(BlueSmeltorDemonExitBack, .to = BrumeTowerToFoyerBonfireFromOutsideFront, .type = CT(Conn_ActivateBrumeTower) | CT(Conn_BrumeTowerKey));
	connection_add(BlueSmeltorDemonExitBack, .to = BrumeTowerToFoyerBonfireFromOutsideBack, .type = CT(Conn_ActivateBrumeTower) | CT(Conn_BrumeTowerKey));
	connection_add(BlueSmeltorDemonExitBack, .to = BrumeTowerFrom2ndBonfireToCentralRoomBack, .type = CT(Conn_ActivateBrumeTower) | CT(Conn_BrumeTowerKey));
	connection_add(BlueSmeltorDemonExitBack, .to = FumeKnightEntryFront, .type = CT(Conn_ActivateBrumeTower) | CT(Conn_BrumeTowerKey));
	connection_add(BlueSmeltorDemonExitBack, .to = BrumeTowerToScorchingIronSceptorKeyFront, .type = CT(Conn_ActivateBrumeTower) | CT(Conn_BrumeTowerKey));
	warp_node_count++;

	// grave of saints
	connection_two_way(MajulaToGraveOfSaintsBack, GraveOfSaintsFromPitFront);
	warp_node_count++;
	warp_node_count++;

	connection_two_way(GraveOfSaintsFromPitBack, GraveOfSaintsNearUpperBonfireFront);
	warp_node_count++;
	warp_node_count++;

	connection_two_way(GraveOfSaintsNearUpperBonfireBack, RoyalRatVanguardEntryFront);
	warp_node_count++;
	warp_node_count++;

	connection_two_way(RoyalRatVanguardEntryBack, RoyalRatAuthorityExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(RoyalRatVanguardExitBack, 2);
	connection_add(RoyalRatVanguardExitBack, .to = MajulaToGutterBack);
	connection_add(RoyalRatVanguardExitBack, .to = TheGutterFromGraveOfSaintsFront);
	warp_node_count++;

	connection_init(MajulaToGutterBack, 1);
	connection_add(MajulaToGutterBack, .to = TheGutterFromGraveOfSaintsFront);
	warp_node_count++;

	connection_init(TheGutterFromGraveOfSaintsFront, 0);
	warp_node_count++;

	// gutter
	connection_init(TheGutterFromGraveOfSaintsBack, 2);
	connection_add(TheGutterFromGraveOfSaintsBack, .to = GutterNearAntQueenFront);
	connection_add(TheGutterFromGraveOfSaintsBack, .to = GutterNearAntQueenBack);
	connection_add(TheGutterFromGraveOfSaintsBack, .to = BlackGulchEntranceFront);
	warp_node_count++;

	connection_init(GutterNearAntQueenFront, 0);
	warp_node_count++;

	connection_init(GutterNearAntQueenBack, 1);
	connection_add(GutterNearAntQueenBack, .to = BlackGulchEntranceFront);
	warp_node_count++;

	connection_init(BlackGulchEntranceFront, 0);
	warp_node_count++;

	// black gulch
	connection_init(BlackGulchEntranceBack, 2);
	connection_add(BlackGulchEntranceBack, .to = TheRottenEntryFront);
	connection_add(BlackGulchEntranceBack, .to = ChasmPortalFromBlackGulchSrc, .type = CT(Conn_DarkCovenentJoined));
	warp_node_count++;

	connection_init(TheRottenEntryFront, 2);
	connection_add(TheRottenEntryFront, .to = BlackGulchEntranceBack);
	connection_add(TheRottenEntryFront, .to = ChasmPortalFromBlackGulchSrc, .type = CT(Conn_DarkCovenentJoined));
	warp_node_count++;

	connection_init(ChasmPortalFromBlackGulchSrc, 1);
	connection_add(ChasmPortalFromBlackGulchSrc, .to = ChasmPortalFromBlackGulchDst, .is_warp = true);
	warp_node_count++;

	connection_init(ChasmPortalFromBlackGulchDst, 1);
	connection_add(ChasmPortalFromBlackGulchDst, .to = DarkChasmFromBlackGulchExitFront);
	warp_node_count++;

	connection_init(DarkChasmFromBlackGulchExitFront, 0);
	warp_node_count++;

	connection_init(DarkChasmFromBlackGulchExitBack, 1);
	connection_add(DarkChasmFromBlackGulchExitBack, .to = ChasmGulchExitWarpSrc);
	warp_node_count++;

	connection_init(ChasmGulchExitWarpSrc, 1);
	connection_add(ChasmGulchExitWarpSrc, .to = ChasmGulchExitWarpDst, .is_warp = true);
	warp_node_count++;

	connection_init(ChasmGulchExitWarpDst, 1);
	connection_add(ChasmGulchExitWarpDst, .to = ChasmPortalFromBlackGulchSrc, .type = CT(Conn_DarkCovenentJoined));
	warp_node_count++;

	connection_two_way(TheRottenEntryBack, TheRottenExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(TheRottenExitBack, 1);
	connection_add(TheRottenExitBack, .to = DLC1EntranceBaseGame);
	warp_node_count++;

	connection_init(DLC1EntranceBaseGame, 2);
	connection_add(DLC1EntranceBaseGame, .to = DLC1EntranceDLC, .is_warp = true);
	connection_add(DLC1EntranceBaseGame, .to = TheRottenExitBack);
	warp_node_count++;

	// dlc1
	connection_init(DLC1EntranceDLC, 2);
	connection_add(DLC1EntranceDLC, .to = DLC1EntranceBaseGame, .is_warp = true);
	connection_add(DLC1EntranceDLC, .to = ShulvaEntranceFront, .type = CT(Conn_DLC1Key));
	warp_node_count++;

	connection_init(ShulvaEntranceFront, 1);
	connection_add(ShulvaEntranceFront, .to = DLC1EntranceDLC, .type = CT(Conn_DLC1Key)); // TODO: verify this
	warp_node_count++;

	connection_init(ShulvaEntranceBack, 2);
	connection_add(ShulvaEntranceBack, .to = ShulvaToGankFight1Front, .type = CT(Conn_ShulvaSanctumKey));
	connection_add(ShulvaEntranceBack, .to = NearJesterThomasFront);
	warp_node_count++;


	connection_init(ShulvaToGankFight1Front, 2);
	connection_add(ShulvaToGankFight1Front, .to = ShulvaEntranceBack);
	connection_add(ShulvaToGankFight1Front, .to = NearJesterThomasFront);
	warp_node_count++;

	connection_init(NearJesterThomasFront, 0);
	warp_node_count++;

	connection_two_way(ShulvaToGankFight1Back, ShulvaToGankFight2Front);
	warp_node_count++;
	warp_node_count++;

	connection_init(ShulvaToGankFight2Back, 1);
	connection_add(ShulvaToGankFight2Back, .to = GankSquadBossEntryFront);
	warp_node_count++;

	connection_init(GankSquadBossEntryFront, 0);
	warp_node_count++;

	connection_two_way(GankSquadBossEntryBack, GankSquadBossExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(GankSquadBossExitBack, 3);
	connection_add(GankSquadBossExitBack, .to = ShulvaToGankFight1Front, .type = CT(Conn_ShulvaSanctumKey));
	connection_add(GankSquadBossExitBack, .to = ShulvaEntranceBack);
	connection_add(GankSquadBossExitBack, .to = NearJesterThomasFront);
	warp_node_count++;

	connection_init(NearJesterThomasBack, 4);
	connection_add(NearJesterThomasBack, .to = ShulvaToGankFight1Front, .type = CT(Conn_ShulvaSanctumKey));
	connection_add(NearJesterThomasBack, .to = ShulvaEntranceBack);
	connection_add(NearJesterThomasBack, .to = NearJesterThomasFront);
	connection_add(NearJesterThomasBack, .to = ElanaTheSqualidQueenEntryFront, .type = CT(Conn_ActivateDragonStone));
	warp_node_count++;

	connection_init(ElanaTheSqualidQueenEntryFront, 0);
	warp_node_count++;

	connection_init(ElanaTheSqualidQueenEntryBack, 1);
	connection_add(ElanaTheSqualidQueenEntryBack, .to = BetweenElanaAndSihnFront);
	warp_node_count++;

	connection_init(BetweenElanaAndSihnFront, 0);
	warp_node_count++;

	connection_two_way(BetweenElanaAndSihnBack, SihnTheSlumberingDragonEntryFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(SihnTheSlumberingDragonEntryBack, 0);
	warp_node_count++;

	// shaded woods
	connection_two_way(MajulaToShadedWoodsBack, ShadedWoodsFromMajulaFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(ShadedWoodsFromMajulaBack, 4);
	connection_add(ShadedWoodsFromMajulaBack, .to = ShadedWoodsToMistyAreaFront, .type = CT(Conn_RosabethBranch));
	connection_add(ShadedWoodsFromMajulaBack, .to = DLC3EntranceBaseGame, .type = CT(Conn_RosabethBranch) | CT(Conn_FirstFourSouls));
	connection_add(ShadedWoodsFromMajulaBack, .to = DrangleicCastleToShadedWoodsFront, .type = CT(Conn_RosabethBranch) | CT(Conn_FirstFourSouls));
	connection_add(ShadedWoodsFromMajulaBack, .to = AldiasKeepEntranceFront, .type = CT(Conn_RosabethBranch) | CT(Conn_KingsRing));
	warp_node_count++;

	connection_init(ShadedWoodsToMistyAreaFront, 3);
	connection_add(ShadedWoodsToMistyAreaFront, .to = DLC3EntranceBaseGame, .type = CT(Conn_FirstFourSouls));
	connection_add(ShadedWoodsToMistyAreaFront, .to = DrangleicCastleToShadedWoodsFront, .type = CT(Conn_FirstFourSouls));
	connection_add(ShadedWoodsToMistyAreaFront, .to = AldiasKeepEntranceFront, .type = CT(Conn_KingsRing));
	warp_node_count++;

	connection_init(DLC3EntranceBaseGame, 4);
	connection_add(DLC3EntranceBaseGame, .to = ShadedWoodsToMistyAreaFront, .type = CT(Conn_FirstFourSouls));
	connection_add(DLC3EntranceBaseGame, .to = DrangleicCastleToShadedWoodsFront, .type = CT(Conn_FirstFourSouls));
	connection_add(DLC3EntranceBaseGame, .to = AldiasKeepEntranceFront, .type = CT(Conn_KingsRing) | CT(Conn_FirstFourSouls));
	connection_add(DLC3EntranceBaseGame, .to = DLC3EntranceDLC, .is_warp = true);
	warp_node_count++;

	connection_init(AldiasKeepEntranceFront, 3);
	connection_add(AldiasKeepEntranceFront, .to = DLC3EntranceBaseGame, .type = CT(Conn_FirstFourSouls) | CT(Conn_KingsRing));
	connection_add(AldiasKeepEntranceFront, .to = DrangleicCastleToShadedWoodsFront, .type = CT(Conn_FirstFourSouls) | CT(Conn_KingsRing));
	connection_add(AldiasKeepEntranceFront, .to = ShadedWoodsToMistyAreaFront, .type = CT(Conn_KingsRing));
	warp_node_count++;

	connection_init(DrangleicCastleToShadedWoodsFront, 3);
	connection_add(DrangleicCastleToShadedWoodsFront, .to = DLC3EntranceBaseGame, .type = CT(Conn_FirstFourSouls));
	connection_add(DrangleicCastleToShadedWoodsFront, .to = AldiasKeepEntranceFront, .type = CT(Conn_FirstFourSouls) | CT(Conn_KingsRing));
	connection_add(DrangleicCastleToShadedWoodsFront, .to = ShadedWoodsToMistyAreaFront, .type = CT(Conn_FirstFourSouls));
	warp_node_count++;

	connection_init(ShadedWoodsToMistyAreaBack, 2);
	connection_add(ShadedWoodsToMistyAreaBack, .to = ScorpionessNajkaEntryFront);
	connection_add(ShadedWoodsToMistyAreaBack, .to = ChasmPortalFromShadedWoodsSrc, .type = CT(Conn_DarkCovenentJoined));
	warp_node_count++;

	connection_init(ScorpionessNajkaEntryFront, 2);
	connection_add(ScorpionessNajkaEntryFront, .to = ShadedWoodsToMistyAreaBack);
	connection_add(ScorpionessNajkaEntryFront, .to = ChasmPortalFromShadedWoodsSrc, .type = CT(Conn_DarkCovenentJoined));
	warp_node_count++;

	connection_two_way(ScorpionessNajkaEntryBack, ScorpionessNajkaExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(ChasmPortalFromShadedWoodsSrc, 1);
	connection_add(ChasmPortalFromShadedWoodsSrc, .to = ChasmPortalFromShadedWoodsDst, .is_warp = true);
	warp_node_count++;

	connection_init(ChasmPortalFromShadedWoodsDst, 1);
	connection_add(ChasmPortalFromShadedWoodsDst, .to = DarkChasmFromShadedWoodsExitFront);
	warp_node_count++;

	connection_init(DarkChasmFromShadedWoodsExitFront, 0);
	warp_node_count++;

	connection_init(DarkChasmFromShadedWoodsExitBack, 1);
	connection_add(DarkChasmFromShadedWoodsExitBack, .to = ChasmShadedWoodsExitWarpSrc);
	warp_node_count++;

	connection_init(ChasmShadedWoodsExitWarpSrc, 1);
	connection_add(ChasmShadedWoodsExitWarpSrc, .to = ChasmShadedWoodsExitWarpDst, .is_warp = true);
	warp_node_count++;

	connection_init(ChasmShadedWoodsExitWarpDst, 3);
	connection_add(ChasmShadedWoodsExitWarpDst, .to = ShadedWoodsToMistyAreaBack);
	connection_add(ChasmShadedWoodsExitWarpDst, .to = ScorpionessNajkaEntryFront);
	connection_add(ChasmShadedWoodsExitWarpDst, .to = ChasmPortalFromShadedWoodsSrc, .type = CT(Conn_DarkCovenentJoined));
	warp_node_count++;

	connection_init(ScorpionessNajkaExitBack, 2);
	connection_add(ScorpionessNajkaExitBack, .to = DoorOfPharrosToRatKingDomainFront);
	connection_add(ScorpionessNajkaExitBack, .to = BrightstoneCoveToDoorsOfPharrosFront);
	warp_node_count++;

	// door of pharros

	connection_init(DoorOfPharrosToRatKingDomainFront, 2);
	connection_add(DoorOfPharrosToRatKingDomainFront, .to = ScorpionessNajkaExitBack);
	connection_add(DoorOfPharrosToRatKingDomainFront, .to = BrightstoneCoveToDoorsOfPharrosFront);
	warp_node_count++;

	connection_init(BrightstoneCoveToDoorsOfPharrosFront, 2);
	connection_add(BrightstoneCoveToDoorsOfPharrosFront, .to = ScorpionessNajkaExitBack);
	connection_add(BrightstoneCoveToDoorsOfPharrosFront, .to = DoorOfPharrosToRatKingDomainFront);
	warp_node_count++;

	connection_init(DoorOfPharrosToRatKingDomainBack, 1);
	connection_add(DoorOfPharrosToRatKingDomainBack, .to = DoorOfPharrosToOrdealEndBonfireFront);
	warp_node_count++;

	connection_init(DoorOfPharrosToOrdealEndBonfireFront, 1);
	connection_add(DoorOfPharrosToOrdealEndBonfireFront, .to = DoorOfPharrosToRatKingDomainBack);
	warp_node_count++;

	connection_two_way(DoorOfPharrosToOrdealEndBonfireBack, RoyalRatAuthorityEntryFront);
	warp_node_count++;
	warp_node_count++;

	connection_two_way(RoyalRatAuthorityEntryBack, RoyalRatVanguardExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(RoyalRatAuthorityExitBack, 3);
	connection_add(RoyalRatAuthorityExitBack, .to = ScorpionessNajkaExitBack);
	connection_add(RoyalRatAuthorityExitBack, .to = DoorOfPharrosToRatKingDomainFront);
	connection_add(RoyalRatAuthorityExitBack, .to = DoorOfPharrosToRatKingDomainFront);
	warp_node_count++;

	// brightstone cove

	connection_init(BrightstoneCoveToDoorsOfPharrosBack, 1);
	connection_add(BrightstoneCoveToDoorsOfPharrosBack, .to = CongregationEntryFront);
	warp_node_count++;

	connection_init(CongregationEntryFront, 0);
	warp_node_count++;

	connection_two_way(CongregationEntryBack, CongregationExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(CongregationExitBack, 1);
	connection_add(CongregationExitBack, .to = DukesDearFreyjaEntryFront);
	warp_node_count++;

	connection_init(DukesDearFreyjaEntryFront, 0);
	warp_node_count++;

	connection_init(DukesDearFreyjaEntryBack, 2);
	connection_add(DukesDearFreyjaEntryBack, .to = DukesDearFreyjaExitFront);
	connection_add(DukesDearFreyjaEntryBack, .to = DragonMemoriesCoveSrc, .type = CT(Conn_AshenMistHeart));
	warp_node_count++;

	connection_init(DragonMemoriesCoveSrc, 1);
	connection_add(DragonMemoriesCoveSrc, .to = DragonMemoriesCoveDst, .is_warp = true);
	warp_node_count++;

	connection_init(DragonMemoriesCoveDst, 1);
	connection_add(DragonMemoriesCoveDst, .to = DragonMemoriesMemorySrc);
	warp_node_count++;

	connection_init(DragonMemoriesMemorySrc, 1);
	connection_add(DragonMemoriesMemorySrc, .to = DragonMemoriesMemoryDst, .is_warp = true);
	warp_node_count++;

	connection_init(DragonMemoriesMemoryDst, 2);
	connection_add(DragonMemoriesMemoryDst, .to = DukesDearFreyjaEntryBack);
	connection_add(DragonMemoriesMemoryDst, .to = DukesDearFreyjaExitFront);
	warp_node_count++;

	connection_init(DukesDearFreyjaExitFront, 2);
	connection_add(DukesDearFreyjaExitFront, .to = DukesDearFreyjaEntryBack);
	connection_add(DukesDearFreyjaExitFront, .to = DragonMemoriesCoveSrc, .type = CT(Conn_AshenMistHeart));
	warp_node_count++;


	connection_init(DukesDearFreyjaExitBack, 0); // TODO: if i want to hide majula i should add to majula warp
	warp_node_count++;

	// dlc3

	connection_init(DLC3EntranceDLC, 3);
	connection_add(DLC3EntranceDLC, .to = AavaTheKingsPetEntryFront, .type = CT(Conn_DLC3Key));
	connection_add(DLC3EntranceDLC, .to = EleumLoyceAfterGettingEyeToSeeGhostsFront, .type = CT(Conn_DLC3Key));
	connection_add(DLC3EntranceDLC, .to = CoffinWarpSrc, .type = CT(Conn_FrigidOutskirtsKey));
	warp_node_count++;

	connection_init(AavaTheKingsPetEntryFront, 3);
	connection_add(AavaTheKingsPetEntryFront, .to = DLC3EntranceDLC, .type = CT(Conn_DLC3Key));
	connection_add(AavaTheKingsPetEntryFront, .to = EleumLoyceAfterGettingEyeToSeeGhostsFront);
	connection_add(AavaTheKingsPetEntryFront, .to = CoffinWarpSrc, .type = CT(Conn_FrigidOutskirtsKey));
	warp_node_count++;

	connection_init(CoffinWarpSrc, 1);
	connection_add(CoffinWarpSrc, .to = CoffinWarpDst, .is_warp = true);
	warp_node_count++;

	connection_init(CoffinWarpDst, 1);
	connection_add(CoffinWarpDst, .to = LudAndZallenEntryFront);
	warp_node_count++;

	connection_init(LudAndZallenEntryFront, 0);
	warp_node_count++;

	connection_two_way(LudAndZallenEntryBack, LudAndZallenExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(LudAndZallenExitBack, 1);
	connection_add(LudAndZallenExitBack, .to = LudAndZallenExitWarpSrc, .type = CT(Conn_LudAndZallenDead));
	warp_node_count++;

	connection_init(LudAndZallenExitWarpSrc, 1);
	connection_add(LudAndZallenExitWarpSrc, .to = LudAndZallenExitWarpDst, .is_warp = true);
	warp_node_count++;

	connection_init(LudAndZallenExitWarpDst, 1);
	connection_add(LudAndZallenExitWarpDst, .to = CoffinWarpSrc);
	warp_node_count++;

	connection_init(EleumLoyceAfterGettingEyeToSeeGhostsFront, 0);
	warp_node_count++;

	connection_init(EleumLoyceAfterGettingEyeToSeeGhostsBack, 6);
	connection_add(EleumLoyceAfterGettingEyeToSeeGhostsBack, .to = AavaTheKingsPetEntryFront);
	connection_add(EleumLoyceAfterGettingEyeToSeeGhostsBack, .to = DLC3EntranceDLC, .type = CT(Conn_DLC3Key));
	connection_add(EleumLoyceAfterGettingEyeToSeeGhostsBack, .to = EleumLoyceAfterGettingEyeToSeeGhostsFront);
	connection_add(EleumLoyceAfterGettingEyeToSeeGhostsBack, .to = CoffinWarpSrc, .type = CT(Conn_FrigidOutskirtsKey));
	connection_add(EleumLoyceAfterGettingEyeToSeeGhostsBack, .to = AavaTheKingsPetExitBack, .type = CT(Conn_DLC3Unfreezed));
	connection_add(EleumLoyceAfterGettingEyeToSeeGhostsBack, .to = EleumLoyceCathedraEntranceFront, .type = CT(Conn_DLC3Unfreezed));
	warp_node_count++;

	connection_init(AavaTheKingsPetExitBack, 1);
	connection_add(AavaTheKingsPetExitBack, .to = EleumLoyceCathedraEntranceFront);
	warp_node_count++;

	connection_init(EleumLoyceCathedraEntranceFront, 1);
	connection_add(EleumLoyceCathedraEntranceFront, .to = AavaTheKingsPetExitBack);
	warp_node_count++;

	connection_two_way(AavaTheKingsPetEntryBack, AavaTheKingsPetExitFront, .type = CT(Conn_DLC3Eye));
	warp_node_count++;
	warp_node_count++;

	connection_init(EleumLoyceCathedraEntranceBack, 1);
	connection_add(EleumLoyceCathedraEntranceBack, .to = IvoryKingFront);
	warp_node_count++;

	connection_init(IvoryKingFront, 0);
	warp_node_count++;

	connection_init(IvoryKingBack, 1);
	connection_add(IvoryKingBack, .to = IvoryKingFightEndSrc);
	warp_node_count++;

	connection_init(IvoryKingFightEndSrc, 1);
	connection_add(IvoryKingFightEndSrc, .to = IvoryKingFightEndDst, .is_warp = true);
	warp_node_count++;

	connection_init(IvoryKingFightEndDst, 1);
	connection_add(IvoryKingFightEndDst, .to = IvoryKingFront);
	warp_node_count++;

	// drangleic castle

	connection_init(DrangleicCastleToShadedWoodsBack, 3);
	connection_add(DrangleicCastleToShadedWoodsBack, .to = DrangleicCastleToChasmPortalFront);
	connection_add(DrangleicCastleToShadedWoodsBack, .to = FinalFightArenaFront, .type = CT(Conn_KingsRing));
	connection_add(DrangleicCastleToShadedWoodsBack, .to = TwinDragonridersEntryFront);
	warp_node_count++;

	connection_init(DrangleicCastleToChasmPortalFront, 2);
	connection_add(DrangleicCastleToChasmPortalFront, .to = FinalFightArenaFront, .type = CT(Conn_KingsRing));
	connection_add(DrangleicCastleToChasmPortalFront, .to = TwinDragonridersEntryFront);
	warp_node_count++;

	connection_init(TwinDragonridersEntryFront, 0);
	warp_node_count++;

	connection_init(FinalFightArenaFront, 2);
	connection_add(FinalFightArenaFront, .to = DrangleicCastleToChasmPortalFront, .type = CT(Conn_KingsRing));
	connection_add(FinalFightArenaFront, .to = TwinDragonridersEntryFront, .type = CT(Conn_KingsRing));
	warp_node_count++;

	connection_init(DrangleicCastleToChasmPortalBack, 1);
	connection_add(DrangleicCastleToChasmPortalBack, .to = ChasmPortalFromCastleSrc, .type = CT(Conn_DarkCovenentJoined));
	warp_node_count++;

	connection_init(ChasmPortalFromCastleSrc, 1);
	connection_add(ChasmPortalFromCastleSrc, .to = ChasmPortalFromCastleDst, .is_warp = true);
	warp_node_count++;

	connection_init(ChasmPortalFromCastleDst, 1);
	connection_add(ChasmPortalFromCastleDst, .to = DarkChasmFromDrangleicCastleExitFront);
	warp_node_count++;

	connection_init(DarkChasmFromDrangleicCastleExitFront, 0);
	warp_node_count++;

	connection_init(DarkChasmFromDrangleicCastleExitBack, 1);
	connection_add(DarkChasmFromDrangleicCastleExitBack, .to = ChasmCastleExitWarpSrc);
	warp_node_count++;

	connection_init(ChasmCastleExitWarpSrc, 1);
	connection_add(ChasmCastleExitWarpSrc, .to = ChasmCastleExitWarpDst, .is_warp = true);
	warp_node_count++;

	connection_init(ChasmCastleExitWarpDst, 1);
	connection_add(ChasmCastleExitWarpDst, .to = ChasmPortalFromCastleSrc, .type = CT(Conn_DarkCovenentJoined));
	warp_node_count++;

	connection_init(FinalFightArenaBack, 0);
	warp_node_count++;

	connection_two_way(TwinDragonridersEntryBack, TwinDragonridersExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(TwinDragonridersExitBack, 1);
	connection_add(TwinDragonridersExitBack, .to = LookingGlassKnightEntryFront, .type = CT(Conn_KeyToKingsPassage));
	warp_node_count++;

	connection_init(LookingGlassKnightEntryFront, 0);
	warp_node_count++;

	connection_two_way(LookingGlassKnightEntryBack, LookingGlassKnightExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_two_way(LookingGlassKnightExitBack, ShrineOfAmanaToDrangleicCastleFront);
	warp_node_count++;
	warp_node_count++;

	// shrine of amana

	connection_two_way(ShrineOfAmanaToDrangleicCastleBack, ShrineOfAmanaTo2ndBonfireFront);
	warp_node_count++;
	warp_node_count++;

	connection_two_way(ShrineOfAmanaTo2ndBonfireBack, ShrineOfAmanaTo3rdBonfireFront);
	warp_node_count++;
	warp_node_count++;

	connection_two_way(ShrineOfAmanaTo3rdBonfireBack, DemonOfSongEntryFront);
	warp_node_count++;
	warp_node_count++;

	connection_two_way(DemonOfSongEntryBack, DemonOfSongExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(DemonOfSongExitBack, 1);
	connection_add(DemonOfSongExitBack, .to = UndeadCryptToShrineOfAmanaFront);
	warp_node_count++;

	connection_init(UndeadCryptToShrineOfAmanaFront, 0);
	warp_node_count++;

	// undead crypt

	connection_init(UndeadCryptToShrineOfAmanaBack, 3);
	connection_add(UndeadCryptToShrineOfAmanaBack, .to = UndeadCryptFromAgdayneToBonfireFront);
	connection_add(UndeadCryptToShrineOfAmanaBack, .to = UndeadCryptFromAgdayneToBonfireBack);
	connection_add(UndeadCryptToShrineOfAmanaBack, .to = VelsdatEntryFront);
	warp_node_count++;

	connection_init(UndeadCryptFromAgdayneToBonfireFront, 3);
	connection_add(UndeadCryptFromAgdayneToBonfireFront, .to = UndeadCryptToShrineOfAmanaBack);
	connection_add(UndeadCryptFromAgdayneToBonfireFront, .to = UndeadCryptFromAgdayneToBonfireBack);
	connection_add(UndeadCryptFromAgdayneToBonfireFront, .to = VelsdatEntryFront);
	warp_node_count++;

	connection_init(UndeadCryptFromAgdayneToBonfireBack, 3);
	connection_add(UndeadCryptFromAgdayneToBonfireBack, .to = UndeadCryptToShrineOfAmanaBack);
	connection_add(UndeadCryptFromAgdayneToBonfireBack, .to = UndeadCryptFromAgdayneToBonfireFront);
	connection_add(UndeadCryptFromAgdayneToBonfireBack, .to = VelsdatEntryFront);
	warp_node_count++;

	connection_init(VelsdatEntryFront, 3);
	connection_add(VelsdatEntryFront, .to = UndeadCryptToShrineOfAmanaBack);
	connection_add(VelsdatEntryFront, .to = UndeadCryptFromAgdayneToBonfireFront);
	connection_add(VelsdatEntryFront, .to = UndeadCryptFromAgdayneToBonfireBack);
	warp_node_count++;

	connection_init(VelsdatEntryBack, 1);
	connection_add(VelsdatEntryBack, .to = VelsdatExitFront);
	warp_node_count++;

	connection_init(VelsdatExitFront, 1);
	connection_add(VelsdatExitFront, .to = VelsdatEntryBack);
	warp_node_count++;

	connection_two_way(VelsdatExitBack, VendrickFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(VendrickBack, 1);
	connection_add(VendrickBack, .to = MemoryOfTheKingCryptSrc, .type = CT(Conn_AshenMistHeart));
	warp_node_count++;

	connection_init(MemoryOfTheKingCryptSrc, 1);
	connection_add(MemoryOfTheKingCryptSrc, .to = MemoryOfTheKingCryptDst, .is_warp = true);
	warp_node_count++;

	connection_init(MemoryOfTheKingCryptDst, 1);
	connection_add(MemoryOfTheKingCryptDst, .to = MemoryOfTheKingMemorySrc);
	warp_node_count++;

	connection_init(MemoryOfTheKingMemorySrc, 1);
	connection_add(MemoryOfTheKingMemorySrc, .to = MemoryOfTheKingMemoryDst, .is_warp = true);
	warp_node_count++;

	connection_init(MemoryOfTheKingMemoryDst, 2);
	connection_add(MemoryOfTheKingMemoryDst, .to = VelsdatEntryBack);
	connection_add(MemoryOfTheKingMemoryDst, .to = VelsdatExitFront);
	warp_node_count++;

	// aldias keep

	connection_init(AldiasKeepEntranceBack, 1);
	connection_add(AldiasKeepEntranceBack, .to = GuardianDragonEntryFront);
	warp_node_count++;

	connection_init(GuardianDragonEntryFront, 0);
	warp_node_count++;

	connection_two_way(GuardianDragonEntryBack, GuardianDragonExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_two_way(GuardianDragonExitBack, DragonAerieToAldiasKeepFront);
	warp_node_count++;
	warp_node_count++;

	// dragon aerie

	connection_two_way(DragonAerieToAldiasKeepBack, AncientDragonFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(AncientDragonBack, 0);
	warp_node_count++;

	// darklurker
	connection_init(DarkLurkerSpawn, 1);
	connection_add(DarkLurkerSpawn, .to = DarkLurkerExitFront);
	warp_node_count++;

	connection_init(DarkLurkerExitFront, 0);
	warp_node_count++;

	connection_init(DarkLurkerExitBack, 1);
	connection_add(DarkLurkerExitBack, .to = ChasmDarkLurkerExitWarpSrc);
	warp_node_count++;

	connection_init(ChasmDarkLurkerExitWarpSrc, 1);
	connection_add(ChasmDarkLurkerExitWarpSrc, .to = ChasmDarkLurkerExitWarpDst, .is_warp = true);
	warp_node_count++;

	connection_init(ChasmDarkLurkerExitWarpDst, 1);
	// TODO: confirm this => this is not correct as it depends on where you entered from
	connection_add(ChasmDarkLurkerExitWarpDst, .to = ChasmPortalFromCastleSrc, .type = CT(Conn_DarkCovenentJoined));
	warp_node_count++;

	assert(warp_node_count == __WarpNodeCount);
	// printf("Completed %zu/%d\n\n", warp_node_count, __WarpNodeCount);
	//
	// size_t count = 0;
	// da_foreach(ConnectionGroup, c, &connections) {
	// 	printf("%zu: [%zu] %s\n", count, c->count, WarpNodeNames[count]);
	// 	count++;
	// }
};

void free_connections() {
	da_foreach(ConnectionGroup, cg, &connections) {
		da_free(*cg);
	}
	da_free(connections);
}

WarpNode default_connections[__WarpNodeCount] = {0};

typedef Ht(WarpNode, WarpNode) Warps;

void warp_add(Warps *warps, WarpNode src) {
	WarpNode *added = ht_find(warps, src);
	if (!added) {
		WarpNode dst = default_connections[src];
		*ht_put(warps, src) = dst;
		ConnectionGroup cg = connections.items[dst];
		da_foreach(Connection, node, &cg) {
			warp_add(warps, node->to);
		}
	}
}

void generate_default_connections() {
#define X(val) default_connections[val ## Src] = val ## Dst;
	WarpPoints
#undef X
#define X(val) default_connections[val ## Front] = val ## Back;
	FogGateNames
#undef X
#define X(val) default_connections[val ## Back] = val ## Front;
	FogGateNames
#undef X
	default_connections[NearPursuerBirdEntry] = NearPursuerBirdExit;
	default_connections[PirateShipWharf] = PirateShipBastille;
	default_connections[DLC1EntranceBaseGame] = DLC1EntranceDLC;
	default_connections[DLC2EntranceBaseGame] = DLC2EntranceDLC;
	default_connections[DLC3EntranceBaseGame] = DLC3EntranceDLC;
	default_connections[DLC1EntranceDLC] = DLC1EntranceBaseGame;
	default_connections[DLC2EntranceDLC] = DLC2EntranceBaseGame;
	default_connections[DLC3EntranceDLC] = DLC3EntranceBaseGame;

}

int get_boss_spawn_event_loc(WarpNode name) {
	switch (name) {
		case DukesDearFreyjaEntryBack: return 100000;
		case OldIronKingEntryBack: return 1500000;
		case DarkLurkerExitFront: return 200000;
		case OldDragonslayerExitFront: return 600000;
		default: return -1;
	}
};

typedef struct {
	size_t count;
	int items[6];
} FixedIntArray;

FixedIntArray BlackGulchChasmEnemyIds = {
	.count = 6,
	.items = {1005, 1006, 1007, 1008, 3200, 3210 }
};
FixedIntArray DrangleicCastleChasmEnemyIds = {
	.count = 4,
	.items = {1004, 1009, 1012, 3100}
};
FixedIntArray ShadedWoodsChasmEnemyIds = {
	.count = 4,
	.items = {1000, 1002, 1003, 3000}
};

FixedIntArray get_chasm_door_enemy_ids(WarpNode name) {
	switch (name) {
		case DarkChasmFromBlackGulchExitFront:
			return BlackGulchChasmEnemyIds;
		case DarkChasmFromShadedWoodsExitFront:
			return ShadedWoodsChasmEnemyIds;
		case DarkChasmFromDrangleicCastleExitFront:
			return DrangleicCastleChasmEnemyIds;
		default:
			return (FixedIntArray){0};
	}
}

int get_chasm_event_flag(WarpNode name) {
	switch (name) {
		case DarkChasmFromShadedWoodsExitFront:
			return 403000001;
		case DarkChasmFromDrangleicCastleExitFront:
			return 403000002;
		case DarkChasmFromBlackGulchExitFront:
			return 403000003;
		default:
			return -1;
	}
}

bool is_bell_check_required(WarpNode name) {
	switch (name) {
		case FlexileSentryEntryFront:
		case FlexileSentryEntryBack:
		case FlexileSentryExitFront:
		case FlexileSentryExitBack:
			return true;
		default:
			return false;
	}
}

DA(ConnectionType, Keys);

Keys null_keys = {0};

typedef enum {
	KeyArea,
	KeyChest,
	KeyBoss,
	KeyEvent,
	KeyGuranteedDrop,
	KeyNPC,
	KeyShop,
} KeyType;

typedef struct {
	size_t id;
	Keys keys;
	KeyType type;
	const char *desc;
} ItemInfo;

DA(ItemInfo, ItemInfoGroup);

Ht(WarpNode, ItemInfoGroup) gate_item_infos = {0};

void generate_item_infos(); // Will have implementation in item_info.c
#include "item_info.c" // autogenerated file

void free_item_infos() {
	ht_foreach(iig, &gate_item_infos) {
		da_foreach(ItemInfo, ii, iig) {
			da_free(ii->keys);
		}
		da_free(*iig);
	}
	ht_free(&gate_item_infos);
}

WarpNode get_item_warp_node(size_t item_id) {
	ht_foreach(iig, &gate_item_infos) {
		WarpNode wn = ht_key(&gate_item_infos, iig);
		da_foreach(ItemInfo, ii, iig) {
			if (ii->id == item_id) {
				return wn;
			}
		}
	}
	return __WarpNodeCount;
}

#define LotID_Chr_BrightstoneKey 50830000
#define LotID_Item_FragrantBranchOfYore 60537000

// not considered in logic:
// 1. Jump down from bonfire above dragonrider
// 2. Jump down from Gilligan ladder location in earthen peak to harvest valley
// 3. Unlocking Gilligan ladder in Majula
// 4. Luring and enemy to the soul-opening-door in Drangleic Castle from behind the main path and opening it to access Ruin
//	  Sentinels Room
// 5. Killing Licia when items are randomized in tower of flame might softlock you as she is able to unlock Rotunda
//
// might be difficult to find these
// 1. You can enter back into smeltor demon arena after the fog gate by jumping down
// 2. You can drop down in undead crypt to skip the first mandatory fog gate and reach bonfire Undead Ditch
// 3. You can drop behind the foggate near the windmill burn location in earthen peak
// 4. You can drop from the bonfire in 3. to access other parts of earthen peak
// 5. In shulva after gank fight foggate you can drop and explore whole initial shulva section
// 6. You do not need to light the sconce in darkchasm dungeons to complete it, just kill all the enemies and exit from fog gate 
//    to resgister it as completed dungeon
//
