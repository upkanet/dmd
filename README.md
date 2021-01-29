# DMD

## Install Python from MS Store
> [Python on MS Store](https://www.microsoft.com/store/productId/9P7QFQMJRFP7)

### Check Python install in Powershell
     python -V

## Install Build tools (Powershell as Admin), may take a while
     npm install --global windows-build-tools

## Install Node-GYP (as Admin also)
     npm install -g node-gyp

## Install local packages (only use NodeJS prompt or Powershell, do not use WSL)
*(inside DMD folder, where package.json is)*

     npm i
