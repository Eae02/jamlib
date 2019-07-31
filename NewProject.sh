#!/bin/bash

if [ "$#" -lt 1 ]; then
	echo "usage: NewProject.sh <name>"
	exit
fi

cp -R "$(dirname "$0")/Template" $1

sed -i "s/%PROJECT_NAME%/$1/g" "$1/CMakeLists.txt"
sed -i "s/%PROJECT_NAME%/$1/g" "$1/.vscode/launch.json"

ln -s $(realpath $(dirname "$0")) "$1/JamLib"

mkdir "$1/Assets"
mkdir "$1/Bin"
