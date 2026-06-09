/**
 * @file    camera_index.h
 * @author  OceanLabz (adapted from Espressif CameraWebServer example)
 * @date    2026
 * @brief   HTML page for the camera streaming web interface.
 *
 *          Served at "/" by the HTTP server. Provides:
 *          - Live MJPEG stream viewer
 *          - Single-frame capture button
 *          - Camera sensor control panel (resolution, quality, etc.)
 *
 *          Adapted for the OceanLabz AI Explorer S3 Board.
 *          Face detection/recognition UI removed for simplicity.
 */

#ifndef CAMERA_INDEX_H
#define CAMERA_INDEX_H

static const char index_html[] = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>OceanLabz AI Explorer S3 — Camera Stream</title>
  <style>
    :root {
      --primary: #0077be;
      --primary-dark: #005a94;
      --bg: #1a1a2e;
      --card-bg: #16213e;
      --text: #e0e0e0;
      --text-muted: #a0a0a0;
      --border: #0f3460;
      --success: #00c853;
      --danger: #ff1744;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      background: var(--bg);
      color: var(--text);
      min-height: 100vh;
    }
    header {
      background: var(--card-bg);
      border-bottom: 2px solid var(--primary);
      padding: 12px 20px;
      display: flex;
      align-items: center;
      justify-content: space-between;
    }
    header h1 {
      font-size: 1.3em;
      color: var(--primary);
    }
    header .status {
      font-size: 0.85em;
      color: var(--text-muted);
    }
    .container {
      max-width: 1200px;
      margin: 0 auto;
      padding: 15px;
      display: flex;
      flex-wrap: wrap;
      gap: 15px;
    }
    .stream-panel {
      flex: 1 1 640px;
      min-width: 320px;
    }
    .control-panel {
      flex: 0 1 320px;
      min-width: 280px;
    }
    .card {
      background: var(--card-bg);
      border: 1px solid var(--border);
      border-radius: 8px;
      padding: 15px;
      margin-bottom: 15px;
    }
    .card h2 {
      font-size: 1.1em;
      color: var(--primary);
      margin-bottom: 10px;
      border-bottom: 1px solid var(--border);
      padding-bottom: 5px;
    }
    .stream-container {
      position: relative;
      background: #000;
      border-radius: 6px;
      overflow: hidden;
    }
    .stream-container img {
      width: 100%;
      display: block;
    }
    .btn-group {
      display: flex;
      gap: 8px;
      margin-top: 10px;
    }
    button {
      background: var(--primary);
      color: #fff;
      border: none;
      border-radius: 4px;
      padding: 8px 16px;
      cursor: pointer;
      font-size: 0.9em;
      transition: background 0.2s;
    }
    button:hover { background: var(--primary-dark); }
    button.danger { background: var(--danger); }
    button.danger:hover { background: #d50000; }
    button.success { background: var(--success); }
    button.success:hover { background: #00a844; }
    .control-row {
      display: flex;
      align-items: center;
      justify-content: space-between;
      margin-bottom: 8px;
    }
    .control-row label {
      font-size: 0.85em;
      color: var(--text-muted);
      min-width: 100px;
    }
    .control-row select,
    .control-row input[type="range"] {
      flex: 1;
      margin-left: 8px;
    }
    select {
      background: var(--bg);
      color: var(--text);
      border: 1px solid var(--border);
      border-radius: 3px;
      padding: 4px 8px;
      font-size: 0.85em;
    }
    input[type="range"] {
      accent-color: var(--primary);
    }
    .toggle-group {
      display: flex;
      flex-wrap: wrap;
      gap: 6px;
      margin-top: 8px;
    }
    .toggle-btn {
      background: var(--bg);
      color: var(--text-muted);
      border: 1px solid var(--border);
      border-radius: 4px;
      padding: 5px 10px;
      cursor: pointer;
      font-size: 0.8em;
      transition: all 0.2s;
    }
    .toggle-btn.active {
      background: var(--primary);
      color: #fff;
      border-color: var(--primary);
    }
    .hidden { display: none; }
    @media (max-width: 600px) {
      .container { flex-direction: column; }
      .control-panel { flex: 1 1 auto; }
    }
  </style>
</head>
<body>
  <header>
    <h1>OceanLabz AI Explorer S3 - Camera</h1>
    <div class="status" id="streamStatus">Disconnected</div>
  </header>

  <div class="container">
    <!-- Stream Panel -->
    <div class="stream-panel">
      <div class="card">
        <h2>Live Stream</h2>
        <div class="stream-container">
          <img id="streamImage" src="" alt="Camera Stream">
        </div>
        <div class="btn-group">
          <button class="success" onclick="startStream()">▶ Start Stream</button>
          <button class="danger"  onclick="stopStream()">■ Stop Stream</button>
          <button onclick="captureFrame()">📷 Capture</button>
          <button onclick="saveCapture()">💾 Save</button>
        </div>
      </div>

      <div class="card">
        <h2>Captured Frame</h2>
        <div class="stream-container">
          <img id="captureImage" src="" alt="Captured Frame">
        </div>
      </div>
    </div>

    <!-- Control Panel -->
    <div class="control-panel">
      <div class="card">
        <h2>Resolution & Quality</h2>
        <div class="control-row">
          <label>Frame Size</label>
          <select id="framesize" onchange="setControl('framesize', this.value)">
            <option value="10">UXGA (1600x1200)</option>
            <option value="9">SXGA (1280x1024)</option>
            <option value="8">XGA  (1024x768)</option>
            <option value="7">SVGA (800x600)</option>
            <option value="6">VGA  (640x480)</option>
            <option value="5">CIF  (400x296)</option>
            <option value="4">QVGA (320x240)</option>
            <option value="3">HQVGA(240x176)</option>
            <option value="2">QQVGA(160x120)</option>
          </select>
        </div>
        <div class="control-row">
          <label>JPEG Quality</label>
          <input type="range" id="quality" min="1" max="63" value="10" oninput="setControl('quality', this.value)">
        </div>
      </div>

      <div class="card">
        <h2>Brightness / Contrast</h2>
        <div class="control-row">
          <label>Brightness</label>
          <input type="range" id="brightness" min="-2" max="2" value="0" oninput="setControl('brightness', this.value)">
        </div>
        <div class="control-row">
          <label>Contrast</label>
          <input type="range" id="contrast" min="-2" max="2" value="0" oninput="setControl('contrast', this.value)">
        </div>
        <div class="control-row">
          <label>Saturation</label>
          <input type="range" id="saturation" min="-2" max="2" value="0" oninput="setControl('saturation', this.value)">
        </div>
      </div>

      <div class="card">
        <h2>Orientation</h2>
        <div class="toggle-group">
          <button class="toggle-btn" id="hmirror" onclick="toggleControl('hmirror')">Mirror H</button>
          <button class="toggle-btn" id="vflip"   onclick="toggleControl('vflip')">Flip V</button>
        </div>
      </div>

      <div class="card">
        <h2>White Balance</h2>
        <div class="toggle-group">
          <button class="toggle-btn" id="awb"      onclick="toggleControl('awb')">AWB</button>
          <button class="toggle-btn" id="awb_gain" onclick="toggleControl('awb_gain')">AWB Gain</button>
          <button class="toggle-btn" id="wb_mode"  onclick="cycleWB()">WB Mode</button>
        </div>
      </div>

      <div class="card">
        <h2>Exposure / Gain</h2>
        <div class="toggle-group">
          <button class="toggle-btn" id="aec"  onclick="toggleControl('aec')">AEC</button>
          <button class="toggle-btn" id="aec2" onclick="toggleControl('aec2')">AEC2</button>
          <button class="toggle-btn" id="agc"  onclick="toggleControl('agc')">AGC</button>
        </div>
        <div class="control-row" style="margin-top:8px">
          <label>AE Level</label>
          <input type="range" id="ae_level" min="-2" max="2" value="0" oninput="setControl('ae_level', this.value)">
        </div>
        <div class="control-row">
          <label>AEC Value</label>
          <input type="range" id="aec_value" min="0" max="1200" value="204" oninput="setControl('aec_value', this.value)">
        </div>
        <div class="control-row">
          <label>AGC Gain</label>
          <input type="range" id="agc_gain" min="0" max="30" value="0" oninput="setControl('agc_gain', this.value)">
        </div>
      </div>

      <div class="card">
        <h2>Effects & Misc</h2>
        <div class="control-row">
          <label>Special Effect</label>
          <select id="special_effect" onchange="setControl('special_effect', this.value)">
            <option value="0">No Effect</option>
            <option value="1">Negative</option>
            <option value="2">Grayscale</option>
            <option value="3">Red Tint</option>
            <option value="4">Green Tint</option>
            <option value="5">Blue Tint</option>
            <option value="6">Sepia</option>
          </select>
        </div>
        <div class="toggle-group">
          <button class="toggle-btn" id="colorbar" onclick="toggleControl('colorbar')">Color Bar</button>
          <button class="toggle-btn" id="bpc"      onclick="toggleControl('bpc')">BPC</button>
          <button class="toggle-btn" id="wpc"      onclick="toggleControl('wpc')">WPC</button>
          <button class="toggle-btn" id="raw_gma"  onclick="toggleControl('raw_gma')">Raw GMA</button>
          <button class="toggle-btn" id="lenc"     onclick="toggleControl('lenc')">LENC</button>
          <button class="toggle-btn" id="dcw"      onclick="toggleControl('dcw')">DCW</button>
        </div>
      </div>
    </div>
  </div>

  <script>
    const baseHost = location.origin;
    // Stream server runs on port 81. If origin shows explicit port, replace it;
    // if port is implicit (default 80, not shown in URL), append :81.
    const streamUrl = (location.port ?
      baseHost.replace(/:\d+$/, ':81') :
      baseHost + ':81') + '/stream';

    let streamActive = false;
    let wbMode = 0;

    // ── Stream Control ──────────────────────────────────────
    function startStream() {
      if (streamActive) return;
      const img = document.getElementById('streamImage');
      img.src = streamUrl;
      streamActive = true;
      document.getElementById('streamStatus').textContent = 'Streaming ●';
      document.getElementById('streamStatus').style.color = '#00c853';
    }

    function stopStream() {
      if (!streamActive) return;
      const img = document.getElementById('streamImage');
      img.src = '';
      streamActive = false;
      document.getElementById('streamStatus').textContent = 'Disconnected';
      document.getElementById('streamStatus').style.color = '#a0a0a0';
    }

    // ── Capture ─────────────────────────────────────────────
    function captureFrame() {
      const url = baseHost + '/capture?_t=' + Date.now();
      document.getElementById('captureImage').src = url;
    }

    function saveCapture() {
      // Fetch a fresh frame as a Blob, then use the Blob URL for both
      // displaying and downloading. Only ONE HTTP request to /capture
      // (the ESP32 camera can only serve one frame at a time).
      const ts = Date.now();
      const url = baseHost + '/capture?_t=' + ts;
      fetch(url)
        .then(function(response) { return response.blob(); })
        .then(function(blob) {
          const blobUrl = URL.createObjectURL(blob);
          // Display the captured frame using the Blob URL
          document.getElementById('captureImage').src = blobUrl;
          // Download via <a> with Blob URL (same-origin, download works reliably)
          const a = document.createElement('a');
          a.href = blobUrl;
          a.download = 'capture_' + ts + '.jpg';
          document.body.appendChild(a);
          a.click();
          document.body.removeChild(a);
        })
        .catch(function(e) { console.error('Save capture failed:', e); });
    }

    // ── Control Helpers ─────────────────────────────────────
    function setControl(varName, value) {
      const url = baseHost + '/control?var=' + varName + '&val=' + value;
      fetch(url).catch(e => console.error(e));
    }

    function toggleControl(varName) {
      const btn = document.getElementById(varName);
      const newVal = btn.classList.contains('active') ? 0 : 1;
      btn.classList.toggle('active');
      setControl(varName, newVal);
    }

    function cycleWB() {
      wbMode = (wbMode + 1) % 4;
      const btn = document.getElementById('wb_mode');
      btn.textContent = 'WB Mode ' + wbMode;
      setControl('wb_mode', wbMode);
    }

    // ── Load Current Settings ───────────────────────────────
    function loadSettings() {
      fetch(baseHost + '/status')
        .then(r => r.json())
        .then(data => {
          // Sliders
          const sliders = ['quality','brightness','contrast','saturation',
                           'ae_level','aec_value','agc_gain'];
          sliders.forEach(s => {
            const el = document.getElementById(s);
            if (el && data[s] !== undefined) el.value = data[s];
          });
          // Selects
          if (data.framesize !== undefined) {
            document.getElementById('framesize').value = data.framesize;
          }
          if (data.special_effect !== undefined) {
            document.getElementById('special_effect').value = data.special_effect;
          }
          // Toggles
          const toggles = ['awb','awb_gain','aec','aec2','agc',
                           'hmirror','vflip','colorbar','bpc','wpc',
                           'raw_gma','lenc','dcw'];
          toggles.forEach(t => {
            const btn = document.getElementById(t);
            if (btn && data[t] === 1) btn.classList.add('active');
            else if (btn) btn.classList.remove('active');
          });
          // WB mode
          if (data.wb_mode !== undefined) wbMode = data.wb_mode;
        })
        .catch(e => console.error('Failed to load settings:', e));
    }

    // ── Init ────────────────────────────────────────────────
    loadSettings();
    // Auto-start stream
    startStream();
  </script>
</body>
</html>
)rawliteral";

#endif // CAMERA_INDEX_H
