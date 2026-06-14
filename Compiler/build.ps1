param(
    [string]$Target = 'all'
)

$ErrorActionPreference = 'Stop'
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $scriptDir

$compilerSources = @(
    'src/mainCompiler.cpp',
    'src/lexer.cpp',
    'src/tokenizer.cpp',
    'src/parser.cpp',
    'src/utils.cpp',
    'src/VM.cpp',
    'src/SymbolTable.cpp'
)

$executorSources = @(
    'src/mainExec.cpp',
    'src/CPU.cpp',
    'src/Loader.cpp',
    'src/Memory.cpp',
    'src/Executor.cpp'
)

$debuggerSources = @(
    'src/mainDebug.cpp',
    'src/CPU.cpp',
    'src/Loader.cpp',
    'src/Memory.cpp',
    'src/Debugger.cpp'
)

function Build-Program([string]$output, [string[]]$sources) {
    Write-Host "Building $output..."
    & clang++ -Wall -Wextra -std=c++17 $sources -o $output
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed for $output"
    }
}

function Clean-Output([string[]]$files) {
    foreach ($file in $files) {
        if (Test-Path $file) {
            Remove-Item $file -Force
            Write-Host "Removed $file"
        }
    }
}

switch ($Target.ToLower()) {
    'all' {
        Build-Program 'Compiler.exe' $compilerSources
        Build-Program 'Executor.exe' $executorSources
        Build-Program 'Debugger.exe' $debuggerSources
    }
    'clean' {
        Clean-Output @('Compiler.exe', 'Executor.exe', 'Debugger.exe')
    }
    'fclean' {
        Clean-Output @('Compiler.exe', 'Executor.exe', 'Debugger.exe')
    }
    default {
        Write-Host "Usage: .\build.ps1 [all|clean|fclean]"
        exit 1
    }
}
