import os, re

base = r"D:\AI-Playground\Age of Animals\Source"

for root, dirs, files in os.walk(base):
    for fname in files:
        if not fname.endswith(".h"):
            continue
        fpath = os.path.join(root, fname)
        with open(fpath, "r", encoding="utf-8") as f:
            c = f.read()
        original = c
        
        # Remove .generated.h includes
        c = re.sub(r'#include "[^"]*\.generated\.h"\n', '', c)
        
        # Remove UCLASS(...) lines
        c = re.sub(r'UCLASS\([^)]*\)\s*\n', '', c)
        # Remove USTRUCT(...) lines
        c = re.sub(r'USTRUCT\([^)]*\)\s*\n', '', c)
        # Remove UENUM(...) lines
        c = re.sub(r'UENUM\([^)]*\)\s*\n', '', c)
        # Remove UPROPERTY(...) lines
        c = re.sub(r'UPROPERTY\([^)]*\)\s*\n', '', c)
        # Remove UFUNCTION(...) lines
        c = re.sub(r'UFUNCTION\([^)]*\)\s*\n', '', c)
        # Remove GENERATED_BODY()
        c = re.sub(r'GENERATED_BODY\(\)\s*\n', '', c)
        # Remove DECLARE_DYNAMIC_MULTICAST_DELEGATE lines
        c = re.sub(r'DECLARE_DYNAMIC_MULTICAST_DELEGATE[^\n]*\n', '', c)
        # Remove UFUNCTION(BlueprintImplementableEvent) blocks
        c = re.sub(r'UFUNCTION\(BlueprintImplementableEvent[^\n]*\n', '', c)
        
        if c != original:
            with open(fpath, "w", encoding="utf-8") as f:
                f.write(c)
            relpath = os.path.relpath(fpath, base)
            print(f"Stripped: {relpath}")

print("Done - all reflection macros removed from all headers")
