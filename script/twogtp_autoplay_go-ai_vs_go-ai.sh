#!/usr/bin/env bash

# Use gogui-twogtp to battle between go_ai and gnugo
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
gogui-twogtp -black ${DIR}/run_go-ai_default_engine.sh -white ${DIR}/run_go-ai_default_engine.sh -verbose -auto
