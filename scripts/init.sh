#!/usr/bin/env bash

# Initialize tm with tags
echo 'Initializing tags'
tm tag add --name 'programming' --color 'light-blue'
tm tag add --name 'math' --color 'blue'
tm tag add --name 'financial' --color 'green'

# Add Generic Projects
echo 'Adding Generic Projects'
tm proj add --name 'Misc'
tm proj add --name 'Ornus'
