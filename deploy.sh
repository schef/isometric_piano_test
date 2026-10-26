#!/usr/bin/env bash

# Compatibility wrapper: this flashes the three-key Fluxpad example only.
set -eu
repo_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
printf 'Deploying the Fluxpad example (not the 60-key controller).\n'
exec bash "$repo_dir/examples/fluxpad_midi/deploy.sh" "$@"
