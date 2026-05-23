#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "common.h"

#define NOB_DA_INIT_CAP 32
#include "nob.h"

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
	Vendrick,
	GuardianDragon,
	AncientDragon,
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

typedef enum {
	WPBT_None = 0,
	WPBT_Inaccessible,
	WPBT_PharrosLockstone,
	WPBT_FragrantBranchOfYore,
} WarpPointBlockType;

typedef enum {
	Lone = 0,
	// game start spawn point
	GameStartSpawnSrc,
	GameStartSpawnDst,

	// warp gates
	NearPursuerBirdEntry,
	NearPursuerGiantMemoryEntrySrc,
	NearPateGiantMemoryEntrySrc,
	GiantLordMemoryEntrySrc,
	NearPursuerGiantMemoryExitDst,
	NearPateGiantMemoryExitDst,
	GiantLordMemoryExitDst,
	//
	NearPursuerBirdExit,
	NearPursuerGiantMemoryEntryDst,
	NearPateGiantMemoryEntryDst,
	GiantLordMemoryEntryDst,
	NearPursuerGiantMemoryExitSrc,
	NearPateGiantMemoryExitSrc,
	GiantLordMemoryExitSrc,
	//
	PirateShipWharf,
	PirateShipBastille,
	//
	DLC1EntranceBaseGame,
	DLC2EntranceBaseGame,
	DLC3EntranceBaseGame,
	DLC1EntranceDLC,
	DLC2EntranceDLC,
	DLC3EntranceDLC,
	//
	SirAlonneArmorDLCEntrySrc,
	SirAlonneArmorDLCEntryDst,
	SirAlonneMemoryExitSrc,
	SirAlonneMemoryExitDst,
	//
	ChasmPortalFromBlackGulchSrc,
	ChasmPortalFromShadedWoodsSrc,
	ChasmPortalFromCastleSrc,
	//
	ChasmPortalFromBlackGulchDst,
	ChasmPortalFromShadedWoodsDst,
	ChasmPortalFromCastleDst,
	//
	ChasmGulchExitWarpSrc,
	ChasmShadedWoodsExitWarpSrc,
	ChasmCastleExitWarpSrc,
	ChasmDarkLurkerExitWarpSrc,
	ChasmGulchExitWarpDst,
	ChasmShadedWoodsExitWarpDst,
	ChasmCastleExitWarpDst,
	ChasmDarkLurkerExitWarpDst,
	//
	IvoryKingFightEndSrc,
	IvoryKingFightEndDst,
	CoffinWarpSrc,
	CoffinWarpDst,
	LudAndZallenExitWarpSrc,
	LudAndZallenExitWarpDst,
	//
	DragonMemoriesCoveSrc,
	DragonMemoriesCoveDst,
	DragonMemoriesMemoryDst,
	DragonMemoriesMemorySrc,
	MemoryOfTheKingCryptSrc,
	MemoryOfTheKingCryptDst,
	MemoryOfTheKingMemoryDst,
	MemoryOfTheKingMemorySrc,

	// Things betwixt
	Tutorial1EntryFront,
	Tutorial1ExitFront,
	Tutorial2EntryFront,
	Tutorial2ExitFront,
	Tutorial3EntryFront,
	Tutorial3ExitFront,
	Tutorial1EntryBack,
	Tutorial1ExitBack,
	Tutorial2EntryBack,
	Tutorial2ExitBack,
	Tutorial3EntryBack,
	Tutorial3ExitBack,

	// Majula
	MajulaToForestOfFallenGiantsFront,
	MajulaToGraveOfSaintsFront,
	MajulaToHuntsmanCopseFront,
	MajulaToRotundaLockstoneFront,
	MajulaToGutterFront,
	MajulaToShadedWoodsFront,
	MajulaToForestOfFallenGiantsBack,
	MajulaToGraveOfSaintsBack,
	MajulaToHuntsmanCopseBack,
	MajulaToRotundaLockstoneBack,
	MajulaToGutterBack,
	MajulaToShadedWoodsBack,

	// Forest of the fallen giants
	LastGiantEntryFront,
	PursuerEntryFront,
	PursuerExitFront,
	ForestOfFallenGiantsBalconyFront,
	ForestOfFallenGiantsToCaleFront,
	ForestOfFallenGiantsAfterFirstBonfireFront,
	ForestOfFallenGiantsFromMajulaFront,
	ForestOfFallenGiantsToPursuerArenaFront,
	LastGiantEntryBack,
	PursuerEntryBack,
	PursuerExitBack,
	ForestOfFallenGiantsBalconyBack,
	ForestOfFallenGiantsToCaleBack,
	ForestOfFallenGiantsAfterFirstBonfireBack,
	ForestOfFallenGiantsFromMajulaBack,
	ForestOfFallenGiantsToPursuerArenaBack,

	// Brightstone cove tseldora
	CongregationEntryFront,
	CongregationExitFront,
	BrightstoneCoveToDoorsOfPharrosFront,
	DukesDearFreyjaEntryFront,
	DukesDearFreyjaExitFront,
	CongregationEntryBack,
	CongregationExitBack,
	BrightstoneCoveToDoorsOfPharrosBack,
	DukesDearFreyjaEntryBack,
	DukesDearFreyjaExitBack,

	// Aldias keep
	GuardianDragonEntryFront,
	GuardianDragonExitFront,
	AldiasKeepEntranceFront,
	GuardianDragonEntryBack,
	GuardianDragonExitBack,
	AldiasKeepEntranceBack,

	// The lost bastille
	GargoylesEntryFront,
	GargoylesExitFront,
	RuinSentinelsEntryFront,
	RuinSentinelsExitFront,
	RuinSentinelsHiddenDoor1ExitFront,
	RuinSentinelsHiddenDoor2ExitFront,
	RuinSentinelsHiddenDoor3ExitFront,
	RuinSentinelsHiddenDoor4ExitFront,
	RuinSentinesUpperExitFront,
	LostBastilleToSinnersRiseFront,
	LostBastilleToShipFromWharfFront,
	LostBastilleToBelfryLunaFront,
	LostSinnerEntryFront,
	LostSinnerExitFront,
	GargoylesEntryBack,
	GargoylesExitBack,
	RuinSentinelsEntryBack,
	RuinSentinelsExitBack,
	RuinSentinelsHiddenDoor1ExitBack,
	RuinSentinelsHiddenDoor2ExitBack,
	RuinSentinelsHiddenDoor3ExitBack,
	RuinSentinelsHiddenDoor4ExitBack,
	RuinSentinesUpperExitBack,
	LostBastilleToSinnersRiseBack,
	LostBastilleToShipFromWharfBack,
	LostBastilleToBelfryLunaBack,
	LostSinnerEntryBack,
	LostSinnerExitBack,

	// Harvest valley and earthen peak
	CovetousDemonEntryFront,
	EarthenPeakNearWindmillBurnLocationFront,
	MythaEntryFront,
	CovetousDemonExitFront,
	MythaExitFront,
	HarvestValleyToSkeletonLordsFront,
	CovetousDemonEntryBack,
	EarthenPeakNearWindmillBurnLocationBack,
	MythaEntryBack,
	CovetousDemonExitBack,
	MythaExitBack,
	HarvestValleyToSkeletonLordsBack,

	// No mans wharf
	FlexileSentryExitFront,
	FlexileSentryEntryFront,
	NoMansWharfToHeidesFront,
	FlexileSentryExitBack,
	FlexileSentryEntryBack,
	NoMansWharfToHeidesBack,

	// Iron keep
	SmeltorDemonEntryFront,
	IronKeepNearFlameThrowerAndTurtleFront,
	BelfrySolExitFront,
	BelfrySolEntryFront,
	OldIronKingEntryFront,
	OldIronKingExitFront,
	SmeltorDemonExitFront,
	SmeltorDemonToBonfireFront,
	IronKeepToBelfryAtPharrosLockstoneFront,
	IronKeepToEarthenPeakFront,
	SmeltorDemonEntryBack,
	IronKeepNearFlameThrowerAndTurtleBack,
	BelfrySolExitBack,
	BelfrySolEntryBack,
	OldIronKingEntryBack,
	OldIronKingExitBack,
	SmeltorDemonExitBack,
	SmeltorDemonToBonfireBack,
	IronKeepToBelfryAtPharrosLockstoneBack,
	IronKeepToEarthenPeakBack,

	// Huntsman Copse
	SkeletonLordsEntryFront,
	ChariotEntryFront,
	SkeletonLordsExitFront,
	ChariotExitFront,
	HuntsmanCopseToMajulaFront,
	SkeletonLordsEntryBack,
	ChariotEntryBack,
	SkeletonLordsExitBack,
	ChariotExitBack,
	HuntsmanCopseToMajulaBack,

	// The gutter and black gulch
	GutterNearAntQueenFront,
	TheRottenExitFront,
	BlackGulchEntranceFront,
	TheGutterFromGraveOfSaintsFront,
	TheRottenEntryFront,
	GutterNearAntQueenBack,
	TheRottenExitBack,
	BlackGulchEntranceBack,
	TheGutterFromGraveOfSaintsBack,
	TheRottenEntryBack,

	// Dragon aerie
	DragonAerieToAldiasKeepFront,
	AncientDragonFront,
	DragonAerieToAldiasKeepBack,
	AncientDragonBack,

	// Shaded woods -> Majula
	ShadedWoodsFromMajulaFront,
	ShadedWoodsFromMajulaBack,

	// Heides tower of flame
	DragonriderEntryFront,
	DragonriderExitFront,
	HeidesToMajulaFront,
	HeidesToWharfFront,
	OldDragonslayerEntryFront,
	OldDragonslayerExitFront,
	DragonriderEntryBack,
	DragonriderExitBack,
	HeidesToMajulaBack,
	HeidesToWharfBack,
	OldDragonslayerEntryBack,
	OldDragonslayerExitBack,

	// Shaded woods -> Shrine of winter
	ScorpionessNajkaExitFront,
	ShadedWoodsToMistyAreaFront,
	ScorpionessNajkaEntryFront,
	ScorpionessNajkaExitBack,
	ShadedWoodsToMistyAreaBack,
	ScorpionessNajkaEntryBack,

	// Door of pharros
	RoyalRatAuthorityExitFront,
	DoorOfPharrosToOrdealEndBonfireFront,
	DoorOfPharrosToRatKingDomainFront,
	RoyalRatAuthorityEntryFront,
	RoyalRatAuthorityExitBack,
	DoorOfPharrosToOrdealEndBonfireBack,
	DoorOfPharrosToRatKingDomainBack,
	RoyalRatAuthorityEntryBack,

	// Grave of saints
	GraveOfSaintsFromPitFront,
	GraveOfSaintsNearUpperBonfireFront,
	RoyalRatVanguardEntryFront,
	RoyalRatVanguardExitFront,
	GraveOfSaintsFromPitBack,
	GraveOfSaintsNearUpperBonfireBack,
	RoyalRatVanguardEntryBack,
	RoyalRatVanguardExitBack,

	// Memory of vammar, orro and jeigh
	GiantLordEntryFront,
	GiantLordExitFront,
	GiantLordEntryBack,
	GiantLordExitBack,

	// Shrine of amana
	DemonOfSongEntryFront,
	ShrineOfAmanaTo2ndBonfireFront,
	ShrineOfAmanaTo3rdBonfireFront,
	DemonOfSongExitFront,
	ShrineOfAmanaToDrangleicCastleFront,
	DemonOfSongEntryBack,
	ShrineOfAmanaTo2ndBonfireBack,
	ShrineOfAmanaTo3rdBonfireBack,
	DemonOfSongExitBack,
	ShrineOfAmanaToDrangleicCastleBack,

	// Drangleic castle
	LookingGlassKnightEntryFront,
	TwinDragonridersEntryFront,
	TwinDragonridersExitFront,
	LookingGlassKnightExitFront,
	FinalFightArenaFront,
	DrangleicCastleToChasmPortalFront,
	DrangleicCastleToShadedWoodsFront,
	LookingGlassKnightEntryBack,
	TwinDragonridersEntryBack,
	TwinDragonridersExitBack,
	LookingGlassKnightExitBack,
	FinalFightArenaBack,
	DrangleicCastleToChasmPortalBack,
	DrangleicCastleToShadedWoodsBack,

	// Undead Crypt
	UndeadCryptToShrineOfAmanaFront,
	VelsdatEntryFront,
	VelsdatExitFront,
	UndeadCryptFromAgdayneToBonfireFront,
	VendrickFront,
	UndeadCryptToShrineOfAmanaBack,
	VelsdatEntryBack,
	VelsdatExitBack,
	UndeadCryptFromAgdayneToBonfireBack,
	VendrickBack,

	// Dark chasm of old
	DarkChasmFromBlackGulchExitFront,
	DarkChasmFromDrangleicCastleExitFront,
	DarkChasmFromShadedWoodsExitFront,
	DarkLurkerExitFront,
	DarkChasmFromBlackGulchExitBack,
	DarkChasmFromDrangleicCastleExitBack,
	DarkChasmFromShadedWoodsExitBack,
	DarkLurkerExitBack,

	// Shulva sanctum city
	SihnTheSlumberingDragonEntryFront,
	ElanaTheSqualidQueenEntryFront,
	GankSquadBossEntryFront,
	GankSquadBossExitFront,
	ShulvaToGankFight1Front,
	ShulvaEntranceFront,
	BetweenElanaAndSihnFront,
	ShulvaToGankFight2Front,
	NearJesterThomasFront,
	SihnTheSlumberingDragonEntryBack,
	ElanaTheSqualidQueenEntryBack,
	GankSquadBossEntryBack,
	GankSquadBossExitBack,
	ShulvaToGankFight1Back,
	ShulvaEntranceBack,
	BetweenElanaAndSihnBack,
	ShulvaToGankFight2Back,
	NearJesterThomasBack,

	// Brume Tower
	BrumeTowerFrom2ndBonfireToCentralRoomFront,
	BrumeTowerToFoyerBonfireFromOutsideFront,
	BrumeTowerToScorchingIronSceptorKeyFront,
	SirAlonneEntryFront,
	SirAlonneExitFront,
	FumeKnightEntryFront,
	FumeKnightExitFront,
	BlueSmeltorDemonEntryFront,
	BlueSmeltorDemonExitFront,
	BrumeTowerEntranceFromLiftFront,
	BrumeTowerToBlueSmelterDungeonFront,
	BrumeTowerFrom2ndBonfireToCentralRoomBack,
	BrumeTowerToFoyerBonfireFromOutsideBack,
	BrumeTowerToScorchingIronSceptorKeyBack,
	SirAlonneEntryBack,
	SirAlonneExitBack,
	FumeKnightEntryBack,
	FumeKnightExitBack,
	BlueSmeltorDemonEntryBack,
	BlueSmeltorDemonExitBack,
	BrumeTowerEntranceFromLiftBack,
	BrumeTowerToBlueSmelterDungeonBack,

	// Frozen eleum loyce
	AavaTheKingsPetExitFront,
	EleumLoyceAfterGettingEyeToSeeGhostsFront,
	IvoryKingFront,
	LudAndZallenEntryFront,
	AavaTheKingsPetEntryFront,
	LudAndZallenExitFront,
	EleumLoyceCathedraEntranceFront,
	AavaTheKingsPetExitBack,
	EleumLoyceAfterGettingEyeToSeeGhostsBack,
	IvoryKingBack,
	LudAndZallenEntryBack,
	AavaTheKingsPetEntryBack,
	LudAndZallenExitBack,
	EleumLoyceCathedraEntranceBack,

	__WarpNodeCount,
} WarpNode;

