#!/usr/bin/env sh

set -e

RUNUSER_UID="${RUNUSER_UID:-1000}"

if [ "$(id -u creator)" != "$RUNUSER_UID" ]; then
    usermod -u "${RUNUSER_UID}" creator
    groupmod -g "${RUNUSER_UID}" creator
    usermod -g "${RUNUSER_UID}" creator
fi

chown creator:creator -R /home

su -l creator -c "qucs $*"
