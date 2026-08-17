import os

base = r"D:\AI-Playground\Age of Animals\Source\AgeOfAnimals"

# Add #include "AoAStubs.h" to files that use the excluded types
files_using_types = [
    "Public/AoAGameMode.h",
    "Public/AoAGameInstance.h",
    "Public/AoAPlayerState.h",
    "Public/AoAPlayerController.h",
    "Public/AoAUnit.h",
    "Public/AoABuilding.h",
    "Public/AoAResourceNode.h",
    "Public/AoAHUD.h",
    "Public/AoAOnlineLobby.h",
    "Private/AoAGameMode.cpp",
    "Private/AoAGameInstance.cpp",
    "Private/AoAPlayerState.cpp",
    "Private/AoAPlayerController.cpp",
    "Private/AoAUnit.cpp",
    "Private/AoABuilding.cpp",
    "Private/AoAResourceNode.cpp",
    "Private/AoAHUD.cpp",
    "Private/AoAOnlineLobby.cpp",
    "Private/AoAAIController.cpp",
    "Private/AoASpriteGenerator.cpp",
]

for fname in files_using_types:
    p = os.path.join(base, fname)
    if not os.path.exists(p): continue
    with open(p, 'r', encoding='utf-8') as f:
        c = f.read()
    if 'AoAStubs.h' not in c:
        # Add after the first #include
        c = c.replace('#include "CoreMinimal.h"', '#include "CoreMinimal.h"\n#include "AoAStubs.h"', 1)
        with open(p, 'w', encoding='utf-8') as f:
            f.write(c)
        print(f'Added stubs: {fname}')

print('Done')
