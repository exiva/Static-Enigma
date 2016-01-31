var pbl = {
  init: function () {
    console.log('Hello.');
  },

  showConfig: function () {
    Pebble.openURL("https://boopy.me/staticEnigma");
  },

  saveConfig: function (e) {
    var cfg = JSON.parse(decodeURIComponent(e.response));
    var dict = {};
    dict['bg'] = parseInt(cfg.bg, 16);
    dict['fg'] = parseInt(cfg.fg, 16);
    Pebble.sendAppMessage(dict);
  }
};

Pebble.addEventListener('ready', pbl.init);
Pebble.addEventListener('showConfiguration', pbl.showConfig);
Pebble.addEventListener('webviewclosed', pbl.saveConfig);
