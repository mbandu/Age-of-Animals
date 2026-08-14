#include "AoAUnitAIController.h"
#include "AoAUnit.h"

AAoAUnitAIController::AAoAUnitAIController()
{
}

void AAoAUnitAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	// Enable navigation for this unit
}
