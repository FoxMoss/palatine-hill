#!/bin/bash
rm -rf bundle/
mkdir -p bundle
cd frontend/ && pnpm run build && cd ..
mkdir -p bundle/public/
cp -r frontend/dist/static/* bundle/public/

cp -r migrations bundle/
cp -r bundle/* build/

