/**
 * Serial Terminal - Web Serial API based UART terminal with CSV graphing mode
 */
const SerialTerminal = (() => {
    // --- State ---
    let _port = null;
    let _reader = null;
    let _readLoopActive = false;
    let _paused = false;          // display paused; port stays open, bytes are silently dropped
    let _graphMode = false;
    let _chart = null;
    let _csvHeaders = [];
    let _csvBuffer = '';
    let _maxDataPoints = 100;
    let _lineCount = 0;
    const MAX_TERMINAL_LINES = 500;

    // Color palette for chart series
    const CHART_COLORS = [
        '#0d6efd', '#dc3545', '#198754', '#fd7e14',
        '#6f42c1', '#20c997', '#ffc107', '#0dcaf0'
    ];

    // --- DOM helpers (lazy – safe to call before DOM is ready) ---
    const $panel     = () => $('#serial-terminal-panel');
    const $fab       = () => $('#serial-fab');
    const $output    = () => $('#serial-output');
    const $input     = () => $('#serial-input');
    const $sendBtn   = () => $('#serial-send');
    const $openBtn   = () => $('#serial-open-btn');
    const $playStop  = () => $('#serial-play-stop');
    const $clearBtn  = () => $('#serial-clear');
    const $graphBtn  = () => $('#serial-graph-mode');
    const $chartArea = () => $('#serial-chart-area');
    const $termArea  = () => $('#serial-text-area');
    const $leSel     = () => $('#serial-line-ending');
    const $statusDot = () => $('#serial-status-dot');
    const $statusTxt = () => $('#serial-status-txt');

    // --- Status helpers ---
    function _setConnected(connected) {
        if (connected) {
            $statusDot().removeClass('serial-status-off').addClass('serial-status-on');
            $statusTxt().text('Connected');
            $openBtn().text('Disconnect').removeClass('btn-success').addClass('btn-danger');
            $playStop().prop('disabled', false).text('Pause');
            $sendBtn().prop('disabled', false);
            $input().prop('disabled', false);
        } else {
            $statusDot().removeClass('serial-status-on').addClass('serial-status-off serial-status-off');
            $statusTxt().text('Disconnected');
            $openBtn().text('Connect').removeClass('btn-danger').addClass('btn-success');
            $playStop().prop('disabled', true).text('Pause');
            $sendBtn().prop('disabled', true);
            $input().prop('disabled', true);
            _paused = false;
        }
    }

    function _setPaused(paused) {
        // If no port is open, pausing/resuming has no visual effect on status
        if (!_port && paused) return;
        _paused = paused;
        if (paused) {
            $playStop().text('Stream').attr('title', 'Resume streaming');
            $statusTxt().text('Paused');
        } else {
            $playStop().text('Pause').attr('title', 'Pause display');
            $statusTxt().text(_port ? 'Connected' : 'Disconnected');
        }
    }

    // --- Output ---
    function _appendLine(text, type = 'rx') {
        if (_paused && type === 'rx') return;
        const $out = $output();
        const cssClass = type === 'tx' ? 'serial-tx' : type === 'info' ? 'serial-info' : 'serial-rx';
        const timestamp = new Date().toLocaleTimeString('en', { hour12: false });
        $out.append(
            $('<div>').addClass('serial-line ' + cssClass).text(`[${timestamp}] ${text}`)
        );
        _lineCount++;
        if (_lineCount > MAX_TERMINAL_LINES) {
            $out.children().first().remove();
            _lineCount--;
        }
        $out.scrollTop($out[0].scrollHeight);
    }

    // --- CSV / Graph ---
    function _initChart() {
        const canvas = document.getElementById('serial-chart-canvas');
        if (!canvas) return;
        if (_chart) { _chart.destroy(); _chart = null; }

        _chart = new Chart(canvas, {
            type: 'line',
            data: { labels: [], datasets: [] },
            options: {
                animation: false,
                responsive: true,
                maintainAspectRatio: false,
                scales: {
                    x: { display: true, title: { display: true, text: 'Sample' } },
                    y: { display: true }
                },
                plugins: {
                    legend: { display: true },
                    tooltip: { mode: 'index', intersect: false }
                }
            }
        });
    }

    function _processCSVLine(line) {
        if (!_chart) return;
        const parts = line.split(',').map(s => s.trim());
        const isNumeric = parts.every(p => p !== '' && !isNaN(Number(p)));

        if (!isNumeric) {
            // Header row
            _csvHeaders = parts;
            _chart.data.labels = [];
            _chart.data.datasets = parts.map((h, i) => ({
                label: h,
                data: [],
                borderColor: CHART_COLORS[i % CHART_COLORS.length],
                backgroundColor: CHART_COLORS[i % CHART_COLORS.length] + '22',
                borderWidth: 2, pointRadius: 2, tension: 0.3, fill: false
            }));
            _chart.update('none');
            return;
        }

        // Auto-create datasets if no header arrived yet
        if (_chart.data.datasets.length === 0) {
            _chart.data.datasets = parts.map((_, i) => ({
                label: _csvHeaders[i] || `Ch${i + 1}`,
                data: [],
                borderColor: CHART_COLORS[i % CHART_COLORS.length],
                backgroundColor: CHART_COLORS[i % CHART_COLORS.length] + '22',
                borderWidth: 2, pointRadius: 2, tension: 0.3, fill: false
            }));
        }

        _chart.data.labels.push(_chart.data.labels.length + 1);
        parts.forEach((val, i) => {
            if (_chart.data.datasets[i]) _chart.data.datasets[i].data.push(Number(val));
        });

        // Sliding window
        if (_chart.data.labels.length > _maxDataPoints) {
            _chart.data.labels.shift();
            _chart.data.datasets.forEach(ds => ds.data.shift());
        }
        _chart.update('none');
    }

    // --- Read loop ---
    // The loop keeps running as long as the port is open.
    // When _paused, incoming bytes are decoded and buffered for line-splitting
    // but not sent to the DOM or chart, keeping memory usage predictable.
    async function _readLoop() {
        const decoder = new TextDecoder();
        _readLoopActive = true;
        try {
            while (_port && _port.readable && _readLoopActive) {
                _reader = _port.readable.getReader();
                try {
                    while (true) {
                        const { value, done } = await _reader.read();
                        if (done) break;

                        _csvBuffer += decoder.decode(value);
                        const lines = _csvBuffer.split(/\r?\n/);
                        _csvBuffer = lines.pop(); // retain partial last line

                        lines.forEach(line => {
                            if (line.length === 0) return;
                            if (!_paused) {
                                if (_graphMode) _processCSVLine(line);
                                _appendLine(line, 'rx');
                            }
                        });
                    }
                } catch (e) {
                    if (_readLoopActive) _appendLine('Read error: ' + e.message, 'info');
                } finally {
                    if (_reader) { _reader.releaseLock(); _reader = null; }
                }
            }
        } catch (e) {
            _appendLine('Port error: ' + e.message, 'info');
        }
        _readLoopActive = false;
    }

    // --- Public API ---

    async function connect() {
        if (_port) { await disconnect(); return; }

        try {
            _port = await navigator.serial.requestPort();
            await _port.open({ baudRate: 115200 });
            // Keep DTR low so opening the port does not trigger a device reset
            await _port.setSignals({ dataTerminalReady: false, requestToSend: false });
            _setConnected(true);
            _appendLine('Port opened.', 'info');
            if (_graphMode) _initChart();
            _readLoop(); // fire-and-forget; errors surface via _appendLine
        } catch (e) {
            _port = null;
            if (e.name !== 'NotFoundError') {
                _appendLine('Could not open port: ' + e.message, 'info');
            }
        }
    }

    async function disconnect() {
        // Stop the read loop first so the reader lock is released cleanly
        _readLoopActive = false;
        if (_reader) {
            try { await _reader.cancel(); } catch (_) {}
            _reader = null;
        }
        if (_port) {
            try { await _port.close(); } catch (_) {}
            _port = null;
        }
        _setConnected(false);
        _appendLine('Port closed.', 'info');
    }

    async function send(text) {
        if (!_port || !_port.writable) return;
        // Read the chosen line ending; unescape the stored value string
        const leRaw = $leSel().val() ?? '\r\n';
        const le = leRaw.replace('\\r', '\r').replace('\\n', '\n');
        const encoder = new TextEncoder();
        const writer = _port.writable.getWriter();
        try {
            await writer.write(encoder.encode(text + le));
            _appendLine(text, 'tx');
        } finally {
            writer.releaseLock();
        }
    }

    function toggleGraphMode(enable) {
        _graphMode = enable;
        if (_graphMode) {
            $termArea().addClass('d-none');
            $chartArea().removeClass('d-none');
            if (!_chart) _initChart();
            $graphBtn().addClass('active');
        } else {
            $chartArea().addClass('d-none');
            $termArea().removeClass('d-none');
            $graphBtn().removeClass('active');
        }
    }

    function clearTerminal() {
        $output().empty();
        _lineCount = 0;
        _csvBuffer = '';
        if (_chart) {
            _chart.data.labels = [];
            _chart.data.datasets.forEach(ds => ds.data = []);
            _chart.update('none');
        }
    }

    // --- Bind UI events ---
    function init() {
        if (!('serial' in navigator)) {
            $openBtn().prop('disabled', true).attr('title', 'Web Serial API not supported in this browser');
            return;
        }

        $openBtn().on('click', connect);

        $playStop().on('click', () => _setPaused(!_paused));

        $sendBtn().on('click', () => {
            const text = $input().val();  // preserve case; don't trim user text
            if (text.length) { send(text); $input().val(''); }
        });

        $input().on('keydown', e => {
            if (e.key === 'Enter') $sendBtn().trigger('click');
        });

        $clearBtn().on('click', clearTerminal);

        $graphBtn().on('click', () => toggleGraphMode(!_graphMode));

        // FAB toggles the whole panel
        $fab().on('click', () => {
            const visible = !$panel().hasClass('d-none');
            $panel().toggleClass('d-none', visible);
            $fab().text(visible ? 'Serial Terminal' : 'Hide Terminal');
        });

        _setConnected(false);
    }

    // Exposed so web_icsp.js can pause/resume around ICSP operations
    return { init, connect, disconnect, send, toggleGraphMode, clearTerminal, setPaused: _setPaused };
})();
