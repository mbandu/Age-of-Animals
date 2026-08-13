#include "AoAUnitAIController.h"
#include "AoAUnit.h"

AAoAUnitAIController::AAoAUnitAIController()
{
	bIsAI = true;
}

void AAoAUnitAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	// Enable navigation for this unit
}
