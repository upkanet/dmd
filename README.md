# DMD

## Install MSBuild
> [Python on MS Store](https://visualstudio.microsoft.com/fr/downloads/?q=build+tools)

### Add MSBuild.exe folder to Path
> C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\MSBuild\Current\Bin

## Compile AxoDMD.exe
> .\build.bat

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
