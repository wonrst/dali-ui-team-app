#!/bin/bash
set -euo pipefail
project_dir=$(cd "$(dirname "$0")/.." && pwd)
sdk_prefix=${BUXBY_SDK_PREFIX:-$HOME/dali-env/opt}
export LD_LIBRARY_PATH="$sdk_prefix/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export LANGUAGE=
exec "$project_dir/build-host/buxby-preview" "$@"
