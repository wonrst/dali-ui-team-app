#!/bin/bash
set -euo pipefail
project_dir=$(cd "$(dirname "$0")/.." && pwd)
# Use an explicit local TV SDK snapshot, never the host SDK or a rolling repository.
: "${BUXBY_TV_INPUTS:?Set BUXBY_TV_INPUTS to the validated TV build-inputs directory}"
inputs=$(realpath "$BUXBY_TV_INPUTS")
build_root=${BUXBY_GBS_ROOT:-$project_dir/artifacts/gbs-armv7l}
for file in gbs.conf tizen_10.1.conf; do
  test -f "$inputs/$file" || { echo "Missing TV build input: $inputs/$file" >&2; exit 1; }
done
repositories=(-R "$inputs")
for repo in compat-repo cached-repo adaptor-build-inputs appfw-dependencies tv-ui; do
  test -d "$inputs/$repo" || { echo "Missing TV repository: $inputs/$repo" >&2; exit 1; }
  repositories+=(-R "$inputs/$repo")
done
if [[ -n ${BUXBY_COMPONENTS_REPO:-} ]]; then
  test -d "$BUXBY_COMPONENTS_REPO" || { echo "Missing UI Components repository: $BUXBY_COMPONENTS_REPO" >&2; exit 1; }
  repositories+=(-R "$(realpath "$BUXBY_COMPONENTS_REPO")")
fi
cd "$project_dir"
exec gbs --conf "$inputs/gbs.conf" build -A armv7l -P tizen_10.1 \
  -B "$build_root" -D "$inputs/tizen_10.1.conf" --skip-conf-repos "${repositories[@]}" \
  --threads "${BUXBY_JOBS:-4}" --define "jobs ${BUXBY_JOBS:-4}" \
  --no-patch-export --overwrite --include-all --packaging-dir packaging \
  --spec dali-buxby.spec --binary-list dali-buxby
