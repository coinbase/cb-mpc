#!/bin/bash

set -e
version="$1"
echo "Releasing to $version branch..."
rm -rf dist
set -x
npm install
npm run build
npm run lint
# npm test
npm prune --production
rm -rf .release
mkdir -p .release
cp action.yml package.json package-lock.json .release/
rsync -R dist/src/*.js .release/
cp -R node_modules .release/node_modules
rm -rf node_modules  # remove node_modules/.cache
rm -rf dist
mkdir -p dist/src
mv .release/action.yml .
mv .release/dist/src/ ./dist/
mv .release/*.json .
mv .release/node_modules .
set +x