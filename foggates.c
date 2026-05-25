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

#define WarpNodeItems \
	X(GameStartSpawnSrc) \
	X(GameStartSpawnDst) \
	X(NearPursuerBirdEntry) \
	X(NearPursuerGiantMemoryEntrySrc) \
	X(NearPateGiantMemoryEntrySrc) \
	X(GiantLordMemoryEntrySrc) \
	X(NearPursuerGiantMemoryExitDst) \
	X(NearPateGiantMemoryExitDst) \
	X(GiantLordMemoryExitDst) \
	X(NearPursuerBirdExit) \
	X(NearPursuerGiantMemoryEntryDst) \
	X(NearPateGiantMemoryEntryDst) \
	X(GiantLordMemoryEntryDst) \
	X(NearPursuerGiantMemoryExitSrc) \
	X(NearPateGiantMemoryExitSrc) \
	X(GiantLordMemoryExitSrc) \
	X(PirateShipWharf) \
	X(PirateShipBastille) \
	X(DLC1EntranceBaseGame) \
	X(DLC2EntranceBaseGame) \
	X(DLC3EntranceBaseGame) \
	X(DLC1EntranceDLC) \
	X(DLC2EntranceDLC) \
	X(DLC3EntranceDLC) \
	X(SirAlonneArmorDLCEntrySrc) \
	X(SirAlonneArmorDLCEntryDst) \
	X(SirAlonneMemoryExitSrc) \
	X(SirAlonneMemoryExitDst) \
	X(ChasmPortalFromBlackGulchSrc) \
	X(ChasmPortalFromShadedWoodsSrc) \
	X(ChasmPortalFromCastleSrc) \
	X(ChasmPortalFromBlackGulchDst) \
	X(ChasmPortalFromShadedWoodsDst) \
	X(ChasmPortalFromCastleDst) \
	X(ChasmGulchExitWarpSrc) \
	X(ChasmShadedWoodsExitWarpSrc) \
	X(ChasmCastleExitWarpSrc) \
	X(ChasmDarkLurkerExitWarpSrc) \
	X(ChasmGulchExitWarpDst) \
	X(ChasmShadedWoodsExitWarpDst) \
	X(ChasmCastleExitWarpDst) \
	X(ChasmDarkLurkerExitWarpDst) \
	X(IvoryKingFightEndSrc) \
	X(IvoryKingFightEndDst) \
	X(CoffinWarpSrc) \
	X(CoffinWarpDst) \
	X(LudAndZallenExitWarpSrc) \
	X(LudAndZallenExitWarpDst) \
	X(DragonMemoriesCoveSrc) \
	X(DragonMemoriesCoveDst) \
	X(DragonMemoriesMemoryDst) \
	X(DragonMemoriesMemorySrc) \
	X(MemoryOfTheKingCryptSrc) \
	X(MemoryOfTheKingCryptDst) \
	X(MemoryOfTheKingMemoryDst) \
	X(MemoryOfTheKingMemorySrc) \
	X(Tutorial1EntryFront) \
	X(Tutorial1ExitFront) \
	X(Tutorial2EntryFront) \
	X(Tutorial2ExitFront) \
	X(Tutorial3EntryFront) \
	X(Tutorial3ExitFront) \
	X(Tutorial1EntryBack) \
	X(Tutorial1ExitBack) \
	X(Tutorial2EntryBack) \
	X(Tutorial2ExitBack) \
	X(Tutorial3EntryBack) \
	X(Tutorial3ExitBack) \
	X(MajulaToForestOfFallenGiantsFront) \
	X(MajulaToGraveOfSaintsFront) \
	X(MajulaToHuntsmanCopseFront) \
	X(MajulaToRotundaLockstoneFront) \
	X(MajulaToGutterFront) \
	X(MajulaToShadedWoodsFront) \
	X(MajulaToForestOfFallenGiantsBack) \
	X(MajulaToGraveOfSaintsBack) \
	X(MajulaToHuntsmanCopseBack) \
	X(MajulaToRotundaLockstoneBack) \
	X(MajulaToGutterBack) \
	X(MajulaToShadedWoodsBack) \
	X(LastGiantEntryFront) \
	X(PursuerEntryFront) \
	X(PursuerExitFront) \
	X(ForestOfFallenGiantsBalconyFront) \
	X(ForestOfFallenGiantsToCaleFront) \
	X(ForestOfFallenGiantsAfterFirstBonfireFront) \
	X(ForestOfFallenGiantsFromMajulaFront) \
	X(ForestOfFallenGiantsToPursuerArenaFront) \
	X(LastGiantEntryBack) \
	X(PursuerEntryBack) \
	X(PursuerExitBack) \
	X(ForestOfFallenGiantsBalconyBack) \
	X(ForestOfFallenGiantsToCaleBack) \
	X(ForestOfFallenGiantsAfterFirstBonfireBack) \
	X(ForestOfFallenGiantsFromMajulaBack) \
	X(ForestOfFallenGiantsToPursuerArenaBack) \
	X(CongregationEntryFront) \
	X(CongregationExitFront) \
	X(BrightstoneCoveToDoorsOfPharrosFront) \
	X(DukesDearFreyjaEntryFront) \
	X(DukesDearFreyjaExitFront) \
	X(CongregationEntryBack) \
	X(CongregationExitBack) \
	X(BrightstoneCoveToDoorsOfPharrosBack) \
	X(DukesDearFreyjaEntryBack) \
	X(DukesDearFreyjaExitBack) \
	X(GuardianDragonEntryFront) \
	X(GuardianDragonExitFront) \
	X(AldiasKeepEntranceFront) \
	X(GuardianDragonEntryBack) \
	X(GuardianDragonExitBack) \
	X(AldiasKeepEntranceBack) \
	X(GargoylesEntryFront) \
	X(GargoylesExitFront) \
	X(RuinSentinelsEntryFront) \
	X(RuinSentinelsExitFront) \
	X(RuinSentinelsHiddenDoor1ExitFront) \
	X(RuinSentinelsHiddenDoor2ExitFront) \
	X(RuinSentinelsHiddenDoor3ExitFront) \
	X(RuinSentinelsHiddenDoor4ExitFront) \
	X(RuinSentinesUpperExitFront) \
	X(LostBastilleToSinnersRiseFront) \
	X(LostBastilleToShipFromWharfFront) \
	X(LostBastilleToBelfryLunaFront) \
	X(LostSinnerEntryFront) \
	X(LostSinnerExitFront) \
	X(GargoylesEntryBack) \
	X(GargoylesExitBack) \
	X(RuinSentinelsEntryBack) \
	X(RuinSentinelsExitBack) \
	X(RuinSentinelsHiddenDoor1ExitBack) \
	X(RuinSentinelsHiddenDoor2ExitBack) \
	X(RuinSentinelsHiddenDoor3ExitBack) \
	X(RuinSentinelsHiddenDoor4ExitBack) \
	X(RuinSentinesUpperExitBack) \
	X(LostBastilleToSinnersRiseBack) \
	X(LostBastilleToShipFromWharfBack) \
	X(LostBastilleToBelfryLunaBack) \
	X(LostSinnerEntryBack) \
	X(LostSinnerExitBack) \
	X(CovetousDemonEntryFront) \
	X(EarthenPeakNearWindmillBurnLocationFront) \
	X(MythaEntryFront) \
	X(CovetousDemonExitFront) \
	X(MythaExitFront) \
	X(HarvestValleyToSkeletonLordsFront) \
	X(CovetousDemonEntryBack) \
	X(EarthenPeakNearWindmillBurnLocationBack) \
	X(MythaEntryBack) \
	X(CovetousDemonExitBack) \
	X(MythaExitBack) \
	X(HarvestValleyToSkeletonLordsBack) \
	X(FlexileSentryExitFront) \
	X(FlexileSentryEntryFront) \
	X(NoMansWharfToHeidesFront) \
	X(FlexileSentryExitBack) \
	X(FlexileSentryEntryBack) \
	X(NoMansWharfToHeidesBack) \
	X(SmeltorDemonEntryFront) \
	X(IronKeepNearFlameThrowerAndTurtleFront) \
	X(BelfrySolExitFront) \
	X(BelfrySolEntryFront) \
	X(OldIronKingEntryFront) \
	X(OldIronKingExitFront) \
	X(SmeltorDemonExitFront) \
	X(SmeltorDemonToBonfireFront) \
	X(IronKeepToBelfryAtPharrosLockstoneFront) \
	X(IronKeepToEarthenPeakFront) \
	X(SmeltorDemonEntryBack) \
	X(IronKeepNearFlameThrowerAndTurtleBack) \
	X(BelfrySolExitBack) \
	X(BelfrySolEntryBack) \
	X(OldIronKingEntryBack) \
	X(OldIronKingExitBack) \
	X(SmeltorDemonExitBack) \
	X(SmeltorDemonToBonfireBack) \
	X(IronKeepToBelfryAtPharrosLockstoneBack) \
	X(IronKeepToEarthenPeakBack) \
	X(SkeletonLordsEntryFront) \
	X(ChariotEntryFront) \
	X(SkeletonLordsExitFront) \
	X(ChariotExitFront) \
	X(HuntsmanCopseToMajulaFront) \
	X(SkeletonLordsEntryBack) \
	X(ChariotEntryBack) \
	X(SkeletonLordsExitBack) \
	X(ChariotExitBack) \
	X(HuntsmanCopseToMajulaBack) \
	X(GutterNearAntQueenFront) \
	X(TheRottenExitFront) \
	X(BlackGulchEntranceFront) \
	X(TheGutterFromGraveOfSaintsFront) \
	X(TheRottenEntryFront) \
	X(GutterNearAntQueenBack) \
	X(TheRottenExitBack) \
	X(BlackGulchEntranceBack) \
	X(TheGutterFromGraveOfSaintsBack) \
	X(TheRottenEntryBack) \
	X(DragonAerieToAldiasKeepFront) \
	X(AncientDragonFront) \
	X(DragonAerieToAldiasKeepBack) \
	X(AncientDragonBack) \
	X(ShadedWoodsFromMajulaFront) \
	X(ShadedWoodsFromMajulaBack) \
	X(DragonriderEntryFront) \
	X(DragonriderExitFront) \
	X(HeidesToMajulaFront) \
	X(HeidesToWharfFront) \
	X(OldDragonslayerEntryFront) \
	X(OldDragonslayerExitFront) \
	X(DragonriderEntryBack) \
	X(DragonriderExitBack) \
	X(HeidesToMajulaBack) \
	X(HeidesToWharfBack) \
	X(OldDragonslayerEntryBack) \
	X(OldDragonslayerExitBack) \
	X(ScorpionessNajkaExitFront) \
	X(ShadedWoodsToMistyAreaFront) \
	X(ScorpionessNajkaEntryFront) \
	X(ScorpionessNajkaExitBack) \
	X(ShadedWoodsToMistyAreaBack) \
	X(ScorpionessNajkaEntryBack) \
	X(RoyalRatAuthorityExitFront) \
	X(DoorOfPharrosToOrdealEndBonfireFront) \
	X(DoorOfPharrosToRatKingDomainFront) \
	X(RoyalRatAuthorityEntryFront) \
	X(RoyalRatAuthorityExitBack) \
	X(DoorOfPharrosToOrdealEndBonfireBack) \
	X(DoorOfPharrosToRatKingDomainBack) \
	X(RoyalRatAuthorityEntryBack) \
	X(GraveOfSaintsFromPitFront) \
	X(GraveOfSaintsNearUpperBonfireFront) \
	X(RoyalRatVanguardEntryFront) \
	X(RoyalRatVanguardExitFront) \
	X(GraveOfSaintsFromPitBack) \
	X(GraveOfSaintsNearUpperBonfireBack) \
	X(RoyalRatVanguardEntryBack) \
	X(RoyalRatVanguardExitBack) \
	X(GiantLordEntryFront) \
	X(GiantLordExitFront) \
	X(GiantLordEntryBack) \
	X(GiantLordExitBack) \
	X(DemonOfSongEntryFront) \
	X(ShrineOfAmanaTo2ndBonfireFront) \
	X(ShrineOfAmanaTo3rdBonfireFront) \
	X(DemonOfSongExitFront) \
	X(ShrineOfAmanaToDrangleicCastleFront) \
	X(DemonOfSongEntryBack) \
	X(ShrineOfAmanaTo2ndBonfireBack) \
	X(ShrineOfAmanaTo3rdBonfireBack) \
	X(DemonOfSongExitBack) \
	X(ShrineOfAmanaToDrangleicCastleBack) \
	X(LookingGlassKnightEntryFront) \
	X(TwinDragonridersEntryFront) \
	X(TwinDragonridersExitFront) \
	X(LookingGlassKnightExitFront) \
	X(FinalFightArenaFront) \
	X(DrangleicCastleToChasmPortalFront) \
	X(DrangleicCastleToShadedWoodsFront) \
	X(LookingGlassKnightEntryBack) \
	X(TwinDragonridersEntryBack) \
	X(TwinDragonridersExitBack) \
	X(LookingGlassKnightExitBack) \
	X(FinalFightArenaBack) \
	X(DrangleicCastleToChasmPortalBack) \
	X(DrangleicCastleToShadedWoodsBack) \
	X(UndeadCryptToShrineOfAmanaFront) \
	X(VelsdatEntryFront) \
	X(VelsdatExitFront) \
	X(UndeadCryptFromAgdayneToBonfireFront) \
	X(VendrickFront) \
	X(UndeadCryptToShrineOfAmanaBack) \
	X(VelsdatEntryBack) \
	X(VelsdatExitBack) \
	X(UndeadCryptFromAgdayneToBonfireBack) \
	X(VendrickBack) \
	X(DarkChasmFromBlackGulchExitFront) \
	X(DarkChasmFromDrangleicCastleExitFront) \
	X(DarkChasmFromShadedWoodsExitFront) \
	X(DarkLurkerExitFront) \
	X(DarkChasmFromBlackGulchExitBack) \
	X(DarkChasmFromDrangleicCastleExitBack) \
	X(DarkChasmFromShadedWoodsExitBack) \
	X(DarkLurkerExitBack) \
	X(SihnTheSlumberingDragonEntryFront) \
	X(ElanaTheSqualidQueenEntryFront) \
	X(GankSquadBossEntryFront) \
	X(GankSquadBossExitFront) \
	X(ShulvaToGankFight1Front) \
	X(ShulvaEntranceFront) \
	X(BetweenElanaAndSihnFront) \
	X(ShulvaToGankFight2Front) \
	X(NearJesterThomasFront) \
	X(SihnTheSlumberingDragonEntryBack) \
	X(ElanaTheSqualidQueenEntryBack) \
	X(GankSquadBossEntryBack) \
	X(GankSquadBossExitBack) \
	X(ShulvaToGankFight1Back) \
	X(ShulvaEntranceBack) \
	X(BetweenElanaAndSihnBack) \
	X(ShulvaToGankFight2Back) \
	X(NearJesterThomasBack) \
	X(BrumeTowerFrom2ndBonfireToCentralRoomFront) \
	X(BrumeTowerToFoyerBonfireFromOutsideFront) \
	X(BrumeTowerToScorchingIronSceptorKeyFront) \
	X(SirAlonneEntryFront) \
	X(SirAlonneExitFront) \
	X(FumeKnightEntryFront) \
	X(FumeKnightExitFront) \
	X(BlueSmeltorDemonEntryFront) \
	X(BlueSmeltorDemonExitFront) \
	X(BrumeTowerEntranceFromLiftFront) \
	X(BrumeTowerToBlueSmelterDungeonFront) \
	X(BrumeTowerFrom2ndBonfireToCentralRoomBack) \
	X(BrumeTowerToFoyerBonfireFromOutsideBack) \
	X(BrumeTowerToScorchingIronSceptorKeyBack) \
	X(SirAlonneEntryBack) \
	X(SirAlonneExitBack) \
	X(FumeKnightEntryBack) \
	X(FumeKnightExitBack) \
	X(BlueSmeltorDemonEntryBack) \
	X(BlueSmeltorDemonExitBack) \
	X(BrumeTowerEntranceFromLiftBack) \
	X(BrumeTowerToBlueSmelterDungeonBack) \
	X(AavaTheKingsPetExitFront) \
	X(EleumLoyceAfterGettingEyeToSeeGhostsFront) \
	X(IvoryKingFront) \
	X(LudAndZallenEntryFront) \
	X(AavaTheKingsPetEntryFront) \
	X(LudAndZallenExitFront) \
	X(EleumLoyceCathedraEntranceFront) \
	X(AavaTheKingsPetExitBack) \
	X(EleumLoyceAfterGettingEyeToSeeGhostsBack) \
	X(IvoryKingBack) \
	X(LudAndZallenEntryBack) \
	X(AavaTheKingsPetEntryBack) \
	X(LudAndZallenExitBack) \
	X(EleumLoyceCathedraEntranceBack) \
	X(DarkLurkerSpawn)

