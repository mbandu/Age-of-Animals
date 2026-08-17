import os, re

base = r'D:\AI-Playground\Age of Animals\Source\AgeOfAnimals\Private'

# Backup and strip all constructors to minimal implementations
# We'll replace constructor bodies with just comments

files_to_strip = [
    'AoAGameMode.cpp',
    'AoAGameState.cpp', 
    'AoAPlayerState.cpp',
    'AoAGameInstance.cpp',
    'AoAPlayerController.cpp',
    'AoAUnit.cpp',
    'AoABuilding.cpp',
    'AoAResourceNode.cpp',
    'AoAHUD.cpp',
    'AoAAIController.cpp',
    'AoAUnitAIController.cpp',
    'AoASpriteGenerator.cpp',
    'AoAOnlineLobby.cpp',
]

for fname in files_to_strip:
    p = os.path.join(base, fname)
    if not os.path.exists(p):
        print(f'Skip (not found): {fname}')
        continue
    with open(p, 'r', encoding='utf-8') as f:
        c = f.read()
    
    # Find constructor patterns: ClassName::ClassName() { ... }
    # Replace the body with just a comment
    # Pattern: ClassName::ClassName(...) : parent_init { ... }
    # or: ClassName::ClassName() { ... }
    
    # Match constructor with any body
    pattern = r'(AAoA\w+::AAoA\w+\([^)]*\)(?:\s*:[^{\n]*)?\s*)\{[^}]*\}'
    
    def replace_ctor(match):
        return match.group(1) + '{\n\t// Constructor stripped for debugging\n}'
    
    c = re.sub(pattern, replace_ctor, c, flags=re.DOTALL)
    
    with open(p, 'w', encoding='utf-8') as f:
        f.write(c)
    print(f'Stripped: {fname}')

print('Done - all constructors stripped')
