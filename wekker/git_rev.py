import subprocess

rev = subprocess.check_output(["git", "rev-parse", "--short", "HEAD"]).strip().decode("utf-8")
print(f"'-DGIT_REV=0x{rev}'")

