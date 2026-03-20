#include "MissionSystemGameMode.h"
#include "Character/User/MissionSystemCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Controllers/MainPlayerController.h"

AMissionSystemGameMode::AMissionSystemGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PlayerControllerClass = AMainPlayerController::StaticClass();
}