typedef enum {
#define X(name) name,
  WarpNodeItems
#undef X
	__WarpNodeCount,
} WarpNode;

const char *WarpNodeNames[] = {
#define X(name) #name,
  WarpNodeItems
#undef X
};

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
	Conn_FrigidOutskirtsKey   = 1 << 23,
	Conn_ShipBellRang         = 1 << 24,
	Conn_IronKeyToFireLizards = 1 << 25,
	Conn_DragonStone          = 1 << 26,
	Conn_BossKilled           = 1 << 27,
	Conn_LudAndZallenDead     = 1 << 28,
	Conn_Firebomb             = 1 << 29,
	Conn_Gilligan_Ladder      = 1 << 30,
	Conn_DLC2TowerKey         = 1 << 31,
	Conn_KeyToKingsPassage    = 1 << 32,
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
	if (connections.items[from].count == 0) {\
		da_init(&connections.items[from], n_elem);\
		connections.count++;\
	}\
} while (0)

#define connection_add(from, ...) da_append(&connections.items[(from)], ((Connection){.is_warp = false, .type = Conn_None, __VA_ARGS__}));

void generate_connections() {
	assert(connections.items == NULL);
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
	connection_add(GameStartSpawnDst, .to = MajulaToGraveOfSaintsFront, .type = Conn_Catring); // TODO: cannot add Conn_Gilligan_Ladder to this type
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

#define connection_two_way_n(src, dst, n, ...) do {\
	connection_init((src), (n));\
	connection_add((src), .to = (dst), __VA_ARGS__);\
	connection_init((dst), (n));\
	connection_add((dst), .to = (src), __VA_ARGS__);\
} while (0)

