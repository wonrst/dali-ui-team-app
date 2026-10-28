#!/bin/bash
set -euo pipefail
project_dir=$(cd "$(dirname "$0")/.." && pwd)
sdk_prefix=${BUXBY_SDK_PREFIX:-$HOME/dali-env/opt}
export PKG_CONFIG_PATH="$sdk_prefix/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}"
cmake -S "$project_dir" -B "$project_dir/build-host" -DBUXBY_HOST_PREVIEW=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build "$project_dir/build-host" -j "${BUXBY_JOBS:-4}"
ctest --test-dir "$project_dir/build-host" --output-on-failure
