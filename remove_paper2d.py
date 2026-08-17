import os

base = r"D:\AI-Playground\Age of Animals\Source\AgeOfAnimals"

# Fix AoAUnitData.h - replace UPaperFlipbook with UObject in UPROPERTY arrays
p = os.path.join(base, "Public", "AoAUnitData.h")
with open(p, "r", encoding="utf-8") as f:
    c = f.read()
# Replace all UPaperFlipbook with UObject* in TArray
c = c.replace("TArray<TObjectPtr<UPaperFlipbook>>", "TArray<UObject*>")
# Remove the PaperFlipbook include
c = c.replace('#include "PaperFlipbook.h"\n', '')
# Also replace individual UPaperFlipbook references
c = c.replace("TObjectPtr<UPaperFlipbook>", "UObject*")
with open(p, "w", encoding="utf-8") as f:
    f.write(c)
print("Fixed: AoAUnitData.h - removed PaperFlipbook UPROPERTY refs")

# Fix AoAUnit.h - replace UPaperFlipbookComponent with a plain UActorComponent
p = os.path.join(base, "Public", "AoAUnit.h")
with open(p, "r", encoding="utf-8") as f:
    c = f.read()
# Replace the PaperFlipbookComponent with a SceneComponent (always available)
c = c.replace("#include \"PaperFlipbookComponent.h\"\n", "")
c = c.replace("UPaperFlipbookComponent", "USceneComponent")
with open(p, "w", encoding="utf-8") as f:
    f.write(c)
print("Fixed: AoAUnit.h - replaced PaperFlipbookComponent with SceneComponent")

# Fix AoAUnit.cpp - replace PaperFlipbookComponent references
p = os.path.join(base, "Private", "AoAUnit.cpp")
with open(p, "r", encoding="utf-8") as f:
    c = f.read()
c = c.replace("UPaperFlipbookComponent", "USceneComponent")
with open(p, "w", encoding="utf-8") as f:
    f.write(c)
print("Fixed: AoAUnit.cpp - replaced PaperFlipbookComponent")

# Remove Paper2D from Build.cs dependencies
p = os.path.join(base, "AgeOfAnimals.Build.cs")
with open(p, "r", encoding="utf-8") as f:
    c = f.read()
c = c.replace('"Paper2D",\n', '')
with open(p, "w", encoding="utf-8") as f:
    f.write(c)
print("Fixed: Build.cs - removed Paper2D dependency")

# Also remove Paper2D from .uproject plugins
import json
uproj_path = os.path.join(base, "..", "..", "AgeOfAnimals.uproject")
with open(uproj_path, "r") as f:
    data = json.load(f)
data["Plugins"] = [p for p in data["Plugins"] if p["Name"] != "Paper2D"]
with open(uproj_path, "w") as f:
    json.dump(data, f, indent="\t")
print("Fixed: .uproject - removed Paper2D plugin")

print("Done - Paper2D completely removed")
