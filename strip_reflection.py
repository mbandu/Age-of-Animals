import os, re

base = r"D:\AI-Playground\Age of Animals\Source\AgeOfAnimals\Public"

# Remove all UE reflection macros from headers
# This will make the classes plain C++ classes (no reflection)
# If the module loads with this, the crash is in reflection metadata

files = [
    "AoAEmpireData.h",
    "AoAUnitData.h", 
    "AoABuildingData.h",
    "AoAGameMode.h",
    "AoAGameState.h",
    "AoAPlayerState.h",
    "AoAGameInstance.h",
    "AoAPlayerController.h",
    "AoAUnit.h",
    "AoABuilding.h",
    "AoAResourceNode.h",
    "AoAHUD.h",
    "AoAAIController.h",
    "AoAUnitAIController.h",
    "AoASpriteGenerator.h",
    "AoAOnlineLobby.h",
]

for fname in files:
    p = os.path.join(base, fname)
    if not os.path.exists(p):
        print(f"Skip: {fname}")
        continue
    with open(p, "r", encoding="utf-8") as f:
        c = f.read()
    
    # Remove UCLASS(...) blocks
    c = re.sub(r"UCLASS\([^)]*\)\s*\n", "", c)
    # Remove USTRUCT(...) blocks
    c = re.sub(r"USTRUCT\([^)]*\)\s*\n", "", c)
    # Remove UENUM(...) blocks  
    c = re.sub(r"UENUM\([^)]*\)\s*\n", "", c)
    # Remove UPROPERTY(...) lines
    c = re.sub(r"UPROPERTY\([^)]*\)\s*\n", "", c)
    # Remove UFUNCTION(...) lines
    c = re.sub(r"UFUNCTION\([^)]*\)\s*\n", "", c)
    # Remove GENERATED_BODY()
    c = re.sub(r"GENERATED_BODY\(\)\s*\n", "", c)
    # Remove DECLARE_DYNAMIC_MULTICAST_DELEGATE lines
    c = re.sub(r"DECLARE_DYNAMIC_MULTICAST_DELEGATE[^\n]*\n", "", c)
    
    with open(p, "w", encoding="utf-8") as f:
        f.write(c)
    print(f"Stripped: {fname}")

print("Done - all reflection macros removed")