typedef struct {
	bool is_foggate;
	int fog_gate_id; // can be generated when reading the map_file
	int loc_id; // needs to be filled dynamically when generating event loc
	MapName map;
	WarpNode cutscene_gate_id;
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
	.loc_id = -1,\
	.map = __MapNameCount,\
	.cutscene_gate_id = __WarpNodeCount,\
	.pvp = false,\
	.reverse = false,\
	.boss = Boss_None,\
	.blocktype = WPBT_None,\
	.obj_id = NULL,\
	.offset = {0},\
	__VA_ARGS__,\
}

WarpPoint warp_points[] = {
	[Lone]                           = NEW_WARP_PT(.is_foggate = false),
	[GameStartSpawnSrc]              = NEW_WARP_PT(.is_foggate = false, .map = ThingsBetwixt),
	[GameStartSpawnDst]              = NEW_WARP_PT(.is_foggate = false, .map = ThingsBetwixt),
	// warp gates
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
	[DukesDearFreyjaEntryFront]            = NEW_WARP_PT(.map = BrightstoneCoveTseldora, .obj_id = "o00_0501_0001", .reverse = true),
	[DukesDearFreyjaExitFront]             = NEW_WARP_PT(.map = BrightstoneCoveTseldora, .obj_id = "o00_0502_0000", .boss = TheDukesDearFreja, .cutscene_gate_id = DukesDearFreyjaEntryBack),

	[CongregationEntryBack]                = NEW_WARP_PT(.map = BrightstoneCoveTseldora, .obj_id = "o00_0500_0000", .boss = ProwlingMagusAndCongregation),
	[CongregationExitBack]                 = NEW_WARP_PT(.map = BrightstoneCoveTseldora, .obj_id = "o00_0500_0001"),
	[BrightstoneCoveToDoorsOfPharrosBack]  = NEW_WARP_PT(.map = BrightstoneCoveTseldora, .obj_id = "o00_0500_0002", .pvp = true),
	[DukesDearFreyjaEntryBack]             = NEW_WARP_PT(.map = BrightstoneCoveTseldora, .obj_id = "o00_0501_0001", .boss = TheDukesDearFreja, .reverse = true),
	[DukesDearFreyjaExitBack]              = NEW_WARP_PT(.map = BrightstoneCoveTseldora, .obj_id = "o00_0502_0000"),

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
	[RuinSentinelsHiddenDoor1ExitBack]  = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0007", .boss = RuinSentinels),
	[RuinSentinelsHiddenDoor2ExitBack]  = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0008", .boss = RuinSentinels),
	[RuinSentinelsHiddenDoor3ExitBack]  = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0009", .boss = RuinSentinels),
	[RuinSentinelsHiddenDoor4ExitBack]  = NEW_WARP_PT(.map = TheLostBastilleBelfryLuna, .obj_id = "o00_0500_0010", .boss = RuinSentinels),
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
	[AncientDragonBack]            = NEW_WARP_PT(.map = DragonAerieDragonShrine, .obj_id = "o00_0502_0000", .boss = AncientDragon),

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
	[VendrickBack]                         = NEW_WARP_PT(.map = UndeadCrypt, .obj_id = "o00_0505_0000", .boss = Vendrick),
	
	// Dark chasm of old
	[DarkChasmFromBlackGulchExitFront]      = NEW_WARP_PT(.map = DarkChasmofOld, .obj_id = "o00_0501_0000"),
	[DarkChasmFromDrangleicCastleExitFront] = NEW_WARP_PT(.map = DarkChasmofOld, .obj_id = "o00_0501_0001"),
	[DarkChasmFromShadedWoodsExitFront]     = NEW_WARP_PT(.map = DarkChasmofOld, .obj_id = "o00_0501_0002"),
	[DarkLurkerExitFront]                   = NEW_WARP_PT(.map = DarkChasmofOld, .obj_id = "o00_0501_0003", .reverse = true, .offset = (V3f){0.0f, 13.792f - 11.966f, 0.0f}, .boss = Darklurker),

	[DarkChasmFromBlackGulchExitBack]       = NEW_WARP_PT(.map = DarkChasmofOld, .obj_id = "o00_0501_0000"),
	[DarkChasmFromDrangleicCastleExitBack]  = NEW_WARP_PT(.map = DarkChasmofOld, .obj_id = "o00_0501_0001"),
	[DarkChasmFromShadedWoodsExitBack]      = NEW_WARP_PT(.map = DarkChasmofOld, .obj_id = "o00_0501_0002"),
	[DarkLurkerExitBack]                    = NEW_WARP_PT(.map = DarkChasmofOld, .obj_id = "o00_0501_0003", .reverse = true, .offset = (V3f){0.0f, 13.792f - 11.966f, 0.0f}),

	// Shulva sanctum city
	[SihnTheSlumberingDragonEntryFront] = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0500_0001", .offset = (V3f){0.0f, 79.928f - 79.436f, 0.0f}),
	[ElanaTheSqualidQueenEntryFront]    = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0000", .offset = (V3f){0.0f, 71.928f - 71.637f, 0.0f}),
	[GankSquadBossEntryFront]           = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0002"),
	[GankSquadBossExitFront]            = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0003", .boss = Gankfight),
	[ShulvaToGankFight1Front]           = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0005", .pvp = true),
	[ShulvaEntranceFront]               = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0007", .pvp = true),
	[BetweenElanaAndSihnFront]          = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0010", .offset = (V3f){0.0f, 5.0f * (74.744f - 73.291f) / 12.0f, 0.0f}),
	[ShulvaToGankFight2Front]           = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0011"),
	[NearJesterThomasFront]             = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0012", .offset = (V3f){0.0f, 41.540f - 41.289f, 0.0f}),

	[SihnTheSlumberingDragonEntryBack]  = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0500_0001", .offset = (V3f){0.0f, 79.928f - 79.436f, 0.0f}, .boss = SinhTheSlumberingDragon),
	[ElanaTheSqualidQueenEntryBack]     = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0000", .offset = (V3f){0.0f, 71.928f - 71.637f, 0.0f}, .boss = ElanaTheSqualidQueen),
	[GankSquadBossEntryBack]            = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0002", .boss = Gankfight),
	[GankSquadBossExitBack]             = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0003"),
	[ShulvaToGankFight1Back]            = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0005", .pvp = true),
	[ShulvaEntranceBack]                = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0007", .pvp = true),
	[BetweenElanaAndSihnBack]           = NEW_WARP_PT(.map = ShulvaSanctumCity, .obj_id = "o00_0501_0010", .offset = (V3f){0.0f, 5.0f * (74.744f - 73.291f) / 12.0f, 0.0f}),
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


typedef enum {
	Conn_None                 = 0,
	Conn_OneWay               = 1 << 0,
	Conn_FragrantBranchOfYore = 1 << 1,
	Conn_RotundaLockstone     = 1 << 2,
	Conn_SoldiersKey          = 1 << 3,
	Conn_KingsRing            = 1 << 4,
	Conn_FirstFourSouls       = 1 << 5,
	Conn_Catring              = 1 << 6,
	Conn_AshenMistHeart       = 1 << 7,
	Conn_KingMemory           = 1 << 8,
	Conn_OldKeyInBastille     = 1 << 9,
	Conn_BastilleKey          = 1 << 10,
	Conn_PharrosLockstone     = 1 << 11,
	Conn_ScorchingIronSceptor = 1 << 12,
	Conn_ActivateBrumeTower   = 1 << 13,
	Conn_DarkCovenentJoined   = 1 << 14,
	Conn_ForgottenKey         = 1 << 15,
	Conn_DLC1Key              = 1 << 16,
	Conn_DLC2Key              = 1 << 17,
	Conn_DLC3Key              = 1 << 18,
	Conn_ShulvaSanctumKey     = 1 << 19,
	Conn_ActivateDragonStone  = 1 << 20,
	Conn_DLC3Eye              = 1 << 21,
	Conn_DLC3Unfreezed        = 1 << 22,
	Conn_DropDownPath         = 1 << 23,
	Conn_FrigidOutskirtsKey   = 1 << 24,
	Conn_ShipBellRang         = 1 << 25,
	Conn_IronKeyToFireLizards = 1 << 26,
	Conn_DragonStone          = 1 << 27,
	Conn_BossKilled           = 1 << 28,
	Conn_LudAndZallenDead     = 1 << 29,
	Conn_Firebomb             = 1 << 30,
} ConnectionType;


typedef struct {
	WarpNode to;
	ConnectionType type;
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
	if (connections.items[from].count == 0) da_init(&connections.items[from], n_elem);\
} while (0)

#define connection_add(from, ...) da_append(&connections.items[(from)], ((Connection){.is_warp = false, .type = Conn_None, __VA_ARGS__}));

void generate_connections() {
	da_init(&connections, __WarpNodeCount);
	memset(connections.items, 0, sizeof(*connections.items)*connections.capacity);
	size_t warp_node_count = 0;

	connection_init(GameStartSpawnSrc, 1);
	connection_add(GameStartSpawnSrc, .to = GameStartSpawnDst);
	warp_node_count++;


	// things betwixt

	connection_init(GameStartSpawnDst, 8);
	connection_add(GameStartSpawnDst, .to = Tutorial1EntryFront);
	connection_add(GameStartSpawnDst, .to = Tutorial2EntryFront);
	connection_add(GameStartSpawnDst, .to = Tutorial3EntryFront, .type = Conn_FragrantBranchOfYore);
	connection_add(GameStartSpawnDst, .to = MajulaToForestOfFallenGiantsFront);
	connection_add(GameStartSpawnDst, .to = MajulaToGraveOfSaintsFront, .type = Conn_Catring);
	connection_add(GameStartSpawnDst, .to = MajulaToRotundaLockstoneFront);
	connection_add(GameStartSpawnDst, .to = MajulaToGutterFront, .type = Conn_Catring);
	connection_add(GameStartSpawnDst, .to = MajulaToShadedWoodsFront);
	warp_node_count++;

	connection_init(Tutorial1EntryFront, 7);
	connection_add(Tutorial1EntryFront, .to = Tutorial2EntryFront);
	connection_add(Tutorial1EntryFront, .to = Tutorial3EntryFront, .type = Conn_FragrantBranchOfYore);
	connection_add(Tutorial1EntryFront, .to = MajulaToForestOfFallenGiantsFront);
	connection_add(Tutorial1EntryFront, .to = MajulaToGraveOfSaintsFront, .type = Conn_Catring);
	connection_add(Tutorial1EntryFront, .to = MajulaToRotundaLockstoneFront);
	connection_add(Tutorial1EntryFront, .to = MajulaToGutterFront, .type = Conn_Catring);
	connection_add(Tutorial1EntryFront, .to = MajulaToShadedWoodsFront);
	warp_node_count++;

	connection_init(Tutorial2EntryFront, 7);
	connection_add(Tutorial2EntryFront, .to = Tutorial1EntryFront);
	connection_add(Tutorial2EntryFront, .to = Tutorial3EntryFront, .type = Conn_FragrantBranchOfYore);
	connection_add(Tutorial2EntryFront, .to = MajulaToForestOfFallenGiantsFront);
	connection_add(Tutorial2EntryFront, .to = MajulaToGraveOfSaintsFront, .type = Conn_Catring);
	connection_add(Tutorial2EntryFront, .to = MajulaToRotundaLockstoneFront);
	connection_add(Tutorial2EntryFront, .to = MajulaToGutterFront, .type = Conn_Catring);
	connection_add(Tutorial2EntryFront, .to = MajulaToShadedWoodsFront);
	warp_node_count++;

	connection_init(Tutorial3EntryFront, 7);
	connection_add(Tutorial3EntryFront, .to = Tutorial1EntryFront);
	connection_add(Tutorial3EntryFront, .to = Tutorial2EntryFront, .type = Conn_FragrantBranchOfYore);
	connection_add(Tutorial3EntryFront, .to = MajulaToForestOfFallenGiantsFront, .type = Conn_FragrantBranchOfYore);
	connection_add(Tutorial3EntryFront, .to = MajulaToGraveOfSaintsFront, .type = Conn_Catring | Conn_FragrantBranchOfYore);
	connection_add(Tutorial3EntryFront, .to = MajulaToRotundaLockstoneFront, .type = Conn_FragrantBranchOfYore);
	connection_add(Tutorial3EntryFront, .to = MajulaToGutterFront, .type = Conn_Catring | Conn_FragrantBranchOfYore);
	connection_add(Tutorial3EntryFront, .to = MajulaToShadedWoodsFront, .type = Conn_FragrantBranchOfYore);
	warp_node_count++;

	connection_init(MajulaToForestOfFallenGiantsFront, 7);
	connection_add(MajulaToForestOfFallenGiantsFront, .to = Tutorial1EntryFront);
	connection_add(MajulaToForestOfFallenGiantsFront, .to = Tutorial2EntryFront);
	connection_add(MajulaToForestOfFallenGiantsFront, .to = Tutorial3EntryFront, .type = Conn_FragrantBranchOfYore);
	connection_add(MajulaToForestOfFallenGiantsFront, .to = MajulaToGraveOfSaintsFront, .type = Conn_Catring);
	connection_add(MajulaToForestOfFallenGiantsFront, .to = MajulaToRotundaLockstoneFront);
	connection_add(MajulaToForestOfFallenGiantsFront, .to = MajulaToGutterFront, .type = Conn_Catring);
	connection_add(MajulaToForestOfFallenGiantsFront, .to = MajulaToShadedWoodsFront);
	warp_node_count++;

	connection_init(MajulaToGraveOfSaintsFront, 1);
	connection_add(MajulaToGraveOfSaintsFront, .to = MajulaToGutterFront, .type = Conn_Catring);
	warp_node_count++;

	connection_init(MajulaToRotundaLockstoneFront, 7);
	connection_add(MajulaToRotundaLockstoneFront, .to = Tutorial1EntryFront);
	connection_add(MajulaToRotundaLockstoneFront, .to = Tutorial2EntryFront);
	connection_add(MajulaToRotundaLockstoneFront, .to = Tutorial3EntryFront, .type = Conn_FragrantBranchOfYore);
	connection_add(MajulaToRotundaLockstoneFront, .to = MajulaToGraveOfSaintsFront, .type = Conn_Catring);
	connection_add(MajulaToRotundaLockstoneFront, .to = MajulaToForestOfFallenGiantsFront);
	connection_add(MajulaToRotundaLockstoneFront, .to = MajulaToGutterFront, .type = Conn_Catring);
	connection_add(MajulaToRotundaLockstoneFront, .to = MajulaToShadedWoodsFront);
	warp_node_count++;

	connection_init(MajulaToGutterFront, 0);
	warp_node_count++;

	connection_init(MajulaToShadedWoodsFront, 7);
	connection_add(MajulaToShadedWoodsFront, .to = Tutorial1EntryFront);
	connection_add(MajulaToShadedWoodsFront, .to = Tutorial2EntryFront);
	connection_add(MajulaToShadedWoodsFront, .to = Tutorial3EntryFront, .type = Conn_FragrantBranchOfYore);
	connection_add(MajulaToShadedWoodsFront, .to = MajulaToGraveOfSaintsFront, .type = Conn_Catring);
	connection_add(MajulaToShadedWoodsFront, .to = MajulaToForestOfFallenGiantsFront);
	connection_add(MajulaToShadedWoodsFront, .to = MajulaToGutterFront, .type = Conn_Catring);
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
	connection_add(Tutorial1ExitBack, .to = Tutorial3EntryFront, .type = Conn_FragrantBranchOfYore);
	connection_add(Tutorial1ExitBack, .to = MajulaToForestOfFallenGiantsFront);
	connection_add(Tutorial1ExitBack, .to = MajulaToGraveOfSaintsFront, .type = Conn_Catring);
	connection_add(Tutorial1ExitBack, .to = MajulaToRotundaLockstoneFront);
	connection_add(Tutorial1ExitBack, .to = MajulaToGutterFront, .type = Conn_Catring);
	connection_add(Tutorial1ExitBack, .to = MajulaToShadedWoodsFront);
	warp_node_count++;

	connection_init(Tutorial2ExitBack, 8);
	connection_add(Tutorial2ExitBack, .to = Tutorial1EntryFront);
	connection_add(Tutorial2ExitBack, .to = Tutorial2EntryFront);
	connection_add(Tutorial2ExitBack, .to = Tutorial3EntryFront, .type = Conn_FragrantBranchOfYore);
	connection_add(Tutorial2ExitBack, .to = MajulaToForestOfFallenGiantsFront);
	connection_add(Tutorial2ExitBack, .to = MajulaToGraveOfSaintsFront, .type = Conn_Catring);
	connection_add(Tutorial2ExitBack, .to = MajulaToRotundaLockstoneFront);
	connection_add(Tutorial2ExitBack, .to = MajulaToGutterFront, .type = Conn_Catring);
	connection_add(Tutorial2ExitBack, .to = MajulaToShadedWoodsFront);
	warp_node_count++;

	connection_init(Tutorial3ExitBack, 8);
	connection_add(Tutorial3ExitBack, .to = Tutorial1EntryFront);
	connection_add(Tutorial3ExitBack, .to = Tutorial2EntryFront);
	connection_add(Tutorial3ExitBack, .to = Tutorial3EntryFront, .type = Conn_FragrantBranchOfYore);
	connection_add(Tutorial3ExitBack, .to = MajulaToForestOfFallenGiantsFront);
	connection_add(Tutorial3ExitBack, .to = MajulaToGraveOfSaintsFront, .type = Conn_Catring);
	connection_add(Tutorial3ExitBack, .to = MajulaToRotundaLockstoneFront);
	connection_add(Tutorial3ExitBack, .to = MajulaToGutterFront, .type = Conn_Catring);
	connection_add(Tutorial3ExitBack, .to = MajulaToShadedWoodsFront);
	warp_node_count++;

#define connection_two_way(src, dst) do {\
	connection_init((src), 1);\
	connection_add((src), .to = (dst));\
	connection_init((dst), 1);\
	connection_add((dst), .to = (src));\
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
	connection_add(ForestOfFallenGiantsAfterFirstBonfireBack, .to = GiantLordMemoryEntrySrc, .type = Conn_KingsRing | Conn_AshenMistHeart | Conn_SoldiersKey);
	connection_add(ForestOfFallenGiantsAfterFirstBonfireBack, .to = LastGiantEntryFront, .type = Conn_Catring | Conn_IronKeyToFireLizards);
	connection_add(ForestOfFallenGiantsAfterFirstBonfireBack, .to = ForestOfFallenGiantsBalconyBack, .type = Conn_Catring | Conn_IronKeyToFireLizards);
	warp_node_count++;

	connection_init(GiantLordMemoryEntrySrc, 1);
	connection_add(GiantLordMemoryEntrySrc, .to = GiantLordMemoryEntryDst, .is_warp = true);
	warp_node_count++;

	connection_init(GiantLordMemoryEntryDst, 1);
	connection_add(GiantLordMemoryEntryDst, .to = GiantLordEntryFront);
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
	connection_add(GiantLordMemoryExitDst, .to = GiantLordMemoryEntrySrc, .type = Conn_AshenMistHeart);
	connection_add(GiantLordMemoryExitDst, .to = LastGiantEntryFront, .type = Conn_Catring | Conn_IronKeyToFireLizards);
	connection_add(GiantLordMemoryExitDst, .to = ForestOfFallenGiantsBalconyBack, .type = Conn_Catring | Conn_IronKeyToFireLizards);
	connection_add(GiantLordMemoryExitDst, .to = ForestOfFallenGiantsAfterFirstBonfireBack);
	warp_node_count++;

	connection_init(ForestOfFallenGiantsToCaleFront, 4);
	connection_add(ForestOfFallenGiantsToCaleFront, .to = GiantLordMemoryEntrySrc, .type = Conn_SoldiersKey | Conn_KingsRing | Conn_AshenMistHeart);
	connection_add(ForestOfFallenGiantsToCaleFront, .to = LastGiantEntryFront, .type = Conn_Catring | Conn_IronKeyToFireLizards);
	connection_add(ForestOfFallenGiantsToCaleFront, .to = ForestOfFallenGiantsBalconyBack, .type = Conn_Catring | Conn_IronKeyToFireLizards);
	connection_add(ForestOfFallenGiantsToCaleFront, .to = ForestOfFallenGiantsAfterFirstBonfireBack, .type = Conn_SoldiersKey | Conn_KingsRing);
	warp_node_count++;

	connection_init(ForestOfFallenGiantsBalconyBack, 4);
	connection_add(ForestOfFallenGiantsBalconyBack, .to = LastGiantEntryFront);
	connection_add(ForestOfFallenGiantsBalconyBack, .to = ForestOfFallenGiantsToCaleFront);
	connection_add(ForestOfFallenGiantsBalconyBack, .to = GiantLordMemoryEntrySrc, .type = Conn_KingsRing | Conn_AshenMistHeart | Conn_SoldiersKey);
	connection_add(ForestOfFallenGiantsBalconyBack, .to = ForestOfFallenGiantsAfterFirstBonfireBack, .type = Conn_SoldiersKey | Conn_KingsRing);
	warp_node_count++;

	connection_init(LastGiantEntryFront, 4);
	connection_add(LastGiantEntryFront, .to = ForestOfFallenGiantsBalconyBack);
	connection_add(LastGiantEntryFront, .to = ForestOfFallenGiantsToCaleFront);
	connection_add(LastGiantEntryFront, .to = GiantLordMemoryEntrySrc, .type = Conn_KingsRing | Conn_AshenMistHeart | Conn_SoldiersKey);
	connection_add(LastGiantEntryFront, .to = ForestOfFallenGiantsAfterFirstBonfireBack, .type = Conn_SoldiersKey | Conn_KingsRing);
	warp_node_count++;

	connection_init(LastGiantEntryBack, 0);
	warp_node_count++;

	connection_init(ForestOfFallenGiantsToCaleBack, 8);
	connection_add(ForestOfFallenGiantsToCaleBack, .to = ForestOfFallenGiantsBalconyFront);
	connection_add(ForestOfFallenGiantsToCaleBack, .to = ForestOfFallenGiantsToPursuerArenaFront, .type = Conn_SoldiersKey);
	connection_add(ForestOfFallenGiantsToCaleBack, .to = NearPateGiantMemoryEntrySrc, .type = Conn_AshenMistHeart);
	connection_add(ForestOfFallenGiantsToCaleBack, .to = ForestOfFallenGiantsToCaleFront, .type = Conn_Firebomb);
	connection_add(ForestOfFallenGiantsToCaleBack, .to = GiantLordMemoryEntrySrc, .type = Conn_SoldiersKey | Conn_KingsRing | Conn_AshenMistHeart | Conn_Firebomb);
	connection_add(ForestOfFallenGiantsToCaleBack, .to = LastGiantEntryFront, .type = Conn_Catring | Conn_IronKeyToFireLizards | Conn_Firebomb);
	connection_add(ForestOfFallenGiantsToCaleBack, .to = ForestOfFallenGiantsBalconyBack, .type = Conn_Catring | Conn_IronKeyToFireLizards | Conn_Firebomb);
	connection_add(ForestOfFallenGiantsToCaleBack, .to = ForestOfFallenGiantsAfterFirstBonfireBack, .type = Conn_Firebomb | Conn_SoldiersKey | Conn_KingsRing);
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
	connection_add(NearPateGiantMemoryExitDst, .to = ForestOfFallenGiantsToPursuerArenaFront, .type = Conn_SoldiersKey);
	connection_add(NearPateGiantMemoryExitDst, .to = NearPateGiantMemoryEntrySrc, .type = Conn_AshenMistHeart);
	connection_add(NearPateGiantMemoryExitDst, .to = ForestOfFallenGiantsToCaleFront, .type = Conn_Firebomb);
	connection_add(NearPateGiantMemoryExitDst, .to = GiantLordMemoryEntrySrc, .type = Conn_SoldiersKey | Conn_KingsRing | Conn_AshenMistHeart | Conn_Firebomb);
	connection_add(NearPateGiantMemoryExitDst, .to = LastGiantEntryFront, .type = Conn_Catring | Conn_IronKeyToFireLizards | Conn_Firebomb);
	connection_add(NearPateGiantMemoryExitDst, .to = ForestOfFallenGiantsBalconyBack, .type = Conn_Catring | Conn_IronKeyToFireLizards | Conn_Firebomb);
	connection_add(NearPateGiantMemoryExitDst, .to = ForestOfFallenGiantsAfterFirstBonfireBack, .type = Conn_Firebomb | Conn_SoldiersKey | Conn_KingsRing);
	warp_node_count++;

	connection_init(ForestOfFallenGiantsToPursuerArenaFront, 0);
	warp_node_count++;

	connection_init(PursuerExitBack, 10);
	connection_add(PursuerExitBack, .to = ForestOfFallenGiantsToCaleBack);
	connection_add(PursuerExitBack, .to = ForestOfFallenGiantsBalconyFront);
	connection_add(PursuerExitBack, .to = ForestOfFallenGiantsToPursuerArenaFront, .type = Conn_SoldiersKey);
	connection_add(PursuerExitBack, .to = NearPateGiantMemoryEntrySrc, .type = Conn_AshenMistHeart);
	connection_add(PursuerExitBack, .to = ForestOfFallenGiantsToCaleFront, .type = Conn_Firebomb);
	connection_add(PursuerExitBack, .to = GiantLordMemoryEntrySrc, .type = Conn_SoldiersKey | Conn_KingsRing | Conn_AshenMistHeart | Conn_Firebomb);
	connection_add(PursuerExitBack, .to = LastGiantEntryFront, .type = Conn_Catring | Conn_IronKeyToFireLizards | Conn_Firebomb);
	connection_add(PursuerExitBack, .to = ForestOfFallenGiantsBalconyBack, .type = Conn_Catring | Conn_IronKeyToFireLizards | Conn_Firebomb);
	connection_add(PursuerExitBack, .to = ForestOfFallenGiantsAfterFirstBonfireBack, .type = Conn_Firebomb | Conn_SoldiersKey | Conn_KingsRing);
	connection_add(PursuerExitBack, .to = NearPursuerBirdEntry);
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

	connection_init(ForestOfFallenGiantsBalconyFront, 8);
	connection_add(ForestOfFallenGiantsBalconyFront, .to = ForestOfFallenGiantsToCaleBack);
	connection_add(ForestOfFallenGiantsBalconyFront, .to = ForestOfFallenGiantsToPursuerArenaFront, .type = Conn_SoldiersKey);
	connection_add(ForestOfFallenGiantsBalconyFront, .to = NearPateGiantMemoryEntrySrc, .type = Conn_AshenMistHeart);
	connection_add(ForestOfFallenGiantsBalconyFront, .to = ForestOfFallenGiantsToCaleFront, .type = Conn_Firebomb);
	connection_add(ForestOfFallenGiantsBalconyFront, .to = GiantLordMemoryEntrySrc, .type = Conn_SoldiersKey | Conn_KingsRing | Conn_AshenMistHeart | Conn_Firebomb);
	connection_add(ForestOfFallenGiantsBalconyFront, .to = LastGiantEntryFront, .type = Conn_Catring | Conn_IronKeyToFireLizards | Conn_Firebomb);
	connection_add(ForestOfFallenGiantsBalconyFront, .to = ForestOfFallenGiantsBalconyBack, .type = Conn_Catring | Conn_IronKeyToFireLizards | Conn_Firebomb);
	connection_add(ForestOfFallenGiantsBalconyFront, .to = ForestOfFallenGiantsAfterFirstBonfireBack, .type = Conn_Firebomb | Conn_SoldiersKey | Conn_KingsRing);
	warp_node_count++;

	connection_two_way(ForestOfFallenGiantsToPursuerArenaBack, PursuerEntryFront);
	warp_node_count++;
	warp_node_count++;

	connection_two_way(PursuerEntryBack, PursuerExitFront);
	warp_node_count++;
	warp_node_count++;


	// lost bastille

	printf("Completed %zu/%d\n", warp_node_count, __WarpNodeCount);


};

void free_connections() {
	da_foreach(ConnectionGroup, cg, &connections) {
		da_free(*cg);
	}
	da_free(connections);
}
