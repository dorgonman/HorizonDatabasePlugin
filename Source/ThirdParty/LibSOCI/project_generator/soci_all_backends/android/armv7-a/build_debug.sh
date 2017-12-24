set -e

CONFIG_PATH=$(pwd)/debug.xml
echo CONFIG_PATH:${CONFIG_PATH}
pushd ../../../../../../python/HorizonBuildTool/HorizonBuildTool/

python Source/HorizonCMakeBuild/Main.py --clean --config=${CONFIG_PATH}
python Source/HorizonCMakeBuild/Main.py --config=${CONFIG_PATH}
popd


cp -r ../../../../intermediate/project/Android/armv7-a/Debug/lib/Debug/ \
	  ../../../../libs/android/armv7-a