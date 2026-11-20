// Simuliertes RC-Multiswitch-Sound (Firmware RC_Multiswitch_Sound, ESP32-S3).
// Schaltteil wie RC-Multiswitch (mock_multiswitch.js), Soundteil wie
// ESP32-RC-Sound V3 (PST.soundCore). Startwerte aus dem Code: build.py fuehrt
// handleStatus(), handleConfig(), buildSoundJson(), handleDebug() und
// handleSoundFiles() trocken aus; Felder, die die Schalt-Simulation nicht
// kennt, kommen aus diesen Beispielantworten.
PST.register('mock_ms_sound', function (variant, ctx) {
  var sw = PST.mocks.mock_multiswitch(variant, ctx);
  var M = PST.autoMock(ctx, {});
  var C = M.get('/api/sconfig'), St = C.settings;
  C.version = ctx.variant.version;
  var apActive = true;
  var snd = PST.soundCore({ C: C, store: 'SD-Karte', total: 7948206080, version: C.version,
    sport: function () { return St.portb_role === 2; } });
  function deep(dst, src) {
    Object.keys(src || {}).forEach(function (k) {
      if (src[k] && typeof src[k] === 'object' && !Array.isArray(src[k]) && dst[k] && typeof dst[k] === 'object' && !Array.isArray(dst[k])) deep(dst[k], src[k]);
      else dst[k] = src[k];
    });
    return dst;
  }
  // Antwort der Schalt-Simulation ueber die Beispielantwort aus dem Code legen
  function overSample(path, r) {
    var s = M.get(path);
    if (!s || !r || r.status !== 200) return r;
    try { return PST.json(deep(JSON.parse(JSON.stringify(s)), JSON.parse(r.text))); } catch (e) { return r; }
  }
  function swCfg() { return JSON.parse(sw.handle('GET', '/api/config', {}, '').text); }
  function debug() {
    var d = JSON.parse(JSON.stringify(M.get('/api/debug'))), t = Date.now() / 1000;
    d.channels = d.channels.map(function (x, i) { return i === 2 ? 172 + Math.round(800 * (1 + Math.sin(t / 2))) : 992; });
    var ok = function (k) { return snd.fsize(C.sound_files[k]) ? 1 : 0; };
    d.wav = { loop: ok('loop'), shut: ok('shut'), start: ok('start'), s: d.wav.s.map(function (x, i) { return ok(String(i + 1)); }) };
    d.crsf_diag = { raw_bytes: Math.round(t * 1450) % 1e7, valid_frames: Math.round(t * 150) % 1e6, crc_errors: 0, device_pings: 3, param_reads: 41, param_writes: 2 };
    d.wifi = { active: apActive, bus_ok: true };
    return d;
  }
  var routes = PST.merge({}, snd.routes);
  PST.merge(routes, {
    'GET /api/sconfig': function () {
      var c = swCfg();   // RC-System, Port B, WLAN und Name teilt der Soundteil mit dem Schaltteil
      St.rc_system = c.rc_system; St.portb_role = c.portb_role; St.portb_baud = c.portb_baud;
      St.mkan_adr0 = c.ek_gadr0; St.mkan_adr1 = c.ek_gadr1;
      St.sbusgrp_ch0 = c.sbus_gch0; St.sbusgrp_ch1 = c.sbus_gch1; St.sbusgrp_mode0 = c.sbus_gmode0; St.sbusgrp_mode1 = c.sbus_gmode1;
      C.wifi.ssid = c.ssid; C.wifi.pass = c.pass; C.wifi.ip = c.ip; C.wifi.device = c.dnam;
      C.wifi.auto_enable = c.wifi_auto ? 1 : 0; C.wifi.auto_timeout = c.wifi_auto_timeout; C.wifi.always_on = c.wifi_always_on ? 1 : 0;
      return PST.json(C);
    },
    'POST /api/sconfig': function (q, b) {
      // nur die Felder, die handleConfigPost() kennt
      var d = PST.parseBody(b);
      if (d.ek_channel != null) St.ek_channel = +d.ek_channel;
      if (d.ek_mode != null) St.ek_mode = +d.ek_mode;
      if (d.modul_adr != null) St.modul_adr = +d.modul_adr;
      if (d.poll_id0 != null) C.sport.poll_id0 = String(d.poll_id0).toUpperCase();
      if (d.poll_id1 != null) C.sport.poll_id1 = String(d.poll_id1).toUpperCase();
      return PST.json({ ok: true });
    },
    '/api/sound': function (q, b) {
      var d = PST.parseBody(b);
      if (d.name != null && +d.id > 0 && C.sounds[d.id - 1]) C.sounds[d.id - 1].name = String(d.name).slice(0, 16);
      return snd.routes['/api/sound'](q, b);
    },
    '/api/debug': function () { return PST.json(debug()); },
    '/api/wifi_enable': function (q, b) { var on = /"on":\s*(true|1)/.test(b || ''); apActive = on; return PST.json({ ok: true, active: on }); },
    '/save': function () { return PST.json({ ok: true }); }
  });
  function handle(m, path, q, b) {
    var h = routes[m + ' ' + path] || routes[path];
    if (h) return h(q, b, m);
    var r = sw.handle(m, path, q, b);
    if (r && m === 'GET' && (path === '/api/config' || path === '/api/status')) return overSample(path, r);
    if (r) return r;
    return M.handle(m, path, q, b);
  }
  // ── Lua: Schaltteil aus mock_multiswitch.js, Motor/Sounds/Zuordnung aus
  // PST.soundCore, dazu der Ordner Einstellungen des Soundteils ──
  var RC_NAMES = ['FrSky', 'FlySky', 'ELRS SBUS', 'Hott', 'ELRS CRSF', 'CRSF + Funk', 'CRSF aus Funk'];
  function swSet(k, v) { var o = {}; o[k] = v; sw.handle('POST', '/api/config', {}, JSON.stringify(o)); }
  var lua = PST.mergeLua({ get: {}, set: {}, views: {} }, { get: sw.get, set: sw.set, views: sw.views }, snd.lua, {
    get: {
      luaGetTest: sw.get.luaGetTest,
      sndLuaGetAddr: function () { return St.modul_adr; },
      sndLuaGetEkChannel: function () { return St.ek_channel === 999 ? 255 : St.ek_channel; },
      sndLuaGetEkMode: function () { return St.ek_mode >= 10 && St.ek_mode <= 13 ? St.ek_mode - 9 : 0; },
      luaGetGroupAdr: function (a) { return swCfg()['ek_gadr' + a]; },
      luaGetSbusGrpCh: function (a) { var c = swCfg()['sbus_gch' + a]; return c === 999 ? 255 : c; },
      luaGetSbusGrpMode: function (a) { var m = swCfg()['sbus_gmode' + a]; return m >= 10 && m <= 13 ? m - 9 : 0; },
      luaGetWifiNow: function () { return apActive ? 1 : 0; }
    },
    set: {
      sndLuaSetAddr: function (a, v) { St.modul_adr = v; },
      sndLuaSetEkChannel: function (a, v) { St.ek_channel = v === 255 ? 999 : Math.min(v, 15); },
      sndLuaSetEkMode: function (a, v) { St.ek_mode = v ? v + 9 : 0; },
      luaSetGroupAdr: function (a, v) { swSet('ek_gadr' + a, v); },
      luaSetSbusGrpCh: function (a, v) { swSet('sbus_gch' + a, v === 255 ? 999 : Math.min(v, 15)); },
      luaSetSbusGrpMode: function (a, v) { swSet('sbus_gmode' + a, v ? v + 9 : 0); },
      luaSetWifiNow: function (a, v) { apActive = !!v; }
    },
    views: {
      luaShowSndName: function (it, v) { var s = C.sounds[lua.get.luaGetSndNr()]; v.text = (s && s.name || '').slice(0, 20); v.max = 0; v.value = 0; },
      luaShowSettingsFolder: function (it, v) { v.name = 'Einstellungen (' + RC_NAMES[Math.min(swCfg().rc_system, 6)] + ')'; },
      luaShowRcSystem: function (it, v) { v.text = RC_NAMES[Math.min(swCfg().rc_system, 6)] + ' (Web aendern)'; },
      luaShowDeviceName: function (it, v) { v.text = swCfg().dnam + ' (Web aendern)'; }
    }
  });
  return {
    handle: handle, get: lua.get, set: lua.set, views: lua.views,
    deviceName: sw.deviceName, reboot: sw.reboot,
    hw: function () { return sw.hw().concat([['Sound-Speicher', 'SD-Karte'], ['Sounds', C.sounds.length + ' + Motor']]); }
  };
});
