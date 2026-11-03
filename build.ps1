# ═══════════════════════════════════════════════════════════
#  build.ps1 —— 一键编译脚本（窗口程序 / 控制台程序通用）
#
#  用法：
#    .\build.ps1                       编译最近改过的 .cpp
#    .\build.ps1 hanshuxvexi           编译指定源码（自动搜索，不用写路径）
#    .\build.ps1 hanshuxvexi -NoRun    只编译，不运行
#    .\build.ps1 hanshuxvexi -Clean    先删旧 exe 再编译
#
#  原理：
#    ① 在 cpp学习 目录下按名字找 .cpp（不给名字就取最近修改的那个）
#    ② 把 TMP/TEMP 指到 output\tmp —— 本机 g++ 写不进系统 %TEMP%
#    ③ 调 g++ 编译，检查退出码
#    ④ 成功才运行；失败打印错误并退出（退出码 1），绝不运行旧 exe
#
#  想改成自己的规则？改下面的 $GXX 路径 和 $linkLibs 就行了。
# ═══════════════════════════════════════════════════════════

param(
    [Parameter(Position = 0)]
    [string]$Name = "",
    [switch]$NoRun,
    [switch]$Clean
)

$ErrorActionPreference = 'Stop'

# ── 路径常量（改这里）──
$ROOT    = $PSScriptRoot                                                    # cpp学习 根目录
$GXX     = 'C:\Users\89520\Desktop\音乐\Dev-C++\Dev-Cpp\MinGW64\bin\g++.exe' # 编译器
$TEMPDIR = Join-Path $ROOT 'output\tmp'                                     # 临时目录（绕开系统 Temp）
$LIBS    = @('-lgdi32', '-luser32', '-lcomctl32')                            # 链接库（窗口程序要）

# ── 打印小工具 ──
function Info($m) { Write-Host "[信息] $m" -ForegroundColor Cyan }
function Ok($m)   { Write-Host "[成功] $m" -ForegroundColor Green }
function Warn($m) { Write-Host "[警告] $m" -ForegroundColor Yellow }
function Fail($m) { Write-Host "[失败] $m" -ForegroundColor Red }

# ── ① 找源码 ──
$allCpp = Get-ChildItem $ROOT -Recurse -Filter '*.cpp' -ErrorAction SilentlyContinue |
          Where-Object { $_.FullName -notmatch '\\output\\' }

if ($Name -ne '') {
    # 支持三种写法：纯名字 hanshuxvexi / 带扩展名 hanshuxvexi.cpp / 完整路径 C:\...\x.cpp
    $baseName = [System.IO.Path]::GetFileNameWithoutExtension($Name)
    $src = $allCpp | Where-Object { $_.BaseName -eq $baseName } |
           Sort-Object LastWriteTime -Descending | Select-Object -First 1
    if (-not $src) {
        Fail "找不到源码：$Name"
        Info '可选的源码：'
        $allCpp | Sort-Object LastWriteTime -Descending | ForEach-Object { Write-Host ('   ' + $_.BaseName) }
        exit 1
    }
} else {
    $src = $allCpp | Sort-Object LastWriteTime -Descending | Select-Object -First 1
    if (-not $src) { Fail '当前没有 .cpp 文件'; exit 1 }
}

$srcDir  = $src.DirectoryName
$exeName = $src.BaseName + '.exe'
$exePath = Join-Path $srcDir $exeName

Info ('源码    : ' + $src.FullName)
Info ('输出    : ' + $exePath)
Info ('源码时间: ' + $src.LastWriteTime.ToString('yyyy-MM-dd HH:mm:ss'))

# ── ② 环境准备 ──
if (-not (Test-Path $TEMPDIR)) { New-Item -ItemType Directory -Path $TEMPDIR -Force | Out-Null }
$env:TMP  = $TEMPDIR
$env:TEMP = $TEMPDIR

if ($Clean -and (Test-Path $exePath)) {
    try { Remove-Item $exePath -Force; Info '已删除旧 exe' }
    catch { Warn '旧 exe 删不掉（可能正在运行，先关掉程序）' }
}

# ── ③ 编译 ──
# 说明：不用 & 直接调 g++，因为 $ErrorActionPreference='Stop' 会把 g++ 的 stderr
#       当成"终止性错误"，导致脚本还没打印自己的失败信息就退出。
#       改用 Start-Process + 重定向到临时文件，能干净地拿到 stdout / stderr / 退出码。
$gArgs = @('-std=c++11', '-Wall', '-g') + @('-o', $exePath, $src.FullName) + $LIBS
Write-Host ''
Info ('编译命令: g++ ' + ($gArgs -join ' '))
Write-Host ''

# 给带空格的参数加引号（当前路径没有空格，但以后换个目录就可能踩到）
function Quote-Arg($a) {
    if ($a -match '[\s"]') { return '"' + ($a -replace '"', '\"') + '"' }
    return $a
}
$argString = (($gArgs | ForEach-Object { Quote-Arg $_ }) -join ' ')

$outFile = Join-Path $TEMPDIR 'build.out.txt'
$errFile = Join-Path $TEMPDIR 'build.err.txt'
$proc = Start-Process -FilePath $GXX -ArgumentList $argString -Wait -NoNewWindow -PassThru `
            -RedirectStandardOutput $outFile -RedirectStandardError $errFile

if (Test-Path $outFile) { Get-Content $outFile -Encoding UTF8 | ForEach-Object { Write-Host ('  ' + $_) } }

# 注意：g++（MinGW）的错误输出是系统 ANSI 编码（中文系统 = GBK/936），不是 UTF-8。
# 用 -Encoding Default 按系统代码页读，中文路径才不会变成乱码。
if (Test-Path $errFile) { Get-Content $errFile -Encoding Default | ForEach-Object { Write-Host ('  ' + $_) -ForegroundColor Red } }

if ($proc.ExitCode -ne 0) {
    Fail ('编译失败（退出码 ' + $proc.ExitCode + '）')
    if (Test-Path $exePath) {
        $age = (Get-Date) - (Get-Item $exePath).LastWriteTime
        if ($age.TotalSeconds -gt 60) { Warn '注意：现有 exe 是上一次编译的旧文件，不要拿它测新代码' }
    }
    exit 1
}

Ok ('编译成功：' + $exeName + '  (' + [math]::Round((Get-Item $exePath).Length / 1KB, 0) + ' KB)')

# ── ④ 运行 ──
if ($NoRun) { Info '已指定 -NoRun，不运行'; exit 0 }

$isGui = Select-String -Path $src.FullName -Pattern 'CreateWindowExW' -SimpleMatch -Quiet
if ($isGui) {
    Info '窗口程序：后台启动（脚本立即返回）'
    Start-Process -FilePath $exePath -WorkingDirectory $srcDir | Out-Null
} else {
    Info '控制台程序：在当前终端运行'
    & $exePath
}
exit 0
