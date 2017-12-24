set -e

CONFIG_PATH=$(pwd)/debug.xml
echo CONFIG_PATH:${CONFIG_PATH}
pushd ../../../../../../python/HorizonBuildTool/HorizonBuildTool/

python Source/HorizonCMakeBuild/Main.py --clean --config=${CONFIG_PATH}
python Source/HorizonCMakeBuild/Main.py --config=${CONFIG_PATH}

popd


cp -r ../../../../intermediate/project/macosx/i386_x86_64/Debug/lib/Debug \
	  ../../../../libs/macosx/i386_x86_64/