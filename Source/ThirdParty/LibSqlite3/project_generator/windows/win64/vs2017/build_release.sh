set -e

CONFIG_PATH=$(pwd)/release.xml
echo CONFIG_PATH:${CONFIG_PATH}
HORIZON_BUILD_TOOL=$(cd ../../../../../../../../../ci_scripts/function/python/HorizonBuildTool/HorizonBuildTool/;pwd)
echo "====================HORIZON_BUILD_TOOL:" ${HORIZON_BUILD_TOOL}
pushd ${HORIZON_BUILD_TOOL}

python Source/HorizonCMakeBuild/Main.py --clean --config=${CONFIG_PATH}
python Source/HorizonCMakeBuild/Main.py --config=${CONFIG_PATH}
popd
