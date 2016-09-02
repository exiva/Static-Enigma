var clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new clay(clayConfig);

var pbl = {
  init: function () {
    console.log('Hello.');
  }
};

Pebble.addEventListener('ready', pbl.init);
