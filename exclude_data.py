import os

base = r"D:\AI-Playground\Age of Animals\Source\AgeOfAnimals"

# Wrap data asset headers in #if 0 / #endif
data_assets = [
    ("Public/AoAEmpireData.h", "Private/AoAEmpireData.cpp"),
    ("Public/AoAUnitData.h", "Private/AoAUnitData.cpp"),
    ("Public/AoABuildingData.h", "Private/AoABuildingData.cpp"),
]

for hdr, cpp in data_assets:
    hdr_path = os.path.join(base, hdr)
    cpp_path = os.path.join(base, cpp)
    
    # Wrap entire header file content in #if 0
    with open(hdr_path, 'r', encoding='utf-8') as f:
        content = f.read()
    if '#if 0' not in content[:20]:
        with open(hdr_path, 'w', encoding='utf-8') as f:
            f.write('#if 0\n' + content + '\n#endif\n')
        print(f'Wrapped: {hdr}')
    
    # Wrap entire cpp file content in #if 0
    if os.path.exists(cpp_path):
        with open(cpp_path, 'r', encoding='utf-8') as f:
            content = f.read()
        if '#if 0' not in content[:20]:
            with open(cpp_path, 'w', encoding='utf-8') as f:
                f.write('#if 0\n' + content + '\n#endif\n')
            print(f'Wrapped: {cpp}')

print('Done - data assets excluded')
