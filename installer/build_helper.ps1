param([string]$appDir)

$Host.UI.RawUI.WindowTitle = 'Voxira Game Engine - Building'

function Draw-Bar {
    param([int]$pct, [string]$status)
    $w = 46
    $fill = [Math]::Min([int]($w * $pct / 100), $w)
    $done = if ($fill -gt 0) { '=' * ($fill - 1) + '>' } else { '' }
    $rest = ' ' * ($w - $fill)
    $line = "  [$done$rest] $("$pct".PadLeft(3))%  $status"
    Write-Host ("`r" + $line + (' ' * 10)) -NoNewline
}

function Run-Step {
    param([string]$exe, [string[]]$argList, [string]$label, [int]$from, [int]$to, [int]$estSecs)
    $log    = "$env:TEMP\voxira_out.log"
    $logErr = "$env:TEMP\voxira_err.log"
    $proc = Start-Process $exe -ArgumentList $argList -WorkingDirectory $appDir `
        -PassThru -NoNewWindow -RedirectStandardOutput $log -RedirectStandardError $logErr
    $start = Get-Date
    while (!$proc.HasExited) {
        $elapsed = ((Get-Date) - $start).TotalSeconds
        $ratio   = [Math]::Min($elapsed / $estSecs, 0.93)
        $pct     = [int]($from + ($to - $from) * $ratio)
        Draw-Bar $pct $label
        Start-Sleep -Milliseconds 120
    }
    Draw-Bar $to $label
    return $proc.ExitCode
}

Clear-Host
Write-Host ''
Write-Host '  Voxira Game Engine' -ForegroundColor Cyan
Write-Host ('  ' + ('=' * 54))
Write-Host ''
Write-Host '  >> Dieses Fenster nicht schliessen! <<' -ForegroundColor Yellow
Write-Host '     Engine wird zum ersten Mal gebaut...'
Write-Host ''

$code = Run-Step 'cmake' @('-B', 'build') 'Konfiguriere CMake...' 0 20 30
if ($code -ne 0) {
    Write-Host ''
    Write-Host '  FEHLER: CMake-Konfiguration fehlgeschlagen.' -ForegroundColor Red
    Write-Host '  Visual Studio Build Tools mit C++ Workload benoetigt.' -ForegroundColor Red
    Write-Host ''
    Read-Host '  Enter druecken zum Schliessen'
    exit 1
}

Write-Host ''
$code = Run-Step 'cmake' @('--build', 'build', '--config', 'Release') 'Kompiliere...  (beim ersten Mal ein paar Minuten)' 20 100 300
if ($code -ne 0) {
    Write-Host ''
    Write-Host '  FEHLER: Kompilierung fehlgeschlagen.' -ForegroundColor Red
    Write-Host "  Log: $env:TEMP\voxira_err.log" -ForegroundColor DarkGray
    Write-Host ''
    Read-Host '  Enter druecken zum Schliessen'
    exit 1
}

Write-Host ''
Write-Host ''
Write-Host '  Fertig! Ordner wird geoeffnet...' -ForegroundColor Green
Start-Sleep -Seconds 1
Start-Process 'explorer.exe' -ArgumentList $appDir
