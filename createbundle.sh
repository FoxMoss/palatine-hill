#!/bin/bash
rm -rf bundle/
mkdir -p bundle
cd frontend/ && pnpm run build && cd ..
mkdir -p bundle/public/
cp -r frontend/dist/static/* bundle/public/

cp -r migrations bundle/
cd debian-build/ && strat debian ninja && cd ..
cp debian-build/src/palatine bundle/

cp bundle/public build/ -r
cp bundle/migrations build/ -r

cd bundle && tar -czvf ../bundle.tar.gz ./
