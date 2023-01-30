#!/usr/bin/env bash

lib_version="0.18.1"
curl -s https://raw.githubusercontent.com/objectbox/objectbox-c/main/download.sh | bash /dev/stdin $lib_version
ls -lhR download/
echo "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
echo "^^^ Please check if you need to update the includes from the download dir ^^^"

# The standard lib in download/ does not come with TS, contact ObjectBox to get a TS lib
lib_file="../objectbox/cmake-build-relwithdebinfo/objectbox-c/libobjectbox.so"
if [ -f "$lib_file" ]; then
  cp "$lib_file" lib/
  ls -lh lib/
fi

generator_exe="objectbox-generator"
if ! command -v $generator_exe &> /dev/null; then
  generator_exe="./objectbox-generator"
  if [ ! -f "$generator_exe" ]; then
    generator_exe="../objectbox-generator/objectbox-generator"
    if [ ! -f "$generator_exe" ]; then
      echo "If you want to adjust the data model (ts-data-model.fbs), you also need ObjectBox Generator."
      echo "------------------------------------------------------------"
      echo "Please get the latest ObjectBox Generator release from here:"
      echo "https://github.com/objectbox/objectbox-generator/releases/latest"
      echo "More details: https://github.com/objectbox/objectbox-generator"
      exit
    fi
  fi
fi

echo "$generator_exe found"
"$generator_exe" -cpp ts-data-model.fbs
rsync -a *.{hpp,cpp} "src/"