#define connection_two_way(src, dst, ...) connection_two_way_n(src, dst, 1, __VA_ARGS__)

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

	connection_init(NearPursuerGiantMemoryExitDst, 11);
	connection_add(NearPursuerGiantMemoryExitDst, .to = PursuerExitBack);
	connection_add(NearPursuerGiantMemoryExitDst, .to = ForestOfFallenGiantsToCaleBack);
	connection_add(NearPursuerGiantMemoryExitDst, .to = ForestOfFallenGiantsBalconyFront);
	connection_add(NearPursuerGiantMemoryExitDst, .to = ForestOfFallenGiantsToPursuerArenaFront, .type = Conn_SoldiersKey);
	connection_add(NearPursuerGiantMemoryExitDst, .to = NearPateGiantMemoryEntrySrc, .type = Conn_AshenMistHeart);
	connection_add(NearPursuerGiantMemoryExitDst, .to = ForestOfFallenGiantsToCaleFront, .type = Conn_Firebomb);
	connection_add(NearPursuerGiantMemoryExitDst, .to = GiantLordMemoryEntrySrc, .type = Conn_SoldiersKey | Conn_KingsRing | Conn_AshenMistHeart | Conn_Firebomb);
	connection_add(NearPursuerGiantMemoryExitDst, .to = LastGiantEntryFront, .type = Conn_Catring | Conn_IronKeyToFireLizards | Conn_Firebomb);
	connection_add(NearPursuerGiantMemoryExitDst, .to = ForestOfFallenGiantsBalconyBack, .type = Conn_Catring | Conn_IronKeyToFireLizards | Conn_Firebomb);
	connection_add(NearPursuerGiantMemoryExitDst, .to = ForestOfFallenGiantsAfterFirstBonfireBack, .type = Conn_Firebomb | Conn_SoldiersKey | Conn_KingsRing);
	connection_add(NearPursuerGiantMemoryExitDst, .to = NearPursuerBirdEntry);
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
	connection_init(NearPursuerBirdExit, 1);
	connection_add(NearPursuerBirdExit, .to = RuinSentinelsEntryFront, .type = Conn_FragrantBranchOfYore);
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
	connection_add(RuinSentinesUpperExitBack, .to = RuinSentinelsEntryFront, .type = Conn_FragrantBranchOfYore);
	connection_add(RuinSentinesUpperExitBack, .to = LostBastilleToSinnersRiseFront);
	connection_add(RuinSentinesUpperExitBack, .to = LostBastilleToShipFromWharfBack, .type = Conn_OldKeyInBastille);
	connection_add(RuinSentinesUpperExitBack, .to = LostBastilleToBelfryLunaFront, .type = Conn_PharrosLockstone);
	warp_node_count++;

	connection_init(LostBastilleToSinnersRiseBack, 1);
	connection_add(LostBastilleToSinnersRiseBack, .to = LostSinnerEntryFront);
	warp_node_count++;

	connection_init(LostSinnerEntryFront, 1);
	connection_add(LostSinnerEntryFront, .to = LostBastilleToSinnersRiseBack, .type = Conn_BastilleKey);
	warp_node_count++;

	connection_init(LostSinnerEntryBack, 1);
	connection_add(LostSinnerEntryBack, .to = LostSinnerExitFront);
	warp_node_count++;

	connection_init(LostSinnerExitFront, 0);
	warp_node_count++;

	connection_init(LostSinnerExitBack, 0);
	warp_node_count++;

	connection_init(LostBastilleToShipFromWharfBack, 2);
	connection_add(LostBastilleToShipFromWharfBack, .to = RuinSentinelsEntryFront, .type = Conn_FragrantBranchOfYore);
	connection_add(LostBastilleToShipFromWharfBack, .to = LostBastilleToSinnersRiseFront, .type = Conn_OldKeyInBastille);
	warp_node_count++;

	connection_init(LostBastilleToShipFromWharfFront, 0);
	warp_node_count++;

	connection_init(PirateShipBastille, 1);
	connection_add(PirateShipBastille, .to = LostBastilleToShipFromWharfFront);
	warp_node_count++;

	connection_init(LostBastilleToBelfryLunaFront, 4);
	connection_add(LostBastilleToBelfryLunaFront, .to = RuinSentinesUpperExitBack);
	connection_add(LostBastilleToBelfryLunaFront, .to = RuinSentinelsEntryFront, .type = Conn_FragrantBranchOfYore);
	connection_add(LostBastilleToBelfryLunaFront, .to = LostBastilleToSinnersRiseFront);
	connection_add(LostBastilleToBelfryLunaFront, .to = LostBastilleToShipFromWharfBack, .type = Conn_OldKeyInBastille);
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
	connection_add(MajulaToRotundaLockstoneBack, .to = MajulaToHuntsmanCopseFront, .type = Conn_RotundaLockstone);
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

	connection_two_way_n(HuntsmanCopseToMajulaBack, ChariotEntryFront, 2);
	warp_node_count++;
	warp_node_count++;

	connection_two_way_n(HuntsmanCopseToMajulaBack, SkeletonLordsEntryFront, 2);
	warp_node_count++;
	warp_node_count++;

	connection_two_way_n(ChariotEntryFront, SkeletonLordsEntryFront, 2);
	warp_node_count++;
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
	connection_add(IronKeepToEarthenPeakBack, .to = IronKeepToBelfryAtPharrosLockstoneFront, .type = Conn_PharrosLockstone);
	connection_add(IronKeepToEarthenPeakBack, .to = IronKeepNearFlameThrowerAndTurtleFront);
	connection_add(IronKeepToEarthenPeakBack, .to = IronKeepNearFlameThrowerAndTurtleBack);
	connection_add(IronKeepToEarthenPeakBack, .to = OldIronKingEntryFront);
	warp_node_count++;

	connection_init(SmeltorDemonEntryFront, 4);
	connection_add(SmeltorDemonEntryFront, .to = IronKeepToBelfryAtPharrosLockstoneFront, .type = Conn_PharrosLockstone);
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
	connection_add(IronKeepNearFlameThrowerAndTurtleFront, .to = IronKeepToBelfryAtPharrosLockstoneFront, .type = Conn_PharrosLockstone);
	connection_add(IronKeepNearFlameThrowerAndTurtleFront, .to = IronKeepNearFlameThrowerAndTurtleBack);
	connection_add(IronKeepNearFlameThrowerAndTurtleFront, .to = SmeltorDemonEntryFront);
	connection_add(IronKeepNearFlameThrowerAndTurtleFront, .to = OldIronKingEntryFront);
	warp_node_count++;

	connection_init(IronKeepToBelfryAtPharrosLockstoneBack, 6);
	connection_add(IronKeepToBelfryAtPharrosLockstoneBack, .to = IronKeepToBelfryAtPharrosLockstoneFront, .type = Conn_PharrosLockstone);
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
	connection_add(SmeltorDemonToBonfireBack, .to = IronKeepToBelfryAtPharrosLockstoneFront, .type = Conn_PharrosLockstone);
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
	connection_add(BelfrySolExitBack, .to = IronKeepToBelfryAtPharrosLockstoneFront, .type = Conn_PharrosLockstone);
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
	connection_add(OldIronKingExitBack, DLC2EntranceBaseGame);
	warp_node_count++;

	connection_init(DLC2EntranceBaseGame, 2);
	connection_add(DLC2EntranceBaseGame, DLC2EntranceDLC, .is_warp = true);
	connection_add(DLC2EntranceBaseGame, OldIronKingExitBack);
	warp_node_count++;

	// dlc2
	connection_init(DLC2EntranceDLC, 2);
	connection_add(DLC2EntranceDLC, .to = BrumeTowerEntranceFromLiftFront, .type = Conn_DLC2Key);
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
	connection_add(BrumeTowerFrom2ndBonfireToCentralRoomBack, .to = SirAlonneArmorDLCEntrySrc, .type = Conn_ActivateBrumeTower | Conn_AshenMistHeart);
	connection_add(BrumeTowerFrom2ndBonfireToCentralRoomBack, .to = BrumeTowerToFoyerBonfireFromOutsideFront);
	connection_add(BrumeTowerFrom2ndBonfireToCentralRoomBack, .to = BrumeTowerToFoyerBonfireFromOutsideBack, .type = Conn_ActivateBrumeTower);
	connection_add(BrumeTowerFrom2ndBonfireToCentralRoomBack, .to = BrumeTowerToBlueSmelterDungeonFront, .type = Conn_ActivateBrumeTower | Conn_DLC2TowerKey);
	connection_add(BrumeTowerFrom2ndBonfireToCentralRoomBack, .to = FumeKnightEntryFront, .type = Conn_ActivateBrumeTower);
	connection_add(BrumeTowerFrom2ndBonfireToCentralRoomBack, .to = BrumeTowerToScorchingIronSceptorKeyFront, .type = Conn_ActivateBrumeTower);
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

	connection_init(SirAlonneMemoryExitDst, 6);
	connection_add(SirAlonneMemoryExitDst, .to = SirAlonneArmorDLCEntrySrc, .type = Conn_AshenMistHeart);
	connection_add(SirAlonneMemoryExitDst, .to = BrumeTowerToFoyerBonfireFromOutsideFront, .type = Conn_ActivateBrumeTower);
	connection_add(SirAlonneMemoryExitDst, .to = BrumeTowerToFoyerBonfireFromOutsideBack, .type = Conn_ActivateBrumeTower);
	connection_add(SirAlonneMemoryExitDst, .to = BrumeTowerToBlueSmelterDungeonFront, .type = Conn_ActivateBrumeTower | Conn_DLC2TowerKey);
	connection_add(SirAlonneMemoryExitDst, .to = FumeKnightEntryFront, .type = Conn_ActivateBrumeTower);
	connection_add(SirAlonneMemoryExitDst, .to = BrumeTowerToScorchingIronSceptorKeyFront, .type = Conn_ActivateBrumeTower);
	warp_node_count++;

	connection_init(BrumeTowerToFoyerBonfireFromOutsideFront, 0);
	warp_node_count++;

	connection_init(BrumeTowerToFoyerBonfireFromOutsideBack, 5);
	connection_add(BrumeTowerToFoyerBonfireFromOutsideBack, .to = SirAlonneArmorDLCEntrySrc, .type = Conn_ActivateBrumeTower | Conn_AshenMistHeart);
	connection_add(BrumeTowerToFoyerBonfireFromOutsideBack, .to = BrumeTowerToFoyerBonfireFromOutsideFront, .type = Conn_ActivateBrumeTower);
	connection_add(BrumeTowerToFoyerBonfireFromOutsideBack, .to = BrumeTowerToBlueSmelterDungeonFront, .type = Conn_ActivateBrumeTower | Conn_DLC2TowerKey);
	connection_add(BrumeTowerToFoyerBonfireFromOutsideBack, .to = FumeKnightEntryFront, .type = Conn_ActivateBrumeTower);
	connection_add(BrumeTowerToFoyerBonfireFromOutsideBack, .to = BrumeTowerToScorchingIronSceptorKeyFront);
	warp_node_count++;

	connection_init(BrumeTowerToBlueSmelterDungeonFront, 6);
	connection_add(BrumeTowerToBlueSmelterDungeonFront, .to = SirAlonneArmorDLCEntrySrc, .type = Conn_ActivateBrumeTower | Conn_DLC2TowerKey | Conn_AshenMistHeart);
	connection_add(BrumeTowerToBlueSmelterDungeonFront, .to = BrumeTowerToFoyerBonfireFromOutsideFront, .type = Conn_ActivateBrumeTower | Conn_DLC2TowerKey);
	connection_add(BrumeTowerToBlueSmelterDungeonFront, .to = BrumeTowerToFoyerBonfireFromOutsideBack, .type = Conn_ActivateBrumeTower | Conn_DLC2TowerKey);
	connection_add(BrumeTowerToBlueSmelterDungeonFront, .to = BrumeTowerFrom2ndBonfireToCentralRoomBack, .type = Conn_ActivateBrumeTower | Conn_DLC2TowerKey);
	connection_add(BrumeTowerToBlueSmelterDungeonFront, .to = FumeKnightEntryFront, .type = Conn_ActivateBrumeTower | Conn_DLC2TowerKey);
	connection_add(BrumeTowerToBlueSmelterDungeonFront, .to = BrumeTowerToScorchingIronSceptorKeyFront, .type = Conn_ActivateBrumeTower | Conn_DLC2TowerKey);
	warp_node_count++;

	connection_init(FumeKnightEntryFront, 0);
	warp_node_count++;

	connection_two_way(FumeKnightEntryBack, FumeKnightExitFront);
	warp_node_count++;
	warp_node_count++;
	
	connection_init(FumeKnightExitBack, 0);
	warp_node_count++;

	connection_init(BrumeTowerToScorchingIronSceptorKeyFront, 6);
	connection_add(BrumeTowerToScorchingIronSceptorKeyFront, .to = SirAlonneArmorDLCEntrySrc, .type = Conn_ActivateBrumeTower | Conn_AshenMistHeart);
	connection_add(BrumeTowerToScorchingIronSceptorKeyFront, .to = BrumeTowerToFoyerBonfireFromOutsideFront, .type = Conn_ActivateBrumeTower);
	connection_add(BrumeTowerToScorchingIronSceptorKeyFront, .to = BrumeTowerToFoyerBonfireFromOutsideBack);
	connection_add(BrumeTowerToScorchingIronSceptorKeyFront, .to = BrumeTowerFrom2ndBonfireToCentralRoomBack, .type = Conn_ActivateBrumeTower);
	connection_add(BrumeTowerToScorchingIronSceptorKeyFront, .to = FumeKnightEntryFront, .type = Conn_ActivateBrumeTower);
	connection_add(BrumeTowerToScorchingIronSceptorKeyFront, .to = BrumeTowerToBlueSmelterDungeonFront, .type = Conn_ActivateBrumeTower | Conn_DLC2TowerKey);
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
	connection_add(BlueSmeltorDemonExitBack, .to = SirAlonneArmorDLCEntrySrc, .type = Conn_ActivateBrumeTower | Conn_DLC2TowerKey | Conn_AshenMistHeart);
	connection_add(BlueSmeltorDemonExitBack, .to = BrumeTowerToFoyerBonfireFromOutsideFront, .type = Conn_ActivateBrumeTower | Conn_DLC2TowerKey);
	connection_add(BlueSmeltorDemonExitBack, .to = BrumeTowerToFoyerBonfireFromOutsideBack, .type = Conn_ActivateBrumeTower | Conn_DLC2TowerKey);
	connection_add(BlueSmeltorDemonExitBack, .to = BrumeTowerFrom2ndBonfireToCentralRoomBack, .type = Conn_ActivateBrumeTower | Conn_DLC2TowerKey);
	connection_add(BlueSmeltorDemonExitBack, .to = FumeKnightEntryFront, .type = Conn_ActivateBrumeTower | Conn_DLC2TowerKey);
	connection_add(BlueSmeltorDemonExitBack, .to = BrumeTowerToScorchingIronSceptorKeyFront, .type = Conn_ActivateBrumeTower | Conn_DLC2TowerKey);
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

	connection_init(RoyalRatAuthorityExitBack, 2);
	connection_add(RoyalRatAuthorityExitBack, .to = MajulaToGutterBack);
	connection_add(RoyalRatAuthorityExitBack, .to = TheGutterFromGraveOfSaintsFront);
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
	connection_add(BlackGulchEntranceBack, TheRottenEntryFront);
	connection_add(BlackGulchEntranceBack, ChasmPortalFromBlackGulchSrc, .type = Conn_DarkCovenentJoined);
	warp_node_count++;

	connection_init(TheRottenEntryFront, 2);
	connection_add(TheRottenEntryFront, BlackGulchEntranceBack);
	connection_add(TheRottenEntryFront, ChasmPortalFromBlackGulchSrc, .type = Conn_DarkCovenentJoined);
	warp_node_count++;

	connection_init(ChasmPortalFromBlackGulchSrc, 1);
	connection_add(ChasmPortalFromBlackGulchSrc, ChasmPortalFromBlackGulchDst, .is_warp = true);
	warp_node_count++;

	connection_init(ChasmPortalFromBlackGulchDst, 1);
	connection_add(ChasmPortalFromBlackGulchDst, DarkChasmFromBlackGulchExitFront);
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
	connection_add(ChasmGulchExitWarpDst, .to = ChasmPortalFromBlackGulchSrc, .type = Conn_DarkCovenentJoined);
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
	connection_add(DLC1EntranceDLC, .to = ShulvaEntranceFront, .type = Conn_DLC1Key);
	warp_node_count++;

	connection_init(ShulvaEntranceFront, 1);
	connection_add(ShulvaEntranceFront, .to = DLC1EntranceBaseGame, .type = Conn_DLC1Key);
	warp_node_count++;

	connection_init(ShulvaEntranceBack, 2);
	connection_add(ShulvaEntranceBack, .to = ShulvaToGankFight1Front, .type = Conn_ShulvaSanctumKey);
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
	connection_add(GankSquadBossExitBack, .to = ShulvaToGankFight1Front, .type = Conn_ShulvaSanctumKey);
	connection_add(GankSquadBossExitBack, .to = ShulvaEntranceBack);
	connection_add(GankSquadBossExitBack, .to = NearJesterThomasFront);
	warp_node_count++;

	connection_init(NearJesterThomasBack, 4);
	connection_add(NearJesterThomasBack, .to = ShulvaToGankFight1Front, .type = Conn_ShulvaSanctumKey);
	connection_add(NearJesterThomasBack, .to = ShulvaEntranceBack);
	connection_add(NearJesterThomasBack, .to = NearJesterThomasFront);
	connection_add(NearJesterThomasBack, .to = ElanaTheSqualidQueenEntryFront, .type = Conn_ActivateDragonStone);
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
	connection_add(ShadedWoodsFromMajulaBack, .to = ShadedWoodsToMistyAreaFront, .type = Conn_FragrantBranchOfYore);
	connection_add(ShadedWoodsFromMajulaBack, .to = DLC3EntranceBaseGame, .type = Conn_FragrantBranchOfYore | Conn_FirstFourSouls);
	connection_add(ShadedWoodsFromMajulaBack, .to = DrangleicCastleToShadedWoodsFront, .type = Conn_FragrantBranchOfYore | Conn_FirstFourSouls);
	connection_add(ShadedWoodsFromMajulaBack, .to = AldiasKeepEntranceFront, .type = Conn_FragrantBranchOfYore | Conn_KingsRing);
	warp_node_count++;

	connection_init(ShadedWoodsToMistyAreaFront, 3);
	connection_add(ShadedWoodsToMistyAreaFront, .to = DLC3EntranceBaseGame, .type = Conn_FirstFourSouls);
	connection_add(ShadedWoodsToMistyAreaFront, .to = DrangleicCastleToShadedWoodsFront, .type = Conn_FirstFourSouls);
	connection_add(ShadedWoodsToMistyAreaFront, .to = AldiasKeepEntranceFront, .type = Conn_KingsRing);
	warp_node_count++;

	connection_init(DLC3EntranceBaseGame, 4);
	connection_add(DLC3EntranceBaseGame, .to = ShadedWoodsToMistyAreaFront, .type = Conn_FirstFourSouls);
	connection_add(DLC3EntranceBaseGame, .to = DrangleicCastleToShadedWoodsFront, .type = Conn_FirstFourSouls);
	connection_add(DLC3EntranceBaseGame, .to = AldiasKeepEntranceFront, .type = Conn_KingsRing | Conn_FirstFourSouls);
	connection_add(DLC3EntranceBaseGame, .to = DLC3EntranceDLC, .is_warp = true);
	warp_node_count++;

	connection_init(AldiasKeepEntranceFront, 3);
	connection_add(AldiasKeepEntranceFront, .to = DLC3EntranceBaseGame, .type = Conn_FirstFourSouls | Conn_KingsRing);
	connection_add(AldiasKeepEntranceFront, .to = DrangleicCastleToShadedWoodsFront, .type = Conn_FirstFourSouls | Conn_KingsRing);
	connection_add(AldiasKeepEntranceFront, .to = ShadedWoodsToMistyAreaFront, .type = Conn_KingsRing);
	warp_node_count++;

	connection_init(DrangleicCastleToShadedWoodsFront, 3);
	connection_add(DrangleicCastleToShadedWoodsFront, .to = DLC3EntranceBaseGame, .type = Conn_FirstFourSouls);
	connection_add(DrangleicCastleToShadedWoodsFront, .to = AldiasKeepEntranceFront, .type = Conn_FirstFourSouls | Conn_KingsRing);
	connection_add(DrangleicCastleToShadedWoodsFront, .to = ShadedWoodsToMistyAreaFront, .type = Conn_FirstFourSouls);
	warp_node_count++;

	connection_init(ShadedWoodsToMistyAreaBack, 2);
	connection_add(ShadedWoodsToMistyAreaBack, .to = ScorpionessNajkaEntryFront);
	connection_add(ShadedWoodsToMistyAreaBack, .to = ChasmPortalFromShadedWoodsSrc, .type = Conn_DarkCovenentJoined);
	warp_node_count++;

	connection_init(ScorpionessNajkaEntryFront, 2);
	connection_add(ScorpionessNajkaEntryFront, .to = ShadedWoodsToMistyAreaBack);
	connection_add(ScorpionessNajkaEntryFront, .to = ChasmPortalFromShadedWoodsSrc, .type = Conn_DarkCovenentJoined);
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
	connection_add(ChasmShadedWoodsExitWarpDst, .to = ChasmPortalFromShadedWoodsSrc, .type = Conn_DarkCovenentJoined);
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

	connection_init(RoyalRatVanguardExitBack, 3);
	connection_add(RoyalRatVanguardExitBack, .to = ScorpionessNajkaExitBack);
	connection_add(RoyalRatVanguardExitBack, .to = DoorOfPharrosToRatKingDomainFront);
	connection_add(RoyalRatVanguardExitBack, .to = DoorOfPharrosToRatKingDomainFront);
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
	connection_add(CongregationExitBack, DukesDearFreyjaEntryFront);
	warp_node_count++;

	connection_init(DukesDearFreyjaEntryFront, 0);
	warp_node_count++;

	connection_init(DukesDearFreyjaEntryBack, 2);
	connection_add(DukesDearFreyjaEntryBack, .to = DukesDearFreyjaExitFront);
	connection_add(DukesDearFreyjaEntryBack, .to = DragonMemoriesCoveSrc, .type = Conn_AshenMistHeart);
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
	connection_add(DukesDearFreyjaExitFront, .to = DragonMemoriesCoveSrc, .type = Conn_AshenMistHeart);
	warp_node_count++;


	connection_init(DukesDearFreyjaExitBack, 0); // TODO: if i want to hide majula i should add to majula warp
	warp_node_count++;

	// dlc3

	connection_init(DLC3EntranceDLC, 3);
	connection_add(DLC3EntranceDLC, .to = AavaTheKingsPetEntryFront, .type = Conn_DLC3Key);
	connection_add(DLC3EntranceDLC, .to = EleumLoyceAfterGettingEyeToSeeGhostsFront, .type = Conn_DLC3Key);
	connection_add(DLC3EntranceDLC, .to = CoffinWarpSrc, .type = Conn_FrigidOutskirtsKey);
	warp_node_count++;

	connection_init(AavaTheKingsPetEntryFront, 3);
	connection_add(AavaTheKingsPetEntryFront, .to = DLC3EntranceDLC, .type = Conn_DLC3Key);
	connection_add(AavaTheKingsPetEntryFront, .to = EleumLoyceAfterGettingEyeToSeeGhostsFront);
	connection_add(AavaTheKingsPetEntryFront, .to = CoffinWarpSrc, .type = Conn_FrigidOutskirtsKey);
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
	connection_add(LudAndZallenExitBack, .to = LudAndZallenExitWarpSrc, .type = Conn_LudAndZallenDead);
	warp_node_count++;

	connection_init(LudAndZallenExitWarpSrc, 1);
	connection_add(LudAndZallenExitWarpSrc, .to = LudAndZallenExitWarpDst, .is_warp = true);
	warp_node_count++;

	connection_init(LudAndZallenExitWarpDst, 1);
	connection_add(LudAndZallenExitWarpDst, .to = CoffinWarpSrc);
	warp_node_count++;

	connection_init(EleumLoyceAfterGettingEyeToSeeGhostsFront, 0);
	warp_node_count++;

	
	connection_init(EleumLoyceAfterGettingEyeToSeeGhostsFront, 0);
	warp_node_count++;

	connection_init(EleumLoyceAfterGettingEyeToSeeGhostsBack, 6);
	connection_add(EleumLoyceAfterGettingEyeToSeeGhostsBack, .to = AavaTheKingsPetEntryFront);
	connection_add(EleumLoyceAfterGettingEyeToSeeGhostsBack, .to = DLC3EntranceDLC, .type = Conn_DLC3Key);
	connection_add(EleumLoyceAfterGettingEyeToSeeGhostsBack, .to = EleumLoyceAfterGettingEyeToSeeGhostsFront);
	connection_add(EleumLoyceAfterGettingEyeToSeeGhostsBack, .to = CoffinWarpSrc, .type = Conn_FrigidOutskirtsKey);
	connection_add(EleumLoyceAfterGettingEyeToSeeGhostsBack, .to = AavaTheKingsPetExitBack, .type = Conn_DLC3Unfreezed);
	connection_add(EleumLoyceAfterGettingEyeToSeeGhostsBack, .to = EleumLoyceCathedraEntranceFront, .type = Conn_DLC3Unfreezed);
	warp_node_count++;

	connection_init(AavaTheKingsPetExitBack, 1);
	connection_add(AavaTheKingsPetExitBack, .to = EleumLoyceCathedraEntranceFront);
	warp_node_count++;

	connection_init(EleumLoyceCathedraEntranceFront, 1);
	connection_add(EleumLoyceCathedraEntranceFront, .to = AavaTheKingsPetExitBack);
	warp_node_count++;

	connection_two_way(AavaTheKingsPetEntryBack, AavaTheKingsPetExitFront, .type = Conn_DLC3Eye);
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
	connection_add(DrangleicCastleToShadedWoodsBack, .to = FinalFightArenaFront, .type = Conn_KingsRing);
	connection_add(DrangleicCastleToShadedWoodsBack, .to = TwinDragonridersEntryFront);
	warp_node_count++;

	connection_init(DrangleicCastleToChasmPortalFront, 2);
	connection_add(DrangleicCastleToChasmPortalFront, .to = FinalFightArenaFront, .type = Conn_KingsRing);
	connection_add(DrangleicCastleToChasmPortalFront, .to = TwinDragonridersEntryFront);
	warp_node_count++;

	connection_init(TwinDragonridersEntryFront, 0);
	warp_node_count++;

	connection_init(FinalFightArenaFront, 2);
	connection_add(FinalFightArenaFront, .to = DrangleicCastleToChasmPortalFront, .type = Conn_KingsRing);
	connection_add(FinalFightArenaFront, .to = TwinDragonridersEntryFront, .type = Conn_KingsRing);
	warp_node_count++;

	connection_init(DrangleicCastleToChasmPortalBack, 1);
	connection_add(DrangleicCastleToChasmPortalBack, .to = ChasmPortalFromCastleSrc, .type = Conn_DarkCovenentJoined);
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
	connection_add(ChasmCastleExitWarpDst, .to = ChasmPortalFromCastleSrc, .type = Conn_DarkCovenentJoined);
	warp_node_count++;

	connection_init(FinalFightArenaBack, 0);
	warp_node_count++;

	connection_two_way(TwinDragonridersEntryBack, TwinDragonridersExitFront);
	warp_node_count++;
	warp_node_count++;

	connection_init(TwinDragonridersExitBack, 1);
	connection_add(TwinDragonridersExitBack, .to = LookingGlassKnightEntryFront, .type = Conn_KeyToKingsPassage);
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
	connection_add(VendrickBack, .to = MemoryOfTheKingCryptSrc, .type = Conn_AshenMistHeart);
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
	connection_add(ChasmDarkLurkerExitWarpDst, .to = ChasmPortalFromCastleSrc, .type = Conn_DarkCovenentJoined);
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

// not considered in logic:
// 1. Jump down from bonfire above dragonrider
// 2. Jump down from Gilligan ladder location in earthen peak to harvest valley
// 3. Unlocking Gilligan ladder in Majula
// 4. Luring and enemy to the soul-opening-door in Drangleic Castle from behind the main path and opening it to access Ruin
//	  Sentinels Room
//
// might be difficult to find these
// 1. You can enter back into smeltor demon arena after the fog gate by jumping down
// 2. You can drop down in undead crypt to skip the first mandatory fog gate and reach bonfire Undead Ditch
// 3. You can drop behind the foggate near the windmill burn location in earthen peak
// 4. You can drop from the bonfire in 3. to access other parts of earthen peak
// 5. In shulva after gank fight foggate you can drop and explore whole initial shulva section
