#!/bin/fish

if count $argv > /dev/null
	set ICON_NAME $argv[1]
else if test -f ./Icon.png
	set ICON_NAME ./Icon.png
else
	set ICON_NAME (read -P "Icon name: ")
end

mkdir -p .build/icon
set ICON_DIR_NAME (realpath "./.build/icon")

convert "$ICON_NAME" -define icon:auto-resize="128,96,64,48,32,16" "$ICON_DIR_NAME/icon.ico"
echo "id ICON \"$ICON_DIR_NAME/icon.ico\"" > $ICON_DIR_NAME/icon.rc

cp $ICON_NAME "$ICON_DIR_NAME/jm_game_icon"

pushd .
cd $ICON_DIR_NAME
ld -r -b binary -o icon.o jm_game_icon
popd
