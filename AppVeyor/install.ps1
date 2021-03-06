# Sample script to install Python and pip under Windows
# Authors: Olivier Grisel, Jonathan Helmus, Kyle Kastner, and Alex Willmer
# License: CC0 1.0 Universal: http://creativecommons.org/publicdomain/zero/1.0/

$WINSDK_URL = "https://www.python.org/ftp/python/"
$GCCCOMPILER_URL = "https://www.python.org/ftp/python/"

$PYTHON_PRERELEASE_REGEX = @"
(?x)
(?<major>\d+)
\.
(?<minor>\d+)
\.
(?<micro>\d+)
(?<prerelease>[a-z]{1,2}\d+)
"@

function InstallNETMFSDK()
{
    # NG?
    # Start-FileDownload "https://github.com/NETMF/netmf-interpreter/releases/download/NETMFCryptoLibraries/NetMFCryptoLibs.msi"
    Write-Output "NetMFCryptoLibs.msi : InstallStart"
    msiexec.exe NetMFCryptoLibs.msi /S /v/norestart /v/qn
}

function InstallGCCCompiler()
{
    # NG?
    # Start-FileDownload "https://launchpad.net/gcc-arm-embedded/5.0/5-2015-q4-major/+download/gcc-arm-none-eabi-5_2-2015q4-20151219-win32.zip"

    # 当スクリプトのディレクトリ
    # $basedir = Convert-Path $(Split-Path -Path $MyInvocation.InvocationName -Parent)
    # Set-Location $basedir

    # ZIPファイル展開(GCC)
    # $shell = New-Object -ComObject Shell.Application
    # $archivePath = "C:\\projects\\MicroFrameworkPK-v4-3"
    # $unzipDirObj = $sh.NameSpace($archivePath)
    # $zipFilePath = "C:\\projects\\MicroFrameworkPK-v4-3\\gcc-arm-none-eabi-5_2-2015q4-20151219-win32.zip"
    # $zipPathObj = $sh.NameSpace($zipFilePath)
    # Write-Output "GCCCompiler : ArchiverDecompressStart $zipPath"
    # $unzipDirObj.CopyHere($zipPathObj.Items())
    
    # $archivePath = "C:\\projects\\gcc-arm-none-eabi-5_2"
    $archivePath = "C:\\projects\\gcc-arm-none-eabi-4_9"
    # $zipFilePath = "C:\\projects\\MicroFrameworkPK-v4-3\\gcc-arm-none-eabi-5_2-2015q4-20151219-win32.zip"
    $zipFilePath = "C:\\projects\\MicroFrameworkPK-v4-3\\gcc-arm-none-eabi-4_9-2014q4-20141203-win32.zip"
    New-ZipExtract -source $zipFilePath -destination $archivePath -force -verbose
}

function InstallBuildTools()
{
    # NG?
    # Start-FileDownload "http://netmf.github.io/downloads/build-tools.zip"

    # ZIPファイル展開(SDK Tools)
    # Appveyor - NG
    # $shell = New-Object -ComObject Shell.Application
    # $archivePath = "C:\\projects"
    # $unzipDirObj = $shell.NameSpace($archivePath)
    # $zipFilePath = "C:\\projects\\MicroFrameworkPK-v4-3\\build-tools.zip"
    # $zipPathObj = $sh.NameSpace($zipFilePath)
    # Write-Output "BuildTools : ArchiveDecompressStart $zipPath"
    # $unzipDirObj.CopyHere($zipPathObj.Items())
    
    $archivePath = "C:\\projects\\build-tools"
    $zipFilePath = "C:\\projects\\MicroFrameworkPK-v4-3\\build-tools.zip"
    New-ZipExtract -source $zipFilePath -destination $archivePath -force -verbose
}

function main () 
{
    # Version 4.3
    InstallGCCCompiler

    # Version 4.4
    # InstallNETMFSDK
    # InstallGCCCompiler
    # InstallBuildTools
}

main
